#ifndef STANDARDBML_CARBON_ASSIMILATION_TO_BIOMASS_H
#define STANDARDBML_CARBON_ASSIMILATION_TO_BIOMASS_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class carbon_assimilation_to_biomass
 *
 * @brief
 *
 * ### Overivew
 *
 * Models of photosynthesis compute the rate of CO2 assimilation.
 * However, many BioCro modules require the rate at which photosynthesis
 * increases plant biomass. Unless a model computes different components
 * of plant biomass explicitly, the calculation is assumed to be a simple
 * conversion factor.
 *
 * ### How the conversion is computed
 *
 * The conversion is based on the fact that plant
 * biomass is primarily composed of carbohydrates and the ratio of carbon
 * to oxygen and hydrogen is nearly the same for all carbohydrates. Thus,
 * given a number of carbon atoms (i.e., moles of carbon), we can estimate
 * the carbon-hydrogen-oxygen mass (the CHO mass) which will account for
 * ~95% of plant biomass. We call this ratio \f$\kappa\f$ the CHO carbon
 * molar mass. This is not to be confused with the molar mass of carbon.
 *
 * \f[
 *  \kappa = \frac{\mu}{(\text{carbon atoms per mol})}
 * \f]
 *
 * Where \f$\mu\f$ is the molar mass.
 *
 * For example, glucose has a molar mass of 180 g/mol and 6 carbon atoms.
 * So every mol C contributes 12 g of C + 2 g H + 16 g O = 30 g CHO mass.
 * Or (180 g / mol glucose) / (6 mol C / mol glucose) = 30 g CHO / mol C.
 * All monosaccharides have this value. All disaccharides are 28.5 g/mol.
 * Starch, cellulose, and hemi-celluloses have a value of 27 g/mol.
 *
 * Any mixture of these substances will have a carbon molar mass between
 * 27 and 30 g/mol.
 *
 * Thus, given a carbon molar mass \f$\kappa\f$, this module will convert
 * the CO2 assimilation rate in micromol C / m^2 land / s to a biomass
 * accumulation rate in Mg / Ha / hr.
 *
 * ### Conversion from square meters to hectares and seconds to hours.
 *
 * To convert micromol / m^2 / s into Mg / ha / hour, we also must
 * convert the units of time and land area as well as to shift the SI
 * prefixes. The following factor gives the correct conversion:
 *
 * (carbon molar mass) * (3600 s / hr) * (1e-6 mol / umol) *
 * (1e-3 Mg / kg) * (1e4 m^2 / ha)
 * = (carbon molar mass) * 3.6e-5 (s * mol * Mg * m^2) / (hr * micromol * g * ha)
 *
 *
 * ### Generalization of CHO carbon molar mass
 *
 * Note that if other elements are assumed to be in constant ratio with
 * carbon, then this conversion factor gives total biomass, not just
 * CHO mass. The `CHO_carbon_molar_mass` is the amount of dry biomass (grams)
 * per carbon (mol C).
 *
 * For instance, the amino acid glutamate has a carbon molar mass of 29.4
 * g / mol C. But every 5 mol C contributes 1 mol N. However, the carbon
 * molar mass varies more for amino acids than for carbohydrates.
 * Proteinogenic amino acids vary from 18 - 41 g / mol C.
 *
 * Mixtures of amino acids and carbohydrates will have a carbon molar mass
 * in this range, but given that carbohydrates are the largest fraction of plant biomass
 * the carbon molar mass of plant biomass will be close to 27 g / mol C as this
 * is the carbon molar mass of starch, cellulose, and hemi-celluloses.
 *
 * ### Measuring the CHO carbon molar mass of a tissue sample.
 *
 * The carbon molar mass \f$\kappa\f$ of a biomass sample of mass \f$m\f$ can be measured by identifying
 * the number of carbon moles \f$C\f$ contained in the sample:
 *
 * \f[
 *   \kappa = \frac{m}{C}
 * \f]
 *
 * Given a C mass fraction \f$x\f$, the number of mol C in the sample is:
 *
 * \f[ C = \frac{m x}{\mu_C} \f]
 *
 * Where \f$\mu_C=12\f$ g/mol is the molar mass of carbon. Substituting yields:
 *
 * \f[ \kappa = \frac{\mu_C}{x} \f]
 *
 */
class carbon_assimilation_to_biomass : public direct_module
{
   public:
    carbon_assimilation_to_biomass(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          canopy_assimilation_rate_CO2{get_input(input_quantities, "canopy_assimilation_rate_CO2")},
          GrossAssim_CO2{get_input(input_quantities, "GrossAssim_CO2")},
          canopy_photorespiration_rate_CO2{get_input(input_quantities, "canopy_photorespiration_rate_CO2")},
          CHO_carbon_molar_mass{get_input(input_quantities, "CHO_carbon_molar_mass")},

          // Get pointers to output quantities
          canopy_assimilation_rate_op{get_op(output_quantities, "canopy_assimilation_rate")},
          GrossAssim_op{get_op(output_quantities, "GrossAssim")},
          canopy_photorespiration_rate_op{get_op(output_quantities, "canopy_photorespiration_rate")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "carbon_assimilation_to_biomass"; }

   private:
    // References to input quantities
    double const& canopy_assimilation_rate_CO2;
    double const& GrossAssim_CO2;
    double const& canopy_photorespiration_rate_CO2;
    double const& CHO_carbon_molar_mass;

    // Pointers to output quantities
    double* canopy_assimilation_rate_op;
    double* GrossAssim_op;
    double* canopy_photorespiration_rate_op;

    // Main operation
    void do_operation() const;
};

string_vector carbon_assimilation_to_biomass::get_inputs()
{
    return {
        "canopy_assimilation_rate_CO2",      // micromol CO2 / m^2 / s
        "GrossAssim_CO2",                    // micromol CO2 / m^2 / s
        "canopy_photorespiration_rate_CO2",  // micromol CO2 / m^2 / s
        "CHO_carbon_molar_mass"              // g CHO / mol C
    };
}

string_vector carbon_assimilation_to_biomass::get_outputs()
{
    return {
        "canopy_assimilation_rate",     // Mg / ha / hr
        "GrossAssim",                   // Mg / ha / hr
        "canopy_photorespiration_rate"  // Mg / ha / hr
    };
}

void carbon_assimilation_to_biomass::do_operation() const
{
    // Make calculations here

    constexpr double sec_per_hour = 3600;
    constexpr double mol_per_micromol = 1e-6;
    constexpr double square_meters_per_hectare = 1e4;
    constexpr double megagram_per_gram = 1e-6;
    constexpr double a = sec_per_hour * mol_per_micromol * square_meters_per_hectare * megagram_per_gram;

    const double cf = CHO_carbon_molar_mass * a;

    // Use `update` to set outputs
    update(canopy_assimilation_rate_op, canopy_assimilation_rate_CO2 * cf);
    update(GrossAssim_op, GrossAssim_CO2 * cf);
    update(canopy_photorespiration_rate_op, canopy_photorespiration_rate_CO2 * cf);
}

}  // namespace standardBML
#endif
