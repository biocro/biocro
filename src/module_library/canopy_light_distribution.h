#ifndef CANOPY_LIGHT_DISTRIBUTION_H
#define CANOPY_LIGHT_DISTRIBUTION_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "canopy_light_helpers.h"

namespace standardBML
{
class canopy_light_distribution : public direct_module
{
   public:
    canopy_light_distribution(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          atmospheric_pressure{get_input(input_quantities, "atmospheric_pressure")},
          atmospheric_transmittance{get_input(input_quantities, "atmospheric_transmittance")},
          atmospheric_scattering{get_input(input_quantities, "atmospheric_scattering")},
          chil{get_input(input_quantities, "chil")},
          cosine_zenith_angle{get_input(input_quantities, "cosine_zenith_angle")},
          cumulative_lai{get_input(input_quantities, "cumulative_lai")},
          heightf{get_input(input_quantities, "heightf")},
          k_diffuse{get_input(input_quantities, "k_diffuse")},
          lai{get_input(input_quantities, "lai")},
          leaf_reflectance_nir{get_input(input_quantities, "leaf_reflectance_nir")},
          leaf_reflectance_par{get_input(input_quantities, "leaf_reflectance_par")},
          leaf_transmittance_nir{get_input(input_quantities, "leaf_transmittance_nir")},
          leaf_transmittance_par{get_input(input_quantities, "leaf_transmittance_par")},
          par_energy_content{get_input(input_quantities, "par_energy_content")},
          par_energy_fraction{get_input(input_quantities, "par_energy_fraction")},
          solar{get_input(input_quantities, "solar")},

          // Get pointers to output quantities
          height_op{get_op(output_quantities, "height")},
          sunlit_fraction_op{get_op(output_quantities, "sunlit_fraction")},
          sunlit_absorbed_ppfd_op{get_op(output_quantities, "sunlit_absorbed_ppfd")},
          sunlit_absorbed_shortwave_op{get_op(output_quantities, "sunlit_absorbed_shortwave")},
          sunlit_incident_nir_op{get_op(output_quantities, "sunlit_incident_nir")},
          sunlit_incident_ppfd_op{get_op(output_quantities, "sunlit_incident_ppfd")},
          shaded_fraction_op{get_op(output_quantities, "shaded_fraction")},
          shaded_absorbed_ppfd_op{get_op(output_quantities, "shaded_absorbed_ppfd")},
          shaded_absorbed_shortwave_op{get_op(output_quantities, "shaded_absorbed_shortwave")},
          shaded_incident_nir_op{get_op(output_quantities, "shaded_incident_nir")},
          shaded_incident_ppfd_op{get_op(output_quantities, "shaded_incident_ppfd")}
    {
    }

    static string_vector get_inputs()
    {
        return {
            "atmospheric_pressure",       // Pa
            "atmospheric_scattering",     // dimensionless
            "atmospheric_transmittance",  // dimensionless
            "chil",                       // dimensionless from m^2 / m^2
            "cosine_zenith_angle",        // dimensionless
            "cumulative_lai",             // dimensionless from m^2 leaf / m^2 ground
            "heightf",                    // m^-1
            "k_diffuse",                  // dimensionless
            "lai",                        // dimensionless from m^2 / m^2
            "leaf_reflectance_nir",       // dimensionless
            "leaf_reflectance_par",       // dimensionless
            "leaf_transmittance_nir",     // dimensionless
            "leaf_transmittance_par",     // dimensionless
            "par_energy_content",         // J / micromol
            "par_energy_fraction",        // dimensionless
            "solar"                       // micromol / m^2 / s
        };
    }

    static string_vector get_outputs()
    {
        return {
            "height",
            "sunlit_fraction",
            "sunlit_absorbed_ppfd",
            "sunlit_absorbed_shortwave",
            "sunlit_incident_nir",
            "sunlit_incident_ppfd",
            "shaded_fraction",
            "shaded_absorbed_ppfd",
            "shaded_absorbed_shortwave",
            "shaded_incident_nir",
            "shaded_incident_ppfd"};
    }

    static std::string get_name() { return "canopy_light_distribution"; }

   private:
    // References to input quantities
    double const& atmospheric_pressure;       // Pa
    double const& atmospheric_transmittance;  // dimensionless
    double const& atmospheric_scattering;     // dimensionless
    double const& chil;                       // dimensionless from m^2 / m^2
    double const& cosine_zenith_angle;        // dimensionless
    double const& cumulative_lai;             // dimensionless from m^2 leaf / m^2 ground
    double const& heightf;                    // m^-1 from m^2 leaf / m^2 ground / m height
    double const& k_diffuse;                  // dimensionless
    double const& lai;                        // dimensionless from m^2 / m^2
    double const& leaf_reflectance_nir;       // dimensionless
    double const& leaf_reflectance_par;       // dimensionless
    double const& leaf_transmittance_nir;     // dimensionless
    double const& leaf_transmittance_par;     // dimensionless
    double const& par_energy_content;         // J / micromol
    double const& par_energy_fraction;        // dimensionless
    double const& solar;                      // micromol / m^2 / s

    // Pointers to output quantities
    double* height_op;
    double* sunlit_fraction_op;
    double* sunlit_absorbed_ppfd_op;
    double* sunlit_absorbed_shortwave_op;
    double* sunlit_incident_nir_op;
    double* sunlit_incident_ppfd_op;
    double* shaded_fraction_op;
    double* shaded_absorbed_ppfd_op;
    double* shaded_absorbed_shortwave_op;
    double* shaded_incident_nir_op;
    double* shaded_incident_ppfd_op;

    void do_operation() const
    {
        atmosphere_light_scattering light_scattering = {
            cosine_zenith_angle,
            atmospheric_pressure,
            atmospheric_transmittance,
            atmospheric_scattering};

        canopy_light::parameters params =
            {chil,
             cosine_zenith_angle,
             heightf,
             k_diffuse,
             lai,
             leaf_reflectance_nir,
             leaf_reflectance_par,
             leaf_transmittance_nir,
             leaf_transmittance_par,
             par_energy_content,
             par_energy_fraction};

        canopy_light light_model = canopy_light::from_solar(
            solar,
            light_scattering,
            params);

        light_profile profile = light_model.get_light_profile(cumulative_lai);

        update(height_op, profile.height);
        update(sunlit_fraction_op, profile.sunlit.fraction);
        update(sunlit_absorbed_ppfd_op, profile.sunlit.absorbed_ppfd);
        update(sunlit_absorbed_shortwave_op, profile.sunlit.absorbed_shortwave);
        update(sunlit_incident_nir_op, profile.sunlit.incident_nir);
        update(sunlit_incident_ppfd_op, profile.sunlit.incident_ppfd);
        update(shaded_fraction_op, profile.shaded.fraction);
        update(shaded_absorbed_ppfd_op, profile.shaded.absorbed_ppfd);
        update(shaded_absorbed_shortwave_op, profile.shaded.absorbed_shortwave);
        update(shaded_incident_nir_op, profile.shaded.incident_nir);
        update(shaded_incident_ppfd_op, profile.shaded.incident_ppfd);
    }
};

}  // namespace standardBML
#endif
