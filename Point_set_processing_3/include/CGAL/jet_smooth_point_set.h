// Copyright (c) 2007-09  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/trunk/Point_set_processing_3/include/CGAL/improved_laplacian_smooth_point_set.h $
// $Id: improved_laplacian_smooth_point_set.h 48866 2009-04-22 15:06:24Z lsaboret $
//
// Author(s) : Pierre Alliez, Marc Pouget and Laurent Saboret

#ifndef CGAL_JET_SMOOTH_POINT_SET_H
#define CGAL_JET_SMOOTH_POINT_SET_H

#include <CGAL/Search_traits_3.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Monge_via_jet_fitting.h>
#include <CGAL/point_set_property_map.h>
#include <CGAL/point_set_processing_assertions.h>

#include <iterator>
#include <list>

CGAL_BEGIN_NAMESPACE


// ----------------------------------------------------------------------------
// Private section
// ----------------------------------------------------------------------------
namespace CGALi {


/// Smoothes one point position using jet fitting on the k
/// nearest neighbors and reprojection onto the jet.
///
/// @commentheading Precondition: k >= 2.
///
/// @commentheading Template Parameters:
/// @param Kernel Geometric traits class.
/// @param Tree KD-tree.
///
/// @return computed point
template <typename Kernel,
          typename Tree>
typename Kernel::Point_3
jet_smooth_point(
  const typename Kernel::Point_3& query, ///< 3D point to project
  Tree& tree, ///< KD-tree
  const unsigned int k, ///< number of neighbors.
  const unsigned int degree_fitting,
  const unsigned int degree_monge)
{
  // basic geometric types
  typedef typename Kernel::Point_3 Point;

  // types for K nearest neighbors search
  typedef typename CGAL::Search_traits_3<Kernel> Tree_traits;
  typedef typename CGAL::Orthogonal_k_neighbor_search<Tree_traits> Neighbor_search;
  typedef typename Neighbor_search::iterator Search_iterator;

  // types for jet fitting
  typedef typename CGAL::Monge_via_jet_fitting<Kernel> Monge_jet_fitting;
  typedef typename Monge_jet_fitting::Monge_form Monge_form;

  // Gather set of (k+1) neighboring points.
  // Performs k + 1 queries (if unique the query point is
  // output first). Search may be aborted if k is greater
  // than number of input points.
  std::vector<Point> points; points.reserve(k+1);
  Neighbor_search search(tree,query,k+1);
  Search_iterator search_iterator = search.begin();
  unsigned int i;
  for(i=0;i<(k+1);i++)
  {
    if(search_iterator == search.end())
      break; // premature ending
    points.push_back(search_iterator->first);
    search_iterator++;
  }
  CGAL_point_set_processing_precondition(points.size() >= 1);

  // performs jet fitting
  Monge_jet_fitting monge_fit;
  Monge_form monge_form = monge_fit(points.begin(), points.end(),
                                    degree_fitting, degree_monge);

  // output projection of query point onto the jet
  return monge_form.origin();
}


} /* namespace CGALi */


// ----------------------------------------------------------------------------
// Public section
// ----------------------------------------------------------------------------


/// Smoothes the [first, beyond) range of points using jet fitting on the k
/// nearest neighbors and reprojection onto the jet.
/// As this method relocates the points, it
/// should not be called on containers sorted w.r.t. point locations.
///
/// @commentheading Precondition: k >= 2.
///
/// @commentheading Template Parameters:
/// @param ForwardIterator iterator over input points.
/// @param PointPMap is a model of boost::ReadablePropertyMap with a value_type = Point_3<Kernel>.
///        It can be omitted if ForwardIterator value_type is convertible to Point_3<Kernel>.
/// @param Kernel Geometric traits class.
///        It can be omitted and deduced automatically from PointPMap value_type.

// This variant requires all parameters.
template <typename ForwardIterator,
          typename PointPMap,
          typename Kernel
>
void
jet_smooth_point_set(
  ForwardIterator first,  ///< iterator over the first input point.
  ForwardIterator beyond, ///< past-the-end iterator over the input points.
  PointPMap point_pmap, ///< property map ForwardIterator -> Point_3.
  unsigned int k, ///< number of neighbors.
  const Kernel& kernel, ///< geometric traits.
  unsigned int degree_fitting = 2,
  unsigned int degree_monge = 2)
{
  // Input points types
  typedef typename boost::property_traits<PointPMap>::value_type Point;

  // types for K nearest neighbors search structure
  typedef typename CGAL::Search_traits_3<Kernel> Tree_traits;
  typedef typename CGAL::Orthogonal_k_neighbor_search<Tree_traits> Neighbor_search;
  typedef typename Neighbor_search::Tree Tree;

  // precondition: at least one element in the container.
  // to fix: should have at least three distinct points
  // but this is costly to check
  CGAL_point_set_processing_precondition(first != beyond);

  // precondition: at least 2 nearest neighbors
  CGAL_point_set_processing_precondition(k >= 2);

  // Instanciate a KD-tree search
  Tree tree(first,beyond);

  // Iterates over input points and mutates them.
  ForwardIterator it;
  for(it = first; it != beyond; it++)
  {
    Point& p = get(point_pmap, it);
    p = CGALi::jet_smooth_point<Kernel>(p,tree,k,degree_fitting,degree_monge);
  }
}

/// @cond SKIP_IN_MANUAL
// This variant deduces the kernel from the point property map.
template <typename ForwardIterator,
          typename PointPMap
>
void
jet_smooth_point_set(
  ForwardIterator first, ///< iterator over the first input point
  ForwardIterator beyond, ///< past-the-end iterator
  PointPMap point_pmap, ///< property map ForwardIterator -> Point_3
  unsigned int k, ///< number of neighbors.
  const unsigned int degree_fitting = 2,
  const unsigned int degree_monge = 2)
{
  typedef typename boost::property_traits<PointPMap>::value_type Point;
  typedef typename Kernel_traits<Point>::Kernel Kernel;
  jet_smooth_point_set(
    first,beyond,
    point_pmap,
    k,
    Kernel(),
    degree_fitting,degree_monge);
}
/// @endcond

/// @cond SKIP_IN_MANUAL
// This variant creates a default point property map = Dereference_property_map.
template <typename ForwardIterator
>
void
jet_smooth_point_set(
  ForwardIterator first, ///< iterator over the first input point
  ForwardIterator beyond, ///< past-the-end iterator
  unsigned int k, ///< number of neighbors.
  const unsigned int degree_fitting = 2,
  const unsigned int degree_monge = 2)
{
  jet_smooth_point_set(
    first,beyond,
    make_dereference_property_map(first),
    k,
    degree_fitting,degree_monge);
}
/// @endcond


CGAL_END_NAMESPACE

#endif // CGAL_JET_SMOOTH_POINT_SET_H

