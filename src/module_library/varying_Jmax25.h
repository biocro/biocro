#ifndef VARYING_JMAX25_H
#define VARYING_JMAX25_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class varying_Jmax25
 *
 *  @brief Allows Jmax (at 25 degrees C) to vary based on development index.
 *
 *  Here we wish to specify separate values of \f$ J_{max} \f$ (at 25 degrees C)
 *  for mature and emerging plants, where the value for mature plants is
 *  \f$ J_{max}^{mature} \f$ and the value for emerging plants is
 *  \f$ J_{max}^{emerging} = f_{emerging} \cdot J_{max}^{mature} \f$. Here
 *  \f$ f_{emerging} \f$ is a scaling factor that is typically less than 1; in
 *  other words, \f$ J_{max}^{mature} \f$ is typically larger than
 *  \f$ J_{max}^{emerging} \f$.
 *
 *  We would like to specify a value of \f$ DVI \f$ where this transition
 *  occurs (\f$ DVI_0 \f$), and we wish the transition to be sudden but
 *  continuous. One way to accomplish this is to use a logistic function with a
 *  specified transition width \f$ \Delta_{DVI} \f$:
 *
 *  \f[
 *    J_{max}(DVI) = \frac{L}{1 + e^{-k \cdot (DVI - DVI_0) / \Delta_{DVI}}} + C
 *    \qquad \text{Equation (1)}
 *  \f]
 *
 *  Here \f$ L \f$, \f$ k \f$, and \f$ C \f$ are constants whose values we will
 *  soon determine.
 *
 *  When \f$ DVI \f$ approaches \f$ - \infty \f$, \f$ J_{max}(DVI) \f$
 *  approaches \f$ C \f$. This corresponds to the limit of small \f$ DVI \f$,
 *  where the plant should be in the emerging stage. From this, we can see that
 *  \f$ C = J_{max}^{emerging} \f$.
 *
 *  When \f$ DVI \f$ approaches \f$ \infty \f$, \f$ J_{max}(DVI) \f$
 *  approaches \f$ L + C \f$. This corresponds to the limit of large
 *  \f$ DVI \f$, where the plant should be in the mature stage. From this, we
 *  can see that \f$ L + C = J_{max}^{mature} \f$.
 *
 *  With this information, we can now update Equation `(1)` to
 *
 *  \f[
 *    J_{max}(DVI) = (1 - f_{emerging}) \cdot J_{max}^{mature} \cdot
 *      \frac{1}{1 + e^{-k \cdot (DVI - DVI_0) / \Delta_{DVI}}}
 *      + f_{emerging} \cdot J_{max}^{mature}
 *    \qquad \text{Equation (2)}
 *  \f]
 *
 *  Using Equation `(2)` we can see that
 *  \f$ J_{max}(DVI_0) = J_{max}^{mature} \cdot (1 + f_{emerging}) \f$, which is
 *  the average of \f$ J_{max}^{mature} \f$ and \f$ J_{max}^{emerging} \f$.
 *  Thus, \f$ DVI_0 \f$ marks the midpoint of the transition. As \f$ DVI \f$
 *  increases beyond \f$ DVI_0 \f$, \f$ J_{max}(DVI) \f$ gets closer and closer
 *  to the mature value. If we want to specify that the transition is 95%
 *  complete when \f$ DVI = DVI_0 + \Delta_{DVI} \f$, then we can choose
 *  \f$ k = - ln(1 / 0.95 - 1) \f$. (We omit the math here, but it is
 *  straightforward to arrive at this answer.)
 *
 *  Now, we can update the equation one more time:
 *
 *  \f[
 *    J_{max}(DVI) = (1 - f_{emerging}) \cdot J_{max}^{mature} \cdot
 *      \frac{1}{1 + e^{-ln(1 / 0.95 - 1) \cdot (DVI - DVI_0) / \Delta_{DVI}}} +
 *      f_{emerging} \cdot J_{max}^{mature}
 *    \qquad \text{Equation (3)}
 *  \f]
 *
 *  This is the desired function that switches continuously between the two
 *  values, where the transition is centered at \f$ DVI_0 \f$ and has a width of
 *  \f$ \Delta_{DVI} \f$ (in the sense described above).
 *
 *  This module implements Equation `(3)` where the values of
 *  \f$ J_{max}^{mature} \f$, \f$ DVI_0 \f$, \f$ \Delta_{DVI} \f$ and
 *  \f$ f_{emerging} \f$ are taken from the BioCro quantities called
 *  ``'jmax_mature'``, ``'DVI0_jmax'``, ``'Delta_DVI_jmax'``, and ``'sf_jmax'``,
 *  respectively.
 */
class varying_Jmax25 : public direct_module
{
   public:
    varying_Jmax25(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          DVI{get_input(input_quantities, "DVI")},
          jmax_mature{get_input(input_quantities, "jmax_mature")},
          DVI0_jmax{get_input(input_quantities, "DVI0_jmax")},
          D_DVI{get_input(input_quantities, "Delta_DVI_jmax")},
          sf_jmax{get_input(input_quantities, "sf_jmax")},

          // Get pointers to output quantities
          jmax_op{get_op(output_quantities, "jmax")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "varying_Jmax25"; }

   private:
    // Pointers to input quantities
    double const& DVI;
    double const& jmax_mature;
    double const& DVI0_jmax;
    double const& D_DVI;
    double const& sf_jmax;

    // Pointers to output quantities
    double* jmax_op;

    // Main operation
    void do_operation() const;
};

string_vector varying_Jmax25::get_inputs()
{
    return {
        "DVI",             // dimensionless
        "sf_jmax",         // dimensionless
        "DVI0_jmax",       // dimensionless
        "Delta_DVI_jmax",  // dimensionless
        "jmax_mature"      // micromol / m^2 / s
    };
}

string_vector varying_Jmax25::get_outputs()
{
    return {
        "jmax"  // micromol / m^2 / s
    };
}

void varying_Jmax25::do_operation() const
{
    double const k = -log(1.0 / 0.95 - 1.0);  // dimensionless

    double const C = jmax_mature * sf_jmax;        // micromol / m^2 / s
    double const L = jmax_mature * (1 - sf_jmax);  // micromol / m^2 / s

    double const jmax = L / (1.0 + exp(-k * (DVI - DVI0_jmax) / D_DVI)) + C;  // micromol / m^2 / s

    update(jmax_op, jmax);
}

}  // namespace standardBML
#endif
