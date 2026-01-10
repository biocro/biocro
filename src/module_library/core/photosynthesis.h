#ifndef CANOPY_PHOTO_CORE_H
#define CANOPY_PHOTO_CORE_H

#include "canopy_light_distribution.h"    // CanopyLight, LightProfile
#include "atmosphere_light_scattering.h"  // AtmosphereLightScattering
#include "../../framework/constants.h"  
/*
This file contains code used in photosynthesis modules to compute canopy photosynthesis integrals. However, this code is not a biocro module
*/

namespace PhotoCore {
/**
 * @brief A simple structure for holding the output of leaf photosynthesis
 * calculations; which will be summed into canopy photosynthesis rates. This type must have vector space operations: vector addition and scalar multiplication.
 */
struct LeafAssim {
    double assim = 0;                           //!< Net CO2 assimilation rate (micromol / m^2 / s)
    double stomatal_vapor_conductance = 0;      //!< Stomatal conductance to water vapor (mol / m^2 / s)
    double penman = 0;                          //!< P-M transpiration rate (mmol / m^2 / s)
    double priestly = 0 ;                       //!< Priestly transpiration rate (mmol / m^2 / s)
    double carboxylation = 0;                   //!< Gross CO2 assimilation rate (micromol / m^2 / s)
    double leaf_respiration =0 ;                //!< Rate of non-photorespiratory CO2 release in the light (micromol / m^2 / s)
    double photorespiration = 0;                //!< Rate of photorespiration (micromol / m^2 / s)
    double transpiration = 0;                   //!< Transpiration rate (Mg / ha / hr)
    double whole_plant_growth_respiration =0 ;  //!< Whole-plant growth respiration rate (micromol / m^2 / s)
   
    LeafAssim() = default;  
    
    LeafAssim& operator+=(const LeafAssim& rhs)
    {
        assim += rhs.assim;
        stomatal_vapor_conductance += rhs.stomatal_vapor_conductance;
        penman += rhs.penman;
        priestly += rhs.priestly;
        carboxylation += rhs.carboxylation;
        leaf_respiration += rhs.leaf_respiration;
        photorespiration += rhs.photorespiration;
        transpiration += rhs.transpiration;
        return *this;
    }
    
    LeafAssim& operator*=(double scalar){
        assim *= scalar;
        stomatal_vapor_conductance *= scalar;
        penman *= scalar;
        priestly *= scalar;
        carboxylation *= scalar;
        leaf_respiration *= scalar;
        photorespiration *= scalar;
        transpiration *= scalar;
        return *this;
    }

};

inline LeafAssim operator+(const LeafAssim& lhs, const LeafAssim& rhs) {
    LeafAssim out = lhs;
    out += rhs;
    return out;
}

inline LeafAssim operator*(const LeafAssim& lhs, double scalar) {
    LeafAssim out = lhs;
    out *= scalar;
    return out;
}

inline LeafAssim operator*(double scalar, const LeafAssim& rhs) {
    LeafAssim out = rhs;
    out *= scalar;
    return out;
}


template<typename LeafPhoto>
struct CanopyIntegrand {
    

    CanopyIntegrand(
        LeafPhoto photo_func,
        CanopyLight light_model,
        double kpLN,
        double leafN,            // micromol / m^2 / s
        double wind_speed            // m / s
    
    ) : leaf_photosynthesis{photo_func},
        canopy_light_model{light_model},
        kpLN{kpLN},
        leafN{leafN},
        wind_speed{wind_speed}
     {
      }

    LeafAssim operator()(double cumulative_lai){
        
        // Calculations that are the same for sunlit and shaded leaves
        double layer_leafN = leaf_nitrogen_profile(cumulative_lai, leafN, kpLN);
        double layer_wind_speed = wind_speed_profile(cumulative_lai, wind_speed);
        LightProfile light_profile = canopy_light_model.get_light_profile(cumulative_lai);

        // Calculations for sunlit leaves. 
        double i_dir = light_profile.sunlit.incident_ppfd;       // micromol / m^2 / s
        double j_dir = light_profile.sunlit.absorbed_shortwave;  // J / m^2 / s
        LeafAssim leaf_assim = leaf_photosynthesis(i_dir, j_dir, layer_wind_speed, layer_leafN) * light_profile.sunlit.fraction; 

        // Calculations for shaded leaves. 
        double i_diff = light_profile.shaded.incident_ppfd;       // micromol / m^2 / s
        double j_diff = light_profile.shaded.absorbed_shortwave;  // J / m^2 / s
        leaf_assim += leaf_photosynthesis(i_diff, j_diff, layer_wind_speed, layer_leafN) * light_profile.shaded.fraction; 
        return leaf_assim;
    }

private:
    
    LeafPhoto leaf_photosynthesis;
    CanopyLight canopy_light_model;
    double kpLN;
    double leafN;    
    double wind_speed;

};


}
#endif
