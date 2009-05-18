#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/remove_outliers.h>
#include <CGAL/IO/read_xyz_points.h>

#include <vector>
#include <fstream>

// types
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point;

int main(void)
{
  // Reads a .xyz point set file in points[].
  std::vector<Point> points;
  std::ifstream stream("data/oni.xyz");
  if (!stream ||
      !CGAL::read_xyz_points(stream,
                             std::back_inserter(points)))
  {
    return EXIT_FAILURE;
  }

  // Removes outliers using erase-remove idiom
  const double removed_percentage = 5.0; // percentage of points to remove
  const int nb_neighbors = 7; // considers 7 nearest neighbor points
  points.erase(CGAL::remove_outliers(points.begin(), points.end(),
                                     nb_neighbors,
                                     removed_percentage),
               points.end());

  return EXIT_SUCCESS;
}

