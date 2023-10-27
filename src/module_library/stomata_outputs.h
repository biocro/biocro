#ifndef STOMATA_OUTPUTS_H
#define STOMATA_OUTPUTS_H

/**
 * @brief A simple structure for holding the output of stomatal conductance
 * calculations.
 */
struct stomata_outputs {
    double cs;   //!< CO2 concentration at the leaf surface (micromol / mol)
    double hs;   //!< Relative humidity at the leaf surface (dimensionless)
    double gsw;  //!< Stomatal conductance to water vapor (mmol / m^2 / s)
};

#endif
