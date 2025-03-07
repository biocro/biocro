#include <cmath>  // for pow, std::abs
#include "leaf_energy_balance.h"
#include "boundary_layer_conductance.h"
#include "water_and_air_properties.h"  // for TempToCp, dry_air_density, etc
#include "../framework/constants.h"    // for stefan_boltzmann,
                                       // celsius_to_kelvin, molar_mass_of_water

/**
 *  @brief Use Equation 14.5a from Thornley & Johnson (1990) to calculate water
 *  vapor density from water vapor pressure.
 *
 *  This equation is described as follows:
 *
 *  > Before considering the bahavior of eqn (14.4k), we should point out that
 *  > vapour pressure rather than vapour density is frequently used in the
 *  > treatment of evaporation and transpiration (e.g. Monteith 1973, Jones
 *  > 1983). It can be shown (Exercise 14.3) that vapour density and pressure
 *  > are related by
 *  >
 *  >  `rho_v = rho * epsilon * p_v / (P - p_v)`,  (14.5a)
 *  >
 *  > where `P` is the total atmospheric pressure (dry air plus water vapour)
 *  > (Pa), `p_v` is the vapour pressure (or partial vapour pressure) (Pa),
 *  > `rho`, as defined above, is the density of dry air (kg / m^3), and
 *  > `epsilon` is the ratio of the relative molecular mass of water to the
 *  > relative molar mass of dry air (`epsilon` = 0.622 (Exercise 14.3)).
 *
 *  Note: eqn (14.4k) is the Penman-Monteith equation for canopy transpiration
 *  as expressed in this source.
 */
double vapor_density_from_pressure(
    double density_of_dry_air,  // kg / m^3
    double total_pressure,      // Pa
    double vapor_pressure       // Pa
)
{
    // Specify the ratio of the relative molecular mass of water to the relative
    // molecular mass of dry air. See Thornley & Johnson (1990), page 409.
    constexpr double molecular_ratio_water_air = 0.622;  // dimensionless

    return density_of_dry_air * molecular_ratio_water_air *
           vapor_pressure / (total_pressure - vapor_pressure);  // kg / m^3
}

/**
 *  @brief Calculates leaf-level temperature and transpiration rate for a leaf
 *  within a canopy using a Penman-Monteith approach.
 *
 *  Here we consider the path of water vapor to pass through three sequential
 *  barriers: the stomata, the leaf boundary layer, and finally the canopy
 *  boundary layer. Conductance across the leaf boundary layer is calculated
 *  using the `leaf_boundary_layer_conductance_nikolov()` function, while the
 *  other conductances must be provided as inputs.
 *
 *  Leaf temperature and transpiration are calculated using Equations 14.11e
 *  and 14.4c, respectively, from Thornley & Johnson (1990).
 *
 *  @param [in] absorbed_longwave_energy Absorbed light energy in the longwave
 *              (infrared) band (J / m^2 / s)
 *
 *  @param [in] absorbed_shortwave_energy Absorbed light energy in the shortwave (PAR
 *              and near-infrared) band (J / m^2 / s)
 *
 *  @param [in] air_pressure Total air pressure (Pa)
 *
 *  @param [in] air_temperature Bulk air temperature (degrees C)
 *
 *  @param [in] gbw_canopy Conductance to H2O diffusion across the canopy boundary
 *              layer (m / s)
 *
 *  @param [in] leaf_width Characteristic dimension of a typical leaf (m)
 *
 *  @param [in] relative_humidity Relative humidity in the bulk air (dimensionless
 *              from Pa / Pa)
 *
 *  @param [in] stomatal_conductance Conductance to H2O diffusion across the stomata
 *              (mol / m^2 / s)
 *
 *  @param [in] wind_speed Wind speed within the canopy just outside the leaf
 *              boundary layer (m / s)
 */
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
)
{
    // Set some constants
    double constexpr epsilon_s = 1.0;  // dimensionless

    // Get water vapor and air properties based on the air temperature
    const double c_p = TempToCp(air_temperature);                                        // J / kg / K
    const double lambda = water_latent_heat_of_vaporization_henderson(air_temperature);  // J / kg
    const double p_w_sat_air = saturation_vapor_pressure(air_temperature);               // Pa
    const double rho_ta = dry_air_density(air_temperature, air_pressure);                // kg / m^3
    const double s = TempToSFS(air_temperature);                                         // kg / m^3 / K

    // Get the pyschrometric parameter
    const double gamma = rho_ta * c_p / lambda;  // kg / m^3 / K

    // Get vapor density in the ambient air.
    const double p_w_air = p_w_sat_air * relative_humidity;  // Pa

    const double rho_w_air =
        vapor_density_from_pressure(rho_ta, air_pressure, p_w_air);  // kg / m^3

    // Get vapor density deficit
    const double rho_w_sat =
        vapor_density_from_pressure(rho_ta, air_pressure, p_w_sat_air);  // kg / m^3

    const double Delta_rho = rho_w_sat - rho_w_air;  // kg / m^3

    // Get total absorbed light energy (longwave and shortwave)
    const double J_a = absorbed_shortwave_energy + absorbed_longwave_energy;  // J / m^2 / s

    // Make an initial guess for the leaf temperature
    double leaf_temperature{air_temperature + 0.1};  // degrees C

    // Initialize other loop variables; their values will be set during the loop
    double gbw_leaf{};   // m / s
    double gbw{};        // m / s
    double gsw{};        // m / s
    double gw{};         // m / s
    double mv_tl{};      // m^3 / mol
    double Phi_N{};      // J / m^2 / s
    double pm_bottom{};  // J / m^3 / K
    double pm_top{};     // J / m^3
    double R_l{};        // J / m^2 / s

    // Run loop to find the leaf temperature
    double constexpr max_delta{10};  // degrees C
    double old_leaf_temperature{};   // degrees C
    double change_in_tl{};           // degrees C
    int counter{0};

    do {
        // Store leaf temperature from previous loop iteration
        old_leaf_temperature = leaf_temperature;  // degrees C

        // Get longwave energy losses
        R_l = epsilon_s * physical_constants::stefan_boltzmann *
              pow(conversion_constants::celsius_to_kelvin + leaf_temperature, 4);  // J / m^2 / s

        // Get the energy available for transpiration and heat loss
        Phi_N = J_a - R_l;  // J / m^2 / s

        // Get stomatal conductance to water vapor as a mass conductance
        mv_tl = molar_volume(leaf_temperature, air_pressure);  // m^3 / mol
        gsw = stomatal_conductance * mv_tl;                    // m / s

        // Get leaf boundary layer conductance to water vapor
        gbw_leaf = leaf_boundary_layer_conductance_nikolov(
            air_temperature,
            leaf_temperature - air_temperature,
            p_w_air,
            gsw,
            leaf_width,
            wind_speed,
            air_pressure);  // m / s

        // Get the boundary layer conductance and total conductance to water
        // vapor
        gbw = 1.0 / (1.0 / gbw_leaf + 1.0 / gbw_canopy);  // m / s
        gw = 1.0 / (1.0 / gsw + 1.0 / gbw);               // m / s

        // Get the new leaf temperature using the Penman-Monteith equation
        pm_top = Phi_N / gw - lambda * Delta_rho;              // J / m^3
        pm_bottom = lambda * (s + gamma * (1.0 + gbw / gsw));  // J / m^3 / K

        leaf_temperature = air_temperature +
                           std::min(std::max(pm_top / pm_bottom, -max_delta), max_delta);  // degrees C

        // Get the change in leaf temperature relative to the previous iteration
        change_in_tl = std::abs(leaf_temperature - old_leaf_temperature);  // degrees C

    } while ((++counter <= 50) && (change_in_tl > 0.25));

    // Calculate additional outputs
    double const Delta_T = leaf_temperature - air_temperature;  // degrees C
    double const E = (Delta_rho + s * Delta_T) * gw;            // kg / m^2 / s
    double const H = rho_ta * c_p * Delta_T * gbw;              // J / m^2 / s
    double const storage = Phi_N - H - lambda * E;              // J / m^2 / s

    // Relative humidity just outside the leaf boundary layer
    double const RH_canopy = (rho_w_air - E / gbw_canopy) / rho_w_sat;  // dimensionless

    // Potential evapotranspiration can be calculated assuming infinite stomatal
    // conductance; here we call this "Penman transpiration."
    double const EPen = (s * Phi_N + lambda * gamma * gbw * Delta_rho) /
                        (lambda * (s + gamma));  // kg / m^2 / s

    // Evapotranspiration according to Priestly (?)
    double constexpr dryness_coefficient = 1.26;  // dimensionless
    double const EPries = dryness_coefficient * s * Phi_N /
                          (lambda * (s + gamma));  // kg / m^2 / s

    // The transpiration rates here have units of kg / m^2 / s. They can be
    // converted to mmol / m^2 / s using the molar mass of water (in kg / mol)
    // and noting that 1e3 mmol = 1 mol.
    double constexpr cf = 1e3 / physical_constants::molar_mass_of_water;  // mmol / kg for water

    return energy_balance_outputs{
        /* Deltat = */ Delta_T,             // degrees C
        /* E_loss = */ lambda * E,          // J / m^2 / s
        /* EPenman = */ EPen * cf,          // mmol / m^2 / s
        /* EPriestly = */ EPries * cf,      // mmol / m^2 / s
        /* gbw = */ gbw,                    // m / s
        /* gbw_canopy = */ gbw_canopy,      // m / s
        /* gbw_leaf = */ gbw_leaf,          // m / s
        /* gbw_molecular = */ gbw / mv_tl,  // mol / m^2 / s
        /* gsw = */ gsw,                    // m / s
        /* H = */ H,                        // J / m^2 / s
        /* PhiN = */ Phi_N,                 // J / m^2 / s
        /* RH_canopy = */ RH_canopy,        // dimensionless
        /* storage = */ storage,            // J / m^2 / s
        /* TransR = */ E * cf,              // mmol / m^2 / s
        /* iterations = */ counter          // not a physical quantity
    };
}
