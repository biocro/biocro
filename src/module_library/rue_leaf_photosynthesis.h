#ifndef RUE_LEAF_PHOTOSYNTHESIS_H
#define RUE_LEAF_PHOTOSYNTHESIS_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class rue_leaf_photosynthesis
 *
 *  @brief  Calculates leaf photosynthesis parameters using a simple radiation
 *  use efficiency (RUE) model where net CO2 assimilation is assumed to be
 *  directly proportional to the incident photosynthetically active photon flux
 *  density (PPFD).
 *
 *  ### General overview of RUE models
 *
 *  The general RUE concept is based on the empirical observation that a measure
 *  of plant growth is often found to be directly proportional to a measure of
 *  light exposure. There are three distinct factors involved in a RUE model:
 *
 *  - The measure of growth, which can be the net CO2 uptake by the above-ground
 *    portions of the canopy, total above-ground dry matter, total plant dry
 *    matter including roots, etc.
 *
 *  - The measure of light exposure, which can be intercepted solar radiation,
 *    absorbed solar radiation, intercepted photosynthetically active radiation,
 *    etc.
 *
 *  - The time scale over which RUE is evaluated, where growth and light
 *    exposure can be evaluated instantaneously or integrated at an hourly,
 *    daily, weekly, seasonal, or annual basis.
 *
 *  Thus, it is better to think of RUE as a family of models rather than a
 *  single model. See Arkebauer et al. (1994) for more discussion about the
 *  variability of RUE models. Besides the difficulties associated with
 *  comparing RUE proportionality constants that have been determined using
 *  different options for the measures of growth, light exposure, and time,
 *  several other problems associated with RUE models have been noted in the
 *  literature:
 *
 *  - RUE models mix cause and effect to some degree because a larger canopy
 *    intercepts more light simply due to its size; in fact, it is possible to
 *    define a RUE coefficient for non-photosynthesizing organisms such as
 *    chickens (Demetriades-Shah et al. (1992)).
 *
 *  - When RUE is evaluated over a longer time scale such as hours or days,
 *    the apparent proportionality between growth and light interception may be
 *    an artifact of integration; in fact, it is possible to find a linear
 *    relationship between accumulated `X` and accumulated `Y` even if `X` and
 *    `Y` take random values (Demetriades-Shah et al. (1992)).
 *
 *  - RUE must be recharacterized for different crops, locations, years, and
 *    other environmental values, giving it limited predictive power in
 *    situations outside of experience. A crucial shortcoming is its inability
 *    to predict changes in crop photosynthesis that would result from increases
 *    in atmospheric [CO2] (Humphries and Long (1995)).
 *
 *  This model is included in BioCro primarily to highlight the importance of
 *  using mechanistic photosynthesis models and should be avoided in
 *  non-pedagogical situations.
 *
 *  ### This particular RUE model
 *
 *  Here we use a RUE model where the measure of growth is the leaf-level net
 *  CO2 assimilation rate (`A_n`) and the measure of light exposure is the
 *  amount of PPFD intercepted by the leaf (`Q`). Both of these quantities are
 *  considered to be instantaneous rates, and the equation relating them is
 *
 *  > `A_n = alpha_rue * Q` (1)
 *
 *  BioCro uses units of micromol / m^2 / s for both `A_n` and `Q`, so the
 *  proportionality constant `alpha_rue` is dimensionless. Alternatively, it can
 *  be considered to have units of C / photon.
 *
 *  The model calculates an instantaneous CO2 assimilation rate from `Q` so it
 *  can be used in place of the C3 and C4 versions (`c3_leaf_photosynthesis` and
 *  `c4_leaf_photosynthesis`, respectively) inside a multi-layered canopy,
 *  allowing users to switch between RUE and mechanistic models with as few
 *  other changes as possible.
 *
 *  For interchangability with the C3 and C4 versions, other output quantities
 *  are also provided, such as the transpiration rate. However, these always
 *  take the same default value of 0.0, and are not actually calculated based on
 *  any model.
 *
 *  Unlike the C3 and C4 versions, no attempt was made to modify `A_n` according
 *  to water stress. However, in principle, the value of `alpha_rue` could be
 *  modified according to water stress or other factors by a hypothetical
 *  associated module.
 *
 *  This version of the RUE model has been simplified compared to the one
 *  originally presented in Lochocki et al. (2022).
 *
 *  ### Sources
 *
 *  - Arkebauer, T. J., Weiss, A., Sinclair, T. R. & Blum, A. "In defense of
 *    radiation use efficiency: a response to Demetriades-Shah et al. (1992)"
 *    [Agricultural and Forest Meteorology 68, 221–227 (1994)]
 *    (https://doi.org/10.1016/0168-1923(94)90038-8)
 *
 *  - Demetriades-Shah, T. H., Fuchs, M., Kanemasu, E. T. & Flitcroft, I.
 *    "A note of caution concerning the relationship between cumulated intercepted
 *    solar radiation and crop growth" [Agricultural and Forest Meteorology 58,
 *    193–207 (1992)](https://doi.org/10.1016/0168-1923(92)90061-8)
 *
 *  - Humphries, S. W. & Long, S. P. "WIMOVAC: a software package for modelling
 *    the dynamics of plant leaf and canopy photosynthesis" [Bioinformatics 11,
 *    361–371 (1995)](https://doi.org/10.1093/bioinformatics/11.4.361)
 *
 *  - Lochocki, E. B. et al. "BioCro II: a Software Package for Modular Crop
 *    Growth Simulations."[in silico Plants diac003(2022)]
 *    (https://doi.org/10.1093/insilicoplants/diac003)

 */
class rue_leaf_photosynthesis : public direct_module
{
   public:
    rue_leaf_photosynthesis(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input parameters
          alpha_rue{get_input(input_quantities, "alpha_rue")},
          incident_ppfd{get_input(input_quantities, "incident_ppfd")},

          // Get pointers to output parameters
          Assim_op{get_op(output_quantities, "Assim")},
          GrossAssim_op{get_op(output_quantities, "GrossAssim")},
          Gs_op{get_op(output_quantities, "Gs")},
          RL_op{get_op(output_quantities, "RL")},
          Rp_op{get_op(output_quantities, "Rp")},
          TransR_op{get_op(output_quantities, "TransR")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "rue_leaf_photosynthesis"; }

   private:
    // References to input parameters
    double const& alpha_rue;
    double const& incident_ppfd;

    // Pointers to output parameters
    double* Assim_op;
    double* GrossAssim_op;
    double* Gs_op;
    double* RL_op;
    double* Rp_op;
    double* TransR_op;

    // Main operation
    void do_operation() const;
};

}  // namespace standardBML
#endif
