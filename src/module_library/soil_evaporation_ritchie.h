#ifndef SOIL_EVAPORATION_RITCHIE_H
#define SOIL_EVAPORATION_RITCHIE_H

#include <algorithm>                 // for std::min, std::max
#include <cmath>                     // for pow, std
#include <stdexcept>                 // for std::logic_error, std::range_error
#include "../framework/constants.h"  // for eps_zero
#include "../framework/module.h"
#include "../framework/state_map.h"
#include "soil_evaporation_functions.h"

namespace standardBML
{
/**
 *  @class soil_evaporation_ritchie
 *
 *  @brief Calculates the change in the soil evaporation rate that will occur
 *  over the following (hourly) time step.
 *
 *  ### Model overview
 *
 *  This module is based on a model for soil surface evaporation originally
 *  presented in Ritchie (1972).
 *
 *  Broadly, this model divides soil surface water evaporation into two stages,
 *  referred to as Stage 1 and Stage 2 evaporation. They are described in the
 *  paper:
 *
 *  > In the constant rate stage (stage 1), the soil is sufficiently wet for the
 *  > water to be transported to the surface at a rate at least equal to the
 *  > evaporation potential. In the falling rate stage (stage 2), the surface
 *  > soil water content has decreased below a threshold value, so that `ES`
 *  > depends on the flux of water through the upper layer of soil to the
 *  > evaporating site near the surface.
 *
 *  Here, `ES` the rate of evaporation of water from the soil surface.
 *
 *  In the model, Stage 1 evaporation generally proceeds at its potential rate
 *  `EOS`. Stage 2 evaporation is independent of environmental conditions, and
 *  instead depends on the total amount of time spent in Stage 2 evaporation.
 *
 *  The cumulative soil evaporation during Stage 2 is given by Equation (6) in
 *  the paper:
 *
 *  > `sumes2 = alpha * sqrt(t)`,
 *
 *  where `sumes2` is the sum of evaporation during Stage 2, `t` is the time
 *  spent during Stage 2, and `alpha` is a constant whose value depends on the
 *  type of soil.
 *
 *  With this equation, it is possible to calculate the total water evaporated
 *  during a time interval:
 *
 *  > Delta_sumes2 = alpha * sqrt(t + Delta_t) - sumes2(t)`,
 *
 *  where `Delta_sumes2` is the water evaporated during the interval, `Delta_t`
 *  is the length of the time interval, and `sumes2(t)` is the cumulative
 *  evaporation that had occurred at time `t`. With this, the average rate of
 *  evaporation during the time interval can be found as
 *
 *  > ES = Delta_sumes2 / Delta_t = (alpha * sqrt(t + Delta_t) - sumes2(t)) / Delta_t`
 *
 *  This is Equation (8) in the paper. However, note that the version in the
 *  paper omits the `Delta_t` denominator. Thus, it is inconsistent in terms of
 *  units, because the left-hand-side is a rate (having units of mm / day in the
 *  paper) and the right-hand-side is an amount of evaporation (having units of
 *  mm in the original paper).
 *
 *  The infiltration of water from precipitation or irrigation counteracts
 *  losses from evaporation. For example, infiltration can extend the time spend
 *  in Stage 1 evaporation. Or, with enough infiltration, all cumulative losses
 *  during Stage 1 evaporation can be compensated, returning the soil surface to
 *  a fully wet state. Likewise, sufficiently large infiltration can compensate
 *  for the cumulative losses in Stage 2, returning the soil to Stage 1. Each of
 *  these scenarios is handled separately in the flow chart provided in
 *  Ritchie (1972), where `P` is the rate of water infiltration into the soil
 *  surface.
 *
 *  #### Additional considerations for Stage 2 evaporation
 *
 *  From the paper:
 *
 *  > Twice during Stage 2 drying `ES` as calculated by (8) does not apply.
 *  >
 *  > The first time is when `P < sumes2` as is shown in the flow diagram at D.
 *  > The water content of the soil increases in proportion to `P`. The
 *  > evaporation rate in this special case `ESX` is first approximated as
 *  > `0.8 * infiltrated_water`. This relation was obtained locally from bare
 *  > soil lysimeter evaporation measurements after small rains (1-6 mm) that
 *  > came when the surface soil was relatively dry.
 *  >
 *  > There are two possibilities for modifying the first approximation of
 *  > `ESX`. If `ESX` as calculated is less than or equal to `ES` calculated
 *  > from (8), `ESX` is equated to the predicted `ES + P`, when `P` values are
 *  > unusually small. However, if `P` is rather large, so that `ESX > EOS`,
 *  > then `ESX` is limited to `EOS`.
 *  >
 *  > The second time in Stage 2 drying when (8) does not apply occurs when the
 *  > predicted `ES > EOS`. Then `ES` is limited to `EOS` as shown in the flow
 *  > diagram.
 *  >
 *  > A final step in calculating Stage 2 evaporation is shown at G in the flow
 *  > diagram. An updated value for `sumes2` is calculated, and the time is
 *  > adjusted if `ES` was determined where (8) did not apply.
 *
 *  In other words, when there is a small amount of infiltration during Stage 2
 *  drying, it is assumed to completely evaporate, and hence, the total
 *  evaporation rate becomes `ES + P`, where `ES` was the rate predicted using
 *  Equation 8. When there is a larger amount of infiltration (but not enough to
 *  end Stage 2 drying), the evaporation rate increases due to the infiltration,
 *  but is limited to the potential rate `EOS`.
 *
 *  Note that the comparison `P < sumes2` does not make sense because `P` is a
 *  rate (mm / day in the original model) and `sumes2` is a cumulative amount of
 *  evaporated water (mm). Instead, it is likely that this comparison should  be
 *  `P * Delta_t < sumes2` instead. This would describe a scenario where the
 *  total water infiltrated over a single time step is unable to compensate for
 *  the total amount lost during Stage 2 evaporation, which seems to agree with
 *  the concepts explained in the quote above.
 *
 *  #### Additional adjustments to prevent conceptual errors
 *
 *  The DSSAT code includes additional calculations after the ones described in
 *  Ritchie (1972). There are described in a comment:
 *
 *  > Soil evaporation can not be larger than the current extractable soil water
 *  > in the top layer. If available soil water is less than soil evaporation,
 *  > adjust first and second Stage evaporation and soil evaporation accordingly
 *
 *  In other words, if the evaporation rate is too high when using an Euler
 *  solver, it can potentially "overdraw" on the available soil water reserves,
 *  which could produce a negative soil water content (something that should not
 *  be possible). To fix this, limits are placed on the evaporation rate besides
 *  the ones discussed in the Ritchie (1972) paper.
 *
 *  These additional calculations do not seem to be fully described in any
 *  publications. When possible, we have added our own comments in the code to
 *  explain them, although some parts remain opaque.
 *
 *  ### Model implementation
 *
 *  Ritchie (1972) provides a flow chart and equations that can be used to
 *  implement the model. Rather than basing our code directly on this resource,
 *  we have also looked to DSSAT, and our code is largely based on its SOILEV
 *  subroutine.
 *
 *  The DSSAT module cites Ritchie (1972) as its main reference, but there are
 *  a few differences between the DSSAT code and the equations presented in
 *  Ritchie (1972). For example, Ritchie (1972) represents the "rainfall or
 *  irrigation rate" using the symbol `P`, with units of `mm / day`. Comparing
 *  the equations in Ritchie (1972) against those in SOILEV, it is clear that
 *  the variable `WINF`, defined as the "potential precipitation for
 *  infiltration," is intended to be identical to `P`. Yet, the units of `WINF`
 *  are given as `mm` rather than `mm / day`. In cases such as this, we try to
 *  follow Ritchie (1972) when possible, since some of the units given in SOILEV
 *  are not consistent with each other (see below for more info). Even so, some
 *  of the units in Ritchie (1972) are also inconsistent; for an example, see
 *  the discussion of Equation (8) above.
 *
 *  The original model and its DSSAT implementation assume a daily time step,
 *  and rates are expressed as mm / day. In our implementation, we have
 *  assumed an hourly timestep and expressed rates in units of mm / hr for
 *  consistency with other BioCro modules.
 *
 *  ### Notes about solver
 *
 *  The original model (and its DSSAT implementation) is formulated using
 *  difference equations rather than differential equations. Effectively, this
 *  can be viewed as assuming an Euler solver with a fixed step size of 1 is
 *  being used to solve a set of corresponding differential equations. Here are
 *  several examples of where this occurs:
 *
 *  1. For several quantities (`days_stage2`, `soil_evaporation_rate`, `sumes1`,
 *     and `sumes2`), this module returns the change relative to the value at
 *     the current time step. In other words, it calculates Delta_V =
 *     V_next_step - V_current_step with the expectation that the value at the
 *     next step will be equal to V_current_step + Delta_V. This is only
 *     guaranteed when using an Euler solver with a time step of 1 hour.
 *
 *  2. During Stage 2 evaporation, the next value of `days_stage2` is found by
 *     adding 1 hour to its current value, which only makes sense when using an
 *     Euler solver with a time step of 1 hour.
 *
 *  3. In some of the original calculations, no distinction was made between
 *     soil water content and its rate of change. For example, one line reads
 *     `ES = 3.5 * T**0.5 - SUMES2`. Here, `ES` is the soil water evaporation
 *     rate (in mm / day) and `SUMES2` is the cumulative soil water evaporated
 *     during Stage 2 drying (in mm). This seems to imply an "invisible"
 *     division of the change in soil water content (in mm) by the time step
 *     size (one day) to produce an output rate with the correct units
 *     (mm / day).
 *
 *  To clarify these points, we have defined a `timestep` variable in the code
 *  below.
 *
 *  Note that although some of the equations were developed by assuming an Euler
 *  solver with a step size of 1, this does not prevent other solvers from being
 *  used to solve the equations.
 *
 *  ### Definitions for some parameters
 *
 *  - `bare_soil_albedo_max`: Maximum bare soil albedo
 *
 *  - `days_stage2`: Time elapsed in Stage 2 evaporation

 *  - `infiltrated_water`: Rate of water infiltration, equal to rainfall
 *     minus runoff plus net irrigation
 *
 *  - `sumes1`: Cumulative soil evaporation in Stage 1
 *
 *  - `sumes2`: Cumulative soil evaporation in Stage 2
 *
 *  ### Source
 *
 *  - [Ritchie, J. T. "Model for predicting evaporation from a row crop with incomplete cover."
 *    Water Resources Research 8, 1204–1213 (1972)]
 *    (https://doi.org/10.1029/WR008i005p01204)
 *
 *  - DSSAT Fortran source code:
 *    github.com/DSSAT/dssat-csm-os/blob/develop/SPAM/SOILEV.for
 */
class soil_evaporation_ritchie : public differential_module
{
   public:
    soil_evaporation_ritchie(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module(),

          // Get references to input quantities
          atmospheric_pressure{get_input(input_quantities, "atmospheric_pressure")},
          bare_soil_albedo_max{get_input(input_quantities, "bare_soil_albedo_max")},
          cosine_zenith_angle{get_input(input_quantities, "cosine_zenith_angle")},
          days_stage2{get_input(input_quantities, "days_stage2")},
          deltaS_1{get_input(input_quantities, "deltaS_1")},
          deltaU_1{get_input(input_quantities, "deltaU_1")},
          fractional_doy{get_input(input_quantities, "fractional_doy")},
          infiltrated_water{get_input(input_quantities, "infiltrated_water")},
          irradiance_diffuse_transmittance{get_input(input_quantities, "irradiance_diffuse_transmittance")},
          irradiance_direct_transmittance{get_input(input_quantities, "irradiance_direct_transmittance")},
          kcbmax{get_input(input_quantities, "kcbmax")},
          kcbmin{get_input(input_quantities, "kcbmin")},
          lai{get_input(input_quantities, "lai")},
          par_energy_content{get_input(input_quantities, "par_energy_content")},
          par_energy_fraction{get_input(input_quantities, "par_energy_fraction")},
          rh{get_input(input_quantities, "rh")},
          skc{get_input(input_quantities, "skc")},
          soil_depth_1{get_input(input_quantities, "soil_depth_1")},
          soil_evaporation_rate{get_input(input_quantities, "soil_evaporation_rate")},
          soil_water_content_1{get_input(input_quantities, "soil_water_content_1")},
          soil_wilting_point_1{get_input(input_quantities, "soil_wilting_point_1")},
          solar{get_input(input_quantities, "solar")},
          sumes1{get_input(input_quantities, "sumes1")},
          sumes2{get_input(input_quantities, "sumes2")},
          temp{get_input(input_quantities, "temp")},
          windspeed{get_input(input_quantities, "windspeed")},
          windspeed_height{get_input(input_quantities, "windspeed_height")},

          // Get pointers to output quantities
          days_stage2_op{get_op(output_quantities, "days_stage2")},
          soil_evaporation_rate_op{get_op(output_quantities, "soil_evaporation_rate")},
          sumes1_op{get_op(output_quantities, "sumes1")},
          sumes2_op{get_op(output_quantities, "sumes2")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_evaporation_ritchie"; }

   private:
    // References to input quantities
    double const& atmospheric_pressure;
    double const& bare_soil_albedo_max;
    double const& cosine_zenith_angle;
    double const& days_stage2;
    double const& deltaS_1;
    double const& deltaU_1;
    double const& fractional_doy;
    double const& infiltrated_water;
    double const& irradiance_diffuse_transmittance;
    double const& irradiance_direct_transmittance;
    double const& kcbmax;
    double const& kcbmin;
    double const& lai;
    double const& par_energy_content;
    double const& par_energy_fraction;
    double const& rh;
    double const& skc;
    double const& soil_depth_1;
    double const& soil_evaporation_rate;
    double const& soil_water_content_1;
    double const& soil_wilting_point_1;
    double const& solar;
    double const& sumes1;
    double const& sumes2;
    double const& temp;
    double const& windspeed;
    double const& windspeed_height;

    // Pointers to output quantities
    double* days_stage2_op;
    double* soil_evaporation_rate_op;
    double* sumes1_op;
    double* sumes2_op;

    // Main operation
    void do_operation() const;
};

string_vector soil_evaporation_ritchie::get_inputs()
{
    return {
        "atmospheric_pressure",              // Pa
        "bare_soil_albedo_max",              // dimensionless
        "cosine_zenith_angle",               // dimensionless
        "days_stage2",                       // day
        "deltaS_1",                          // m^3 / m^3
        "deltaU_1",                          // m^3 / m^3
        "fractional_doy",                    // day
        "infiltrated_water",                 // mm / hr
        "irradiance_diffuse_transmittance",  // dimensionless
        "irradiance_direct_transmittance",   // dimensionless
        "kcbmax",                            // dimensionless
        "kcbmin",                            // dimensionless
        "lai",                               // dimensionless from (m^2 leaf) / (m^2 ground)
        "par_energy_content",                // J / micromol
        "par_energy_fraction",               // dimensionless
        "rh",                                // dimensionless
        "skc",                               // dimensionless
        "soil_depth_1",                      // cm
        "soil_evaporation_rate",             // Mg / ha / hr
        "soil_water_content_1",              // m^3 / m^3
        "soil_wilting_point_1",              // m^3 / m^3
        "solar",                             // micromol / m^2 / s
        "sumes1",                            // mm
        "sumes2",                            // mm
        "temp",                              // degrees C
        "windspeed",                         // m / s
        "windspeed_height"                   // m
    };
}

string_vector soil_evaporation_ritchie::get_outputs()
{
    return {
        "days_stage2",            // day / hr
        "soil_evaporation_rate",  // Mg / ha / hr^2
        "sumes1",                 // mm / hr
        "sumes2"                  // mm / hr
    };
}

void soil_evaporation_ritchie::do_operation() const
{
    using calculation_constants::eps_zero;
    using std::max;
    using std::min;

    // Define conversion constants to avoid magic numbers
    double constexpr cm_to_mm = 10.0;       // mm / cm
    double constexpr hours_per_day = 24.0;  // hr / day
    double constexpr mm_to_Mg_per_ha = 10;  // (Mg / ha) / mm

    // Hard-coded model parameter values
    double constexpr canopyHeight = 1.0;  // m
    double constexpr SWEF_depth = 30;     // cm
    double constexpr timestep = 1;        // hr

    // Soil hydraulic properties. Yolo Loam
    // See Table 1 in Ritchie (1972), https://doi.org/10.1029/WR008i005p01204
    double constexpr soil_evaporation_alpha = 4.0;  // mm / day^(0.5)

    // Upper Limit of Stage 1 Cumulative Evaporation. Yolo Loam
    // Table 1. Ritchie (1972)
    double constexpr evap_limit = 9.0;  // mm

    double const surface_soil_depth_in_mm = soil_depth_1 * cm_to_mm;  // mm

    double reference_et = reference_evapotranspiration(
        atmospheric_pressure,
        cosine_zenith_angle,
        fractional_doy,
        irradiance_diffuse_transmittance,
        irradiance_direct_transmittance,
        par_energy_content,
        par_energy_fraction,
        rh,
        solar,
        temp,
        windspeed,
        windspeed_height);  // mm / hr

    double EOS = potential_soil_evaporation(
        skc,
        kcbmax,
        kcbmin,
        lai,
        canopyHeight,
        reference_et);  // mm / hr

    // Initialize temporary variables used to determine the values of key
    // quantities at the next time step.
    double ES = soil_evaporation_rate / mm_to_Mg_per_ha;  // mm / hr
    double sumes1_next = sumes1;                          // mm
    double sumes2_next = sumes2;                          // mm
    double days_stage2_next = days_stage2;                // day
    double old_ES = ES;                                   // mm / hr

    // If the potential evaporation rate is nonzero, calculate new values of key
    // quantities using the Ritchie soil evaporation routine
    if (EOS > 1e-6) {
        // Intermediate calculations to help identify which scenario applies.
        //
        // Here it is assumed that the total water infiltrated during the next
        // time step is `infiltrated_water * timestep`, so these calculations
        // require an Euler solver.
        double const new_infil = infiltrated_water * timestep;  // mm
        bool const S1_over_thresh = sumes1 >= evap_limit;       // Cumulative Stage 1 evaporation has exceeded its threshold
        bool const S2_undone = new_infil >= sumes2;             // New infiltration exceeds total evaporation in Stage 2
        bool const S1_undone = new_infil >= sumes1;             // New infiltration exceeds total evaporation in Stage 1

        if (S1_over_thresh && S2_undone) {
            // Scenario A (Stage 1): Here the cumulative evaporation in Stage 1
            // is above the threshold, which would normally indicate Stage 2
            // evaporation. However, the amount of infiltrated water is enough
            // to trigger the end of Stage 2 evaporation, so we remain in
            // Stage 1.

            // Calculate the infiltrated water that remains after compensating
            // for the cumulative losses during Stage 2 evaporation
            double const excess_infil = new_infil - sumes2;  // mm

            // The excess infiltration begins to compensate for cumulative
            // losses during Stage 1 evaporation
            sumes1_next = max(0.0, evap_limit - excess_infil);  // mm

            // Stage 2 has ended, so reset the Stage 2 counters
            sumes2_next = 0.0;       // mm
            days_stage2_next = 0.0;  // day

            // Handle a (possible) transition from Stage 1 to Stage 2
            evap_str const evap_comp = ritchie_s1_to_s2(
                days_stage2_next,
                EOS,
                evap_limit,
                soil_evaporation_alpha,
                sumes1_next,
                sumes2_next,
                timestep);

            sumes1_next = evap_comp.sumes1_next;            // mm
            sumes2_next = evap_comp.sumes2_next;            // mm
            days_stage2_next = evap_comp.days_stage2_next;  // day
            ES = evap_comp.ES;                              // mm / hr
        } else if (S1_over_thresh && !S2_undone) {
            // Scenario B (Stage 2): Here the cumulative evaporation in Stage 1
            // is above the threshold, indicating Stage 2 evaporation. There is
            // not enough water infiltration to end Stage 2 evaporation, so we
            // remain in Stage 2.

            // Increment the amount of time spent in Stage 2 evaporation
            days_stage2_next = days_stage2 + timestep / hours_per_day;  // day

            // Check for error conditions
            if (days_stage2_next < -eps_zero) {
                throw std::range_error("Thrown in soil_evaporation_ritchie: days_stage2_next is negative.");
            }

            // Use Equation (8) from Ritchie (1972) to calculate the soil
            // evaporation rate
            ES = (soil_evaporation_alpha * sqrt(days_stage2_next) - sumes2) / timestep;  // mm / hr

            // Handle the special sub-cases of Stage 2 evaporation
            if (infiltrated_water > 0.0) {
                // This is the first scenario where Equation (8) does not apply
                double ESX = 0.8 * infiltrated_water;  // mm / hr

                if (ESX <= ES) {
                    ESX = ES + infiltrated_water;  // mm / hr
                }

                if (ESX > EOS) {
                    ESX = EOS;  // mm / hr
                }

                ES = ESX;  // mm / hr
            } else if (ES > EOS) {
                // The is the second scenario where Equation (8) does not apply
                ES = EOS;  // mm / hr
            }

            // Calculate the new value of sumes2, and find the value of
            // days_stage2 that would produce this value of sumes2 when using
            // Equation 6
            sumes2_next = sumes2 + ES * timestep - new_infil;                   // mm
            days_stage2_next = pow((sumes2_next / soil_evaporation_alpha), 2);  // day
        } else if (!S1_over_thresh && S1_undone) {
            // Scenario 3 (Stage 1): Here the cumulative evaporation in Stage 1
            // is below the threshold, so we remain in Stage 1. There is also
            // enough water infitration to return the soil surface to a fully
            // wet state.

            // Reset Stage 1 evaporation
            sumes1_next = 0.0;  // mm

            // Handle a (possible) transition from Stage 1 to Stage 2
            evap_str const evap_comp = ritchie_s1_to_s2(
                days_stage2_next,
                EOS,
                evap_limit,
                soil_evaporation_alpha,
                sumes1_next,
                sumes2_next,
                timestep);

            sumes1_next = evap_comp.sumes1_next;            // mm
            sumes2_next = evap_comp.sumes2_next;            // mm
            days_stage2_next = evap_comp.days_stage2_next;  // day
            ES = evap_comp.ES;                              // mm / hr
        } else if (!S1_over_thresh && !S1_undone) {
            // Scenario 4 (Stage 1): Here the cumulative evaporation in Stage 1
            // is below the threshold, and there is not enough water
            // infiltration to return the soil to its saturated state, so we
            // remain in Stage 1.

            // The new water infiltration compensates Stage 1 evaporative losses
            sumes1_next = sumes1 - new_infil;  // mm

            // Handle a (possible) transition from Stage 1 to Stage 2
            evap_str const evap_comp = ritchie_s1_to_s2(
                days_stage2_next,
                EOS,
                evap_limit,
                soil_evaporation_alpha,
                sumes1_next,
                sumes2_next,
                timestep);

            sumes1_next = evap_comp.sumes1_next;            // mm
            sumes2_next = evap_comp.sumes2_next;            // mm
            days_stage2_next = evap_comp.days_stage2_next;  // day
            ES = evap_comp.ES;                              // mm / hr
        } else {
            throw std::logic_error("Thrown in soil_evaporation_ritchie: unusual conditions detected in main calculations.");
        }

        // The remaining calculations are not described in the original
        // Ritchie (1972) paper

        // Transpiration by a crop can reduce the soil water content to the
        // wilting point, but evaporation can reduce it further in the top soil
        // layer. The lower limit for evaporation is related to the wilting
        // point via a multiplicative factor, which is called the "soil water
        // evaporation fraction" or `SWEF` in DSSAT.
        double const SWEF = 0.9 - 0.00038 * pow((soil_depth_1 - SWEF_depth), 2);  // dimensionless

        // Find the water available for evaporation
        double const sw_avail_evap =
            max(0.0, surface_soil_depth_in_mm *
                         (soil_water_content_1 - soil_wilting_point_1 * SWEF));  // mm

        // Find the total evaporative losses over the next time step
        double const es_total = ES * timestep;  // mm

        // Find the excess evaporative loss
        double const excess_evap = es_total - sw_avail_evap;  // mm

        // If the available soil water is less than the evaporative losses over
        // the next time step, there would be an "excess loss." Two steps must
        // be taken to prevent that from occurring:
        //  1. The cumulative evaporation in Stage 1, Stage 2, or both must be
        //     reduced.
        //  2. The evaporation rate must be adjusted to a lower value.
        if (excess_evap > 0.0) {
            // Intermediate calculations to help identify which scenario applies
            //
            // Note from EL on 2026-04-23: I do not understand why we would
            // check for `sumes2_next > es_total`. I think it would make more
            // sense to check for `sumes2_next > excess_evap`, since this would
            // be the appropriate check to ensure `sumes2_next` does not become
            // negative in Scenario A below.
            bool const S1_over_thresh = sumes1_next >= evap_limit;  // Cumulative Stage 1 evaporation at the next step will exceed its threshold
            bool const large_S2 = sumes2_next > es_total;           // Cumulative evaporation in Stage 2 at the next step is large
            bool const nonzero_S2 = sumes1_next > 0.0;              // Cumulative evaporation in Stage 2 at the next step is nonzero

            if (S1_over_thresh && large_S2) {
                // Scenario A: Here we are in Stage 2 and the cumulative
                // evaporation in Stage 2 is large, so we can just reduce the
                // Stage 2 losses without considering a potential transition
                // back to Stage 1
                sumes2_next = sumes2_next - excess_evap;                            // mm
                days_stage2_next = pow((sumes2_next / soil_evaporation_alpha), 2);  // day
            } else if (S1_over_thresh && !large_S2 && nonzero_S2) {
                // Scenario B: Here we are in Stage 2 and the cumulative
                // evaporation in Stage 2 is not large, so we reduce both
                // Stage 2 and Stage 1 losses.
                //
                // Note from EL on 2026-04-23: I do not understand what's going
                // on here. I would have expected to reduce `sumes1_next` by
                // `excess_evap - sumes2_next`, and then to reduce `sumes2_next`
                // to 0. In other words, to reduce Stage 2 evaporation as much
                // as possible (until it resets to 0), and then accomplish the
                // remaining reduction by reducing Stage 1 evaporation. But the
                // code here seems to do something else.
                sumes1_next = sumes1_next - (es_total - sumes2_next);               // mm
                sumes2_next = max(sumes1_next + sw_avail_evap - evap_limit, 0.0);   // mm
                sumes1_next = min(sumes1_next + sw_avail_evap, evap_limit);         // mm
                days_stage2_next = pow((sumes2_next / soil_evaporation_alpha), 2);  // day
            } else if (!S1_over_thresh || (S1_over_thresh && !large_S2 && !nonzero_S2)) {
                // Scenario C: Here we are in Stage 1, or possibly right at the
                // start of Stage 2 (where sumes2 is still 0). In either case,
                // we just reduce the Stage 1 losses
                sumes1_next = sumes1_next - excess_evap;  // mm
            } else {
                throw std::logic_error("Thrown in soil_evaporation_ritchie: unusual conditions detected in adjustment for limited water availability.");
            }

            // Limit the evaporation rate
            ES = sw_avail_evap / timestep;  // mm / hr
        }

        // Find the water available for evaporation
        //
        // Note from EL on 2026-04-23: This is very similar to the calculation
        // of `sw_avail_evap` above. The difference is that here we use
        // `sw_avail_1 = soil_water_content_1 + deltaS_1 + deltaU_1`
        // rather than `soil_water_content_1`.  This was originally described
        // in DSSAT as:
        //
        // "Available water = SW - air dry limit + infil. or sat. flow"
        double const sw_avail_1 =
            max(0.0, soil_water_content_1 + deltaS_1 + deltaU_1);  // m^3 / m^3

        double const sw_min =
            max(0.0, surface_soil_depth_in_mm *
                         (sw_avail_1 - soil_wilting_point_1 * SWEF));  // mm

        // Limit ES to between zero and avail water in soil layer 1
        //
        // Note from EL on 2026-04-23: This check is very similar to the more
        // elaborate one applied above using `sw_avail_evap` as the limit. I
        // do not understand why this check only reduces the soil evaporation
        // rate without also reducing the cumulative totals in Stages 1 and 2.
        // My guess is that this check is not implemented properly. It might
        // make more sense to use the smaller of `sw_avail_evap` and `sw_min`
        // as the limit, and then apply the full scenario-dependent adjustment
        // of `ES`, `sumes1_next`, and `sumes2_next`.
        if (ES * timestep > sw_min) {
            ES = sw_min / timestep;  // mm / hr
        }

        ES = max(ES, 0.0);  // mm / hr
    }

    // Determine the changes in key variables relative to the current time step
    double const delta_sumes1 = sumes1_next - sumes1;                 // mm
    double const delta_sumes2 = sumes2_next - sumes2;                 // mm
    double const delta_days_stage2 = days_stage2_next - days_stage2;  // day
    double const delta_ES = ES - old_ES;                              // mm / hr

    // Update the output quantity list
    update(sumes1_op, delta_sumes1 / timestep);                               // mm / hr
    update(sumes2_op, delta_sumes2 / timestep);                               // mm / hr
    update(days_stage2_op, delta_days_stage2 / timestep);                     // day / hr
    update(soil_evaporation_rate_op, delta_ES * mm_to_Mg_per_ha / timestep);  // Mg / ha / hr^2
}

}  // namespace standardBML
#endif
