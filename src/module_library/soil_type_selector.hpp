#ifndef SOIL_TYPE_SELECTOR_H
#define SOIL_TYPE_SELECTOR_H

#include "../modules.h"
#include "../state_map.h"
#include "AuxBioCro.h"  // For soilText_str and get_soil_properties

class soil_type_selector : public direct_module {
	public:
		soil_type_selector(state_map const& input_quantities, state_map* output_quantities) :
			// Define basic module properties by passing its name to its parent class
			direct_module("soil_type_selector"),
			// Get pointers to input quantities
			soil_type_indicator_ip(get_ip(input_quantities, "soil_type_indicator")),
			// Get pointers to output quantities
			soil_silt_content_op(get_op(output_quantities, "soil_silt_content")),
			soil_clay_content_op(get_op(output_quantities, "soil_clay_content")),
			soil_sand_content_op(get_op(output_quantities, "soil_sand_content")),
			soil_air_entry_op(get_op(output_quantities, "soil_air_entry")),
			soil_b_coefficient_op(get_op(output_quantities, "soil_b_coefficient")),
			soil_saturated_conductivity_op(get_op(output_quantities, "soil_saturated_conductivity")),
			soil_saturation_capacity_op(get_op(output_quantities, "soil_saturation_capacity")),
			soil_field_capacity_op(get_op(output_quantities, "soil_field_capacity")),
			soil_wilting_point_op(get_op(output_quantities, "soil_wilting_point")),
			soil_bulk_density_op(get_op(output_quantities, "soil_bulk_density"))
		{}
		static string_vector get_inputs();
		static string_vector get_outputs();
	private:
		// Pointers to input quantities
		const double* soil_type_indicator_ip;
		// Pointers to output quantities
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

string_vector soil_type_selector::get_inputs() {
	return {
		"soil_type_indicator"
	};
}

string_vector soil_type_selector::get_outputs() {
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

	// Update the output quantity list
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


