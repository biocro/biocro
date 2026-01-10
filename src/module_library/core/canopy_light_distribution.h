#ifndef CANOPY_LIGHT_DISTRIBUTION_H
#define CANOPY_LIGHT_DISTRIBUTION_H
#include "atmosphere_light_scattering.h"
namespace PhotoCore {

struct LightProfile {
    struct LightType {
        double fraction;
        double absorbed_ppfd;
        double absorbed_shortwave;
        double incident_nir;
        double incident_ppfd;
    };
    
    double height;                     // m
    LightType shaded;       // micromol / (m^2 leaf) / s
    LightType sunlit;       // micromol / (m^2 leaf) / s
    
};

double thin_layer_absorption(
    double leaf_reflectance,    // dimensionless
    double leaf_transmittance,  // dimensionless
    double incident_light       // Light units such as `micromol / m^2 / s` or
                                //   `J / m^2 / s`
);

double thick_layer_absorption(
    double leaf_reflectance,    // dimensionless
    double leaf_transmittance,  // dimensionless
    double incident_light       // Light units such as `micromol / m^2 / s` or
                                //   `J / m^2 / s`
);

double nir_from_ppfd(
    double ppfd,                // micromol / m^2 / s
    double par_energy_content,  // J / micromol
    double par_energy_fraction  // dimensionless
);

double absorbed_shortwave(
    double incident_nir,            // J / m^2 / s
    double incident_ppfd,           // micromol / m^2 / s
    double par_energy_content,      // J / micromol
    double leaf_reflectance_par,    // dimensionless
    double leaf_transmittance_par,  // dimensionless
    double leaf_reflectance_nir,    // dimensionless
    double leaf_transmittance_nir   // dimensionless
);

double total_radiation(
    double Q_o,    // Light units such as `micromol / m^2 / s` or `J / m^2 / s`
    double k,      // dimensionless
    double alpha,  // dimensionless
    double ell     // dimensionless from m^2 leaf / m^2 ground
);

double downscattered_radiation(
    double Q_ob,   // Light units such as `micromol / m^2 / s` or `J / m^2 / s`
    double k,      // dimensionless
    double alpha,  // dimensionless
    double ell     // dimensionless from m^2 leaf / m^2 ground
);

double shaded_radiation(
    double Q_ob,       // Light units such as `micromol / m^2 / s` or `J / m^2 / s`
    double Q_od,       // same units as `Q_ob`
    double k_direct,   // dimensionless
    double k_diffuse,  // dimensionless
    double alpha,      // dimensionless
    double ell         // dimensionless from m^2 leaf / m^2 ground
);

struct CanopyLight {

    const double ambient_ppfd_beam;       // micromol / (m^2 beam) / s
    const double ambient_ppfd_diffuse;    // micromol / m^2 / s
    const double chil;                    // dimensionless from m^2 / m^2
    const double cosine_zenith_angle;     // dimensionless
    const double heightf;                 // m^-1 from m^2 leaf / m^2 ground / m height
    const double k_diffuse;               // dimensionless
    const double lai;                     // dimensionless from m^2 / m^2
    const double leaf_reflectance_nir;    // dimensionless
    const double leaf_reflectance_par;    // dimensionless
    const double leaf_transmittance_nir;  // dimensionless
    const double leaf_transmittance_par;  // dimensionless
    const double par_energy_content;      // J / micromol
    const double par_energy_fraction;     // dimensionless

    LightProfile get_light_profile(double cumulative_lai) const;
    
    
    CanopyLight(
        double ambient_ppfd_beam,
        double ambient_ppfd_diffuse,
        double chil,                    // dimensionless from m^2 / m^2
        double cosine_zenith_angle,     // dimensionless
        double heightf,                 // m^-1 from m^2 leaf / m^2 ground / m height
        double k_diffuse,               // dimensionless
        double lai,                     // dimensionless from m^2 / m^2
        double leaf_reflectance_nir,    // dimensionless
        double leaf_reflectance_par,    // dimensionless
        double leaf_transmittance_nir,  // dimensionless
        double leaf_transmittance_par,  // dimensionless
        double par_energy_content,      // J / micromol
        double par_energy_fraction      // dimensionless
    );    

    static CanopyLight from_solar(
        double solarR,
        double direct_fraction,
        double diffuse_fraction,
        double chil,                    // dimensionless from m^2 / m^2
        double cosine_zenith_angle,     // dimensionless
        double heightf,                 // m^-1 from m^2 leaf / m^2 ground / m height
        double k_diffuse,               // dimensionless
        double lai,                     // dimensionless from m^2 / m^2
        double leaf_reflectance_nir,    // dimensionless
        double leaf_reflectance_par,    // dimensionless
        double leaf_transmittance_nir,  // dimensionless
        double leaf_transmittance_par,  // dimensionless
        double par_energy_content,      // J / micromol
        double par_energy_fraction      // dimensionless
    );
    
    // computed during initialization by constructor
    const double absorptance_nir;
    const double absorptance_par;
    double k1;
    double k_direct;
    double canopy_direct_transmission_fraction;  // dimensionless

    // Calculate the ambient direct PPFD through a surface parallel to the ground
    double ambient_ppfd_beam_ground;  // micromol / (m^2 ground) / s

    // Calculate the ambient direct PPFD through a unit area of leaf surface
    double ambient_ppfd_beam_leaf;  // micromol / (m^2 leaf) / s

    // Calculate related NIR energy fluxes
    double ambient_nir_beam;         // J / (m^2 beam) / s
    double ambient_nir_beam_ground;  // J / (m^2 ground) / s
    double ambient_nir_diffuse;      // J / (m^2 ground) / s
    double ambient_nir_beam_leaf;
};

}
#endif
