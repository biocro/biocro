#ifndef GRIMM_SOYBEAN_FLOWERING_CALCULATOR_H
#define GRIMM_SOYBEAN_FLOWERING_CALCULATOR_H

#include "../modules.h"

/**
 * \brief Model for soybean development and flowering based on Grimm et al. (1993)
 * 
 * This module is intended to be used to model soybean development and flowering following
 * the model introduced by Grimm et al. (1993) and further investigated by Piper et al. (1996).
 * 
 * Once the day of year exceeds the sowing date, the plant is assumed to be in a photoperiod-insensitive juvenile stage where
 * physiological days accumulate at a rate solely determined by temperature. Here the temperature dependence is given by a
 * piecewise-linear function defined by four critical temperature values.
 * 
 * Once the plant's physiological age exceeds a threshold value, it enters a photoperiod-sensitive stage where physological
 * days accumulate at a rate determined by both temperature and night length. Here the overall rate is the product of two factors:
 * R = F(T) * F(N), where F(T) is a linear-plateau function of temperature with two critical temperature values and F(N) is a
 * linear-plateau function of night length with two critical night length values.
 * 
 * When the physological age exceeds a third threshold, the plant begins to flower (R1). Here this is represented by the state variable
 * `grimm_flowering,` whose rate of change becomes nonzero when the plant begins to flower.
 * 
 * The Grimm model cites the method from Goudriaan (1978) for calculating day length. Unfortunately the Goudriaan paper includes two
 * day lengths: the astronomical and effective day lengths. The Grimm model does not explicitly mention the effective day length,
 * which seems to be used in Goudriaan for calculating average carbon assimilation by a plant canopy. Here we assume the Grimm model
 * requires the astronomical day length, although a day length correction factor is included in the code in case it is required in
 * the future.
 * 
 * (For the effective day length, sunrise and sunset are defined to occur when the solar zenith angle is equal to 8 degrees, rather
 * than 0 degrees (or -0.53 degrees) as is typically used in astronomical calculations. Using 2018 as an example, we have determined
 * that for 90 <= DOY <= 250 (a range that includes typical sowing and flowering dates in Illinois), the effective day length
 * calculated using the 8 degree method is 88% of the astronomical day length.)
 * 
 * When using oscillators to determine the day length, there is a slight lag compared to the celestial calculations. However, the error
 * during typical Illinois soybean growth days is generally within +/- 30 minutes, so no correction is applied.
 * 
 * References:
 *  `Grimm, S. S., Jones, J. W., Boote, K. J. & Hesketh, J. D. Parameter Estimation for Predicting Flowering Date of Soybean Cultivars. Crop Science 33, 137–144 (1993)`
 *  `Piper, E. L., Boote, K. J., Jones, J. W. & Grimm, S. S. Comparison of Two Phenology Models for Predicting Flowering and Maturity Date of Soybean. Crop Science 36, 1606–1614 (1996)`
 *  `Goudriaan, J. & Van Laar, H. Calculation of dairy totals of the gross CO2 assimilation of leaf canopies. Netherlands Journal of Agricultural Science 26, 373–382 (1978)`
 */
class grimm_soybean_flowering_calculator : public SteadyModule
{
   public:
    grimm_soybean_flowering_calculator(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters) :  // Define basic module properties by passing its name to its parent class
                                                                      SteadyModule("grimm_soybean_flowering_calculator"),
                                                                      // Get pointers to input parameters
                                                                      grimm_sowing_doy_ip(get_ip(input_parameters, "grimm_sowing_doy")),
                                                                      grimm_physiological_age_ip(get_ip(input_parameters, "grimm_physiological_age")),
                                                                      grimm_juvenile_T0_ip(get_ip(input_parameters, "grimm_juvenile_T0")),
                                                                      grimm_juvenile_T1_ip(get_ip(input_parameters, "grimm_juvenile_T1")),
                                                                      grimm_juvenile_T2_ip(get_ip(input_parameters, "grimm_juvenile_T2")),
                                                                      grimm_juvenile_T3_ip(get_ip(input_parameters, "grimm_juvenile_T3")),
                                                                      grimm_juvenile_pd_threshold_ip(get_ip(input_parameters, "grimm_juvenile_pd_threshold")),
                                                                      grimm_T_min_ip(get_ip(input_parameters, "grimm_T_min")),
                                                                      grimm_T_opt_ip(get_ip(input_parameters, "grimm_T_opt")),
                                                                      grimm_N_min_ip(get_ip(input_parameters, "grimm_N_min")),
                                                                      grimm_N_opt_ip(get_ip(input_parameters, "grimm_N_opt")),
                                                                      grimm_flowering_threshold_ip(get_ip(input_parameters, "grimm_flowering_threshold")),
                                                                      doy_dbl_ip(get_ip(input_parameters, "doy_dbl")),
                                                                      temp_ip(get_ip(input_parameters, "temp")),
                                                                      day_length_ip(get_ip(input_parameters, "day_length")),
                                                                      // Get pointers to output parameters
                                                                      grimm_rate_op(get_op(output_parameters, "grimm_rate")),
                                                                      grimm_rate_temperature_op(get_op(output_parameters, "grimm_rate_temperature")),
                                                                      grimm_rate_photoperiod_op(get_op(output_parameters, "grimm_rate_photoperiod")),
                                                                      grimm_flowering_op(get_op(output_parameters, "grimm_flowering"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* grimm_sowing_doy_ip;
    const double* grimm_physiological_age_ip;
    const double* grimm_juvenile_T0_ip;
    const double* grimm_juvenile_T1_ip;
    const double* grimm_juvenile_T2_ip;
    const double* grimm_juvenile_T3_ip;
    const double* grimm_juvenile_pd_threshold_ip;
    const double* grimm_T_min_ip;
    const double* grimm_T_opt_ip;
    const double* grimm_N_min_ip;
    const double* grimm_N_opt_ip;
    const double* grimm_flowering_threshold_ip;
    const double* doy_dbl_ip;
    const double* temp_ip;
    const double* day_length_ip;
    // Pointers to output parameters
    double* grimm_rate_op;
    double* grimm_rate_temperature_op;
    double* grimm_rate_photoperiod_op;
    double* grimm_flowering_op;
    // Main operation
    void do_operation() const;
};

std::vector<std::string> grimm_soybean_flowering_calculator::get_inputs()
{
    return {
        "grimm_sowing_doy",
        "grimm_physiological_age",
        "grimm_juvenile_T0",
        "grimm_juvenile_T1",
        "grimm_juvenile_T2",
        "grimm_juvenile_T3",
        "grimm_juvenile_pd_threshold",
        "grimm_T_min",
        "grimm_T_opt",
        "grimm_N_min",
        "grimm_N_opt",
        "grimm_flowering_threshold",
        "doy_dbl",
        "temp",
        "day_length"};
}

std::vector<std::string> grimm_soybean_flowering_calculator::get_outputs()
{
    return {
        "grimm_rate",
        "grimm_rate_temperature",
        "grimm_rate_photoperiod",
        "grimm_flowering"};
}

void grimm_soybean_flowering_calculator::do_operation() const
{
    // Collect inputs
    const double grimm_sowing_doy = *grimm_sowing_doy_ip;                        // calendar day of year
    const double grimm_physiological_age = *grimm_physiological_age_ip;          // physiological days
    const double grimm_juvenile_T0 = *grimm_juvenile_T0_ip;                      // deg. C
    const double grimm_juvenile_T1 = *grimm_juvenile_T1_ip;                      // deg. C
    const double grimm_juvenile_T2 = *grimm_juvenile_T2_ip;                      // deg. C
    const double grimm_juvenile_T3 = *grimm_juvenile_T3_ip;                      // deg. C
    const double grimm_juvenile_pd_threshold = *grimm_juvenile_pd_threshold_ip;  // physiological days
    const double grimm_T_min = *grimm_T_min_ip;                                  // deg. C
    const double grimm_T_opt = *grimm_T_opt_ip;                                  // deg. C
    const double grimm_N_min = *grimm_N_min_ip;                                  // hours
    const double grimm_N_opt = *grimm_N_opt_ip;                                  // hours
    const double grimm_flowering_threshold = *grimm_flowering_threshold_ip;      // physiological days
    const double doy_dbl = *doy_dbl_ip;                                          // fractional day of year
    const double temp = *temp_ip;                                                // deg. C
    const double day_length = *day_length_ip;                                    // hours

    // Calculate the night length, applying a correction to convert from astronomical day
    constexpr double day_length_correction = 1.0;                           // dimensionless (from effective day length / celestial day length). Set to 0.883 to use the effective day length from Goudriaan (1978).
    const double night_length = 24.0 - day_length * day_length_correction;  // hours

    // Calculate the normalized growth rate based on the day of year, night length, temperature, and growth stage
    double rate_normalized = 0.0;              // dimensionless
    double temperature_rate_normalized = 0.0;  // dimensionless
    double photoperiod_rate_normalized = 0.0;  // dimensionless
    if (doy_dbl < grimm_sowing_doy) {
        // The plant has not been sowed yet, so it can't grow
        rate_normalized = 0.0;
    } else if (grimm_physiological_age < grimm_juvenile_pd_threshold) {
        // The plant is still in the juvenile (photoperiod-insensitive) phase
        if (temp < grimm_juvenile_T0) {
            // The temperature is too low for the plant to grow
            rate_normalized = 0.0;
        } else if (temp < grimm_juvenile_T1) {
            // There is a linear increase from 0 to 1 as temp changes from T0 to T1
            rate_normalized = (temp - grimm_juvenile_T0) / (grimm_juvenile_T1 - grimm_juvenile_T0);
        } else if (temp < grimm_juvenile_T2) {
            // We are in the flat region at maximum growth
            rate_normalized = 1.0;
        } else if (temp < grimm_juvenile_T3) {
            // There is a linear decrease from 1 to 0 as temp changes from T2 to T3
            rate_normalized = 1.0 - (temp - grimm_juvenile_T2) / (grimm_juvenile_T3 - grimm_juvenile_T2);
        } else {
            // The temperature is too high for the plant to grow
            rate_normalized = 0.0;
        }
    } else {
        // The plant has entered the photoperiod-sensitive phase
        if (temp < grimm_T_min) {
            // The temperature is too low for the plant to develop
            temperature_rate_normalized = 0.0;
        } else if (temp < grimm_T_opt) {
            // There is a linear increase from 0 to 1 as temp changes from T_min to T_opt
            temperature_rate_normalized = (temp - grimm_T_min) / (grimm_T_opt - grimm_T_min);
        } else {
            // The development rate is maxed out at 1
            temperature_rate_normalized = 1.0;
        }
        if (night_length < grimm_N_min) {
            // The night is too short for the plant to develop
            photoperiod_rate_normalized = 0.0;
        } else if (night_length < grimm_N_opt) {
            // There is a linear increase from 0 to 1 as night_length changes from N_min to N_opt
            photoperiod_rate_normalized = (night_length - grimm_N_min) / (grimm_N_opt - grimm_N_min);
        } else {
            // The development rate is maxed out at 1
            photoperiod_rate_normalized = 1.0;
        }
        rate_normalized = temperature_rate_normalized * photoperiod_rate_normalized;
    }

    // Decide if the plant is flowering
    const bool is_flowering = (grimm_physiological_age > grimm_juvenile_pd_threshold + grimm_flowering_threshold);

    // Scale the rates to the maximum value, which is 1 physiological day per day = 1 / 24 physiological days per hour
    const double rate_per_hour = rate_normalized / 24.0;
    const double temperature_rate_per_hour = temperature_rate_normalized / 24.0;
    const double photoperiod_rate_per_hour = photoperiod_rate_normalized / 24.0;

    // Update the output parameter list
    update(grimm_rate_op, rate_per_hour);
    update(grimm_rate_temperature_op, temperature_rate_per_hour);
    update(grimm_rate_photoperiod_op, photoperiod_rate_per_hour);
    update(grimm_flowering_op, is_flowering);
}

#endif
