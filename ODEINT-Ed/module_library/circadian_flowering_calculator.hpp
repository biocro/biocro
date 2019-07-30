#ifndef CIRCADIAN_FLOWERING_CALCULATOR_H
#define CIRCADIAN_FLOWERING_CALCULATOR_H

#include "../module.hpp"

class circadian_flowering_calculator : public SteadyModule {
	public:
		circadian_flowering_calculator(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("circadian_flowering_calculator"),
			// Get pointers to input parameters
			grain_TTc_ip(get_ip(input_parameters, "grain_TTc")),
			TTc_ip(get_ip(input_parameters, "TTc")),
			rate_constant_leaf_ip(get_ip(input_parameters, "rate_constant_leaf")),
			rate_constant_stem_ip(get_ip(input_parameters, "rate_constant_stem")),
			rate_constant_root_ip(get_ip(input_parameters, "rate_constant_root")),
			rate_constant_rhizome_ip(get_ip(input_parameters, "rate_constant_rhizome")),
			rate_constant_grain_ip(get_ip(input_parameters, "rate_constant_grain")),
			KmLeaf_ip(get_ip(input_parameters, "KmLeaf")),
			KmStem_ip(get_ip(input_parameters, "KmStem")),
			KmRoot_ip(get_ip(input_parameters, "KmRoot")),
			KmRhizome_ip(get_ip(input_parameters, "KmRhizome")),
			KmGrain_ip(get_ip(input_parameters, "KmGrain")),
			resistance_leaf_to_stem_ip(get_ip(input_parameters, "resistance_leaf_to_stem")),
			resistance_stem_to_grain_ip(get_ip(input_parameters, "resistance_stem_to_grain")),
			resistance_stem_to_root_ip(get_ip(input_parameters, "resistance_stem_to_root")),
			resistance_stem_to_rhizome_ip(get_ip(input_parameters, "resistance_stem_to_rhizome")),
			Leaf_ip(get_ip(input_parameters, "Leaf")),
			Stem_ip(get_ip(input_parameters, "Stem")),
			Root_ip(get_ip(input_parameters, "Root")),
			Grain_ip(get_ip(input_parameters, "Grain")),
			Rhizome_ip(get_ip(input_parameters, "Rhizome")),
			substrate_pool_leaf_ip(get_ip(input_parameters, "substrate_pool_leaf")),
			substrate_pool_stem_ip(get_ip(input_parameters, "substrate_pool_stem")),
			substrate_pool_root_ip(get_ip(input_parameters, "substrate_pool_root")),
			substrate_pool_rhizome_ip(get_ip(input_parameters, "substrate_pool_rhizome")),
			substrate_pool_grain_ip(get_ip(input_parameters, "substrate_pool_grain")),
			// Get pointers to output parameters
			kGrain_scale_op(get_op(output_parameters, "kGrain_scale")),
			utilization_leaf_op(get_op(output_parameters, "utilization_leaf")),
			utilization_stem_op(get_op(output_parameters, "utilization_stem")),
			utilization_root_op(get_op(output_parameters, "utilization_root")),
			utilization_rhizome_op(get_op(output_parameters, "utilization_rhizome")),
			utilization_grain_op(get_op(output_parameters, "utilization_grain")),
			transport_leaf_to_stem_op(get_op(output_parameters, "transport_leaf_to_stem")),
			transport_stem_to_grain_op(get_op(output_parameters, "transport_stem_to_grain")),
			transport_stem_to_root_op(get_op(output_parameters, "transport_stem_to_root")),
			transport_stem_to_rhizome_op(get_op(output_parameters, "transport_stem_to_rhizome")),
			mass_fraction_leaf_op(get_op(output_parameters, "mass_fraction_leaf")),
			mass_fraction_stem_op(get_op(output_parameters, "mass_fraction_stem")),
			mass_fraction_root_op(get_op(output_parameters, "mass_fraction_root")),
			mass_fraction_rhizome_op(get_op(output_parameters, "mass_fraction_rhizome")),
			mass_fraction_grain_op(get_op(output_parameters, "mass_fraction_grain"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* grain_TTc_ip;
		const double* TTc_ip;
		const double* rate_constant_leaf_ip;
		const double* rate_constant_stem_ip;
		const double* rate_constant_root_ip;
		const double* rate_constant_rhizome_ip;
		const double* rate_constant_grain_ip;
		const double* KmLeaf_ip;
		const double* KmStem_ip;
		const double* KmRoot_ip;
		const double* KmRhizome_ip;
		const double* KmGrain_ip;
		const double* resistance_leaf_to_stem_ip;
		const double* resistance_stem_to_grain_ip;
		const double* resistance_stem_to_root_ip;
		const double* resistance_stem_to_rhizome_ip;
		const double* Leaf_ip;
		const double* Stem_ip;
		const double* Root_ip;
		const double* Grain_ip;
		const double* Rhizome_ip;
		const double* substrate_pool_leaf_ip;
		const double* substrate_pool_stem_ip;
		const double* substrate_pool_root_ip;
		const double* substrate_pool_rhizome_ip;
		const double* substrate_pool_grain_ip;
		// Pointers to output parameters
		double* kGrain_scale_op;
		double* utilization_leaf_op;
		double* utilization_stem_op;
		double* utilization_root_op;
		double* utilization_rhizome_op;
		double* utilization_grain_op;
		double* transport_leaf_to_stem_op;
		double* transport_stem_to_grain_op;
		double* transport_stem_to_root_op;
		double* transport_stem_to_rhizome_op;
		double* mass_fraction_leaf_op;
		double* mass_fraction_stem_op;
		double* mass_fraction_root_op;
		double* mass_fraction_rhizome_op;
		double* mass_fraction_grain_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> circadian_flowering_calculator::get_inputs() {
	return {
		"grain_TTc",
		"TTc",
		"rate_constant_leaf",
		"rate_constant_stem",
		"rate_constant_root",
		"rate_constant_rhizome",
		"rate_constant_grain",
		"KmLeaf",
		"KmStem",
		"KmRoot",
		"KmRhizome",
		"KmGrain",
		"resistance_leaf_to_stem",
		"resistance_stem_to_grain",
		"resistance_stem_to_root",
		"resistance_stem_to_rhizome",
		"Leaf",
		"Stem",
		"Root",
		"Grain",
		"Rhizome",
		"substrate_pool_leaf",
		"substrate_pool_stem",
		"substrate_pool_root",
		"substrate_pool_rhizome",
		"substrate_pool_grain"
	};
}

std::vector<std::string> circadian_flowering_calculator::get_outputs() {
	return {
		"kGrain_scale",
		"utilization_leaf",
		"utilization_stem",
		"utilization_root",
		"utilization_rhizome",
		"utilization_grain",
		"transport_leaf_to_stem",
		"transport_stem_to_grain",
		"transport_stem_to_root",
		"transport_stem_to_rhizome",
		"mass_fraction_leaf",
		"mass_fraction_stem",
		"mass_fraction_root",
		"mass_fraction_rhizome",
		"mass_fraction_grain"
	};
}

void circadian_flowering_calculator::do_operation() const {	
	// Collect inputs and make calculations
	
	// Proteins
	double CO_protein = *CO_protein_ip;
	double GF_complex = *GF_complex_ip;
	double FKF1_protein = *FKF1_protein_ip;
	double GI_protein = *GI_protein_ip;
	
	// mRNA
	double FT_mRNA = *FT_mRNA_ip;
	double CO_mRNA = *CO_mRNA_ip;
	double CDF_mRNA = *CDF_mRNA_ip;
	double FKF1_mRNA = *FKF1_mRNA_ip;
	double GI_mRNA = *GI_mRNA_ip;
	
	// Intrinsic transcription rates
	double alpha_0_FT = *alpha_0_FT_ip;
	double alpha_0_CO = *alpha_0_CO_ip;
	
	// Other transcription parameters
	double alpha_FT = *alpha_FT_ip;
	double k_FT_mRNA = *k_FT_mRNA_ip;
	double alpha_CO = *alpha_CO_ip;
	double k_CO_mRNA = *k_CO_mRNA_ip;
	
	// Translation parameters
	double gamma_CO = *gamma_CO_ip;
	double gamma_CDF = *gamma_CDF_ip;
	double gamma_FKF1 = *gamma_FKF1_ip;
	double gamma_GI = *gamma_GI;
	
	// Intrinsic decay rate constants
	double beta_0_FT_mRNA = *beta_0_FT_mRNA_ip;
	double beta_0_CO_protein = *beta_0_CO_protein_ip;
	double beta_0_CO_mRNA = *beta_0_CO_mRNA_ip;
	double beta_0_CDF_protein = *beta_0_CDF_protein_ip;
	double beta_0_GF_complex = *beta_0_GF_complex_ip;
	double beta_0_FKF1 = *beta_0_FKF1_ip;
	double beta_0_GI = *beta_0_GI_ip;
	
	// Other decay parameters
	double beta_CDF_protein = *beta_CDF_protein_ip;
	double k_CDF_protein = *k_CDF_protein_ip;
	
	// Parameters for light-assisted complex formation
	double solar = *solar_ip;
	double alpha_FKF1 = *alpha_FKF1_ip;
	double alpha_GI = *alpha_GI_ip;
	
	// Update the output parameter list
	
	// Transcription
	update(FT_transcription_op, alpha_0_FT + alpha_FT * CO_protein / (CO_protein + k_FT_mRNA));	// CO protein activates FT transcription
	update(CO_transcription_op, alpha_0_CO + alpha_CO / (CDF_protein + k_CO_mRNA));				// CDF protein represses CO transcription
	
	// Translation
	update(CO_translation_op, gamma_CO * CO_mRNA);
	update(CDF_translation_op, gamma_CDF * CDF_mRNA);
	update(FKF1_translation_op, gamma_FKF1 * FKF1_mRNA);
	update(GI_translation_op, gamma_GI * GI_mRNA);
	
	// Light-assisted complex formation
	update(GF_complex_formation_op, solar*(alpha_FKF1 * FKF1_protein * FKF1_protein + alpha_GI * GI_protein * GI_protein));
	
	// Decay
	update(FT_mRNA_decay_op, beta_0_FT_mRNA * FT_mRNA);
	update(CO_protein_decay_op, beta_0_CO_protein * CO_protein);
	update(CO_mRNA_decay_op, beta_0_CO_mRNA * CO_mRNA);
	update(CDF_protein_decay_op, beta_0_CDF_protein + beta_CDF_protein * GF_complex / (GF_complex + k_CDF_protein));	// The GI-FKF1 complex assists in CDF protein degradation (this formula is a guess! not based on anything)
	update(GF_complex_decay_op, beta_0_GF_complex * GF_complex);
	update(FKF1_decay_op, beta_0_FKF1 * FKF1_protein);
	update(GI_decay_op, beta_0_GI * GI_protein);
}

#endif
