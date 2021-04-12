#ifndef STANDALONE_SS_H
#define STANDALONE_SS_H

#include <vector>
#include <memory>  // For unique_ptr and shared_ptr
#include <unordered_map>
#include <set>
#include "modules.h"
#include "module_library/module_wrapper_factory.h"
#include "system_helper_functions.h"
#include "validate_system.h"

bool validate_standalone_ss_inputs(
    std::string& message,
    string_vector const& ss_module_names,
    std::unordered_map<std::string, const double*> const& input_ptrs,
    std::unordered_map<std::string, double*> const& output_ptrs);

/**
 * @class Standalone_SS
 * 
 * @brief This class defines a standalone steady state module, i.e.,
 * a module that can easily be used without creating a system.
 * It may be formed from one or more steady state modules.
 * Its main intended use is to allow modules to call other modules as
 * part of their operation.
 */
class Standalone_SS
{
   public:
    Standalone_SS(
        string_vector const& ss_module_names,
        std::unordered_map<std::string, const double*> const& input_ptrs,
        std::unordered_map<std::string, double*> const& output_ptrs);
    
    // For returning information about the collection of sub-modules
    bool is_adaptive_compatible() const
    {
        return check_adaptive_compatible(&steady_state_modules);
    }
    
    string_set get_inputs() const {
        return find_strictly_required_inputs(std::vector<string_vector>{steady_state_module_names});
    }
    
    string_vector get_outputs() const {
        return string_set_to_string_vector(find_unique_module_outputs(std::vector<string_vector>{steady_state_module_names}));
    }
    
    // For generating reports to the user
    std::string generate_startup_report() const { return startup_message; }
    
    // Main operation
    void run() const;

   private:
    // For storing the constructor inputs
    const std::vector<std::string> steady_state_module_names;

    // Quantity maps defined during construction
    state_map quantities;
    state_map module_output_map;

    // Module lists defined during construction
    module_vector steady_state_modules;
    
    // Pointers to quantity values defined during construction
    std::vector<double*> module_output_ptrs;
    std::vector<std::pair<double*, const double*>> module_output_ptr_pairs;
    std::vector<std::pair<double*, const double*>> input_ptr_pairs;
    std::vector<std::pair<const double*, double*>> output_ptr_pairs;
    
    // For generating reports to the user
    bool fully_initialized;
    std::string startup_message;
};

#endif
