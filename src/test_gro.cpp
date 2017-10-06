#include <vector>
#include "modules.h"

void test_gro () {
    state_map results;

    state_map state;
    std::vector<IModule*> steady_modules;
    std::vector<IModule*> derivative_modules;

    derivative_modules.push_back(new c3_canopy());
    derivative_modules.push_back(new one_layer_soil_profile());

    results = Gro(state, steady_modules, derivative_modules);
}

