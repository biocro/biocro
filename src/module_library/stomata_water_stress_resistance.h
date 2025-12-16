#ifndef STOMATA_WATER_STRESS_RESISTANCE_H
#define STOMATA_WATER_STRESS_RESISTANCE_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class stomata_water_stress_resistance
 *
 * @brief Calculates a 0-1 water stress factor that is applied to b0 an b1
 * of the Ball-Berry model. Currently compatible with the Euler solver only.
 *
 * ### Model overview
 * Following the [Van den Honert Equation]
 * (https://doi.org/10.5194/hess-2-31-1998) for plant hydrology,
 * $\Psi_{leaf} = \Psi_{soil} - R_{hydro} * E$, where $\Psi$ is water potential,
 * $E$ is transpiration rate, $R_{hydro}$ is total hydraulic resistance (soil +
 * root + xylem), with units of Pressure per Flux (e.g., $MPa \cdot m^2 \cdot s
 * \cdot mol^{-1}$). Our goal is to estimate the water potential ($\Psi_{leaf}$)
 * and stress in the leaf. To map the physical quantity to the 0-1 stress factor
 * ($f_{ws}$), we assume a critical water potential $\Psi_{crit}$ and use it to
 * scale the above equation, so we have:

 * $$\frac{\Psi_{leaf}}{\Psi_{crit}} = \frac{\Psi_{soil}}{\Psi_{crit}} -  E *
 * \frac{R_{hydro}}{\Psi_{crit}}$$

 * which equals,

 * $$f_{ws} = f_{ws}^{linear} - E *R_{norm}$$
 * Where $R_{norm}$ is the normalized hydraulic resistance (unit: 1/E),
 * representing the fraction cost of moving 1 unit of water. Now our new $f_{ws}$
 * is nonlinearly responding to changes in both supply (soil water) and demand
 * (E). Since E has a clear diurnal signal, $f_{ws}$ also has a diurnal signal.
 * This makes more sense because a plant at noon is likely under more hydraulic
 * tension than a plant at dawn, even if the soil water content hasn't changed.
 * Lastly, if we directly use uptake for this module, there will be a module
 * cyclic issue like this:
 * <p align="center">
 * water_stress → gs → E (uptake) → water_stress
 * </p>
 * Since soil water potential changes slowly (it doesn't jump wildly in 1 hour),
 * the error introduced by using "1-hour-old" uptake data is negligible. Therefore,
 * I use the uptake from the previous hour to calculate stress for the current
 * hour. For this, I added a state variable named `uptake_laststep`, simply
 * tracking the last step's value total uptake (E).
 *
 */
class stomata_water_stress_resistance : public direct_module
{
   public:
    stomata_water_stress_resistance(state_map const& input_quantities,
                                    state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          Catm_ip{get_ip(input_quantities, "Catm")},
          resistance_amplifier_ip{get_ip(input_quantities, "resistance_amplifier")},
          resistance_base_ip{get_ip(input_quantities, "resistance_base")},
          uptake_laststep_ip{get_ip(input_quantities, "uptake_laststep")},
          soil_field_capacity_ip{get_ip(input_quantities, "soil_field_capacity")},
          soil_wilting_point_ip{get_ip(input_quantities, "soil_wilting_point")},
          soil_water_content_ip{get_ip(input_quantities, "soil_water_content")},

          // Get pointers to output quantities
          StomataWS_op{get_op(output_quantities, "StomataWS")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "stomata_water_stress_resistance"; }

   private:
    // Pointers to input quantities
    const double* Catm_ip;
    const double* resistance_amplifier_ip;
    const double* resistance_base_ip;
    const double* uptake_laststep_ip;
    const double* soil_field_capacity_ip;
    const double* soil_wilting_point_ip;
    const double* soil_water_content_ip;

    // Pointers to output quantities
    double* StomataWS_op;

    // Main operation
    void do_operation() const;
};

string_vector stomata_water_stress_resistance::get_inputs()
{
    return {"Catm",
            "resistance_amplifier",
            "resistance_base",
            "uptake_laststep",
            "soil_field_capacity",
            "soil_wilting_point",
            "soil_water_content"};
}

string_vector stomata_water_stress_resistance::get_outputs()
{
    return {"StomataWS"};
}

void stomata_water_stress_resistance::do_operation() const
{
    // starts linear stress a bit lower than the fc, mimicing the "Readily
    // Available Water" plateau Within the RAW plateau, plants can maintain full
    // turgor and achieve maximum potential evapotranspiration without suffering
    // from water deficit. TAW(total available water) = FC-WP RAW_sf lowers TAW
    // so stress starts a bit lower than FC Ref:FAO 1998: Table 22 in Chapter 8
    // - ETc under soil water stress conditions
    // https://www.fao.org/4/x0490e/x0490e0e.htm
    // #chapter%208%20%20%20etc%20under%20soil%20water%20stress%20conditions
    // The value for soybean comes close to 0.7 based the FAO reference.
    // However, it is for irrigation scheduling. Here the value is more like
    // a physiological threshold for when stomata first begin to close.
    // Therefore, I use a higher value than 0.7 because our soil water 
    // rarely go below the content (corresponding to 0.7) to trigger stress 
    const double RAW_sf = 0.9;
    // Normalized Hydraulic Resistance; 1/(ET or t/ha/hr)
    double resistance_base = *resistance_base_ip;
    double sensitivity_exponent = 1.0;
    double Catm = *Catm_ip;
    // eCO2 plants are x times more sensitive to flow/drying
    // As xylem ABA increased (drought signal), the eCO2 plants closed their
    // stomata more aggressively than ambient plants
    // The resistance_amplifier is tunnable to see different 
    // level of eCO2 response.
    if (Catm > 500) sensitivity_exponent = *resistance_amplifier_ip;
    double uptake_laststep = *uptake_laststep_ip;
    double soil_wilting_point = *soil_wilting_point_ip;
    double soil_field_capacity = *soil_field_capacity_ip;
    double soil_water_content = *soil_water_content_ip;
    double slope = 1.0 / (RAW_sf * soil_field_capacity - soil_wilting_point);
    double intercept = 1.0 - RAW_sf * soil_field_capacity * slope;

    double x =
        std::min(std::max(slope * soil_water_content + intercept, 1e-10), 1.0);

    // Dynamic Resistance based on soil water status 
    // At x=1 (Wet): R = base.
    // At x=0.5 (Dry): R = 2^n*base. (StomataWS drops significantly).
    double resistance_dynamic = resistance_base / std::pow(x, sensitivity_exponent);

    // The uptake_laststep is negative. We need the positive 
    double f_ws = x - resistance_dynamic * std::abs(uptake_laststep);
    f_ws = std::min(std::max(f_ws, 1e-10), 1.0);

    // Update the output quantity list
    update(StomataWS_op, f_ws);
}

}  // namespace standardBML
#endif
