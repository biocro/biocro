#include "rue_leaf_photosynthesis.h"

using standardBML::rue_leaf_photosynthesis;

string_vector rue_leaf_photosynthesis::get_inputs()
{
    return {
        "alpha_rue",     // dimensionless
        "incident_ppfd"  // micromol / (m^2 leaf) / s
    };
}

string_vector rue_leaf_photosynthesis::get_outputs()
{
    return {
        "Assim",       // micromol / m^2 /s
        "GrossAssim",  // micromol / m^2 /s
        "Gs",          // mol / m^2 / s
        "RL",          // micromol / m^2 / s
        "Rp",          // micromol / m^2 / s
        "TransR"       // mmol / m^2 / s
    };
}

void rue_leaf_photosynthesis::do_operation() const
{
    // Calculate the net CO2 assimilation rate using the RUE assumption
    double const An = incident_ppfd * alpha_rue;  // micromol / m^2 / s

    // Update the outputs
    update(Assim_op, An);
    update(GrossAssim_op, An);
    update(Gs_op, 0.0);
    update(RL_op, 0.0);
    update(Rp_op, 0.0);
    update(TransR_op, 0.0);
}
