#include "modules.h"

/**
 *  @brief Runs each module in the input list
 *
 *  @param [in] modules A vector of unique pointers to module objects
 */
void run_module_list(std::vector<std::unique_ptr<Module>> const& modules)
{
    for (std::unique_ptr<Module> const& m : modules) {
        m->run();
    }
}
