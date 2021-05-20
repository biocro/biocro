#ifndef UTILIZATION_SENESCENCE_H
#define UTILIZATION_SENESCENCE_H

#include "../modules.h"
#include "../state_map.h"

class utilization_senescence : public DerivModule {
	public:
		utilization_senescence(const state_map* input_quantities, state_map* output_quantities) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("utilization_senescence"),
			// Get pointers to input quantities
		    remobilization_fraction_ip(get_ip(input_quantities, "remobilization_fraction")),
	        senescence_leaf_ip(get_ip(input_quantities, "senescence_leaf")),
	        senescence_stem_ip(get_ip(input_quantities, "senescence_stem")),
	        senescence_root_ip(get_ip(input_quantities, "senescence_root")),
	        senescence_rhizome_ip(get_ip(input_quantities, "senescence_rhizome")),
			// Get pointers to output quantities
			Leaf_op(get_op(output_quantities, "Leaf")),
	        substrate_pool_leaf_op(get_op(output_quantities, "substrate_pool_leaf")),
	        LeafLitter_op(get_op(output_quantities, "LeafLitter")),
	        Stem_op(get_op(output_quantities, "Stem")),
	        substrate_pool_stem_op(get_op(output_quantities, "substrate_pool_stem")),
	        StemLitter_op(get_op(output_quantities, "StemLitter")),
	        Root_op(get_op(output_quantities, "Root")),
	        substrate_pool_root_op(get_op(output_quantities, "substrate_pool_root")),
	        RootLitter_op(get_op(output_quantities, "RootLitter")),
	        Rhizome_op(get_op(output_quantities, "Rhizome")),
	        substrate_pool_rhizome_op(get_op(output_quantities, "substrate_pool_rhizome")),
	        RhizomeLitter_op(get_op(output_quantities, "RhizomeLitter"))
		{}
		static string_vector get_inputs();
		static string_vector get_outputs();
	private:
		// Pointers to input quantities
	    const double* remobilization_fraction_ip;
        const double* senescence_leaf_ip;
        const double* senescence_stem_ip;
        const double* senescence_root_ip;
        const double* senescence_rhizome_ip;
		// Pointers to output quantities
		double* Leaf_op;
        double* substrate_pool_leaf_op;
        double* LeafLitter_op;
        double* Stem_op;
        double* substrate_pool_stem_op;
        double* StemLitter_op;
        double* Root_op;
        double* substrate_pool_root_op;
        double* RootLitter_op;
        double* Rhizome_op;
        double* substrate_pool_rhizome_op;
        double* RhizomeLitter_op;
		// Main operation
		void do_operation() const;
};

string_vector utilization_senescence::get_inputs() {
	return {
	    "remobilization_fraction",
	    "senescence_leaf",
	    "senescence_stem",
	    "senescence_root",
	    "senescence_rhizome"
	};
}

string_vector utilization_senescence::get_outputs() {
	return {
		"Leaf",
        "substrate_pool_leaf",
        "LeafLitter",
        "Stem",
        "substrate_pool_stem",
        "StemLitter",
        "Root",
        "substrate_pool_root",
        "RootLitter",
        "Rhizome",
        "substrate_pool_rhizome",
        "RhizomeLitter"
	};
}

void utilization_senescence::do_operation() const {
	// Collect inputs and make calculations
    double remobilization_fraction = *remobilization_fraction_ip;

    double senescence_leaf = *senescence_leaf_ip;
    double senescence_stem = *senescence_stem_ip;
    double senescence_root = *senescence_root_ip;
    double senescence_rhizome = *senescence_rhizome_ip;

	// Update the output quantity list
	update(Leaf_op, -senescence_leaf);
    update(substrate_pool_leaf_op, senescence_leaf * remobilization_fraction);
    update(LeafLitter_op, senescence_leaf * (1 - remobilization_fraction));

    update(Stem_op, -senescence_stem);
    update(substrate_pool_stem_op, senescence_stem * remobilization_fraction);
    update(StemLitter_op, senescence_stem * (1 - remobilization_fraction));

    update(Root_op, -senescence_root);
    update(substrate_pool_root_op, senescence_root * remobilization_fraction);
    update(RootLitter_op, senescence_root * (1 - remobilization_fraction));

    update(Rhizome_op, -senescence_rhizome);
    update(substrate_pool_rhizome_op, senescence_rhizome * remobilization_fraction);
    update(RhizomeLitter_op, senescence_rhizome * (1 - remobilization_fraction));
}

#endif
