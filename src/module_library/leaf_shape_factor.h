#ifndef LEAF_SHAPE_FACTOR_H
#define LEAF_SHAPE_FACTOR_H

#include <limits>
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class leaf_shape_factor
 *
 * @brief Determines the leaf shape factor `k` from the solar zenith angle and
 * the leaf shape chi_l (expressed as the ratio between horizontal and vertical
 * leaf elements, where chi = 0, 1, and infinity for horizontal, spherical, and
 * vertical leaf distributions, respectively).
 *
 * The shape factor is the ratio of the leaf shadow area projected on a
 * horizontal plane (A_h) and the leaf surface area (A), i.e., k = A_h / A.
 *
 * See sections 7.1.1 and 7.1.1.1 in Monteith & Unsworth (2013) for a discussion
 * of its meaning and section 8.1.1.1 for a simple application to plant canopies
 * where its use as an extinction coefficient is explained.
 *
 * Here we calculate k using Eq. 15.4 in Campbell & Norman (1998), where we make
 * use of the fact that if x = cos(theta), then tan^2(theta) = (1 - x^2) / x^2.
 *
 * Note that although k is nominally dimensionless (m^2 / m^2), we should be
 * careful to distinguish between ground and leaf area. With this in mind, k has
 * units (m^2 ground) / (m^2 leaf).
 *
 * Also note that as cos_zenith_angle approaches 0, the leaf shadow area A_h
 * becomes infinite and so does the leaf shape factor k. This is the expected
 * behavior and isn't an actual error.
 */
class leaf_shape_factor : public direct_module
{
   public:
    leaf_shape_factor(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          cosine_zenith_angle{get_input(input_quantities, "cosine_zenith_angle")},
          chil{get_input(input_quantities, "chil")},

          // Get pointers to output quantities
          leaf_shape_factor_op{get_op(output_quantities, "leaf_shape_factor")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "leaf_shape_factor"; }

   private:
    // References to input quantities
    double const& cosine_zenith_angle;
    double const& chil;

    // Pointers to output quantities
    double* leaf_shape_factor_op;

    // Main operation
    void do_operation() const;
};

string_vector leaf_shape_factor::get_inputs()
{
    return {
        "cosine_zenith_angle",  // dimensionless
        "chil"                  // dimensionless
    };
}

string_vector leaf_shape_factor::get_outputs()
{
    return {
        "leaf_shape_factor"  // (m^2 ground) / (m^2 leaf)
    };
}

void leaf_shape_factor::do_operation() const
{
    // Calculate the leaf shape factor
    double LSF;
    if (cosine_zenith_angle == 0) {
        LSF = std::numeric_limits<double>::infinity();
    } else {
        const double k_numer =
            sqrt(chil * chil + (1.0 - cosine_zenith_angle * cosine_zenith_angle) /
                                   (cosine_zenith_angle * cosine_zenith_angle));
        const double k_denom = chil + 1.744 * pow((chil + 1.182), -0.733);
        LSF = k_numer / k_denom;
    }

    // Update the output
    update(leaf_shape_factor_op, LSF);
}

}  // namespace standardBML
#endif
