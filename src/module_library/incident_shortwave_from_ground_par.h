#ifndef INCIDENT_SHORTWAVE_FROM_GROUND_PAR_H
#define INCIDENT_SHORTWAVE_FROM_GROUND_PAR_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "../framework/constants.h"
#include "canopy_light_helpers.h"  // for nir_from_ppfd

namespace standardBML
{
/**
 *  @class incident_shortwave_from_ground_par
 *
 *  @brief Uses a value for total PAR flux density measured at the Earth's
 *  surface to calculate the total light flux density in the direct beam and as
 *  diffuse radiation in both the PAR and NIR bands.
 *
 *  Here the input quantity `solar` represents the total flux density of PAR
 *  photons as measured by a pyranometer or other similar instrument. It should
 *  be a `global` flux density that includes direct and diffuse light, and it
 *  should represent flux through a plane perpendicular to the beam direction.
 *  For consistency with typical BioCro units, this flux density should be
 *  reported in units of micromol / m^2 / s. However, the output quantities will
 *  be converted into the SI standard energy flux density units of J / m^2 / s
 *  or, equivalently, W / m^2.
 *
 *  The `par_energy_fraction` quantity should represent the fraction of solar
 *  energy that lies in the PAR band. This value is often near 0.5.
 */
class incident_shortwave_from_ground_par : public direct_module
{
   public:
    incident_shortwave_from_ground_par(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          irradiance_diffuse_fraction{get_input(input_quantities, "irradiance_diffuse_fraction")},
          irradiance_direct_fraction{get_input(input_quantities, "irradiance_direct_fraction")},
          par_energy_content{get_input(input_quantities, "par_energy_content")},
          par_energy_fraction{get_input(input_quantities, "par_energy_fraction")},
          solar{get_input(input_quantities, "solar")},

          // Get pointers to output quantities
          nir_incident_diffuse_op{get_op(output_quantities, "nir_incident_diffuse")},
          nir_incident_direct_op{get_op(output_quantities, "nir_incident_direct")},
          par_incident_diffuse_op{get_op(output_quantities, "par_incident_diffuse")},
          par_incident_direct_op{get_op(output_quantities, "par_incident_direct")},
          ppfd_incident_diffuse_op{get_op(output_quantities, "ppfd_incident_diffuse")},
          ppfd_incident_direct_op{get_op(output_quantities, "ppfd_incident_direct")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "incident_shortwave_from_ground_par"; }

   private:
    // References to input quantities
    double const& irradiance_diffuse_fraction;
    double const& irradiance_direct_fraction;
    double const& par_energy_content;
    double const& par_energy_fraction;
    double const& solar;

    // Pointers to output quantities
    double* nir_incident_diffuse_op;
    double* nir_incident_direct_op;
    double* par_incident_diffuse_op;
    double* par_incident_direct_op;
    double* ppfd_incident_diffuse_op;
    double* ppfd_incident_direct_op;

    // Main operation
    void do_operation() const;
};

string_vector incident_shortwave_from_ground_par::get_inputs()
{
    return {
        "irradiance_diffuse_fraction",  // dimensionless
        "irradiance_direct_fraction",   // dimensionless
        "par_energy_content",           // J / micromol
        "par_energy_fraction",          // dimensionless
        "solar"                         // micromol / (m^2 beam) / s [area perpendicular to beam]
    };
}

string_vector incident_shortwave_from_ground_par::get_outputs()
{
    return {
        "nir_incident_diffuse",   // J / m^2 / s        [through any plane]
        "nir_incident_direct",    // J / (m^2 beam) / s [area perpendicular to beam]
        "par_incident_diffuse",   // J / m^2 / s        [through any plane]
        "par_incident_direct",    // J / (m^2 beam) / s [area perpendicular to beam]
        "ppfd_incident_diffuse",  // J / m^2 / s        [through any plane]
        "ppfd_incident_direct"    // J / (m^2 beam) / s [area perpendicular to beam]
    };
}

void incident_shortwave_from_ground_par::do_operation() const
{
    // Calculate the photosynthetically active photon flux density in the direct
    // beam and the diffuse radiation
    double const ppfd_incident_diffuse = solar * irradiance_diffuse_fraction;  // micromol / m^2 / s
    double const ppfd_incident_direct = solar * irradiance_direct_fraction;    // micromol / m^2 / s

    // Calculate the corresponding energy flux densities in the PAR and NIR
    // bands for direct and diffuse radiation
    double const par_incident_diffuse = ppfd_incident_diffuse * par_energy_content;  // J / m^2 / s
    double const par_incident_direct = ppfd_incident_direct * par_energy_content;    // J / m^2 / s

    double const nir_incident_diffuse =
        nir_from_ppfd(ppfd_incident_diffuse, par_energy_content, par_energy_fraction);  // J / m^2 / s

    double const nir_incident_direct =
        nir_from_ppfd(ppfd_incident_direct, par_energy_content, par_energy_fraction);  // J / m^2 / s

    // Check for error conditions
    std::map<std::string, bool> errors_to_check = {
        {"par_energy_fraction cannot be zero", par_energy_fraction == 0}  // divide by zero
    };

    check_error_conditions(errors_to_check, get_name());

    // Update outputs
    update(nir_incident_diffuse_op, nir_incident_diffuse);
    update(nir_incident_direct_op, nir_incident_direct);
    update(par_incident_diffuse_op, par_incident_diffuse);
    update(par_incident_direct_op, par_incident_direct);
    update(ppfd_incident_diffuse_op, ppfd_incident_diffuse);
    update(ppfd_incident_direct_op, ppfd_incident_direct);
}

}  // namespace standardBML
#endif
