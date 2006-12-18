// Copyright (c) 1997-2001  ETH Zurich (Switzerland).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
// See the file LICENSE.LGPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
//
// Author(s)     : Bernd Gaertner <gaertner@inf.ethz.ch>

#ifndef CGAL_GMPZF_H
#define CGAL_GMPZF_H

// includes
#include <CGAL/number_type_basic.h>
#include <CGAL/GMP/Gmpzf_type.h>
#include <CGAL/Gmp_coercion_traits.h>
#include <CGAL/Gmpz.h>

CGAL_BEGIN_NAMESPACE

// Algebraic structure traits
template <> class Algebraic_structure_traits< Gmpzf >
    : public Algebraic_structure_traits_base< Gmpzf, Euclidean_ring_tag >  {
public:
    typedef Tag_true            Is_exact;

    struct Is_zero 
        : public Unary_function< Type, bool > {
    public:        
        bool operator()( const Type& x ) const {
            return x.is_zero();
        }
    };
    
    struct Integral_division
        : public Binary_function< Type, 
                                Type,
                                Type > {
    public:
        Type operator()( 
                const Type& x,
                const Type& y ) const {
            return x.integral_division(y);
        }
    };
    
    struct Gcd
        : public Binary_function< Type, 
                                Type,
                                Type > {
    public:
        Type operator()( 
                const Type& x,
                const Type& y ) const {
            return x.gcd(y);
        }
        CGAL_IMPLICIT_INTEROPERABLE_BINARY_OPERATOR(int);
    };
    
    typedef INTERN_AST::Div_per_operator< Type > Div;
    typedef INTERN_AST::Mod_per_operator< Type > Mod;

    struct Sqrt 
        : public Unary_function< Type, Type > {
        Type operator()( const Type& x ) const {
            return x.sqrt();
        }
    };       
    typedef INTERN_AST::Is_square_per_sqrt<Type> Is_square;
    
};


// Real embeddable traits
template <> 
class Real_embeddable_traits< Gmpzf > 
    : public Real_embeddable_traits_base< Gmpzf > {
    
    typedef Algebraic_structure_traits<Gmpzf> AST;
public:
    typedef AST::Is_zero Is_zero;

    struct Sign 
        : public Unary_function< Type, ::CGAL::Sign > {
    public:
        ::CGAL::Sign operator()( const Type& x ) const {
            return x.sign();
        }        
    };
    
    struct Compare 
        : public Binary_function< Type, 
                                  Type,
                                  Comparison_result > {
    public:
        Comparison_result operator()( 
                const Type& x, 
                const Type& y ) const {
            return x.compare(y);
        }
    };
    
    struct To_double 
        : public Unary_function< Type, double > {
    public:
        double operator()( const Type& x ) const {
            return x.to_double();
        }
    };
    
    struct To_interval 
        : public Unary_function< Type, std::pair< double, double > > {
    public:
        std::pair<double, double> operator()( const Type& x ) const {
            // dummy
            return std::pair<double,double>(0.0,0.0);
        }
    };
};

// specialization of to double functor
template<> 
class Real_embeddable_traits< Quotient<Gmpzf> >
    : public 
INTERN_QUOTIENT::Real_embeddable_traits_quotient_base< Quotient<Gmpzf> > 
{
public:
    struct To_double: public Unary_function<Quotient<Gmpzf>, double>{
        inline
        double operator()(const Quotient<Gmpzf>& q) const {
            double man = to_double(Quotient<Gmpz>(
                                           q.numerator().man(), 
                                           q.denominator().man()));
                return std::ldexp(
                        man, 
                        q.numerator().exp()-q.denominator().exp()
                );
        }
    };
    struct To_interval
        : public Unary_function<Quotient<Gmpzf>, std::pair<double,double> >{
        inline
        std::pair<double,double> operator()(const Quotient<Gmpzf>& q) const {
            //dummy 
            return std::pair<double,double>(0.0,0.0);
        }
    };
};

CGAL_END_NAMESPACE

#endif // CGAL_GMPZF_H

// ===== EOF ==================================================================
