#ifndef THERMAL_TIME_SENESCENCE_H
#define THERMAL_TIME_SENESCENCE_H

#include "../modules.h"
#include "../state_map.h"

class thermal_time_senescence : public DerivModule
{
   public:
    thermal_time_senescence(
        state_map const* input_parameters,
        state_map* output_parameters)
        :  // Define basic module properties by passing its name to its parent
           // class and indicate that this module is not compatible with
           // adaptive step size integrators
          DerivModule("thermal_time_senescence", false),

          // Get pointers to input parameters
          TTc{get_input(input_parameters, "TTc")},
          seneLeaf{get_input(input_parameters, "seneLeaf")},
          seneStem{get_input(input_parameters, "seneStem")},
          seneRoot{get_input(input_parameters, "seneRoot")},
          seneRhizome{get_input(input_parameters, "seneRhizome")},
          leaf_senescence_index{get_input(input_parameters, "leaf_senescence_index")},
          stem_senescence_index{get_input(input_parameters, "stem_senescence_index")},
          root_senescence_index{get_input(input_parameters, "root_senescence_index")},
          rhizome_senescence_index{get_input(input_parameters, "rhizome_senescence_index")},
          kStem{get_input(input_parameters, "kStem")},
          kRoot{get_input(input_parameters, "kRoot")},
          kRhizome{get_input(input_parameters, "kRhizome")},
          kGrain{get_input(input_parameters, "kGrain")},
          net_assimilation_rate_leaf{get_input(input_parameters, "net_assimilation_rate_leaf")},
          net_assimilation_rate_stem{get_input(input_parameters, "net_assimilation_rate_stem")},
          net_assimilation_rate_root{get_input(input_parameters, "net_assimilation_rate_root")},
          net_assimilation_rate_rhizome{get_input(input_parameters, "net_assimilation_rate_rhizome")},

          // Get pointers to output parameters
          Leaf_op{get_op(output_parameters, "Leaf")},
          LeafLitter_op{get_op(output_parameters, "LeafLitter")},
          leaf_senescence_index_op{get_op(output_parameters, "leaf_senescence_index")},
          Stem_op{get_op(output_parameters, "Stem")},
          StemLitter_op{get_op(output_parameters, "StemLitter")},
          stem_senescence_index_op{get_op(output_parameters, "stem_senescence_index")},
          Root_op{get_op(output_parameters, "Root")},
          RootLitter_op{get_op(output_parameters, "RootLitter")},
          root_senescence_index_op{get_op(output_parameters, "root_senescence_index")},
          Rhizome_op{get_op(output_parameters, "Rhizome")},
          RhizomeLitter_op{get_op(output_parameters, "RhizomeLitter")},
          rhizome_senescence_index_op{get_op(output_parameters, "rhizome_senescence_index")},
          Grain_op{get_op(output_parameters, "Grain")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // Vectors for storing information about growth history
    //  Note: this feature is peculiar to this module
    //   and should be avoided in general since it
    //   precludes the use of any integration method
    //   except fixed-step Euler
    std::vector<double> mutable newLeaf_vec;
    std::vector<double> mutable newStem_vec;
    std::vector<double> mutable newRoot_vec;
    std::vector<double> mutable newRhizome_vec;

    // Pointers to input parameters
    double const& TTc;
    double const& seneLeaf;
    double const& seneStem;
    double const& seneRoot;
    double const& seneRhizome;
    double const& leaf_senescence_index;
    double const& stem_senescence_index;
    double const& root_senescence_index;
    double const& rhizome_senescence_index;
    double const& kStem;
    double const& kRoot;
    double const& kRhizome;
    double const& kGrain;
    double const& net_assimilation_rate_leaf;
    double const& net_assimilation_rate_stem;
    double const& net_assimilation_rate_root;
    double const& net_assimilation_rate_rhizome;

    // Pointers to output parameters
    double* Leaf_op;
    double* LeafLitter_op;
    double* leaf_senescence_index_op;
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

string_vector thermal_time_senescence::get_inputs()
{
    return {
        "TTc",                           // degree C * day
        "seneLeaf",                      // degree C * day
        "seneStem",                      // degree C * day
        "seneRoot",                      // degree C * day
        "seneRhizome",                   // degree C * day
        "leaf_senescence_index",         // dimensionless
        "stem_senescence_index",         // dimensionless
        "root_senescence_index",         // dimensionless
        "rhizome_senescence_index",      // dimensionless
        "kStem",                         // dimensionless
        "kRoot",                         // dimensionless
        "kRhizome",                      // dimensionless
        "kGrain",                        // dimensionless
        "net_assimilation_rate_leaf",    // Mg / ha / hour
        "net_assimilation_rate_stem",    // Mg / ha / hour
        "net_assimilation_rate_root",    // Mg / ha / hour
        "net_assimilation_rate_rhizome"  // Mg / ha / hour
    };
}

string_vector thermal_time_senescence::get_outputs()
{
    return {
        "Leaf",                      // Mg / ha / hour
        "LeafLitter",                // Mg / ha / hour
        "leaf_senescence_index",     // hour^-1
        "Stem",                      // Mg / ha / hour
        "StemLitter",                // Mg / ha / hour
        "stem_senescence_index",     // hour^-1
        "Root",                      // Mg / ha / hour
        "RootLitter",                // Mg / ha / hour
        "root_senescence_index",     // hour^-1
        "Rhizome",                   // Mg / ha / hour
        "RhizomeLitter",             // Mg / ha / hour
        "rhizome_senescence_index",  // hour^-1
        "Grain"                      // Mg / ha / hour
    };
}

void thermal_time_senescence::do_operation() const
{
    // Add the new tissue growth to the history vectors
    newLeaf_vec.push_back(net_assimilation_rate_leaf);
    newStem_vec.push_back(net_assimilation_rate_stem);
    newRoot_vec.push_back(net_assimilation_rate_root);
    newRhizome_vec.push_back(net_assimilation_rate_rhizome);

    // Initialize variables
    double dLeaf{0.0};
    double dStem{0.0};
    double dRoot{0.0};
    double dRhizome{0.0};
    double dGrain{0.0};
    double dLeafLitter{0.0};
    double dStemLitter{0.0};
    double dRootLitter{0.0};
    double dRhizomeLitter{0.0};
    double dleaf_senescence_index{0.0};
    double dstem_senescence_index{0.0};
    double droot_senescence_index{0.0};
    double drhizome_senescence_index{0.0};

    if (TTc >= seneLeaf) {
        // Look back in time to find out how much the tissue grew in the past.
        double change = newLeaf_vec[leaf_senescence_index];

        // Subtract the new growth from the tissue derivative
        // This means that the new value of leaf is the previous value
        //  plus the newLeaf(Senescence might start when there is still leaf being produced)
        //  minus the leaf produced at the corresponding k
        dLeaf -= change;

        // Remobilize some of the lost leaf tissue and send the rest to the litter
        double remob = 0.6;
        dLeafLitter += change * (1.0 - remob);
        dRhizome += kRhizome * change * remob;
        dStem += kStem * change * remob;
        dRoot += kRoot * change * remob;
        dGrain += kGrain * change * remob;

        // Increment the tissue senescence index
        dleaf_senescence_index++;
    }

    if (TTc >= seneStem) {
        // Look back in time to find out how much the tissue grew in the past.
        double change = newStem_vec[stem_senescence_index];

        // Subtract the new growth from the tissue derivative
        dStem -= change;

        // Send the lost tissue to the litter
        dStemLitter += change;

        // Increment the tissue senescence index
        dstem_senescence_index++;
    }

    if (TTc >= seneRoot) {
        // Look back in time to find out how much the tissue grew in the past.
        double change = newRoot_vec[root_senescence_index];

        // Subtract the new growth from the tissue derivative
        dRoot -= change;

        // Send the lost tissue to the litter
        dRootLitter += change;

        // Increment the tissue senescence index
        droot_senescence_index++;
    }

    if (TTc >= seneRhizome) {
        // Look back in time to find out how much the tissue grew in the past.
        double change = newRhizome_vec[rhizome_senescence_index];

        // Subtract the new growth from the tissue derivative
        dRhizome -= change;

        // Send the lost tissue to the litter
        dRhizomeLitter += change;

        // Increment the tissue senescence index
        drhizome_senescence_index++;
    }

    // Update the output parameter list
    update(Leaf_op, dLeaf);
    update(Stem_op, dStem);
    update(Root_op, dRoot);
    update(Rhizome_op, dRhizome);
    update(Grain_op, dGrain);

    update(LeafLitter_op, dLeafLitter);
    update(StemLitter_op, dStemLitter);
    update(RootLitter_op, dRootLitter);
    update(RhizomeLitter_op, dRhizomeLitter);

    update(leaf_senescence_index_op, dleaf_senescence_index);
    update(stem_senescence_index_op, dstem_senescence_index);
    update(root_senescence_index_op, droot_senescence_index);
    update(rhizome_senescence_index_op, drhizome_senescence_index);
}

#endif
