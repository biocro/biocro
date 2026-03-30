#ifndef LEAF_CI_BALANCE_H
#define LEAF_CI_BALANCE_H

namespace photosynthesis {
/**
 *  @brief Calculates leaf ci balance
 *
 *  @details
 */
struct leaf_ci_balance {
    // inputs needed to compute leaf temperature
    double absorbed_radiation;                             // W / m^2
    double air_temperature;                                // deg C
    double air_pressure;                                   // Pa
    double leaf_width;                                     // m
    double relative_humidity;                              // Pa / Pa
    double canopy_boundary_layer_conductance_water_vapor;  // m / s
    double wind_speed;                                     // m / s

    // constructor
    leaf_ci_balance(
        double absorbed_longwave_energy,   // J / m^2 / s
        double absorbed_shortwave_energy,  // J / m^2 / s
        double air_pressure,               // Pa
        double air_temperature,            // degrees C
        double gbw_canopy,                 // m / s
        double leaf_width,                 // m
        double relative_humidity,          // dimensionless from Pa / Pa
        double wind_speed                  // m / s
    );


    // leaf heat balance based on:
    // Equation 14.1, pg 224 in Campbell & Norman, "An Introduction to Environmental Biophysics" 2ed.
    double heat_balance(double const& leaf_temperature, double const& stomatal_conductance_water_vapor) const;
    double blackbody_radiation(double const& leaf_temperature) const;
    double sensible_heat_flux(double const& leaf_temperature, double const& _water_vapor_conductance) const;
    double latent_heat_flux(double const& _leaf_transpiration) const;
    double leaf_transpiration(double const& leaf_temperature, double const& _water_vapor_conductance) const;
    double heat_conductance(double const& leaf_temperature) const;
    double water_vapor_conductance(double const& leaf_temperature, double const& stomatal_conductance_water_vapor) const;

};

}

#endif
