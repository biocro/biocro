#ifndef MODULE_WRAPPER_H
#define MODULE_WRAPPER_H

#include <memory>       // for unique_ptr
#include "state_map.h"  // for state_map and string_vector
#include "modules.h"

// R does not have the ability to create C++ objects directly, so we must create
// them via strings passed from R. We'd like to have a table mapping strings of
// names to something that will produce objects of the correct type. We can use
// a template class to wrap the module_base class, so that we have access to whatever
// members we need.

// Since the table needs to have elements of all the same type, we need an
// abstract base class.
class module_wrapper_base
{
   public:
    virtual string_vector get_inputs() = 0;
    virtual string_vector get_outputs() = 0;

    virtual std::unique_ptr<module_base> createModule(
        state_map const& input_quantities, state_map* output_quantities) = 0;

    virtual ~module_wrapper_base() = 0;
};

// The destructor of the base class must always be implemented, even if it's
// pure virtual.
inline module_wrapper_base::~module_wrapper_base() {}

// Define a template class that wraps the behavior of Modules.
template <typename T>
class module_wrapper : public module_wrapper_base
{
   public:
    string_vector get_inputs()
    {
        return T::get_inputs();
    }

    string_vector get_outputs()
    {
        return T::get_outputs();
    }

    std::unique_ptr<module_base> createModule(
        state_map const& input_quantities, state_map* output_quantities)
    {
        return std::unique_ptr<module_base>(new T(
            input_quantities, output_quantities));
    }
};

#endif
