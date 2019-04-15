#include "ModuleFactory.h"
#include "../big_leaf_multilayer_canopy.hpp"
#include "biomass_leaf_n_limitation.hpp"
#include "parameter_calculator.hpp"
#include "test_derivs.hpp"
#include "test_calc_state.hpp"
#include "position_oscillator.hpp"
#include "velocity_oscillator.hpp"
#include "gamma_oscillator.hpp"
#include "c4_canopy.hpp"
#include "c3_canopy.hpp"
#include "FvCB.hpp"
#include "ball_berry_module.hpp"
#include "one_layer_soil_profile.hpp"
#include "two_layer_soil_profile.hpp"
#include "water_vapor_properties_from_air_temperature.hpp"
#include "thermal_time_senescence.hpp"
#include "thermal_time_and_frost_senescence.hpp"
#include "utilization_senescence.hpp"
#include "utilization_growth_module.hpp"
#include "empty_senescence.hpp"
#include "utilization_growth_flowering.hpp"
#include "penman_monteith_leaf_temperature.hpp"

ModuleFactory::module_map ModuleFactory::modules = {
                { "velocity_oscillator",                &createModule<velocity_oscillator>},
                { "position_oscillator",                &createModule<position_oscillator>},
                { "gamma_oscillator",                   &createModule<gamma_oscillator>},
                { "c4_canopy",                          &createModule<c4_canopy>},
                { "c3_canopy",                          &createModule<c3_canopy>},
                { "one_layer_soil_profile",             &createModule<one_layer_soil_profile>},
                { "two_layer_soil_profile",             &createModule<two_layer_soil_profile>},
                { "partitioning_growth",                &createModule<partitioning_growth_module>},
                { "no_leaf_resp_partitioning_growth",   &createModule<no_leaf_resp_partitioning_growth_module>},
                { "utilization_growth_and_senescence",  &createModule<utilization_growth_and_senescence_module>},
                { "utilization_growth",                 &createModule<utilization_growth_module>},
                { "utilization_growth_flowering",       &createModule<utilization_growth_flowering>},
                { "utilization_senescence",             &createModule<utilization_senescence>},
                { "empty_senescence",                   &createModule<empty_senescence>},
                { "thermal_time_and_frost_senescence",  &createModule<thermal_time_and_frost_senescence>},
                { "thermal_time_senescence",            &createModule<thermal_time_senescence>},
                { "one_layer_soil_profile_derivatives", &createModule<one_layer_soil_profile_derivatives>},
                { "soil_type_selector",                 &createModule<soil_type_selector>},
                { "test_derivs",                        &createModule<test_derivs>},
                { "test_calc_state",                    &createModule<test_calc_state>},
                { "parameter_calculator",               &createModule<parameter_calculator>},
                { "thermal_time_accumulator",           &createModule<thermal_time_accumulator>},
                { "stomata_water_stress_linear",        &createModule<stomata_water_stress_linear>},
                { "stomata_water_stress_sigmoid",       &createModule<stomata_water_stress_sigmoid>},
                { "stomata_water_stress_exponential",   &createModule<stomata_water_stress_exponential>},
                { "leaf_water_stress_exponential",      &createModule<leaf_water_stress_exponential>},
                { "biomass_leaf_n_limitation",          &createModule<biomass_leaf_n_limitation>},
                { "bucket_soil_drainage",               &createModule<bucket_soil_drainage>},
                { "soil_evaporation",                   &createModule<soil_evaporation>},
                { "partitioning_coefficient_selector",  &createModule<partitioning_coefficient_selector>},
                { "collatz_leaf",                       &createModule<collatz_leaf>},
                { "water_vapor_properties_from_air_temperature",  &createModule<water_vapor_properties_from_air_temperature>},
                { "big_leaf_multilayer_canopy",         &createModule<big_leaf_multilayer_canopy>},
            };

std::unique_ptr<IModule> ModuleFactory::operator()(std::string const &module_name) const {
            try {
                return this->modules.at(module_name)();
            } catch (std::out_of_range const &oor) {
                throw std::out_of_range(std::string("\"") + module_name + std::string("\"") + std::string(" was given as a module name, but no module with that name could be found.\n"));
            }
        }

ModuleFactory module_factory;

