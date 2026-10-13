#ifndef LEAF_HEAT_BALANCE_H
#define LEAF_HEAT_BALANCE_H

#include "../math/roots/onedim/dekker.h"  // for dekker
#include "leaf_energy_balance.h"
/**
 *  @brief Calculates leaf-level temperature and transpiration rate for a leaf
 *  within a canopy using a heat balance equation
 *
 *  @details
 *
 *  Here we consider the path of water vapor to pass through three sequential
 *  barriers: the stomata, the leaf boundary layer, and finally the canopy
 *  boundary layer. Conductance across the leaf boundary layer is calculated
 *  using the `leaf_boundary_layer_conductance_nikolov()` function, while the
 *  other conductances must be provided as inputs.
 *
 *  Leaf temperature and transpiration using
 *  Equation 14.1, pg 224 in Campbell & Norman, "An Introduction to Environmental Biophysics" 2ed.
 *
 *  @param [in] absorbed_radiation All Absorbed light energy. E.g., sum of
 *              the longwave (infrared) and shortwave (PAR and near-infrared)
 *              bands (J / m^2 / s)
 *
 *  @param [in] air_pressure Total air pressure (Pa)
 *
 *  @param [in] air_temperature Bulk air temperature (degrees C)
 *
 *  @param [in] canopy_boundary_layer_conductance_water_vapor Conductance to H2O diffusion
 *              across the canopy boundary layer (m / s)
 *
 *  @param [in] leaf_width Characteristic dimension of a typical leaf (m)
 *
 *  @param [in] relative_humidity Relative humidity in the bulk air (Pa / Pa)
 *
 *  @param [in] stomatal_conductance_water_vapor Conductance to H2O diffusion across the stomata
 *              (mol / m^2 / s)
 *
 *  @param [in] wind_speed Wind speed within the canopy just outside the leaf
 *              boundary layer (m / s)
 */
struct leaf_heat_balance {
    // inputs needed to compute leaf temperature
    double absorbed_radiation;                             // W / m^2
    double air_temperature;                                // deg C
    double air_pressure;                                   // Pa
    double leaf_width;                                     // m
    double relative_humidity;                              // Pa / Pa
    double canopy_boundary_layer_conductance_water_vapor;  // m / s
    double stomatal_conductance_water_vapor;               // mol / m^2 / s
    double wind_speed;                                     // m / s

    static constexpr double leaf_emissivity = 1;  // dimensionless
    // constructor
    leaf_heat_balance(
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

    // solve heat balance equation, using root_finding
    root_finding::result_t solve() const;
    // extract results, given the correct value of leaf_temperature
    energy_balance_outputs make_result(root_finding::result_t const& result) const;

    // leaf heat balance based on:
    // Equation 14.1, pg 224 in Campbell & Norman, "An Introduction to Environmental Biophysics" 2ed.
    double heat_balance(double const& leaf_temperature) const;
    double blackbody_radiation(double const& leaf_temperature) const;
    double sensible_heat_flux(double const& leaf_temperature, double const& _water_vapor_conductance) const;
    double latent_heat_flux(double const& _leaf_transpiration) const;
    double leaf_transpiration(double const& leaf_temperature, double const& _water_vapor_conductance) const;
    double heat_conductance(double const& leaf_temperature) const;
    double water_vapor_conductance(double const& leaf_temperature) const;

   private:
    // equation_solver must be mutable as it has internal that it manages.
    mutable root_finding::dekker _equation_solver{100, 1e-12, 1e-12};
};

#endif
