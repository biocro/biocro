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
 *
 *  Denoting the zenith angle by \f$\theta_s\f$ and its cosine by
 *  \f$x\f$, the formula used to compute \f$\theta_s\f$ (in degrees)
 *  from \f$x\f$ is
 *  \f[
 *      \theta_s =
        \begin{cases} \arccos(x) \cdot 180/\pi,  & \text{if }-1 \le x \le 1; \\
                      \text{undefined,}          & \text{otherwise.}
        \end{cases}
 *  \f]
 *  where the usual range \f$ 0 \le x \le \pi \f$ for the \f$\arccos\f$
 *  function is used and undefined values are represented by NaN.
 *
 */
class solar_zenith_angle_in_degrees : public direct_module {

 public:
    solar_zenith_angle_in_degrees(const state_map& input_quantities, state_map* output_quantities)
        : direct_module{"solar_zenith_angle_in_degrees"},
          cosine_zenith_angle{get_input(input_quantities, "cosine_zenith_angle")},
          zenith_angle_in_degrees_op{get_op(output_quantities, "zenith_angle_in_degrees")}
        {}

    static string_vector get_inputs();
    static string_vector get_outputs();

 private:
    // References to input parameters:
    const double& cosine_zenith_angle;

    // Pointers to output parameters:
    double* zenith_angle_in_degrees_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};


string_vector solar_zenith_angle_in_degrees::get_inputs() {
    return {
        "cosine_zenith_angle"
    };
}


string_vector solar_zenith_angle_in_degrees::get_outputs() {
    return {
        "zenith_angle_in_degrees"
    };
}

void solar_zenith_angle_in_degrees::do_operation() const {
    double zenith_angle { acos(cosine_zenith_angle)
                            * 180 / math_constants::pi };
    update(zenith_angle_in_degrees_op, zenith_angle);
}

#endif
