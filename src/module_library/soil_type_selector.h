#ifndef SOIL_TYPE_SELECTOR_H
#define SOIL_TYPE_SELECTOR_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
struct soilText_str {
    double silt;
    double clay;
    double sand;
    double air_entry;
    double b;
    double Ks;
    double satur;
    double fieldc;
    double wiltp;
    double bulk_density;
};

enum class SoilType { sand,
                      loamy_sand,
                      sandy_loam,
                      loam,
                      silt_loam,
                      sandy_clay_loam,
                      clay_loam,
                      silty_clay_loam,
                      sandy_clay,
                      silty_clay,
                      clay };

const soilText_str get_soil_properties(SoilType soiltype);

// For backward compatibility:
soilText_str get_soil_properties(int soiltype);
// This map is based on Table 9.1 on page 130 of Campbell and Norman,
// Introduction to Environmental Biophysics.  Bulk density values are
// taken from function getsoilprop.c from Melanie (Colorado). The bulk
// density of sand in getsoilprop.c is 0, which isn't sensible, and
// here 1.60 Mg / m^-3 is used.
//
// The wiltp value of 0.21 (corrected from 0.32) for silty clay loam
// is based on the list of book corrections published at
// http://www.public.iastate.edu/~bkh/teaching/505/norman_book_corrections.pdf.
//
// YH: Not sure where the satur values come from. They are not in Table 9.1 in Campbell and Norman.
// I think it represents porosity. Rawls et al 1982 (https://elibrary.asabe.org/abstract.asp?aid=33720) provides a good source for the parameter value. However, the porosity value has large uncertainty as shown in the paper. This means they can be changed as needed.
// Based on my calibration of soil water data against Gray et al 2016
// (https://www.nature.com/articles/nplants2016132),
// I modified some of them by aligning the high and low flows to observations.
std::map<SoilType, soilText_str> soil_parameters =
    {
        //                             d = dimensionless
        //                             d     d     d     J kg^-1     d     kg s m-3     d     d      d     Mg m^-3
        //                             silt  clay  sand  air_entry   b     Ks          satur fieldc wiltp bulk_density
        {SoilType::sand,            {0.05, 0.03, 0.92, -0.7, 1.7, 5.8e-3, 0.437, 0.09, 0.03, 1.60}},
        {SoilType::loamy_sand,      {0.12, 0.07, 0.81, -0.9, 2.1, 1.7e-3, 0.437, 0.13, 0.06, 1.55}},
        {SoilType::sandy_loam,      {0.25, 0.10, 0.65, -1.5, 3.1, 7.2e-4, 0.42,  0.21, 0.10, 1.50}},
        {SoilType::loam,            {0.39, 0.21, 0.40, -1.1, 4.5, 3.7e-4, 0.42,  0.27, 0.12, 1.30}},
        {SoilType::silt_loam,       {0.65, 0.15, 0.20, -2.1, 4.7, 1.9e-4, 0.501, 0.33, 0.13, 1.36}},
        {SoilType::sandy_clay_loam, {0.13, 0.27, 0.60, -2.8, 4.0, 1.2e-4, 0.398, 0.26, 0.15, 1.39}},
        {SoilType::clay_loam,       {0.34, 0.34, 0.32, -2.6, 5.2, 6.4e-5, 0.420, 0.32, 0.20, 1.35}},
        {SoilType::silty_clay_loam, {0.58, 0.33, 0.09, -3.3, 6.6, 4.2e-5, 0.471, 0.37, 0.21, 1.24}},
        {SoilType::sandy_clay,      {0.07, 0.40, 0.53, -2.9, 6.0, 3.3e-5, 0.430, 0.34, 0.24, 1.30}},
        {SoilType::silty_clay,      {0.45, 0.45, 0.10, -3.4, 7.9, 2.5e-5, 0.479, 0.39, 0.25, 1.28}},
        {SoilType::clay,            {0.20, 0.60, 0.20, -3.7, 7.6, 1.7e-5, 0.475, 0.40, 0.27, 1.19}}

};

soilText_str get_soil_properties(int soiltype)
{
    return get_soil_properties(static_cast<SoilType>(soiltype));
}

const soilText_str get_soil_properties(SoilType soiltype)
{
    return soil_parameters.at(soiltype);
}

class soil_type_selector : public direct_module
{
   public:
    soil_type_selector(state_map const& input_quantities, state_map* output_quantities)
        : direct_module(),

          // Get pointers to input quantities
          soil_type_indicator_1(get_input(input_quantities, "soil_type_indicator_1")),
          soil_type_indicator_2(get_input(input_quantities, "soil_type_indicator_2")),
          soil_type_indicator_3(get_input(input_quantities, "soil_type_indicator_3")),
          soil_type_indicator_4(get_input(input_quantities, "soil_type_indicator_4")),
          soil_type_indicator_5(get_input(input_quantities, "soil_type_indicator_5")),
          soil_type_indicator_6(get_input(input_quantities, "soil_type_indicator_6")),

          // Get pointers to output quantities for layer 1
          soil_silt_content_1_op(get_op(output_quantities, "soil_silt_content_1")),
          soil_clay_content_1_op(get_op(output_quantities, "soil_clay_content_1")),
          soil_saturated_conductivity_1_op(get_op(output_quantities, "soil_saturated_conductivity_1")),
          soil_saturation_capacity_1_op(get_op(output_quantities, "soil_saturation_capacity_1")),
          soil_field_capacity_1_op(get_op(output_quantities, "soil_field_capacity_1")),
          soil_wilting_point_1_op(get_op(output_quantities, "soil_wilting_point_1")),
          soil_bulk_density_1_op(get_op(output_quantities, "soil_bulk_density_1")),

          // Get pointers to output quantities for layer 2
          soil_silt_content_2_op(get_op(output_quantities, "soil_silt_content_2")),
          soil_clay_content_2_op(get_op(output_quantities, "soil_clay_content_2")),
          soil_saturated_conductivity_2_op(get_op(output_quantities, "soil_saturated_conductivity_2")),
          soil_saturation_capacity_2_op(get_op(output_quantities, "soil_saturation_capacity_2")),
          soil_field_capacity_2_op(get_op(output_quantities, "soil_field_capacity_2")),
          soil_wilting_point_2_op(get_op(output_quantities, "soil_wilting_point_2")),
          soil_bulk_density_2_op(get_op(output_quantities, "soil_bulk_density_2")),

          // Get pointers to output quantities for layer 3
          soil_silt_content_3_op(get_op(output_quantities, "soil_silt_content_3")),
          soil_clay_content_3_op(get_op(output_quantities, "soil_clay_content_3")),
          soil_saturated_conductivity_3_op(get_op(output_quantities, "soil_saturated_conductivity_3")),
          soil_saturation_capacity_3_op(get_op(output_quantities, "soil_saturation_capacity_3")),
          soil_field_capacity_3_op(get_op(output_quantities, "soil_field_capacity_3")),
          soil_wilting_point_3_op(get_op(output_quantities, "soil_wilting_point_3")),
          soil_bulk_density_3_op(get_op(output_quantities, "soil_bulk_density_3")),

          // Get pointers to output quantities for layer 4
          soil_silt_content_4_op(get_op(output_quantities, "soil_silt_content_4")),
          soil_clay_content_4_op(get_op(output_quantities, "soil_clay_content_4")),
          soil_saturated_conductivity_4_op(get_op(output_quantities, "soil_saturated_conductivity_4")),
          soil_saturation_capacity_4_op(get_op(output_quantities, "soil_saturation_capacity_4")),
          soil_field_capacity_4_op(get_op(output_quantities, "soil_field_capacity_4")),
          soil_wilting_point_4_op(get_op(output_quantities, "soil_wilting_point_4")),
          soil_bulk_density_4_op(get_op(output_quantities, "soil_bulk_density_4")),

          // Get pointers to output quantities for layer 5
          soil_silt_content_5_op(get_op(output_quantities, "soil_silt_content_5")),
          soil_clay_content_5_op(get_op(output_quantities, "soil_clay_content_5")),
          soil_saturated_conductivity_5_op(get_op(output_quantities, "soil_saturated_conductivity_5")),
          soil_saturation_capacity_5_op(get_op(output_quantities, "soil_saturation_capacity_5")),
          soil_field_capacity_5_op(get_op(output_quantities, "soil_field_capacity_5")),
          soil_wilting_point_5_op(get_op(output_quantities, "soil_wilting_point_5")),
          soil_bulk_density_5_op(get_op(output_quantities, "soil_bulk_density_5")),

          // Get pointers to output quantities for layer 6
          soil_silt_content_6_op(get_op(output_quantities, "soil_silt_content_6")),
          soil_clay_content_6_op(get_op(output_quantities, "soil_clay_content_6")),
          soil_saturated_conductivity_6_op(get_op(output_quantities, "soil_saturated_conductivity_6")),
          soil_saturation_capacity_6_op(get_op(output_quantities, "soil_saturation_capacity_6")),
          soil_field_capacity_6_op(get_op(output_quantities, "soil_field_capacity_6")),
          soil_wilting_point_6_op(get_op(output_quantities, "soil_wilting_point_6")),
          soil_bulk_density_6_op(get_op(output_quantities, "soil_bulk_density_6"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_type_selector"; }

   private:
    // Pointers to input quantities
    const double& soil_type_indicator_1;
    const double& soil_type_indicator_2;
    const double& soil_type_indicator_3;
    const double& soil_type_indicator_4;
    const double& soil_type_indicator_5;
    const double& soil_type_indicator_6;

    // Pointers to output quantities for layer 1
    double* soil_silt_content_1_op;
    double* soil_clay_content_1_op;
    double* soil_saturated_conductivity_1_op;
    double* soil_saturation_capacity_1_op;
    double* soil_field_capacity_1_op;
    double* soil_wilting_point_1_op;
    double* soil_bulk_density_1_op;

    // Pointers to output quantities for layer 2
    double* soil_silt_content_2_op;
    double* soil_clay_content_2_op;
    double* soil_saturated_conductivity_2_op;
    double* soil_saturation_capacity_2_op;
    double* soil_field_capacity_2_op;
    double* soil_wilting_point_2_op;
    double* soil_bulk_density_2_op;

    // Pointers to output quantities for layer 3
    double* soil_silt_content_3_op;
    double* soil_clay_content_3_op;
    double* soil_saturated_conductivity_3_op;
    double* soil_saturation_capacity_3_op;
    double* soil_field_capacity_3_op;
    double* soil_wilting_point_3_op;
    double* soil_bulk_density_3_op;

    // Pointers to output quantities for layer 4
    double* soil_silt_content_4_op;
    double* soil_clay_content_4_op;
    double* soil_saturated_conductivity_4_op;
    double* soil_saturation_capacity_4_op;
    double* soil_field_capacity_4_op;
    double* soil_wilting_point_4_op;
    double* soil_bulk_density_4_op;

    // Pointers to output quantities for layer 5
    double* soil_silt_content_5_op;
    double* soil_clay_content_5_op;
    double* soil_saturated_conductivity_5_op;
    double* soil_saturation_capacity_5_op;
    double* soil_field_capacity_5_op;
    double* soil_wilting_point_5_op;
    double* soil_bulk_density_5_op;

    // Pointers to output quantities for layer 6
    double* soil_silt_content_6_op;
    double* soil_clay_content_6_op;
    double* soil_saturated_conductivity_6_op;
    double* soil_saturation_capacity_6_op;
    double* soil_field_capacity_6_op;
    double* soil_wilting_point_6_op;
    double* soil_bulk_density_6_op;

    // Main operation
    void do_operation() const;
};

string_vector soil_type_selector::get_inputs()
{
    return {
        "soil_type_indicator_1",  // unitless int
        "soil_type_indicator_2",  // unitless int
        "soil_type_indicator_3",  // unitless int
        "soil_type_indicator_4",  // unitless int
        "soil_type_indicator_5",  // unitless int
        "soil_type_indicator_6"   // unitless int
    };
}

string_vector soil_type_selector::get_outputs()
{
    return {
        "soil_silt_content_1",            // unitless
        "soil_clay_content_1",            // unitless
        "soil_saturated_conductivity_1",  // kg s / m^3
        "soil_saturation_capacity_1",     // m^3 / m^3
        "soil_field_capacity_1",          // m^3 / m^3
        "soil_wilting_point_1",           // m^3 / m^3
        "soil_bulk_density_1",            // Mg / m^3
        "soil_silt_content_2",            // unitless
        "soil_clay_content_2",            // unitless
        "soil_saturated_conductivity_2",  // kg s / m^3
        "soil_saturation_capacity_2",     // m^3 / m^3
        "soil_field_capacity_2",          // m^3 / m^3
        "soil_wilting_point_2",           // m^3 / m^3
        "soil_bulk_density_2",            // Mg / m^3
        "soil_silt_content_3",            // unitless
        "soil_clay_content_3",            // unitless
        "soil_saturated_conductivity_3",  // kg s / m^3
        "soil_saturation_capacity_3",     // m^3 / m^3
        "soil_field_capacity_3",          // m^3 / m^3
        "soil_wilting_point_3",           // m^3 / m^3
        "soil_bulk_density_3",            // Mg / m^3
        "soil_silt_content_4",            // unitless
        "soil_clay_content_4",            // unitless
        "soil_saturated_conductivity_4",  // kg s / m^3
        "soil_saturation_capacity_4",     // m^3 / m^3
        "soil_field_capacity_4",          // m^3 / m^3
        "soil_wilting_point_4",           // m^3 / m^3
        "soil_bulk_density_4",            // Mg / m^3
        "soil_silt_content_5",            // unitless
        "soil_clay_content_5",            // unitless
        "soil_saturated_conductivity_5",  // kg s / m^3
        "soil_saturation_capacity_5",     // m^3 / m^3
        "soil_field_capacity_5",          // m^3 / m^3
        "soil_wilting_point_5",           // m^3 / m^3
        "soil_bulk_density_5",            // Mg / m^3
        "soil_silt_content_6",            // unitless
        "soil_clay_content_6",            // unitless
        "soil_saturated_conductivity_6",  // kg s / m^3
        "soil_saturation_capacity_6",     // m^3 / m^3
        "soil_field_capacity_6",          // m^3 / m^3
        "soil_wilting_point_6",           // m^3 / m^3
        "soil_bulk_density_6"             // Mg / m^3
    };
}

void soil_type_selector::do_operation() const
{
    // Collect inputs and make calculations
    soilText_str soil_properties_1 = get_soil_properties((int)(soil_type_indicator_1));  // get_soil_properties(int) is located in AuxBioCro.cpp
    soilText_str soil_properties_2 = get_soil_properties((int)(soil_type_indicator_2));
    soilText_str soil_properties_3 = get_soil_properties((int)(soil_type_indicator_3));
    soilText_str soil_properties_4 = get_soil_properties((int)(soil_type_indicator_4));
    soilText_str soil_properties_5 = get_soil_properties((int)(soil_type_indicator_5));
    soilText_str soil_properties_6 = get_soil_properties((int)(soil_type_indicator_6));

    double saturated_conductivity_1 = soil_properties_1.Ks;  // Convert units to cm/hr (through linear fit)
    double saturated_conductivity_2 = soil_properties_2.Ks;
    double saturated_conductivity_3 = soil_properties_3.Ks;
    double saturated_conductivity_4 = soil_properties_4.Ks;
    double saturated_conductivity_5 = soil_properties_5.Ks;
    double saturated_conductivity_6 = soil_properties_6.Ks;

    // Update the output quantity list for layer 1
    update(soil_silt_content_1_op, soil_properties_1.silt);
    update(soil_clay_content_1_op, soil_properties_1.clay);
    update(soil_saturated_conductivity_1_op, saturated_conductivity_1);
    update(soil_saturation_capacity_1_op, soil_properties_1.satur);
    update(soil_field_capacity_1_op, soil_properties_1.fieldc);
    update(soil_wilting_point_1_op, soil_properties_1.wiltp);
    update(soil_bulk_density_1_op, soil_properties_1.bulk_density);

    // Update the output quantity list for layer 2
    update(soil_silt_content_2_op, soil_properties_2.silt);
    update(soil_clay_content_2_op, soil_properties_2.clay);
    update(soil_saturated_conductivity_2_op, saturated_conductivity_2);
    update(soil_saturation_capacity_2_op, soil_properties_2.satur);
    update(soil_field_capacity_2_op, soil_properties_2.fieldc);
    update(soil_wilting_point_2_op, soil_properties_2.wiltp);
    update(soil_bulk_density_2_op, soil_properties_2.bulk_density);

    // Update the output quantity list for layer 3
    update(soil_silt_content_3_op, soil_properties_3.silt);
    update(soil_clay_content_3_op, soil_properties_3.clay);
    update(soil_saturated_conductivity_3_op, saturated_conductivity_3);
    update(soil_saturation_capacity_3_op, soil_properties_3.satur);
    update(soil_field_capacity_3_op, soil_properties_3.fieldc);
    update(soil_wilting_point_3_op, soil_properties_3.wiltp);
    update(soil_bulk_density_3_op, soil_properties_3.bulk_density);

    // Update the output quantity list for layer 4
    update(soil_silt_content_4_op, soil_properties_4.silt);
    update(soil_clay_content_4_op, soil_properties_4.clay);
    update(soil_saturated_conductivity_4_op, saturated_conductivity_4);
    update(soil_saturation_capacity_4_op, soil_properties_4.satur);
    update(soil_field_capacity_4_op, soil_properties_4.fieldc);
    update(soil_wilting_point_4_op, soil_properties_4.wiltp);
    update(soil_bulk_density_4_op, soil_properties_4.bulk_density);

    // Update the output quantity list for layer 5
    update(soil_silt_content_5_op, soil_properties_5.silt);
    update(soil_clay_content_5_op, soil_properties_5.clay);
    update(soil_saturated_conductivity_5_op, saturated_conductivity_5);
    update(soil_saturation_capacity_5_op, soil_properties_5.satur);
    update(soil_field_capacity_5_op, soil_properties_5.fieldc);
    update(soil_wilting_point_5_op, soil_properties_5.wiltp);
    update(soil_bulk_density_5_op, soil_properties_5.bulk_density);

    // Update the output quantity list for layer 6
    update(soil_silt_content_6_op, soil_properties_6.silt);
    update(soil_clay_content_6_op, soil_properties_6.clay);
    update(soil_saturated_conductivity_6_op, saturated_conductivity_6);
    update(soil_saturation_capacity_6_op, soil_properties_6.satur);
    update(soil_field_capacity_6_op, soil_properties_6.fieldc);
    update(soil_wilting_point_6_op, soil_properties_6.wiltp);
    update(soil_bulk_density_6_op, soil_properties_6.bulk_density);
}
}  // namespace standardBML
#endif
