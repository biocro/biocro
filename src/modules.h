#ifndef MODULES_H
#define MODULES_H

#include <vector>
#include <memory>                     // For std::unique_ptr
#include "module_helper_functions.h"  // Essential for all modules

/**
 *  @class module_base
 *
 *  @brief Represents one or more equations
 *
 *  A module can be one of the following subtypes:
 *
 *  - `direct_module`: directly calculates the instantaneous value(s) of one or
 *    more quantities
 *
 *  - `differential_module`: a module that calculates the instantaneous rate(s)
 *    of change for one or more quantities
 *
 *  A module must also indicate whether or not it requires an Euler ODE solver.
 *  Most modules do not. However, a few "legacy" modules require information
 *  from previous times and will only work properly with a fixed step size
 *  Euler ODE solver.
 *
 *  This class has a pure virtual destructor to designate it as being
 *  intentionally abstract.
 */
class module_base
{
   public:
    module_base(
        std::string const& module_name,
        bool const& differential,
        bool const& requires_euler)
        : module_name{module_name},
          differential{differential},
          requires_euler{requires_euler}
    {
    }

    virtual ~module_base() = 0;

    // Functions for returning module information
    std::string get_name() const { return module_name; }
    bool is_differential() const { return differential; }
    bool requires_euler_ode_solver() const { return requires_euler; }

    // Functions for running the module
    void run() const { do_operation(); }

   private:
    virtual void do_operation() const = 0;

    std::string const module_name;
    bool const differential;
    bool const requires_euler;

   protected:
    // Updates the values of output quantities
    virtual void update(double* output_ptr, const double& value) const = 0;
};

/**
 *  @brief A destructor must be defined, and since the default is overridden
 *  when defining it as pure virtual, add an inline one in the header
 */
inline module_base::~module_base() {}

/**
 *  @class direct_module
 *
 *  @brief This class represents a direct module.
 *
 *  This class has a pure virtual destructor to designate it as being
 *  intentionally abstract.
 */
class direct_module : public module_base
{
   public:
    direct_module(
        const std::string& module_name, bool requires_euler = false)
        : module_base{module_name, 0, requires_euler}
    {
    }

    virtual ~direct_module() = 0;

   protected:
    void update(double* output_ptr, const double& value) const;
};

/**
 *  @brief A destructor must be defined, and since the default is overridden
 *  when defining it as pure virtual, add an inline one in the header
 */
inline direct_module::~direct_module() {}

/**
 *  @brief In a valid `dynamical_system` object, a given quantity may be defined
 *         by only one direct module. Thus the value at `output_ptr` will only
 *         be writable from a single module, and we can overwrite any previously
 *         stored value.
 *
 *  See `differential_module::update()` for a contrasting situation.
 */
inline void direct_module::update(double* output_ptr, const double& value) const
{
    *output_ptr = value;
}

/**
 *  @class differential_module
 *
 *  @brief This class represents a differential module.
 *
 *  This class has a pure virtual destructor to designate it as being
 *  intentionally abstract.
 */
class differential_module : public module_base
{
   public:
    differential_module(const std::string& module_name, bool requires_euler = false)
        : module_base{module_name, 1, requires_euler}
    {
    }

    virtual ~differential_module() = 0;

   protected:
    void update(double* output_ptr, const double& value) const;
};

/**
 *  @brief A destructor must be defined, and since the default is overridden
 *  when defining it as pure virtual, add an inline one in the header
 */
inline differential_module::~differential_module() {}

/**
 *  @brief The outputs of a differential module represent additive terms in the
 *         derivatives of its output quantities, and, in a valid
 *         `dynamical_system` object, it is possible for one derivative to be
 *         determined by more than one module; for this reason, the output
 *         values should be added to the previously stored values.
 *
 *  See `direct_module::update()` for a contrasting situation.
 */
inline void differential_module::update(double* output_ptr, const double& value) const
{
    *output_ptr += value;
}

void run_module_list(std::vector<std::unique_ptr<module_base>> const& modules);

#endif
