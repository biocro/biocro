#include <cmath>
#include <stdexcept>
#include "ball_berry_gs.h"
#include "../framework/constants.h"       // for dr_boundary
#include "../framework/quadratic_root.h"  // for quadratic_root_plus
#include "water_and_air_properties.h"     // for saturation_vapor_pressure

using calculation_constants::eps_zero;
using physical_constants::dr_boundary;

/**
 *  @brief Calculates steady-state stomatal conductance to water vapor using the
 *  Ball-Berry model.
 *
 *  The Ball-Berry is a simple empirical model for the steady-state response of
 *  stomata to external conditions and was first described in Ball and Berry
 *  (1987). The main idea is that stomata open in response to brighter light or
 *  low CO2 availability, and close in response to low humidity (to limit water
 *  losses from transpiration). This idea can be expressed mathematically as
 *
 *  \f[
 *    g_{sw} = b_0 + b_1 \cdot \frac{ A_n \cdot h_s}{C_s} \quad
 *      \text{if} \; A_n \geq 0, \qquad \text{(1)}
 *  \f]
 *
 *  where \f$ g_{sw} \f$ is the stomatal conductance to water vapor diffusion,
 *  \f$ A_n \f$ is the net CO2 assimilation rate, \f$ b_0 \f$ and \f$ b_1 \f$
 *  are the Ball-Berry intercept and slope, \f$ h_s \f$ is the relative humidity
 *  at the leaf surface, and \f$ C_s \f$ is the CO2 concentration at the leaf
 *  surface. When \f$ A_n < 0 \f$, \f$ g_{sw} = b_0 \f$. Here, the net
 *  assimilation is a proxy for for the incident light intensity and captures
 *  the nonlinear response of \f$ g_{sw} \f$ to light. The quantity
 *  \f$ A_n \cdot h_s / C_s \f$ is often called the Ball-Berry index; thus, the
 *  model states that stomatal conductance is a linear function of the
 *  Ball-Berry index, and the model is parameterized by the Ball-Berry slope and
 *  intercept.
 *
 *  Several critiques of this model have been made, and other models of stomatal
 *  response exist; however, the Ball-Berry model remains widely-used due to its
 *  simplicity. For more info, see Tardieu and Davies (1993), Leuning (1995),
 *  and Dewar (2002).
 *
 *  There are several complexities associated with using this equation in crop
 *  growth modeling. One is that \f$ A_n \f$ depends on the CO2 concentration
 *  inside the leaf, so combined models employing the Ball-Berry equation and
 *  mechanistic equations for photosynthesis must generally be solved
 *  iteratively. In BioCro, this is accomplished via the `c3photoC()` and
 *  `c4photoC()` functions.
 *
 *  Another complexity associated with crop growth modeling is that the CO2 and
 *  H2O concentrations at the leaf surface are not usually known beforehand.
 *  Instead, they must be determined from the ambient values, boundary layer
 *  conductances, and fluxes using one-dimensional gas flow equations of the
 *  form \f$ F = g \cdot (c_2 - c_1) \f$, where \f$ F \f$ is a flux, \f$ g \f$
 *  is a conductance, and \f$ c_1 \f$ and \f$ c_2 \f$ are gas concentrations at
 *  two physical locations.
 *
 *  Assuming steady-state conditions, the CO2 flux across the boundary layer is
 *  given by \f$ A_n \f$, allowing us to find \f$ C_s \f$ using a single gas
 *  flow equation:
 *
 *  \f[
 *    C_s = C_a - \frac{A_n}{g_{bc}} = C_a - \frac{A_n \cdot 1.37}{g_{bw}},
 *      \qquad \text{(2)}
 *  \f]
 *
 *  where \f$ C_a \f$ is the ambient CO2 concentration, \f$ g_{bc} \f$ is
 *  the boundary layer conductance to CO2 diffusion, and \f$ g_{bw} \f$ is the
 *  boundary layer conductance to water vapor diffusion. The two conductances
 *  are related by the ratio of diffusivities of CO2 and H2O in the boundary
 *  layer, which is typically taken to be 1.37; for more information, see the
 *  `physical_constants` namespace.
 *
 *  The situation with water vapor is more complicated. The first point is that
 *  water vapor flows along concentration gradients, _not_ relative humidity
 *  gradients. These quantities are related by
 *  \f$ h = P_w / P_{w,sat}(T) = w \cdot P_{tot} / P_{w,sat}(T) \f$, where
 *  \f$ h \f$ is the relative humidity, \f$ P_w \f$ is the partial pressure of
 *  water vapor, \f$ P_{w,sat} \f$ is the saturation water vapor pressure
 *  corresponding to the air temperature \f$ T \f$, and \f$ P_{tot} \f$ is the
 *  total gas pressure. The second point is that the water vapor flux \f$ E \f$
 *  is unknown; instead, we assume that water vapor is fully saturated within
 *  the leaf and that the water vapor flow has reached steady-state conditions.
 *  Thus, we can begin with two gas flow equations corresponding to water vapor
 *  flux across the boundary layer and stomata (\f$ E_b \f$ and \f$ E_s \f$,
 *  respectively):
 *
 *  \f[ E_b = g_{bw} \cdot (w_s - w_a) \qquad \text{(3)} \f]
 *
 *  and
 *
 *  \f[ E_s = g_{sw} \cdot (w_i - w_s), \qquad \text{(4)} \f]
 *
 *  where \f$ w_a \f$, \f$ w_s \f$, and \f$ w_i \f$ are the water vapor
 *  concentrations in the ambient air, at the leaf surface, and in the leaf
 *  interior, respectively. Note that these concentrations can be re-expressed
 *  using the corresponding relative humidities (\f$ h_a \f$, \f$ h_s \f$, and
 *  \f$ h_i \f$) and air temperatures (\f$ T_a \f$, \f$ T_s \f$, and
 *  \f$ T_i \f$) at the same locations as follows:
 *
 *  \f[ w_a = h_a \cdot \frac{P_{w,sat}(T_a)}{P_{tot}}, \qquad \text{(5a)} \f]
 *
 *  \f[ w_s = h_s \cdot \frac{P_{w,sat}(T_s)}{P_{tot}}, \qquad \text{(5b)} \f]
 *
 *  and
 *
 *  \f[
 *    w_i = h_i \cdot \frac{P_{w,sat}(T_i)}{P_{tot}} =
 *      \frac{P_{w,sat}(T_i)}{P_{tot}}, \qquad \text{(5c)}
 *  \f]
 *
 *  where we have set \f$ h_i = 1 \f$ in the latter to reflect the assumption
 *  that water vapor is saturated inside the leaf. The leaf's surface and its
 *  interior spaces are assumed to have the same temperature and hence the same
 *  saturation water vapor pressure, so we can also set
 *
 *  \f[ T_s = T_i = T_l, \qquad \text{5d} \f]
 *
 *  where \f$ T_l \f$ is the leaf temperature.
 *
 *  Now, we can replace\f$ g_{sw} \f$ in Equation `(4)` with the expression
 *  from Equation `(1)`, replace the water vapor concentrations and temperatures
 *  in Equations `(3)` and `(4)` with the expressions from Equation `(5)`, and
 *  equate the two fluxes in Equations `(3)` and `(4)` since they must be equal
 *  under steady-state conditions. Putting this all together, we can write
 *
 *  \f[
 *    \frac{g_{bw}}{P_{tot}} \cdot
 *      \left( h_s \cdot P_{w,sat}(T_l) - h_a \cdot P_{w,sat}(T_a) \right) =
 *      \frac{P_{w,sat}(T_l)}{P_{tot}} \cdot
 *      \big[ b_0 + b_1 \cdot \frac{A_n \cdot h_s}{C_s} \big]
 *      \cdot \left( 1 - h_s \right). \qquad \text{(6)}
 *  \f]
 *
 *  When multiplied out, Equation `(6)` becomes a quadratic equation for
 *  \f$ h_s \f$:
 *
 *  \f[
 *    h_s^2 \cdot \left( b_1 \cdot \frac{A_n}{C_s} \right) +
 *      h_s \cdot \left( b_0 + g_{bw} - b_1 \cdot \frac{A_n}{C_s} \right) -
 *      \left( g_{bw} \cdot h_a \cdot \frac{P_{w,sat}(T_a)}{P_{w,sat}(T_l)} +
 *        b_0 \right) = 0. \qquad \text{(7)}
 *  \f]
 *
 *  Equation `(7)` can be solved for \f$ h_s \f$ using the quadratic formula
 *  \f$ (-b \pm \sqrt{b^2 - 4 \cdot a \cdot c}) / (2 \cdot a) \f$, where
 *
 *  \f[ a = b_1 \cdot \frac{A_n}{C_s}, \qquad \text{(8a)} \f]
 *
 *  \f[ b = b_0 + g_{bw} - b_1 \cdot \frac{A_n}{C_s}, \qquad \text{(8b)} \f]
 *
 *  and
 *
 *  \f[
 *    c = - \left( g_{bw} \cdot h_a \cdot \frac{P_{w,sat}(T_a)}{P_{w,sat}(T_l)} +
 *      b_0 \right) \qquad \text{(8c)}
 *  \f].
 *
 *  Note that with our assumptions, \f$ a \geq 0 \f$ and \f$ c \leq 0 \f$. Thus,
 *  \f$ b^2 - 4 \cdot a \cdot c \geq b^2\f$, and the
 *  \f$ \sqrt{b^2 - 4 \cdot a \cdot c} \f$ term in the quadratic equation is
 *  always larger than (or equal to) \f$ |b| \f$. Thus, the "minus" root from
 *  the quadratic formula corresponds to a phyically-impossible negative value
 *  for \f$ h_s \f$, and we always choose the "plus" root
 *
 *  \f[
 *    h_s = \frac{-b + \sqrt{b^2 - 4 \cdot a \cdot c}}{2 \cdot a} \qquad
 *      \text{(9)}
 *  \f]
 *
 *  with \f$ a \f$, \f$ b \f$, and \f$ c \f$ as defined in Equation `(8)`.
 *
 *  If the leaf temperature is lower than the ambient air temperature, it is
 *  possible for the water vapor concentration at the leaf surface to exceed the
 *  saturation water vapor pressure at the leaf temperature. This would result
 *  in \f$ h_s > 1 \f$, which is not possible. This outcome indicates that dew
 *  would have formed on the leaf surface. We do not have a way to deal with
 *  this in BioCro at the moment, so we force \f$ h_s \leq 1 \f$ in the code.
 *
 *  References:
 *  - [Ball, Woodrow, and Berry. "A Model Predicting Stomatal Conductance and its Contribution
 *    to the Control of Photosynthesis under Different Environmental Conditions" (1987)]
 *    (https://doi.org/10.1007/978-94-017-0519-6_48)
 *  - [Tardieu and Davies. "Integration of hydraulic and chemical signalling in the control of
 *    stomatal conductance and water status of droughted plants." (1993)]
 *    (https://doi.org/10.1111/j.1365-3040.1993.tb00880.x)
 *  - [Leuning. "A critical appraisal of a combined stomatal-photosynthesis model
 *    for C3 plants." (1995)](https://doi.org/10.1111/j.1365-3040.1995.tb00370.x)
 *  - [Dewar. "The Ball–Berry–Leuning and Tardieu–Davies stomatal models: synthesis and extension
 *    within a spatially aggregated picture of guard cell function." (2002)]
 *   (https://doi.org/10.1046/j.1365-3040.2002.00909.x)
 *
 *  @param [in] assimilation Net CO2 assimilation rate \f$ A_n \f$ in units of
 *              mol / m^2 / s.
 *
 *  @param [in] ambient_c Ambient CO2 concentration \f$ C_a \f$ in units of
 *              mol / mol.
 *
 *  @param [in] ambient_rh Ambient relative humidity \f$ h_a \f$ expressed as a
 *              fraction between 0 and 1 (dimensionless from Pa / Pa).
 *
 *  @param [in] bb_offset Ball-Berry offset \f$ b_0 \f$ in units of
 *              mol / m^2 / s.
 *
 *  @param [in] bb_slope Ball-Berry slope \f$ b_1 \f$ (dimensionless from
 *              [mol / m^2 / s] / [mol / m^2 / s]).
 *
 *  @param [in] gbw Boundary layer conductance to water vapor diffusion
 *              \f$ g_{bw} \f$ in units of mol / m^2 / s.
 *
 *  @param [in] leaf_temperature \f$ T_l \f$ in units of degrees C.
 *
 *  @param [in] ambient_air_temperature \f$ T_a \f$ in units of degrees C.
 *
 *  @return Stomatal conductance to water vapor diffusion \f$ g_{sw} \f$ in
 *          units of mmol / m^2 / s
 */
double ball_berry_gs(
    double assimilation,            // mol / m^2 / s
    double ambient_c,               // mol / mol
    double ambient_rh,              // Pa / Pa
    double bb_offset,               // mol / m^2 / s
    double bb_slope,                // dimensionless from [mol / m^2 / s] / [mol / m^2 / s]
    double gbw,                     // mol / m^2 / s
    double leaf_temperature,        // degrees C
    double ambient_air_temperature  // degrees C
)
{
    // If An < 0, set b1 = 0 to ensure that gsw = b0 in Equation (1) as defined
    // above
    if (assimilation < 0) {
        bb_slope = 0.0;  // mol / m^2 / s
    }

    // Determine Cs using Equation (2) as defined above
    const double Cs = ambient_c -
                      (dr_boundary / gbw) * assimilation;  // mol / mol.

    if (Cs < 0.0) {
        throw std::range_error("Thrown in ball_berry_gs: Cs is less than 0.");
    }

    // Calculate some variables that will be used in later equations
    const double acs = assimilation / Cs;  // mol / m^2 / s

    const double swvp_ratio =
        saturation_vapor_pressure(ambient_air_temperature) /
        saturation_vapor_pressure(leaf_temperature);  // dimensionless

    // Calculate hs using Equations (8) and (9) as defined above
    const double a = bb_slope * acs;                              // mol / m^2 / s
    const double b = bb_offset + gbw - a;                         // mol / m^2 / s
    const double c = -ambient_rh * gbw * swvp_ratio - bb_offset;  // mol / m^2 / s

    const double hs = std::min(1.0, quadratic_root_plus(a, b, c));  // dimensionless

    if (hs < 0) {
        throw std::range_error("Thrown in ball_berry_gs: hs is less than 0.");
    }

    // Calculate stomatal conductance using Equation (1) above
    double const gswmol = a * hs + bb_offset;  // mol / m^2 / s

    return gswmol * 1000;  // mmol / m^2 / s
}
