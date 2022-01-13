#ifndef MODULE_LIBRARY_H
#define MODULE_LIBRARY_H

#include <string>
#include <map>
#include <unordered_map>
#include "../module_creator.h"

/**
 *  @class module_library
 *
 *  @brief A factory class that creates `module_creator` objects; more
 *  colloquially, we say that a `module` or `module_creator` is "retrieved" from
 *  the module library
 *
 *  The module_library provides the main route for module creation by linking
 *  module names (strings) to `module_creator` objects via its `retrieve()`
 *  function. Once a `module_creator` representing a particular `module` class
 *  has been retrieved from the library, it can be used to identify the module's
 *  input and output quantities, and eventually to instantiate the module class,
 *  making it available for calculations.
 */
class module_library
{
   public:
    static module_creator* retrieve(std::string const& module_name);
    static string_vector get_all_modules();
    static std::unordered_map<std::string, string_vector> get_all_quantities();

   private:
    using creator_fcn = module_creator* (*)();
    using creator_map = std::map<std::string, creator_fcn>;
    static creator_map library_entries;
};

#endif
