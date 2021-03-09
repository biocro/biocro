#ifndef HEATING_DEGREE_DAYS_H
#define HEATING_DEGREE_DAYS_H

#include "../modules.h"
#include "../state_map.h"

class heating_degree_days : public DerivModule {


   public:
    heating_degree_days(
        const state_map*
            input_parameters,
        state_map*
            output_parameters
    )
    : DerivModule{"heating_degree_days"},
      temp_ip{get_ip(input_parameters, "temp")},
      base_temperature_ip{get_ip(input_parameters, "base_temperature")},
      heating_degree_days_op{get_op(output_parameters, "heating_degree_days")}
    {}

    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();
    static std::string get_description();

  private:
    // Pointers to input parameters:
    const double* temp_ip;
    const double* base_temperature_ip;

    // Pointers to output parameters:
    double* heating_degree_days_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};

std::vector<std::string> heating_degree_days::get_inputs() {
    return {
        "temp",
        "base_temperature"
      };
}

std::vector<std::string> heating_degree_days::get_outputs() {
    return {
        "heating_degree_days"
    };
}

void heating_degree_days::do_operation() const {
    double temperature_deficit { *base_temperature_ip > *temp_ip ?
            (*base_temperature_ip - *temp_ip) : 0 };
    update(heating_degree_days_op, temperature_deficit / 24.0);
}


#endif
