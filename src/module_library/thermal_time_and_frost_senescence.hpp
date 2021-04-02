#ifndef THERMAL_TIME_AND_FROST_SENESCENCE_H
#define THERMAL_TIME_AND_FROST_SENESCENCE_H

#include "../modules.h"
#include "../state_map.h"

class thermal_time_and_frost_senescence : public DerivModule
{
   public:
    thermal_time_and_frost_senescence(
        state_map const* input_parameters,
        state_map* output_parameters)
        :  // Define basic module properties by passing its name to its parent
           // class and indicate that this module is not compatible with
           // adaptive step size integrators
          DerivModule("thermal_time_and_frost_senescence", false),

          // Get pointers to input parameters
          TTc{get_input(input_parameters, "TTc")},
          seneLeaf{get_input(input_parameters, "seneLeaf")},
          seneStem{get_input(input_parameters, "seneStem")},
          seneRoot{get_input(input_parameters, "seneRoot")},
          seneRhizome{get_input(input_parameters, "seneRhizome")},
          Leaf{get_input(input_parameters, "Leaf")},
          leafdeathrate{get_input(input_parameters, "leafdeathrate")},
          lat{get_input(input_parameters, "lat")},
          time{get_input(input_parameters, "time")},
          temp{get_input(input_parameters, "temp")},
          Tfrostlow{get_input(input_parameters, "Tfrostlow")},
          Tfrosthigh{get_input(input_parameters, "Tfrosthigh")},
          stem_senescence_index{get_input(input_parameters, "stem_senescence_index")},
          root_senescence_index{get_input(input_parameters, "root_senescence_index")},
          rhizome_senescence_index{get_input(input_parameters, "rhizome_senescence_index")},
          kLeaf{get_input(input_parameters, "kLeaf")},
          kStem{get_input(input_parameters, "kStem")},
          kRoot{get_input(input_parameters, "kRoot")},
          kRhizome{get_input(input_parameters, "kRhizome")},
          kGrain{get_input(input_parameters, "kGrain")},
          remobilization_fraction{get_input(input_parameters, "remobilization_fraction")},
          net_assimilation_rate_leaf{get_input(input_parameters, "net_assimilation_rate_leaf")},
          net_assimilation_rate_stem{get_input(input_parameters, "net_assimilation_rate_stem")},
          net_assimilation_rate_root{get_input(input_parameters, "net_assimilation_rate_root")},
          net_assimilation_rate_rhizome{get_input(input_parameters, "net_assimilation_rate_rhizome")},

          // Get pointers to output parameters
          leafdeathrate_op{get_op(output_parameters, "leafdeathrate")},
          Leaf_op{get_op(output_parameters, "Leaf")},
          LeafLitter_op{get_op(output_parameters, "LeafLitter")},
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
    std::vector<double> mutable assim_rate_stem_vec;
    std::vector<double> mutable assim_rate_root_vec;
    std::vector<double> mutable assim_rate_rhizome_vec;

    // Pointers to input parameters
    double const& TTc;
    double const& seneLeaf;
    double const& seneStem;
    double const& seneRoot;
    double const& seneRhizome;
    double const& Leaf;
    double const& leafdeathrate;
    double const& lat;
    double const& time;
    double const& temp;
    double const& Tfrostlow;
    double const& Tfrosthigh;
    double const& stem_senescence_index;
    double const& root_senescence_index;
    double const& rhizome_senescence_index;
    double const& kLeaf;
    double const& kStem;
    double const& kRoot;
    double const& kRhizome;
    double const& kGrain;
    const double& remobilization_fraction;
    double const& net_assimilation_rate_leaf;
    double const& net_assimilation_rate_stem;
    double const& net_assimilation_rate_root;
    double const& net_assimilation_rate_rhizome;

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

string_vector thermal_time_and_frost_senescence::get_inputs()
{
    return {
        "TTc",                           // degree C * day
        "seneLeaf",                      // degree C * day
        "seneStem",                      // degree C * day
        "seneRoot",                      // degree C * day
        "seneRhizome",                   // degree C * day
        "Leaf",                          // Mg / ha
        "leafdeathrate",                 // percent
        "lat",                           // degrees
        "time",                          // days
        "temp",                          // degrees C
        "Tfrostlow",                     // degrees C
        "Tfrosthigh",                    // degrees C
        "stem_senescence_index",         // dimensionless
        "root_senescence_index",         // dimensionless
        "rhizome_senescence_index",      // dimensionless
        "kLeaf",                         // dimensionless
        "kStem",                         // dimensionless
        "kRoot",                         // dimensionless
        "kRhizome",                      // dimensionless
        "kGrain",                        // dimensionless
        "remobilization_fraction",       // dimensionless
        "net_assimilation_rate_leaf",    // Mg / ha / hour
        "net_assimilation_rate_stem",    // Mg / ha / hour
        "net_assimilation_rate_root",    // Mg / ha / hour
        "net_assimilation_rate_rhizome"  // Mg / ha / hour
    };
}

string_vector thermal_time_and_frost_senescence::get_outputs()
{
    return {
        "leafdeathrate",             // percent / hour
        "Leaf",                      // Mg / ha / hour
        "LeafLitter",                // Mg / ha / hour
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

void thermal_time_and_frost_senescence::do_operation() const
{
    // Add the new tissue growth to the history vectors
    assim_rate_stem_vec.push_back(net_assimilation_rate_stem);
    assim_rate_root_vec.push_back(net_assimilation_rate_root);
    assim_rate_rhizome_vec.push_back(net_assimilation_rate_rhizome);

    // Initialize variables
    double dLeafdeathrate{0.0};
    double dLeaf{0.0};
    double dStem{0.0};
    double dRoot{0.0};
    double dRhizome{0.0};
    double dGrain{0.0};
    double dLeafLitter{0.0};
    double dStemLitter{0.0};
    double dRootLitter{0.0};
    double dRhizomeLitter{0.0};
    double dstem_senescence_index{0.0};
    double droot_senescence_index{0.0};
    double drhizome_senescence_index{0.0};

    // Calculate the leaf death rate due to frost
    double frost_leaf_death_rate{0.0};
    if (TTc >= seneLeaf) {                 // Leaf senescence has started
        bool A = lat >= 0.0;               // In Northern hemisphere
        bool B = time >= 180.0;            // In second half of the year
        if ((A && B) || ((!A) && (!B))) {  // Winter in either hemisphere
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
    if (TTc >= seneLeaf) {
        // Use the leaf death rate to determine the change in leaf mass
        double change = Leaf * current_leaf_death_rate * (0.01 / 24);

        // Remobilize some of the lost leaf tissue and send the rest to the litter
        dLeaf += -change + kLeaf * change * remobilization_fraction;  // Why does the leaf remobilize its own tissue? (EBL)
        dLeafLitter += change * (1.0 - remobilization_fraction);
        dRhizome += kRhizome * change * remobilization_fraction;
        dStem += kStem * change * remobilization_fraction;
        dRoot += kRoot * change * remobilization_fraction;
        dGrain += kGrain * change * remobilization_fraction;
    }

    // Calculate stem senescence
    if (TTc >= seneStem) {
        // Look back in time to find out how much the tissue grew in the past.
        double change = assim_rate_stem_vec[stem_senescence_index];

        // Subtract the new growth from the tissue derivative
        dStem -= change;

        // Send the lost tissue to the litter
        dStemLitter += change;

        // Increment the tissue senescence index
        dstem_senescence_index++;
    }

    // Calculate root senescence
    if (TTc >= seneRoot) {
        // Look back in time to find out how much the tissue grew in the past.
        double change = assim_rate_root_vec[root_senescence_index];

        // Subtract the new growth from the tissue derivative
        dRoot -= change;

        // Send the lost tissue to the litter
        dRootLitter += change;

        // Increment the tissue senescence index
        droot_senescence_index++;
    }

    if (kRhizome > 0) {
        // Increment the rhizome senescence index if it is acting as a sink
        drhizome_senescence_index++;
    }

    // Calculate rhizome senescence
    if (TTc >= seneRhizome) {
        // Look back in time to find out how much the tissue grew in the past.
        double change = assim_rate_rhizome_vec[rhizome_senescence_index];

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
