#include <string>
#include <vector>
#include <algorithm>
#include "module_wrapper.h"
#include "module_wrapper_factory.h"

// Include all the header files that define the modules.
#include "module_library/harmonic_oscillator.hpp"  // Contains harmonic_oscillator and harmonic_energy
#include "module_library/size_testing.hpp"         // Contains P1, P10, P100, and P1000
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
#include "module_library/module_graph_test.hpp"  // Includes Module_1, Module_2, and Module_3
#include "module_library/collatz_leaf.hpp"
#include "module_library/canac_with_collatz.hpp"
#include "module_library/big_leaf_multilayer_canopy.hpp"
#include "module_library/flowering.hpp"
#include "module_library/flowering_calculator.hpp"
#include "module_library/solar_zenith_angle.hpp"
#include "module_library/light_macro_environment.hpp"
#include "module_library/multilayer_canopy_properties.hpp"
#include "module_library/light_from_solar.hpp"
#include "module_library/night_and_day_trackers.hpp"
#include "module_library/oscillator_clock_calculator.hpp"
#include "module_library/poincare_clock.hpp"
#include "module_library/magic_clock.hpp"
#include "module_library/phase_clock.hpp"
#include "module_library/ed_ball_berry.h"
#include "module_library/ed_collatz_c4_assimilation.h"
#include "module_library/ed_gas_concentrations.h"
#include "module_library/ed_long_wave_energy_loss.h"
#include "module_library/ed_penman_monteith_leaf_temperature.h"
#include "module_library/golden_ratio_hyperbola.h"

template <typename T>
std::unique_ptr<module_wrapper_base> create_wrapper()
{
    return std::unique_ptr<module_wrapper_base>(new module_wrapper<T>);
}

std::unique_ptr<module_wrapper_base> module_wrapper_factory::create(std::string const& module_name)
{
    try {
        return modules.at(module_name)();
    } catch (std::out_of_range const& oor) {
        throw std::out_of_range(std::string("\"") + module_name + std::string("\"") + std::string(" was given as a module name, but no module with that name could be found.\n"));
    }
}

const std::unordered_map<std::string, module_wrapper_factory::f_ptr> module_wrapper_factory::modules =
{
     {"harmonic_oscillator",                             &create_wrapper<harmonic_oscillator>},
     {"harmonic_energy",                                 &create_wrapper<harmonic_energy>},
     //{"P1000",                                         &create_wrapper<P1000>},        // These modules introduce tons of pointless parameters and rarely are used, so just comment them out for now (P1, P10, P100, P1000)
     //{"P100",                                          &create_wrapper<P100>},        // These modules introduce tons of pointless parameters and rarely are used, so just comment them out for now (P1, P10, P100, P1000)
     //{"P10",                                           &create_wrapper<P10>},            // These modules introduce tons of pointless parameters and rarely are used, so just comment them out for now (P1, P10, P100, P1000)
     //{"P1",                                            &create_wrapper<P1>},            // These modules introduce tons of pointless parameters and rarely are used, so just comment them out for now (P1, P10, P100, P1000)
     {"reaction",                                        &create_wrapper<reaction>},
     {"nr_ex",                                           &create_wrapper<nr_ex>},
     {"one_layer_soil_profile",                          &create_wrapper<one_layer_soil_profile>},
     {"one_layer_soil_profile_derivatives",              &create_wrapper<one_layer_soil_profile_derivatives>},
     {"two_layer_soil_profile",                          &create_wrapper<two_layer_soil_profile>},
     {"soil_type_selector",                              &create_wrapper<soil_type_selector>},
     {"soil_evaporation",                                &create_wrapper<soil_evaporation>},
     {"parameter_calculator",                            &create_wrapper<parameter_calculator>},
     {"c3_canopy",                                       &create_wrapper<c3_canopy>},
     {"c4_canopy",                                       &create_wrapper<c4_canopy>},
     {"stomata_water_stress_linear",                     &create_wrapper<stomata_water_stress_linear>},
     {"stomata_water_stress_exponential",                &create_wrapper<stomata_water_stress_exponential>},
     {"stomata_water_stress_linear_and_aba_response",    &create_wrapper<stomata_water_stress_linear_and_aba_response>},
     {"stomata_water_stress_sigmoid",                    &create_wrapper<stomata_water_stress_sigmoid>},
     {"thermal_time_accumulator",                        &create_wrapper<thermal_time_accumulator>},
     {"utilization_growth",                              &create_wrapper<utilization_growth>},
     {"utilization_growth_calculator",                   &create_wrapper<utilization_growth_calculator>},
     {"utilization_senescence",                          &create_wrapper<utilization_senescence>},
     {"utilization_senescence_calculator",               &create_wrapper<utilization_senescence_calculator>},
     {"leaf_water_stress_exponential",                   &create_wrapper<leaf_water_stress_exponential>},
     {"biomass_leaf_n_limitation",                       &create_wrapper<biomass_leaf_n_limitation>},
     {"pokhilko_circadian_clock",                        &create_wrapper<pokhilko_circadian_clock>},
     {"song_flowering",                                  &create_wrapper<song_flowering>},
     {"fake_solar",                                      &create_wrapper<fake_solar>},
     {"partitioning_coefficient_selector",               &create_wrapper<partitioning_coefficient_selector>},
     {"partitioning_growth",                             &create_wrapper<partitioning_growth>},
     {"partitioning_growth_calculator",                  &create_wrapper<partitioning_growth_calculator>},
     {"no_leaf_resp_partitioning_growth_calculator",     &create_wrapper<no_leaf_resp_partitioning_growth_calculator>},
     {"thermal_time_senescence",                         &create_wrapper<thermal_time_senescence>},
     {"thermal_time_and_frost_senescence",               &create_wrapper<thermal_time_and_frost_senescence>},
     {"empty_senescence",                                &create_wrapper<empty_senescence>},
     {"aba_decay",                                       &create_wrapper<aba_decay>},
     {"ball_berry_module",                               &create_wrapper<ball_berry_module>},
     {"water_vapor_properties_from_air_temperature",     &create_wrapper<water_vapor_properties_from_air_temperature>},
     {"penman_monteith_transpiration",                   &create_wrapper<penman_monteith_transpiration>},
     {"penman_monteith_leaf_temperature",                &create_wrapper<penman_monteith_leaf_temperature>},
     {"priestley_transpiration",                         &create_wrapper<priestley_transpiration>},
     {"FvCB",                                            &create_wrapper<FvCB>},
     {"gamma_oscillator",                                &create_wrapper<gamma_oscillator>},
     {"position_oscillator",                             &create_wrapper<position_oscillator>},
     {"velocity_oscillator",                             &create_wrapper<velocity_oscillator>},
     {"test_module",                                     &create_wrapper<test_module>},
     {"test_calc_state",                                 &create_wrapper<test_calc_state>},
     {"test_derivs",                                     &create_wrapper<test_derivs>},
     {"bucket_soil_drainage",                            &create_wrapper<bucket_soil_drainage>},
     {"linear_vmax_from_leaf_n",                         &create_wrapper<linear_vmax_from_leaf_n>},
     {"Module_1",                                        &create_wrapper<Module_1>},
     {"Module_2",                                        &create_wrapper<Module_2>},
     {"Module_3",                                        &create_wrapper<Module_3>},
     {"collatz_leaf",                                    &create_wrapper<collatz_leaf>},
     {"canac_with_collatz",                              &create_wrapper<canac_with_collatz>},
     {"big_leaf_multilayer_canopy",                      &create_wrapper<big_leaf_multilayer_canopy>},
     {"flowering",                                       &create_wrapper<flowering>},
     {"flowering_calculator",                            &create_wrapper<flowering_calculator>},
     {"solar_zenith_angle",                              &create_wrapper<solar_zenith_angle>},
     {"light_macro_environment",                         &create_wrapper<light_macro_environment>},
     {"ten_layer_canopy_properties",                     &create_wrapper<ten_layer_canopy_properties>},
     {"magic_clock",                                     &create_wrapper<magic_clock>},
     {"poincare_clock",                                  &create_wrapper<poincare_clock>},
     {"phase_clock",                                     &create_wrapper<phase_clock>},
     {"oscillator_clock_calculator",                     &create_wrapper<oscillator_clock_calculator>},
     {"night_and_day_trackers",                          &create_wrapper<night_and_day_trackers>},
     {"light_from_solar",                                &create_wrapper<light_from_solar>},
     {"ed_ball_berry",                                   &create_wrapper<ed_ball_berry>},
     {"ed_collatz_c4_assimilation",                      &create_wrapper<ed_collatz_c4_assimilation>},
     {"ed_gas_concentrations",                           &create_wrapper<ed_gas_concentrations>},
     {"ed_long_wave_energy_loss",                        &create_wrapper<ed_long_wave_energy_loss>},
     {"ed_penman_monteith_leaf_temperature",             &create_wrapper<ed_penman_monteith_leaf_temperature>},
     {"golden_ratio_hyperbola",                          &create_wrapper<golden_ratio_hyperbola>}
};

std::vector<std::string> module_wrapper_factory::get_modules()
{
    std::vector<std::string> module_name_vector;
    for (auto const& x : modules) {
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

std::unordered_map<std::string, std::vector<std::string>> module_wrapper_factory::get_all_quantities()
{
    // Make the output map
    std::unordered_map<std::string, std::vector<std::string>> quantity_map = {
        {"module_name",     std::vector<std::string>{}},
        {"quantity_type",   std::vector<std::string>{}},
        {"quantity_name",   std::vector<std::string>{}}
    };

    // Make a lambda function for adding entries to the map
    auto add_quantity_map_entry = [&quantity_map](std::string module_name, std::string quantity_type, std::string quantity_name) {
        quantity_map["module_name"].push_back(module_name);
        quantity_map["quantity_type"].push_back(quantity_type);
        quantity_map["quantity_name"].push_back(quantity_name);
    };

    // Fill the output map with all the quantities
    for (std::string const& module_name : module_wrapper_factory::get_modules()) {
        auto w = module_wrapper_factory::create(module_name);

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