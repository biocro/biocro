#ifndef THERMAL_TIME_AND_FROST_SENESCENCE_H
#define THERMAL_TIME_AND_FROST_SENESCENCE_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class thermal_time_and_frost_senescence
 *
 *  @brief Determines senescence rates for several plant organs based on thermal
 *  time thresholds, the occurrence of frost, and magical time travel; only
 *  compatible with a fixed-step Euler solver.
 *
 *  ### Model overview
 *
 *  This module calculates senescence for the following organs:
 *  - `Leaf`
 *  - `Stem`
 *  - `Root`
 *  - `Rhizome`
 *
 *  For a particular organ, senescence begins to occur after the total thermal
 *  time passes a threshold value. When this happens for the leaf, some of its
 *  mass is lost, with a fraction being remobilized to other organs and the
 *  remainder contributing to the litter. The other organs contribute to litter
 *  but their tissue is not remobilized.
 *
 *  It is possible for the grain to gain mass that was remobilized from the
 *  leaf, although grain senescence is not considered here.
 *
 *  To decide how much mass is being lost from stem, root, and rhizome, this
 *  model looks back in time. For the first timestep after senescence starts,
 *  this model assumes that the amount of mass lost is equal to the amount of
 *  mass that organ gained during its first timestep of growth. Then, for the
 *  second timestep after senescence starts, the organ loses the amount of mass
 *  it gained during its second hour of growth. This process is repeated
 *  indefinitely.
 *
 *  For some reason, this model does not consider any mass gained due to carbon
 *  remoblization or retranslocation; it only considers the net amount of carbon
 *  gained through photosynthesis.
 *
 *  The leaf is treated differently and only begins to senescence when a
 *  temperature threshold is crossed in addition to the thermal time threshold.
 *  Its senescence rate is based on frost alone, rather than magical time
 *  travel.
 *
 *  There are some problems with this type of senescence model:
 *  - In reality, a plant does not "remember" how much it grew at a particular
 *    time in the past.
 *  - This method only works with a fixed-step Euler solver.
 *  - If the model runs long enough, it will become oscillatory.
 *
 *  ### Details of implementation
 *
 *  In general, this model will be solved for a set of N time points `t_0`,
 *  `t_1`, ..., `t_{N-1}`. We can think of them as being labeled by an integer
 *  "index" i = 0, 1, ..., N-1. So, at each time point, this module stores the
 *  net rate of carbon assimilation due to photosynthesis in its
 *  `assim_rate_XXX_vec` members. When senescence begins for an organ, the
 *  senescence rate is determined from the 0th element of its associated
 *  `assim_rate_XXX_vec` vector. For the next timestep, element 1 is used. So on
 *  and so forth. This module uses the "senescence index" quantities to keep
 *  track of the index to use for senescence calculations.
 *
 *  Special care must be taken for the rhizome, since it may begin the
 *  simulation as a carbon source rather than a carbon sink. In this case, the
 *  0th element of the `assim_rate_rhizome_vec` vector would not correspond to
 *  the rhizome's first timestep of growth. To account for this, the
 *  `rhizome_senescence_index` must be incremented while it is a carbon source.
 *  Then, when senescence kicks in later, the rhizome senescence index will
 *  refer to the first time point when the rhizome began to grow, rather than
 *  the first time point of the simulation.
 *
 *  Obviously this system is very fragile and requires some assumption about the
 *  behavior of the rhizome. Also, if any other organs ever act as carbon
 *  sources, they may not be handled properly.
 *
 *  ### Source
 *
 *  The model represented by this module was used in one published BioCro paper:
 *
 *  - Wang, D. et al. "A physiological and biophysical model of coppice willow
 *    (Salix spp.) production yields for the contiguous USA in current and
 *    future climate scenarios" [Plant, Cell & Environment 38, 1850–1865 (2015)]
 *    (https://doi.org/10.1111/pce.12556)
 */
class thermal_time_and_frost_senescence : public differential_module
{
   public:
    thermal_time_and_frost_senescence(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Iindicate that this module requires a fixed step size Euler ODE solver
          differential_module(true),

          // Get pointers to input quantities
          TTc{get_input(input_quantities, "TTc")},
          seneLeaf{get_input(input_quantities, "seneLeaf")},
          seneStem{get_input(input_quantities, "seneStem")},
          seneRoot{get_input(input_quantities, "seneRoot")},
          seneRhizome{get_input(input_quantities, "seneRhizome")},
          Leaf{get_input(input_quantities, "Leaf")},
          leafdeathrate{get_input(input_quantities, "leafdeathrate")},
          lat{get_input(input_quantities, "lat")},
          time{get_input(input_quantities, "time")},
          temp{get_input(input_quantities, "temp")},
          Tfrostlow{get_input(input_quantities, "Tfrostlow")},
          Tfrosthigh{get_input(input_quantities, "Tfrosthigh")},
          stem_senescence_index{get_input(input_quantities, "stem_senescence_index")},
          root_senescence_index{get_input(input_quantities, "root_senescence_index")},
          rhizome_senescence_index{get_input(input_quantities, "rhizome_senescence_index")},
          kLeaf{get_input(input_quantities, "kLeaf")},
          kStem{get_input(input_quantities, "kStem")},
          kRoot{get_input(input_quantities, "kRoot")},
          kRhizome{get_input(input_quantities, "kRhizome")},
          kGrain{get_input(input_quantities, "kGrain")},
          remobilization_fraction{get_input(input_quantities, "remobilization_fraction")},
          net_assimilation_rate_leaf{get_input(input_quantities, "net_assimilation_rate_leaf")},
          net_assimilation_rate_stem{get_input(input_quantities, "net_assimilation_rate_stem")},
          net_assimilation_rate_root{get_input(input_quantities, "net_assimilation_rate_root")},
          net_assimilation_rate_rhizome{get_input(input_quantities, "net_assimilation_rate_rhizome")},

          // Get pointers to output quantities
          leafdeathrate_op{get_op(output_quantities, "leafdeathrate")},
          Leaf_op{get_op(output_quantities, "Leaf")},
          LeafLitter_op{get_op(output_quantities, "LeafLitter")},
          Stem_op{get_op(output_quantities, "Stem")},
          StemLitter_op{get_op(output_quantities, "StemLitter")},
          stem_senescence_index_op{get_op(output_quantities, "stem_senescence_index")},
          Root_op{get_op(output_quantities, "Root")},
          RootLitter_op{get_op(output_quantities, "RootLitter")},
          root_senescence_index_op{get_op(output_quantities, "root_senescence_index")},
          Rhizome_op{get_op(output_quantities, "Rhizome")},
          RhizomeLitter_op{get_op(output_quantities, "RhizomeLitter")},
          rhizome_senescence_index_op{get_op(output_quantities, "rhizome_senescence_index")},
          Grain_op{get_op(output_quantities, "Grain")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "thermal_time_and_frost_senescence"; }

   private:
    // Vectors for storing information about growth history
    //  Note: this feature is peculiar to this module
    //   and should be avoided in general since it
    //   precludes the use of any integration method
    //   except fixed-step Euler
    std::vector<double> mutable assim_rate_stem_vec;
    std::vector<double> mutable assim_rate_root_vec;
    std::vector<double> mutable assim_rate_rhizome_vec;

    // Pointers to input quantities
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

    // Pointers to output quantities
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

    // Calculate the leaf death rate due to frost. Leaf death should only occur
    // due to frost when:
    //   1. Leaf senescence has started (TTc >= seneLeaf)
    //   2. We are in the winter months (definition depends on whether we are in
    //      the Northern or Southern hemisphere)
    // Under these conditions, if the temperature is at or above Tfrosthigh,
    // there is no leaf death due to frost. If the temperature is at or below
    // Tfrostlow, the leaf death rate reaches its maximum (100%). For
    // temperatures between Tfrosthigh and Tfrostlow, the death rate changes
    // linearly with temperature.
    bool const sene_started = TTc >= seneLeaf;
    bool const in_north = lat >= 0.0;
    bool const late_year = time >= 180.0;
    bool const in_winter = (in_north && late_year) || (!in_north && !late_year);

    double const base_frost_death_rate =
        100.0 * std::max(0.0, std::min(1.0, (Tfrosthigh - temp) / (Tfrosthigh - Tfrostlow)));

    double const frost_leaf_death_rate =
        sene_started && in_winter ? base_frost_death_rate : 0.0;

    // The current leaf death rate is the larger of the previously stored leaf
    // death rate and the new frost death rate. I.e., the leaf death rate
    // sometimes increases but never decreases
    double current_leaf_death_rate = std::max(leafdeathrate,
                                              frost_leaf_death_rate);

    // Report the change in leaf death rate as the derivative of leafdeathrate.
    // Note: this will probably only work well with the Euler method
    dLeafdeathrate = current_leaf_death_rate - leafdeathrate;

    // Calculate leaf senescence
    if (TTc >= seneLeaf) {
        // Use the leaf death rate to determine the change in leaf mass
        double change = Leaf * current_leaf_death_rate * (0.01 / 24);

        // Remobilize some of the lost leaf tissue and send the rest to the
        // litter. EBL wonders: why does the leaf remobilize its own tissue?
        dLeaf += -change + kLeaf * change * remobilization_fraction;
        dLeafLitter += change * (1.0 - remobilization_fraction);
        dRhizome += kRhizome * change * remobilization_fraction;
        dStem += kStem * change * remobilization_fraction;
        dRoot += kRoot * change * remobilization_fraction;
        dGrain += kGrain * change * remobilization_fraction;
    }

    // Calculate stem senescence
    if (TTc >= seneStem) {
        // Look back in time to find out how much the tissue grew in the past.
        double change = assim_rate_stem_vec.at(stem_senescence_index);

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
        double change = assim_rate_root_vec.at(root_senescence_index);

        // Subtract the new growth from the tissue derivative
        dRoot -= change;

        // Send the lost tissue to the litter
        dRootLitter += change;

        // Increment the tissue senescence index
        droot_senescence_index++;
    }

    if (kRhizome < 0) {
        // Increment the rhizome senescence index if it is acting as a source
        drhizome_senescence_index++;
    }

    // Calculate rhizome senescence
    if (TTc >= seneRhizome) {
        // Look back in time to find out how much the tissue grew in the past.
        double change = assim_rate_rhizome_vec.at(rhizome_senescence_index);

        // Subtract the new growth from the tissue derivative
        dRhizome -= change;

        // Send the lost tissue to the litter
        dRhizomeLitter += change;

        // Increment the tissue senescence index
        drhizome_senescence_index++;
    }

    // Update the output quantity list
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

}  // namespace standardBML
#endif
