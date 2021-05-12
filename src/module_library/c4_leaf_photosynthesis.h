#ifndef C4_LEAF_PHOTOSYNTHESIS_H
#define C4_LEAF_PHOTOSYNTHESIS_H

#include "../state_map.h"
#include "../modules.h"

/**
 * @class c4_leaf_photosynthesis
 *
 * @brief Uses the method from `CanAC()` to calculate leaf photosynthesis
 * parameters for C4 plants
 */
class c4_leaf_photosynthesis : public SteadyModule
{
   public:
    c4_leaf_photosynthesis(
        state_map const* input_parameters,
        state_map* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("c4_leaf_photosynthesis"),

          // Get references to input parameters
          par_energy_content(get_input(input_parameters, "par_energy_content")),
          incident_par(get_input(input_parameters, "incident_par")),
          temp(get_input(input_parameters, "temp")),
          rh(get_input(input_parameters, "rh")),
          vmax1(get_input(input_parameters, "vmax1")),
          alpha1(get_input(input_parameters, "alpha1")),
          kparm(get_input(input_parameters, "kparm")),
          theta(get_input(input_parameters, "theta")),
          beta(get_input(input_parameters, "beta")),
          Rd(get_input(input_parameters, "Rd")),
          b0(get_input(input_parameters, "b0")),
          b1(get_input(input_parameters, "b1")),
          Gs_min(get_input(input_parameters, "Gs_min")),
          StomataWS(get_input(input_parameters, "StomataWS")),
          Catm(get_input(input_parameters, "Catm")),
          atmospheric_pressure(get_input(input_parameters, "atmospheric_pressure")),
          water_stress_approach(get_input(input_parameters, "water_stress_approach")),
          upperT(get_input(input_parameters, "upperT")),
          lowerT(get_input(input_parameters, "lowerT")),
          incident_average_par(get_input(input_parameters, "incident_average_par")),
          windspeed(get_input(input_parameters, "windspeed")),
          height(get_input(input_parameters, "height")),
          leafwidth(get_input(input_parameters, "leafwidth")),
          specific_heat_of_air(get_input(input_parameters, "specific_heat_of_air")),
          et_equation(get_input(input_parameters, "et_equation")),

          // Get pointers to output parameters
          Assim_op(get_op(output_parameters, "Assim")),
          GrossAssim_op(get_op(output_parameters, "GrossAssim")),
          Ci_op(get_op(output_parameters, "Ci")),
          Gs_op(get_op(output_parameters, "Gs")),
          TransR_op(get_op(output_parameters, "TransR")),
          EPenman_op(get_op(output_parameters, "EPenman")),
          EPriestly_op(get_op(output_parameters, "EPriestly")),
          leaf_temperature_op(get_op(output_parameters, "leaf_temperature"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // References to input parameters
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

    // Pointers to output parameters
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

#endif
