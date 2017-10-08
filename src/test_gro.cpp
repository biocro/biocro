#include <vector>
#include <memory>
#include "modules.h"

void test_gro () {
    state_map results;

    state_map state;
    std::vector<std::unique_ptr<IModule>> steady_modules;
    std::vector<std::unique_ptr<IModule>> derivative_modules;

    derivative_modules.push_back(std::unique_ptr<IModule>(new c3_canopy));
    derivative_modules.push_back(std::unique_ptr<IModule>(new one_layer_soil_profile));

    results = Gro(state, steady_modules, derivative_modules);
}

