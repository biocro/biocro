#ifndef C4_ASSIMILATION_H
#define C4_ASSIMILATION_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "c4photo.h"

namespace standardBML
{
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
 * 11, 361–371 (1995)](https://dx.doi.org/10.1093/bioinformatics/11.4.361)
 *
 * ### BioCro module implementation
 *
 * In BioCro, we use the following names for this model's input quantities:
 * - ``'Qp'`` for the incident quantum flux density of photosynthetically active radiation
 * - ``'Tleaf'`` for the leaf temperature
 * - ``'temp'`` for the ambient temperature
 * - ``'rh'`` for the atmospheric relative humidity
 * - ``'vmax'`` for the rubisco carboxylation rate at 25 degrees C
 * - ``'alpha'`` for the initial slope of the photosynthetic light response
 * - ``'kparm'`` for the initial slope of the photosynthetic CO2 response
 * - ``'theta'`` for the first quadratic mixing parameter
 * - ``'beta'`` for the second quandratic mixing parameter
 * - ``'Rd'`` for the respiration rate at 25 degrees C
 * - ``'b0'`` for the Ball-Berry intercept
 * - ``'b1'`` for the Ball-Berry slope
 * - ``'Gs_min'`` for the minimum stomatal conductance (only used when applying water stress via stomatal conductance)
 * - ``'StomataWS'`` for the water stress factor
 * - ``'Catm'`` for the atmospheric CO2 concentration
 * - ``'atmospheric_pressure'`` for the local atmospheric pressure
 * - ``'upperT'`` for the high temperature cutoff for rubisco activity
 * - ``'lowerT'`` for the low temperature cutoff for rubisco activity
 * - ``'gbw'`` for the boundary layer conductance to water vapor. For an
 *   isolated leaf, this should be the leaf boundary layer conductance; for a
 *   leaf within a canopy, this should be the total conductance including the
 *   leaf and canopy boundary layer conductances.
 *
 * We use the following names for the model's output quantities:
 * - ``'Assim'`` for the net CO2 assimilation rate
 * - ``'Gs'`` for the stomatal conductance for H2O
 * - ``'Cs'`` for the CO2 concentration at the leaf surface
 * - ``'RHs'`` for the relative humidity at the leaf surface
 * - ``'Ci'`` for the intercellular CO2 concentration
 * - ``'GrossAssim'`` for the gross CO2 assimilation rate
 * - ``'Assim_conductance'`` for the maximum net assimilation rate limited by conductance
 * - ``'Rp'`` for the rate of photorespiration
 * - ``'iterations'`` for the number of iterations required for the convergence loop
 */
class c4_assimilation : public direct_module
{
   public:
    c4_assimilation(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          Qp{get_input(input_quantities, "Qp")},
          Tleaf{get_input(input_quantities, "Tleaf")},
          Tambient{get_input(input_quantities, "temp")},
          rh{get_input(input_quantities, "rh")},
          vmax{get_input(input_quantities, "vmax")},
          alpha{get_input(input_quantities, "alpha")},
          kparm{get_input(input_quantities, "kparm")},
          theta{get_input(input_quantities, "theta")},
          beta{get_input(input_quantities, "beta")},
          Rd{get_input(input_quantities, "Rd")},
          b0{get_input(input_quantities, "b0")},
          b1{get_input(input_quantities, "b1")},
          Gs_min{get_input(input_quantities, "Gs_min")},
          StomataWS{get_input(input_quantities, "StomataWS")},
          Catm{get_input(input_quantities, "Catm")},
          atmospheric_pressure{get_input(input_quantities, "atmospheric_pressure")},
          upperT{get_input(input_quantities, "upperT")},
          lowerT{get_input(input_quantities, "lowerT")},
          gbw{get_input(input_quantities, "gbw")},

          // Get pointers to output quantities
          Assim_op{get_op(output_quantities, "Assim")},
          Gs_op{get_op(output_quantities, "Gs")},
          Cs_op{get_op(output_quantities, "Cs")},
          RHs_op{get_op(output_quantities, "RHs")},
          Ci_op{get_op(output_quantities, "Ci")},
          GrossAssim_op{get_op(output_quantities, "GrossAssim")},
          Assim_conductance_op{get_op(output_quantities, "Assim_conductance")},
          Rp_op{get_op(output_quantities, "Rp")},
          iterations_op{get_op(output_quantities, "iterations")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "c4_assimilation"; }

   private:
    // References to input quantities
    double const& Qp;
    double const& Tleaf;
    double const& Tambient;
    double const& rh;
    double const& vmax;
    double const& alpha;
    double const& kparm;
    double const& theta;
    double const& beta;
    double const& Rd;
    double const& b0;
    double const& b1;
    double const& Gs_min;
    double const& StomataWS;
    double const& Catm;
    double const& atmospheric_pressure;
    double const& upperT;
    double const& lowerT;
    double const& gbw;

    // Pointers to output quantities
    double* Assim_op;
    double* Gs_op;
    double* Cs_op;
    double* RHs_op;
    double* Ci_op;
    double* GrossAssim_op;
    double* Assim_conductance_op;
    double* Rp_op;
    double* iterations_op;

    // Main operation
    void do_operation() const;
};

string_vector c4_assimilation::get_inputs()
{
    return {
        "Qp",                    // micromol / m^2 / s
        "Tleaf",                 // degrees C
        "temp",                  // degrees C
        "rh",                    // dimensionless
        "vmax",                  // micromol / m^2 / s
        "alpha",                 // mol / mol
        "kparm",                 // mol / mol
        "theta",                 // dimensionless
        "beta",                  // dimensionless
        "Rd",                    // micromol / m^2 / s
        "b0",                    // mol / m^2 / s
        "b1",                    // dimensionless
        "Gs_min",                // mol / m^2 / s
        "StomataWS",             // dimensionless
        "Catm",                  // micromol / mol
        "atmospheric_pressure",  // Pa
        "upperT",                // degrees C
        "lowerT",                // degrees C
        "gbw"                    // mol / m^2 / s
    };
}

string_vector c4_assimilation::get_outputs()
{
    return {
        "Assim",              // micromol / m^2 / s
        "Gs",                 // millimol / m^2 / s
        "Cs",                 // micromol / m^2 / s
        "RHs",                // dimensionless from Pa / Pa
        "Ci",                 // micromol / mol
        "GrossAssim",         // micromol / m^2 / s
        "Assim_conductance",  // micromol / m^2 / s
        "Rp",                 // micromol / m^2 / s
        "iterations"          // not a physical quantity
    };
}

void c4_assimilation::do_operation() const
{
    photosynthesis_outputs c4_results = c4photoC(
        Qp,
        Tleaf,
        Tambient,
        rh,
        vmax,
        alpha,
        kparm,
        theta,
        beta,
        Rd,
        b0,
        b1,
        Gs_min,
        StomataWS,
        Catm,
        atmospheric_pressure,
        upperT,
        lowerT,
        gbw);

    // Update the output quantity list
    update(Assim_op, c4_results.Assim);
    update(Gs_op, c4_results.Gs);
    update(Cs_op, c4_results.Cs);
    update(RHs_op, c4_results.RHs);
    update(Ci_op, c4_results.Ci);
    update(GrossAssim_op, c4_results.GrossAssim);
    update(Assim_conductance_op, c4_results.Assim_conductance);
    update(Rp_op, c4_results.Rp);
    update(iterations_op, c4_results.iterations);
}

}  // namespace standardBML
#endif
