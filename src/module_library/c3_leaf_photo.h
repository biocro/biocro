#ifndef C3_LEAF_PHOTO_H
#define C3_LEAF_PHOTO_H

#include "../framework/constants.h"
#include "../framework/module.h"
#include "../framework/state_map.h"
#include "c3_temperature_response.h"
#include "core/leaf_photosynthesis.h"
#include "../math/roots/multidim/zeros.h"

namespace standardBML
{

/**
 * @class c3_leaf_photo
 *
 * @brief Solves the coupled C3 photosynthesis and leaf energy balance system
 * using `PhotoCore::C3LeafPhoto` and a Broyden quasi-Newton solver.
 *
 * Unlike `c3_assimilation`, which uses nested 1D solvers (fixed-point over
 * stomatal conductance, Dekker over Ci, Dekker over Tleaf), this module
 * collapses all three into a single 2D solve over `(Cc, Tleaf)`. The two
 * residual equations are:
 *
 *   r[0] = An(Cc, Tleaf) - Gtc(Cc, Tleaf) * (Ca - Cc)   [carbon balance]
 *   r[1] = check_leaf_temp(Gs(Cc, Tleaf), Tleaf, ...)    [energy balance]
 *
 * ### Inputs
 *
 * Energy balance inputs not required by `c3_assimilation`:
 * - `'absorbed_shortwave'` — shortwave energy absorbed by the leaf (J / m^2 / s)
 * - `'gbw_canopy'` — canopy boundary layer conductance to H2O, mass basis (m / s)
 * - `'leaf_width'` — characteristic leaf dimension for boundary layer calc (m)
 * - `'wind_speed'` — wind speed at the leaf surface (m / s)
 *
 * All other inputs match `c3_assimilation`. Note that `gbw_canopy` here is a
 * mass conductance (m / s), whereas `c3_assimilation` uses a molar boundary
 * layer conductance (mol / m^2 / s).
 *
 * ### Outputs
 *
 * - `'Cc'` — chloroplast CO2 concentration (micromol / mol)
 * - `'leaf_temperature'` — solved leaf temperature (degrees C)
 * - `'Assim'` — net CO2 assimilation rate (micromol / m^2 / s)
 * - `'GrossAssim'` — gross CO2 assimilation rate (micromol / m^2 / s)
 * - `'Gs'` — stomatal conductance for H2O (mol / m^2 / s)
 * - `'RL'` — non-photorespiratory CO2 release in the light (micromol / m^2 / s)
 * - `'Rp'` — photorespiration rate (micromol / m^2 / s)
 * - `'EPenman'` — Penman potential transpiration (mmol / m^2 / s)
 * - `'EPriestly'` — Priestly potential transpiration (mmol / m^2 / s)
 * - `'Trans'` — actual transpiration (Mg / ha / hr)
 * - `'residual_C3_Assim'` — carbon balance residual at convergence (micromol / m^2 / s)
 * - `'residual_Tleaf'` — energy balance residual at convergence (degrees C)
 * - `'iteration_C3_Assim'` — number of Broyden iterations
 */
class c3_leaf_photo : public direct_module
{
   public:
    c3_leaf_photo(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          atmospheric_pressure{get_input(input_quantities, "atmospheric_pressure")},
          absorbed_shortwave{get_input(input_quantities, "absorbed_shortwave")},
          b0{get_input(input_quantities, "b0")},
          b1{get_input(input_quantities, "b1")},
          beta_PSII{get_input(input_quantities, "beta_PSII")},
          Catm{get_input(input_quantities, "Catm")},
          electrons_per_carboxylation{get_input(input_quantities, "electrons_per_carboxylation")},
          electrons_per_oxygenation{get_input(input_quantities, "electrons_per_oxygenation")},
          gbw_canopy{get_input(input_quantities, "gbw_canopy")},
          gm_at_25{get_input(input_quantities, "gm_at_25")},
          gm_Ha{get_input(input_quantities, "gm_Ha")},
          gm_Hd{get_input(input_quantities, "gm_Hd")},
          gm_S{get_input(input_quantities, "gm_S")},
          Gs_min{get_input(input_quantities, "Gs_min")},
          Gstar_at_25{get_input(input_quantities, "Gstar_at_25")},
          Gstar_Ea{get_input(input_quantities, "Gstar_Ea")},
          Jmax_at_25{get_input(input_quantities, "Jmax_at_25")},
          Jmax_Ea{get_input(input_quantities, "Jmax_Ea")},
          Kc_at_25{get_input(input_quantities, "Kc_at_25")},
          Kc_Ea{get_input(input_quantities, "Kc_Ea")},
          Ko_at_25{get_input(input_quantities, "Ko_at_25")},
          Ko_Ea{get_input(input_quantities, "Ko_Ea")},
          leaf_width{get_input(input_quantities, "leaf_width")},
          O2{get_input(input_quantities, "O2")},
          phi_PSII_0{get_input(input_quantities, "phi_PSII_0")},
          phi_PSII_1{get_input(input_quantities, "phi_PSII_1")},
          phi_PSII_2{get_input(input_quantities, "phi_PSII_2")},
          Qabs{get_input(input_quantities, "Qabs")},
          rh{get_input(input_quantities, "rh")},
          RL_at_25{get_input(input_quantities, "RL_at_25")},
          RL_Ea{get_input(input_quantities, "RL_Ea")},
          StomataWS{get_input(input_quantities, "StomataWS")},
          Tambient{get_input(input_quantities, "temp")},
          theta_0{get_input(input_quantities, "theta_0")},
          theta_1{get_input(input_quantities, "theta_1")},
          theta_2{get_input(input_quantities, "theta_2")},
          Tp_at_25{get_input(input_quantities, "Tp_at_25")},
          Tp_Ha{get_input(input_quantities, "Tp_Ha")},
          Tp_Hd{get_input(input_quantities, "Tp_Hd")},
          Tp_S{get_input(input_quantities, "Tp_S")},
          Vcmax_at_25{get_input(input_quantities, "Vcmax_at_25")},
          Vcmax_Ea{get_input(input_quantities, "Vcmax_Ea")},
          wind_speed{get_input(input_quantities, "wind_speed")},

          // Get pointers to output quantities
          Cc_op{get_op(output_quantities, "Cc")},
          leaf_temperature_op{get_op(output_quantities, "leaf_temperature")},
          Assim_op{get_op(output_quantities, "Assim")},
          GrossAssim_op{get_op(output_quantities, "GrossAssim")},
          Gs_op{get_op(output_quantities, "Gs")},
          RL_op{get_op(output_quantities, "RL")},
          Rp_op{get_op(output_quantities, "Rp")},
          EPenman_op{get_op(output_quantities, "EPenman")},
          EPriestly_op{get_op(output_quantities, "EPriestly")},
          Trans_op{get_op(output_quantities, "Trans")},
          residual_C3_Assim_op{get_op(output_quantities, "residual_C3_Assim")},
          residual_Tleaf_op{get_op(output_quantities, "residual_Tleaf")},
          iteration_C3_Assim_op{get_op(output_quantities, "iteration_C3_Assim")}
    {
    }

    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "c3_leaf_photo"; }

   private:
    // References to input quantities
    double const& atmospheric_pressure;
    double const& absorbed_shortwave;
    double const& b0;
    double const& b1;
    double const& beta_PSII;
    double const& Catm;
    double const& electrons_per_carboxylation;
    double const& electrons_per_oxygenation;
    double const& gbw_canopy;
    double const& gm_at_25;
    double const& gm_Ha;
    double const& gm_Hd;
    double const& gm_S;
    double const& Gs_min;
    double const& Gstar_at_25;
    double const& Gstar_Ea;
    double const& Jmax_at_25;
    double const& Jmax_Ea;
    double const& Kc_at_25;
    double const& Kc_Ea;
    double const& Ko_at_25;
    double const& Ko_Ea;
    double const& leaf_width;
    double const& O2;
    double const& phi_PSII_0;
    double const& phi_PSII_1;
    double const& phi_PSII_2;
    double const& Qabs;
    double const& rh;
    double const& RL_at_25;
    double const& RL_Ea;
    double const& StomataWS;
    double const& Tambient;
    double const& theta_0;
    double const& theta_1;
    double const& theta_2;
    double const& Tp_at_25;
    double const& Tp_Ha;
    double const& Tp_Hd;
    double const& Tp_S;
    double const& Vcmax_at_25;
    double const& Vcmax_Ea;
    double const& wind_speed;

    // Pointers to output quantities
    double* Cc_op;
    double* leaf_temperature_op;
    double* Assim_op;
    double* GrossAssim_op;
    double* Gs_op;
    double* RL_op;
    double* Rp_op;
    double* EPenman_op;
    double* EPriestly_op;
    double* Trans_op;
    double* residual_C3_Assim_op;
    double* residual_Tleaf_op;
    double* iteration_C3_Assim_op;

    void do_operation() const;
};

string_vector c3_leaf_photo::get_inputs()
{
    return {
        "atmospheric_pressure",         // Pa
        "absorbed_shortwave",           // J / m^2 / s
        "b0",                           // mol / m^2 / s
        "b1",                           // dimensionless
        "beta_PSII",                    // dimensionless
        "Catm",                         // micromol / mol
        "electrons_per_carboxylation",  // dimensionless
        "electrons_per_oxygenation",    // dimensionless
        "gbw_canopy",                   // m / s
        "gm_at_25",                     // mol / m^2 / s / Pa
        "gm_Ha",                        // J / mol
        "gm_Hd",                        // J / mol
        "gm_S",                         // J / K / mol
        "Gs_min",                       // mol / m^2 / s
        "Gstar_at_25",                  // micromol / mol
        "Gstar_Ea",                     // J / mol
        "Jmax_at_25",                   // micromol / m^2 / s
        "Jmax_Ea",                      // J / mol
        "Kc_at_25",                     // micromol / mol
        "Kc_Ea",                        // J / mol
        "Ko_at_25",                     // mmol / mol
        "Ko_Ea",                        // J / mol
        "leaf_width",                   // m
        "O2",                           // mmol / mol
        "phi_PSII_0",                   // dimensionless
        "phi_PSII_1",                   // (degrees C)^(-1)
        "phi_PSII_2",                   // (degrees C)^(-2)
        "Qabs",                         // micromol / m^2 / s
        "rh",                           // dimensionless from Pa / Pa
        "RL_at_25",                     // micromol / m^2 / s
        "RL_Ea",                        // J / mol
        "StomataWS",                    // dimensionless
        "temp",                         // degrees C
        "theta_0",                      // dimensionless
        "theta_1",                      // (degrees C)^(-1)
        "theta_2",                      // (degrees C)^(-2)
        "Tp_at_25",                     // micromol / m^2 / s
        "Tp_Ha",                        // J / mol
        "Tp_Hd",                        // J / mol
        "Tp_S",                         // J / K / mol
        "Vcmax_at_25",                  // micromol / m^2 / s
        "Vcmax_Ea",                     // J / mol
        "wind_speed",                   // m / s
    };
}

string_vector c3_leaf_photo::get_outputs()
{
    return {
        "Cc",                  // micromol / mol
        "leaf_temperature",    // degrees C
        "Assim",               // micromol / m^2 / s
        "GrossAssim",          // micromol / m^2 / s
        "Gs",                  // mol / m^2 / s
        "RL",                  // micromol / m^2 / s
        "Rp",                  // micromol / m^2 / s
        "EPenman",             // mmol / m^2 / s
        "EPriestly",           // mmol / m^2 / s
        "Trans",               // Mg / ha / hr
        "residual_C3_Assim",   // micromol / m^2 / s
        "residual_Tleaf",      // degrees C
        "iteration_C3_Assim",  // not a physical quantity
    };
}

void c3_leaf_photo::do_operation() const
{
    c3_temperature_response_parameters const tr_param{
        gm_Ha, gm_Hd, gm_S,
        Gstar_Ea,
        Jmax_Ea,
        Kc_Ea,
        Ko_Ea,
        phi_PSII_0, phi_PSII_1, phi_PSII_2,
        RL_Ea,
        theta_0, theta_1, theta_2,
        Tp_Ha, Tp_Hd, Tp_S,
        Vcmax_Ea};

    // Absorbed longwave: blackbody emission from surroundings at ambient temperature
    double const absorbed_longwave =
        physical_constants::stefan_boltzmann *
        std::pow(conversion_constants::celsius_to_kelvin + Tambient, 4);  // J / m^2 / s

    // Construct the coupled leaf model.
    // lnb0 = Vcmax_at_25 and lnb1 = 0: Vcmax does not vary with leaf nitrogen
    // at the leaf level (operator() leafN argument is unused).
    PhotoCore::C3LeafPhoto leaf_photo(
        tr_param,
        Qabs,                // iabs (micromol / m^2 / s)
        absorbed_shortwave,  // j_shortwave (J / m^2 / s)
        absorbed_longwave,   // absorbed_longwave (J / m^2 / s)
        Tambient,
        atmospheric_pressure,
        rh,
        Gstar_at_25, Kc_at_25, Ko_at_25,
        Vcmax_at_25, Vcmax_at_25, 0.0,  // lnb0, lnb1
        Jmax_at_25, Tp_at_25, RL_at_25,
        b0, b1, Gs_min, StomataWS,
        Catm, O2,
        electrons_per_carboxylation, electrons_per_oxygenation,
        beta_PSII, gm_at_25,
        gbw_canopy, leaf_width,
        wind_speed);

    auto result = leaf_photo.solve();

    if (!result.success) {
        throw std::runtime_error(
            "c3_leaf_photo: Broyden solver: " + result.status_message(true));
    }

    double const Cc = result.zero[0];     // micromol / mol
    double const Tleaf = result.zero[1];  // degrees C
    double const Gs = result.zero[2];

    PhotoCore::LeafAssim la = leaf_photo.evaluate(Cc, Tleaf, Gs);

    update(Cc_op, Cc);
    update(leaf_temperature_op, Tleaf);
    update(Assim_op, la.assim);
    update(GrossAssim_op, la.carboxylation);
    update(Gs_op, la.stomatal_vapor_conductance);
    update(RL_op, la.leaf_respiration);
    update(Rp_op, la.photorespiration);
    update(EPenman_op, la.penman);
    update(EPriestly_op, la.priestly);
    update(Trans_op, la.transpiration);
    update(residual_C3_Assim_op, result.residual[0]);
    update(residual_Tleaf_op, result.residual[1]);
    update(iteration_C3_Assim_op, static_cast<double>(result.iteration));
}

}  // namespace standardBML
#endif
