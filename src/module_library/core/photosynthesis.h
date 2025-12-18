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
struct CanopyAssim {
    double assim = 0;                           //!< Net CO2 assimilation rate (micromol / m^2 / s)
    double stomatal_vapor_conductance = 0;      //!< Stomatal conductance to water vapor (mol / m^2 / s)
    double penman = 0;                          //!< P-M transpiration rate (mmol / m^2 / s)
    double priestly = 0 ;                       //!< Priestly transpiration rate (mmol / m^2 / s)
    double carboxylation = 0;                   //!< Gross CO2 assimilation rate (micromol / m^2 / s)
    double leaf_respiration =0 ;                //!< Rate of non-photorespiratory CO2 release in the light (micromol / m^2 / s)
    double photorespiration = 0;                //!< Rate of photorespiration (micromol / m^2 / s)
    double transpiration = 0;                   //!< Transpiration rate (Mg / ha / hr)
    double whole_plant_growth_respiration =0 ;  //!< Whole-plant growth respiration rate (micromol / m^2 / s)
   
    CanopyAssim() = default;  
    
    
    CanopyAssim& operator+=(const CanopyAssim& rhs)
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
    
    CanopyAssim& operator*=(double scalar){
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

inline CanopyAssim operator+(const CanopyAssim& lhs, const CanopyAssim& rhs) {
    CanopyAssim out = lhs;
    out += rhs;
    return out;
}

inline CanopyAssim operator*(const CanopyAssim& lhs, double scalar) {
    CanopyAssim out = lhs;
    out *= scalar;
    return out;
}

inline CanopyAssim operator*(double scalar, const CanopyAssim& rhs) {
    CanopyAssim out = rhs;
    out *= scalar;
    return out;
}


template<typename PhotoFunc>
struct CanopyIntegrand {
    PhotoFunc photo;
    
    LightProfile light_profile;
    AtmosphericLightScattering atmo_light_scattering;
    CanopyLight canopy_light_model;
    
    CanopyIntegrand(
        double ambient_temperature,          // degrees C
        double atmospheric_pressure,         // Pa
        double atmospheric_scattering,       // dimensionless
        double atmospheric_transmittance,    // dimensionless
    double Catm,                         // ppm
    double chil,                         // dimensionless
    double cosine_zenith_angle,          // dimensionless
    double gbw_canopy,                   // m / s
    double Gs_min,                       // mol / m^2 / s
    double k_diffuse,                    // dimensionless
    double Kparm,
    double kpLN,
    emissivity
    double LAI,                     // dimensionless from m^2 / m^2
    double leaf_reflectance_nir,    // dimensionless
    double leaf_reflectance_par,    // dimensionless
    double leaf_transmittance_nir,  // dimensionless
    double leaf_transmittance_par,  // dimensionless
    double leafN,
    double leafwidth,            // m
    double lowerT,               // degrees C
    double par_energy_content,   // J / micromol
    double par_energy_fraction,  // dimensionless
    double RH,                   // dimensionless from Pa / Pa
    double RL_at_25,             // micromol / m^2 / s
    double solarR,               // micromol / m^2 / s
    double StomataWS,            // dimensionless
    double theta,                // dimensionless
    double upperT,               // degrees C
    double Vcmax_at_25,          // micromol / m^2 / s
    double WindSpeed,            // m / s
    int lnfun,                   // dimensionless switch
    int nlayers                  // dimensionless
)
//     = lightME(
//        cosine_zenith_angle,
//        atmospheric_pressure,
//        atmospheric_transmittance,
//        atmospheric_scattering);
//    
    // q_dir: flux through a plane perpendicular to the rays of the sun
    // q_diff: flux through any surface
    double q_dir = atmo_light_scattering.direct_fraction * solarR;    // micromol / m^2 / s
    double q_diff = atmo_light_scattering.diffuse_fraction * solarR;  // micromol / m^2 / s
    
        double absorbed_longwave = emissivity_sky * physical_constants::stefan_boltzmann *
        std::pow(conversion_constants::celsius_to_kelvin + temp, 4); 
    // Here we set `heightf = 1`. The value used for `heightf` does not matter,
    // since the canopy height is not used anywhere in this function.
    CanopyLight canopy_light_model{
        q_dir,
        q_diff,
        chil,
        cosine_zenith_angle,
        1,
        k_diffuse,
        LAI,
        leaf_reflectance_nir,
        leaf_reflectance_par,
        leaf_transmittance_nir,
        leaf_transmittance_par,
        par_energy_content,
        par_energy_fraction};

    double lai_per_layer = LAI / nlayers;  // dimensionless
    CanopyPhotosynthesis canopy;

    double gbw_guess{1.2};  // mol / m^2 / s
    
    for (int i = 0; i < nlayers; ++i) {
        // Calculations that are the same for sunlit and shaded leaves

        double cumulative_lai = (0.5 + i) * lai_per_layer;  // midpoint rule
        double cumulative_lai_at_top = i * lai_per_layer;   // windspeed is evaluated at top of layer, not midpoint

        double leafN_lay = leaf_nitrogen_profile(cumulative_lai_at_top, leafN, kpLN);

        double layer_wind_speed = wind_speed_profile(cumulative_lai_at_top, WindSpeed);
        light_profile = canopy_light_model.get_light_profile(cumulative_lai);
        // Calculations for sunlit leaves. First, estimate stomatal conductance
        // by assuming the leaf has the same temperature as the air. Then, use
        // energy balance to get a better temperature estimate using that value
        // of stomatal conductance. Get the final estimate of stomatal
        // conductance using the new value of the leaf temperature.
        double i_dir = light_profile.sunlit_incident_ppfd;       // micromol / m^2 / s
        double j_dir = light_profile.sunlit_absorbed_shortwave;  // J / m^2 / s
        double pLeafsun = light_profile.sunlit_fraction;         // dimensionless. Fraction of LAI that is sunlit.
        double Leafsun = lai_per_layer * pLeafsun;               // dimensionless

        double direct_gsw_estimate =
            c4photoC(
                i_dir, ambient_temperature, ambient_temperature,
                RH, Vcmax_at_25, Alpha, Kparm,
                theta, beta, RL_at_25, b0, b1, Gs_min, StomataWS, Catm,
                atmospheric_pressure, upperT, lowerT,
                gbw_guess)
                .Gs;  // mol / m^2 / s

        energy_balance_outputs et_direct = leaf_energy_balance(
            absorbed_longwave,
            j_dir,
            atmospheric_pressure,
            ambient_temperature,
            gbw_canopy,
            leafwidth,
            RH,
            direct_gsw_estimate,
            layer_wind_speed);

        double leaf_temperature_dir = ambient_temperature + et_direct.Deltat;  // degrees C

        photosynthesis_outputs direct_photo =
            c4photoC(
                i_dir, leaf_temperature_dir, ambient_temperature,
                RH, Vcmax_at_25, Alpha, Kparm,
                theta, beta, RL_at_25, b0, b1, Gs_min, StomataWS, Catm,
                atmospheric_pressure, upperT, lowerT,
                et_direct.gbw_molecular);

        // Calculations for shaded leaves. First, estimate stomatal conductance
        // by assuming the leaf has the same temperature as the air. Then, use
        // energy balance to get a better temperature estimate using that value
        // of stomatal conductance. Get the final estimate of stomatal
        // conductance using the new value of the leaf temperature.
        double i_diff = light_profile.shaded_incident_ppfd;       // micromol / m^2 / s
        double j_diff = light_profile.shaded_absorbed_shortwave;  // J / m^2 / s
        double pLeafshade = light_profile.shaded_fraction;        // dimensionless. Fraction of LAI that is shaded.
        double Leafshade = lai_per_layer * pLeafshade;            // dimensionless

        double diffuse_gsw_estimate =
            c4photoC(
                i_diff, ambient_temperature, ambient_temperature,
                RH, Vcmax_at_25, Alpha, Kparm,
                theta, beta, RL_at_25, b0, b1, Gs_min, StomataWS, Catm,
                atmospheric_pressure, upperT, lowerT,
                gbw_guess)
                .Gs;  // mol / m^2 / s

        energy_balance_outputs et_diffuse = leaf_energy_balance(
            absorbed_longwave,
            j_diff,
            atmospheric_pressure,
            ambient_temperature,
            gbw_canopy,
            leafwidth,
            RH,
            diffuse_gsw_estimate,
            layer_wind_speed);

        double leaf_temperature_diff = ambient_temperature + et_diffuse.Deltat;  // degrees C

        photosynthesis_outputs diffuse_photo =
            c4photoC(
                i_diff, leaf_temperature_diff, ambient_temperature,
                RH, Vcmax_at_25, Alpha, Kparm,
                theta, beta, RL_at_25, b0, b1, Gs_min, StomataWS, Catm,
                atmospheric_pressure, upperT, lowerT,
                et_diffuse.gbw_molecular);

        // Combine sunlit and shaded leaves
        canopy.assim += Leafsun * direct_photo.Assim + Leafshade * diffuse_photo.Assim;             // micromol / m^2 / s
        canopy.transpiration += Leafsun * et_direct.TransR + Leafshade * et_diffuse.TransR;                 // mmol / m^2 / s
        canopy.carboxylation += Leafsun * direct_photo.GrossAssim + Leafshade * diffuse_photo.GrossAssim;  // micromol / m^2 / s
        canopy.photorespiration += Leafsun * direct_photo.Rp + Leafshade * diffuse_photo.Rp;                 // micromol / m^2 / s
        canopy.leaf_respiration += Leafsun * direct_photo.RL + Leafshade * diffuse_photo.RL;                 // micromol / m^2 / s

        canopy.penman += Leafsun * et_direct.EPenman + Leafshade * et_diffuse.EPenman;        // mmol / m^2 / s
        canopy.priestly += Leafsun * et_direct.EPriestly + Leafshade * et_diffuse.EPriestly;    // mmol / m^2 / s
        canopy.stomatal_vapor_conductance += Leafsun * direct_photo.Gs + Leafshade * diffuse_photo.Gs;  // mol / m^2 / s
    }

    // For transpiration, we need to convert mmol / m^2 / s into Mg / ha / hr
    // using the molar mass of water in kg / mol, which can be accomplished by
    // the following conversion factor:
    // (3600 s / hr) * (1e-3 mol / mmol) * (1e-3 Mg / kg) * (1e4 m^2 / ha)
    // = 36 s * mol * Mg * m^2 / (hr * mmol * kg * ha)
    double constexpr cf2 = physical_constants::molar_mass_of_water * 36;  // (Mg / ha / hr) / (mmol / m^2 / s)
    canopy.transpiration *= cf2;
    return canopy;

}

}
#endif
