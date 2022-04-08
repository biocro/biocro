#ifndef MODULE_FACTORY_H
#define MODULE_FACTORY_H

#include <algorithm>  // for std::transform
#include <cctype>     // for std::tolower
#include <string>
#include <map>
#include <unordered_map>
#include "module_creator.h"

/**
 *  @class module_factory
 *
 *  @brief A factory class that creates `module_creator` objects, where the list
 *  of available `module_creator` objects is provided by another class `T`; more
 *  colloquially, we say that the module factory allows us to retrieve a module
 *  from the module library.
 *
 *  The module_factory provides the main route for module creation by linking
 *  module names (strings) to `module_creator` objects via its `retrieve()`
 *  function. Once a `module_creator` representing a particular `module` class
 *  has been retrieved from the library, it can be used to identify the module's
 *  input and output quantities, and eventually to instantiate the module class,
 *  making it available for calculations.
 */
template <typename T>
class module_factory
{
   public:
    static module_creator* retrieve(std::string const& module_name);
    static string_vector get_all_modules();
    static std::unordered_map<std::string, string_vector> get_all_quantities();
};

template <typename T>
module_creator* module_factory<T>::retrieve(std::string const& module_name)
{
    try {
        return T::library_entries.at(module_name)();
    } catch (std::out_of_range const&) {
        std::string message = std::string("\"") + module_name +
                              std::string("\"") +
                              std::string(" was given as a module name, ") +
                              std::string("but no module with that name could be found.\n");

        throw std::out_of_range(message);
    }
}

template <typename T>
string_vector module_factory<T>::get_all_modules()
{
    string_vector module_name_vector;
    for (auto const& x : T::library_entries) {
        module_name_vector.push_back(x.first);
    }

    auto case_insensitive_compare = [](std::string const& a, std::string const& b) {
        // Make a lowercase copy of a
        std::string al = a;
        std::transform(al.begin(), al.end(), al.begin(), [](unsigned char c) { return std::tolower(c); });

        // Make a lowercase copy of b
        std::string bl = b;
        std::transform(bl.begin(), bl.end(), bl.begin(), [](unsigned char c) { return std::tolower(c); });

        int compare = al.compare(bl);
        return (compare > 0) ? false : true;
    };

    std::sort(module_name_vector.begin(), module_name_vector.end(), case_insensitive_compare);

    return module_name_vector;
}

template <typename T>
std::unordered_map<std::string, string_vector> module_factory<T>::get_all_quantities()
{
    // Make the output map
    std::unordered_map<std::string, string_vector> quantity_map = {
        {"module_name", string_vector{}},
        {"quantity_type", string_vector{}},
        {"quantity_name", string_vector{}}};

    // Make a lambda function for adding entries to the map
    auto add_quantity_map_entry = [&quantity_map](std::string module_name, std::string quantity_type, std::string quantity_name) {
        quantity_map["module_name"].push_back(module_name);
        quantity_map["quantity_type"].push_back(quantity_type);
        quantity_map["quantity_name"].push_back(quantity_name);
    };

    // Fill the output map with all the quantities
    for (std::string const& module_name : get_all_modules()) {
        auto w = retrieve(module_name);

        // Add the module's inputs to the parameter map
        for (std::string const& input_name : w->get_inputs()) {
            add_quantity_map_entry(module_name, "input", input_name);
        }

        // Add the module's outputs to the parameter map
        for (std::string const& output_name : w->get_outputs()) {
            add_quantity_map_entry(module_name, "output", output_name);
        }
    }

    return quantity_map;
}

#endif
