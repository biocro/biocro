#include <vector>
#include <memory>
#include "modules.h"

void test_gro () {
    state_map results;

    state_map state;
    std::vector<std::unique_ptr<IModule>> steady_modules;
    std::vector<std::unique_ptr<IModule>> derivative_modules;

    steady_modules.push_back(std::unique_ptr<IModule>(new test_calc_state));
    derivative_modules.push_back(std::unique_ptr<IModule>(new test_derivs));

    results = Gro(state, steady_modules, derivative_modules);
}


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
