#ifndef PARTITIONING_COEFFICIENT_SELECTOR_H
#define PARTITIONING_COEFFICIENT_SELECTOR_H

#include "../modules.h"

class partitioning_coefficient_selector : public SteadyModule {
	public:
		partitioning_coefficient_selector(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("partitioning_coefficient_selector"),
			// Get pointers to input parameters
			kStem1_ip(get_ip(input_parameters, "kStem1")),
			kLeaf1_ip(get_ip(input_parameters, "kLeaf1")),
			kRoot1_ip(get_ip(input_parameters, "kRoot1")),
			kRhizome1_ip(get_ip(input_parameters, "kRhizome1")),
			kGrain1_ip(get_ip(input_parameters, "kGrain1")),
			tp1_ip(get_ip(input_parameters, "tp1")),
			kStem2_ip(get_ip(input_parameters, "kStem2")),
			kLeaf2_ip(get_ip(input_parameters, "kLeaf2")),
			kRoot2_ip(get_ip(input_parameters, "kRoot2")),
			kRhizome2_ip(get_ip(input_parameters, "kRhizome2")),
			kGrain2_ip(get_ip(input_parameters, "kGrain2")),
			tp2_ip(get_ip(input_parameters, "tp2")),
			kStem3_ip(get_ip(input_parameters, "kStem3")),
			kLeaf3_ip(get_ip(input_parameters, "kLeaf3")),
			kRoot3_ip(get_ip(input_parameters, "kRoot3")),
			kRhizome3_ip(get_ip(input_parameters, "kRhizome3")),
			kGrain3_ip(get_ip(input_parameters, "kGrain3")),
			tp3_ip(get_ip(input_parameters, "tp3")),
			kStem4_ip(get_ip(input_parameters, "kStem4")),
			kLeaf4_ip(get_ip(input_parameters, "kLeaf4")),
			kRoot4_ip(get_ip(input_parameters, "kRoot4")),
			kRhizome4_ip(get_ip(input_parameters, "kRhizome4")),
			kGrain4_ip(get_ip(input_parameters, "kGrain4")),
			tp4_ip(get_ip(input_parameters, "tp4")),
			kStem5_ip(get_ip(input_parameters, "kStem5")),
			kLeaf5_ip(get_ip(input_parameters, "kLeaf5")),
			kRoot5_ip(get_ip(input_parameters, "kRoot5")),
			kRhizome5_ip(get_ip(input_parameters, "kRhizome5")),
			kGrain5_ip(get_ip(input_parameters, "kGrain5")),
			tp5_ip(get_ip(input_parameters, "tp5")),
			kStem6_ip(get_ip(input_parameters, "kStem6")),
			kLeaf6_ip(get_ip(input_parameters, "kLeaf6")),
			kRoot6_ip(get_ip(input_parameters, "kRoot6")),
			kRhizome6_ip(get_ip(input_parameters, "kRhizome6")),
			kGrain6_ip(get_ip(input_parameters, "kGrain6")),
			TTc_ip(get_ip(input_parameters, "TTc")),
			// Get pointers to output parameters
			kStem_op(get_op(output_parameters, "kStem")),
			kLeaf_op(get_op(output_parameters, "kLeaf")),
			kRoot_op(get_op(output_parameters, "kRoot")),
			kRhizome_op(get_op(output_parameters, "kRhizome")),
			kGrain_op(get_op(output_parameters, "kGrain"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* kStem1_ip;
		const double* kLeaf1_ip;
		const double* kRoot1_ip;
		const double* kRhizome1_ip;
		const double* kGrain1_ip;
		const double* tp1_ip;
		const double* kStem2_ip;
		const double* kLeaf2_ip;
		const double* kRoot2_ip;
		const double* kRhizome2_ip;
		const double* kGrain2_ip;
		const double* tp2_ip;
		const double* kStem3_ip;
		const double* kLeaf3_ip;
		const double* kRoot3_ip;
		const double* kRhizome3_ip;
		const double* kGrain3_ip;
		const double* tp3_ip;
		const double* kStem4_ip;
		const double* kLeaf4_ip;
		const double* kRoot4_ip;
		const double* kRhizome4_ip;
		const double* kGrain4_ip;
		const double* tp4_ip;
		const double* kStem5_ip;
		const double* kLeaf5_ip;
		const double* kRoot5_ip;
		const double* kRhizome5_ip;
		const double* kGrain5_ip;
		const double* tp5_ip;
		const double* kStem6_ip;
		const double* kLeaf6_ip;
		const double* kRoot6_ip;
		const double* kRhizome6_ip;
		const double* kGrain6_ip;
		const double* TTc_ip;
		// Pointers to output parameters
		double* kStem_op;
		double* kLeaf_op;
		double* kRoot_op;
		double* kRhizome_op;
		double* kGrain_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> partitioning_coefficient_selector::get_inputs() {
	return {
		"kStem1", "kLeaf1", "kRoot1", "kRhizome1", "kGrain1", "tp1",
		"kStem2", "kLeaf2", "kRoot2", "kRhizome2", "kGrain2", "tp2",
		"kStem3", "kLeaf3", "kRoot3", "kRhizome3", "kGrain3", "tp3",
		"kStem4", "kLeaf4", "kRoot4", "kRhizome4", "kGrain4", "tp4",
		"kStem5", "kLeaf5", "kRoot5", "kRhizome5", "kGrain5", "tp5",
		"kStem6", "kLeaf6", "kRoot6", "kRhizome6", "kGrain6",
		"TTc"
	};
}

std::vector<std::string> partitioning_coefficient_selector::get_outputs() {
	return {
		"kStem", "kLeaf", "kRoot", "kRhizome", "kGrain"
	};
}

void partitioning_coefficient_selector::do_operation() const {
	double TTc = *TTc_ip;
	double tp1 = *tp1_ip;
	double tp2 = *tp2_ip;
	double tp3 = *tp3_ip;
	double tp4 = *tp4_ip;
	double tp5 = *tp5_ip;
	double kStem, kLeaf, kRoot, kRhizome, kGrain;
	
	// Determine the coefficients based on the current TTc value
	if(0.0 <= TTc && TTc < tp1) {
		// TTc is in interval 1
		kStem = *kStem1_ip;
		kLeaf = *kLeaf1_ip;
		kRoot = *kRoot1_ip;
		kRhizome = *kRhizome1_ip;
		kGrain = *kGrain1_ip;
	}
	else if(tp1 <= TTc && TTc < tp2) {
		// TTc is in interval 2
		kStem = *kStem2_ip;
		kLeaf = *kLeaf2_ip;
		kRoot = *kRoot2_ip;
		kRhizome = *kRhizome2_ip;
		kGrain = *kGrain2_ip;
	}
	else if(tp2 <= TTc && TTc < tp3) {
		// TTc is in interval 3
		kStem = *kStem3_ip;
		kLeaf = *kLeaf3_ip;
		kRoot = *kRoot3_ip;
		kRhizome = *kRhizome3_ip;
		kGrain = *kGrain3_ip;
	}
	else if(tp3 <= TTc && TTc < tp4) {
		// TTc is in interval 4
		kStem = *kStem4_ip;
		kLeaf = *kLeaf4_ip;
		kRoot = *kRoot4_ip;
		kRhizome = *kRhizome4_ip;
		kGrain = *kGrain4_ip;
	}
	else if(tp4 <= TTc && TTc < tp5) {
		// TTc is in interval 5
		kStem = *kStem5_ip;
		kLeaf = *kLeaf5_ip;
		kRoot = *kRoot5_ip;
		kRhizome = *kRhizome5_ip;
		kGrain = *kGrain5_ip;
	}
	else if(tp5 <= TTc) {
		// TTc is in interval 6
		kStem = *kStem6_ip;
		kLeaf = *kLeaf6_ip;
		kRoot = *kRoot6_ip;
		kRhizome = *kRhizome6_ip;
		kGrain = *kGrain6_ip;
	}
	
	// Update the output parameters
	update(kStem_op, kStem);
	update(kLeaf_op, kLeaf);
	update(kRoot_op, kRoot);
	update(kRhizome_op, kRhizome);
	update(kGrain_op, kGrain);
}

#endif
