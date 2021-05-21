#ifndef C3_LEAF_PHOTOSYNTHESIS_H
#define C3_LEAF_PHOTOSYNTHESIS_H

#include "../modules.h"
#include "../state_map.h"
#include "c3photo.hpp"  // for c3photoC
#include "BioCro.h"     // for c3EvapoTrans

/**
 * @class c3_leaf_photosynthesis
 *
 * @brief Uses the method from `c3CanAC()` to calculate leaf photosynthesis parameters for C3 plants
 */
class c3_leaf_photosynthesis : public SteadyModule
{
   public:
    c3_leaf_photosynthesis(
        state_map const& input_quantities,
        state_map& output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("c3_leaf_photosynthesis"),

          // Get references to input quantities
          par_energy_content(get_input(input_quantities, "par_energy_content")),
          incident_par(get_input(input_quantities, "incident_par")),
          temp(get_input(input_quantities, "temp")),
          rh(get_input(input_quantities, "rh")),
          vmax1(get_input(input_quantities, "vmax1")),
          jmax(get_input(input_quantities, "jmax")),
          tpu_rate_max(get_input(input_quantities, "tpu_rate_max")),
          Rd(get_input(input_quantities, "Rd")),
          b0(get_input(input_quantities, "b0")),
          b1(get_input(input_quantities, "b1")),
          Gs_min(get_input(input_quantities, "Gs_min")),
          Catm(get_input(input_quantities, "Catm")),
          atmospheric_pressure(get_input(input_quantities, "atmospheric_pressure")),
          O2(get_input(input_quantities, "O2")),
          theta(get_input(input_quantities, "theta")),
          StomataWS(get_input(input_quantities, "StomataWS")),
          water_stress_approach(get_input(input_quantities, "water_stress_approach")),
          electrons_per_carboxylation(get_input(input_quantities, "electrons_per_carboxylation")),
          electrons_per_oxygenation(get_input(input_quantities, "electrons_per_oxygenation")),
          incident_average_par(get_input(input_quantities, "incident_average_par")),
          windspeed(get_input(input_quantities, "windspeed")),
          height(get_input(input_quantities, "height")),
          specific_heat_of_air(get_input(input_quantities, "specific_heat_of_air")),

          // Get pointers to output quantities
          Assim_op(get_op(output_quantities, "Assim")),
          GrossAssim_op(get_op(output_quantities, "GrossAssim")),
          Ci_op(get_op(output_quantities, "Ci")),
          Gs_op(get_op(output_quantities, "Gs")),
          TransR_op(get_op(output_quantities, "TransR")),
          EPenman_op(get_op(output_quantities, "EPenman")),
          EPriestly_op(get_op(output_quantities, "EPriestly")),
          leaf_temperature_op(get_op(output_quantities, "leaf_temperature"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // References to input quantities
    double const& par_energy_content;
    double const& incident_par;
    double const& temp;
    double const& rh;
    double const& vmax1;
    double const& jmax;
    double const& tpu_rate_max;
    double const& Rd;
    double const& b0;
    double const& b1;
    double const& Gs_min;
    double const& Catm;
    double const& atmospheric_pressure;
    double const& O2;
    double const& theta;
    double const& StomataWS;
    double const& water_stress_approach;
    double const& electrons_per_carboxylation;
    double const& electrons_per_oxygenation;
    double const& incident_average_par;
    double const& windspeed;
    double const& height;
    double const& specific_heat_of_air;

    // Pointers to output quantities
    double* Assim_op;
    double* GrossAssim_op;
    double* Ci_op;
    double* Gs_op;
    double* TransR_op;
    double* EPenman_op;
    double* EPriestly_op;
    double* leaf_temperature_op;

    // Main operation
    void do_operation() const;
};

string_vector c3_leaf_photosynthesis::get_inputs()
{
    return {
        "par_energy_content",           // J / micromol
        "incident_par",                 // J / (m^2 leaf) / s
        "temp",                         // deg. C
        "rh",                           // dimensionless
        "vmax1",                        // micromole / m^2 / s
        "jmax",                         // micromole / m^2 / s
        "tpu_rate_max",                 // micromole / m^2 / s
        "Rd",                           // micromole / m^2 / s
        "b0",                           // mol / m^2 / s
        "b1",                           // dimensionless
        "Gs_min",                       // mol / m^2 / s
        "Catm",                         // micromole / mol
        "atmospheric_pressure",         // Pa
        "O2",                           // mmol / mol
        "theta",                        // dimensionless
        "StomataWS",                    // dimensionless
        "water_stress_approach",        // a dimensionless switch
        "electrons_per_carboxylation",  // electron / carboxylation
        "electrons_per_oxygenation",    // electron / oxygenation
        "incident_average_par",         // J / (m^2 leaf) / s
        "windspeed",                    // m / s
        "height",                       // m
        "specific_heat_of_air"          // J / kg / K
    };
}

string_vector c3_leaf_photosynthesis::get_outputs()
{
    return {
        "Assim",            // micromole / m^2 /s
        "GrossAssim",       // micromole / m^2 /s
        "Ci",               // micromole / mol
        "Gs",               // mmol / m^2 / s
        "TransR",           // mmol / m^2 / s
        "EPenman",          // mmol / m^2 / s
        "EPriestly",        // mmol / m^2 / s
        "leaf_temperature"  // deg. C
    };
}

void c3_leaf_photosynthesis::do_operation() const
{
    // Convert light inputs from energy to molecular flux densities
    const double incident_par_micromol = incident_par / par_energy_content;                  // micromol / m^2 / s
    const double incident_average_par_micromol = incident_average_par / par_energy_content;  // micromol / m^2 / s

    // Get an initial estimate of stomatal conductance, assuming the leaf is at air temperature
    const double initial_stomatal_conductance = c3photoC(
                                                    incident_par_micromol, temp, rh, vmax1, jmax, tpu_rate_max,
                                                    Rd, b0, b1, Gs_min, Catm, atmospheric_pressure, O2, theta,
                                                    StomataWS, water_stress_approach,
                                                    electrons_per_carboxylation, electrons_per_oxygenation)
                                                    .Gs;  // mmol / m^2 / s

    // Calculate a new value for leaf temperature
    //
    const struct ET_Str et = c3EvapoTrans(incident_average_par_micromol, temp, rh, windspeed, height,
                                          specific_heat_of_air, initial_stomatal_conductance);

    const double leaf_temperature = temp + et.Deltat;  // deg. C

    // Calculate final values for assimilation, stomatal conductance, and Ci using the new leaf temperature
    const struct c3_str photo = c3photoC(
        incident_par_micromol, leaf_temperature, rh, vmax1, jmax, tpu_rate_max,
        Rd, b0, b1, Gs_min, Catm, atmospheric_pressure, O2, theta, StomataWS,
        water_stress_approach, electrons_per_carboxylation, electrons_per_oxygenation);

    // Update the outputs
    update(Assim_op, photo.Assim);
    update(GrossAssim_op, photo.GrossAssim);
    update(Ci_op, photo.Ci);
    update(Gs_op, photo.Gs);
    update(TransR_op, et.TransR);
    update(EPenman_op, et.EPenman);
    update(EPriestly_op, et.EPriestly);
    update(leaf_temperature_op, leaf_temperature);
}

#endif
