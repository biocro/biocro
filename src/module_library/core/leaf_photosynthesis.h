#ifndef LEAF_PHOTOSYNTHESIS_H
#define LEAF_PHOTOSYNTHESIS_H

#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

#include "../../framework/constants.h"          // for dr_boundary, dr_stomata
#include "../ball_berry_gs.h"                   // for ball_berry_gs
#include "../boundary_layer_conductance.h"      // for leaf_boundary_layer_conductance_campbell
#include "../c3_temperature_response.h"         // for c3_temperature_response, c3_temperature_response_parameters
#include "../conductance_helpers.h"             // for g_to_molecular, sequential_conductance
#include "../conductance_limited_assim.h"       // for conductance_limited_assim
#include "../FvCB_assim.h"                      // for FvCB_assim
#include "../leaf_energy_balance.h"             // for check_leaf_temp
#include "../water_and_air_properties.h"        // for TempToCp, dry_air_density, etc.
#include "photosynthesis.h"                     // for LeafAssim
#include "../../math/roots/multidim/broyden.h"  // for Broyden
/**
 * @file
 * @brief Two-dimensional residual functor for the coupled C3 leaf
 * photosynthesis and energy balance system.
 *
 * The current implementation in `c3CanAC` solves three nested 1D problems:
 * - An outer fixed-point loop over stomatal conductance `Gs`
 * - An inner Dekker solve for leaf temperature `Tleaf` (inside `leaf_energy_balance`)
 * - An inner Dekker solve for intercellular CO2 `Ci` (inside `c3photoC`)
 *
 * `C3LeafPhoto` collapses all three into a single 2D system with unknowns
 * `x = {Ci (micromol/mol), Tleaf (°C)}`.  Both residual equations can be
 * evaluated in closed form once `(Ci, Tleaf)` are known — no inner iteration
 * is required — making it suitable for `root_multidim::broyden<2>`.
 *
 * **Residual equations:**
 *
 *   `r[0](Ci, Tleaf) = An(Ci, Tleaf) − Gtc(Ci, Tleaf) · (Ca − Ci)` (carbon balance)
 *
 *   `r[1](Ci, Tleaf) = check_leaf_temp(Gs(Ci, Tleaf), Tleaf, ...)`  (energy balance)
 *
 * **Evaluation chain** (no root-finding, all direct calculations):
 * 1. `c3_temperature_response(Tleaf)` → temperature-adjusted kinetic parameters
 * 2. `FvCB_assim(Ci, ...)` → net assimilation `An`
 * 3. `leaf_boundary_layer_conductance_campbell(Tleaf, ...)` → `gbw_molecular`
 * 4. `ball_berry_gs(An, ..., gbw_molecular)` → stomatal conductance `Gs`
 * 5. Carbon residual from `An`, `Gs`, `Ci`, `Ca`
 * 6. Energy residual from `check_leaf_temp(Gs, Tleaf, ...)`
 *
 * **Note on residual scaling:** `r[0]` is in micromol/m²/s and `r[1]` is in
 * degrees C.  Broyden initialises with the identity Jacobian, so the first
 * step treats both equations as equally weighted.  If convergence is slow,
 * consider normalising the residuals by typical magnitudes (~10 micromol/m²/s
 * and ~5 °C respectively).
 */

namespace PhotoCore
{

/**
 * @brief Solubility of O2 in water relative to its value at 25 °C.
 *
 * Mirrors `solo()` in `c3photo.cpp`.  Reference: Long, Plant, Cell &
 * Environment 14, 729–739 (1991), https://doi.org/10.1111/j.1365-3040.1991.tb01439.x
 */
inline double o2_solubility(double Tleaf  // degrees C
)
{
    return (0.047 - 0.0013087 * Tleaf + 2.5603e-05 * std::pow(Tleaf, 2) -
            2.1441e-07 * std::pow(Tleaf, 3)) /
           0.026934;
}

struct C3LeafPhoto {
    // ---- Photosynthesis parameters (fixed across iterations) ---------------
    c3_temperature_response_parameters tr_param;
    double iabs;         //!< Absorbed PPFD (micromol / m^2 / s)
    double Ca;           //!< Atmospheric CO2 concentration (micromol / mol)
    double RH;           //!< Relative humidity (dimensionless from Pa / Pa)
    double Gstar_at_25;  //!< (micromol / mol)
    double Kc_at_25;     //!< (micromol / mol)
    double Ko_at_25;     //!< (mmol / mol)
    double Vcmax_at_25;  //!< (micromol / m^2 / s)
    double lnb0;         //!< Intercept for nitrogen-Vcmax relationship (micromol / m^2 / s); set to Vcmax_at_25 when not using nitrogen scaling
    double lnb1;         //!< Slope for nitrogen-Vcmax relationship (micromol / m^2 / s per micromol / m^2 / s); set to 0 when not using nitrogen scaling
    double Jmax_at_25;   //!< (micromol / m^2 / s)
    double Tp_at_25;     //!< (micromol / m^2 / s)
    double RL_at_25;     //!< (micromol / m^2 / s)
    double b0_adj;       //!< Stomatal offset, pre-adjusted for water stress (mol / m^2 / s)
    double b1_adj;       //!< Stomatal slope, pre-adjusted for water stress (dimensionless)
    double o2;           //!< Atmospheric O2 concentration (mmol / mol)
    double electrons_per_carboxylation;
    double electrons_per_oxygenation;
    double beta_PSII;  //!< Fraction of absorbed light reaching PSII (dimensionless)
    double gm_at_25;   //!< Mesophyll conductance to CO2 at 25 °C (mol / m^2 / s / Pa)

    // ---- Energy balance parameters -----------------------------------------
    double ambient_temperature;   //!< (degrees C)
    double atmospheric_pressure;  //!< (Pa)
    double gbw_canopy;            //!< Canopy boundary layer conductance to H2O, mass basis (m / s)
    double leaf_width;            //!< (m)
    double wind_speed;            //!< (m / s)
    double absorbed_longwave;     //!< Absorbed longwave energy, constant across layers (J / m^2 / s)

    // ---- Pre-computed vapour/air properties (depend only on fixed ambient conditions)
    double Delta_rho;  //!< Vapour density deficit (kg / m^3)
    double gamma;      //!< Psychrometric parameter (kg / m^3 / K)
    double J_a;        //!< Total absorbed energy = shortwave + longwave (J / m^2 / s)
    double lambda;     //!< Latent heat of vaporization of water (J / kg)
    double s;          //!< d(saturation vapour density)/dT (kg / m^3 / K)

    /**
     * @brief Construct a residual functor for a single leaf class.
     *
     * Pre-computes all air/vapour properties that depend only on ambient
     * conditions, so `operator()` only needs to evaluate temperature-dependent
     * photosynthetic and boundary layer terms.
     *
     * @param b0, b1, Gs_min, StomataWS  Ball-Berry parameters and water stress
     *        factor; `b0_adj` and `b1_adj` are computed here as
     *        `StomataWS * b0 + Gs_min * (1 - StomataWS)` and `StomataWS * b1`.
     */
    C3LeafPhoto(
        c3_temperature_response_parameters tr_param,
        double iabs,                  // micromol / m^2 / s
        double j_shortwave,           // J / m^2 / s
        double absorbed_longwave,     // J / m^2 / s
        double ambient_temperature,   // degrees C
        double atmospheric_pressure,  // Pa
        double RH,                    // dimensionless
        double Gstar_at_25,           // micromol / mol
        double Kc_at_25,              // micromol / mol
        double Ko_at_25,              // mmol / mol
        double Vcmax_at_25,           // micromol / m^2 / s
        double lnb0,                  // micromol / m^2 / s  (set to Vcmax_at_25 when lnfun == 0)
        double lnb1,                  // micromol / m^2 / s per micromol / m^2 / s  (set to 0 when lnfun == 0)
        double Jmax_at_25,            // micromol / m^2 / s
        double Tp_at_25,              // micromol / m^2 / s
        double RL_at_25,              // micromol / m^2 / s
        double b0,                    // mol / m^2 / s
        double b1,                    // dimensionless
        double Gs_min,                // mol / m^2 / s
        double StomataWS,             // dimensionless
        double Ca,                    // micromol / mol
        double o2,                    // mmol / mol
        double electrons_per_carboxylation,
        double electrons_per_oxygenation,
        double beta_PSII,   // dimensionless
        double gm_at_25,    // mol / m^2 / s / Pa
        double gbw_canopy,  // m / s
        double leaf_width,  // m
        double wind_speed   // m / s
        ) : tr_param{tr_param},
            iabs{iabs},
            Ca{Ca},
            RH{RH},
            Gstar_at_25{Gstar_at_25},
            Kc_at_25{Kc_at_25},
            Ko_at_25{Ko_at_25},
            Vcmax_at_25{Vcmax_at_25},
            lnb0{lnb0},
            lnb1{lnb1},
            Jmax_at_25{Jmax_at_25},
            Tp_at_25{Tp_at_25},
            RL_at_25{RL_at_25},
            b0_adj{StomataWS * b0 + Gs_min * (1.0 - StomataWS)},
            b1_adj{StomataWS * b1},
            o2{o2},
            electrons_per_carboxylation{electrons_per_carboxylation},
            electrons_per_oxygenation{electrons_per_oxygenation},
            beta_PSII{beta_PSII},
            gm_at_25{gm_at_25},
            ambient_temperature{ambient_temperature},
            atmospheric_pressure{atmospheric_pressure},
            gbw_canopy{gbw_canopy},
            leaf_width{leaf_width},
            wind_speed{wind_speed},
            absorbed_longwave{absorbed_longwave}
    {
        double const c_p = TempToCp(ambient_temperature);                                             // J / kg / K
        lambda = water_latent_heat_of_vaporization_henderson(ambient_temperature);                    // J / kg
        double const p_w_sat = saturation_vapor_pressure(ambient_temperature);                        // Pa
        double const rho_ta = dry_air_density(ambient_temperature, atmospheric_pressure);             // kg / m^3
        s = TempToSFS(ambient_temperature);                                                           // kg / m^3 / K
        gamma = rho_ta * c_p / lambda;                                                                // kg / m^3 / K
        double const p_w_air = p_w_sat * RH;                                                          // Pa
        double const rho_w_air = vapor_density_from_pressure(rho_ta, atmospheric_pressure, p_w_air);  // kg / m^3
        double const rho_w_sat = vapor_density_from_pressure(rho_ta, atmospheric_pressure, p_w_sat);  // kg / m^3
        Delta_rho = rho_w_sat - rho_w_air;                                                            // kg / m^3
        J_a = j_shortwave + absorbed_longwave;                                                        // J / m^2 / s
    }

    /**
     * @brief Callable interface for `PhotoCore::CanopyIntegrand`.
     *
     * Updates the layer-specific radiation environment, wind speed, and leaf
     * nitrogen concentration, then finds the steady-state `(Cc, Tleaf)` via
     * `root_multidim::broyden<2>` and returns a fully populated `LeafAssim`.
     *
     * **Nitrogen → Vcmax convention:**  pass `lnb0 = Vcmax_at_25, lnb1 = 0`
     * to the constructor when nitrogen scaling is disabled (`lnfun == 0`); pass
     * the actual intercept and slope when it is enabled.  The effective Vcmax at
     * each layer is then `leafN * lnb1 + lnb0`.
     *
     * @param iabs        Absorbed PPFD for this leaf class (micromol / m^2 / s)
     * @param j_shortwave Absorbed shortwave energy for energy balance (J / m^2 / s)
     * @param wind_speed  Wind speed at this canopy layer (m / s)
     * @param leafN       Leaf nitrogen at this layer (micromol / m^2 / s)
     */
    LeafAssim operator()(double iabs, double j_shortwave, double wind_speed, double leafN) const
    {
        C3LeafPhoto layer = *this;
        layer.iabs = iabs;
        layer.J_a = j_shortwave + absorbed_longwave;
        layer.wind_speed = wind_speed;
        layer.Vcmax_at_25 = leafN * lnb1 + lnb0;

        root_multidim::broyden<2> solver(200, 1e-6, 1e-6);
        auto result = solver.solve(
            [&](std::array<double, 2> x) { return layer.residual(x); },
            layer.initial_guess());

        if (!result.success) {
            throw std::runtime_error("C3LeafPhoto: Broyden solver failed to converge");
        }
        return layer.evaluate(result.zero[0], result.zero[1]);
    }

    /**
     * @brief Evaluate the 2D residual at `x = {Cc, Tleaf}`.
     *
     * @param x `x[0]` = Cc (micromol/mol), `x[1]` = Tleaf (degrees C)
     * @return `{r_carbon (micromol/m²/s), r_energy (degrees C)}`
     */
    std::array<double, 2> residual(std::array<double, 2> x) const
    {
        auto const s = _compute(x[0], x[1]);
        return {s.r_carbon, s.r_energy};
    }

    /**
     * @brief Extract all leaf-level outputs at a converged `(Cc, Tleaf)`.
     *
     * Call this once after the Broyden solver has converged to obtain `An`,
     * `Gs`, etc. without repeating any root-finding.
     */
    LeafAssim evaluate(double Cc, double Tleaf) const
    {
        auto const st = _compute(Cc, Tleaf);
        double const Rp = (Cc > 0.0) ? st.Vc * st.Gstar / Cc : 0.0;  // micromol / m^2 / s

        // mmol / m^2 / s -> Mg / ha / hr: (3600 s/hr)(1e-3 mol/mmol)(1e-3 Mg/kg)(1e4 m^2/ha)
        double constexpr cf2 = physical_constants::molar_mass_of_water * 36;
        return {
            /* assim = */ st.An,
            /* stomatal_vapor_conductance = */ st.Gs,
            /* penman = */ st.EPenman,
            /* priestly = */ st.EPriestly,
            /* carboxylation = */ st.Vc,
            /* leaf_respiration = */ st.RL,
            /* photorespiration = */ Rp,
            /* transpiration = */ st.TransR * cf2,
            /* whole_plant_growth_respiration = */ 0.0};
    }

    /**
     * @brief Recommended initial guess: `{Cc, Tleaf} = {0.718 * Ca, ambient_temperature}`.
     *
     * `0.718 * Ca` matches the starting bracket midpoint used by the Dekker
     * solver inside `c3photoC` for Cc.
     */
    std::array<double, 2> initial_guess() const
    {
        return {0.718 * Ca, ambient_temperature};
    }

   private:
    struct _State {
        double An;         // net assimilation (micromol / m^2 / s)
        double Gs;         // stomatal conductance to H2O (mol / m^2 / s)
        double Vc;         // RuBP carboxylation rate (micromol / m^2 / s)
        double RL;         // leaf respiration (micromol / m^2 / s)
        double Gstar;      // CO2 compensation point at Tleaf (micromol / mol)
        double gm;         // mesophyll conductance to CO2 at Tleaf (mol / m^2 / s)
        double r_carbon;   // carbon balance residual (micromol / m^2 / s)
        double r_energy;   // energy balance residual (degrees C)
        double EPenman;    // Penman potential transpiration (mmol / m^2 / s)
        double EPriestly;  // Priestly potential transpiration (mmol / m^2 / s)
        double TransR;     // actual transpiration (mmol / m^2 / s)
    };

    _State _compute(double Cc, double leaf_temperature) const
    {
        // --- 1. Temperature-adjusted kinetic parameters ----------------------
        c3_param_at_tleaf const c3p = c3_temperature_response(tr_param, leaf_temperature);
        double const Gstar = Gstar_at_25 * c3p.Gstar_norm;                // micromol / mol
        double const Jmax = Jmax_at_25 * c3p.Jmax_norm;                   // micromol / m^2 / s
        double const Kc = Kc_at_25 * c3p.Kc_norm;                         // micromol / mol
        double const Ko = Ko_at_25 * c3p.Ko_norm;                         // mmol / mol
        double const RL = RL_at_25 * c3p.RL_norm;                         // micromol / m^2 / s
        double const theta = c3p.theta;                                   // dimensionless
        double const TPU = Tp_at_25 * c3p.Tp_norm;                        // micromol / m^2 / s
        double const Vcmax = Vcmax_at_25 * c3p.Vcmax_norm;                // micromol / m^2 / s
        double const gm = gm_at_25 * c3p.gm_norm * atmospheric_pressure;  // mol / m^2 / s

        // --- 2. Electron transport rate -------------------------------------
        double const I2 = iabs * c3p.phi_PSII * beta_PSII;  // micromol / m^2 / s
        double const J = (Jmax + I2 -
                          std::sqrt(std::pow(Jmax + I2, 2) - 4.0 * theta * I2 * Jmax)) /
                         (2.0 * theta);                          // micromol / m^2 / s
        double const Oi = o2 * o2_solubility(leaf_temperature);  // mmol / mol

        // --- 3. FvCB assimilation (direct, no root-finding) ----------------
        constexpr double alpha_TPU = 0.0;
        FvCB_outputs const fvcb = FvCB_assim(
            Cc, Gstar, J, Kc, Ko, Oi, RL, TPU, Vcmax, alpha_TPU,
            electrons_per_carboxylation, electrons_per_oxygenation);
        double const An = fvcb.An;  // micromol / m^2 / s

        // --- 4. Boundary layer conductance at Tleaf ------------------------
        double const gbw_leaf = leaf_boundary_layer_conductance_campbell(
            ambient_temperature,
            leaf_temperature - ambient_temperature,
            leaf_width,
            wind_speed,
            atmospheric_pressure);                                                            // m / s
        double const gbw_mass = sequential_conductance({gbw_leaf, gbw_canopy});               // m / s
        double const gbw_mol = g_to_molar(atmospheric_pressure, gbw_mass, leaf_temperature);  // mol / m^2 / s

        // --- 5. Stomatal conductance via Ball-Berry -------------------------
        double const inf = std::numeric_limits<double>::infinity();
        double const Gs = ball_berry_gs(
                              std::min(An, conductance_limited_assim(Ca, gbw_mol, inf)) * 1e-6,  // mol / m^2 / s
                              Ca * 1e-6,                                                         // mol / mol
                              RH,
                              b0_adj, b1_adj,
                              gbw_mol,
                              leaf_temperature,
                              ambient_temperature)
                              .gsw;  // mol / m^2 / s

        // --- 6. Carbon balance residual ------------------------------------
        using physical_constants::dr_boundary;
        using physical_constants::dr_stomata;
        double const Gtc = sequential_conductance({gbw_mol / dr_boundary, Gs / dr_stomata, gm});
        double const r_carbon = An - Gtc * (Ca - Cc);  // micromol / m^2 / s

        // --- 7. Energy balance residual ------------------------------------
        constexpr double epsilon_s = 1.0;
        double const r_energy = check_leaf_temp(
            atmospheric_pressure, ambient_temperature, Delta_rho,
            epsilon_s, gamma, gbw_canopy, J_a, lambda,
            leaf_temperature, leaf_width, s, Gs, wind_speed);  // degrees C

        // --- 8. Transpiration rates ------------------------------------------------
        double const gsw_mass = g_to_mass(atmospheric_pressure, Gs, leaf_temperature);  // m / s
        double const gw = sequential_conductance({gsw_mass, gbw_mass});                 // m / s
        double const Phi_N = J_a - physical_constants::stefan_boltzmann *
                                       std::pow(conversion_constants::celsius_to_kelvin + leaf_temperature, 4);  // J / m^2 / s
        double const Delta_T = leaf_temperature - ambient_temperature;                                           // degrees C
        double const E = (Delta_rho + s * Delta_T) * gw;                                                         // kg / m^2 / s
        double constexpr cf = 1e3 / physical_constants::molar_mass_of_water;                                     // mmol / kg
        double const TransR = E * cf;                                                                            // mmol / m^2 / s
        double const EPenman = (s * Phi_N + lambda * gamma * gbw_mass * Delta_rho) /
                               (lambda * (s + gamma)) * cf;                       // mmol / m^2 / s
        double const EPriestly = 1.26 * s * Phi_N / (lambda * (s + gamma)) * cf;  // mmol / m^2 / s

        return {An, Gs, fvcb.Vc, RL, Gstar, gm, r_carbon, r_energy, EPenman, EPriestly, TransR};
    }
};

}  // namespace PhotoCore
#endif
