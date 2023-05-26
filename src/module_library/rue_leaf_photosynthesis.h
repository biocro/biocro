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
 *  use efficiency (RUE) model where gross CO2 assimilation is assumed to be
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
 *  Here we use a RUE model where the measure of growth is the leaf-level gross
 *  CO2 assimilation rate (`A_g`) and the measure of light exposure is the
 *  amount of PPFD intercepted by the leaf (`Q`). Both of these quantities are
 *  considered to be instantaneous rates, and the equation relating them is
 *
 *  > `A_g = alpha_rue * Q` (1)
 *
 *  BioCro uses units of micromol / m^2 / s for both `A_g` and `Q`, so the
 *  proportionality constant `alpha_rue` is dimensionless. Alternatively, it can
 *  be considered to have units of C / photon.
 *
 *  Equation (1) was chosen so that this RUE model could be as close as possible
 *  to the more realistic versions in `c3_leaf_photosynthesis` and
 *  `c4_leaf_photosynthesis`, using the following rationale:
 *
 *  - The model should calculate an instantaneous CO2 assimilation rate so it
 *    can be used in place of the other options inside a multi-layered canopy,
 *    allowing the user to switch between RUE and mechanistic models with as few
 *    other changes as possible.
 *
 *  - It makes more sense to use `A_g` than the net CO2 assimilation rate `A_n`
 *    because `A_n` is generally negative when `Q` is zero due to respiration,
 *    a situation that a RUE model with `A_n = alpha_rue * Q` would not be able
 *    to reproduce.
 *
 *  Unlike the C3 and C4 versions of leaf photosynthesis, no attempt was made to
 *  modify `A_n` according to water stress, as this would not be included in a
 *  pure RUE model. (However, in principle, the value of `alpha_rue` could be
 *  modified according to water stress or other factors by a hypothetical
 *  associated module.) In all other respects besides the calculations for
 *  `A_g` and the lack of water stress, this module is the same as the
 *  `c3_leaf_photosynthesis` module:
 *
 *  - It uses the same equation to calculate day respiration from leaf
 *    temperature.
 *
 *  - It uses the Ball-Berry model to calculate stomatal conductance from `A_n`.
 *
 *  - It uses the `c3EvapoTrans()` function to handle transpiration and
 *    determine leaf temperature.
 *
 *  However, it is important to note that assimilation and stomatal conductance
 *  are not determined iteratively since stomatal conductance has no impact on
 *  assimilation, in contrast to the C3 and C4 cases (described by `c3photoC()`
 *  and `c4photoC()` functions, respectively).
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
 */
class rue_leaf_photosynthesis : public direct_module
{
   public:
    rue_leaf_photosynthesis(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input parameters
          incident_ppfd{get_input(input_quantities, "incident_ppfd")},
          alpha_rue{get_input(input_quantities, "alpha_rue")},
          temp{get_input(input_quantities, "temp")},
          rh{get_input(input_quantities, "rh")},
          Rd{get_input(input_quantities, "Rd")},
          b0{get_input(input_quantities, "b0")},
          b1{get_input(input_quantities, "b1")},
          Catm{get_input(input_quantities, "Catm")},
          average_absorbed_shortwave{get_input(input_quantities, "average_absorbed_shortwave")},
          windspeed{get_input(input_quantities, "windspeed")},
          height{get_input(input_quantities, "height")},
          specific_heat_of_air{get_input(input_quantities, "specific_heat_of_air")},
          minimum_gbw{get_input(input_quantities, "minimum_gbw")},
          windspeed_height{get_input(input_quantities, "windspeed_height")},

          // Get pointers to output parameters
          Assim_op{get_op(output_quantities, "Assim")},
          GrossAssim_op{get_op(output_quantities, "GrossAssim")},
          Rp_op{get_op(output_quantities, "Rp")},
          Ci_op{get_op(output_quantities, "Ci")},
          Gs_op{get_op(output_quantities, "Gs")},
          TransR_op{get_op(output_quantities, "TransR")},
          EPenman_op{get_op(output_quantities, "EPenman")},
          EPriestly_op{get_op(output_quantities, "EPriestly")},
          leaf_temperature_op{get_op(output_quantities, "leaf_temperature")},
          gbw_op{get_op(output_quantities, "gbw")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "rue_leaf_photosynthesis"; }

   private:
    // References to input parameters
    double const& incident_ppfd;
    double const& alpha_rue;
    double const& temp;
    double const& rh;
    double const& Rd;
    double const& b0;
    double const& b1;
    double const& Catm;
    double const& average_absorbed_shortwave;
    double const& windspeed;
    double const& height;
    double const& specific_heat_of_air;
    double const& minimum_gbw;
    double const& windspeed_height;

    // Pointers to output parameters
    double* Assim_op;
    double* GrossAssim_op;
    double* Rp_op;
    double* Ci_op;
    double* Gs_op;
    double* TransR_op;
    double* EPenman_op;
    double* EPriestly_op;
    double* leaf_temperature_op;
    double* gbw_op;

    // Main operation
    void do_operation() const;
};

}  // namespace standardBML
#endif
