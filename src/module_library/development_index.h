#ifndef DEVELOPMENT_INDEX_H
#define DEVELOPMENT_INDEX_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class development_index
 *
 * @brief Calculates the development index (DVI) corresponding to plant growth
 * rate.
 *
 * Osborne et al., (2015) define `DVI` to be:
 *
 * | Development index (DVI)         | Growth stages               |
 * | :-----------------------------: | :-------------------------: |
 * | \f$ -1 \le \text{DVI} < 0 \f$   | sowing to emergence         |
 * | \f$  0 \le \text{DVI} < 1 \f$   | vegetative growth stages    |
 * | \f$  1 \le \text{DVI} < 2 \f$   | reproductive growth stages  |
 *
 *
 *  However, this definition is flexible. For example in the module
 *  `soybean_development_rate_calculator`, we define the stages as:
 *
 * | Development index (DVI)          | Growth stages                          |
 * | :------------------------------: | :------------------------------------: |
 * | \f$  -1 \le \text{DVI} < 0   \f$ | sowing to VE (emergence)               |
 * | \f$   0 \le \text{DVI} < 1/3 \f$ |     VE to V0 (cotyledon)               |
 * | \f$ 1/3 \le \text{DVI} < 2/3 \f$ |     V0 to R0 (end of floral induction) |
 * | \f$ 2/3 \le \text{DVI} < 1   \f$ |     R0 to R1 (flowering)               |
 * | \f$   1 \le \text{DVI}       \f$ |     R1 to R7 (maturity)                |
 *
 * The `DVI` will continue accumulating above 2 if the crop is simulated to reach
 * maturity before the final given timepoint.
 *
 * The `DVI` value can be used in any modules that are dependent on the plant's
 * growth stage, such as carbon partitioning or senescence. The modules
 * `partitioning_coefficient_logistic` and `senescence_coefficient_logistic`
 * are two modules that use the `DVI`.
 *
 * This module takes the `development_rate_per_hour` variable as an input. For
 * soybean, this rate is calculated in the module `soybean_development_rate_calculator`.
 * The `thermal_time_development_rate_calculator` module calculates
 * `development_rate_per_hour` using the thermal time functions from Osborne et al. 2015.
 *
 * ### References:
 *
 * [Osborne, T. et al. 2015. “JULES-Crop: A Parametrisation of Crops in the Joint
 * UK Land Environment Simulator.” Geoscientific Model Development 8(4): 1139–55.]
 * (https://doi.org/10.5194/gmd-8-1139-2015)
 *
 */
class development_index : public differential_module
{
   public:
    development_index(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities
          development_rate_per_hour{get_input(input_quantities, "development_rate_per_hour")},

          // Get pointers to output quantities
          DVI_op{get_op(output_quantities, "DVI")}

    {
    }

    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "development_index"; }

   private:
    // References to input quantities
    const double& development_rate_per_hour;

    // Pointers to output quantities
    double* DVI_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};

string_vector development_index::get_inputs()
{
    return {
        "development_rate_per_hour"  // hour^-1
    };
}

string_vector development_index::get_outputs()
{
    return {
        "DVI"  // dimensionless
    };
}

void development_index::do_operation() const
{
    // Update the output quantity list
    update(DVI_op, development_rate_per_hour);  // dimensionless
}

}  // namespace standardBML
#endif
