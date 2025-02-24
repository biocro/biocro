#ifndef STANDARDBML_CARBON_ASSIMILATION_TO_BIOMASS_H
#define STANDARDBML_CARBON_ASSIMILATION_TO_BIOMASS_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class carbon_assimilation_to_biomass
 *
 * @brief Converts the canopy assimilation rate from a flux of CO2 molecules
 * to a rate of dry biomass accumulation.
 *
 * ### Overview
 *
 * Models of photosynthesis compute the rate of CO2 assimilation.
 * However, many BioCro modules require the rate at which photosynthesis
 * increases plant biomass. Here, the latter rate is calculated from the former
 * using
 *
 *   `biomass_accumulation_rate =
 *      CO2_assimilation_rate * dry_biomass_per_carbon * cf`
 *
 * where `biomass_accumulation_rate` is a rate of dry biomass accumulation
 * (Mg / ha / hr), `CO2_assimilation_rate` is a flux of CO2 molecules
 * (micromol / m^2 / s), `dry_biomass_per_carbon` is the amount of dry biomass
 * accumulated per assimilated CO2 molecule (g / mol), and `cf` is an additional
 * conversion factor accounting for SI prefixes and different units of land area
 * and time.
 *
 * The value of `cf` can be found by noting that 1 hr = 3600 s, 1 mol = 1e-6
 * micromol, 1 ha = 1e4 m^2, and 1 g = 1e-6 Mg:
 *
 *  `cf = 3600 * 1e-6 * 1e4 * 1e-6
 *      = 3.6e-5 (s * mol * Mg * m^2) / (hr * micromol * g * ha)`
 *
 * The value of `dry_biomass_per_carbon` depends on the composition of the
 * accumulated biomass, as described below.
 *
 * Note that by assuming that dry biomass is exactly proportional to number of
 * carbon atoms, this model does not account for any inorganic molecules that
 * may contribute to plant biomass, such as inorganic phosphate. However, in
 * most situations this is not expected to lead to substantial errors.
 *
 * ### Dry biomass per carbon for carbohydrates
 *
 * Plant biomass is primarily composed of carbohydrates, and the ratio of carbon
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
 * Where \f$\mu\f$ is the molar mass. Given the molar mass of a substance,
 * and the number of carbon atoms contained in the substance, we can compute
 * \f$\kappa\f$.
 *
 * For example, glucose has a molar mass of 180 g/mol and 6 carbon atoms,
 * assuming isotopes 1-H, 12-C, 16-O. The molar mass varies with the ratio of
 * isotopes, but the average isotopic ratio is close to the same molar mass.
 * So every mol C contributes 12 g of C + 2 g H + 16 g O = 30 g CHO mass.
 * Or (180 g / mol glucose) / (6 mol C / mol glucose) = 30 g CHO / mol C.
 * All hexoses (e.g., glucose, fructose, galactose) all have the same molecular
 * formula C6H12O6, and thus the same molar mass (180 g / mol) and all consist
 * of 6 carbons, again ignoring any variation in isotope distribution.
 * Thus, they all have a carbon molar mass of about 30 g / mol.
 * Disaccharides consisting of two hexose units such as sucrose, maltose, and
 * lactose all have the same molecular formula C12H22O11, therefore the same
 * molar mass, and thus the same carbon molar mass equal to 28.5 g/mol.
 * Polysaccharides such as starch, cellulose, and hemi-celluloses are
 * hexose polymers all have a formula of (C6H10O5)n for n monomers, and
 * thus they have a carbon molar mass of 27 g/mol.
 *
 * The larger molecules have large molar masses, but also typically a
 * proportional increase in the number of carbon atoms. It isn't particularly
 * useful to talk about molecules of a polymer, but the number of carbon atoms
 * is proportional to the mass of the polymer.
 *
 * The molar mass of a mixture of these substances is a weighted average of
 * of their individual molar masses. And therefore will generally be
 * between 27 and 30 g / mol C.
 *
 * Thus, a common choice for `dry_biomass_per_carbon` is to use the value of
 * \f$\kappa\f$ for monosaccharides. This choice is based on an assumption that
 * dry biomass is predominantly composed of simple carbohydrates, and that the
 * composition of plant biomass is fixed over time.
 *
 * ### Generalized dry biomass per carbon
 *
 * In reality, plants contain other elements and molecules besides
 * carbohydrates, which influences the value `dry_biomass_per_carbon`.
 *
 * For instance, the amino acid glutamate has a carbon molar mass of 29.4
 * g / mol C. But every 5 mol C contributes 1 mol N. However, the carbon
 * molar mass varies more for amino acids than for carbohydrates.
 * Proteinogenic amino acids vary from 18 - 41 g / mol C.
 *
 * Mixtures of amino acids and carbohydrates will have a carbon molar mass
 * in this range, but given that carbohydrates are the largest fraction of plant
 * biomass the carbon molar mass of plant biomass will be close to 27 g / mol C
 * as this is the carbon molar mass of starch, cellulose, and hemi-celluloses.
 *
 * ### Measuring the CHO carbon molar mass of a tissue sample.
 *
 * The carbon molar mass \f$\kappa\f$ of a biomass sample of mass \f$m\f$ can be
 * measured by identifying the number of carbon moles \f$C\f$ contained in the
 * sample:
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
          dry_biomass_per_carbon{get_input(input_quantities, "dry_biomass_per_carbon")},

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
    double const& dry_biomass_per_carbon;

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
        "dry_biomass_per_carbon"             // g biomass / mol C
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

    const double cf = dry_biomass_per_carbon * a;

    // Use `update` to set outputs
    update(canopy_assimilation_rate_op, canopy_assimilation_rate_CO2 * cf);
    update(GrossAssim_op, GrossAssim_CO2 * cf);
    update(canopy_photorespiration_rate_op, canopy_photorespiration_rate_CO2 * cf);
}

}  // namespace standardBML
#endif
