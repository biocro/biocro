#include "module_library.h"
#include "../framework/module_creator.h"  // for create_mc

// When creating a new module library R package, it will be necessary to modify
// the namespace in this file to match the one defined in `module_library.h`.
// See that file for more details. It will also be necessary to include
// different module header files and to make corresponding changes to the
// entries in the `creator_map` table.

// Include all the header files that define the modules.
#include "harmonic_oscillator.h"  // Contains harmonic_oscillator and harmonic_energy
#include "nr_ex.h"
#include "one_layer_soil_profile.h"
#include "one_layer_soil_profile_derivatives.h"
#include "two_layer_soil_profile.h"
#include "soil_evaporation.h"
#include "parameter_calculator.h"
#include "c3_canopy.h"
#include "c4_canopy.h"
#include "varying_Jmax25.h"
#include "stomata_water_stress_linear.h"
#include "stomata_water_stress_exponential.h"
#include "stomata_water_stress_linear_aba_response.h"
#include "stomata_water_stress_sigmoid.h"
#include "thermal_time_linear.h"
#include "thermal_time_linear_extended.h"
#include "thermal_time_bilinear.h"
#include "thermal_time_trilinear.h"
#include "thermal_time_beta.h"
#include "leaf_water_stress_exponential.h"
#include "biomass_leaf_n_limitation.h"
#include "song_flowering.h"
#include "fake_solar.h"
#include "partitioning_coefficient_selector.h"
#include "partitioning_growth.h"
#include "partitioning_growth_calculator.h"
#include "no_leaf_resp_partitioning_growth_calculator.h"
#include "thermal_time_senescence.h"
#include "thermal_time_and_frost_senescence.h"
#include "aba_decay.h"
#include "ball_berry.h"
#include "water_vapor_properties_from_air_temperature.h"
#include "penman_monteith_transpiration.h"
#include "penman_monteith_leaf_temperature.h"
#include "priestley_transpiration.h"
#include "FvCB.h"
#include "bucket_soil_drainage.h"
#include "linear_vmax_from_leaf_n.h"
#include "module_graph_test.h"  // Includes Module_1, Module_2, and Module_3
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
#include "light_from_solar.h"
#include "night_and_day_trackers.h"
#include "oscillator_clock_calculator.h"
#include "poincare_clock.h"
#include "magic_clock.h"
#include "phase_clock.h"
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
#include "canopy_gbw_thornley.h"
#include "leaf_gbw_nikolov.h"
#include "example_model_mass_gain.h"
#include "example_model_partitioning.h"
#include "litter_cover.h"
#include "soil_sunlight.h"
#include "leaf_evapotranspiration.h"

creator_map standardBML::module_library::library_entries =
{
     {"harmonic_oscillator",                                   &create_mc<harmonic_oscillator>},
     {"harmonic_energy",                                       &create_mc<harmonic_energy>},
     {"nr_ex",                                                 &create_mc<nr_ex>},
     {"one_layer_soil_profile",                                &create_mc<one_layer_soil_profile>},
     {"one_layer_soil_profile_derivatives",                    &create_mc<one_layer_soil_profile_derivatives>},
     {"two_layer_soil_profile",                                &create_mc<two_layer_soil_profile>},
     {"soil_evaporation",                                      &create_mc<soil_evaporation>},
     {"parameter_calculator",                                  &create_mc<parameter_calculator>},
     {"c3_canopy",                                             &create_mc<c3_canopy>},
     {"c4_canopy",                                             &create_mc<c4_canopy>},
     {"varying_Jmax25",                                        &create_mc<varying_Jmax25>},
     {"stomata_water_stress_linear",                           &create_mc<stomata_water_stress_linear>},
     {"stomata_water_stress_exponential",                      &create_mc<stomata_water_stress_exponential>},
     {"stomata_water_stress_linear_and_aba_response",          &create_mc<stomata_water_stress_linear_and_aba_response>},
     {"stomata_water_stress_sigmoid",                          &create_mc<stomata_water_stress_sigmoid>},
     {"thermal_time_linear",                                   &create_mc<thermal_time_linear>},
     {"thermal_time_linear_extended",                          &create_mc<thermal_time_linear_extended>},
     {"thermal_time_bilinear",                                 &create_mc<thermal_time_bilinear>},
     {"thermal_time_trilinear",                                &create_mc<thermal_time_trilinear>},
     {"thermal_time_beta",                                     &create_mc<thermal_time_beta>},
     {"leaf_water_stress_exponential",                         &create_mc<leaf_water_stress_exponential>},
     {"biomass_leaf_n_limitation",                             &create_mc<biomass_leaf_n_limitation>},
     {"song_flowering",                                        &create_mc<song_flowering>},
     {"fake_solar",                                            &create_mc<fake_solar>},
     {"partitioning_coefficient_selector",                     &create_mc<partitioning_coefficient_selector>},
     {"partitioning_growth",                                   &create_mc<partitioning_growth>},
     {"partitioning_growth_calculator",                        &create_mc<partitioning_growth_calculator>},
     {"no_leaf_resp_partitioning_growth_calculator",           &create_mc<no_leaf_resp_partitioning_growth_calculator>},
     {"thermal_time_senescence",                               &create_mc<thermal_time_senescence>},
     {"thermal_time_and_frost_senescence",                     &create_mc<thermal_time_and_frost_senescence>},
     {"aba_decay",                                             &create_mc<aba_decay>},
     {"ball_berry",                                            &create_mc<ball_berry>},
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
     {"partitioning_coefficient_logistic",                     &create_mc<partitioning_coefficient_logistic>},
     {"senescence_coefficient_logistic",                       &create_mc<senescence_coefficient_logistic>},
     {"senescence_logistic",                                   &create_mc<senescence_logistic>},
     {"development_index",                                     &create_mc<development_index>},
     {"soybean_development_rate_calculator",                   &create_mc<soybean_development_rate_calculator>},
     {"thermal_time_development_rate_calculator",              &create_mc<thermal_time_development_rate_calculator>},
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
     {"canopy_gbw_thornley",                                   &create_mc<canopy_gbw_thornley>},
     {"leaf_gbw_nikolov",                                      &create_mc<leaf_gbw_nikolov>},
     {"example_model_mass_gain",                               &create_mc<example_model_mass_gain>},
     {"example_model_partitioning",                            &create_mc<example_model_partitioning>},
     {"litter_cover",                                          &create_mc<litter_cover>},
     {"soil_sunlight",                                         &create_mc<soil_sunlight>},
     {"leaf_evapotranspiration",                               &create_mc<leaf_evapotranspiration>}
};
