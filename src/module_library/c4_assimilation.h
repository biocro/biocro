#ifndef C4_ASSIMILATION_H
#define C4_ASSIMILATION_H

#include "../modules.h"
#include "../state_map.h"
#include "c4photo.h"

/**
 * @class c4_assimilation
 *
 * @brief Calculates net assimilation, stomatal conductance, and
 * intercellular CO2 concentration for a C4 leaf using `c4photoC()`.
 *
 * ### Model overview
 *
 * The Ball-Berry stomatal conductance model and the Collatz et al. model for C4
 * photosynthesis, along with simple 1D gas diffusion, define a set of coupled
 * equations with three unknowns: net assimilation, intercellular CO2
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
 * - ``'alpha'`` for the initial slope of the photosynthetic light response
 * - ``'kparm'`` for the initial slope of the photosynthetic CO2 response
 * - ``'theta'`` for the first quadratic mixing parameter
 * - ``'beta'`` for the second quandratic mixing parameter
 * - ``'Rd'`` for the respiration rate at 25 degrees C
 * - ``'bb0'`` for the Ball-Berry intercept
 * - ``'bb1'`` for the Ball-Berry slope
 * - ``'Gs_min'`` for the minimum stomatal conductance (only used when applying water stress via stomatal conductance)
 * - ``'StomataWS'`` for the water stress factor
 * - ``'Catm'`` for the atmospheric CO2 concentration
 * - ``'atmospheric_pressure'`` for the local atmospheric pressure
 * - ``'water_stress_approach'`` indicates whether to apply water stress via assimilation (0) or stomatal conductance (1)
 * - ``'upperT'`` for the high temperature cutoff for rubisco activity
 * - ``'lowerT'`` for the low temperature cutoff for rubisco activity
 *
 * We use the following names for the model's output quantities:
 * - ``'Assim'`` for the net CO2 assimilation rate
 * - ``'Gs'`` for the stomatal conductance for H2O
 * - ``'Ci'`` for the intercellular CO2 concentration
 * - ``'GrossAssim'`` for the gross CO2 assimilation rate
 */
class c4_assimilation : public direct_module
{
   public:
    c4_assimilation(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          direct_module("c4_assimilation"),

          // Get pointers to input quantities
          Qp(get_input(input_quantities, "Qp")),
          Tleaf(get_input(input_quantities, "Tleaf")),
          rh(get_input(input_quantities, "rh")),
          vmax(get_input(input_quantities, "vmax")),
          alpha(get_input(input_quantities, "alpha")),
          kparm(get_input(input_quantities, "kparm")),
          theta(get_input(input_quantities, "theta")),
          beta(get_input(input_quantities, "beta")),
          Rd(get_input(input_quantities, "Rd")),
          bb0(get_input(input_quantities, "bb0")),
          bb1(get_input(input_quantities, "bb1")),
          Gs_min(get_input(input_quantities, "Gs_min")),
          StomataWS(get_input(input_quantities, "StomataWS")),
          Catm(get_input(input_quantities, "Catm")),
          atmospheric_pressure(get_input(input_quantities, "atmospheric_pressure")),
          water_stress_approach(get_input(input_quantities, "water_stress_approach")),
          upperT(get_input(input_quantities, "upperT")),
          lowerT(get_input(input_quantities, "lowerT")),

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
    double const& alpha;
    double const& kparm;
    double const& theta;
    double const& beta;
    double const& Rd;
    double const& bb0;
    double const& bb1;
    double const& Gs_min;
    double const& StomataWS;
    double const& Catm;
    double const& atmospheric_pressure;
    double const& water_stress_approach;
    double const& upperT;
    double const& lowerT;

    // Pointers to output quantities
    double* Assim_op;
    double* Gs_op;
    double* Ci_op;
    double* GrossAssim_op;

    // Main operation
    void do_operation() const;
};

string_vector c4_assimilation::get_inputs()
{
    return {
        "Qp",                     // micromol / m^2 / s
        "Tleaf",                  // degrees C
        "rh",                     // dimensionless
        "vmax",                   // micromol / m^2 / s
        "alpha",                  // mol / mol
        "kparm",                  // mol / mol
        "theta",                  // dimensionless
        "beta",                   // dimensionless
        "Rd",                     // micromol / m^2 / s
        "bb0",                    // mol / m^2 / s
        "bb1",                    // dimensionless
        "Gs_min",                 // mol / m^2 / s
        "StomataWS",              // dimensionless
        "Catm",                   // micromol / mol
        "atmospheric_pressure",   // Pa
        "water_stress_approach",  // dimensionless
        "upperT",                 // degrees C
        "lowerT"                  // degrees C
    };
}

string_vector c4_assimilation::get_outputs()
{
    return {
        "Assim",      // micromol / m^2 / s
        "Gs",         // millimol / m^2 / s
        "Ci",         // micromol / mol
        "GrossAssim"  // micromol / m^2 / s
    };
}

void c4_assimilation::do_operation() const
{
    c4_str c4_results = c4photoC(
        Qp,
        Tleaf,
        rh,
        vmax,
        alpha,
        kparm,
        theta,
        beta,
        Rd,
        bb0,
        bb1,
        Gs_min,
        StomataWS,
        Catm,
        atmospheric_pressure,
        water_stress_approach,
        upperT,
        lowerT);

    // Update the output quantity list
    update(Assim_op, c4_results.Assim);
    update(Gs_op, c4_results.Gs);
    update(Ci_op, c4_results.Ci);
    update(GrossAssim_op, c4_results.GrossAssim);
}

#endif
