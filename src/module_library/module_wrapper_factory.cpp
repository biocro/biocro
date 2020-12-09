#include <algorithm> // for std::transform
#include <cctype>   // for std::tolower
#include "module_wrapper_factory.h"

// Include all the header files that define the modules.
#include "harmonic_oscillator.hpp"  // Contains harmonic_oscillator and harmonic_energy
#include "nr_ex.hpp"
#include "one_layer_soil_profile.hpp"
#include "one_layer_soil_profile_derivatives.hpp"
#include "two_layer_soil_profile.hpp"
#include "soil_type_selector.hpp"
#include "soil_evaporation.hpp"
#include "parameter_calculator.hpp"
#include "c3_canopy.hpp"
#include "c4_canopy.hpp"
#include "stomata_water_stress_linear.hpp"
#include "stomata_water_stress_exponential.hpp"
#include "stomata_water_stress_linear_aba_response.hpp"
#include "stomata_water_stress_sigmoid.hpp"
#include "thermal_time_linear.h"
#include "thermal_time_linear_extended.h"
#include "thermal_time_bilinear.h"
#include "thermal_time_trilinear.h"
#include "thermal_time_beta.h"
#include "utilization_growth.hpp"
#include "utilization_growth_calculator.hpp"
#include "utilization_senescence.hpp"
#include "utilization_senescence_calculator.hpp"
#include "leaf_water_stress_exponential.hpp"
#include "biomass_leaf_n_limitation.hpp"
#include "pokhilko_circadian_clock.hpp"
#include "song_flowering.hpp"
#include "fake_solar.hpp"
#include "partitioning_coefficient_selector.hpp"
#include "partitioning_growth.hpp"
#include "partitioning_growth_calculator.hpp"
#include "no_leaf_resp_partitioning_growth_calculator.hpp"
#include "thermal_time_senescence.hpp"
#include "thermal_time_and_frost_senescence.hpp"
#include "empty_senescence.hpp"
#include "aba_decay.hpp"
#include "ball_berry_module.hpp"
#include "water_vapor_properties_from_air_temperature.hpp"
#include "penman_monteith_transpiration.hpp"
#include "penman_monteith_leaf_temperature.hpp"
#include "priestley_transpiration.hpp"
#include "FvCB.hpp"
#include "bucket_soil_drainage.hpp"
#include "linear_vmax_from_leaf_n.hpp"
#include "module_graph_test.hpp"  // Includes Module_1, Module_2, and Module_3
#include "collatz_leaf.hpp"
#include "canac_with_collatz.hpp"
#include "big_leaf_multilayer_canopy.hpp"
#include "solar_zenith_angle.h"
#include "shortwave_atmospheric_scattering.h"
#include "incident_shortwave_from_ground_par.h"
#include "leaf_shape_factor.h"
#include "c3_assimilation.h"
#include "c3_leaf_photosynthesis.h"
#include "c4_assimilation.h"
#include "c4_leaf_photosynthesis.h"
#include "multilayer_canopy_properties.h"
#include "multilayer_c3_canopy.h"
#include "multilayer_c4_canopy.h"
#include "multilayer_canopy_integrator.h"
#include "light_from_solar.hpp"
#include "night_and_day_trackers.hpp"
#include "oscillator_clock_calculator.hpp"
#include "poincare_clock.hpp"
#include "magic_clock.hpp"
#include "phase_clock.hpp"
#include "ed_water_vapor_properties.h"
#include "ed_rh_to_mole_fraction.h"
#include "ed_nikolov_conductance.h"
#include "ed_boundary_conductance.h"
#include "ed_ball_berry.h"
#include "ed_collatz_c4_assimilation.h"
#include "ed_gas_concentrations.h"
#include "ed_long_wave_energy_loss.h"
#include "ed_penman_monteith_leaf_temperature.h"
#include "ed_stomata_water_stress_linear.h"
#include "ed_apply_water_stress.h"
#include "ed_c4_leaf_photosynthesis.h"
#include "ed_c4photo.h"
#include "ed_evapotrans2.h"
#include "ed_canac_leaf.h"
#include "hyperbolas.h"

/**
 * @brief A function that returns a unique_ptr to a module_wrapper_base object.
 */
template <typename T>
std::unique_ptr<module_wrapper_base> create_wrapper()
{
    return std::unique_ptr<module_wrapper_base>(new module_wrapper<T>);
}

std::unique_ptr<module_wrapper_base> module_wrapper_factory::create(std::string const& module_name)
{
    try {
        return module_wrapper_factory::module_wrapper_creators.at(module_name)();
    } catch (std::out_of_range) {
        std::string message = std::string("\"") + module_name +
                       std::string("\"") +
                       std::string(" was given as a module name, ") +
                       std::string("but no module with that name could be found.\n");

        throw std::out_of_range(message);
    }
}

module_wrapper_factory::module_wrapper_creator_map module_wrapper_factory::module_wrapper_creators =
{
     {"harmonic_oscillator",                             &create_wrapper<harmonic_oscillator>},
     {"harmonic_energy",                                 &create_wrapper<harmonic_energy>},
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
     {"thermal_time_linear",                             &create_wrapper<thermal_time_linear>},
     {"thermal_time_linear_extended",                    &create_wrapper<thermal_time_linear_extended>},
     {"thermal_time_bilinear",                           &create_wrapper<thermal_time_bilinear>},
     {"thermal_time_trilinear",                          &create_wrapper<thermal_time_trilinear>},
     {"thermal_time_beta",                               &create_wrapper<thermal_time_beta>},
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
     {"bucket_soil_drainage",                            &create_wrapper<bucket_soil_drainage>},
     {"linear_vmax_from_leaf_n",                         &create_wrapper<linear_vmax_from_leaf_n>},
     {"Module_1",                                        &create_wrapper<Module_1>},
     {"Module_2",                                        &create_wrapper<Module_2>},
     {"Module_3",                                        &create_wrapper<Module_3>},
     {"collatz_leaf",                                    &create_wrapper<collatz_leaf>},
     {"canac_with_collatz",                              &create_wrapper<canac_with_collatz>},
     {"big_leaf_multilayer_canopy",                      &create_wrapper<big_leaf_multilayer_canopy>},
     {"solar_zenith_angle",                              &create_wrapper<solar_zenith_angle>},
     {"shortwave_atmospheric_scattering",                &create_wrapper<shortwave_atmospheric_scattering>},
     {"incident_shortwave_from_ground_par",              &create_wrapper<incident_shortwave_from_ground_par>},
     {"leaf_shape_factor",                               &create_wrapper<leaf_shape_factor>},
     {"c3_assimilation",                                 &create_wrapper<c3_assimilation>},
     {"c3_leaf_photosynthesis",                          &create_wrapper<c3_leaf_photosynthesis>},
     {"c4_assimilation",                                 &create_wrapper<c4_assimilation>},
     {"c4_leaf_photosynthesis",                          &create_wrapper<c4_leaf_photosynthesis>},
     {"ten_layer_canopy_properties",                     &create_wrapper<ten_layer_canopy_properties>},
     {"ten_layer_c3_canopy",                             &create_wrapper<ten_layer_c3_canopy>},
     {"ten_layer_c4_canopy",                             &create_wrapper<ten_layer_c4_canopy>},
     {"ten_layer_canopy_integrator",                     &create_wrapper<ten_layer_canopy_integrator>},
     {"magic_clock",                                     &create_wrapper<magic_clock>},
     {"poincare_clock",                                  &create_wrapper<poincare_clock>},
     {"phase_clock",                                     &create_wrapper<phase_clock>},
     {"oscillator_clock_calculator",                     &create_wrapper<oscillator_clock_calculator>},
     {"night_and_day_trackers",                          &create_wrapper<night_and_day_trackers>},
     {"light_from_solar",                                &create_wrapper<light_from_solar>},
     {"ed_water_vapor_properties",                       &create_wrapper<ed_water_vapor_properties>},
     {"ed_rh_to_mole_fraction",                          &create_wrapper<ed_rh_to_mole_fraction>},
     {"ed_nikolov_conductance_forced",                   &create_wrapper<ed_nikolov_conductance_forced>},
     {"ed_nikolov_conductance_free",                     &create_wrapper<ed_nikolov_conductance_free>},
     {"ed_boundary_conductance_max",                     &create_wrapper<ed_boundary_conductance_max>},
     {"ed_boundary_conductance_quadrature",              &create_wrapper<ed_boundary_conductance_quadrature>},
     {"ed_ball_berry",                                   &create_wrapper<ed_ball_berry>},
     {"ed_collatz_c4_assimilation",                      &create_wrapper<ed_collatz_c4_assimilation>},
     {"ed_gas_concentrations",                           &create_wrapper<ed_gas_concentrations>},
     {"ed_long_wave_energy_loss",                        &create_wrapper<ed_long_wave_energy_loss>},
     {"ed_penman_monteith_leaf_temperature",             &create_wrapper<ed_penman_monteith_leaf_temperature>},
     {"ed_stomata_water_stress_linear",                  &create_wrapper<ed_stomata_water_stress_linear>},
     {"ed_apply_stomatal_water_stress_via_conductance",  &create_wrapper<ed_apply_stomatal_water_stress_via_conductance>},
     {"ed_apply_stomatal_water_stress_via_assimilation", &create_wrapper<ed_apply_stomatal_water_stress_via_assimilation>},
     {"ed_c4_leaf_photosynthesis",                       &create_wrapper<ed_c4_leaf_photosynthesis>},
     {"ed_c4photo",                                      &create_wrapper<ed_c4photo>},
     {"ed_evapotrans2",                                  &create_wrapper<ed_evapotrans2>},
     {"ed_canac_leaf",                                   &create_wrapper<ed_canac_leaf>},
     {"golden_ratio_hyperbola",                          &create_wrapper<golden_ratio_hyperbola>},
     {"hyperbola_2d",                                    &create_wrapper<hyperbola_2d>}
};

std::vector<std::string> module_wrapper_factory::get_modules()
{
    std::vector<std::string> module_name_vector;
    for (auto const& x : module_wrapper_creators) {
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
