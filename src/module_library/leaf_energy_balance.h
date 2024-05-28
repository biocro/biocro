#ifndef ENERGY_BALANCE_H
#define ENERGY_BALANCE_H

struct energy_balance_outputs {
    double Deltat;      //!< Temperature difference (leaf - air) (degrees C)
    double EPenman;     //!< Potential transpiration rate (mmol / m^2 / s)
    double EPriestly;   //!< Priestly transpiration rate (mmol / m^2 / s)
    double E_loss;      //!< Rate of energy loss due to transpiration (J / m^2 / s)
    double gbw;         //!< Boundary layer conductance to water vapor (m / s)
    double gbw_canopy;  //!< Canopy boundary layer conductance to water vapor (m / s)
    double gbw_leaf;    //!< Leaf boundary layer conductance to water vapor (m / s)
    double gsw;         //!< Stomatal conductance to water vapor (m / s)
    double H;           //!< Rate of sensible heat loss (J / m^2 / s)
    double PhiN;        //!< Net energy available for transpiration and heat loss (J / m^2 / s)
    double storage;     //!< Rate of energy storage by the leaf; should be zero (J / m^2 / s)
    double TransR;      //!< Transpiration rate (mmol / m^2 / s)
    int iterations;     //!< Number of iterations used by convergence loop
};

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
