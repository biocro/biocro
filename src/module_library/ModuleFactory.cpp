#include "ModuleFactory.h"

// Include all the m_XXXXXX.h header files that describe the modules
#include "harmonic_oscillator.hpp"	// Contains harmonic_oscillator and harmonic_energy
#include "size_testing.hpp"			// Contains P1, P10, P100, and P1000
#include "reaction.hpp"
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
#include "thermal_time_accumulator.hpp"
#include "utilization_growth.hpp"
#include "utilization_growth_calculator.hpp"
#include "utilization_senescence.hpp"
#include "utilization_senescence_calculator.hpp"
#include "leaf_water_stress_exponential.hpp"
#include "biomass_leaf_n_limitation.hpp"
#include "circadian_clock.hpp"
#include "circadian_clock_calculator.hpp"
#include "circadian_clock2.hpp"
#include "circadian_clock_calculator2.hpp"
#include "pokhilko_circadian_clock.hpp"
#include "song_flowering.hpp"
#include "fake_solar.hpp"
#include "clock_testing.hpp"
#include "clock_testing_calc.hpp"
#include "partitioning_coefficient_selector.hpp"
#include "partitioning_growth.hpp"
#include "partitioning_growth_calculator.hpp"
#include "no_leaf_resp_partitioning_growth_calculator.hpp"
#include "thermal_time_senescence.hpp"
#include "thermal_time_and_frost_senescence_calculator.hpp"
#include "thermal_time_and_frost_senescence.hpp"
#include "empty_senescence.hpp"
#include "aba_decay.hpp"
#include "ball_berry_module.hpp"
#include "water_vapor_properties_from_air_temperature.hpp"
#include "penman_monteith_transpiration.hpp"
#include "penman_monteith_leaf_temperature.hpp"
#include "priestley_transpiration.hpp"
#include "FvCB.hpp"
#include "gamma_oscillator.hpp"
#include "position_oscillator.hpp"
#include "velocity_oscillator.hpp"
#include "test_module.hpp"
#include "test_calc_state.hpp"
#include "test_derivs.hpp"
#include "bucket_soil_drainage.hpp"
#include "linear_vmax_from_leaf_n.hpp"
#include "module_graph_test.hpp"		// Includes Module_1, Module_2, and Module_3
#include "collatz_leaf.hpp"

ModuleFactory::ModuleFactory(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
	_input_parameters(input_parameters),
	_output_parameters(output_parameters)
{
	modules = {
		{"harmonic_oscillator",								createModule<harmonic_oscillator>},
		{"harmonic_energy",									createModule<harmonic_energy>},
		{"P1000",											createModule<P1000>},
		{"P100",											createModule<P100>},
		{"P10",												createModule<P10>},
		{"P1",												createModule<P1>},
		{"reaction",										createModule<reaction>},
		{"nr_ex",											createModule<nr_ex>},
		{"one_layer_soil_profile",							createModule<one_layer_soil_profile>},
		{"one_layer_soil_profile_derivatives",				createModule<one_layer_soil_profile_derivatives>},
		{"two_layer_soil_profile",							createModule<two_layer_soil_profile>},
		{"soil_type_selector",								createModule<soil_type_selector>},
		{"soil_evaporation",								createModule<soil_evaporation>},
		{"parameter_calculator",							createModule<parameter_calculator>},
		{"c3_canopy",										createModule<c3_canopy>},
		{"c4_canopy",										createModule<c4_canopy>},
		{"stomata_water_stress_linear",						createModule<stomata_water_stress_linear>},
		{"stomata_water_stress_exponential",				createModule<stomata_water_stress_exponential>},
		{"stomata_water_stress_linear_and_aba_response",	createModule<stomata_water_stress_linear_and_aba_response>},
		{"stomata_water_stress_sigmoid",					createModule<stomata_water_stress_sigmoid>},
		{"thermal_time_accumulator",						createModule<thermal_time_accumulator>},
		{"utilization_growth",								createModule<utilization_growth>},
		{"utilization_growth_calculator",					createModule<utilization_growth_calculator>},
		{"utilization_senescence",							createModule<utilization_senescence>},
		{"utilization_senescence_calculator",				createModule<utilization_senescence_calculator>},
		{"leaf_water_stress_exponential",					createModule<leaf_water_stress_exponential>},
		{"biomass_leaf_n_limitation",						createModule<biomass_leaf_n_limitation>},
		{"circadian_clock",									createModule<circadian_clock>},
		{"circadian_clock_calculator",						createModule<circadian_clock_calculator>},
		{"circadian_clock2",								createModule<circadian_clock2>},
		{"circadian_clock_calculator2",						createModule<circadian_clock_calculator2>},
		{"pokhilko_circadian_clock",						createModule<pokhilko_circadian_clock>},
		{"song_flowering",									createModule<song_flowering>},
		{"fake_solar",										createModule<fake_solar>},
		{"clock_testing",									createModule<clock_testing>},
		{"clock_testing_calc",								createModule<clock_testing_calc>},
		{"partitioning_coefficient_selector",				createModule<partitioning_coefficient_selector>},
		{"partitioning_growth",								createModule<partitioning_growth>},
		{"partitioning_growth_calculator",					createModule<partitioning_growth_calculator>},
		{"no_leaf_resp_partitioning_growth_calculator",		createModule<no_leaf_resp_partitioning_growth_calculator>},
		{"thermal_time_senescence",							createModule<thermal_time_senescence>},
		{"thermal_time_and_frost_senescence_calculator",	createModule<thermal_time_and_frost_senescence_calculator>},
		{"thermal_time_and_frost_senescence",				createModule<thermal_time_and_frost_senescence>},
		{"empty_senescence",								createModule<empty_senescence>},
		{"aba_decay",										createModule<aba_decay>},
		{"ball_berry_module",								createModule<ball_berry_module>},
		{"water_vapor_properties_from_air_temperature",		createModule<water_vapor_properties_from_air_temperature>},
		{"penman_monteith_transpiration",					createModule<penman_monteith_transpiration>},
		{"penman_monteith_leaf_temperature",				createModule<penman_monteith_leaf_temperature>},
		{"priestley_transpiration",							createModule<priestley_transpiration>},
		{"FvCB",											createModule<FvCB>},
		{"gamma_oscillator",								createModule<gamma_oscillator>},
		{"position_oscillator",								createModule<position_oscillator>},
		{"velocity_oscillator",								createModule<velocity_oscillator>},
		{"test_module",										createModule<test_module>},
		{"test_calc_state",									createModule<test_calc_state>},
		{"test_derivs",										createModule<test_derivs>},
		{"bucket_soil_drainage",							createModule<bucket_soil_drainage>},
		{"linear_vmax_from_leaf_n",							createModule<linear_vmax_from_leaf_n>},
		{"Module_1",										createModule<Module_1>},
		{"Module_2",										createModule<Module_2>},
		{"Module_3",										createModule<Module_3>},
		{"collatz_leaf",									createModule<collatz_leaf>}
	};
	input_parameter_names = {
		{"harmonic_oscillator",								harmonic_oscillator::get_inputs()},
		{"harmonic_energy",									harmonic_energy::get_inputs()},
		{"P1000",											P1000::get_inputs()},
		{"P100",											P100::get_inputs()},
		{"P10",												P10::get_inputs()},
		{"P1",												P1::get_inputs()},
		{"reaction",										reaction::get_inputs()},
		{"nr_ex",											nr_ex::get_inputs()},
		{"one_layer_soil_profile",							one_layer_soil_profile::get_inputs()},
		{"one_layer_soil_profile_derivatives",				one_layer_soil_profile_derivatives::get_inputs()},
		{"two_layer_soil_profile",							two_layer_soil_profile::get_inputs()},
		{"soil_type_selector",								soil_type_selector::get_inputs()},
		{"soil_evaporation",								soil_evaporation::get_inputs()},
		{"parameter_calculator",							parameter_calculator::get_inputs()},
		{"c3_canopy",										c3_canopy::get_inputs()},
		{"c4_canopy",										c4_canopy::get_inputs()},
		{"stomata_water_stress_linear",						stomata_water_stress_linear::get_inputs()},
		{"stomata_water_stress_exponential",				stomata_water_stress_exponential::get_inputs()},
		{"stomata_water_stress_linear_and_aba_response",	stomata_water_stress_linear_and_aba_response::get_inputs()},
		{"stomata_water_stress_sigmoid",					stomata_water_stress_sigmoid::get_inputs()},
		{"thermal_time_accumulator",						thermal_time_accumulator::get_inputs()},
		{"utilization_growth",								utilization_growth::get_inputs()},
		{"utilization_growth_calculator",					utilization_growth_calculator::get_inputs()},
		{"utilization_senescence",							utilization_senescence::get_inputs()},
		{"utilization_senescence_calculator",				utilization_senescence_calculator::get_inputs()},
		{"leaf_water_stress_exponential",					leaf_water_stress_exponential::get_inputs()},
		{"biomass_leaf_n_limitation",						biomass_leaf_n_limitation::get_inputs()},
		{"circadian_clock",									circadian_clock::get_inputs()},
		{"circadian_clock_calculator",						circadian_clock_calculator::get_inputs()},
		{"circadian_clock2",								circadian_clock2::get_inputs()},
		{"circadian_clock_calculator2",						circadian_clock_calculator2::get_inputs()},
		{"pokhilko_circadian_clock",						pokhilko_circadian_clock::get_inputs()},
		{"song_flowering",									song_flowering::get_inputs()},
		{"fake_solar",										fake_solar::get_inputs()},
		{"clock_testing",									clock_testing::get_inputs()},
		{"clock_testing_calc",								clock_testing_calc::get_inputs()},
		{"partitioning_coefficient_selector",				partitioning_coefficient_selector::get_inputs()},
		{"partitioning_growth",								partitioning_growth::get_inputs()},
		{"partitioning_growth_calculator",					partitioning_growth_calculator::get_inputs()},
		{"no_leaf_resp_partitioning_growth_calculator",		no_leaf_resp_partitioning_growth_calculator::get_inputs()},
		{"thermal_time_senescence",							thermal_time_senescence::get_inputs()},
		{"thermal_time_and_frost_senescence_calculator",	thermal_time_and_frost_senescence_calculator::get_inputs()},
		{"thermal_time_and_frost_senescence",				thermal_time_and_frost_senescence::get_inputs()},
		{"empty_senescence",								empty_senescence::get_inputs()},
		{"aba_decay",										aba_decay::get_inputs()},
		{"ball_berry_module",								ball_berry_module::get_inputs()},
		{"water_vapor_properties_from_air_temperature",		water_vapor_properties_from_air_temperature::get_inputs()},
		{"penman_monteith_transpiration",					penman_monteith_transpiration::get_inputs()},
		{"penman_monteith_leaf_temperature",				penman_monteith_leaf_temperature::get_inputs()},
		{"priestley_transpiration",							priestley_transpiration::get_inputs()},
		{"FvCB",											FvCB::get_inputs()},
		{"gamma_oscillator",								gamma_oscillator::get_inputs()},
		{"position_oscillator",								position_oscillator::get_inputs()},
		{"velocity_oscillator",								velocity_oscillator::get_inputs()},
		{"test_module",										test_module::get_inputs()},
		{"test_calc_state",									test_calc_state::get_inputs()},
		{"test_derivs",										test_derivs::get_inputs()},
		{"bucket_soil_drainage",							bucket_soil_drainage::get_inputs()},
		{"linear_vmax_from_leaf_n",							linear_vmax_from_leaf_n::get_inputs()},
		{"Module_1",										Module_1::get_inputs()},
		{"Module_2",										Module_2::get_inputs()},
		{"Module_3",										Module_3::get_inputs()},
		{"collatz_leaf",									collatz_leaf::get_inputs()}
	};
	output_parameter_names = {
		{"harmonic_oscillator",								harmonic_oscillator::get_outputs()},
		{"harmonic_energy",									harmonic_energy::get_outputs()},
		{"P1000",											P1000::get_outputs()},
		{"P100",											P100::get_outputs()},
		{"P10",												P10::get_outputs()},
		{"P1",												P1::get_outputs()},
		{"reaction",										reaction::get_outputs()},
		{"nr_ex",											nr_ex::get_outputs()},
		{"one_layer_soil_profile",							one_layer_soil_profile::get_outputs()},
		{"one_layer_soil_profile_derivatives",				one_layer_soil_profile_derivatives::get_outputs()},
		{"two_layer_soil_profile",							two_layer_soil_profile::get_outputs()},
		{"soil_type_selector",								soil_type_selector::get_outputs()},
		{"soil_evaporation",								soil_evaporation::get_outputs()},
		{"parameter_calculator",							parameter_calculator::get_outputs()},
		{"c3_canopy",										c3_canopy::get_outputs()},
		{"c4_canopy",										c4_canopy::get_outputs()},
		{"stomata_water_stress_linear",						stomata_water_stress_linear::get_outputs()},
		{"stomata_water_stress_exponential",				stomata_water_stress_exponential::get_outputs()},
		{"stomata_water_stress_linear_and_aba_response",	stomata_water_stress_linear_and_aba_response::get_outputs()},
		{"stomata_water_stress_sigmoid",					stomata_water_stress_sigmoid::get_outputs()},
		{"thermal_time_accumulator",						thermal_time_accumulator::get_outputs()},
		{"utilization_growth",								utilization_growth::get_outputs()},
		{"utilization_growth_calculator",					utilization_growth_calculator::get_outputs()},
		{"utilization_senescence",							utilization_senescence::get_outputs()},
		{"utilization_senescence_calculator",				utilization_senescence_calculator::get_outputs()},
		{"leaf_water_stress_exponential",					leaf_water_stress_exponential::get_outputs()},
		{"biomass_leaf_n_limitation",						biomass_leaf_n_limitation::get_outputs()},
		{"circadian_clock",									circadian_clock::get_outputs()},
		{"circadian_clock_calculator",						circadian_clock_calculator::get_outputs()},
		{"circadian_clock2",								circadian_clock2::get_outputs()},
		{"circadian_clock_calculator2",						circadian_clock_calculator2::get_outputs()},
		{"pokhilko_circadian_clock",						pokhilko_circadian_clock::get_outputs()},
		{"song_flowering",									song_flowering::get_outputs()},
		{"fake_solar",										fake_solar::get_outputs()},
		{"clock_testing",									clock_testing::get_outputs()},
		{"clock_testing_calc",								clock_testing_calc::get_outputs()},
		{"partitioning_coefficient_selector",				partitioning_coefficient_selector::get_outputs()},
		{"partitioning_growth",								partitioning_growth::get_outputs()},
		{"partitioning_growth_calculator",					partitioning_growth_calculator::get_outputs()},
		{"no_leaf_resp_partitioning_growth_calculator",		no_leaf_resp_partitioning_growth_calculator::get_outputs()},
		{"thermal_time_senescence",							thermal_time_senescence::get_outputs()},
		{"thermal_time_and_frost_senescence_calculator",	thermal_time_and_frost_senescence_calculator::get_outputs()},
		{"thermal_time_and_frost_senescence",				thermal_time_and_frost_senescence::get_outputs()},
		{"empty_senescence",								empty_senescence::get_outputs()},
		{"aba_decay",										aba_decay::get_outputs()},
		{"ball_berry_module",								ball_berry_module::get_outputs()},
		{"water_vapor_properties_from_air_temperature",		water_vapor_properties_from_air_temperature::get_outputs()},
		{"penman_monteith_transpiration",					penman_monteith_transpiration::get_outputs()},
		{"penman_monteith_leaf_temperature",				penman_monteith_leaf_temperature::get_outputs()},
		{"priestley_transpiration",							priestley_transpiration::get_outputs()},
		{"FvCB",											FvCB::get_outputs()},
		{"gamma_oscillator",								gamma_oscillator::get_outputs()},
		{"position_oscillator",								position_oscillator::get_outputs()},
		{"velocity_oscillator",								velocity_oscillator::get_outputs()},
		{"test_module",										test_module::get_outputs()},
		{"test_calc_state",									test_calc_state::get_outputs()},
		{"test_derivs",										test_derivs::get_outputs()},
		{"bucket_soil_drainage",							bucket_soil_drainage::get_outputs()},
		{"linear_vmax_from_leaf_n",							linear_vmax_from_leaf_n::get_outputs()},
		{"Module_1",										Module_1::get_outputs()},
		{"Module_2",										Module_2::get_outputs()},
		{"Module_3",										Module_3::get_outputs()},
		{"collatz_leaf",									collatz_leaf::get_outputs()}
	};
}

std::unique_ptr<Module> ModuleFactory::create(std::string const &module_name) const {
	try {
		return this->modules.at(module_name)(_input_parameters, _output_parameters);
	}
	catch (std::out_of_range) {
		throw std::out_of_range(std::string("'") + module_name + std::string("'") + std::string(" was given as a module name, but the ModuleFactory::create could not find a module with that name.\n"));
	}
	catch (std::logic_error const &e) {
		throw std::logic_error(std::string("Upon construction, module '") + module_name + std::string("'") + std::string(" tried to access a parameter called '") + std::string(e.what()) + std::string("', but this parameter was not in the list. Check for spelling errors!\n"));
	}
}

std::vector<std::string> ModuleFactory::get_inputs(std::string const &module_name) const {
	try {
		return this->input_parameter_names.at(module_name);
	}
	catch (std::out_of_range) {
		throw std::out_of_range(std::string("'") + module_name + std::string("'") + std::string(" was given as a module name, but ModuleFactory::get_inputs could not find a module with that name.\n"));
	}
}

std::vector<std::string> ModuleFactory::get_outputs(std::string const &module_name) const {
	try {
		return this->output_parameter_names.at(module_name);
	}
	catch (std::out_of_range) {
		throw std::out_of_range(std::string("'") + module_name + std::string("'") + std::string(" was given as a module name, but ModuleFactory::get_outputs could not find a module with that name.\n"));
	}
}
