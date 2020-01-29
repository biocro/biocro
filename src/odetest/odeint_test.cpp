/*
 Copyright 2010-2012 Karsten Ahnert
 Copyright 2011-2013 Mario Mulansky
 Copyright 2013 Pascal Germroth
 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <iostream>
#include <vector>
#include <string>

#include <boost/serialization/array_wrapper.hpp>
#include <boost/numeric/odeint.hpp>

#include "../state_map.h"

/* The type of container used to hold the state vector */
typedef std::vector< double > state_type;

namespace boost { namespace numeric { namespace odeint {
template<>
struct vector_space_norm_inf< state_map >
{
    typedef double result_type;
    double operator()( const state_map &x ) const
    {
        using std::max;
        using std::abs;
        auto it = x.begin();
        double result = it->second;
        ++it;
        for(;it != x.end(); ++it) {
            result = max(result, it->second);
        }
        return result;
    }
};
} } }  // boost, numeric, odeint.

state_map abs(state_map x)
{
    for(auto it = x.begin(); it != x.end(); ++it) {
        it->second = abs(it->second);
    }
    return x;
}

/* The rhs of x' = f(x) defined as a class */
class harm_osc {
    double m_gam;

public:
    harm_osc( double gam ) : m_gam(gam) { }

    void operator() ( const state_type &x, state_type &dxdt, const double /* t */ )
    {
        dxdt[0] = x[1];
        dxdt[1] = -x[0] - m_gam*x[1];
    }

    void operator() ( const state_map &x, state_map &dxdt, const double /* t */ )
    {
        dxdt["zero"] = x.at("one");
        dxdt["one"] = -x.at("zero") - m_gam*x.at("one");
    }
};
//]

template <typename T>
struct push_back_state_and_time
{
    std::vector< double >& m_times;
    std::vector< T >& m_states;
    
    push_back_state_and_time( std::vector< T > &states, std::vector< double > &times )
    : m_states( states ), m_times( times ) { }


    void operator()( const T &x, double t )
    {
        m_states.push_back( x );
        m_times.push_back( t );
    }
};


int main(int /* argc */, char** /* argv */ )
{
    using namespace std;
    using namespace boost::numeric::odeint;

    harm_osc ho(0.15);

    state_type x(2);
    x[0] = 1.0; // start at x=1.0, p=0.0
    x[1] = 0.0;

    //[ integrate_observ
    vector<state_type> x_vec;
    vector<double> times;

    cout << "Before try\n";
    try {
        runge_kutta4<state_type, double, state_type, double> stepper;
        size_t steps = integrate_const(stepper, ho, x, 0.0, 10.0, 0.1,
                push_back_state_and_time<state_type>( x_vec, times ) );
        cout << "after integrate\n";

        /* output */
        for( size_t i=0; i<=steps; i++ )
        {
            cout << times[i] << '\t' << x_vec[i][0] << '\t' << x_vec[i][1] << '\n';
        }
    } catch (...) {
        cout << "Caught exception: " << '\n';

    }

    state_map x_map;
    x_map["zero"] = 1.0; // start at x=1.0, p=0.0
    x_map["one"] = 0.0;

    vector<state_map> x_map_vec;
    vector<double> times_map;

    cout << "Before try\n";
    try {
        runge_kutta4<state_map, double, state_map, double, vector_space_algebra> stepper;

        //integrate_const(stepper, ho, x_map, 0.0, 10.0, 0.01 );
        size_t steps = integrate_const(stepper, ho, x_map, 0.0, 10.0, 0.1,
                push_back_state_and_time<state_map>( x_map_vec, times_map ) );
        //size_t steps = integrate( ho, x_map, 0.0, 10.0, 0.1,
                //push_back_state_and_time<state_map>( x_map_vec, times_map ) );
        cout << "after integrate\n";

        /* output */
        for( size_t i=0; i<=steps; i++ )
        {
            cout << times_map[i] << '\t' << x_map_vec[i]["zero"] << '\t' << x_map_vec[i]["one"] << '\n';
        }
    } catch (...) {
        cout << "Caught exception: " << '\n';

    }
//

    //]

    //[ define_const_stepper
//    runge_kutta4< state_type > stepper;
//    integrate_const( stepper, ho, x, 0.0, 10.0, 0.01 );
    //]

    //[ integrate_const_loop
//    const double dt = 0.01;
//    for( double t = 0.0; t < 10.0; t += dt )
//        stepper.do_step( ho, x, t, dt );
//    //]
//
//
//    //[ define_adapt_stepper
//    typedef runge_kutta_cash_karp54< state_type > error_stepper_type;
//    //]
//
//    //[ integrate_adapt
//    typedef controlled_runge_kutta< error_stepper_type > controlled_stepper_type;
//    controlled_stepper_type controlled_stepper;
//    integrate_adaptive( controlled_stepper, ho, x, 0.0, 10.0, 0.01 );
//    //]
//
//    {
//    //[integrate_adapt_full
//    double abs_err = 1.0e-10 , rel_err = 1.0e-6 , a_x = 1.0 , a_dxdt = 1.0;
//    controlled_stepper_type controlled_stepper( 
//        default_error_checker< double , range_algebra , default_operations >( abs_err , rel_err , a_x , a_dxdt ) );
//    integrate_adaptive( controlled_stepper , ho , x , 0.0 , 10.0 , 0.01 );
//    //]
//    }
//
//
//    //[integrate_adapt_make_controlled
//    integrate_adaptive( make_controlled< error_stepper_type >( 1.0e-10 , 1.0e-6 ) , 
//                        ho , x , 0.0 , 10.0 , 0.01 );
//    //]
//
//
//
//
//    //[integrate_adapt_make_controlled_alternative
//    integrate_adaptive( make_controlled( 1.0e-10 , 1.0e-6 , error_stepper_type() ) , 
//                        ho , x , 0.0 , 10.0 , 0.01 );
//    //]
}

