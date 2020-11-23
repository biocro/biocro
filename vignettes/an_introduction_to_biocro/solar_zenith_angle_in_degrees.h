#ifndef SOLAR_ZENITH_ANGLE_IN_DEGREES_H
#define SOLAR_ZENITH_ANGLE_IN_DEGREES_H

#include <cmath> // for acos
#include "../constants.h" // for pi
#include "../modules.h"
#include "../state_map.h"

/**
 *  Given a value for the BioCro variable "cosine_zenith_angle",
 *  compute and output a value for the variable
 *  "zenith_angle_in_degrees" that corresponds to the angle having the
 *  given cosine.
 *
 *  The module allows negative cosine values, corresponding to
 *  positions of the sun below the horizon.  If out-of-range values
 *  for the cosine are given, the output variable is set to NaN
 *  ("not-a-number").
 */
class solar_zenith_angle_in_degrees : public SteadyModule {

 public:
    solar_zenith_angle_in_degrees(const state_map* input_parameters, state_map* output_parameters)
        : SteadyModule{"solar_zenith_angle_in_degrees"},
          cosine_zenith_angle_ip{get_ip(input_parameters, "cosine_zenith_angle")},
          zenith_angle_in_degrees_op{get_op(output_parameters, "zenith_angle_in_degrees")}
        {}
        
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();
   
 private:
    // Pointers to input parameters:
    const double* cosine_zenith_angle_ip;

    // Pointers to output parameters:
    double* zenith_angle_in_degrees_op;
    
    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};


std::vector<std::string> solar_zenith_angle_in_degrees::get_inputs() {
    return {
        "cosine_zenith_angle"
    };
}


std::vector<std::string> solar_zenith_angle_in_degrees::get_outputs() {
    return {
        "zenith_angle_in_degrees"
    };
}

void solar_zenith_angle_in_degrees::do_operation() const {
    double zenith_angle { acos(*cosine_zenith_angle_ip)
                            * 180 / math_constants::pi };
    update(zenith_angle_in_degrees_op, zenith_angle);
}

#endif
