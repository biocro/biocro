#ifndef C4_LEAF_PHOTOSYNTHESIS_H
#define C4_LEAF_PHOTOSYNTHESIS_H

#include "../modules.h"
#include "../state_map.h"
#include "c4photo.h"    // for c4photoC
#include "AuxBioCro.h"  // for EvapoTrans2

/**
 * @class c4_leaf_photosynthesis
 *
 * @brief Uses the method from `CanAC()` to calculate leaf photosynthesis parameters for C4 plants
 */
class c4_leaf_photosynthesis : public SteadyModule
{
   public:
    c4_leaf_photosynthesis(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("c4_leaf_photosynthesis"),

          // Get references to input quantities
          par_energy_content(get_input(input_quantities, "par_energy_content")),
          incident_par(get_input(input_quantities, "incident_par")),
          temp(get_input(input_quantities, "temp")),
          rh(get_input(input_quantities, "rh")),
          vmax1(get_input(input_quantities, "vmax1")),
          alpha1(get_input(input_quantities, "alpha1")),
          kparm(get_input(input_quantities, "kparm")),
          theta(get_input(input_quantities, "theta")),
          beta(get_input(input_quantities, "beta")),
          Rd(get_input(input_quantities, "Rd")),
          b0(get_input(input_quantities, "b0")),
          b1(get_input(input_quantities, "b1")),
          Gs_min(get_input(input_quantities, "Gs_min")),
          StomataWS(get_input(input_quantities, "StomataWS")),
          Catm(get_input(input_quantities, "Catm")),
          atmospheric_pressure(get_input(input_quantities, "atmospheric_pressure")),
          water_stress_approach(get_input(input_quantities, "water_stress_approach")),
          upperT(get_input(input_quantities, "upperT")),
          lowerT(get_input(input_quantities, "lowerT")),
          incident_average_par(get_input(input_quantities, "incident_average_par")),
          windspeed(get_input(input_quantities, "windspeed")),
          height(get_input(input_quantities, "height")),
          leafwidth(get_input(input_quantities, "leafwidth")),
          specific_heat_of_air(get_input(input_quantities, "specific_heat_of_air")),
          et_equation(get_input(input_quantities, "et_equation")),

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
    double const& alpha1;
    double const& kparm;
    double const& theta;
    double const& beta;
    double const& Rd;
    double const& b0;
    double const& b1;
    double const& Gs_min;
    double const& StomataWS;
    double const& Catm;
    double const& atmospheric_pressure;
    double const& water_stress_approach;
    double const& upperT;
    double const& lowerT;
    double const& incident_average_par;
    double const& windspeed;
    double const& height;
    double const& leafwidth;
    double const& specific_heat_of_air;
    double const& et_equation;

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

string_vector c4_leaf_photosynthesis::get_inputs()
{
    return {
        "par_energy_content",     // J / micromol
        "incident_par",           // J / (m^2 leaf) / s
        "temp",                   // deg. C
        "rh",                     // dimensionless
        "vmax1",                  // micromole / m^2 / s
        "alpha1",                 // mol / mol
        "kparm",                  // mol / m^2 / s
        "theta",                  // dimensionless
        "beta",                   // dimensionless
        "Rd",                     // micromole / m^2 / s
        "b0",                     // mol / m^2 / s
        "b1",                     // dimensionless
        "Gs_min",                 // mol / m^2 / s
        "StomataWS",              // dimensionless
        "Catm",                   // micromole / mol
        "atmospheric_pressure",   // Pa
        "water_stress_approach",  // a dimensionless switch
        "upperT",                 // deg. C
        "lowerT",                 // deg. C
        "incident_average_par",   // J / (m^2 leaf) / s
        "windspeed",              // m / s
        "height",                 // m
        "leafwidth",              // m
        "specific_heat_of_air",   // J / kg / K
        "et_equation"             // a dimensionless switch
    };
}

string_vector c4_leaf_photosynthesis::get_outputs()
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

void c4_leaf_photosynthesis::do_operation() const
{
    // Convert light inputs from energy to molecular flux densities
    const double incident_par_micromol = incident_par / par_energy_content;                  // micromol / m^2 / s
    const double incident_average_par_micromol = incident_average_par / par_energy_content;  // micromol / m^2 / s

    // Get an initial estimate of stomatal conductance, assuming the leaf is at air temperature
    const double initial_stomatal_conductance = c4photoC(
                                                    incident_par_micromol, temp, rh, vmax1, alpha1,
                                                    kparm, theta, beta, Rd, b0, b1, Gs_min * 1e3, StomataWS, Catm,
                                                    atmospheric_pressure, water_stress_approach, upperT, lowerT)
                                                    .Gs;  // mmol / m^2 / s

    // Calculate a new value for leaf temperature
    double constexpr LAI = 0.0;  // EvapoTrans2 does not actually use LAI for anything
    const struct ET_Str et = EvapoTrans2(incident_par_micromol, incident_average_par_micromol, temp, rh, windspeed,
                                         LAI, height, initial_stomatal_conductance, leafwidth, specific_heat_of_air, et_equation);

    const double leaf_temperature = temp + et.Deltat;  // deg. C

    // Calculate final values for assimilation, stomatal conductance, and Ci using the new leaf temperature
    const struct c4_str photo = c4photoC(
        incident_par_micromol, leaf_temperature, rh, vmax1, alpha1,
        kparm, theta, beta, Rd, b0, b1, Gs_min * 1e3, StomataWS, Catm,
        atmospheric_pressure, water_stress_approach, upperT, lowerT);

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
