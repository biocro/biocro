#ifndef MODULE_CREATOR_H
#define MODULE_CREATOR_H

#include <string>       // for std::string
#include <memory>       // for unique_ptr
#include "state_map.h"  // for state_map and string_vector
#include "module.h"

/**
 *  @class module_creator
 *
 *  @brief Facilitates module creation
 *
 *  This class addresses a difficulty associated with the creation of concrete
 *  `module` objects. The constructor of a concrete `module` class must store
 *  pointers to elements of `state_map` objects that correspond to its input and
 *  output quantities. Thus, in order to ensure that the `state_map` objects
 *  include the elements required by the module, the module's input and output
 *  quantities must be known before it is created. Naively, we would want the
 *  `module` class to have static virtual methods for reporting inputs and
 *  outputs prior to instantiation. However, C++ does not allow static virtual
 *  methods.
 *
 *  Instead, we use the `module_creator` class to provide similar functionality
 *  to hypothetical static virtual methods. This class provides essential
 *  information about a module that is required before creating it (its inputs,
 *  outputs, and name), as well as a method for creating it. Its key features
 *  are that (1) the module class's inputs and outputs can be accessed before
 *  creating an instance of it and (2) it defines a common signature for a
 *  concrete `module` class constuctor.
 *
 *  This class is pure abstract, defining a common type that can be created
 *  using a factory class (the `module_factory`). Its implementation for a
 *  particular concrete module is achieved using the derived template class
 *  `module_creator_impl`.
 */
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

/**
 *  @class module_creator_impl
 *
 *  @brief A template class that provides a common interface to the constructor
 *  and essential static methods of a concrete `module` class
 *
 *  Here, the typename `T` should be the type of a concrete `module` class; in
 *  other words, `T` should be a class derived from `module`. The
 *  `module_creator_impl` class enforces the requirement that each concrete
 *  `module` should have static methods `get_inputs()`, `get_outputs()`, and
 *  `get_name()` that return strings or vectors of strings.
 */
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

/**
 * @brief A function that returns a pointer to a module_creator object.
 */
template <typename T>
module_creator* create_mc()
{
    return new module_creator_impl<T>;
}

/**
 * @brief `mc_vector` serves as an alias for a type widely used to hold lists of
 * module_creators.
 *
 * Formally, it is a `std::vector` of pointers to `module_creator` objects.
 */
using mc_vector = std::vector<module_creator*>;

/**
 *  @brief `creator_fcn` serves as an alias for a type widely used to define a
 *  function pointer that returns a pointer to a module_creator object.
 */
using creator_fcn = module_creator* (*)();

/**
 *  @brief `creator_map` serves as an alias for a type widely used to define a
 *  map of creator_fcn function pointers.
 */
using creator_map = std::map<std::string, creator_fcn>;

#endif
