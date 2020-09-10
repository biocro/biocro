#ifndef NO_LEAF_RESP_PARTITIONING_GROWTH_CALCULATOR_H
#define NO_LEAF_RESP_PARTITIONING_GROWTH_CALCULATOR_H

#include "../modules.h"

/**
 * @class no_leaf_resp_partitioning_growth_calculator
 * 
 * @brief Records new tissue derived from assimilation. This module is mostly the same as the
 * partitioning_growth_module except that leaf respiration is treated differently. CanopyA already
 * includes losses from leaf respiration, so it should only be removed from leaf mass.
 * 
 * NOTE: This approach record new tissue derived from assimilation in the new*col arrays, but it doesn't
 * record any new tissue derived from reallocation from other tissues, e.g., from rhizomes to the rest of the plant.
 * Since it's not recorded, that part will never senesce.
 * Also, the partitioning coefficiencts (kLeaf, kRoot, etc.) must be set to 0 for a long enough time
 * at the end of the season for all of the tissue to senesce.
 * This doesn't seem like a good approach.
 */
class no_leaf_resp_partitioning_growth_calculator : public SteadyModule
{
   public:
    no_leaf_resp_partitioning_growth_calculator(const std::unordered_map<std::string, double>* input_parameters,
                                                std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("no_leaf_resp_partitioning_growth_calculator"),
          // Get pointers to input parameters
          kLeaf_ip(get_ip(input_parameters, "kLeaf")),
          kStem_ip(get_ip(input_parameters, "kStem")),
          kRoot_ip(get_ip(input_parameters, "kRoot")),
          kRhizome_ip(get_ip(input_parameters, "kRhizome")),
          canopy_assimilation_rate_ip(get_ip(input_parameters, "canopy_assimilation_rate")),
          mrc1_ip(get_ip(input_parameters, "mrc1")),
          mrc2_ip(get_ip(input_parameters, "mrc2")),
          temp_ip(get_ip(input_parameters, "temp")),
          // Get pointers to output parameters
          newLeafcol_op(get_op(output_parameters, "newLeafcol")),
          newStemcol_op(get_op(output_parameters, "newStemcol")),
          newRootcol_op(get_op(output_parameters, "newRootcol")),
          newRhizomecol_op(get_op(output_parameters, "newRhizomecol"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* kLeaf_ip;
    const double* kStem_ip;
    const double* kRoot_ip;
    const double* kRhizome_ip;
    const double* canopy_assimilation_rate_ip;
    const double* mrc1_ip;
    const double* mrc2_ip;
    const double* temp_ip;
    // Pointers to output parameters
    double* newLeafcol_op;
    double* newStemcol_op;
    double* newRootcol_op;
    double* newRhizomecol_op;
    // Main operation
    void do_operation() const;
};

std::vector<std::string> no_leaf_resp_partitioning_growth_calculator::get_inputs()
{
    return {
        "kLeaf",
        "kStem",
        "kRoot",
        "kRhizome",
        "canopy_assimilation_rate",
        "mrc1",
        "mrc2",
        "temp"};
}

std::vector<std::string> no_leaf_resp_partitioning_growth_calculator::get_outputs()
{
    return {
        "newLeafcol",
        "newStemcol",
        "newRootcol",
        "newRhizomecol"};
}

void no_leaf_resp_partitioning_growth_calculator::do_operation() const
{
    // Collect inputs and make calculations

    double kLeaf = *kLeaf_ip;
    double kStem = *kStem_ip;
    double kRoot = *kRoot_ip;
    double kRhizome = *kRhizome_ip;
    double canopy_assimilation_rate = *canopy_assimilation_rate_ip;
    double mrc1 = *mrc1_ip;
    double mrc2 = *mrc2_ip;
    double temp = *temp_ip;

    double newLeafcol, newStemcol, newRootcol, newRhizomecol;

    double nonleaf_carbon_flux;
    if (canopy_assimilation_rate < 0) {
        nonleaf_carbon_flux = 0.0;
    }

    else {
        nonleaf_carbon_flux = canopy_assimilation_rate;
    }

    // Calculate the amount of new leaf produced
    if (kLeaf > 0) {
        // If assimilation is negative then leaves are respiring more than photosynthesizing.
        if (canopy_assimilation_rate < 0) {
            newLeafcol = canopy_assimilation_rate;  // assimilation is negative here, so this removes leaf mass
        } else {
            newLeafcol = canopy_assimilation_rate * kLeaf;
        }
    } else {
        newLeafcol = 0.0;
    }

    // Calculate the amount of new stem produced
    if (kStem >= 0) {
        newStemcol = nonleaf_carbon_flux * kStem;
        newStemcol = resp(newStemcol, mrc1, temp);
    } else {
        newStemcol = 0.0;
    }

    // Calculate the amount of new root produced
    if (kRoot > 0) {
        newRootcol = nonleaf_carbon_flux * kRoot;
        newRootcol = resp(newRootcol, mrc2, temp);
    } else {
        newRootcol = 0.0;
    }

    // Calculate the amount of new rhizome produced
    if (kRhizome > 0) {
        newRhizomecol = nonleaf_carbon_flux * kRhizome;
        newRhizomecol = resp(newRhizomecol, mrc2, temp);
    } else {
        newRhizomecol = 0.0;
    }

    // Grain has no respiration or senescence at the moment, so we don't need to calculate
    //  the amount of new grain here

    // Update the output parameter list
    update(newLeafcol_op, newLeafcol);
    update(newStemcol_op, newStemcol);
    update(newRootcol_op, newRootcol);
    update(newRhizomecol_op, newRhizomecol);
}

#endif
