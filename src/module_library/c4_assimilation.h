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
 * determine these unknowns using the secant method.
 *
 * See the original WIMOVAC paper for a description of the assimilation model
 * and additional citations: [Humphries, S. W. & Long, S. P. Bioinformatics
 * 11, 361–371 (1995)](https://dx.doi.org/10.1093/bioinformatics/11.4.361)
 *
 * ### BioCro module implementation
 *
 * In BioCro, we use the following names for this model's input quantities:
 * - ``'alpha'`` for the initial slope of the photosynthetic light response
 * - ``'atmospheric_pressure'`` for the local atmospheric pressure
 * - ``'b0'`` for the Ball-Berry intercept
 * - ``'b1'`` for the Ball-Berry slope
 * - ``'beta'`` for the second quandratic mixing parameter
 * - ``'Catm'`` for the atmospheric CO2 concentration
 * - ``'gbw'`` for the boundary layer conductance to water vapor. For an
 *   isolated leaf, this should be the leaf boundary layer conductance; for a
 *   leaf within a canopy, this should be the total conductance including the
 *   leaf and canopy boundary layer conductances.
 * - ``'Gs_min'`` for the minimum stomatal conductance (only used when applying water stress via stomatal conductance)
 * - ``'kparm'`` for the initial slope of the photosynthetic CO2 response
 * - ``'lowerT'`` for the low temperature cutoff for rubisco activity
 * - ``'Qp'`` for the incident quantum flux density of photosynthetically active radiation
 * - ``'rh'`` for the atmospheric relative humidity
 * - ``'RL_at_25'`` for the rate of non-photorespiratory CO2 release in the light at 25 degrees C
 * - ``'StomataWS'`` for the water stress factor
 * - ``'temp'`` for the ambient temperature
 * - ``'theta'`` for the first quadratic mixing parameter
 * - ``'Tleaf'`` for the leaf temperature
 * - ``'upperT'`` for the high temperature cutoff for rubisco activity
 * - ``'Vcmax_at_25'`` for the rubisco carboxylation rate at 25 degrees C
 *
 * We use the following names for the model's output quantities:
 * - ``'Assim'`` for the net CO2 assimilation rate
 * - ``'Assim_check'`` for an indicator of whether the loop converged
 * - ``'Assim_conductance'`` for the maximum net assimilation rate limited by conductance
 * - ``'Ci'`` for the intercellular CO2 concentration
 * - ``'Cs'`` for the CO2 concentration at the leaf surface
 * - ``'GrossAssim'`` for the gross CO2 assimilation rate
 * - ``'Gs'`` for the stomatal conductance for H2O
 * - ``'RHs'`` for the relative humidity at the leaf surface
 * - ``'RL'`` for the rate of non-photorespiratory CO2 release in the light
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
          alpha{get_input(input_quantities, "alpha")},
          atmospheric_pressure{get_input(input_quantities, "atmospheric_pressure")},
          b0{get_input(input_quantities, "b0")},
          b1{get_input(input_quantities, "b1")},
          beta{get_input(input_quantities, "beta")},
          Catm{get_input(input_quantities, "Catm")},
          gbw{get_input(input_quantities, "gbw")},
          Gs_min{get_input(input_quantities, "Gs_min")},
          kparm{get_input(input_quantities, "kparm")},
          lowerT{get_input(input_quantities, "lowerT")},
          Qp{get_input(input_quantities, "Qp")},
          rh{get_input(input_quantities, "rh")},
          RL_at_25{get_input(input_quantities, "RL_at_25")},
          StomataWS{get_input(input_quantities, "StomataWS")},
          Tambient{get_input(input_quantities, "temp")},
          theta{get_input(input_quantities, "theta")},
          Tleaf{get_input(input_quantities, "Tleaf")},
          upperT{get_input(input_quantities, "upperT")},
          Vcmax_at_25{get_input(input_quantities, "Vcmax_at_25")},

          // Get pointers to output quantities
          Assim_op{get_op(output_quantities, "Assim")},
          Assim_check_op{get_op(output_quantities, "Assim_check")},
          Assim_conductance_op{get_op(output_quantities, "Assim_conductance")},
          Ci_op{get_op(output_quantities, "Ci")},
          Cs_op{get_op(output_quantities, "Cs")},
          GrossAssim_op{get_op(output_quantities, "GrossAssim")},
          Gs_op{get_op(output_quantities, "Gs")},
          RHs_op{get_op(output_quantities, "RHs")},
          RL_op{get_op(output_quantities, "RL")},
          Rp_op{get_op(output_quantities, "Rp")},
          iterations_op{get_op(output_quantities, "iterations")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "c4_assimilation"; }

   private:
    // References to input quantities
    double const& alpha;
    double const& atmospheric_pressure;
    double const& b0;
    double const& b1;
    double const& beta;
    double const& Catm;
    double const& gbw;
    double const& Gs_min;
    double const& kparm;
    double const& lowerT;
    double const& Qp;
    double const& rh;
    double const& RL_at_25;
    double const& StomataWS;
    double const& Tambient;
    double const& theta;
    double const& Tleaf;
    double const& upperT;
    double const& Vcmax_at_25;

    // Pointers to output quantities
    double* Assim_op;
    double* Assim_check_op;
    double* Assim_conductance_op;
    double* Ci_op;
    double* Cs_op;
    double* GrossAssim_op;
    double* Gs_op;
    double* RHs_op;
    double* RL_op;
    double* Rp_op;
    double* iterations_op;

    // Main operation
    void do_operation() const;
};

string_vector c4_assimilation::get_inputs()
{
    return {
        "alpha",                 // mol / mol
        "atmospheric_pressure",  // Pa
        "b0",                    // mol / m^2 / s
        "b1",                    // dimensionless
        "beta",                  // dimensionless
        "Catm",                  // micromol / mol
        "gbw",                   // mol / m^2 / s
        "Gs_min",                // mol / m^2 / s
        "kparm",                 // mol / mol
        "lowerT",                // degrees C
        "Qp",                    // micromol / m^2 / s
        "rh",                    // dimensionless
        "RL_at_25",              // micromol / m^2 / s
        "StomataWS",             // dimensionless
        "temp",                  // degrees C
        "theta",                 // dimensionless
        "Tleaf",                 // degrees C
        "upperT",                // degrees C
        "Vcmax_at_25"            // micromol / m^2 / s
    };
}

string_vector c4_assimilation::get_outputs()
{
    return {
        "Assim",              // micromol / m^2 / s
        "Assim_check",        // micromol / m^2 / s
        "Assim_conductance",  // micromol / m^2 / s
        "Ci",                 // micromol / mol
        "Cs",                 // micromol / m^2 / s
        "GrossAssim",         // micromol / m^2 / s
        "Gs",                 // mol / m^2 / s
        "RHs",                // dimensionless from Pa / Pa
        "RL",                 // micromol / m^2 / s
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
        Vcmax_at_25,
        alpha,
        kparm,
        theta,
        beta,
        RL_at_25,
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
    update(Assim_check_op, c4_results.Assim_check);
    update(Assim_conductance_op, c4_results.Assim_conductance);
    update(Ci_op, c4_results.Ci);
    update(Cs_op, c4_results.Cs);
    update(GrossAssim_op, c4_results.GrossAssim);
    update(Gs_op, c4_results.Gs);
    update(RHs_op, c4_results.RHs);
    update(RL_op, c4_results.RL);
    update(Rp_op, c4_results.Rp);
    update(iterations_op, c4_results.iterations);
}

}  // namespace standardBML
#endif
