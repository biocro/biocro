#ifndef PHOTOSYNTHESIS_OUTPUTS_H
#define PHOTOSYNTHESIS_OUTPUTS_H

/**
 * @brief A simple structure for holding the output of photosynthesis
 * calculations.
 */
struct photosynthesis_outputs {
    double Assim;              //!< Net CO2 assimilation rate (micromol / m^2 / s)
    double Assim_check;        //!< Equals zero if loop has converged (micromol / m^2 / s)
    double Assim_conductance;  //!< Conductance-limited net CO2 assim. rate (micromol / m^2 / s)
    double Ci;                 //!< CO2 concentration in intercellular spaces (micromol / mol)
    double Cs;                 //!< CO2 concentration at the leaf surface (micromol / mol)
    double GrossAssim;         //!< Gross CO2 assimilation rate (micromol / m^2 / s)
    double Gs;                 //!< Stomatal conductance to water vapor (mol / m^2 / s)
    double RHs;                //!< Relative humidity at the leaf surface (dimensionless)
    double RL;                 //!< Rate of non-photorespiratory CO2 release in the light (micromol / m^2 / s)
    double Rp;                 //!< Rate of photorespiration (micromol / m^2 / s)
    size_t iterations;         //!< Number of iterations used by convergence loop
};

#endif
