#ifndef UTILIZATION_GROWTH_CALCULATOR_H
#define UTILIZATION_GROWTH_CALCULATOR_H

#include "../modules.h"

class utilization_growth_calculator : public SteadyModule {
	public:
		utilization_growth_calculator(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("utilization_growth_calculator"),
			// Get pointers to input parameters
			//dawn_phase_ip(get_ip(input_parameters, "dawn_phase")),
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
		static std::string get_description();
	private:
		// Pointers to input parameters
		//const double* dawn_phase_ip;
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

std::vector<std::string> utilization_growth_calculator::get_inputs() {
	return {
		//"dawn_phase",
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

std::vector<std::string> utilization_growth_calculator::get_outputs() {
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

std::string utilization_growth_calculator::get_description() {
	std::string description = std::string("  The utilization growth model is based on several papers published by Thornley and is implemented in BioCro via two separate modules.\n");
	description += std::string("  This module calculates utilization and transport rates for different plant tissue types.\n");
	description += std::string("  It is intended to be run along with the utilization_growth module.\n");
	description += std::string("  See utilization_growth.hpp and utilization_growth_calculator.hpp for more information.");
	return description;
}

inline double utilization_hill(double KM, double n, double concentration) {
	return pow(concentration, n) / (pow(KM, n) + pow(concentration, n));
}

void utilization_growth_calculator::do_operation() const {	
	// Collect inputs and make calculations	
	
	//double dawn_phase = *dawn_phase_ip;
	//if(dawn_phase > 3.14159265358979323846) dawn_phase -= 2*3.14159265358979323846;
	//double rate_factor = exp(-(dawn_phase + 1) * (dawn_phase + 1));
	
	double grain_TTc = *grain_TTc_ip;
    double TTc = *TTc_ip;
	
    double kLeaf = *rate_constant_leaf_ip;
    double kStem = *rate_constant_stem_ip;
    double kRoot = *rate_constant_root_ip;
    double kRhizome = *rate_constant_rhizome_ip;
    double kGrain = *rate_constant_grain_ip;
    
	/*
    double kLeaf = *rate_constant_leaf_ip * rate_factor;
    double kStem = *rate_constant_stem_ip * rate_factor;
    double kRoot = *rate_constant_root_ip * rate_factor;
    double kRhizome = *rate_constant_rhizome_ip * rate_factor;
    double kGrain = *rate_constant_grain_ip * rate_factor;
	 */

    double KmLeaf = *KmLeaf_ip;
    double KmStem = *KmStem_ip;
    double KmRoot = *KmRoot_ip;
    double KmRhizome = *KmRhizome_ip;
    double KmGrain = *KmGrain_ip;

    double resistance_leaf_to_stem = *resistance_leaf_to_stem_ip;
    double resistance_stem_to_grain = *resistance_stem_to_grain_ip;
    double resistance_stem_to_root = *resistance_stem_to_root_ip;
    double resistance_stem_to_rhizome = *resistance_stem_to_rhizome_ip;

    double Leaf = *Leaf_ip;
    double Stem = *Stem_ip;
    double Root = *Root_ip;
    double Grain = *Grain_ip;
    double Rhizome = *Rhizome_ip;

    double substrate_pool_leaf = *substrate_pool_leaf_ip;
    double substrate_pool_stem = *substrate_pool_stem_ip;
    double substrate_pool_root = *substrate_pool_root_ip;
    double substrate_pool_rhizome = *substrate_pool_rhizome_ip;
    double substrate_pool_grain = *substrate_pool_grain_ip;
    
    // Create a logistic scaling coefficient to smoothly ramp up the grain growth rate
	//  The midpoint is a TTc value defined in the input parameters
	//  The maximum value is 1
	//  The steepness is chosen so the coefficient reaches 95% of its maximum
	//    value when TTc passes the midpoint by 8 thermal days:
	//    k = ln(0.95/(1-0.95))/8 = 0.368
	//  This roughly corresponds to grain growth "turning on" over the course of
	//    2*8 = 16 thermal days
	
	double kGrain_scale = 1/(1 + exp(-0.368*(TTc - grain_TTc)));
	
	// Create an exponential scaling coefficient to smoothly ramp down the rhizome
	//  "retranslocation" rate
	
	//double kRhizome_retrans = 0.01 * kRhizome * exp(-TTc/300.0);
	double kRhizome_retrans = 0.0;
    
    // Calculate the substrate utilization rates
    //  mass_fraction_leaf = substrate_pool_leaf / Leaf is a measure of the substrate concentration in the leaf,
    //    made under the assumption that leaf volume is proportional to its structural carbon component, i.e., Leaf
    //  We assume there is a catalyzed reaction transforming the substrate carbon into structural carbon.
    //    Using Michaelis-Menton kinetics, the rate of change of the structural carbon concentration is proportional
    //    to [substrate]/(Km + [substrate]), i.e., mass_fraction_leaf / (KmLeaf + mass_fraction_leaf)
    //  The total substrate utilization is this rate multiplied by the leaf volume, i.e.
    //    kLeaf * Leaf * mass_fraction_leaf / (KmLeaf + mass_fraction_leaf) = kLeaf * substrate_pool_leaf / (KmLeaf + mass_fraction_leaf)
    //    where kLeaf includes the various proportionality constants mentioned previously
    //  See Thornley, J. H. M. A Model to Describe the Partitioning of Photosynthate during Vegetative Plant Growth. Ann Bot 36, 419430 (1972)
    // Alternatively, we could use a Hill equation to model utilization. This may be especially helpful for the leaf, since utilization
    //  can be made to fall off faster at small substrate concentrations. Note that the M-M equation is a special case of the Hill
    //  equation when n = 1.
    
    double n = 2.0;		// Set to 1.0 for Michaelis-Morley kinetics. Use larger values to get a steeper utilization cutoff at small substrate concentrations.
    
    double mass_fraction_leaf = substrate_pool_leaf / Leaf;
    double utilization_leaf = kLeaf * Leaf * utilization_hill(KmLeaf, n, mass_fraction_leaf);
	
    double mass_fraction_stem = substrate_pool_stem / Stem;
    double utilization_stem = kStem * Stem * utilization_hill(KmStem, n, mass_fraction_stem);
	
    double mass_fraction_root = substrate_pool_root / Root;
    double utilization_root = kRoot * Root * utilization_hill(KmRoot, n, mass_fraction_root);
	
    double mass_fraction_rhizome = substrate_pool_rhizome / Rhizome;
    double utilization_rhizome = kRhizome * Rhizome * utilization_hill(KmRhizome, n, mass_fraction_rhizome);
    utilization_rhizome -= kRhizome_retrans * Rhizome;	// Add an additional term that allows the rhizome to redistribute some of its mass in the early stages of growth, i.e., when TTc is small
	
    double mass_fraction_grain = substrate_pool_grain / Grain;
    double utilization_grain = kGrain * kGrain_scale * Grain * utilization_hill(KmGrain, n, mass_fraction_grain);
    
    // Calculate the transport rates
    //  We make the simple assumption that substrate transport is proportional to a concentration gradient between tissues
    //   and inversely proportional to a resistance, analogous to current in an electrical circuit
    //  It is important to scale the transport as the plant grows. For example, if the plant uniformly doubles in volume,
    //   the roots now require twice as much carbon substrate to continue growth at the same rate, so the resistance between
    //   the stem and root should be halved (or transport should double). We accomplish this scaling with the beta factor.
    //  See Thornley, J. H. M. A Model to Describe the Partitioning of Photosynthate during Vegetative Plant Growth. Ann Bot 36, 419430 (1972)
    
    double beta = Leaf + Grain + Stem + Root + Rhizome;
    
    double transport_leaf_to_stem = beta * (mass_fraction_leaf - mass_fraction_stem) / resistance_leaf_to_stem;
    double transport_stem_to_grain = beta * (mass_fraction_stem - mass_fraction_grain) / resistance_stem_to_grain;
    double transport_stem_to_root = beta * (mass_fraction_stem - mass_fraction_root) / resistance_stem_to_root;
    double transport_stem_to_rhizome = beta * (mass_fraction_stem - mass_fraction_rhizome) / resistance_stem_to_rhizome;
	
	// Update the output parameter list
    update(utilization_leaf_op, utilization_leaf);
    update(utilization_stem_op, utilization_stem);
    update(utilization_root_op, utilization_root);
    update(utilization_rhizome_op, utilization_rhizome);
    update(utilization_grain_op, utilization_grain);

    update(transport_leaf_to_stem_op, transport_leaf_to_stem);
    update(transport_stem_to_grain_op, transport_stem_to_grain);
    update(transport_stem_to_root_op, transport_stem_to_root);
    update(transport_stem_to_rhizome_op, transport_stem_to_rhizome);
    
    update(mass_fraction_leaf_op, mass_fraction_leaf);
    update(mass_fraction_stem_op, mass_fraction_stem);
    update(mass_fraction_root_op, mass_fraction_root);
    update(mass_fraction_rhizome_op, mass_fraction_rhizome);
    update(mass_fraction_grain_op, mass_fraction_grain);
    
    update(kGrain_scale_op, kGrain_scale);
}

#endif
