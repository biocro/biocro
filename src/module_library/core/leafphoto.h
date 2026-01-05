#ifndef LEAF_PHOTO_H
#define LEAF_PHOTO_H

#include <array>

namespace photosynthesis {

struct LeafPhotoC3 {
    // external inputs needed to solve equation
    double absorbed_radiation;                             // W / m^2
    double air_temperature;                                // deg C
    double air_pressure;                                   // Pa
    double leaf_width;                                     // m
    double relative_humidity;                              // Pa / Pa
    double canopy_boundary_layer_conductance_water_vapor;  // m / s
    double wind_speed;                                     // m / s
    
    LeafPhotoC3(
        double absorbed_radiation,                             // W / m^2
        double air_temperature,                                // deg C
        double air_pressure,                                   // Pa
        double leaf_width,                                     // m
        double relative_humidity,                              // Pa / Pa
        double gbw_canopy,  // m / s
        double wind_speed                                     // m / s
    ) : absorbed_radiation{absorbed_radiation}, 
        air_temperature{air_temperature},
        air_pressure{air_pressure},
        leaf_width{leaf_width},
        relative_humidity{relative_humidity},
        canopy_boundary_layer_conductance_water_vapor{gbw_canopy},
        wind_speed{wind_speed} 
    {   
        wa = relative_humidity * saturation_vapor_pressure(air_temperature) / air pressure;
        
    }
    // computed once
    double wa; 
    
    // system of equations to solve
    static size_t dim = 5;
  
    std::array<double, dim> operator()(std::array<double, dim>const& x) {
        std::array<double, dim> y;
        double bb_index = assim  * hs / Cs;
        double assim = FvCB;
        y[0] = ci_balance_equation(Ci, gs, assim);
        y[1] = cs_balance_equation(Cs, Ci, gs);
        y[2] = stomatal_equation(Cs, gsw, hs, assim);
        y[3] = surface_relative_humidity_equation(gsw, hs, leaf_temperature);
        y[4] = heat_balance(leaf_temperature, gsw);
        return y;
    }     

    double ci_balance_equation(double Ci, double gs, double assim) const {
        return gsc * (Cs - Ci) - assim;   
    }

    double cs_balance_equation(double Cs, double Ci, double gsc) const {
        return gbc * (Ca - Cs) - gsc * (Cs - Ci);    
    }

    double stomatal_equation(double Cs, double gsw, double hs, double assim) const {
        double bb_index = std::max(assim, 0)  * hs/ Cs
        double g0 = bb_intercept + bb_slope * bb_index; 
        return g0 - gs;   
    }
    
    double surface_relative_humidity_equation(double gsw, double hs, double leaf_temperature) const {
        double gbw = boundary_water_vapor_conductance(leaf_temperature);
        double wi = saturation_vapor_pressure(leaf_temperature) / air_pressure ;
        double ws = hs * wi;
        return gbw * (wa - ws) - gsw * (ws - wi);
    }
     // leaf heat balance based on:
    // Equation 14.1, pg 224 in Campbell & Norman, "An Introduction to Environmental Biophysics" 2ed.
    double heat_balance(double const& leaf_temperature, double const& gsw) const
    {
        double gbw = boundary_water_vapor_conductance(leaf_temperature);
        double vapor_cond = sequential_conductance(gbw, gsw);
        double E = leaf_transpiration(leaf_temperature, vapor_cond);
        return absorbed_radiation - blackbody_radiation(leaf_temperature) - sensible_heat_flux(leaf_temperature, vapor_cond) - latent_heat_flux(E);  // J / m^2 / s
    }

    double blackbody_radiation(double const& leaf_temperature) const
    {
        using conversion_constants::celsius_to_kelvin;
        using physical_constants::stefan_boltzmann;

        double temp = celsius_to_kelvin + leaf_temperature;             // K
        return leaf_emissivity * stefan_boltzmann * std::pow(temp, 4);  // J / m^2 / s
    }

    double sensible_heat_flux(double const& leaf_temperature) const
    {
        using physical_constants::molar_mass_of_dry_air;
        const double cp = molar_mass_of_dry_air * TempToCp(air_temperature);  // J / mol

        double delta_temp = leaf_temperature - air_temperature;
        return cp * heat_conductance(leaf_temperature) * (delta_temp);
    };

    double latent_heat_flux(double const& _leaf_transpiration) const
    {
        using physical_constants::molar_mass_of_water;                                       // kg /mol
        double const lambda = water_latent_heat_of_vaporization_henderson(air_temperature);  // J / kg

        return lambda * molar_mass_of_water * _leaf_transpiration;  // J / m^2 / s
    }

    double leaf_transpiration(double const& leaf_temperature, double const& _water_vapor_conductance) const
    {
        double vp_air = relative_humidity * saturation_vapor_pressure(air_temperature);  // Pa

        // assuming leaf's interior has relative humidity = 1
        double vp_leaf = saturation_vapor_pressure(leaf_temperature);         // Pa
        return _water_vapor_conductance * (vp_leaf - vp_air) / air_pressure;  // mol / m^2 / s
    }
    
    double heat_conductance(double const& leaf_temperature) const
    {
        double const gbv_canopy = g_to_molecular(air_pressure, canopy_boundary_layer_conductance_water_vapor, leaf_temperature);  // mol / m^2 / s
        // convert to a heat transfer conductance
        constexpr double heat_to_vapor_conductance_ratio = 0.135 / 0.147;
        double const gbh_canopy = heat_to_vapor_conductance_ratio * gbv_canopy;
        // from Table 7.6 on pg. 109 in Campbell & Norman, "An Introduction to Environmental Biophysics" 2ed.
        // Set constants
        double constexpr coef_forced = 0.135;
        double constexpr coef_free = 0.05;

        // Calculate conductances
        double const gbh_forced = coef_forced * std::sqrt(wind_speed / leaf_width);                                     // mol / m^2 / s
        double const gbh_free = coef_free * std::pow(std::abs(leaf_temperature - air_temperature) / leaf_width, 0.25);  // mol / m^2 / s

        // The overall conductance is the larger one
        double const gbh_leaf = std::max(gbh_forced, gbh_free);  // mol / m^2 / s

        // using the forced convection; ratio is almost the same for free convection (I think the numbers in the table are rounded)
        double const gth = sequential_conductance(gbh_leaf, gbh_canopy);  // mol/ m^2 / s

        return gth;  // mol / m^2 / s
    }

    double boundary_water_vapor_conductance(double const& leaf_temperature) const const
    {
        double const gbv_canopy = g_to_molecular(air_pressure, canopy_boundary_layer_conductance_water_vapor, leaf_temperature);  // mol / m^2 / s

        // Set constants
        double constexpr coef_forced = 0.147;
        double constexpr coef_free = 0.055;

        // Calculate conductances
        double const gbv_forced = coef_forced * std::sqrt(wind_speed / leaf_width);                                     // mol / m^2 / s
        double const gbv_free = coef_free * std::pow(std::abs(leaf_temperature - air_temperature) / leaf_width, 0.25);  // mol / m^2 / s

        // The overall conductance is the larger one
        double const gbv_leaf = std::max(gbv_forced, gbv_free);  // mol / m^2 / s

        // Get the boundary layer conductance and total conductance to water
        // vapor
        return sequential_conductance(gbv_leaf, gbv_canopy);                  // mol/ m^2 / s
        
    }

      
    
};
} // namespace photosynthesis
/*
photosynthesis_outputs c3photoC(
    c3_temperature_response_parameters const tr_param,
    double const absorbed_ppfd,                // micromol / m^2 / s
    double const Tleaf,                        // degrees C
    double const Tambient,                     // degrees C
    double const RH,                           // dimensionless
    double const Vcmax_at_25,                  // micromol / m^2 / s
    double const Jmax_at_25,                   // micromol / m^2 / s
    double const TPU_rate_max,                 // micromol / m^2 / s
    double const RL_at_25,                     // micromol / m^2 / s
    double const b0,                           // mol / m^2 / s
    double const b1,                           // dimensionless
    double const Gs_min,                       // mol / m^2 / s
    double const Ca,                           // micromol / mol
    double const AP,                           // Pa (TEMPORARILY UNUSED)
    double const O2,                           // millimol / mol (atmospheric oxygen mole fraction)
    double const StomWS,                       // dimensionless
    double const electrons_per_carboxylation,  // self-explanatory units
    double const electrons_per_oxygenation,    // self-explanatory units
    double const beta_PSII,                    // dimensionless (fraction of absorbed light that reaches photosystem II)
    double const gbw                           // mol / m^2 / s
)
{
    // Define infinity
    double const inf = std::numeric_limits<double>::infinity();

    // Check inputs
    if (absorbed_ppfd < 0) {
        throw std::out_of_range("Input `absorbed_ppfd` cannot be negative. Check `solar` is not negative.");
    }

    // Calculate values of key parameters at leaf temperature
    c3_param_at_tleaf c3_param = c3_temperature_response(tr_param, Tleaf);

    double const dark_adapted_phi_PSII = c3_param.phi_PSII;  // dimensionless
    double const Gstar = c3_param.Gstar;                     // micromol / mol
    double const Jmax = Jmax_at_25 * c3_param.Jmax_norm;     // micromol / m^2 / s
    double const Kc = c3_param.Kc;                           // micromol / mol
    double const Ko = c3_param.Ko;                           // mmol / mol
    double const RL = RL_at_25 * c3_param.RL_norm;           // micromol / m^2 / s
    double const theta = c3_param.theta;                     // dimensionless
    double const TPU = TPU_rate_max * c3_param.Tp_norm;      // micromol / m^2 / s
    double const Vcmax = Vcmax_at_25 * c3_param.Vcmax_norm;  // micromol / m^2 / s

    // The variable that we call `I2` here has been described as "the useful
    // light absorbed by photosystem II" (S. von Caemmerer (2002)) and "the
    // maximum fraction of incident quanta that could be utilized in electron
    // transport" (Bernacchi et al. (2003)). Here we calculate its value using
    // Equation 3 from Bernacchi et al. (2003), except that we have replaced the
    // factor `Q * alpha_leaf` (the product of the incident PPFD `Q` and the
    // leaf absorptance) with the absorbed PPFD, as this is clearly the intended
    // meaning of the `Q * alpha_leaf` factor. See also Equation 8 from the
    // original FvCB paper, where `J` (equivalent to our `I2`) is proportional
    // to the absorbed PPFD rather than the incident PPFD.
    double I2 = absorbed_ppfd * dark_adapted_phi_PSII * beta_PSII;  // micromol / m^2 / s

    double const J =
        (Jmax + I2 - sqrt(pow(Jmax + I2, 2) - 4.0 * theta * I2 * Jmax)) /
        (2.0 * theta);  // micromol / m^2 / s

    double const Oi = O2 * solo(Tleaf);  // mmol / mol

    // The alpha constant for calculating Ap is from Eq. 2.26, von Caemmerer, S.
    // Biochemical models of leaf photosynthesis.
    double const alpha_TPU = 0.0;  // dimensionless. Without more information, alpha=0 is often assumed.

    // Adjust Ball-Berry parameters in response to water stress
    double const b0_adj = StomWS * b0 + Gs_min * (1.0 - StomWS);
    double const b1_adj = StomWS * b1;

    // Initialize variables before running fixed point iteration in a loop
    // these are updated as a side effect in the secant method iterations
    FvCB_outputs FvCB_res;
    stomata_outputs BB_res;
    double Gs{1e3};     // mol / m^2 / s  (initial guess)
    double Assim{0.0};  // micromol / mol (initial guess)

    // This lambda function equals zero only if Ci satisfies both the FvCB and
    // Ball-Berry models. Here, Ci should be expressed in micromol / mol.
    auto check_assim_rate = [=, &FvCB_res, &BB_res, &Gs, &Assim](double Ci) {
        // Use Ci to compute the assimilation rate according to the FvCB model.
        FvCB_res = FvCB_assim(
            Ci, Gstar, J, Kc, Ko, Oi, RL, TPU, Vcmax, alpha_TPU,
            electrons_per_carboxylation,
            electrons_per_oxygenation);

        Assim = FvCB_res.An;  // micromol / m^2 / s

        // Use Assim to compute the stomatal conductance according to the
        // Ball-Berry model. If Assim is too high, Cs will take a negative
        // value, which is not allowed by the Ball-Berry model. To avoid this,
        // we clamp Assim to the value that produces Cs = 0; this will result
        // in Gs = infinity.
        BB_res = ball_berry_gs(
            std::min(Assim, conductance_limited_assim(Ca, gbw, inf)) * 1e-6,
            Ca * 1e-6,
            RH,
            b0_adj,
            b1_adj,
            gbw,
            Tleaf,
            Tambient);

        Gs = BB_res.gsw;  // mol / m^2 / s

        // Using Ci and Gs, make a new estimate of the assimilation rate. If
        // the initial value of Ci was correct, this should be identical to
        // Assim.
        double Gt = sequential_conductance(gbw / dr_boundary, Gs / dr_stomata);  // mol / m^2 / s

        return Assim - Gt * (Ca - Ci);  // micromol / m^2 / s
    };

    // Get an upper bound for Ci by finding the most negative value of An (which
    // occurs when Ci = 0), the smallest total conductance to CO2 (which occurs
    // when gsw takes its minimum value b0), and then using Ci = Ca - An / gtc.
    double const A_min =
        FvCB_assim(
            0.0, Gstar, J, Kc, Ko, Oi, RL, TPU, Vcmax, alpha_TPU,
            electrons_per_carboxylation,
            electrons_per_oxygenation)
            .An;  // micromol / m^2 / s

    double const Ci_max =
        Ca - A_min * (dr_boundary / gbw + dr_stomata / b0_adj);  // micromol / mol

    // Run the Dekker method
    using namespace root_finding;
    dekker solve{500, 1e-12, 1e-12};
    result_t result = solve(
        check_assim_rate,
        0.718 * Ca,
        0,
        Ci_max * 1.01);

    // Throw exception if not converged
    if (!is_successful(result.flag)) {
        throw std::runtime_error(
            "Ci solver reports failed convergence with termination flag:\n    " +
            flag_message(result.flag));
    }

    // Get final values
    double const Ci = result.root;                                         // micromol / mol
    double const an_conductance = conductance_limited_assim(Ca, gbw, Gs);  // micromol / m^2 / s

    
}

#endif
