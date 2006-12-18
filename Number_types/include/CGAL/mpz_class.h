// Copyright (c) 2002,2003  Utrecht University (The Netherlands),
// ETH Zurich (Switzerland), Freie Universitaet Berlin (Germany),
// INRIA Sophia-Antipolis (France), Martin-Luther-University Halle-Wittenberg
// (Germany), Max-Planck-Institute Saarbruecken (Germany), RISC Linz (Austria),
// and Tel-Aviv University (Israel).  All rights reserved.
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
// Author(s)     : Sylvain Pion
 
#ifndef CGAL_MPZ_CLASS_H
#define CGAL_MPZ_CLASS_H

#include <CGAL/number_type_basic.h>
#include <CGAL/gmpxx_coercion_traits.h>


// This file gathers the necessary adaptors so that the following
// C++ number types that come with GMP can be used by CGAL :
// - mpz_class

// Note that GMP++ use the expression template mechanism, which makes things
// a little bit complicated in order to make square(x+y) work for example.
// Reading gmpxx.h shows that ::__gmp_expr<T, T> is the mp[zqf]_class proper,
// while ::__gmp_expr<T, U> is the others "expressions".


#define CGAL_CHECK_GMP_EXPR                                             \
    BOOST_STATIC_ASSERT(                                                \
            (::boost::is_same< ::__gmp_expr< T , T >,Type>::value ));   

CGAL_BEGIN_NAMESPACE

// AST for mpz_class

template<>
class Algebraic_structure_traits< mpz_class >
    :public Algebraic_structure_traits_base<  mpz_class , Null_tag > {
public:
    typedef mpz_class           Type;
    typedef Euclidean_ring_tag  Algebraic_category;
    typedef Tag_true            Is_exact;
    typedef Tag_false           Is_numerical_sensitive;
    
    struct Is_zero: public Unary_function< mpz_class , bool > {
        template <typename T, typename U> 
        bool operator()( const ::__gmp_expr< T , U >& x) const {
            CGAL_CHECK_GMP_EXPR;
            return ::sgn(x) == 0;
        }
    };  

    struct Is_one: public Unary_function< mpz_class , bool > {
        template <typename T, typename U> 
        bool operator()( const ::__gmp_expr< T , U >& x) const {
            CGAL_CHECK_GMP_EXPR;
            return x == 1;
        }
    }; 

    struct Simplify: public Unary_function< mpz_class , void > {        
        template <class T, class U> 
        void operator()( const ::__gmp_expr< T ,U >& x) const {
            CGAL_CHECK_GMP_EXPR;
        }
    }; 
    
    struct Square: public Unary_function< mpz_class , mpz_class > {
        template <typename T, typename U> 
        mpz_class operator()( const ::__gmp_expr< T , U >& x) const {
            CGAL_CHECK_GMP_EXPR;
            return x*x;
        }
    }; 

    struct Unit_part: public Unary_function< mpz_class , mpz_class > {
        template <typename T, typename U> 
        mpz_class operator()( const ::__gmp_expr< T , U >& x) const {
            CGAL_CHECK_GMP_EXPR;
            return( x < mpz_class(0)) ?  mpz_class(-1) : mpz_class(1); 
        }
    }; 

   

    struct Integral_division: 
        public Binary_function< mpz_class , mpz_class, mpz_class > {
        template <typename T,  typename U1, typename U2> 
        mpz_class operator()( 
                const ::__gmp_expr< T , U1 >& x,
                const ::__gmp_expr< T , U2 >& y) const {
            CGAL_CHECK_GMP_EXPR;
            mpz_class result = x / y;
            CGAL_precondition_msg( result * y == x,
            "'x' must be divisible by 'y' in "
            "Algebraic_structure_traits<mpz_class>::Integral_div()(x,y)" );
            return result;         
        } 
        CGAL_IMPLICIT_INTEROPERABLE_BINARY_OPERATOR( Type )
    }; 
    
    struct Gcd : public Binary_function< mpz_class, mpz_class, mpz_class > {
        template <typename T,  typename U1, typename U2> 
        mpz_class operator()( 
                const ::__gmp_expr< T , U1 >& x,
                const ::__gmp_expr< T , U2 >& y) const {
            CGAL_CHECK_GMP_EXPR;
            mpz_class c;
            mpz_gcd(c.get_mpz_t(),mpz_class(x).get_mpz_t(), mpz_class(y).get_mpz_t() );
            return c;
        } 
        CGAL_IMPLICIT_INTEROPERABLE_BINARY_OPERATOR( Type )
    };
    
    struct Div : public Binary_function< mpz_class, mpz_class, mpz_class > {
        template <typename T,  typename U1, typename U2> 
        mpz_class operator()( 
                const ::__gmp_expr< T , U1 >& x,
                const ::__gmp_expr< T , U2 >& y) const {
            CGAL_CHECK_GMP_EXPR;
            return x / y; 
        } 
        CGAL_IMPLICIT_INTEROPERABLE_BINARY_OPERATOR( Type )
    };
    
    struct Mod : public Binary_function< mpz_class, mpz_class, mpz_class > {
        template <typename T,  typename U1, typename U2> 
        mpz_class operator()( 
                const ::__gmp_expr< T , U1 >& x,
                const ::__gmp_expr< T , U2 >& y) const {
            CGAL_CHECK_GMP_EXPR;
            return x % y; 
        }
        CGAL_IMPLICIT_INTEROPERABLE_BINARY_OPERATOR( Type )
    };
    struct Div_mod {
        typedef mpz_class    first_argument_type;
        typedef mpz_class    second_argument_type;
        typedef mpz_class&   third_argument_type;
        typedef mpz_class&   fourth_argument_type;
        typedef Arity_tag< 4 >         Arity;
        typedef void         result_type;
        template <typename T,  typename U1, typename U2> 
        void operator()( 
                const ::__gmp_expr< T , U1 >& x,
                const ::__gmp_expr< T , U2 >& y,
                mpz_class& q, 
                mpz_class& r
        ) const {
            CGAL_CHECK_GMP_EXPR;
            typedef Algebraic_structure_traits<mpz_class> Traits;
                typename Traits::Div  actual_div;
                typename Traits::Mod  actual_mod;
                q = actual_div( x, y );
                r = actual_mod( x, y );          
                return;
            };  
    };
    
    
    struct Sqrt: public Unary_function< mpz_class , mpz_class > {
        template <typename T, typename U> 
        mpz_class operator()( const ::__gmp_expr< T , U >& x) const {
            CGAL_CHECK_GMP_EXPR;
            return ::sqrt(x);
        }
    }; 
   

    /*struct Is_square: public Binary_function< mpz_class , mpz_class& , bool > {
        template <typename T, typename U> 
        bool operator()(
                const ::__gmp_expr< T , U >& x,
                mpz_class&                              r){
            r = ::sqrt(x);
            return (r*r==x) ? true : false; 
        }
        template <typename T, typename U> 
        bool operator()(const ::__gmp_expr< T , U >& x){
            mpz_class r = ::sqrt(x);
            return (r*r==x) ? true : false; 
        }
        };*/ 
};

// RET for mpz_class
template<>
class Real_embeddable_traits< mpz_class  > 
    : public Real_embeddable_traits_base< mpz_class > {
public:
    
    typedef mpz_class  Type;

    struct Is_zero: public Unary_function< mpz_class , bool > {
        template <typename T, typename U> 
        bool operator()( const ::__gmp_expr< T , U >& x) const {
            CGAL_CHECK_GMP_EXPR;
            return ::sgn(x) == 0;
        }
    }; 
    struct Is_finite: public Unary_function<mpz_class,bool> {
        template <typename T, typename U> 
        bool operator()( const ::__gmp_expr< T , U >& x) const {
            CGAL_CHECK_GMP_EXPR;
            return true;
        }
    };

    struct Is_positive: public Unary_function< mpz_class , bool > {
        template <typename T, typename U> 
        bool operator()( const ::__gmp_expr< T , U >& x) const {
            CGAL_CHECK_GMP_EXPR;
            return ::sgn(x) > 0;
        }
    }; 
  
    struct Is_negative: public Unary_function< mpz_class , bool > {
        template <typename T, typename U> 
        bool operator()( const ::__gmp_expr< T , U >& x) const {
            CGAL_CHECK_GMP_EXPR;
            return ::sgn(x) < 0;
        }
    };

    struct Abs: public Unary_function< mpz_class , mpz_class > {
        template <typename T, typename U> 
        mpz_class operator()( const ::__gmp_expr< T , U >& x) const {
            CGAL_CHECK_GMP_EXPR;
            return ::abs(x);
        }
    };
    
    struct Sign 
        : public Unary_function< mpz_class, ::CGAL::Sign > {
    public:
        template <typename T, typename U> 
        ::CGAL::Sign operator()( const ::__gmp_expr< T , U >& x ) const {
            CGAL_CHECK_GMP_EXPR;
            return (::CGAL::Sign) ::sgn( x );
        }        
    };
    
    struct Compare 
        : public Binary_function< mpz_class, mpz_class, Comparison_result > {
        template <typename T,  typename U1, typename U2>
        Comparison_result operator()( 
                const ::__gmp_expr< T , U1 >& x, 
                const ::__gmp_expr< T , U2 >& y ) const {
            CGAL_CHECK_GMP_EXPR;
            // cmp returns any int value, not just -1/0/1...
            return (Comparison_result) CGAL_NTS sign( ::cmp(x, y) );
        }
        CGAL_IMPLICIT_INTEROPERABLE_BINARY_OPERATOR_WITH_RT
        ( Type, Comparison_result);
    };
    
    struct To_double 
        : public Unary_function< mpz_class, double > {
        template < typename T, typename U >
        double operator()( const ::__gmp_expr< T , U >& x ) const {
            CGAL_CHECK_GMP_EXPR;
            return mpz_class(x).get_d();
        }
    };
    
    struct To_interval 
    
        : public Unary_function< mpz_class, std::pair< double, double > > {
       template <typename T, typename U> 
        std::pair<double, double> 
        operator()( const ::__gmp_expr< T , U >& x_ ) const {
            CGAL_CHECK_GMP_EXPR;
            mpz_class x = mpz_class(x_);
            mpfr_t y;
            mpfr_init2 (y, 53); /* Assume IEEE-754 */
            mpfr_set_z (y, x.get_mpz_t(), GMP_RNDD);
            double i = mpfr_get_d (y, GMP_RNDD); /* EXACT but can overflow */
            mpfr_set_z (y, x.get_mpz_t(), GMP_RNDU);
            double s = mpfr_get_d (y, GMP_RNDU); /* EXACT but can overflow */
            mpfr_clear (y);
            return std::pair<double, double>(i, s);
        }
    };
};

CGAL_END_NAMESPACE

#undef CGAL_CHECK_GMP_EXPR 

#endif // CGAL_MPZ_CLASS_H
