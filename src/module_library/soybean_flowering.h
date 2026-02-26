#ifndef SOYBEAN_FLOWERING
#define SOYBEAN_FLOWERING

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{

/**
 * @class soybean_flowering
 *
 * @brief Put documentation here.
 *
 */
class soybean_flowering: public differential_module
{
   public:
    soybean_flowering(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities

          flowering_photoperiod_trigger{get_input(input_quantities, "flowering_photoperiod_trigger")},
          day_length{get_input(input_quantities, "day_length")},
          flowering_rate{get_input(input_quantities, "flowering_rate")},
          fruit_set_rate{get_input(input_quantities, "fruit_set_rate")},
          flowering_signal{get_input(input_quantities, "flowering_signal")},
          fruit_set_signal{get_input(input_quantities, "fruit_set_signal")},

          // Get pointers to output quantities
          flowering_signal_op{get_op(output_quantities, "flowering_signal")},
          fruit_set_signal_op{get_op(output_quantities, "fruit_set_signal")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soybean_flowering"; }

   private:
    // References to input quantities

    double const& flowering_photoperiod_trigger;
    double const& day_length;
    double const& flowering_rate;
    double const& fruit_set_rate;
    double const& flowering_signal;
    double const& fruit_set_signal;

    // Pointers to output quantities
    double* flowering_signal_op;
    double* fruit_set_signal_op;
    // Main operation
    void do_operation() const;
};

string_vector soybean_flowering::get_inputs()
{
    return {

        "flowering_photoperiod_trigger",  // m^2 / m^2
        "day_length",                     // m^2 / m^2
        "flowering_signal_rate",          // 1 / d
        "flowering_signal"               // dimensionless
    };
}

string_vector soybean_flowering::get_outputs()
{
    return {
        "flowering_signal",  // 1 / h
    };
}

void soybean_flowering::do_operation() const
{
    double s = day_length < flowering_photoperiod_trigger ? 1 : 0;

    constexpr double eps = 0.01;
    update(flowering_signal_op, flowering_rate / 24 * s * (1 - flowering_signal) * (eps + flowering_signal));
}


// class soybean_development: public direct_module
// {
//    public:
//     soybean_development(
//         state_map const& input_quantities,
//         state_map* output_quantities)
//         : direct_module{},

//           // Get references to input quantities

//           flowering_photoperiod_trigger{get_input(input_quantities, "flowering_photoperiod_trigger")},
//           day_length{get_input(input_quantities, "day_length")},

//           // Get pointers to output quantities
//           flowering_signal_op{get_op(output_quantities, "flowering_signal")},
//           fruit_set_signal_op{get_op(output_quantities, "fruit_set_signal")}
//     {
//     }

//     static string_vector get_inputs() {

//     }

//     static string_vector get_outputs(){

//     }

//     static std::string get_name() { return "soybean_development"; }

//    private:
//     // References to input quantities

//     double const& flowering_photoperiod_trigger;
//     double const& day_length;
//     double const& flowering_rate;
//     double const& fruit_set_rate;
//     double const& flowering_signal;
//     double const& fruit_set_signal;

//     // Pointers to output quantities
//     double* flowering_signal_op;
//     double* fruit_set_signal_op;
//     // Main operation
//     void do_operation() const {
//         update(flowering_signal_op, flowering_rate / 24 * s * (1 - flowering_signal) * (eps + flowering_signal));
//     }
// };


}
#endif
