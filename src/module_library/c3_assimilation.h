#ifndef C3_ASSIMILATION_H
#define C3_ASSIMILATION_H

#include "../modules.h"
#include "../state_map.h"
#include "c3photo.hpp"

/**
 * @class c3_assimilation
 *
 * @brief Calculates net assimilation, stomatal conductance, and
 * intercellular CO2 concentration for a C3 leaf using `c3photoC()`.
 *
 * ### Model overview
 *
 * The Ball-Berry stomatal conductance model and the Farquar-von-Cammerer-Berry
 * model for C3 photosynthesis, along with simple 1D gas diffusion, define a set
 * of coupled equations with three unknowns: net assimilation, intercellular CO2
 * concetration, and stomatal conductance. This function attempts to numerically
 * determine these unknowns using a fixed-point iteration method. Beware: the
 * code often fails to find the true solution, and some parts of the models are
 * implemented incorrectly.
 *
 * See the original WIMOVAC paper for a description of the assimilation model
 * and additional citations: [Humphries, S. W. & Long, S. P. Bioinformatics
 * 11, 361–371 (1995)](http://dx.doi.org/10.1093/bioinformatics/11.4.361)
 *
 * ### BioCro module implementation
 *
 * In BioCro, we use the following names for this model's input quantities:
 * - ``'Qp'`` for the incident quantum flux density of photosynthetically active radiation
 * - ``'Tleaf'`` for the leaf temperature
 * - ``'rh'`` for the atmospheric relative humidity
 * - ``'vmax'`` for the rubisco carboxylation rate at 25 degrees C
 * - ``'jmax'`` for the electron transport rate
 * - ``'tpu_rate_max'`` for the maximum triphosphate utilization rate
 * - ``'Rd'`` for the respiration rate at 25 degrees C
 * - ``'bb0'`` for the Ball-Berry intercept
 * - ``'bb1'`` for the Ball-Berry slope
 * - ``'Gs_min'`` for the minimum stomatal conductance (only used when applying water stress via stomatal conductance)
 * - ``'Catm'`` for the atmospheric CO2 concentration
 * - ``'atmospheric_pressure'`` for the local atmospheric_pressure
 * - ``'O2'`` for the atmospheric O2 concentration
 * - ``'theta'`` for the ???
 * - ``'StomataWS'`` for the water stress factor
 * - ``'water_stress_approach'`` indicates whether to apply water stress via assimilation (0) or stomatal conductance (1)
 * - ``'electrons_per_carboxylation'`` for the number of electrons per carboxylation event
 * - ``'electrons_per_oxygenation'`` for the number of electrons per oxygenation event
 *
 * We use the following names for the model's output quantities:
 * - ``'Assim'`` for the net CO2 assimilation rate
 * - ``'Gs'`` for the stomatal conductance for H2O
 * - ``'Ci'`` for the intercellular CO2 concentration
 * - ``'GrossAssim'`` for the gross CO2 assimilation rate
 */
class c3_assimilation : public direct_module
{
   public:
    c3_assimilation(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          direct_module("c3_assimilation"),

          // Get pointers to input quantities
          Qp(get_input(input_quantities, "Qp")),
          Tleaf(get_input(input_quantities, "Tleaf")),
          rh(get_input(input_quantities, "rh")),
          vmax(get_input(input_quantities, "vmax")),
          jmax(get_input(input_quantities, "jmax")),
          tpu_rate_max(get_input(input_quantities, "tpu_rate_max")),
          Rd(get_input(input_quantities, "Rd")),
          bb0(get_input(input_quantities, "bb0")),
          bb1(get_input(input_quantities, "bb1")),
          Gs_min(get_input(input_quantities, "Gs_min")),
          Catm(get_input(input_quantities, "Catm")),
          atmospheric_pressure(get_input(input_quantities, "atmospheric_pressure")),
          O2(get_input(input_quantities, "O2")),
          theta(get_input(input_quantities, "theta")),
          StomataWS(get_input(input_quantities, "StomataWS")),
          water_stress_approach(get_input(input_quantities, "water_stress_approach")),
          electrons_per_carboxylation(get_input(input_quantities, "electrons_per_carboxylation")),
          electrons_per_oxygenation(get_input(input_quantities, "electrons_per_oxygenation")),

          // Get pointers to output quantities
          Assim_op(get_op(output_quantities, "Assim")),
          Gs_op(get_op(output_quantities, "Gs")),
          Ci_op(get_op(output_quantities, "Ci")),
          GrossAssim_op(get_op(output_quantities, "GrossAssim"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // References to input quantities
    double const& Qp;
    double const& Tleaf;
    double const& rh;
    double const& vmax;
    double const& jmax;
    double const& tpu_rate_max;
    double const& Rd;
    double const& bb0;
    double const& bb1;
    double const& Gs_min;
    double const& Catm;
    double const& atmospheric_pressure;
    double const& O2;
    double const& theta;
    double const& StomataWS;
    double const& water_stress_approach;
    double const& electrons_per_carboxylation;
    double const& electrons_per_oxygenation;

    // Pointers to output quantities
    double* Assim_op;
    double* Gs_op;
    double* Ci_op;
    double* GrossAssim_op;

    // Main operation
    void do_operation() const;
};

string_vector c3_assimilation::get_inputs()
{
    return {
        "Qp",                           // micromol / m^2 / s
        "Tleaf",                        // degrees C
        "rh",                           // dimensionless
        "vmax",                         // micromol / m^2 / s
        "jmax",                         // micromol / m^2 / s
        "tpu_rate_max",                 // micromol / m^2 / s
        "Rd",                           // micromol / m^2 / s
        "bb0",                          // mol / m^2 / s
        "bb1",                          // dimensionless
        "Gs_min",                       // mol / m^2 / s
        "Catm",                         // micromol / mol
        "atmospheric_pressure",         // Pa
        "O2",                           // millimol / mol
        "theta",                        // dimensionless
        "StomataWS",                    // dimensionless
        "water_stress_approach",        // dimensionless
        "electrons_per_carboxylation",  // self-explanatory units
        "electrons_per_oxygenation"     // self-explanatory units
    };
}

string_vector c3_assimilation::get_outputs()
{
    return {
        "Assim",      // micromol / m^2 / s
        "Gs",         // millimol / m^2 / s
        "Ci",         // micromol / mol
        "GrossAssim"  // micromol / m^2 / s
    };
}

void c3_assimilation::do_operation() const
{
    c3_str c3_results = c3photoC(
        Qp,
        Tleaf,
        rh,
        vmax,
        jmax,
        tpu_rate_max,
        Rd,
        bb0,
        bb1,
        Gs_min,
        Catm,
        atmospheric_pressure,
        O2,
        theta,
        StomataWS,
        water_stress_approach,
        electrons_per_carboxylation,
        electrons_per_oxygenation);

    // Update the output quantity list
    update(Assim_op, c3_results.Assim);
    update(Gs_op, c3_results.Gs);
    update(Ci_op, c3_results.Ci);
    update(GrossAssim_op, c3_results.GrossAssim);
}

#endif
