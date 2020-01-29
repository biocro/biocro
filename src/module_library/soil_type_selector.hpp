#ifndef SOIL_TYPE_SELECTOR_H
#define SOIL_TYPE_SELECTOR_H

#include "../modules.h"

class soil_type_selector : public SteadyModule {
	public:
		soil_type_selector(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("soil_type_selector"),
			// Get pointers to input parameters
			soil_type_indicator_ip(get_ip(input_parameters, "soil_type_indicator")),
			// Get pointers to output parameters
			soil_silt_content_op(get_op(output_parameters, "soil_silt_content")),
			soil_clay_content_op(get_op(output_parameters, "soil_clay_content")),
			soil_sand_content_op(get_op(output_parameters, "soil_sand_content")),
			soil_air_entry_op(get_op(output_parameters, "soil_air_entry")),
			soil_b_coefficient_op(get_op(output_parameters, "soil_b_coefficient")),
			soil_saturated_conductivity_op(get_op(output_parameters, "soil_saturated_conductivity")),
			soil_saturation_capacity_op(get_op(output_parameters, "soil_saturation_capacity")),
			soil_field_capacity_op(get_op(output_parameters, "soil_field_capacity")),
			soil_wilting_point_op(get_op(output_parameters, "soil_wilting_point")),
			soil_bulk_density_op(get_op(output_parameters, "soil_bulk_density"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* soil_type_indicator_ip;
		// Pointers to output parameters
		double* soil_silt_content_op;
		double* soil_clay_content_op;
		double* soil_sand_content_op;
		double* soil_air_entry_op;
		double* soil_b_coefficient_op;
		double* soil_saturated_conductivity_op;
		double* soil_saturation_capacity_op;
		double* soil_field_capacity_op;
		double* soil_wilting_point_op;
		double* soil_bulk_density_op;		
		// Main operation
		void do_operation() const;
};

std::vector<std::string> soil_type_selector::get_inputs() {
	return {
		"soil_type_indicator"
	};
}

std::vector<std::string> soil_type_selector::get_outputs() {
	return {
        "soil_silt_content",
        "soil_clay_content",
        "soil_sand_content",
        "soil_air_entry",
        "soil_b_coefficient",
        "soil_saturated_conductivity",
        "soil_saturation_capacity",
        "soil_field_capacity",
        "soil_wilting_point",
        "soil_bulk_density"
	};
}

void soil_type_selector::do_operation() const {
	// Collect inputs and make calculations
	soilText_str soil_properties = get_soil_properties((int)(*soil_type_indicator_ip));	// get_soil_properties(int) is located in AuxBioCro.cpp
	
	// Update the output parameter list
	update(soil_silt_content_op,soil_properties.silt);
    update(soil_clay_content_op,soil_properties.clay);
    update(soil_sand_content_op,soil_properties.sand);
    update(soil_air_entry_op,soil_properties.air_entry);
    update(soil_b_coefficient_op,soil_properties.b);
    update(soil_saturated_conductivity_op,soil_properties.Ks);
    update(soil_saturation_capacity_op,soil_properties.satur);
    update(soil_field_capacity_op,soil_properties.fieldc);
    update(soil_wilting_point_op,soil_properties.wiltp);
    update(soil_bulk_density_op,soil_properties.bulk_density);
}

#endif


