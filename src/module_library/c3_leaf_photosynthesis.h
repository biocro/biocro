#ifndef C3_LEAF_PHOTOSYNTHESIS_H
#define C3_LEAF_PHOTOSYNTHESIS_H

#include "../state_map.h"
#include "../modules.h"

/**
 * @class c3_leaf_photosynthesis
 *
 * @brief Uses the method from `c3CanAC()` to calculate leaf photosynthesis
 * parameters for C3 plants
 */
class c3_leaf_photosynthesis : public SteadyModule
{
   public:
    c3_leaf_photosynthesis(
        state_map const* input_parameters,
        state_map* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("c3_leaf_photosynthesis"),

          // Get references to input parameters
          par_energy_content(get_input(input_parameters, "par_energy_content")),
          incident_par(get_input(input_parameters, "incident_par")),
          temp(get_input(input_parameters, "temp")),
          rh(get_input(input_parameters, "rh")),
          vmax1(get_input(input_parameters, "vmax1")),
          jmax(get_input(input_parameters, "jmax")),
          tpu_rate_max(get_input(input_parameters, "tpu_rate_max")),
          Rd(get_input(input_parameters, "Rd")),
          b0(get_input(input_parameters, "b0")),
          b1(get_input(input_parameters, "b1")),
          Gs_min(get_input(input_parameters, "Gs_min")),
          Catm(get_input(input_parameters, "Catm")),
          atmospheric_pressure(get_input(input_parameters, "atmospheric_pressure")),
          O2(get_input(input_parameters, "O2")),
          theta(get_input(input_parameters, "theta")),
          StomataWS(get_input(input_parameters, "StomataWS")),
          water_stress_approach(get_input(input_parameters, "water_stress_approach")),
          electrons_per_carboxylation(get_input(input_parameters, "electrons_per_carboxylation")),
          electrons_per_oxygenation(get_input(input_parameters, "electrons_per_oxygenation")),
          incident_average_par(get_input(input_parameters, "incident_average_par")),
          windspeed(get_input(input_parameters, "windspeed")),
          height(get_input(input_parameters, "height")),
          specific_heat_of_air(get_input(input_parameters, "specific_heat_of_air")),

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
