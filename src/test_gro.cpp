// EBL is commenting out this entire file because the module class has changed significantly (2019-08-22)
// Note: the "f" and "intergrator" functions were already commented out

/*
#include <vector>
#include <memory>
#include "modules.h"
#include "Gro.h"
#include "module_library/test_calc_state.hpp"
#include "module_library/test_derivs.hpp"

state_map test_gro(state_map const &state) {
    std::vector<std::unique_ptr<IModule>> steady_modules;
    std::vector<std::unique_ptr<IModule>> derivative_modules;

    steady_modules.push_back(std::unique_ptr<IModule>(new test_calc_state));
    derivative_modules.push_back(std::unique_ptr<IModule>(new test_derivs));

    return Gro(state, steady_modules, derivative_modules);
}
*/


/*
void f(parameters)
{
    biocro_state = subset(parameters);
    simroot_state = subset(parameters);

    biocro_dxdt = biocro(biocrostate);
    simroot_dxdt = biocro(simroot_state);



    return union(biocro_dxdt, simroot_dxdt);
}


intergrator(f, start, end)

*/
