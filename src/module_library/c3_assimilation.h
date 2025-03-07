#ifndef C3_ASSIMILATION_H
#define C3_ASSIMILATION_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "c3_temperature_response.h"  // for c3_temperature_response_parameters
#include "c3photo.h"

namespace standardBML
{
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
 * 11, 361–371 (1995)](https://dx.doi.org/10.1093/bioinformatics/11.4.361)
 *
 * ### BioCro module implementation
 *
 * In BioCro, we use the following names for this model's input quantities:
 * - ``'Qabs'`` for the absorbed quantum flux density of photosynthetically active radiation
 * - ``'Tleaf'`` for the leaf temperature
 * - ``'temp'`` for the ambient temperature
 * - ``'rh'`` for the atmospheric relative humidity
 * - ``'vmax1'`` for the rubisco carboxylation rate at 25 degrees C
 * - ``'jmax'`` for the electron transport rate
 * - ``'tpu_rate_max'`` for the maximum triose phosphate utilization rate
 * - ``'Rd'`` for the respiration rate at 25 degrees C
 * - ``'b0'`` for the Ball-Berry intercept
 * - ``'b1'`` for the Ball-Berry slope
 * - ``'Gs_min'`` for the minimum stomatal conductance (only used when applying water stress via stomatal conductance)
 * - ``'Catm'`` for the atmospheric CO2 concentration
 * - ``'atmospheric_pressure'`` for the local atmospheric_pressure
 * - ``'O2'`` for the atmospheric O2 concentration
 * - ``'theta'`` for the ???
 * - ``'StomataWS'`` for the water stress factor
 * - ``'electrons_per_carboxylation'`` for the number of electrons per carboxylation event
 * - ``'electrons_per_oxygenation'`` for the number of electrons per oxygenation event
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
class c3_assimilation : public direct_module
{
   public:
    c3_assimilation(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          atmospheric_pressure{get_input(input_quantities, "atmospheric_pressure")},
          b0{get_input(input_quantities, "b0")},
          b1{get_input(input_quantities, "b1")},
          beta_PSII{get_input(input_quantities, "beta_PSII")},
          Catm{get_input(input_quantities, "Catm")},
          electrons_per_carboxylation{get_input(input_quantities, "electrons_per_carboxylation")},
          electrons_per_oxygenation{get_input(input_quantities, "electrons_per_oxygenation")},
          gbw{get_input(input_quantities, "gbw")},
          Gs_min{get_input(input_quantities, "Gs_min")},
          Gstar_c{get_input(input_quantities, "Gstar_c")},
          Gstar_Ea{get_input(input_quantities, "Gstar_Ea")},
          jmax{get_input(input_quantities, "jmax")},
          Jmax_c{get_input(input_quantities, "Jmax_c")},
          Jmax_Ea{get_input(input_quantities, "Jmax_Ea")},
          Kc_c{get_input(input_quantities, "Kc_c")},
          Kc_Ea{get_input(input_quantities, "Kc_Ea")},
          Ko_c{get_input(input_quantities, "Ko_c")},
          Ko_Ea{get_input(input_quantities, "Ko_Ea")},
          O2{get_input(input_quantities, "O2")},
          phi_PSII_0{get_input(input_quantities, "phi_PSII_0")},
          phi_PSII_1{get_input(input_quantities, "phi_PSII_1")},
          phi_PSII_2{get_input(input_quantities, "phi_PSII_2")},
          Qabs{get_input(input_quantities, "Qabs")},
          Rd{get_input(input_quantities, "Rd")},
          Rd_c{get_input(input_quantities, "Rd_c")},
          Rd_Ea{get_input(input_quantities, "Rd_Ea")},
          rh{get_input(input_quantities, "rh")},
          StomataWS{get_input(input_quantities, "StomataWS")},
          Tambient{get_input(input_quantities, "temp")},
          theta_0{get_input(input_quantities, "theta_0")},
          theta_1{get_input(input_quantities, "theta_1")},
          theta_2{get_input(input_quantities, "theta_2")},
          Tleaf{get_input(input_quantities, "Tleaf")},
          Tp_c{get_input(input_quantities, "Tp_c")},
          Tp_Ha{get_input(input_quantities, "Tp_Ha")},
          Tp_Hd{get_input(input_quantities, "Tp_Hd")},
          Tp_S{get_input(input_quantities, "Tp_S")},
          tpu_rate_max{get_input(input_quantities, "tpu_rate_max")},
          Vcmax_c{get_input(input_quantities, "Vcmax_c")},
          Vcmax_Ea{get_input(input_quantities, "Vcmax_Ea")},
          vmax1{get_input(input_quantities, "vmax1")},

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
    static std::string get_name() { return "c3_assimilation"; }

   private:
    // References to input quantities
    double const& atmospheric_pressure;
    double const& b0;
    double const& b1;
    double const& beta_PSII;
    double const& Catm;
    double const& electrons_per_carboxylation;
    double const& electrons_per_oxygenation;
    double const& gbw;
    double const& Gs_min;
    double const& Gstar_c;
    double const& Gstar_Ea;
    double const& jmax;
    double const& Jmax_c;
    double const& Jmax_Ea;
    double const& Kc_c;
    double const& Kc_Ea;
    double const& Ko_c;
    double const& Ko_Ea;
    double const& O2;
    double const& phi_PSII_0;
    double const& phi_PSII_1;
    double const& phi_PSII_2;
    double const& Qabs;
    double const& Rd;
    double const& Rd_c;
    double const& Rd_Ea;
    double const& rh;
    double const& StomataWS;
    double const& Tambient;
    double const& theta_0;
    double const& theta_1;
    double const& theta_2;
    double const& Tleaf;
    double const& Tp_c;
    double const& Tp_Ha;
    double const& Tp_Hd;
    double const& Tp_S;
    double const& tpu_rate_max;
    double const& Vcmax_c;
    double const& Vcmax_Ea;
    double const& vmax1;

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

string_vector c3_assimilation::get_inputs()
{
    return {
        "atmospheric_pressure",         // Pa
        "b0",                           // mol / m^2 / s
        "b1",                           // dimensionless
        "beta_PSII",                    // dimensionless (fraction of absorbed light that reaches photosystem II)
        "Catm",                         // micromol / mol
        "electrons_per_carboxylation",  // self-explanatory units
        "electrons_per_oxygenation",    // self-explanatory units
        "gbw",                          // mol / m^2 / s
        "Gs_min",                       // mol / m^2 / s
        "Gstar_c",                      // dimensionless
        "Gstar_Ea",                     // J / mol
        "jmax",                         // micromol / m^2 / s
        "Jmax_c",                       // dimensionless
        "Jmax_Ea",                      // J / mol
        "Kc_c",                         // dimensionless
        "Kc_Ea",                        // J / mol
        "Ko_c",                         // dimensionless
        "Ko_Ea",                        // J / mol
        "O2",                           // millimol / mol
        "phi_PSII_0",                   // dimensionless
        "phi_PSII_1",                   // (degrees C)^(-1)
        "phi_PSII_2",                   // (degrees C)^(-2)
        "Qabs",                         // micromol / m^2 / s
        "Rd",                           // micromol / m^2 / s
        "Rd_c",                         // dimensionless
        "Rd_Ea",                        // J / mol
        "rh",                           // dimensionless
        "StomataWS",                    // dimensionless
        "temp",                         // degrees C
        "theta_0",                      // dimensionless
        "theta_1",                      // (degrees C)^(-1)
        "theta_2",                      // (degrees C)^(-2)
        "Tleaf",                        // degrees C
        "Tp_c",                         // dimensionless
        "Tp_Ha",                        // J / mol
        "Tp_Hd",                        // J / mol
        "Tp_S",                         // J / K / mol
        "tpu_rate_max",                 // micromol / m^2 / s
        "Vcmax_c",                      // dimensionless
        "Vcmax_Ea",                     // J / mol
        "vmax1"                         // micromol / m^2 / s
    };
}

string_vector c3_assimilation::get_outputs()
{
    return {
        "Assim",              // micromol / m^2 / s
        "Gs",                 // mol / m^2 / s
        "Cs",                 // micromol / m^2 / s
        "RHs",                // dimensionless from Pa / Pa
        "Ci",                 // micromol / mol
        "GrossAssim",         // micromol / m^2 / s
        "Assim_conductance",  // micromol / m^2 / s
        "Rp",                 // micromol / m^2 / s
        "iterations"          // not a physical quantity
    };
}

void c3_assimilation::do_operation() const
{
    // Combine temperature response parameters
    c3_temperature_response_parameters const tr_param{
        Gstar_c,
        Gstar_Ea,
        Jmax_c,
        Jmax_Ea,
        Kc_c,
        Kc_Ea,
        Ko_c,
        Ko_Ea,
        phi_PSII_0,
        phi_PSII_1,
        phi_PSII_2,
        Rd_c,
        Rd_Ea,
        theta_0,
        theta_1,
        theta_2,
        Tp_c,
        Tp_Ha,
        Tp_Hd,
        Tp_S,
        Vcmax_c,
        Vcmax_Ea};

    photosynthesis_outputs c3_results = c3photoC(
        tr_param,
        Qabs,
        Tleaf,
        Tambient,
        rh,
        vmax1,
        jmax,
        tpu_rate_max,
        Rd,
        b0,
        b1,
        Gs_min,
        Catm,
        atmospheric_pressure,
        O2,
        StomataWS,
        electrons_per_carboxylation,
        electrons_per_oxygenation,
        beta_PSII,
        gbw);

    // Update the output quantity list
    update(Assim_op, c3_results.Assim);
    update(Gs_op, c3_results.Gs);
    update(Cs_op, c3_results.Cs);
    update(RHs_op, c3_results.RHs);
    update(Ci_op, c3_results.Ci);
    update(GrossAssim_op, c3_results.GrossAssim);
    update(Assim_conductance_op, c3_results.Assim_conductance);
    update(Rp_op, c3_results.Rp);
    update(iterations_op, c3_results.iterations);
}

}  // namespace standardBML
#endif
