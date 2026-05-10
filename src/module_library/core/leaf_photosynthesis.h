#ifndef LEAF_PHOTOSYNTHESIS_H
#define LEAF_PHOTOSYNTHESIS_H

#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

#include "../../framework/constants.h"        // for dr_boundary, dr_stomata
#include "../boundary_layer_conductance.h"    // for leaf_boundary_layer_conductance_campbell
#include "../c3_temperature_response.h"       // for c3_temperature_response, c3_temperature_response_parameters
#include "../conductance_helpers.h"           // for g_to_molar, g_to_mass, sequential_conductance
#include "../FvCB_assim.h"                    // for FvCB_assim
#include "../leaf_energy_balance.h"           // for check_leaf_temp
#include "../water_and_air_properties.h"      // for saturation_vapor_pressure, dry_air_density, etc.
#include "photosynthesis.h"                   // for LeafAssim
#include "../../math/roots/multidim/zeros.h"  // for root_multidim::SafeBroyden

/**
 * @file
 * @brief Residual functor for the coupled C3 leaf photosynthesis and energy
 * balance system, solved simultaneously over `{Cc, Tleaf, Gs}`.
 *
 * `Cs` and `Ci` are not solve variables; they are derived analytically from
 * the converged `(Cc, Gs, Tleaf)` inside each residual evaluation:
 *
 *   `Cs = Ca − An / gbc`  (boundary-layer flux = An)
 *   `Ci = Cc + An / gm`   (mesophyll flux = An; equals Cc when gm = ∞)
 *
 * This keeps the system 3D regardless of whether mesophyll resistance is
 * active, eliminates the 4D/5D runtime dispatch, and restores well-conditioned
 * Jacobian structure for Broyden.
 *
 * CO2 concentrations are carried internally in **mmol/mol** to balance
 * residual magnitudes:
 *
 *   `r_energy`  (~°C)          — energy balance
 *   `r_stomata` (~mol/m²/s)    — `Gs − Ball-Berry(An, Cs, hs)`
 *   `r_cc`      (~mmol/m²/s)   — `An·10⁻³ − Gtc·(Ca − Cc)` (carbon balance)
 *
 * The public interface (`operator()`, `evaluate`, `c3_leaf_photo`) works in
 * micromol/mol for Cc and reports Cs, Ci as derived outputs via `_State`.
 */

namespace PhotoCore
{

/**
 * @brief Solubility of O2 in water relative to its value at 25 °C.
 */
inline double o2_solubility(double Tleaf)
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
    double lnb0;         //!< Vcmax intercept for nitrogen scaling (micromol / m^2 / s)
    double lnb1;         //!< Vcmax slope for nitrogen scaling (dimensionless)
    double Jmax_at_25;   //!< (micromol / m^2 / s)
    double Tp_at_25;     //!< (micromol / m^2 / s)
    double RL_at_25;     //!< (micromol / m^2 / s)
    double b0_adj;       //!< Stomatal offset, pre-adjusted for water stress (mol / m^2 / s)
    double b1_adj;       //!< Stomatal slope, pre-adjusted for water stress (dimensionless)
    double o2;           //!< Atmospheric O2 concentration (mmol / mol)
    double electrons_per_carboxylation;
    double electrons_per_oxygenation;
    double beta_PSII;  //!< Fraction of absorbed light reaching PSII (dimensionless)
    double gm_at_25;   //!< Mesophyll conductance at 25 °C (mol / m^2 / s / Pa); infinity → no resistance

    // ---- Energy balance parameters -----------------------------------------
    double ambient_temperature;   //!< (degrees C)
    double atmospheric_pressure;  //!< (Pa)
    double gbw_canopy;            //!< Canopy boundary layer conductance to H2O, mass basis (m / s)
    double leaf_width;            //!< (m)
    double wind_speed;            //!< (m / s)
    double absorbed_longwave;     //!< (J / m^2 / s)

    // ---- Pre-computed ambient vapour/air properties ------------------------
    double Delta_rho;  //!< rho_w_sat(Tambient) − rho_w_air (kg / m^3)
    double gamma;      //!< Psychrometric parameter (kg / m^3 / K)
    double J_a;        //!< Total absorbed energy = shortwave + longwave (J / m^2 / s)
    double lambda;     //!< Latent heat of vaporization (J / kg)
    double s;          //!< d(rho_w_sat)/dT at Tambient (kg / m^3 / K)
    double rho_w_air;  //!< Ambient vapour density (kg / m^3)

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
        double lnb0,                  // micromol / m^2 / s
        double lnb1,                  // dimensionless
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
        double const c_p = TempToCp(ambient_temperature);                                                     // J / kg / K
        lambda = water_latent_heat_of_vaporization_henderson(ambient_temperature);                            // J / kg
        double const p_w_sat_amb = saturation_vapor_pressure(ambient_temperature);                            // Pa
        double const rho_ta = dry_air_density(ambient_temperature, atmospheric_pressure);                     // kg / m^3
        s = TempToSFS(ambient_temperature);                                                                   // kg / m^3 / K
        gamma = rho_ta * c_p / lambda;                                                                        // kg / m^3 / K
        double const p_w_air = p_w_sat_amb * RH;                                                              // Pa
        rho_w_air = vapor_density_from_pressure(rho_ta, atmospheric_pressure, p_w_air);                       // kg / m^3
        double const rho_w_sat_amb = vapor_density_from_pressure(rho_ta, atmospheric_pressure, p_w_sat_amb);  // kg / m^3
        Delta_rho = rho_w_sat_amb - rho_w_air;                                                                // kg / m^3
        J_a = j_shortwave + absorbed_longwave;                                                                // J / m^2 / s
    }

    /**
     * @brief Callable interface for `PhotoCore::CanopyIntegrand`.
     *
     * Updates layer-specific radiation, wind speed, and leaf nitrogen, then
     * solves for steady-state `(Cc, Tleaf, Gs)` and returns `LeafAssim`.
     */
    LeafAssim operator()(double iabs, double j_shortwave, double wind_speed, double leafN) const
    {
        C3LeafPhoto layer = *this;
        layer.iabs = iabs;
        layer.J_a = j_shortwave + absorbed_longwave;
        layer.wind_speed = wind_speed;
        layer.Vcmax_at_25 = leafN * lnb1 + lnb0;

        auto result = layer.solve();

        if (!result.success) {
            throw std::runtime_error(
                "C3LeafPhoto::operator(): solver " + result.status_message(true) +
                "; iabs=" + std::to_string(iabs) + " micromol/m2/s" +
                ", j_shortwave=" + std::to_string(j_shortwave) + " J/m2/s" +
                ", wind_speed=" + std::to_string(wind_speed) + " m/s" +
                ", leafN=" + std::to_string(leafN) + " micromol/m2/s" +
                ", gbw_canopy=" + std::to_string(gbw_canopy) + " m/s");
        }
        return layer.evaluate(result.zero[0], result.zero[1]);
    }

    /**
     * @brief Solve for steady-state `(Cc_mm, Tleaf, Gs)`.
     *
     * `Cc_mm` is Cc in mmol/mol.  The result is `result_t<3>`; convert
     * `result.zero[0]` to micromol/mol by multiplying by 1000.
     */
    root_multidim::result_t<2> solve() const
    {
        root_multidim::LevenbergMarquardt<2> solver;
        // solver.constraints.lower = {0.0,  0.0,  b0_adj};
        // solver.constraints.upper = {2.0,  50.0, 3.0   };
        return solver.solve(
            [this](std::array<double, 2> x) {
                return _residual(x[0], x[1]);
            },
            _initial_guess());
    }

    /**
     * @brief Extract leaf-level outputs at converged `(Cc_mm, Tleaf, Gs)`.
     *
     * `Cc_mm` is in mmol/mol (as returned by `solve().zero[0]`).
     */
    LeafAssim evaluate(double Cc_mm, double Tleaf) const
    {
        auto const st = _compute(Cc_mm, Tleaf);
        double const Cc = Cc_mm * 1e3;                               // mmol/mol → micromol/mol for photorespiration
        double const Rp = (Cc > 0.0) ? st.Vc * st.Gstar / Cc : 0.0;  // micromol / m^2 / s
        double constexpr cf2 = physical_constants::molar_mass_of_water * 36;
        return {
            /* assim = */ st.An,
            /* stomatal_vapor_conductance = */ st.gsw,
            /* penman = */ st.EPenman,
            /* priestly = */ st.EPriestly,
            /* carboxylation = */ st.Vc,
            /* leaf_respiration = */ st.RL,
            /* photorespiration = */ Rp,
            /* transpiration = */ st.TransR * cf2,
            /* whole_plant_growth_respiration = */ 0.0};
    }

   private:
    struct _State {
        double An = 0;         //!< net assimilation (micromol / m^2 / s)
        double Vc = 0;         //!< carboxylation rate (micromol / m^2 / s)
        double RL = 0;         //!< leaf respiration (micromol / m^2 / s)
        double Gstar = 0;      //!< CO2 compensation point at Tleaf (micromol / mol)
        double gm = 0;         //!< mesophyll conductance (mol / m^2 / s)
        double gbc = 0;        //!< boundary layer CO2 conductance (mol / m^2 / s)
        double gsc = 0;        //!< stomatal CO2 conductance (mol / m^2 / s)
        double gsw = 0;        //!< stomatal H2o conductance (mol / m^2 / s)
        double gbw_mass = 0;   //!< boundary layer H2O conductance, mass basis (m / s)
        double hs = 0;         //!< relative humidity at leaf surface (dimensionless)
        double Cs_mm = 0;      //!< CO2 at leaf surface, derived (mmol / mol)
        double Ci_mm = 0;      //!< intercellular CO2, derived (mmol / mol)
        double r_energy = 0;   //!< energy balance residual (degrees C)
        double r_stomata = 0;  //!< Ball-Berry residual (mol / m^2 / s)
        double r_cc = 0;       //!< carbon balance residual (mmol / m^2 / s)
        double EPenman = 0;    //!< Penman potential transpiration (mmol / m^2 / s)
        double EPriestly = 0;  //!< Priestley potential transpiration (mmol / m^2 / s)
        double TransR = 0;     //!< actual transpiration (mmol / m^2 / s)
    };

    /**
     * @brief Full evaluation at `(Cc_mm, Tleaf, Gs)`.
     *
     * Computes all photosynthetic, conductance, and energy-balance quantities.
     * `Cs` and `Ci` are derived analytically from An and the conductances:
     *
     *   `Cs = Ca − An / gbc`    (clamped above zero to guard Ball-Berry)
     *   `Ci = Cc + An / gm`     (equals Cc when gm = ∞, safe via IEEE 754)
     *
     * CO2 cascade residuals use mmol/mol to balance the solver's residual norm.
     */
    _State _compute(double Cc_mm, double leaf_temperature) const
    {
        using physical_constants::dr_boundary;
        using physical_constants::dr_stomata;
        _State st;

        double const Cc = Cc_mm * 1e3;   // mmol/mol → micromol/mol for FvCB
        double const Ca_mm = Ca * 1e-3;  // micromol/mol → mmol/mol

        // --- 1. Temperature-adjusted kinetic parameters ----------------------
        c3_param_at_tleaf const c3p = c3_temperature_response(tr_param, leaf_temperature);
        st.Gstar = Gstar_at_25 * c3p.Gstar_norm;         // micromol / mol
        double const Jmax = Jmax_at_25 * c3p.Jmax_norm;  // micromol / m^2 / s
        double const Kc = Kc_at_25 * c3p.Kc_norm;        // micromol / mol
        double const Ko = Ko_at_25 * c3p.Ko_norm;        // mmol / mol
        st.RL = RL_at_25 * c3p.RL_norm;                  // micromol / m^2 / s
        double const theta = c3p.theta;
        double const TPU = Tp_at_25 * c3p.Tp_norm;              // micromol / m^2 / s
        double const Vcmax = Vcmax_at_25 * c3p.Vcmax_norm;      // micromol / m^2 / s
        st.gm = gm_at_25 * c3p.gm_norm * atmospheric_pressure;  // mol / m^2 / s

        // --- 2. Electron transport rate -------------------------------------
        double const I2 = iabs * c3p.phi_PSII * beta_PSII;
        double const J = (Jmax + I2 -
                          std::sqrt(std::pow(Jmax + I2, 2) - 4.0 * theta * I2 * Jmax)) /
                         (2.0 * theta);                          // micromol / m^2 / s
        double const Oi = o2 * o2_solubility(leaf_temperature);  // mmol / mol

        // --- 3. FvCB assimilation -------------------------------------------
        constexpr double alpha_TPU = 0.0;
        FvCB_outputs const fvcb = FvCB_assim(
            Cc, st.Gstar, J, Kc, Ko, Oi, st.RL, TPU, Vcmax, alpha_TPU,
            electrons_per_carboxylation, electrons_per_oxygenation);
        st.An = fvcb.An;  // micromol / m^2 / s
        st.Vc = fvcb.Vc;  // micromol / m^2 / s

        // --- 4. Boundary layer conductances at Tleaf ------------------------
        double const gbw_leaf = leaf_boundary_layer_conductance_campbell(
            ambient_temperature,
            leaf_temperature - ambient_temperature,
            leaf_width, wind_speed, atmospheric_pressure);                                       // m / s
        st.gbw_mass = sequential_conductance({gbw_leaf, gbw_canopy});                            // m / s
        double const gbw_mol = g_to_molar(atmospheric_pressure, st.gbw_mass, leaf_temperature);  // mol / m^2 / s
        st.gbc = gbw_mol / dr_boundary;
        double const inf = std::numeric_limits<double>::infinity();
        st.gsw = ball_berry_gs(
                     std::min(st.An, conductance_limited_assim(Ca, st.gbw_mass, inf)) * 1e-6,
                     Ca * 1e-6,
                     RH,
                     b0_adj,
                     b1_adj,
                     st.gbw_mass,
                     leaf_temperature,
                     ambient_temperature)
                     .gsw;                                                                  // mol / m^2 / s
        double const gsw_mass = g_to_mass(atmospheric_pressure, st.gsw, leaf_temperature);  // m / s
        st.gsc = st.gsw / dr_stomata;                                                       // mol / m^2 / s

        // --- 5. Derived CO2 concentrations (mmol/mol) -----------------------
        // Safe when An ≤ 0 (respiration): Cs = Ca + |An|/gbc > Ca.
        // Clamped above a small positive value to guard Ball-Berry division.
        st.Cs_mm = std::max(Ca_mm - st.An * 1e-3 / st.gbc, 1e-6);
        // When gm = ∞: An*1e-3/gm = 0 via IEEE 754, so Ci = Cc.
        st.Ci_mm = Cc_mm + st.An * 1e-3 / st.gm;

        // --- 6. Vapour flux and leaf-surface humidity (hs) ------------------
        double const gw = sequential_conductance({gsw_mass, st.gbw_mass});  // m / s
        double const Delta_T = leaf_temperature - ambient_temperature;      // degrees C
        double const E = (Delta_rho + s * Delta_T) * gw;                    // kg / m^2 / s

        double const p_w_sat_leaf = saturation_vapor_pressure(leaf_temperature);
        double const rho_ta_leaf = dry_air_density(leaf_temperature, atmospheric_pressure);
        double const rho_w_sat_leaf = vapor_density_from_pressure(
            rho_ta_leaf, atmospheric_pressure, p_w_sat_leaf);  // kg / m^3
        double const rho_w_s = rho_w_air + E / st.gbw_mass;    // kg / m^3
        st.hs = std::clamp(rho_w_s / rho_w_sat_leaf, 0.0, 1.0);

        // --- 7. Ball-Berry residual (Cs and hs used directly) ---------------
        double const An_mol = st.An * 1e-6;     // mol / m^2 / s
        double const Cs_mol = st.Cs_mm * 1e-3;  // mmol/mol → mol / mol
        double const gs_bb = b0_adj + (An_mol < 0.0 ? 0.0 : b1_adj * An_mol * st.hs / Cs_mol);
        st.r_stomata = Gs - gs_bb;  // mol / m^2 / s

        // --- 8. Energy balance residual -------------------------------------
        constexpr double epsilon_s = 1.0;
        st.r_energy = check_leaf_temp(
            atmospheric_pressure, ambient_temperature, Delta_rho,
            epsilon_s, gamma, gbw_canopy, J_a, lambda,
            leaf_temperature, leaf_width, s, Gs, wind_speed);  // degrees C

        // --- 9. Carbon balance residual (mmol/m^2/s) ------------------------
        double const Gtc = sequential_conductance({st.gbc, st.gsc, st.gm});  // mol / m^2 / s
        st.r_cc = st.An * 1e-3 - Gtc * (Ca_mm - Cc_mm);                      // mmol / m^2 / s

        // --- 10. Transpiration outputs --------------------------------------
        double const Phi_N = J_a - physical_constants::stefan_boltzmann *
                                       std::pow(conversion_constants::celsius_to_kelvin + leaf_temperature, 4);
        double constexpr cf = 1e3 / physical_constants::molar_mass_of_water;  // mmol / kg
        st.TransR = E * cf;                                                   // mmol / m^2 / s
        st.EPenman = (s * Phi_N + lambda * gamma * st.gbw_mass * Delta_rho) /
                     (lambda * (s + gamma)) * cf;                       // mmol / m^2 / s
        st.EPriestly = 1.26 * s * Phi_N / (lambda * (s + gamma)) * cf;  // mmol / m^2 / s

        return st;
    }

    std::array<double, 3> _residual(double Cc_mm, double Tleaf, double Gs) const
    {
        auto const st = _compute(Cc_mm, Tleaf, Gs);
        return {st.r_energy, st.r_stomata, st.r_cc};
    }

    std::array<double, 3> _initial_guess() const
    {
        double const Cc_mm = (Ca / 400.0 * std::expm1(6.4123770 - 0.1565084 * std::log1p(iabs))) * 1e-3;
        double const gsw_guess = 0.008 + 0.001 * iabs;
        return {Cc_mm, ambient_temperature, gsw_guess};
    }
};

}  // namespace PhotoCore
#endif
