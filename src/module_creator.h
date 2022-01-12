#ifndef MODULE_CREATOR_H
#define MODULE_CREATOR_H

#include <string>       // for std::string
#include <memory>       // for unique_ptr
#include "state_map.h"  // for state_map and string_vector
#include "module.h"

// R does not have the ability to create C++ objects directly, so we must create
// them via strings passed from R. We'd like to have a table mapping strings of
// names to something that will produce objects of the correct type. We can use
// a template class to wrap the module class, so that we have access to
// whatever members we need.

// Since the table needs to have elements of all the same type, we need an
// abstract base class.
class module_creator
{
   public:
    virtual string_vector get_inputs() = 0;
    virtual string_vector get_outputs() = 0;
    virtual std::string get_name() = 0;

    virtual std::unique_ptr<module> create_module(
        state_map const& input_quantities, state_map* output_quantities) = 0;

    virtual ~module_creator() = 0;
};

// The destructor of the base class must always be implemented, even if it's
// pure virtual.
inline module_creator::~module_creator() {}

// Define a template class that wraps the behavior of Modules.
template <typename T>
class module_creator_impl : public module_creator
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

    std::string get_name()
    {
        return T::get_name();
    }

    std::unique_ptr<module> create_module(
        state_map const& input_quantities, state_map* output_quantities)
    {
        return std::unique_ptr<module>(new T(
            input_quantities, output_quantities));
    }
};

// We will often work with a std::vector of pointers to module_creator (mc)
// objects, so it is useful to define an alias for this.
using mc_vector = std::vector<module_creator*>;

#endif
