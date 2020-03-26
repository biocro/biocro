#ifndef THERMAL_TIME_AND_FROST_SENESCENCE_H
#define THERMAL_TIME_AND_FROST_SENESCENCE_H

#include "../modules.h"

class thermal_time_and_frost_senescence : public DerivModule {
	public:
		thermal_time_and_frost_senescence(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("thermal_time_and_frost_senescence", false),	// Indicate that this module is not compatible with adaptive step size integrators
			// Get pointers to input parameters
			TTc_ip(get_ip(input_parameters, "TTc")),
			seneLeaf_ip(get_ip(input_parameters, "seneLeaf")),
			seneStem_ip(get_ip(input_parameters, "seneStem")),
			seneRoot_ip(get_ip(input_parameters, "seneRoot")),
			seneRhizome_ip(get_ip(input_parameters, "seneRhizome")),
			Leaf_ip(get_ip(input_parameters, "Leaf")),
			leafdeathrate_ip(get_ip(input_parameters, "leafdeathrate")),
			lat_ip(get_ip(input_parameters, "lat")),
			doy_dbl_ip(get_ip(input_parameters, "doy_dbl")),
			temp_ip(get_ip(input_parameters, "temp")),
			Tfrostlow_ip(get_ip(input_parameters, "Tfrostlow")),
			Tfrosthigh_ip(get_ip(input_parameters, "Tfrosthigh")),
			stem_senescence_index_ip(get_ip(input_parameters, "stem_senescence_index")),
			root_senescence_index_ip(get_ip(input_parameters, "root_senescence_index")),
			rhizome_senescence_index_ip(get_ip(input_parameters, "rhizome_senescence_index")),
			kLeaf_ip(get_ip(input_parameters, "kLeaf")),
			kStem_ip(get_ip(input_parameters, "kStem")),
			kRoot_ip(get_ip(input_parameters, "kRoot")),
			kRhizome_ip(get_ip(input_parameters, "kRhizome")),
			kGrain_ip(get_ip(input_parameters, "kGrain")),
			newLeafcol_ip(get_ip(input_parameters, "newLeafcol")),
			newStemcol_ip(get_ip(input_parameters, "newStemcol")),
			newRootcol_ip(get_ip(input_parameters, "newRootcol")),
			newRhizomecol_ip(get_ip(input_parameters, "newRhizomecol")),
			// Get pointers to output parameters
			leafdeathrate_op(get_op(output_parameters, "leafdeathrate")),
			Leaf_op(get_op(output_parameters, "Leaf")),
			LeafLitter_op(get_op(output_parameters, "LeafLitter")),
			Stem_op(get_op(output_parameters, "Stem")),
			StemLitter_op(get_op(output_parameters, "StemLitter")),
			stem_senescence_index_op(get_op(output_parameters, "stem_senescence_index")),
			Root_op(get_op(output_parameters, "Root")),
			RootLitter_op(get_op(output_parameters, "RootLitter")),
			root_senescence_index_op(get_op(output_parameters, "root_senescence_index")),
			Rhizome_op(get_op(output_parameters, "Rhizome")),
			RhizomeLitter_op(get_op(output_parameters, "RhizomeLitter")),
			rhizome_senescence_index_op(get_op(output_parameters, "rhizome_senescence_index")),
			Grain_op(get_op(output_parameters, "Grain"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Vectors for storing information about growth history
		//  Note: this feature is peculiar to this module
		//   and should be avoided in general since it
		//   precludes the use of any integration method
		//   except fixed-step Euler
		mutable std::vector<double> newStem_vec;
		mutable std::vector<double> newRoot_vec;
		mutable std::vector<double> newRhizome_vec;
		// Pointers to input parameters
		const double* TTc_ip;
		const double* seneLeaf_ip;
		const double* seneStem_ip;
		const double* seneRoot_ip;
		const double* seneRhizome_ip;
		const double* Leaf_ip;
		const double* leafdeathrate_ip;
		const double* lat_ip;
		const double* doy_dbl_ip;
		const double* temp_ip;
		const double* Tfrostlow_ip;
		const double* Tfrosthigh_ip;
		const double* stem_senescence_index_ip;
		const double* root_senescence_index_ip;
		const double* rhizome_senescence_index_ip;
		const double* kLeaf_ip;
		const double* kStem_ip;
		const double* kRoot_ip;
		const double* kRhizome_ip;
		const double* kGrain_ip;
		const double* newLeafcol_ip;
		const double* newStemcol_ip;
		const double* newRootcol_ip;
		const double* newRhizomecol_ip;
		// Pointers to output parameters
		double* leafdeathrate_op;
		double* Leaf_op;
		double* LeafLitter_op;
		double* Stem_op;
		double* StemLitter_op;
		double* stem_senescence_index_op;
		double* Root_op;
		double* RootLitter_op;
		double* root_senescence_index_op;
		double* Rhizome_op;
		double* RhizomeLitter_op;
		double* rhizome_senescence_index_op;
		double* Grain_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> thermal_time_and_frost_senescence::get_inputs() {
	return {
		"TTc",
		"seneLeaf", "seneStem", "seneRoot", "seneRhizome",
		"Leaf", "leafdeathrate", "lat", "doy_dbl", "temp", "Tfrostlow", "Tfrosthigh",
		"stem_senescence_index", "root_senescence_index", "rhizome_senescence_index",
		"kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
		"newLeafcol", "newStemcol", "newRootcol", "newRhizomecol"
	};
}

std::vector<std::string> thermal_time_and_frost_senescence::get_outputs() {
	return {
		"leafdeathrate",
		"Leaf", "LeafLitter",
		"Stem", "StemLitter", "stem_senescence_index",
		"Root", "RootLitter", "root_senescence_index",
		"Rhizome", "RhizomeLitter", "rhizome_senescence_index",
		"Grain"
	};
}

void thermal_time_and_frost_senescence::do_operation() const {
	// Add the new tissue growth to the history vectors
	newStem_vec.push_back(*newStemcol_ip);
	newRoot_vec.push_back(*newRootcol_ip);
	newRhizome_vec.push_back(*newRhizomecol_ip);
	
	// Collect inputs and make calculations
	double TTc = *TTc_ip;
	
	double seneLeaf = *seneLeaf_ip;
	double seneStem = *seneStem_ip;
	double seneRoot = *seneRoot_ip;
	double seneRhizome = *seneRhizome_ip;
	
	double Leaf = *Leaf_ip;
	double leafdeathrate = *leafdeathrate_ip;
	double lat = *lat_ip;
	double doy_dbl = *doy_dbl_ip;
	double temp = *temp_ip;
	double Tfrostlow = *Tfrostlow_ip;
	double Tfrosthigh = *Tfrosthigh_ip;
	
	int stem_senescence_index = (int) *stem_senescence_index_ip;
	int root_senescence_index = (int) *root_senescence_index_ip;
	int rhizome_senescence_index = (int) *rhizome_senescence_index_ip;
	
	double kLeaf = *kLeaf_ip;
	double kStem = *kStem_ip;
	double kRoot = *kRoot_ip;
	double kRhizome = *kRhizome_ip;
	double kGrain = *kGrain_ip;
	
	double dLeafdeathrate = 0.0;
	double dLeaf = 0.0, dStem = 0.0, dRoot = 0.0, dRhizome = 0.0, dGrain = 0.0;
	double dLeafLitter = 0.0, dStemLitter = 0.0, dRootLitter = 0.0, dRhizomeLitter = 0.0;
	double dstem_senescence_index = 0.0, droot_senescence_index = 0.0, drhizome_senescence_index = 0.0;
	
	// Calculate the leaf death rate due to frost
	double frost_leaf_death_rate = 0.0;
	if(TTc >= seneLeaf) {	// Leaf senescence has started
		bool A = lat >= 0.0;		// In Northern hemisphere
		bool B = doy_dbl >= 180.0;	// In second half of the year
		if((A && B) || ((!A) && (!B))) {	// Winter in either hemisphere
			// frost_leaf_death_rate changes linearly from 100 to 0 as temp changes from Tfrostlow to Tfrosthigh and is limited to [0,100]
			std::max(0.0, std::min(100.0, 100.0 * (Tfrosthigh - temp) / (Tfrosthigh - Tfrostlow)));
		}
	}
	
	// The current leaf death rate is the larger of the previously stored leaf death rate and the new frost death rate
	// I.e., the leaf death rate sometimes increases but never decreases
	double current_leaf_death_rate = std::max(leafdeathrate, frost_leaf_death_rate);
	
	// Report the change in leaf death rate as the derivative of leafdeathrate
	// Note: this will probably only work well with the Euler method
	dLeafdeathrate = current_leaf_death_rate - leafdeathrate;
	
	// Calculate leaf senescence
	if(TTc >= seneLeaf) {
		// Use the leaf death rate to determine the change in leaf mass
		double change = Leaf * current_leaf_death_rate * (0.01 / 24);
		
		// Remobilize some of the lost leaf tissue and send the rest to the litter
		double remob = 0.6;
		dLeaf += -change + kLeaf * change * remob;	// Why does the leaf remobilize its own tissue? (EBL)
		dLeafLitter += change * (1.0 - remob);
		dRhizome += kRhizome * change * remob;
		dStem += kStem * change * remob;
		dRoot += kRoot * change * remob;
		dGrain += kGrain * change * remob;
	}
	
	// Calculate stem senescence
	if(TTc >= seneStem) {
		// Look back in time to find out how much the tissue grew in the past.
		double change = newStem_vec[stem_senescence_index];
		
		// Subtract the new growth from the tissue derivative
		dStem -= change;
		
		// Send the lost tissue to the litter
		dStemLitter += change;
		
		// Increment the tissue senescence index
		dstem_senescence_index++;
	}
	
	// Calculate root senescence
	if(TTc >= seneRoot) {
		// Look back in time to find out how much the tissue grew in the past.
		double change = newRoot_vec[root_senescence_index];
		
		// Subtract the new growth from the tissue derivative
		dRoot -= change;
		
		// Send the lost tissue to the litter
		dRootLitter += change;
		
		// Increment the tissue senescence index
		droot_senescence_index++;
	}
	
	// Calculate rhizome senescence
	if(TTc >= seneRhizome) {
		// Look back in time to find out how much the tissue grew in the past.
		double change = newRhizome_vec[rhizome_senescence_index];
		
		// Subtract the new growth from the tissue derivative
		dRhizome -= change;
		
		// Send the lost tissue to the litter
		dRhizomeLitter += change;
		
		// Increment the tissue senescence index
		drhizome_senescence_index++;
	}
	
	// No grain senescence, although grain might gain some remobilized leaf tissue
	
	// Update the output parameter list
	update(leafdeathrate_op, dLeafdeathrate);
	
	update(Leaf_op, dLeaf);
	update(Stem_op, dStem);
	update(Root_op, dRoot);
	update(Rhizome_op, dRhizome);
	update(Grain_op, dGrain);
	
	update(LeafLitter_op, dLeafLitter);
	update(StemLitter_op, dStemLitter);
	update(RootLitter_op, dRootLitter);
	update(RhizomeLitter_op, dRhizomeLitter);
	
	update(stem_senescence_index_op, dstem_senescence_index);
	update(root_senescence_index_op, droot_senescence_index);
	update(rhizome_senescence_index_op, drhizome_senescence_index);
}

#endif
