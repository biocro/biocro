#ifndef PARTITIONING_GROWTH_CALCULATOR_H
#define PARTITIONING_GROWTH_CALCULATOR_H

#include "../modules.h"

/**
 * @class partitioning_growth_calculator
 * 
 * @brief Records new tissue derived from assimilation.
 * 
 * NOTE: This approach record new tissue derived from assimilation in the new*col arrays, but it doesn't
 * record any new tissue derived from reallocation from other tissues, e.g., from rhizomes to the rest of the plant.
 * Since it's not recorded, that part will never senesce.
 * Also, the partitioning coefficiencts (kLeaf, kRoot, etc.) must be set to 0 for a long enough time
 * at the end of the season for all of the tissue to senesce.
 * This doesn't seem like a good approach.
 */
class partitioning_growth_calculator : public SteadyModule
{
   public:
    partitioning_growth_calculator(const std::unordered_map<std::string, double>* input_parameters,
                                   std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("partitioning_growth_calculator"),
          // Get pointers to input parameters
          kLeaf_ip(get_ip(input_parameters, "kLeaf")),
          kStem_ip(get_ip(input_parameters, "kStem")),
          kRoot_ip(get_ip(input_parameters, "kRoot")),
          kRhizome_ip(get_ip(input_parameters, "kRhizome")),
          canopy_assimilation_rate_ip(get_ip(input_parameters, "canopy_assimilation_rate")),
          LeafWS_ip(get_ip(input_parameters, "LeafWS")),
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
    const double* LeafWS_ip;
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

std::vector<std::string> partitioning_growth_calculator::get_inputs()
{
    return {
        "kLeaf",
        "kStem",
        "kRoot",
        "kRhizome",
        "canopy_assimilation_rate",
        "LeafWS",
        "mrc1",
        "mrc2",
        "temp"};
}

std::vector<std::string> partitioning_growth_calculator::get_outputs()
{
    return {
        "newLeafcol",
        "newStemcol",
        "newRootcol",
        "newRhizomecol"};
}

void partitioning_growth_calculator::do_operation() const
{
    // Collect inputs and make calculations

    double kLeaf = *kLeaf_ip;
    double kStem = *kStem_ip;
    double kRoot = *kRoot_ip;
    double kRhizome = *kRhizome_ip;
    double canopy_assimilation_rate = *canopy_assimilation_rate_ip;
    double LeafWS = *LeafWS_ip;
    double mrc1 = *mrc1_ip;
    double mrc2 = *mrc2_ip;
    double temp = *temp_ip;

    double newLeafcol, newStemcol, newRootcol, newRhizomecol;

    // Calculate the amount of new leaf produced
    if (kLeaf > 0) {
        // The major effect of water stress is on leaf expansion rate. See Boyer (1970)
        //  Plant. Phys. 46, 233-235. For this the water stress coefficient is different
        //  for leaf and vmax.
        newLeafcol = canopy_assimilation_rate * kLeaf * LeafWS;

        // Tissue respiration. See Amthor (1984) PCE 7, 561-
        newLeafcol = resp(newLeafcol, mrc1, temp);

        // It makes sense to use i because when kLeaf
        //  is negative no new leaf is being accumulated
        //  and thus would not be subjected to senescence.
        // Note: what is 'i'??? (EBL)
    } else {
        newLeafcol = 0.0;
    }

    // Calculate the amount of new stem produced
    if (kStem >= 0) {
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

    // Grain has no respiration or senescence at the moment, so we don't need to calculate
    //  the amount of new grain here

    // Update the output parameter list
    update(newLeafcol_op, newLeafcol);
    update(newStemcol_op, newStemcol);
    update(newRootcol_op, newRootcol);
    update(newRhizomecol_op, newRhizomecol);
}

#endif
