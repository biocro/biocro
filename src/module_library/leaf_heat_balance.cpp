#include <cmath>                          // for std::pow, std::abs
#include "../framework/constants.h"       // for stefan_boltzmann,
                                          // celsius_to_kelvin, molar_mass_of_water
#include "boundary_layer_conductance.h"   // for leaf_boundary_layer_conductance_nikolov
#include "conductance_helpers.h"          // for g_to_mass, g_to_molecular, sequential_conductance
#include "../math/roots/onedim/dekker.h"  // for dekker
#include "water_and_air_properties.h"     // for TempToCp, dry_air_density, etc
#include "leaf_heat_balance.h"

leaf_heat_balance::leaf_heat_balance(
    double absorbed_longwave_energy,   // J / m^2 / s
    double absorbed_shortwave_energy,  // J / m^2 / s
    double air_pressure,               // Pa
    double air_temperature,            // degrees C
    double gbw_canopy,                 // m / s
    double leaf_width,                 // m
    double relative_humidity,          // dimensionless from Pa / Pa
    double stomatal_conductance,       // mol / m^2 / s
    double wind_speed                  // m / s
    ) : absorbed_radiation{absorbed_longwave_energy + absorbed_shortwave_energy},

        air_temperature{air_temperature},
        air_pressure{air_pressure},
        leaf_width{leaf_width},
        relative_humidity{relative_humidity},
        canopy_boundary_layer_conductance_water_vapor{gbw_canopy},
        stomatal_conductance_water_vapor{stomatal_conductance},
        wind_speed{wind_speed}

{
}

root_finding::result_t leaf_heat_balance::solve() const
{
    // Run Dekker's method

    root_finding::result_t _result = _equation_solver.solve(
        [this](double x) { return this->heat_balance(x); },
        air_temperature,                                                     // guess
        air_temperature - 20,                                                // lower
        air_temperature + 30 / (1 + stomatal_conductance_water_vapor / 0.5)  // upper
    );

    // Throw exception if not converged
    if (!root_finding::is_successful(_result.flag)) {
        throw std::runtime_error(
            "leaf_temperature solver reports failed convergence with termination flag:\n    " +
            root_finding::flag_message(_result.flag));
    }

    return _result;
}

energy_balance_outputs leaf_heat_balance::make_result(root_finding::result_t const& result) const
{
    // Get final value
    double leaf_temperature = result.root;  // deg C

    double vapor_cond = water_vapor_conductance(leaf_temperature);
    double E = leaf_transpiration(leaf_temperature, vapor_cond);
    double B = blackbody_radiation(leaf_temperature);             // J / m^2 / s
    double H = sensible_heat_flux(leaf_temperature, vapor_cond);  // J / m^2 / s
    double L = latent_heat_flux(E);                               // J / m^2 / s

    // Calculate additional outputs
    double const gsw = g_to_mass(air_pressure, stomatal_conductance_water_vapor, leaf_temperature);  // m / s
    double const gbw_molecular = water_vapor_conductance(leaf_temperature);                          // mol / m^2 / s
    double const gbw_leaf = g_to_mass(air_pressure, gbw_molecular, leaf_temperature);                // mol / m^2 / s

    return energy_balance_outputs{
        /* Deltat = */ leaf_temperature - air_temperature,                 // degrees C
        /* E_loss = */ L,                                                  // J / m^2 / s
        /* EPenman = */ 0,                                                 // mmol / m^2 / s
        /* EPriestly = */ 0,                                               // mmol / m^2 / s
        /* gbw = */ 0,                                                     // m / s
        /* gbw_canopy = */ canopy_boundary_layer_conductance_water_vapor,  // m / s
        /* gbw_leaf = */ gbw_leaf,                                         // m / s
        /* gbw_molecular = */ vapor_cond,                                  // mol / m^2 / s
        /* gsw = */ gsw,                                                   // m / s
        /* H = */ H,                                                       // J / m^2 / s
        /* leaf_temp_check = */ result.residual,                           // degrees C
        /* PhiN = */ absorbed_radiation - B,                               // J / m^2 / s
        /* RH_canopy = */ 0,                                               // dimensionless
        /* storage = */ absorbed_radiation - B - H - L,                    // J / m^2 / s
        /* TransR = */ 1e3 * E,                                            // mmol / m^2 / s
        /* iterations = */ result.iteration                                // not a physical quantity
    };
}

// Equation 14.1, pg 224 of Campbell & Norman, "An Introduction to Environmental Biophysics" 2ed.
double leaf_heat_balance::heat_balance(double const& leaf_temperature) const
{
    double vapor_cond = water_vapor_conductance(leaf_temperature);
    double E = leaf_transpiration(leaf_temperature, vapor_cond);
    return absorbed_radiation - blackbody_radiation(leaf_temperature) - sensible_heat_flux(leaf_temperature, vapor_cond) - latent_heat_flux(E);  // J / m^2 / s
}

// Stefan Boltzmann Law
double leaf_heat_balance::blackbody_radiation(double const& leaf_temperature) const
{
    using conversion_constants::celsius_to_kelvin;
    using physical_constants::stefan_boltzmann;

    double temp = celsius_to_kelvin + leaf_temperature;             // K
    return leaf_emissivity * stefan_boltzmann * std::pow(temp, 4);  // J / m^2 / s
}

double leaf_heat_balance::sensible_heat_flux(double const& leaf_temperature, double const& _water_vapor_conductance) const
{
    using physical_constants::molar_mass_of_dry_air;
    const double cp = molar_mass_of_dry_air * TempToCp(air_temperature);  // J / mol

    double delta_temp = leaf_temperature - air_temperature;
    return cp * heat_conductance(_water_vapor_conductance) * (delta_temp);
}

double leaf_heat_balance::latent_heat_flux(double const& _leaf_transpiration) const
{
    using physical_constants::molar_mass_of_water;                                       // kg /mol
    double const lambda = water_latent_heat_of_vaporization_henderson(air_temperature);  // J / kg

    return lambda * molar_mass_of_water * _leaf_transpiration;  // J / m^2 / s
}

double leaf_heat_balance::leaf_transpiration(double const& leaf_temperature, double const& _water_vapor_conductance) const
{
    double vp_air = relative_humidity * saturation_vapor_pressure(air_temperature);  // Pa

    // assuming leaf's interior has relative humidity = 1
    double vp_leaf = saturation_vapor_pressure(leaf_temperature);         // Pa
    return _water_vapor_conductance * (vp_leaf - vp_air) / air_pressure;  // mol / m^2 / s
}

double leaf_heat_balance::heat_conductance(double const& _water_vapor_conductance) const
{
    // from Table 7.6 on pg. 109 in Campbell & Norman, "An Introduction to Environmental Biophysics" 2ed.
    // using the forced convection; ratio is almost the same for free convection (I think the numbers in the table are rounded)
    constexpr double heat_to_vapor_conductance_ratio = 0.135 / 0.147;
    return heat_to_vapor_conductance_ratio * _water_vapor_conductance;  // mol / m^2 / s
}

double leaf_heat_balance::water_vapor_conductance(double const& leaf_temperature) const
{
    // Get stomatal conductance to water vapor as a mass conductance
    double const gbv_canopy = g_to_molecular(air_pressure, canopy_boundary_layer_conductance_water_vapor, leaf_temperature);  // m / s

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
    double const gbw = sequential_conductance(gbv_leaf, gbv_canopy);                  // mol/ m^2 / s
    double const gw = sequential_conductance(stomatal_conductance_water_vapor, gbw);  // mol / m^2 / s
    return gw;                                                                        // mol / m^2 / s
}
