#ifndef SOIL_SUNLIGHT_H
#define SOIL_SUNLIGHT_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class soil_sunlight
 *
 *  @brief Determines the fraction of ground area under the canopy exposed to
 *  direct sunlight.
 *
 *  The probability that a ray of light reaches the soil (\f$ p_{soil} \f$) can
 *  be expressed as the product of two other probabilities: the probability that
 *  a ray passes through the canopy without hitting a leaf (\f$ p_{canopy} \f$)
 *  and the probability that a ray passes through any litter that may be
 *  covering the ground (\f$ p_{litter} \f$). In turn, the probability of a ray
 *  passing through the litter can be expressed as \f$ 1 - f_{lc} \f$, where
 *  \f$ f_{lc} \f$ is the fraction of the ground area covered by litter.
 *
 *  If the light incident on the upper canopy is uniform, then the fraction of
 *  ground that is exposed to direct sunlight (\f$ f_{soil} \f$) is equivalent
 *  to \f$ p_{soil} \f$. In other words,
 *
 *  > \f$ f_{soil} = p_{canopy} * (1 - f_{lc}) \f$ [Equation `(1)`]
 *
 *  This module implements Equation `(1)` where the values of \f$ p_{canopy} \f$
 *  and \f$ f_{lc} \f$ are taken from the BioCro quantities named
 *  ``'canopy_direct_transmission_fraction'`` and ``'litter_cover_fraction'``.
 *  This module is often used along with the `multilayer_canopy_properties` and
 *  `litter_cover` modules.
 */
class soil_sunlight : public direct_module
{
   public:
    soil_sunlight(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          canopy_direct_transmission_fraction{get_input(input_quantities, "canopy_direct_transmission_fraction")},
          litter_cover_fraction{get_input(input_quantities, "litter_cover_fraction")},

          // Get pointers to output quantities
          soil_sunlit_fraction_op{get_op(output_quantities, "soil_sunlit_fraction")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_sunlight"; }

   private:
    // References to input quantities
    double const& canopy_direct_transmission_fraction;
    double const& litter_cover_fraction;

    // Pointers to output quantities
    double* soil_sunlit_fraction_op;

    // Main operation
    void do_operation() const;
};

string_vector soil_sunlight::get_inputs()
{
    return {
        "canopy_direct_transmission_fraction",  // dimensionless
        "litter_cover_fraction"                 // dimensionless
    };
}

string_vector soil_sunlight::get_outputs()
{
    return {
        "soil_sunlit_fraction"  // dimensionless
    };
}

void soil_sunlight::do_operation() const
{
    update(soil_sunlit_fraction_op, canopy_direct_transmission_fraction *
                                        (1.0 - litter_cover_fraction));
}

}  // namespace standardBML
#endif
