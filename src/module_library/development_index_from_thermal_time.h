#ifndef DEVELOPMENT_INDEX_FROM_THERMAL_TIME_H
#define DEVELOPMENT_INDEX_FROM_THERMAL_TIME_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class development_index_from_thermal_time
 *
 *  @brief Calculates the development index from the thermal time.
 *
 *  As thermal time (`TTc`) accumulates following sowing, the crop emerges,
 *  passes through vegetative growth stages, and then passes through
 *  reproductive growth stages, eventually reaching maturity. In this model,
 *  these three transitions (emergence, flowering, and maturity) occur at
 *  specific values of thermal time, referred to as `TTemr`, `TTflowering`, and
 *  `TTmaturity`.
 *
 *  At the same time, the current development stage can be characterized by the
 *  development index. At sowing, emergence, flowering, and maturity, `DVI` is
 *  equal to -1, 0, 1, and 2.
 *
 *  In this model, there is a linear dependence of `DVI` on `TTc` within each
 *  stage. So, between sowing and emergence, `DVI = -1 + TTc / TTemr`. Between
 *  emergence and flowering, `DVI = (TTc - TTemr) / TTveg`, where `TTveg` is the
 *  amount of accumulated thermal time required to pass through the vegetative
 *  growth stages. Between flowering and maturity,
 *  `DVI = 1 + (TTc - TTflowering) / TTrep`, where `TTrep` is the amount of
 *  thermal time required to pass through the reproductive stages. Past
 *  maturity, `DVI` continue to increase according to
 *  `DVI = 1 + (TTc - TTflowering) / TTrep`.
 *
 *  This module is intended to be used along with one of the thermal time
 *  differential modules, such as `thermal_time_linear`,
 *  `thermal_time_bilinear`, etc. For an alternative approach, see the
 *  `thermal_time_development_rate_calculator` module.
 */
class development_index_from_thermal_time : public direct_module
{
   public:
    development_index_from_thermal_time(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          TTc{get_input(input_quantities, "TTc")},
          TTemr{get_input(input_quantities, "TTemr")},
          TTveg{get_input(input_quantities, "TTveg")},
          TTrep{get_input(input_quantities, "TTrep")},

          // Get pointers to output quantities
          DVI_op{get_op(output_quantities, "DVI")}

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "development_index_from_thermal_time"; }

   private:
    // Pointers to input quantities
    double const& TTc;
    double const& TTemr;
    double const& TTveg;
    double const& TTrep;

    // Pointers to output quantities
    double* DVI_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};

string_vector development_index_from_thermal_time::get_inputs()
{
    return {
        "TTc",    // degrees C * days
        "TTemr",  // degrees C * days, thermal time from sowing to emergence
        "TTveg",  // degrees C * days, thermal time of vegetative states
        "TTrep"   // degrees C * days, thermal time of reproductive states
    };
}

string_vector development_index_from_thermal_time::get_outputs()
{
    return {
        "DVI"  // dimensionless
    };
}

void development_index_from_thermal_time::do_operation() const
{
    double const TTflowering = TTemr + TTveg;  // degrees C * days

    double const DVI =
        TTc < TTemr         ? TTc / TTemr - 1.0
        : TTc < TTflowering ? (TTc - TTemr) / TTveg
                            : 1.0 + (TTc - TTflowering) / TTrep;  // dimensionless

    // Update the output quantity list
    update(DVI_op, DVI);
}

}  // namespace standardBML
#endif
