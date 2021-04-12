#ifndef THERMAL_TIME_DEVELOPMENT_RATE_CALCULATOR_H
#define THERMAL_TIME_DEVELOPMENT_RATE_CALCULATOR_H

#include "../modules.h"
#include "../state_map.h"

/**
 * @class thermal_time_development_rate_calculator
 *
 * @brief Calculates hourly plant development rate based on thermal time ranges
 * of emergence, vegetative, and reproductive growth stages.
 *
 * This module is designed to be used with the `development_index module`.
 *
 * Based on growth rate functions from Osborne et al. and uses the following
 * `DVI` ranges:
 *
 * | Development index (DVI)         | Growth stages               |
 * | :-----------------------------: | :-------------------------: |
 * | \f$ -1 \le \text{DVI} < 0 \f$   | sowing to emergence         |
 * | \f$  0 \le \text{DVI} < 1 \f$   | vegetative growth stages    |
 * | \f$  1 \le \text{DVI} < 2 \f$   | reproductive growth stages  |
 *
 *
 * The `development_rate`, \f$ r \f$ is defined as:
 *
 * \f[ r = \frac{t-t_\text{base}}{TT_\text{stage}}, \f]
 *
 * where \f$ t \f$ is the temperature (`temp`), \f$ t_\text{base} \f$ is the
 * base temperature (`tbase`), and \f$ TT_\text{stage} \f$ is the accumulated
 * thermal time for each of the above three growth stages (`TTemr`, `TTveg`,
 * and `TTrep`).
 *
 *
 * ### References:
 *
 *  [Osborne, T. et al. 2015. “JULES-Crop: A Parametrisation of Crops in the Joint UK Land Environment
 *  Simulator.” Geoscientific Model Development 8(4): 1139–55.]
 *  (https://doi.org/10.5194/gmd-8-1139-2015)
 */
class thermal_time_development_rate_calculator : public SteadyModule
{
public:
    thermal_time_development_rate_calculator(
        const state_map* input_parameters,
        state_map* output_parameters
    )
    : SteadyModule{"thermal_time_development_rate_calculator"},

    // Get pointers to input parameters
    DVI{get_input(input_parameters,"DVI")},
    temp{get_input(input_parameters,"temp")},
    tbase{get_input(input_parameters,"tbase")},
    TTemr{get_input(input_parameters,"TTemr")},
    TTveg{get_input(input_parameters, "TTveg")},
    TTrep{get_input(input_parameters, "TTrep")},

    // Get pointers to output parameters
    development_rate_per_hour_op{get_op(output_parameters,"development_rate_per_hour")}

    {}
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

    private:
    // Pointers to input parameters
    const double& DVI;
    const double& temp;
    const double& tbase;
    const double& TTemr;
    const double& TTveg;
    const double& TTrep;

    // Pointers to output parameters
    double* development_rate_per_hour_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;

};

string_vector thermal_time_development_rate_calculator::get_inputs()
{
    return {
        "DVI",    // dimensionless, development index
        "temp",   // degrees C
        "tbase",  // degrees C, base temperature
        "TTemr",  // degrees C * days, thermal time from sowing to emergence
        "TTveg",  // degrees C * days, thermal time of vegetative states
        "TTrep"   // degrees C * days, thermal time of reproductive states
    };
}

string_vector thermal_time_development_rate_calculator::get_outputs()
{
    return {
        "development_rate_per_hour" // hour^-1
    };
}


void thermal_time_development_rate_calculator::do_operation() const {

    // Calculate the development_rate
    double development_rate; // day^-1
    double temp_diff = temp - tbase; // degrees C
    temp_diff = (temp_diff > 0) ? temp_diff : 0; // if temp < tbase, temp_diff = 0

    if (DVI < -1) {
        // error, DVI out of bounds, this should never occur unless initial DVI
        // state is less than -1.
        development_rate = 0;
    } else if (DVI < 0) {
        // 1. Sowing to emergence
        development_rate = temp_diff / TTemr; // day^-1

    } else if (DVI < 1) {
        // 2. Vegetative stages
        development_rate = temp_diff / TTveg; // day^-1

    } else {
        // 3. Reproductive Stages
        development_rate = temp_diff / TTrep; // day^-1

    }

    double development_rate_per_hour = development_rate / 24.0; // hour^-1

    // Update the output parameter list
    update(development_rate_per_hour_op, development_rate_per_hour);
}

#endif
