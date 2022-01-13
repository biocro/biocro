#include "module.h"

/**
 *  @brief Runs each module in the input list
 *
 *  @param [in] modules A vector of unique pointers to module objects
 */
void run_module_list(module_vector const& modules)
{
    for (auto const& m : modules) {
        m->run();
    }
}
