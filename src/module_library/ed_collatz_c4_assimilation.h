#ifndef ED_COLLATZ_C4_ASSIMILATION_H
#define ED_COLLATZ_C4_ASSIMILATION_H

#include <cmath>  // For pow, exp
#include "../modules.h"

/**
 * @class ed_collatz_assimilation
 * 
 * @brief Uses the equations from Collatz et. al (1992) to calculate the CO2 assimilation rate.
 * Currently only intended for use by Ed.
 * 
 * First, reaction rates are modified from their values at 25 deg. C according to the leaf temperature.
 * 
 * Then, three different CO2 assimilation rates are calculated, corresponding to conditions limited by
 * rubisco, light, and CO2.
 * 
 * Finally, an overall gross assimilation rate is calculated based on these three limits, and net assimilation
 * is found by subtracting leaf respiration.
 */
class ed_collatz_c4_assimilation : public SteadyModule
{
   public:
    ed_collatz_c4_assimilation(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_collatz_c4_assimilation"),
          // Get pointers to input parameters
          collatz_q10_ip(get_ip(input_parameters, "collatz_q10")),
          temperature_leaf_ip(get_ip(input_parameters, "temperature_leaf")),
          collatz_k_ip(get_ip(input_parameters, "collatz_k")),
          collatz_rd_ip(get_ip(input_parameters, "collatz_rd")),
          collatz_vmax_ip(get_ip(input_parameters, "collatz_vmax")),
          collatz_rubisco_temperature_lower_ip(get_ip(input_parameters, "collatz_rubisco_temperature_lower")),
          collatz_rubisco_temperature_upper_ip(get_ip(input_parameters, "collatz_rubisco_temperature_upper")),
          collatz_alpha_ip(get_ip(input_parameters, "collatz_alpha")),
          collatz_PAR_flux_ip(get_ip(input_parameters, "collatz_PAR_flux")),
          mole_fraction_co2_intercellular_ip(get_ip(input_parameters, "mole_fraction_co2_intercellular")),
          collatz_theta_ip(get_ip(input_parameters, "collatz_theta")),
          collatz_beta_ip(get_ip(input_parameters, "collatz_beta")),
          assimilation_adjustment_factor_WS_ip(get_ip(input_parameters, "assimilation_adjustment_factor_WS")),
          // Get pointers to output parameters
          collatz_KT_op(get_op(output_parameters, "collatz_KT")),
          collatz_RT_op(get_op(output_parameters, "collatz_RT")),
          collatz_VT_op(get_op(output_parameters, "collatz_VT")),
          assimilation_carbon_limited_op(get_op(output_parameters, "assimilation_carbon_limited")),
          assimilation_rubisco_limited_op(get_op(output_parameters, "assimilation_rubisco_limited")),
          assimilation_light_limited_op(get_op(output_parameters, "assimilation_light_limited")),
          assimilation_gross_op(get_op(output_parameters, "assimilation_gross")),
          respiration_op(get_op(output_parameters, "respiration")),
          assimilation_net_op(get_op(output_parameters, "assimilation_net"))

    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* collatz_q10_ip;
    const double* temperature_leaf_ip;
    const double* collatz_k_ip;
    const double* collatz_rd_ip;
    const double* collatz_vmax_ip;
    const double* collatz_rubisco_temperature_lower_ip;
    const double* collatz_rubisco_temperature_upper_ip;
    const double* collatz_alpha_ip;
    const double* collatz_PAR_flux_ip;
    const double* mole_fraction_co2_intercellular_ip;
    const double* collatz_theta_ip;
    const double* collatz_beta_ip;
    const double* assimilation_adjustment_factor_WS_ip;
    // Pointers to output parameters
    double* collatz_KT_op;
    double* collatz_RT_op;
    double* collatz_VT_op;
    double* assimilation_carbon_limited_op;
    double* assimilation_rubisco_limited_op;
    double* assimilation_light_limited_op;
    double* assimilation_gross_op;
    double* respiration_op;
    double* assimilation_net_op;
    // Main operation
    void do_operation() const override;
};

std::vector<std::string> ed_collatz_c4_assimilation::get_inputs()
{
    return {
        "collatz_q10",                        // dimensionless
        "temperature_leaf",                   // deg. C
        "collatz_k",                          // mol / m^2 / s
        "collatz_rd",                         // mol / m^2 / s
        "collatz_vmax",                       // mol / m^2 / s
        "collatz_rubisco_temperature_lower",  // deg. C
        "collatz_rubisco_temperature_upper",  // deg. C
        "collatz_alpha",                      // dimensionless
        "collatz_PAR_flux",                   // mol / m^2 / s
        "mole_fraction_co2_intercellular",    // mol / mol
        "collatz_theta",                      // dimensionless
        "collatz_beta",                       // dimensionless
        "assimilation_adjustment_factor_WS"   // dimensionless
    };
}

std::vector<std::string> ed_collatz_c4_assimilation::get_outputs()
{
    return {
        "collatz_KT",                    // mol / m^2 / s
        "collatz_RT",                    // mol / m^2 / s
        "collatz_VT",                    // mol / m^2 / s
        "assimilation_carbon_limited",   // mol / m^2 / s
        "assimilation_rubisco_limited",  // mol / m^2 / s
        "assimilation_light_limited",    // mol / m^2 / s
        "assimilation_gross",            // mol / m^2 / s
        "respiration",                   // mol / m^2 / s
        "assimilation_net"               // mol / m^2 / s
    };
}

void ed_collatz_c4_assimilation::do_operation() const
{
    // Calculate the temperature factor
    const double temperature_factor = pow(*collatz_q10_ip, (*temperature_leaf_ip - 25.0) / 10.0);

    // Calculate the carbon-limited assimilation rate
    const double co2_rate_constant = *collatz_k_ip * temperature_factor;
    const double assimilation_carbon_limited = co2_rate_constant * *mole_fraction_co2_intercellular_ip;

    // Calculate the rubisco-limited assimilation rate
    const double rubisco_capacity_base = *collatz_vmax_ip * temperature_factor;
    const double rubisco_low_temp_inhibition = 1 + exp(0.3 * (*collatz_rubisco_temperature_lower_ip - *temperature_leaf_ip));
    const double rubisco_high_temp_inhibition = 1 + exp(0.3 * (*temperature_leaf_ip - *collatz_rubisco_temperature_upper_ip));
    const double assimilation_rubisco_limited = rubisco_capacity_base / (rubisco_low_temp_inhibition * rubisco_high_temp_inhibition);

    // Calculate the light-limited assimilation rate
    const double assimilation_light_limited = *collatz_alpha_ip * *collatz_PAR_flux_ip;

    // Use the mixing parameter theta to determine the gross assimilation rate limited by rubisco and light
    const double M_a = *collatz_theta_ip;
    const double M_b = -1.0 * (assimilation_rubisco_limited + assimilation_light_limited);
    const double M_c = assimilation_rubisco_limited * assimilation_light_limited;
    const double M_root_term = M_b * M_b - 4 * M_a * M_c;

    double M;
    if (M_a == 0) {
        M = -M_c / M_b;
    } else {
        const double M_plus = (-M_b + sqrt(M_root_term)) / (2 * M_a);
        const double M_minus = (-M_b - sqrt(M_root_term)) / (2 * M_a);
        M = std::min(M_plus, M_minus);
    }

    // Use the mixing parameter beta to determine the gross assimilation rate limited by carbon, rubisco, and light
    const double A_a = *collatz_beta_ip;
    const double A_b = -1.0 * (M + assimilation_carbon_limited);
    const double A_c = M * assimilation_carbon_limited;
    const double A_root_term = A_b * A_b - 4 * A_a * A_c;

    double A;
    if (A_a == 0) {
        A = -A_c / A_b;
    } else {
        const double A_plus = (-A_b + sqrt(A_root_term)) / (2 * A_a);
        const double A_minus = (-A_b - sqrt(A_root_term)) / (2 * A_a);
        A = std::min(A_plus, A_minus);
    }

    // Calculate the temperature-dependent leaf respiration
    const double respiration_base = *collatz_rd_ip * temperature_factor;
    const double respiration_high_temp_inhibition = 1 + exp(0.3 * (*temperature_leaf_ip - 55.0));
    const double respiration = respiration_base / respiration_high_temp_inhibition;

    // Calculate net assimilation rate
    const double assimilation_net = A - respiration;

    // Apply a possible reduction accounting for the effect of water stress on the stomata
    const double adjusted_assimilation_net = assimilation_net * *assimilation_adjustment_factor_WS_ip;

    // Check for error conditions
    std::map<std::string, bool> errors_to_check = {
        {"the quadratic M_a and M_b terms cannot both be zero",         M_a == 0 && M_b == 0},                      // divide by zero
        {"the quadratic M_root_term cannot be negative for M_a != 0",   M_a != 0 && M_root_term < 0},               // imaginary sqrt
        {"the quadratic A_a and A_b terms cannot both be zero",         A_a == 0 && A_b == 0},                      // divide by zero
        {"the quadratic A_root_term cannot be negative for A_a != 0",   A_a != 0 && A_root_term < 0}                // imaginary sqrt
    };

    check_error_conditions(errors_to_check, get_name());

    // Update outputs
    update(collatz_KT_op, co2_rate_constant);
    update(collatz_RT_op, respiration);
    update(collatz_VT_op, assimilation_rubisco_limited);
    update(assimilation_carbon_limited_op, assimilation_carbon_limited);
    update(assimilation_rubisco_limited_op, assimilation_rubisco_limited);
    update(assimilation_light_limited_op, assimilation_light_limited);
    update(assimilation_gross_op, A);
    update(respiration_op, respiration);
    update(assimilation_net_op, adjusted_assimilation_net);
}

#endif
