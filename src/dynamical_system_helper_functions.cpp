#include "dynamical_system_helper_functions.h"
#include "validate_dynamical_system.h"  // For module_vector

/**
 * @brief Checks whether the collection of modules is compatible with
 * adaptive step size solvers
 */
bool check_adaptive_compatible(const module_vector* ptr_to_module_vector)
{
    bool adaptive_compatible = true;
    const module_vector& module_vector_reference = *ptr_to_module_vector;
    for (size_t i = 0; i < module_vector_reference.size(); i++) {
        adaptive_compatible *= module_vector_reference[i]->is_adaptive_compatible();
    }
    return adaptive_compatible;
}
