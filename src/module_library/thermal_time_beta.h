#ifndef THERMAL_TIME_BETA_H
#define THERMAL_TIME_BETA_H

#include <cmath>  // for pow
#include "../modules.h"

/**
 * @class thermal_time_beta
 * 
 * @brief Calculates the rate of thermal time accumulation using a beta distribution
 * function.
 * 
 * See Yan, W. & Hunt, L. A. "An Equation for Modelling the Temperature Response
 * of Plants using only the Cardinal Temperatures" Ann Bot 84, 607–614 (1999) and
 * McMaster, G. S. & Moragues, M. "Crop Development Related to Temperature and
 * Photoperiod" in "Encyclopedia of Sustainability Science and Technology" (2018)
 * for an overview of the different methods that can be used for calculating
 * thermal time.
 * 
 * This module is based off Yin et al. "A nonlinear model for crop development as
 * a function of temperature." Agricultural and Forest Meteorology 77, 1–16 (1995).
 * 
 * In that paper, the development rate is given by:
 * 
 *  rate = 0                            :  temp < temp_b || temp > temp_c
 * 
 *  rate = exp(mu) *
 *         (temp - temp_b)^alpha *
 *         (temp_c - temp)^beta         :  otherwise
 * 
 * where temp_b and temp_c are the lower and upper limits of the development range,
 * and mu, alpha, and beta determine the scaling and shape of the curve. The authors
 * also show that the maximum development rate occurs at an optimal temperature
 * 
 *  optimum_temp = (alpha * temp_c + beta * temp_b) / (alpha + beta)
 * 
 * and is given by
 * 
 *  max_rate = exp(mu) * alpha^alpha * beta^beta * 
 *             [(temp_c - temp_b) / (alpha + beta)]^(alpha + beta).
 * 
 * To make this model more consistent with other BioCro thermal time models, we
 * replace the exp(mu) factor by a thermal time scaling factor ttc_scale having
 * units of deg. C * day / hour. We also think of temp_b as a base_temp and temp_c
 * as a max_temp.
 * 
 * This model can be characterized as a nonlinear model having 3 cardinal temperatures.
 * (Note that although only two temperatures -- a base and maximum --  are directly
 * specified, an optimal temperature nevertheless exists and is determined by the
 * values of alpha and beta.) In this sense it is most directly comparable to the
 * thermal_time_bilinear model. However, there is a range of temperatures where
 * the development rate is close to its maximum value and there are actually 4 parameters
 * that define the shape of the curve, so this model also has some similarity to the
 * thermal_time_trilinear model.
 * 
 * Suggestion for choosing a ttc_scale value: taking inspiration from the
 * thermal_time_bilinear and thermal_time_trilinear models, one can require that the
 * maximum development rate should be given by
 * 
 *  max_rate = optimum_temp - base_temp,
 * 
 * where this rate is given in units of °Cd / day = °C. (Note that BioCro derivatives
 * are specified on a per hour basis.)
 * 
 * In this case, one should set
 * 
 *  ttc_scale = (optimum_temp - base_temp) / 
 *              (alpha^alpha * beta^beta * [(max_temp - base_temp) / (alpha + beta)]^(alpha + beta)) / 
 *              24.0.
 * 
 * where optimum_temp is defined, as above, by
 * 
 *  optimum_temp = (alpha * max_temp + beta * base_temp) / (alpha + beta).
 */
class thermal_time_beta : public DerivModule
{
   public:
    thermal_time_beta(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          DerivModule("thermal_time_beta"),
          // Get pointers to input parameters
          temp(get_input(input_parameters, "temp")),
          tbase(get_input(input_parameters, "tbase")),
          tmax(get_input(input_parameters, "tmax")),
          talpha(get_input(input_parameters, "talpha")),
          tbeta(get_input(input_parameters, "tbeta")),
          ttc_scale(get_input(input_parameters, "ttc_scale")),
          // Get pointers to output parameters
          TTc_op(get_op(output_parameters, "TTc"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // References to input parameters
    double const& temp;
    double const& tbase;
    double const& tmax;
    double const& talpha;
    double const& tbeta;
    double const& ttc_scale;

    // Pointers to output parameters
    double* TTc_op;

    // Main operation
    void do_operation() const;
};

std::vector<std::string> thermal_time_beta::get_inputs()
{
    return {
        "temp",      // deg. C
        "tbase",     // deg. C
        "tmax",      // deg. C
        "talpha",    // dimensionless
        "tbeta",     // dimensionless
        "ttc_scale"  // deg. C * day / hr
    };
}

std::vector<std::string> thermal_time_beta::get_outputs()
{
    return {
        "TTc"  // deg. C * day / hr
    };
}

void thermal_time_beta::do_operation() const
{
    // Find the rate of change
    double rate;  // deg. C * day / hr
    if (temp <= tbase) {
        rate = 0.0;
    } else if (temp <= tmax) {
        rate = ttc_scale * pow(temp - tbase, talpha) * pow(tmax - temp, tbeta);
    } else {
        rate = 0.0;
    }

    // Update the output parameter list
    update(TTc_op, rate);
}

#endif
