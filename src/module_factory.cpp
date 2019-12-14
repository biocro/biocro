#include <string>
#include <vector>
#include <algorithm>
#include "module_wrapper.h"
#include "module_factory.h"

// IMPORTANT: each module in the factory must have one entry each in the modules, input_parameter_names, and output_parameter_names lists
// The descriptions included in module_descriptions are recommended but not necessary

// Include all the header files that describe the modules
#include "module_library/harmonic_oscillator.hpp"    // Contains harmonic_oscillator and harmonic_energy
#include "module_library/size_testing.hpp"            // Contains P1, P10, P100, and P1000
#include "module_library/reaction.hpp"
#include "module_library/nr_ex.hpp"
#include "module_library/one_layer_soil_profile.hpp"
#include "module_library/one_layer_soil_profile_derivatives.hpp"
#include "module_library/two_layer_soil_profile.hpp"
#include "module_library/soil_type_selector.hpp"
#include "module_library/soil_evaporation.hpp"
#include "module_library/parameter_calculator.hpp"
#include "module_library/c3_canopy.hpp"
#include "module_library/c4_canopy.hpp"
#include "module_library/stomata_water_stress_linear.hpp"
#include "module_library/stomata_water_stress_exponential.hpp"
#include "module_library/stomata_water_stress_linear_aba_response.hpp"
#include "module_library/stomata_water_stress_sigmoid.hpp"
#include "module_library/thermal_time_accumulator.hpp"
#include "module_library/utilization_growth.hpp"
#include "module_library/utilization_growth_calculator.hpp"
#include "module_library/utilization_senescence.hpp"
#include "module_library/utilization_senescence_calculator.hpp"
#include "module_library/leaf_water_stress_exponential.hpp"
#include "module_library/biomass_leaf_n_limitation.hpp"
#include "module_library/pokhilko_circadian_clock.hpp"
#include "module_library/song_flowering.hpp"
#include "module_library/fake_solar.hpp"
#include "module_library/partitioning_coefficient_selector.hpp"
#include "module_library/partitioning_growth.hpp"
#include "module_library/partitioning_growth_calculator.hpp"
#include "module_library/no_leaf_resp_partitioning_growth_calculator.hpp"
#include "module_library/thermal_time_senescence.hpp"
#include "module_library/thermal_time_and_frost_senescence.hpp"
#include "module_library/empty_senescence.hpp"
#include "module_library/aba_decay.hpp"
#include "module_library/ball_berry_module.hpp"
#include "module_library/water_vapor_properties_from_air_temperature.hpp"
#include "module_library/penman_monteith_transpiration.hpp"
#include "module_library/penman_monteith_leaf_temperature.hpp"
#include "module_library/priestley_transpiration.hpp"
#include "module_library/FvCB.hpp"
#include "module_library/gamma_oscillator.hpp"
#include "module_library/position_oscillator.hpp"
#include "module_library/velocity_oscillator.hpp"
#include "module_library/test_module.hpp"
#include "module_library/test_calc_state.hpp"
#include "module_library/test_derivs.hpp"
#include "module_library/bucket_soil_drainage.hpp"
#include "module_library/linear_vmax_from_leaf_n.hpp"
#include "module_library/module_graph_test.hpp"        // Includes Module_1, Module_2, and Module_3
//#include "module_library/collatz_leaf.hpp"
//#include "module_library/canac_with_collatz.hpp"
//#include "module_library/big_leaf_multilayer_canopy.hpp"
#include "module_library/flowering.hpp"
#include "module_library/flowering_calculator.hpp"
#include "module_library/solar_zenith_angle.hpp"
#include "module_library/light_macro_environment.hpp"
//#include "module_library/multilayer_canopy_properties.hpp"
#include "module_library/light_from_solar.hpp"
#include "module_library/night_and_day_trackers.hpp"
#include "module_library/oscillator_clock_calculator.hpp"
#include "module_library/poincare_clock.hpp"
#include "module_library/magic_clock.hpp"
#include "module_library/phase_clock.hpp"


module_wrapper_base* ModuleFactory::create(std::string const &module_name) {
    try {
        return modules.at(module_name);
    } catch (std::out_of_range const &oor) {
        throw std::out_of_range(std::string("\"") + module_name + std::string("\"") + std::string(" was given as a module name, but no module with that name could be found.\n"));
    }
}


const std::unordered_map<std::string, module_wrapper_base*> ModuleFactory::modules =
    {
     {"harmonic_oscillator",                             new module_wrapper<harmonic_oscillator>()},
     {"harmonic_energy",                                 new module_wrapper<harmonic_energy>()},
     //{"P1000",                                         new module_wrapper<P1000>()},        // These modules introduce tons of pointless parameters and rarely are used, so just comment them out for now (P1, P10, P100, P1000)
     //{"P100",                                          new module_wrapper<P100>()},        // These modules introduce tons of pointless parameters and rarely are used, so just comment them out for now (P1, P10, P100, P1000)
     //{"P10",                                           new module_wrapper<P10>()},            // These modules introduce tons of pointless parameters and rarely are used, so just comment them out for now (P1, P10, P100, P1000)
     //{"P1",                                            new module_wrapper<P1>()},            // These modules introduce tons of pointless parameters and rarely are used, so just comment them out for now (P1, P10, P100, P1000)
     {"reaction",                                        new module_wrapper<reaction>()},
     {"nr_ex",                                           new module_wrapper<nr_ex>()},
     {"one_layer_soil_profile",                          new module_wrapper<one_layer_soil_profile>()},
     {"one_layer_soil_profile_derivatives",              new module_wrapper<one_layer_soil_profile_derivatives>()},
     {"two_layer_soil_profile",                          new module_wrapper<two_layer_soil_profile>()},
     {"soil_type_selector",                              new module_wrapper<soil_type_selector>()},
     {"soil_evaporation",                                new module_wrapper<soil_evaporation>()},
     {"parameter_calculator",                            new module_wrapper<parameter_calculator>()},
     {"c3_canopy",                                       new module_wrapper<c3_canopy>()},
     {"c4_canopy",                                       new module_wrapper<c4_canopy>()},
     {"stomata_water_stress_linear",                     new module_wrapper<stomata_water_stress_linear>()},
     {"stomata_water_stress_exponential",                new module_wrapper<stomata_water_stress_exponential>()},
     {"stomata_water_stress_linear_and_aba_response",    new module_wrapper<stomata_water_stress_linear_and_aba_response>()},
     {"stomata_water_stress_sigmoid",                    new module_wrapper<stomata_water_stress_sigmoid>()},
     {"thermal_time_accumulator",                        new module_wrapper<thermal_time_accumulator>()},
     {"utilization_growth",                              new module_wrapper<utilization_growth>()},
     {"utilization_growth_calculator",                   new module_wrapper<utilization_growth_calculator>()},
     {"utilization_senescence",                          new module_wrapper<utilization_senescence>()},
     {"utilization_senescence_calculator",               new module_wrapper<utilization_senescence_calculator>()},
     {"leaf_water_stress_exponential",                   new module_wrapper<leaf_water_stress_exponential>()},
     {"biomass_leaf_n_limitation",                       new module_wrapper<biomass_leaf_n_limitation>()},
     {"pokhilko_circadian_clock",                        new module_wrapper<pokhilko_circadian_clock>()},
     {"song_flowering",                                  new module_wrapper<song_flowering>()},
     {"fake_solar",                                      new module_wrapper<fake_solar>()},
     {"partitioning_coefficient_selector",               new module_wrapper<partitioning_coefficient_selector>()},
     {"partitioning_growth",                             new module_wrapper<partitioning_growth>()},
     {"partitioning_growth_calculator",                  new module_wrapper<partitioning_growth_calculator>()},
     {"no_leaf_resp_partitioning_growth_calculator",     new module_wrapper<no_leaf_resp_partitioning_growth_calculator>()},
     {"thermal_time_senescence",                         new module_wrapper<thermal_time_senescence>()},
     {"thermal_time_and_frost_senescence",               new module_wrapper<thermal_time_and_frost_senescence>()},
     {"empty_senescence",                                new module_wrapper<empty_senescence>()},
     {"aba_decay",                                       new module_wrapper<aba_decay>()},
     {"ball_berry_module",                               new module_wrapper<ball_berry_module>()},
     {"water_vapor_properties_from_air_temperature",     new module_wrapper<water_vapor_properties_from_air_temperature>()},
     {"penman_monteith_transpiration",                   new module_wrapper<penman_monteith_transpiration>()},
     {"penman_monteith_leaf_temperature",                new module_wrapper<penman_monteith_leaf_temperature>()},
     {"priestley_transpiration",                         new module_wrapper<priestley_transpiration>()},
     {"FvCB",                                            new module_wrapper<FvCB>()},
     {"gamma_oscillator",                                new module_wrapper<gamma_oscillator>()},
     {"position_oscillator",                             new module_wrapper<position_oscillator>()},
     {"velocity_oscillator",                             new module_wrapper<velocity_oscillator>()},
     {"test_module",                                     new module_wrapper<test_module>()},
     {"test_calc_state",                                 new module_wrapper<test_calc_state>()},
     {"test_derivs",                                     new module_wrapper<test_derivs>()},
     {"bucket_soil_drainage",                            new module_wrapper<bucket_soil_drainage>()},
     {"linear_vmax_from_leaf_n",                         new module_wrapper<linear_vmax_from_leaf_n>()},
     {"Module_1",                                        new module_wrapper<Module_1>()},
     {"Module_2",                                        new module_wrapper<Module_2>()},
     {"Module_3",                                        new module_wrapper<Module_3>()},
     //{"collatz_leaf",                                    new module_wrapper<collatz_leaf>()},
     //{"canac_with_collatz",                              new module_wrapper<canac_with_collatz>()},
     //{"big_leaf_multilayer_canopy",                      new module_wrapper<big_leaf_multilayer_canopy>()},
     {"flowering",                                       new module_wrapper<flowering>()},
     {"flowering_calculator",                            new module_wrapper<flowering_calculator>()},
     {"solar_zenith_angle",                              new module_wrapper<solar_zenith_angle>()},
     {"light_macro_environment",                         new module_wrapper<light_macro_environment>()},
     //{"ten_layer_canopy_properties",                     new module_wrapper<ten_layer_canopy_properties>()},
     {"magic_clock",                                     new module_wrapper<magic_clock>()},
     {"poincare_clock",                                  new module_wrapper<poincare_clock>()},
     {"phase_clock",                                     new module_wrapper<phase_clock>()},
     {"oscillator_clock_calculator",                     new module_wrapper<oscillator_clock_calculator>()},
     {"night_and_day_trackers",                          new module_wrapper<night_and_day_trackers>()},
     {"light_from_solar",                                new module_wrapper<light_from_solar>()}
    };

std::vector<std::string> ModuleFactory::get_modules() {
    std::vector<std::string> module_name_vector;
    for (auto const& x : modules) {
        module_name_vector.push_back(x.first);
    }

    class Case_Insensitive_Compare {
    public:
        bool operator()(std::string const &a, std::string const &b) {
            // Make a lowercase copy of a
            std::string al = a;
            std::transform(al.begin(), al.end(), al.begin(), [](unsigned char c){return std::tolower(c);});
    
            // Make a lowercase copy of b
            std::string bl = b;
            std::transform(bl.begin(), bl.end(), bl.begin(), [](unsigned char c){return std::tolower(c);});
    
            int compare = al.compare(bl);
            return (compare > 0) ? false : true;
        }
    };

    std::sort(module_name_vector.begin(), module_name_vector.end());//, Case_Insensitive_Compare {});

    return module_name_vector;
}
