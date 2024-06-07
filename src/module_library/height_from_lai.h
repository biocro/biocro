#ifndef HEIGHT_FROM_LAI_H
#define HEIGHT_FROM_LAI_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class height_from_lai
 *
 *  @brief Estimates the canopy height as being proportional to LAI.
 *
 *  This is probably not a realistic model. Height and LAI are positively
 *  correlated during vegetative growth, but this relationship breaks once
 *  leaf senescence starts; when leaves senesce, LAI will decrease without a
 *  corresponding change in height. Even in vegetative stages, the relationship
 *  between height and LAI is probably not linear. Nevertheless, this model is
 *  good enough for many purposes. Here it is implemented as
 *
 *  > `canopy_height = lai / heightf`,
 *
 *  where `lai` is the total leaf area index of the canopy (dimensionless from
 *  m^2 leaf / m^2 ground), `heightf` is a proportionality factor representing
 *  the LAI per height (m^(-1)), and `canopy_height` is the canopy height (m).
 */
class height_from_lai : public direct_module
{
   public:
    height_from_lai(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          heightf{get_input(input_quantities, "heightf")},
          lai{get_input(input_quantities, "lai")},

          // Get pointers to output quantities
          canopy_height_op{get_op(output_quantities, "canopy_height")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "height_from_lai"; }

   private:
    // References to input quantities
    double const& heightf;
    double const& lai;

    // Pointers to output quantities
    double* canopy_height_op;

    // Main operation
    void do_operation() const;
};

string_vector height_from_lai::get_inputs()
{
    return {
        "heightf",  // m^(-1)
        "lai"       // dimensionless
    };
}

string_vector height_from_lai::get_outputs()
{
    return {
        "canopy_height"  // m
    };
}

void height_from_lai::do_operation() const
{
    update(canopy_height_op, lai / heightf);  // m
}

}  // namespace standardBML
#endif
