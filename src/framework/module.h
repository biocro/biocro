#ifndef MODULE_H
#define MODULE_H

#include <vector>
#include <memory>                     // For std::unique_ptr
#include "module_helper_functions.h"  // Essential for all modules

/**
 *  @class module
 *
 *  @brief Represents one or more equations
 *
 *  This class defines the operational interface to all module classes.
 *
 *  The central feature of a module is its `run()` method, which should retrieve
 *  values of its input quantities, perform calculations, and update the values
 *  of its outputs. These operations are achieved in the `do_operation()` method
 *  of a concrete derived class using pointers; upon construction, a module
 *  should store references (or pointers) to its input quantities and pointers
 *  to its output quantities. Input and output quantities alike are stored as
 *  elements of `state_map` objects which are passed by reference to module
 *  constructors.
 *
 *  A module can be one of the following subtypes:
 *
 *  - `direct_module`: directly calculates the instantaneous value(s) of one or
 *    more quantities
 *
 *  - `differential_module`: a module that calculates the instantaneous rate(s)
 *    of change for one or more quantities
 *
 *  A module must also indicate whether or not it requires an Euler
 *  `ode_solver`. Most modules do not. However, a few "legacy" modules require
 *  information from previous times and will only work properly with a fixed
 *  step size Euler ODE solver.
 *
 *  Typically, concrete classes derived from this one are not instantiated
 *  directly; instead, module objects are created by `module_creator` objects
 *  retrieved from the `module_factory`.
 */
class module
{
   public:
    module(
        bool const& differential,
        bool const& requires_euler)
        : differential{differential},
          requires_euler{requires_euler}
    {
    }

    virtual ~module() = 0;

    // Functions for returning module information
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
inline module::~module() {}

/**
 *  @class direct_module
 *
 *  @brief This class represents a direct module.
 *
 *  This class has a pure virtual destructor to designate it as being
 *  intentionally abstract.
 */
class direct_module : public module
{
   public:
    direct_module(bool requires_euler = false)
        : module{false, requires_euler}
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
class differential_module : public module
{
   public:
    differential_module(bool requires_euler = false)
        : module{true, requires_euler}
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

/**
 * @brief `module_vector` serves as an alias for a type widely used to
 * hold lists of modules.
 *
 * Formally, it is a `std::vector` of `std::unique_ptr`s that point to
 * `module` objects.
 */
using module_vector = std::vector<std::unique_ptr<module>>;

void run_module_list(module_vector const& modules);

#endif
