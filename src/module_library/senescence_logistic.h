#ifndef SENESCENCE_LOGISTIC_H
#define SENESCENCE_LOGISTIC_H

#include "../modules.h"
#include "../state_map.h"

/**
 * @class senescence_logistic
 *
 * @brief Calculates the change in plant organ biomasses due to senescence.
 *
 * Intended to be used with the `senescence_coefficient_logistic` module.
 *
 * The amount that each plant component is senesced is determined as a percentage
 * (`kSeneLeaf`, `kSeneStem`, `kSeneRoot`, `kSeneRhizome`) of its current biomass.
 * Remobilization of senesced leaf tissue is also included based on the
 * partitioning growth parameters.
 *
 */
class senescence_logistic : public DerivModule
{
   public:
    senescence_logistic(
        const state_map* input_parameters,
        state_map* output_parameters)
        : DerivModule{"senescence_logistic"},

          // Get references to input parameters
          Leaf{get_input(input_parameters, "Leaf")},
          Stem{get_input(input_parameters, "Stem")},
          Root{get_input(input_parameters, "Root")},
          Rhizome{get_input(input_parameters, "Rhizome")},
          kSeneLeaf{get_input(input_parameters, "kSeneLeaf")},
          kSeneStem{get_input(input_parameters, "kSeneStem")},
          kSeneRoot{get_input(input_parameters, "kSeneRoot")},
          kSeneRhizome{get_input(input_parameters, "kSeneRhizome")},
          kLeaf{get_input(input_parameters, "kLeaf")},
          kStem{get_input(input_parameters, "kStem")},
          kRoot{get_input(input_parameters, "kRoot")},
          kRhizome{get_input(input_parameters, "kRhizome")},
          kGrain{get_input(input_parameters, "kGrain")},
          remobilization_fraction{get_input(input_parameters, "remobilization_fraction")},

          // Get pointers to output parameters
          Leaf_op{get_op(output_parameters, "Leaf")},
          LeafLitter_op{get_op(output_parameters, "LeafLitter")},
          Stem_op{get_op(output_parameters, "Stem")},
          StemLitter_op{get_op(output_parameters, "StemLitter")},
          Root_op{get_op(output_parameters, "Root")},
          RootLitter_op{get_op(output_parameters, "RootLitter")},
          Rhizome_op{get_op(output_parameters, "Rhizome")},
          RhizomeLitter_op{get_op(output_parameters, "RhizomeLitter")},
          Grain_op{get_op(output_parameters, "Grain")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // References to input parameters
    const double& Leaf;
    const double& Stem;
    const double& Root;
    const double& Rhizome;
    const double& kSeneLeaf;
    const double& kSeneStem;
    const double& kSeneRoot;
    const double& kSeneRhizome;
    const double& kLeaf;
    const double& kStem;
    const double& kRoot;
    const double& kRhizome;
    const double& kGrain;
    const double& remobilization_fraction;

    // Pointers to output parameters
    double* Leaf_op;
    double* LeafLitter_op;
    double* Stem_op;
    double* StemLitter_op;
    double* Root_op;
    double* RootLitter_op;
    double* Rhizome_op;
    double* RhizomeLitter_op;
    double* Grain_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};

string_vector senescence_logistic::get_inputs()
{
    return {
        "Leaf",                    // Mg / ha
        "Stem",                    // Mg / ha
        "Root",                    // Mg / ha
        "Rhizome",                 // Mg / ha
        "kSeneLeaf",               // dimensionless, fraction of Leaf senesced
        "kSeneStem",               // dimensionless, fraction of Stem senesced
        "kSeneRoot",               // dimensionless, fraction of Root senesced
        "kSeneRhizome",            // dimensionless, fraction of Rhizome senesced
        "kLeaf",                   // dimensionless, fraction carbon allocated to
                                   // Leaf
        "kStem",                   // dimensionless, fraction carbon allocated to
                                   // Stem
        "kRoot",                   // dimensionless, fraction carbon allocated to
                                   // Root
        "kRhizome",                // dimensionless, fraction carbon allocated to
                                   // Rhizome
        "kGrain",                  // dimensionless, fraction carbon allocated to
                                   // Grain
        "remobilization_fraction"  // dimensionless, fraction of senesced leaf
                                   // tissue remobilized to other plant organs
    };
}

string_vector senescence_logistic::get_outputs()
{
    return {
        "Leaf",           // Mg / ha
        "LeafLitter",     // Mg / ha
        "Stem",           // Mg / ha
        "StemLitter",     // Mg / ha
        "Root",           // Mg / ha
        "RootLitter",     // Mg / ha
        "Rhizome",        // Mg / ha
        "RhizomeLitter",  // Mg / ha
        "Grain"           // Mg / ha
    };
}

void senescence_logistic::do_operation() const
{
    double senescence_leaf = kSeneLeaf * Leaf;           // Mg / ha, amount of leaf senesced
    double senescence_stem = kSeneStem * Stem;           // Mg / ha, amount of stem senesced
    double senescence_root = kSeneRoot * Root;           // Mg / ha, amount of root senesced
    double senescence_rhizome = kSeneRhizome * Rhizome;  // Mg / ha, amount of
                                                         // rhizome senesced

    // change in leaf biomass = minus amount senesced + new leaf tissue from
    // remobilized amount (Allows for leaves to start senescing while new leaves
    // are still being produced).
    double dLeaf = -senescence_leaf + kLeaf * senescence_leaf * remobilization_fraction;  // Mg / ha

    // change in amount of leaf litter
    double dLeafLitter = senescence_leaf * (1 - remobilization_fraction); // Mg / ha

    // change in stem biomass = minus amount senesced + new stem tissue from
    // remobilized leaf fraction
    double dStem = -senescence_stem + kStem * senescence_leaf * remobilization_fraction;  // Mg / ha

    double dStemLitter = senescence_stem; // Mg / ha, change in amount of stem litter

    // change in root biomass = minus amount senesced + new root tissue from remobilized
    // leaf fraction
    double dRoot = -senescence_root + kRoot * senescence_leaf * remobilization_fraction; // Mg / ha

    double dRootLitter = senescence_root; // Mg / ha, change in amount of root litter

    // change in rhizome biomass = minus amount senesced + new rhizome from remobilized
    // leaf fraction
    double dRhizome = -senescence_rhizome + kRhizome * senescence_leaf * remobilization_fraction; // Mg / ha
    double dRhizomeLitter = senescence_rhizome; // Mg / ha, change in rhizome litter

    // change in grain biomass = new grain from remobilized leaf fraction.
    // currently do not include grain senescence.
    double dGrain = kGrain * senescence_leaf * remobilization_fraction;  // Mg / ha

    update(Leaf_op, dLeaf);                    // Mg / ha
    update(Stem_op, dStem);                    // Mg / ha
    update(Root_op, dRoot);                    // Mg / ha
    update(Rhizome_op, dRhizome);              // Mg / ha
    update(Grain_op, dGrain);                  // Mg / ha
    update(LeafLitter_op, dLeafLitter);        // Mg / ha
    update(StemLitter_op, dStemLitter);        // Mg / ha
    update(RootLitter_op, dRootLitter);        // Mg / ha
    update(RhizomeLitter_op, dRhizomeLitter);  // Mg / ha
}

#endif
