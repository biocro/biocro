#ifndef CANOPY_PHOTOSYNTHESIS_OUTPUTS_H
#define CANOPY_PHOTOSYNTHESIS_OUTPUTS_H

/**
 * @brief A simple structure for holding the output of canopy photosynthesis
 * calculations.
 */
struct canopy_photosynthesis_outputs {
    double Assim;                          //!< Net CO2 assimilation rate (micromol / m^2 / s)
    double canopy_conductance;             //!< Stomatal conductance to water vapor (mol / m^2 / s)
    double canopy_transpiration_penman;    //!< P-M transpiration rate (mmol / m^2 / s)
    double canopy_transpiration_priestly;  //!< Priestly transpiration rate (mmol / m^2 / s)
    double GrossAssim;                     //!< Gross CO2 assimilation rate (micromol / m^2 / s)
    double RL;                             //!< Rate of non-photorespiratory CO2 release in the light (micromol / m^2 / s)
    double Rp;                             //!< Rate of photorespiration (micromol / m^2 / s)
    double Trans;                          //!< Transpiration rate (Mg / ha / hr)
    double whole_plant_gr;                 //!< Whole-plant growth respiration rate (micromol / m^2 / s)
};

#endif
