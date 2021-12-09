#ifndef DYNAMICAL_SYSTEM_CALLER_H
#define DYNAMICAL_SYSTEM_CALLER_H

#include "numerical_jacobian.h"

/**
 * @class dynamical_system_pointer_wrapper
 *
 * @brief This is a simple wrapper class that prevents odeint from making
 * zillions of copies of a `dynamical_system`.
 */
class dynamical_system_pointer_wrapper
{
   public:
    dynamical_system_pointer_wrapper(std::shared_ptr<dynamical_system> sys)
        : sys(sys)
    {
    }

    template <typename state_type, typename time_type>
    void operator()(state_type const& x, state_type& dxdt, time_type const& t)
    {
        sys->calculate_derivative(x, dxdt, t);
    }

    template <typename state_type, typename jacobi_type, typename time_type>
    void operator()(state_type const& x, jacobi_type& jacobi, time_type const& t, state_type& dfdt)
    {
        calculate_jacobian_and_time_derivative(sys, sys->get_ntimes() - 1.0, x, t, jacobi, dfdt);
    }

    size_t get_ntimes() const { return sys->get_ntimes(); }

   private:
    std::shared_ptr<dynamical_system> sys;
};

/**
 * @class dynamical_system_caller
 *
 * @brief This is a simple class that allows us to use the same object as an
 * input to `boost::numeric::odeint::integrate_const` with either an explicit or
 * implicit stepper.
 */
class dynamical_system_caller : public dynamical_system_pointer_wrapper
{
   public:
    dynamical_system_caller(std::shared_ptr<dynamical_system> sys)
        : dynamical_system_pointer_wrapper(sys),
          first(dynamical_system_pointer_wrapper(sys)),
          second(dynamical_system_pointer_wrapper(sys))
    {
    }

    // Provide the member types and variables that a std::pair would have
    typedef dynamical_system_pointer_wrapper first_type;
    typedef first_type second_type;
    first_type first;
    second_type second;
};

#endif
