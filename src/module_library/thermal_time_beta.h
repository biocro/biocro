#ifndef THERMAL_TIME_BETA_H
#define THERMAL_TIME_BETA_H

#include <cmath>  // for pow
#include "../modules.h"
#include "../module_helper_functions.h"
#include "../state_map.h"

/**
 * @class thermal_time_beta
 * 
 * @brief Calculates the rate of thermal time accumulation using a beta distribution
 * function.
 * 
 * For an overview of the different methods that can be used for calculating thermal
 * time, see Yan, W. & Hunt, L. A. "An Equation for Modelling the Temperature Response
 * of Plants using only the Cardinal Temperatures" Ann Bot 84, 607–614 (1999) and
 * McMaster, G. S. & Moragues, M. "Crop Development Related to Temperature and
 * Photoperiod" in "Encyclopedia of Sustainability Science and Technology" (2018).
 * 
 * This module is based off Yin et al. "A nonlinear model for crop development as
 * a function of temperature." Agricultural and Forest Meteorology 77, 1–16 (1995).
 * 
 * In this model, the development rate is given by:
 * 
 *  rate = 0                        :  when temp is below tbase
 * 
 *  rate = ttc_scale *
 *         (temp - tbase)^talpha *
 *         (tmax - temp)^tbeta      :  when temp is between tbase and tmax
 * 
 *  rate = 0                        :  when temp is above tmax
 * 
 * where ttc_scale, talpha, and tbeta determine the scaling and shape of the curve.
 * Yin et al. also show that the maximum development rate occurs at an optimal
 * temperature
 * 
 *  topt = (talpha * tmax + tbeta * tbase) / (talpha + tbeta)
 * 
 * and is given by
 * 
 *  max_rate = ttc_scale * talpha^talpha * tbeta^tbeta * 
 *             [(tmax - tbase) / (talpha + tbeta)]^(talpha + tbeta).
 * 
 * This model can be characterized as a nonlinear model having 3 cardinal temperatures.
 * Note that although only two temperatures -- a base and maximum --  are directly
 * specified, an optimal temperature nevertheless exists and is determined by the
 * values of talpha and tbeta. In this sense it is most directly comparable to the
 * thermal_time_bilinear model. However, there is a range of temperatures where
 * the development rate is close to its maximum value and there are actually 4 parameters
 * that define the shape of the curve, so this model also has some similarity to the
 * thermal_time_trilinear model.
 * 
 * Suggestion for choosing a ttc_scale value:
 * 
 * Taking inspiration from the thermal_time_bilinear and thermal_time_trilinear models,
 * one can require that the maximum development rate should be given by
 * 
 *  max_rate = topt - tbase,
 * 
 * where this rate is given in units of degrees C * day / day = degrees C. (Note that
 * BioCro derivatives are specified on a per hour basis, so it will be necessary to
 * include a conversion when calculating ttc_scale.)
 * 
 * In this case, one should set
 * 
 *  ttc_scale = (topt - tbase) / 
 *              (talpha^talpha * tbeta^tbeta * [(tmax - tbase) / (talpha + tbeta)]^(talpha + tbeta)) / 
 *              24.0.
 * 
 * where topt is defined, as above, by
 * 
 *  topt = (talpha * tmax + tbeta * tbase) / (talpha + tbeta).
 */
class thermal_time_beta : public DerivModule
{
   public:
    thermal_time_beta(
        state_map const* input_parameters,
        state_map* output_parameters)
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
        "temp",      // degrees C
        "tbase",     // degrees C
        "tmax",      // degrees C
        "talpha",    // dimensionless
        "tbeta",     // dimensionless
        "ttc_scale"  // degrees C * day / hr
    };
}

std::vector<std::string> thermal_time_beta::get_outputs()
{
    return {
        "TTc"  // degrees C * day / hr
    };
}

void thermal_time_beta::do_operation() const
{
    // Find the rate of change
    double const rate = temp <= tbase ? 0.0
                      : temp <= tmax  ? ttc_scale * pow(temp - tbase, talpha) * pow(tmax - temp, tbeta)
                      :                 0.0;  // degrees C * day / hr

    // Update the output parameter list
    update(TTc_op, rate);
}

#endif
