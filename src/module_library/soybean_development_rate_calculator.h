#ifndef SOYBEAN_DEVELOPMENT_RATE_CALCULATOR_H
#define SOYBEAN_DEVELOPMENT_RATE_CALCULATOR_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include <cmath>      // for log, pow
#include <algorithm>  // for max

namespace standardBML
{
/**
 * @class soybean_development_rate_calculator
 *
 * @brief Determines hourly soybean developments rate using photothermal functions.
 *
 * This module is designed to be used with the `development_index` module.
 *
 * This module calculates soybean development rate using photothermal functions
 * across 5 different stages.
 *
 * | Development index (DVI)          | Growth stages                          |
 * | :------------------------------: | :------------------------------------: |
 * | \f$  -1 \le \text{DVI} < 0   \f$ | sowing to VE (emergence)               |
 * | \f$   0 \le \text{DVI} < 1/3 \f$ |     VE to V0 (cotyledon)               |
 * | \f$ 1/3 \le \text{DVI} < 2/3 \f$ |     V0 to R0 (end of floral induction) |
 * | \f$ 2/3 \le \text{DVI} < 1   \f$ |     R0 to R1 (flowering)               |
 * | \f$   1 \le \text{DVI}       \f$ |     R1 to R7 (maturity)                |
 *
 * The development rate (\f$ r \f$) is calculated using accumulated thermal time for
 * the first stage from from sowing to VE. For the remaining stages after VE,
 * \f$ r \f$ is calculated using a subset of the photothermal functions from the
 *  SOYDEV model: \f$ r = R_{max} f_T f_P \f$ (Setiyono et al. 2007).
 *
 * See Matthews et al. for a more detailed description of the functions used in
 * this module.
 *
 * ### References:
 *
 *  Matthews, M.L. et al. 2021. in preparation. "Soybean-BioCro: A
 *  semi-mechanistic model of soybean growth"
 *
 *  [Setiyono, T.D. et al. 2007. “Understanding and Modeling the Effect of
 *  Temperature and Daylength on Soybean Phenology under High-Yield Conditions.”
 *  Field Crops Research 100(2–3): 257–71.]
 *  (https://doi.org/10.1016/j.fcr.2006.07.011)
 *
 *  [Osborne, T. et al. 2015. “JULES-Crop: A Parametrisation of Crops in the Joint
 *  UK Land Environment Simulator.” Geoscientific Model Development 8(4): 1139–55.]
 *  (https://doi.org/10.5194/gmd-8-1139-2015)
 */
double photoFunc(double P, double Popt, double Pcrit);
double tempFunc(double T, double Tmin, double Topt, double Tmax);

class soybean_development_rate_calculator : public direct_module
{
   public:
    soybean_development_rate_calculator(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          time{get_input(input_quantities, "time")},
          sowing_time{get_input(input_quantities, "sowing_time")},
          maturity_group{get_input(input_quantities, "maturity_group")},
          DVI{get_input(input_quantities, "DVI")},
          day_length{get_input(input_quantities, "day_length")},
          temp{get_input(input_quantities, "temp")},
          Tbase_emr{get_input(input_quantities, "Tbase_emr")},
          TTemr_threshold{get_input(input_quantities, "TTemr_threshold")},
          Rmax_emrV0{get_input(input_quantities, "Rmax_emrV0")},
          Tmin_emrV0{get_input(input_quantities, "Tmin_emrV0")},
          Topt_emrV0{get_input(input_quantities, "Topt_emrV0")},
          Tmax_emrV0{get_input(input_quantities, "Tmax_emrV0")},
          Tmin_R0R1{get_input(input_quantities, "Tmin_R0R1")},
          Topt_R0R1{get_input(input_quantities, "Topt_R0R1")},
          Tmax_R0R1{get_input(input_quantities, "Tmax_R0R1")},
          Tmin_R1R7{get_input(input_quantities, "Tmin_R1R7")},
          Topt_R1R7{get_input(input_quantities, "Topt_R1R7")},
          Tmax_R1R7{get_input(input_quantities, "Tmax_R1R7")},

          // Get pointers to output quantities
          development_rate_per_hour_op{get_op(output_quantities, "development_rate_per_hour")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soybean_development_rate_calculator"; }

   private:
    // References to input quantities
    double const& time;
    double const& sowing_time;
    double const& maturity_group;
    double const& DVI;
    double const& day_length;
    double const& temp;
    double const& Tbase_emr;
    double const& TTemr_threshold;
    double const& Rmax_emrV0;
    double const& Tmin_emrV0;
    double const& Topt_emrV0;
    double const& Tmax_emrV0;
    double const& Tmin_R0R1;
    double const& Topt_R0R1;
    double const& Tmax_R0R1;
    double const& Tmin_R1R7;
    double const& Topt_R1R7;
    double const& Tmax_R1R7;

    // Pointers to output quantities
    double* development_rate_per_hour_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};

string_vector soybean_development_rate_calculator::get_inputs()
{
    return {
        "time",             // days
        "sowing_time",      // days
        "maturity_group",   // dimensionless; maturity group of soybean cultivar
        "DVI",              // dimensionless; development index, see Osborne et al. 2015
        "day_length",       // hours
        "temp",             // degrees C
        "Tbase_emr",        // degrees C; base temp for thermal time from sowing to emergence
        "TTemr_threshold",  // degrees C * days; accumulated thermal time for emergence
        "Rmax_emrV0",       // day^-1
        "Tmin_emrV0",       // degrees C
        "Topt_emrV0",       // degrees C
        "Tmax_emrV0",       // degrees C
        "Tmin_R0R1",        // degrees C
        "Topt_R0R1",        // degrees C
        "Tmax_R0R1",        // degrees C
        "Tmin_R1R7",        // degrees C
        "Topt_R1R7",        // degrees C
        "Tmax_R1R7",        // degrees C
    };
}

string_vector soybean_development_rate_calculator::get_outputs()
{
    return {
        "development_rate_per_hour"  // hour^-1
    };
}

void soybean_development_rate_calculator::do_operation() const
{
    double soybean_development_rate;  // day^-1

    if (time < sowing_time) {
        // The seeds haven't been sown yet, so no development should occur
        soybean_development_rate = 0;  // day^-1
    } else if (DVI < -1) {
        // error, DVI out of bounds, this should never occur unless initial DVI
        // state is less than -1.
        soybean_development_rate = 0;  // day^-1

    } else if (DVI < 0) {
        // 1. Sowing to emergence

        double temp_diff = temp - Tbase_emr;                                    // degrees C
        soybean_development_rate = std::max(0.0, temp_diff / TTemr_threshold);  // day^-1

    } else if (DVI < 0.333) {
        // 2a. Emergence - V0 (cotyledon stage); r = Rmax * f(T)

        soybean_development_rate = Rmax_emrV0 * tempFunc(temp, Tmin_emrV0, Topt_emrV0, Tmax_emrV0) / 3.0;  // day^-1
    } else if (DVI < 0.667) {
        // 2b. V0 (cotyledon) - R0 (end of floral induction); r = Rmax * f(P)

        const double Rmax_V0R0 = 0.0294 + 0.1561 / maturity_group;  // day^-1;
                                                                    // Setiyono et al., 2007, Fig. 12a

        const double Popt_V0R0 = 12.759 - 0.388 * maturity_group -
                                 0.058 * pow(maturity_group, 2);  // hours; Setiyono et al., 2007, Fig 4
        const double Pcrit_V0R0 = 27.275 - 0.493 * maturity_group -
                                  0.066 * pow(maturity_group, 2);  // hours; Setiyono et al., 2007, Fig 4

        double day_length_civil = 1.072 * day_length;  // hours;
            // Converts day length calculated based on solar elevation = -0.83 degrees
            // to civil day length (based on solar elevation angle = -6 degrees)

        soybean_development_rate = Rmax_V0R0 *
                                   photoFunc(day_length_civil, Popt_V0R0, Pcrit_V0R0) / 3.0;  // day^-1

    } else if (DVI < 1) {
        // 2c. R0 (end of floral induction) - R1 (flowering); r = Rmax * f(T)

        const double Rmax_R0R1 = (0.2551 + 0.0965 * maturity_group) /
                                 (1 + 2.1107 * maturity_group);  // day^-1; Setiyono et al., 2007, Fig 11a

        soybean_development_rate = Rmax_R0R1 *
                                   tempFunc(temp, Tmin_R0R1, Topt_R0R1, Tmax_R0R1) / 3.0;  // day^-1

    } else {
        // 3. Reproductive stages, R1 (flowering) - R7 (maturity);
        // r = Rmax * f(T) * f(P)

        const double Rmax_R1R7 = (0.0563 + 0.0228 * maturity_group) /
                                 (1 + 1.9683 * maturity_group);  // day^-1; Setiyono et al., 2007, Fig 11d

        const double Popt_R1R7 = 10.6595 + 2.9706 / maturity_group;  // hrs;
            // Setiyono et al., 2007, Fig 12c

        const double Pcrit_R1R7 = 16.1257 + 4.3143 / maturity_group;  // hrs;
            // Setiyono et al., 2007, Fig 12c

        soybean_development_rate = Rmax_R1R7 *
                                   tempFunc(temp, Tmin_R1R7, Topt_R1R7, Tmax_R1R7) *
                                   photoFunc(day_length, Popt_R1R7, Pcrit_R1R7);  // day^-1
    }

    double development_rate_per_hour = soybean_development_rate / 24.0;  // hr^-1

    // Update the output quantity list
    update(development_rate_per_hour_op, development_rate_per_hour);
}

double tempFunc(double T, double Tmin, double Topt, double Tmax)
{
    double fT;  // dimensionless

    if (T > Tmin && T < Tmax) {
        double alpha = log(2.0) / log((Tmax - Tmin) / (Topt - Tmin));              // dimensionless
        double fT_num_pt1 = 2.0 * pow(T - Tmin, alpha) * pow(Topt - Tmin, alpha);  // dimensionless
        double fT_num_pt2 = pow(T - Tmin, 2.0 * alpha);                            // dimensionless
        double fT_denom = pow(Topt - Tmin, 2.0 * alpha);                           // dimensionless

        fT = (fT_num_pt1 - fT_num_pt2) / fT_denom;  // dimensionless
    } else {
        fT = 0.0;  // dimensionless
    }

    return fT;  // dimensionless
}

double photoFunc(double P, double Popt, double Pcrit)
{
    double m = 3.0;  // hrs; Setiyono et al., 2007 Equation 4, value in text below equation
    double fP;       // dimensionless

    if (P <= Popt) {
        fP = 1.0;  // dimensionless

    } else if (P < Pcrit) {
        double alpha = log(2.0) / log(1.0 + (Pcrit - Popt) / m);  // dimensionless
        double fP_pt1 = 1.0 + (P - Popt) / m;                     // dimensionless
        double fP_pt2 = (Pcrit - P) / (Pcrit - Popt);             // dimensionless
        double fP_pt3 = (Pcrit - Popt) / m;                       // dimensionless

        fP = pow(fP_pt1 * pow(fP_pt2, fP_pt3), alpha);  // dimensionless

    } else {
        fP = 0.0;  // dimensionless
    }

    return fP;  // dimensionless
}

}  // namespace standardBML
#endif
