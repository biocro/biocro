#ifndef ED_C4_LEAF_PHOTOSYNTHESIS_H
#define ED_C4_LEAF_PHOTOSYNTHESIS_H

#include "ed_leaf_photosynthesis.h"
#include "ed_leaf_photosynthesis_nr.h"  // for get_reference_inputs and get_reference_outputs

/**
 * @class ed_c4_leaf_photosynthesis
 * 
 * @brief Module describing photosynthesis at the leaf level using
 * the default sub-modules defined by ed_leaf_photosynthesis::reference_module_names.
 * The Newton-Raphson method is used to find self-consistent values for 
 * stomatal conductance, boundary layer conductance, net assimilation,
 * and leaf temperature.
 */
class ed_c4_leaf_photosynthesis : public ed_leaf_photosynthesis_nr::module_base
{
   public:
    ed_c4_leaf_photosynthesis(
        const state_map* input_parameters,
        state_map* output_parameters)
        : ed_leaf_photosynthesis_nr::module_base("ed_c4_leaf_photosynthesis",
                                                 ed_leaf_photosynthesis::reference_module_names,
                                                 input_parameters,
                                                 output_parameters)
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
};

// Get inputs from the typical modules
string_vector ed_c4_leaf_photosynthesis::get_inputs()
{
    return ed_leaf_photosynthesis::get_reference_inputs();
}

// Get outputs from the typical modules
string_vector ed_c4_leaf_photosynthesis::get_outputs()
{
    return ed_leaf_photosynthesis::get_reference_outputs();
}

#endif
