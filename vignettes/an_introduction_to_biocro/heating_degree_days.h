#ifndef HEATING_DEGREE_DAYS_H
#define HEATING_DEGREE_DAYS_H

#include "../modules.h"
#include "../state_map.h"

class heating_degree_days : public differential_module {


   public:
    heating_degree_days(
        const state_map&
            input_quantities,
        state_map*
            output_quantities
    )
    : differential_module{"heating_degree_days"},
      temp{get_input(input_quantities, "temp")},
      base_temperature{get_input(input_quantities, "base_temperature")},
      heating_degree_days_op{get_op(output_quantities, "heating_degree_days")}
    {}

    static string_vector get_inputs();
    static string_vector get_outputs();

  private:
    // References to input parameters:
    const double& temp;
    const double& base_temperature;

    // Pointers to output parameters:
    double* heating_degree_days_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};

string_vector heating_degree_days::get_inputs() {
    return {
        "temp",
        "base_temperature"
      };
}

string_vector heating_degree_days::get_outputs() {
    return {
        "heating_degree_days"
    };
}

void heating_degree_days::do_operation() const {
    double temperature_deficit { base_temperature > temp ?
                                 (base_temperature - temp) : 0 };
    update(heating_degree_days_op, temperature_deficit / 24.0);
}


#endif
