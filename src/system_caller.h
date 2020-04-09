#ifndef SYSTEM_CALLER_H
#define SYSTEM_CALLER_H

#include "numerical_jacobian.h"

/**
 * @class SystemPointerWrapper
 * 
 * @brief This is a simple wrapper class that prevents odeint from making zillions of copies of an input system.
 */
class SystemPointerWrapper
{
   public:
    SystemPointerWrapper(std::shared_ptr<System> sys) : sys(sys) {}

    template <typename state_type, typename time_type>
    void operator()(state_type const& x, state_type& dxdt, time_type const& t)
    {
        sys->operator()(x, dxdt, t);
    }

    template <typename state_type, typename jacobi_type, typename time_type>
    void operator()(state_type const& x, jacobi_type& jacobi, time_type const& t, state_type& dfdt)
    {
        calculate_jacobian_and_time_derivative(sys, sys->get_ntimes() - 1.0, x, t, jacobi, dfdt);
    }

    size_t get_ntimes() const { return sys->get_ntimes(); }

   private:
    std::shared_ptr<System> sys;
};

/**
 * @class SystemCaller
 * 
 * @brief This is a simple class that allows the same object to be used as inputs to integrate_const with
 * explicit and implicit steppers
 */
class SystemCaller : public SystemPointerWrapper
{
   public:
    SystemCaller(std::shared_ptr<System> sys) : SystemPointerWrapper(sys),
                                                first(SystemPointerWrapper(sys)),
                                                second(SystemPointerWrapper(sys)) {}

    // Provide the member types and variables that a std::pair would have
    typedef SystemPointerWrapper first_type;
    typedef first_type second_type;
    first_type first;
    second_type second;
};

#endif