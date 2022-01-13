#ifndef ED_LEAF_TEMPERATURE_H
#define ED_LEAF_TEMPERATURE_H

#include <cmath>           // for fabs
#include "../framework/constants.h"  // for eps_zero, molar_mass_of_water
#include "../framework/module.h"
#include "../framework/state_map.h"

/**
 * @class ed_leaf_temperature
 *
 * @brief Calculates leaf temperature from energy balance without using the
 * Penman-Monteith approximation. Currently only intended for use by Ed.
 *
 * A leaf absorbs energy from the sun and the surrounding air, and loses energy
 * via blackbody radiation, sensible heat, and water evaporation. At
 * steady-state these energy terms must all balance to zero:
 *
 * J_a - R_L - H - lambda * E = 0,  (1)
 *
 * where J_a is the absorbed solar radiation (J / m^2 / s), R_L (J / m^2 / s) is
 * the net energy loss due to blackbody radiation, H (J / m^2 / s) is the
 * sensible heat flux, lambda is the molar latent heat of vaporization for water
 * (J / mol / K), and E is the water evapotranspiration rate (mol / m^2 / s).
 *
 * We can express the sensible heat loss as
 *
 * H = c_p * g_b * (T_l - T_a),  (2)
 *
 * where c_p is the molar specific heat capacity of air (J / mol / K), g_b is
 * the boundary layer conductance for heat (mol / m^2 / s), T_l is the leaf
 * temperature (degrees C), and T_a (degrees C) is the ambient air temperature.
 *
 * Inserting Equation (1) into Equation (2), we can solve for the leaf
 * temperature:
 *
 * T_l = T_a + (J_a - R_L - lambda * E) / (c_p * g_b).  (3)
 *
 * The evapotranspiration rate can be determined according to:
 *
 * E = g_s * (w_s - w_i),  (4)
 *
 * where g_s is the stomatal conductance to water (mol / m^2 / s), w_s is the
 * mole fraction of water vapor at the leaf surface (mol / mol), and w_i is the
 * mole fraction of water vapor inside the leaf (mol / mol).
 *
 * Finally, we assume that the boundary layer conductances for water and heat
 * are identical following Equation 14.4j in Thornley. On page 408, he writes:
 * "For turbulent flow, transport of both heat and water vapor is dominated by
 * eddy diffusion so that the processes are the same."
 *
 * Note that Campbell & Norman take a more nuanced approach, as seen in Table
 * 7.6 on page 109. Here the conductances for heat and vapor transport are the
 * same in the "free" convection regime where buoyancy effects dominate, but in
 * the "forced" convection regime the heat conductance is smaller by a factor of
 * 0.135 / 0.147 = 0.918. In the future we may wish to account for this by
 * adding boundary layer conductance to heat as an additional output from the
 * boundary layer conductance module.
 *
 * Note that several terms on the right hand side of Equation 3 depend on the
 * leaf temperature, either explicitly or implicitly. Therefore, it is expected
 * that a self-consistent value for the leaf temperature will be found using an
 * se_solver.
 */
class ed_leaf_temperature : public direct_module
{
   public:
    ed_leaf_temperature(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Get references to input quantities
          long_wave_energy_loss_leaf{get_input(input_quantities, "long_wave_energy_loss_leaf")},
          solar_energy_absorbed_leaf{get_input(input_quantities, "solar_energy_absorbed_leaf")},
          latent_heat_vaporization_of_water{get_input(input_quantities, "latent_heat_vaporization_of_water")},
          specific_heat_of_air{get_input(input_quantities, "specific_heat_of_air")},
          conductance_boundary_h2o{get_input(input_quantities, "conductance_boundary_h2o")},
          conductance_stomatal_h2o{get_input(input_quantities, "conductance_stomatal_h2o")},
          mole_fraction_h2o_leaf_surface{get_input(input_quantities, "mole_fraction_h2o_leaf_surface")},
          mole_fraction_h2o_intercellular{get_input(input_quantities, "mole_fraction_h2o_intercellular")},
          temperature_air{get_input(input_quantities, "temp")},

          // Get pointers to output quantities
          temperature_leaf_op{get_op(output_quantities, "temperature_leaf")},
          transpiration_rate_op{get_op(output_quantities, "transpiration_rate")},
          transpiration_energy_op{get_op(output_quantities, "transpiration_energy")},
          sensible_heat_loss_op{get_op(output_quantities, "sensible_heat_loss")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "ed_leaf_temperature"; }

   private:
    // References to input quantities
    double const& long_wave_energy_loss_leaf;
    double const& solar_energy_absorbed_leaf;
    double const& latent_heat_vaporization_of_water;
    double const& specific_heat_of_air;
    double const& conductance_boundary_h2o;
    double const& conductance_stomatal_h2o;
    double const& mole_fraction_h2o_leaf_surface;
    double const& mole_fraction_h2o_intercellular;
    double const& temperature_air;

    // Pointers to output quantities
    double* temperature_leaf_op;
    double* transpiration_rate_op;
    double* transpiration_energy_op;
    double* sensible_heat_loss_op;

    // Main operation
    void do_operation() const override;
};

string_vector ed_leaf_temperature::get_inputs()
{
    return {
        "long_wave_energy_loss_leaf",         // J / m^2 / s
        "solar_energy_absorbed_leaf",         // J / m^2 / s
        "latent_heat_vaporization_of_water",  // J / kg
        "specific_heat_of_air",               // J / kg / K
        "conductance_boundary_h2o",           // mol / m^2 / s
        "conductance_stomatal_h2o",           // mol / m^2 / s
        "mole_fraction_h2o_leaf_surface",     // dimensionless from mol / mol
        "mole_fraction_h2o_intercellular",    // dimensionless from mol / mol
        "temp",                               // deg. C
    };
}

string_vector ed_leaf_temperature::get_outputs()
{
    return {
        "temperature_leaf",      // deg. C
        "transpiration_rate",    // mol / m^2 / s
        "transpiration_energy",  // J / m^2 / s
        "sensible_heat_loss"     // J / m^2 / s
    };
}

void ed_leaf_temperature::do_operation() const
{
    // Determine the energy used for evaporating water from the leaf's interior
    // surfaces
    double const evapotranspiration_rate =
        conductance_stomatal_h2o *
        (mole_fraction_h2o_intercellular - mole_fraction_h2o_leaf_surface);  // mol / m^2 / s

    double const et_energy =
        evapotranspiration_rate * physical_constants::molar_mass_of_water *
        latent_heat_vaporization_of_water;  // J / m^2 / s

    // Apply energy balance to calculate the sensible heat loss
    double const sensible_heat_loss =
        solar_energy_absorbed_leaf - long_wave_energy_loss_leaf - et_energy;  // J / m^2 / s

    // Use the sensible heat loss to calculate the leaf temperature
    double const molar_mass_air = 28.97e-3;  // kg / mol
    double const delta_t = sensible_heat_loss /
                           specific_heat_of_air /
                           molar_mass_air /
                           conductance_boundary_h2o;            // degrees C
    double const temperature_leaf = temperature_air + delta_t;  // degrees C

    // Check for error conditions
    std::map<std::string, bool> errors_to_check = {
        {"conductance_boundary_h2o cannot be zero", fabs(conductance_boundary_h2o) < calculation_constants::eps_zero}  // divide by zero
    };

    check_error_conditions(errors_to_check, get_name());

    update(temperature_leaf_op, temperature_leaf);
    update(transpiration_rate_op, evapotranspiration_rate);
    update(transpiration_energy_op, et_energy);
    update(sensible_heat_loss_op, sensible_heat_loss);
}

#endif
