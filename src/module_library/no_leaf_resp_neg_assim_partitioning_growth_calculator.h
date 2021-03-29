#ifndef NO_LEAF_RESP_NEG_ASSIM_PARTITIONING_GROWTH_CALCULATOR_H
#define NO_LEAF_RESP_NEG_ASSIM_PARTITIONING_GROWTH_CALCULATOR_H

#include "../modules.h"
#include "../state_map.h"

/**
 * @class no_leaf_resp_neg_assim_partitioning_growth_calculator
 *
 * @brief Records new tissue derived from assimilation. This module is mostly the same as the
 * partitioning_growth_module except that leaf respiration is treated differently. CanopyA already
 * includes losses from leaf respiration.
 *
 * NOTE: This approach records new tissue derived from assimilation in the new*col arrays, but these variables
 * don't include new tissue derived from reallocation from other tissues.
 *
 */
class no_leaf_resp_neg_assim_partitioning_growth_calculator : public SteadyModule
{
   public:
    no_leaf_resp_neg_assim_partitioning_growth_calculator(
        const state_map* input_parameters,
        state_map* output_parameters)
        : SteadyModule{"no_leaf_resp_neg_assim_partitioning_growth_calculator"},

          // Get references to input parameters
          kLeaf{get_input(input_parameters, "kLeaf")},
          kStem{get_input(input_parameters, "kStem")},
          kRoot{get_input(input_parameters, "kRoot")},
          kRhizome{get_input(input_parameters, "kRhizome")},
          kGrain{get_input(input_parameters, "kGrain")},
          canopy_assimilation_rate{get_input(input_parameters, "canopy_assimilation_rate")},
          mrc1{get_input(input_parameters, "mrc1")},
          mrc2{get_input(input_parameters, "mrc2")},
          temp{get_input(input_parameters, "temp")},

          // Get pointers to output parameters
          newLeafcol_op{get_op(output_parameters, "newLeafcol")},
          newStemcol_op{get_op(output_parameters, "newStemcol")},
          newRootcol_op{get_op(output_parameters, "newRootcol")},
          newRhizomecol_op{get_op(output_parameters, "newRhizomecol")},
          newGraincol_op{get_op(output_parameters, "newGraincol")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // References to input parameters
    const double& kLeaf;
    const double& kStem;
    const double& kRoot;
    const double& kRhizome;
    const double& kGrain;
    const double& canopy_assimilation_rate;
    const double& mrc1;
    const double& mrc2;
    const double& temp;

    // Pointers to output parameters
    double* newLeafcol_op;
    double* newStemcol_op;
    double* newRootcol_op;
    double* newRhizomecol_op;
    double* newGraincol_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};

string_vector no_leaf_resp_neg_assim_partitioning_growth_calculator::get_inputs()
{
    return {
        "kLeaf",                     // dimensionless
        "kStem",                     // dimensionless
        "kRoot",                     // dimensionless
        "kRhizome",                  // dimensionless
        "kGrain",                    // dimensionless
        "canopy_assimilation_rate",  // Mg / ha / hour
        "mrc1",                      // hour^-1
        "mrc2",                      // hour^-1
        "temp"                       // degrees C
    };
}

string_vector no_leaf_resp_neg_assim_partitioning_growth_calculator::get_outputs()
{
    return {
        "newLeafcol",     // Mg / ha / hour
        "newStemcol",     // Mg / ha / hour
        "newRootcol",     // Mg / ha / hour
        "newRhizomecol",  // Mg / ha / hour
        "newGraincol"     // Mg / ha / hour
    };
}

void no_leaf_resp_neg_assim_partitioning_growth_calculator::do_operation() const
{
    double newLeafcol, newStemcol, newRootcol, newRhizomecol, newGraincol;

    // Calculate the amount of new leaf produced
    if (kLeaf > 0) {
        newLeafcol = canopy_assimilation_rate * kLeaf;
    } else {
        newLeafcol = 0.0;
    }

    // Calculate the amount of new stem produced
    if (kStem > 0) {
        newStemcol = canopy_assimilation_rate * kStem;
        newStemcol = resp(newStemcol, mrc1, temp);
    } else {
        newStemcol = 0.0;
    }

    // Calculate the amount of new root produced
    if (kRoot > 0) {
        newRootcol = canopy_assimilation_rate * kRoot;
        newRootcol = resp(newRootcol, mrc2, temp);
    } else {
        newRootcol = 0.0;
    }

    // Calculate the amount of new rhizome produced
    if (kRhizome > 0) {
        newRhizomecol = canopy_assimilation_rate * kRhizome;
        newRhizomecol = resp(newRhizomecol, mrc2, temp);
    } else {
        newRhizomecol = 0.0;
    }

    // Calculate the amount of new grain produced
    if (kGrain > 0) {
        newGraincol = canopy_assimilation_rate * kGrain;
    } else {
        newGraincol = 0.0;
    }

    // Update the output parameter list
    update(newLeafcol_op, newLeafcol);
    update(newStemcol_op, newStemcol);
    update(newRootcol_op, newRootcol);
    update(newRhizomecol_op, newRhizomecol);
    update(newGraincol_op, newGraincol);
}

#endif
