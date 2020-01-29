#ifndef UTILIZATION_GROWTH_H
#define UTILIZATION_GROWTH_H

#include "../modules.h"

class utilization_growth : public DerivModule {
	public:
		utilization_growth(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("utilization_growth"),
			// Get pointers to input parameters
			canopy_assimilation_rate_ip(get_ip(input_parameters, "canopy_assimilation_rate")),
			utilization_leaf_ip(get_ip(input_parameters, "utilization_leaf")),
			utilization_stem_ip(get_ip(input_parameters, "utilization_stem")),
			utilization_root_ip(get_ip(input_parameters, "utilization_root")),
			utilization_rhizome_ip(get_ip(input_parameters, "utilization_rhizome")),
			utilization_grain_ip(get_ip(input_parameters, "utilization_grain")),
			transport_leaf_to_stem_ip(get_ip(input_parameters, "transport_leaf_to_stem")),
			transport_stem_to_grain_ip(get_ip(input_parameters, "transport_stem_to_grain")),
			transport_stem_to_root_ip(get_ip(input_parameters, "transport_stem_to_root")),
			transport_stem_to_rhizome_ip(get_ip(input_parameters, "transport_stem_to_rhizome")),
			// Get pointers to output parameters
			Leaf_op(get_op(output_parameters, "Leaf")),
			substrate_pool_leaf_op(get_op(output_parameters, "substrate_pool_leaf")),
			Stem_op(get_op(output_parameters, "Stem")),
			substrate_pool_stem_op(get_op(output_parameters, "substrate_pool_stem")),
			Grain_op(get_op(output_parameters, "Grain")),
			substrate_pool_grain_op(get_op(output_parameters, "substrate_pool_grain")),
			Root_op(get_op(output_parameters, "Root")),
			substrate_pool_root_op(get_op(output_parameters, "substrate_pool_root")),
			Rhizome_op(get_op(output_parameters, "Rhizome")),
			substrate_pool_rhizome_op(get_op(output_parameters, "substrate_pool_rhizome"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
		static std::string get_description();
	private:
		// Pointers to input parameters
		const double* canopy_assimilation_rate_ip;
		const double* utilization_leaf_ip;
		const double* utilization_stem_ip;
		const double* utilization_root_ip;
		const double* utilization_rhizome_ip;
		const double* utilization_grain_ip;
		const double* transport_leaf_to_stem_ip;
		const double* transport_stem_to_grain_ip;
		const double* transport_stem_to_root_ip;
		const double* transport_stem_to_rhizome_ip;
		// Pointers to output parameters
		double* Leaf_op;
		double* substrate_pool_leaf_op;
		double* Stem_op;
		double* substrate_pool_stem_op;
		double* Grain_op;
		double* substrate_pool_grain_op;
		double* Root_op;
		double* substrate_pool_root_op;
		double* Rhizome_op;
		double* substrate_pool_rhizome_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> utilization_growth::get_inputs() {
	return {
		"canopy_assimilation_rate",
	    "utilization_leaf",
	    "utilization_stem",
	    "utilization_grain",
	    "utilization_root",
	    "utilization_rhizome",
	    "transport_leaf_to_stem",
	    "transport_stem_to_grain",
	    "transport_stem_to_root",
	    "transport_stem_to_rhizome"
	};
}

std::vector<std::string> utilization_growth::get_outputs() {
	return {
		"Leaf",
		"substrate_pool_leaf",
		"Stem",
		"substrate_pool_stem",
		"Grain",
		"substrate_pool_grain",
		"Root",
		"substrate_pool_root",
		"Rhizome",
		"substrate_pool_rhizome"
	};
}

std::string utilization_growth::get_description() {
	std::string description = std::string("  The utilization growth model is based on several papers published by Thornley and is implemented in BioCro via two separate modules.\n");
	description += std::string("  This module returns derivatives for the plant tissue mass state variables.\n");
	description += std::string("  It is intended to be run along with the utilization_growth_calculator module.\n");
	description += std::string("  See utilization_growth.hpp and utilization_growth_calculator.hpp for more information.");
	return description;
}

void utilization_growth::do_operation() const {
	// Collect inputs and make calculations	
	double canopy_assimilation_rate = *canopy_assimilation_rate_ip;
	
    double utilization_leaf = *utilization_leaf_ip;
    double utilization_stem = *utilization_stem_ip;
    double utilization_grain = *utilization_grain_ip;
    double utilization_root = *utilization_root_ip;
    double utilization_rhizome = *utilization_rhizome_ip;
    
    double transport_leaf_to_stem = *transport_leaf_to_stem_ip;
    double transport_stem_to_grain = *transport_stem_to_grain_ip;
    double transport_stem_to_root = *transport_stem_to_root_ip;
    double transport_stem_to_rhizome = *transport_stem_to_rhizome_ip;
    
    // The rate of change of the carbon substrate pool in each tissue is the sum of its inputs and outputs
    //  The leaf gets carbon from canopy photosynthesis (a conceptual rather than physical distinction), uses some for growth, and transfers some to the stem
    //  The stem gets carbon from the leaf, uses some for growth, and transfers some to the root, rhizome, and grain
    //  The root, rhizome, and grain each get carbon from the stem and use some for growth
    // See Thornley, J. H. M. A Model to Describe the Partitioning of Photosynthate during Vegetative Plant Growth. Ann Bot 36, 419430 (1972)

    double d_substrate_leaf = canopy_assimilation_rate - transport_leaf_to_stem - utilization_leaf;
    double d_substrate_stem = transport_leaf_to_stem - transport_stem_to_grain - transport_stem_to_root - transport_stem_to_rhizome - utilization_stem;
    double d_substrate_grain = transport_stem_to_grain - utilization_grain;
    double d_substrate_root = transport_stem_to_root - utilization_root;
    double d_substrate_rhizome = transport_stem_to_rhizome - utilization_rhizome;
    
    // In Thornley's model, the utilized carbon substrate is split into two parts:
    //  (1) A fraction YG is used for growth (0 < YG < 1)
    //  (2) The remaining fraction (1 - YG) is lost to respiration
    // In BioCro, the canopy assimilation rate (used here as the carbon input) already includes a negative
	//  respiratory contribution based on Bernacchi et al., Plant, Cell & Environment 24, 253259 (2001)
	// So we simply set YG = 1 when allocating the substrate for growth, i.e.,
	//  the rate of change of Leaf is utilization_leaf, etc
	
	// Update the output parameter list
    update(Leaf_op, utilization_leaf);
    update(substrate_pool_leaf_op, d_substrate_leaf);

    update(Stem_op, utilization_stem);
    update(substrate_pool_stem_op, d_substrate_stem);

    update(Grain_op, utilization_grain);
    update(substrate_pool_grain_op, d_substrate_grain);

    update(Root_op, utilization_root);
    update(substrate_pool_root_op, d_substrate_root);

    update(Rhizome_op, utilization_rhizome);
    update(substrate_pool_rhizome_op, d_substrate_rhizome);
}

#endif
