#include <algorithm>  // for std::transform
#include <cctype>     // for std::tolower
#include "module_library.h"

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
#include "shortwave_atmospheric_scattering.h"
#include "incident_shortwave_from_ground_par.h"
#include "leaf_shape_factor.h"
#include "rue_leaf_photosynthesis.h"
#include "c3_assimilation.h"
#include "c3_leaf_photosynthesis.h"
#include "c4_assimilation.h"
#include "c4_leaf_photosynthesis.h"
#include "multilayer_canopy_properties.h"
#include "multilayer_rue_canopy.h"
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
#include "ed_leaf_temperature.h"
#include "ed_penman_monteith_transpiration.h"
#include "ed_stomata_water_stress_linear.h"
#include "ed_apply_water_stress.h"
#include "ed_c4_leaf_photosynthesis.h"
#include "ed_c4_leaf_photosynthesis2.h"
#include "ed_c4_leaf_photosynthesis3.h"
#include "ed_c4_leaf_photosynthesis4.h"
#include "ed_c4photo.h"
#include "ed_evapotrans2.h"
#include "ed_canac_leaf.h"
#include "ed_abc_blc.h"
#include "ed_multilayer_canopy_properties.h"
#include "ed_multilayer_c4_canopy.h"
#include "ed_multilayer_canopy_integrator.h"
#include "hyperbolas.h"
#include "partitioning_coefficient_logistic.h"
#include "senescence_coefficient_logistic.h"
#include "senescence_logistic.h"
#include "development_index.h"
#include "soybean_development_rate_calculator.h"
#include "thermal_time_development_rate_calculator.h"
#include "no_leaf_resp_neg_assim_partitioning_growth_calculator.h"
#include "rasmussen_specific_heat.h"
#include "buck_swvp.h"
#include "rh_to_mole_fraction.h"
#include "total_biomass.h"
#include "grimm_soybean_flowering.h"
#include "grimm_soybean_flowering_calculator.h"
#include "solar_position_michalsky.h"
#include "leaf_gbw_thornley.h"
#include "leaf_gbw_nikolov.h"
#include "example_model_mass_gain.h"
#include "example_model_partitioning.h"

/**
 * @brief A function that returns a pointer to a module_creator object.
 */
template <typename T>
module_creator* create_mc()
{
    return new module_creator_impl<T>;
}

module_creator* module_library::retrieve(std::string const& module_name)
{
    try {
        return module_library::library_entries.at(module_name)();
    } catch (std::out_of_range const&) {
        std::string message = std::string("\"") + module_name +
                       std::string("\"") +
                       std::string(" was given as a module name, ") +
                       std::string("but no module with that name could be found.\n");

        throw std::out_of_range(message);
    }
}

module_library::creator_map module_library::library_entries =
{
     {"harmonic_oscillator",                                   &create_mc<harmonic_oscillator>},
     {"harmonic_energy",                                       &create_mc<harmonic_energy>},
     {"nr_ex",                                                 &create_mc<nr_ex>},
     {"one_layer_soil_profile",                                &create_mc<one_layer_soil_profile>},
     {"one_layer_soil_profile_derivatives",                    &create_mc<one_layer_soil_profile_derivatives>},
     {"two_layer_soil_profile",                                &create_mc<two_layer_soil_profile>},
     {"soil_type_selector",                                    &create_mc<soil_type_selector>},
     {"soil_evaporation",                                      &create_mc<soil_evaporation>},
     {"parameter_calculator",                                  &create_mc<parameter_calculator>},
     {"c3_canopy",                                             &create_mc<c3_canopy>},
     {"c4_canopy",                                             &create_mc<c4_canopy>},
     {"stomata_water_stress_linear",                           &create_mc<stomata_water_stress_linear>},
     {"stomata_water_stress_exponential",                      &create_mc<stomata_water_stress_exponential>},
     {"stomata_water_stress_linear_and_aba_response",          &create_mc<stomata_water_stress_linear_and_aba_response>},
     {"stomata_water_stress_sigmoid",                          &create_mc<stomata_water_stress_sigmoid>},
     {"thermal_time_linear",                                   &create_mc<thermal_time_linear>},
     {"thermal_time_linear_extended",                          &create_mc<thermal_time_linear_extended>},
     {"thermal_time_bilinear",                                 &create_mc<thermal_time_bilinear>},
     {"thermal_time_trilinear",                                &create_mc<thermal_time_trilinear>},
     {"thermal_time_beta",                                     &create_mc<thermal_time_beta>},
     {"utilization_growth",                                    &create_mc<utilization_growth>},
     {"utilization_growth_calculator",                         &create_mc<utilization_growth_calculator>},
     {"utilization_senescence",                                &create_mc<utilization_senescence>},
     {"utilization_senescence_calculator",                     &create_mc<utilization_senescence_calculator>},
     {"leaf_water_stress_exponential",                         &create_mc<leaf_water_stress_exponential>},
     {"biomass_leaf_n_limitation",                             &create_mc<biomass_leaf_n_limitation>},
     {"pokhilko_circadian_clock",                              &create_mc<pokhilko_circadian_clock>},
     {"song_flowering",                                        &create_mc<song_flowering>},
     {"fake_solar",                                            &create_mc<fake_solar>},
     {"partitioning_coefficient_selector",                     &create_mc<partitioning_coefficient_selector>},
     {"partitioning_growth",                                   &create_mc<partitioning_growth>},
     {"partitioning_growth_calculator",                        &create_mc<partitioning_growth_calculator>},
     {"no_leaf_resp_partitioning_growth_calculator",           &create_mc<no_leaf_resp_partitioning_growth_calculator>},
     {"thermal_time_senescence",                               &create_mc<thermal_time_senescence>},
     {"thermal_time_and_frost_senescence",                     &create_mc<thermal_time_and_frost_senescence>},
     {"aba_decay",                                             &create_mc<aba_decay>},
     {"ball_berry_module",                                     &create_mc<ball_berry_module>},
     {"water_vapor_properties_from_air_temperature",           &create_mc<water_vapor_properties_from_air_temperature>},
     {"penman_monteith_transpiration",                         &create_mc<penman_monteith_transpiration>},
     {"penman_monteith_leaf_temperature",                      &create_mc<penman_monteith_leaf_temperature>},
     {"priestley_transpiration",                               &create_mc<priestley_transpiration>},
     {"FvCB",                                                  &create_mc<FvCB>},
     {"bucket_soil_drainage",                                  &create_mc<bucket_soil_drainage>},
     {"linear_vmax_from_leaf_n",                               &create_mc<linear_vmax_from_leaf_n>},
     {"Module_1",                                              &create_mc<Module_1>},
     {"Module_2",                                              &create_mc<Module_2>},
     {"Module_3",                                              &create_mc<Module_3>},
     {"shortwave_atmospheric_scattering",                      &create_mc<shortwave_atmospheric_scattering>},
     {"incident_shortwave_from_ground_par",                    &create_mc<incident_shortwave_from_ground_par>},
     {"leaf_shape_factor",                                     &create_mc<leaf_shape_factor>},
     {"rue_leaf_photosynthesis",                               &create_mc<rue_leaf_photosynthesis>},
     {"c3_assimilation",                                       &create_mc<c3_assimilation>},
     {"c3_leaf_photosynthesis",                                &create_mc<c3_leaf_photosynthesis>},
     {"c4_assimilation",                                       &create_mc<c4_assimilation>},
     {"c4_leaf_photosynthesis",                                &create_mc<c4_leaf_photosynthesis>},
     {"ten_layer_canopy_properties",                           &create_mc<ten_layer_canopy_properties>},
     {"ten_layer_rue_canopy",                                  &create_mc<ten_layer_rue_canopy>},
     {"ten_layer_c3_canopy",                                   &create_mc<ten_layer_c3_canopy>},
     {"ten_layer_c4_canopy",                                   &create_mc<ten_layer_c4_canopy>},
     {"ten_layer_canopy_integrator",                           &create_mc<ten_layer_canopy_integrator>},
     {"magic_clock",                                           &create_mc<magic_clock>},
     {"poincare_clock",                                        &create_mc<poincare_clock>},
     {"phase_clock",                                           &create_mc<phase_clock>},
     {"oscillator_clock_calculator",                           &create_mc<oscillator_clock_calculator>},
     {"night_and_day_trackers",                                &create_mc<night_and_day_trackers>},
     {"light_from_solar",                                      &create_mc<light_from_solar>},
     {"ed_water_vapor_properties",                             &create_mc<ed_water_vapor_properties>},
     {"ed_rh_to_mole_fraction",                                &create_mc<ed_rh_to_mole_fraction>},
     {"ed_nikolov_conductance_forced",                         &create_mc<ed_nikolov_conductance_forced>},
     {"ed_nikolov_conductance_free",                           &create_mc<ed_nikolov_conductance_free>},
     {"ed_nikolov_conductance_free_solve",                     &create_mc<ed_nikolov_conductance_free_solve>},
     {"ed_boundary_conductance_max",                           &create_mc<ed_boundary_conductance_max>},
     {"ed_boundary_conductance_quadrature",                    &create_mc<ed_boundary_conductance_quadrature>},
     {"ed_ball_berry",                                         &create_mc<ed_ball_berry>},
     {"ed_collatz_c4_assimilation",                            &create_mc<ed_collatz_c4_assimilation>},
     {"ed_gas_concentrations",                                 &create_mc<ed_gas_concentrations>},
     {"ed_long_wave_energy_loss",                              &create_mc<ed_long_wave_energy_loss>},
     {"ed_penman_monteith_leaf_temperature",                   &create_mc<ed_penman_monteith_leaf_temperature>},
     {"partitioning_coefficient_logistic",                     &create_mc<partitioning_coefficient_logistic>},
     {"senescence_coefficient_logistic",                       &create_mc<senescence_coefficient_logistic>},
     {"senescence_logistic",                                   &create_mc<senescence_logistic>},
     {"development_index",                                     &create_mc<development_index>},
     {"soybean_development_rate_calculator",                   &create_mc<soybean_development_rate_calculator>},
     {"thermal_time_development_rate_calculator",              &create_mc<thermal_time_development_rate_calculator>},
     {"ed_p_m_temperature_solve",                              &create_mc<ed_p_m_temperature_solve>},
     {"ed_leaf_temperature",                                   &create_mc<ed_leaf_temperature>},
     {"ed_penman_monteith_transpiration",                      &create_mc<ed_penman_monteith_transpiration>},
     {"ed_stomata_water_stress_linear",                        &create_mc<ed_stomata_water_stress_linear>},
     {"ed_apply_stomatal_water_stress_via_conductance",        &create_mc<ed_apply_stomatal_water_stress_via_conductance>},
     {"ed_apply_stomatal_water_stress_via_assimilation",       &create_mc<ed_apply_stomatal_water_stress_via_assimilation>},
     {"ed_c4_leaf_photosynthesis",                             &create_mc<ed_c4_leaf_photosynthesis>},
     {"ed_c4_leaf_photosynthesis2",                            &create_mc<ed_c4_leaf_photosynthesis2>},
     {"ed_c4_leaf_photosynthesis3",                            &create_mc<ed_c4_leaf_photosynthesis3>},
     {"ed_c4_leaf_photosynthesis4",                            &create_mc<ed_c4_leaf_photosynthesis4>},
     {"ed_c4photo",                                            &create_mc<ed_c4photo>},
     {"ed_evapotrans2",                                        &create_mc<ed_evapotrans2>},
     {"ed_canac_leaf",                                         &create_mc<ed_canac_leaf>},
     {"ed_abc_blc",                                            &create_mc<ed_abc_blc>},
     {"ed_ten_layer_canopy_properties",                        &create_mc<ed_ten_layer_canopy_properties>},
     {"ed_ten_layer_c4_canopy",                                &create_mc<ed_ten_layer_c4_canopy>},
     {"ed_ten_layer_canopy_integrator",                        &create_mc<ed_ten_layer_canopy_integrator>},
     {"golden_ratio_hyperbola",                                &create_mc<golden_ratio_hyperbola>},
     {"hyperbola_2d",                                          &create_mc<hyperbola_2d>},
     {"no_leaf_resp_neg_assim_partitioning_growth_calculator", &create_mc<no_leaf_resp_neg_assim_partitioning_growth_calculator>},
     {"rasmussen_specific_heat",                               &create_mc<rasmussen_specific_heat>},
     {"buck_swvp",                                             &create_mc<buck_swvp>},
     {"rh_to_mole_fraction",                                   &create_mc<rh_to_mole_fraction>},
     {"total_biomass",                                         &create_mc<total_biomass>},
     {"grimm_soybean_flowering",                               &create_mc<grimm_soybean_flowering>},
     {"grimm_soybean_flowering_calculator",                    &create_mc<grimm_soybean_flowering_calculator>},
     {"solar_position_michalsky",                              &create_mc<solar_position_michalsky>},
     {"leaf_gbw_thornley",                                     &create_mc<leaf_gbw_thornley>},
     {"leaf_gbw_nikolov",                                      &create_mc<leaf_gbw_nikolov>},
     {"example_model_mass_gain",                               &create_mc<example_model_mass_gain>},
     {"example_model_partitioning",                            &create_mc<example_model_partitioning>},
};

string_vector module_library::get_all_modules()
{
    string_vector module_name_vector;
    for (auto const& x : library_entries) {
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

std::unordered_map<std::string, string_vector> module_library::get_all_quantities()
{
    // Make the output map
    std::unordered_map<std::string, string_vector> quantity_map = {
        {"module_name",     string_vector{}},
        {"quantity_type",   string_vector{}},
        {"quantity_name",   string_vector{}}
    };

    // Make a lambda function for adding entries to the map
    auto add_quantity_map_entry = [&quantity_map](std::string module_name, std::string quantity_type, std::string quantity_name) {
        quantity_map["module_name"].push_back(module_name);
        quantity_map["quantity_type"].push_back(quantity_type);
        quantity_map["quantity_name"].push_back(quantity_name);
    };

    // Fill the output map with all the quantities
    for (std::string const& module_name : module_library::get_all_modules()) {
        auto w = module_library::retrieve(module_name);

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
