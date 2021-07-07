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
class c4_leaf_photosynthesis : public direct_module
{
   public:
    c4_leaf_photosynthesis(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          direct_module("c4_leaf_photosynthesis"),

          // Get references to input quantities
          incident_ppfd(get_input(input_quantities, "incident_ppfd")),
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
          average_absorbed_shortwave(get_input(input_quantities, "average_absorbed_shortwave")),
          absorbed_shortwave(get_input(input_quantities, "absorbed_shortwave")),
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
    double const& incident_ppfd;
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
    double const& average_absorbed_shortwave;
    double const& absorbed_shortwave;
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

#endif
