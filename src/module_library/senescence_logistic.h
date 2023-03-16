#ifndef SENESCENCE_LOGISTIC_H
#define SENESCENCE_LOGISTIC_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class senescence_logistic
 *
 * @brief Calculates the change in plant organ biomasses due to senescence.
 *
 * Intended to be used with the `senescence_coefficient_logistic` module.
 *
 * The amount that each plant component is senesced is determined as a
 * percentage (`kSeneLeaf`, `kSeneStem`, `kSeneRoot`, `kSeneRhizome`) of its
 * current biomass. Remobilization of senesced leaf tissue is also included
 * based on the partitioning growth parameters.
 *
 */
class senescence_logistic : public differential_module
{
   public:
    senescence_logistic(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities
          Leaf{get_input(input_quantities, "Leaf")},
          Stem{get_input(input_quantities, "Stem")},
          Root{get_input(input_quantities, "Root")},
          Rhizome{get_input(input_quantities, "Rhizome")},
          kSeneLeaf{get_input(input_quantities, "kSeneLeaf")},
          kSeneStem{get_input(input_quantities, "kSeneStem")},
          kSeneRoot{get_input(input_quantities, "kSeneRoot")},
          kSeneRhizome{get_input(input_quantities, "kSeneRhizome")},
          kLeaf{get_input(input_quantities, "kLeaf")},
          kStem{get_input(input_quantities, "kStem")},
          kRoot{get_input(input_quantities, "kRoot")},
          kRhizome{get_input(input_quantities, "kRhizome")},
          kGrain{get_input(input_quantities, "kGrain")},
          kShell{get_input(input_quantities, "kShell")},
          remobilization_fraction{get_input(input_quantities, "remobilization_fraction")},

          // Get pointers to output quantities
          Leaf_op{get_op(output_quantities, "Leaf")},
          LeafLitter_op{get_op(output_quantities, "LeafLitter")},
          Stem_op{get_op(output_quantities, "Stem")},
          StemLitter_op{get_op(output_quantities, "StemLitter")},
          Root_op{get_op(output_quantities, "Root")},
          RootLitter_op{get_op(output_quantities, "RootLitter")},
          Rhizome_op{get_op(output_quantities, "Rhizome")},
          RhizomeLitter_op{get_op(output_quantities, "RhizomeLitter")},
          Grain_op{get_op(output_quantities, "Grain")},
          Shell_op{get_op(output_quantities, "Shell")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "senescence_logistic"; }

   private:
    // References to input quantities
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
    const double& kShell;
    const double& remobilization_fraction;

    // Pointers to output quantities
    double* Leaf_op;
    double* LeafLitter_op;
    double* Stem_op;
    double* StemLitter_op;
    double* Root_op;
    double* RootLitter_op;
    double* Rhizome_op;
    double* RhizomeLitter_op;
    double* Grain_op;
    double* Shell_op;

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
        "kLeaf",                   // dimensionless, fraction carbon allocated to Leaf
        "kStem",                   // dimensionless, fraction carbon allocated to Stem
        "kRoot",                   // dimensionless, fraction carbon allocated to Root
        "kRhizome",                // dimensionless, fraction carbon allocated to Rhizome
        "kGrain",                  // dimensionless, fraction carbon allocated to Grain
        "kShell",                  // dimensionless, fraction carbon allocated to Shell
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
        "Shell",          // Mg / ha
        "Grain"           // Mg / ha
    };
}

void senescence_logistic::do_operation() const
{
    double senescence_leaf = kSeneLeaf * Leaf;           // Mg / ha, amount of leaf senesced
    double senescence_stem = kSeneStem * Stem;           // Mg / ha, amount of stem senesced
    double senescence_root = kSeneRoot * Root;           // Mg / ha, amount of root senesced
    double senescence_rhizome = kSeneRhizome * Rhizome;  // Mg / ha, amount of rhizome senesced

    // change in leaf biomass = minus amount senesced + new leaf tissue from
    // remobilized amount (Allows for leaves to start senescing while new leaves
    // are still being produced).
    double dLeaf = -senescence_leaf + kLeaf * senescence_leaf * remobilization_fraction;  // Mg / ha

    // change in amount of leaf litter
    double dLeafLitter = senescence_leaf * (1 - remobilization_fraction);  // Mg / ha

    // change in stem biomass = minus amount senesced + new stem tissue from
    // remobilized leaf fraction
    double dStem = -senescence_stem + kStem * senescence_leaf * remobilization_fraction;  // Mg / ha

    double dStemLitter = senescence_stem;  // Mg / ha, change in amount of stem litter

    // change in root biomass = minus amount senesced + new root tissue from remobilized
    // leaf fraction
    double dRoot = -senescence_root + kRoot * senescence_leaf * remobilization_fraction;  // Mg / ha

    double dRootLitter = senescence_root;  // Mg / ha, change in amount of root litter

    // change in rhizome biomass = minus amount senesced + new rhizome from remobilized
    // leaf fraction
    double dRhizome = -senescence_rhizome + kRhizome * senescence_leaf * remobilization_fraction;  // Mg / ha
    double dRhizomeLitter = senescence_rhizome;                                                    // Mg / ha, change in rhizome litter

    // change in grain biomass = new grain from remobilized leaf fraction.
    // currently do not include grain senescence.
    double dGrain = kGrain * senescence_leaf * remobilization_fraction;  // Mg / ha

    // change in shell biomass = new shell from remobilized leaf fraction.
    // currently do not include shell senescence.
    double dShell = kShell * senescence_leaf * remobilization_fraction;  // Mg / ha

    update(Leaf_op, dLeaf);                    // Mg / ha
    update(Stem_op, dStem);                    // Mg / ha
    update(Root_op, dRoot);                    // Mg / ha
    update(Rhizome_op, dRhizome);              // Mg / ha
    update(Grain_op, dGrain);                  // Mg / ha
    update(Shell_op, dShell);                  // Mg / ha
    update(LeafLitter_op, dLeafLitter);        // Mg / ha
    update(StemLitter_op, dStemLitter);        // Mg / ha
    update(RootLitter_op, dRootLitter);        // Mg / ha
    update(RhizomeLitter_op, dRhizomeLitter);  // Mg / ha
}

}  // namespace standardBML
#endif
