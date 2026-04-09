#ifndef ENERGY_BALANCE_H
#define ENERGY_BALANCE_H
#include <cmath>                          // for pow, std::abs
#include "../framework/constants.h"       // for stefan_boltzmann,
                                          // celsius_to_kelvin, molar_mass_of_water
#include "boundary_layer_conductance.h"   // for leaf_boundary_layer_conductance_nikolov
#include "conductance_helpers.h"          // for g_to_mass, g_to_molecular, sequential_conductance
#include "../math/roots/onedim/dekker.h"  // for dekker
#include "water_and_air_properties.h"     // for TempToCp, dry_air_density, etc
#include <cstddef>                        // for size_t

struct energy_balance_outputs {
    double Deltat;                          //!< Temperature difference (leaf - air) (degrees C)
    double E_loss;                          //!< Rate of energy loss due to transpiration (J / m^2 / s)
    double EPenman;                         //!< Potential transpiration rate (mmol / m^2 / s)
    double EPriestly;                       //!< Priestly transpiration rate (mmol / m^2 / s)
    double gbw;                             //!< Total boundary layer conductance to water vapor, for mass fluxes (m / s)
    double gbw_canopy;                      //!< Canopy boundary layer conductance to water vapor, for mass fluxes (m / s)
    double gbw_leaf;                        //!< Leaf boundary layer conductance to water vapor, for mass fluxes (m / s)
    double gbw_molecular;                   //!< Total boundary layer conductance to water vapor, for molecular fluxes (mol / m^2 / s)
    double gsw;                             //!< Stomatal conductance to water vapor, for mass fluxes (m / s)
    double H;                               //!< Rate of sensible heat loss (J / m^2 / s)
    double residual_energy_balance_Tleaf;   //!< Equals zero if loop has converged (degrees C)
    double PhiN;                            //!< Net energy available for transpiration and heat loss (J / m^2 / s)
    double RH_canopy;                       //!< Relative humidity in the canopy, just outside the leaf boundary layer (dimensionless)
    double storage;                         //!< Rate of energy storage by the leaf; should be zero (J / m^2 / s)
    double TransR;                          //!< Transpiration rate (mmol / m^2 / s)
    size_t iteration_energy_balance_Tleaf;  //!< Number of iterations used by convergence loop
};

double check_leaf_temp(
    double const air_pressure,          // Pa
    double const air_temperature,       // degrees C
    double const Delta_rho,             // kg / m^3
    double const epsilon_s,             // dimensionless
    double const gamma,                 // kg / m^3 / K
    double const gbw_canopy,            // m / s
    double const J_a,                   // J / m^2 / s
    double const lambda,                // J / kg
    double const leaf_temperature,      // degrees C
    double const leaf_width,            // m
    double const s,                     // kg / m^3 / K
    double const stomatal_conductance,  // mol / m^2 / s
    double const wind_speed             // m / s
);

double calculate_gbw_leaf(
    double const air_pressure,      // Pa
    double const air_temperature,   // degrees C
    double const leaf_temperature,  // degrees C
    double const leaf_width,        // m
    double const wind_speed         // m / s
);

auto make_check_leaf_temp(
    double absorbed_longwave_energy,   // J / m^2 / s
    double absorbed_shortwave_energy,  // J / m^2 / s
    double air_pressure,               // Pa
    double air_temperature,            // degrees C
    double gbw_canopy,                 // m / s
    double leaf_width,                 // m
    double relative_humidity,          // dimensionless from Pa / Pa
    double wind_speed                  // m / s
)
{
    // Set some constants
    double constexpr epsilon_s = 1.0;  // dimensionless

    // Get water vapor and air properties based on the air temperature
    double const c_p = TempToCp(air_temperature);                                        // J / kg / K
    double const lambda = water_latent_heat_of_vaporization_henderson(air_temperature);  // J / kg
    double const p_w_sat_air = saturation_vapor_pressure(air_temperature);               // Pa
    double const rho_ta = dry_air_density(air_temperature, air_pressure);                // kg / m^3
    double const s = TempToSFS(air_temperature);                                         // kg / m^3 / K

    // Get the pyschrometric parameter
    double const gamma = rho_ta * c_p / lambda;  // kg / m^3 / K

    // Get vapor density in the ambient air.
    double const p_w_air = p_w_sat_air * relative_humidity;  // Pa

    double const rho_w_air =
        vapor_density_from_pressure(rho_ta, air_pressure, p_w_air);  // kg / m^3

    // Get vapor density deficit
    double const rho_w_sat =
        vapor_density_from_pressure(rho_ta, air_pressure, p_w_sat_air);  // kg / m^3

    double const Delta_rho = rho_w_sat - rho_w_air;  // kg / m^3

    // Get total absorbed light energy (longwave and shortwave)
    double const J_a = absorbed_shortwave_energy + absorbed_longwave_energy;  // J / m^2 / s

    // Use partial application to fix all inputs to `check_leaf_temp` except
    // leaf temperature. To solve the energy balance equations, a root of this
    // function must be found.
    return [=](double const leaf_temperature, double const stomatal_conductance) {
        return check_leaf_temp(
            air_pressure,          // Pa
            air_temperature,       // degrees C
            Delta_rho,             // kg / m^3
            epsilon_s,             // dimensionless
            gamma,                 // kg / m^3 / K
            gbw_canopy,            // m / s
            J_a,                   // J / m^2 / s
            lambda,                // J / kg
            leaf_temperature,      // degrees C
            leaf_width,            // m
            s,                     // kg / m^3 / K
            stomatal_conductance,  // mol / m^2 / s
            wind_speed             // m / s
        );
    };
}

energy_balance_outputs leaf_energy_balance(
    double absorbed_longwave_energy,   // J / m^2 / s
    double absorbed_shortwave_energy,  // J / m^2 / s
    double air_pressure,               // Pa
    double air_temperature,            // degrees C
    double gbw_canopy,                 // m / s
    double leaf_width,                 // m
    double relative_humidity,          // dimensionless from Pa / Pa
    double stomatal_conductance,       // mol / m^2 / s
    double wind_speed                  // m / s
);

#endif
