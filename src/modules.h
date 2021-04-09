#ifndef MODULES_H
#define MODULES_H

#include <vector>
#include <memory>                     // For std::unique_ptr
#include "module_helper_functions.h"  // Essential for all modules

/**
 *  @class Module
 *
 *  @brief Represents a component of a larger system of differential equations.
 *
 *  A module can be one of the following subtypes:
 *
 *  - `SteadyModule`: a "steady state" module that calculates the value(s) of
 *    one or more quantities
 *
 *  - `DerivModule`: a "derivative" module that calculates the rate(s) of change
 *    for one or more quantities
 *
 *  A module must also indicate whether or not it is compatible with adaptive
 *  step size integrators. For most modules, this is the case. However, a few
 *  "legacy" modules require information from previous times and will only work
 *  properly with a fixed step size Euler integrator.
 *
 *  This class has a pure virtual destructor to designate it as being
 *  intentionally abstract.
 */
class Module
{
   public:
    Module(
        std::string const& module_name,
        bool const& deriv,
        bool const& adaptive_compatible)
        : module_name{module_name},
          deriv{deriv},
          adaptive_compatible{adaptive_compatible}
    {
    }

    virtual ~Module() = 0;

    // Functions for returning module information
    std::string get_name() const { return module_name; }
    bool is_deriv() const { return deriv; }
    bool is_adaptive_compatible() const { return adaptive_compatible; }

    // Functions for running the module
    void run() const { do_operation(); }

   private:
    virtual void do_operation() const = 0;

    std::string const module_name;
    bool const deriv;
    bool const adaptive_compatible;

   protected:
    // Updates the values of output quantities
    virtual void update(double* output_ptr, const double& value) const = 0;
};

/**
 *  @brief A destructor must be defined, and since the default is overridden
 *  when defining it as pure virtual, add an inline one in the header
 */
inline Module::~Module() {}

/**
 *  @class SteadyModule
 *
 *  @brief This class represents a steady state `Module`.
 *
 *  This class has a pure virtual destructor to designate it as being
 *  intentionally abstract.
 */
class SteadyModule : public Module
{
   public:
    SteadyModule(
        const std::string& module_name, bool adaptive_compatible = true)
        : Module{module_name, 0, adaptive_compatible}
    {
    }

    virtual ~SteadyModule() = 0;

   protected:
    void update(double* output_ptr, const double& value) const;
};

/**
 *  @brief A destructor must be defined, and since the default is overridden
 *  when defining it as pure virtual, add an inline one in the header
 */
inline SteadyModule::~SteadyModule() {}

/**
 *  @brief The output parameters of a steady state module are unique, so we can
 *  just overwrite the previously stored value.
 */
inline void SteadyModule::update(double* output_ptr, const double& value) const
{
    *output_ptr = value;
}

/**
 *  @class DerivModule
 *
 *  @brief This class represents a derivative `Module`.
 *
 *  This class has a pure virtual destructor to designate it as being
 *  intentionally abstract.
 */
class DerivModule : public Module
{
   public:
    DerivModule(const std::string& module_name, bool adaptive_compatible = true)
        : Module{module_name, 1, adaptive_compatible}
    {
    }

    virtual ~DerivModule() = 0;

   protected:
    void update(double* output_ptr, const double& value) const;
};

/**
 *  @brief A destructor must be defined, and since the default is overridden
 *  when defining it as pure virtual, add an inline one in the header
 */
inline DerivModule::~DerivModule() {}

/**
 *  @brief Derivative modules calculate terms of a derivative and it is possible
 *  for one derivative to be determined by multiple modules; for this reason,
 *  the outputs should be added to the previously stored value.
 */
inline void DerivModule::update(double* output_ptr, const double& value) const
{
    *output_ptr += value;
}

void run_module_list(std::vector<std::unique_ptr<Module>> const& modules);

#endif
