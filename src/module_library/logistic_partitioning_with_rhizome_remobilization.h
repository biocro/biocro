#ifndef LOGISTIC_PARTITIONING_WITH_RHIZOME_REMOBILIZATION_H
#define LOGISTIC_PARTITIONING_WITH_RHIZOME_REMOBILIZATION_H

#include <cmath>      // for exp
#include <stdexcept>  // for std::range_error
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class logistic_partitioning_with_rhizome_remobilization
 *
 * @brief Calculates carbon partitioning coefficients for plants with rhizomes
 * such as Miscanthus from the development index, using constant coefficients 
 * before emergence (when remobilizing the rhizome carbon) and the logistic-based 
 * functions from Osborne et al. 2015 afterwards.
 *
 * Intended to be used with any of the following modules:
 * - `partitioning_growth_calculator_leaf_costs`
 * - `partitioning_growth_calculator`
 *
 * Note: The "partitioning growth calculator" modules listed above override
 * these coefficients when the net canopy CO2 assimilation rate is negative.
 * Thus, the coefficients calculated here are best understood as coefficients
 * for new growth only.
 *
 * During emergence (\f$ x < 0 \f$, where \f$ x \f$ is the development index),
 * the rhizome acts as a carbon source: \f$ k_{Rhi} \f$ is set to
 * `kRhizome_emr` (which must not be positive), \f$ k_L \f$ and \f$ k_S \f$ are
 * set to `kLeaf_emr` and `kStem_emr`, and the root receives the remaining
 * carbon, \f$ k_R = 1 - k_L - k_S \f$.
 *
 * Otherwise (\f$ x \geq 0 \f$), the coefficients of the leaf, root, and stem
 * are given by
 *
 * \f[ k_i = \frac{\exp{(\alpha_i+\beta_i x)}}  {\exp{(\alpha_R+\beta_R x)} +
 * \exp{(\alpha_S+\beta_S x)} + \exp{(\alpha_L+\beta_L x)} + 1}, \f]
 *
 * where \f$ i = {L, R, S} \f$ for leaf, root, and stem, respectively. The
 * rhizome is the reference organ, with a sink strength of 1:
 *
 * \f[ k_{Rhi} = \frac{1}{\exp{(\alpha_R+\beta_R x)} +
 * \exp{(\alpha_S+\beta_S x)} + \exp{(\alpha_L+\beta_L x)} + 1}. \f]
 *
 * Unlike `partitioning_coefficient_logistic`, this module is intended for
 * crops without grain or shell, so `kGrain` and `kShell` are always zero. (They
 * are still calculated because they are required by the partitioning growth
 * calculators.)
 *
 * ### References:
 *
 * [Osborne, T. et al. 2015. "JULES-Crop: A Parametrisation of Crops in the Joint
 * UK Land Environment Simulator." Geoscientific Model Development 8(4): 1139–55.]
 * (https://doi.org/10.5194/gmd-8-1139-2015)
 */
class logistic_partitioning_with_rhizome_remobilization : public direct_module
{
   public:
    logistic_partitioning_with_rhizome_remobilization(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          alphaLeaf{get_input(input_quantities, "alphaLeaf")},
          alphaRoot{get_input(input_quantities, "alphaRoot")},
          alphaStem{get_input(input_quantities, "alphaStem")},
          betaLeaf{get_input(input_quantities, "betaLeaf")},
          betaRoot{get_input(input_quantities, "betaRoot")},
          betaStem{get_input(input_quantities, "betaStem")},
          DVI{get_input(input_quantities, "DVI")},
          kLeaf_emr{get_input(input_quantities, "kLeaf_emr")},
          kRhizome_emr{get_input(input_quantities, "kRhizome_emr")},
          kStem_emr{get_input(input_quantities, "kStem_emr")},

          // Get pointers to output quantities
          kGrain_op{get_op(output_quantities, "kGrain")},
          kLeaf_op{get_op(output_quantities, "kLeaf")},
          kRhizome_op{get_op(output_quantities, "kRhizome")},
          kRoot_op{get_op(output_quantities, "kRoot")},
          kShell_op{get_op(output_quantities, "kShell")},
          kStem_op{get_op(output_quantities, "kStem")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "logistic_partitioning_with_rhizome_remobilization"; }

   private:
    // References to input quantities
    const double& alphaLeaf;
    const double& alphaRoot;
    const double& alphaStem;
    const double& betaLeaf;
    const double& betaRoot;
    const double& betaStem;
    const double& DVI;
    const double& kLeaf_emr;
    const double& kRhizome_emr;
    const double& kStem_emr;

    // Pointers to output quantities
    double* kGrain_op;
    double* kLeaf_op;
    double* kRhizome_op;
    double* kRoot_op;
    double* kShell_op;
    double* kStem_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};

string_vector logistic_partitioning_with_rhizome_remobilization::get_inputs()
{
    return {
        "alphaLeaf",     // dimensionless
        "alphaRoot",     // dimensionless
        "alphaStem",     // dimensionless
        "betaLeaf",      // dimensionless
        "betaRoot",      // dimensionless
        "betaStem",      // dimensionless
        "DVI",           // dimensionless
        "kLeaf_emr",     // dimensionless
        "kRhizome_emr",  // dimensionless
        "kStem_emr"      // dimensionless
    };
}

string_vector logistic_partitioning_with_rhizome_remobilization::get_outputs()
{
    return {
        "kGrain",    // dimensionless
        "kLeaf",     // dimensionless
        "kRhizome",  // dimensionless
        "kRoot",     // dimensionless
        "kShell",    // dimensionless
        "kStem"      // dimensionless
    };
}

void logistic_partitioning_with_rhizome_remobilization::do_operation() const
{
    // Check for error conditions; kRhizome_emr should be zero or negative,
    // since it applies when the rhizome is acting as a carbon source, and the
    // root coefficient during emergence should not be negative.
    if (kRhizome_emr > 0.0) {
        throw std::range_error("Thrown in logistic_partitioning_with_rhizome_remobilization: kRhizome_emr is positive.");
    }

    if (kLeaf_emr + kStem_emr > 1.0) {
        throw std::range_error("Thrown in logistic_partitioning_with_rhizome_remobilization: kLeaf_emr + kStem_emr is larger than 1.");
    }

    double kLeaf, kRhizome, kRoot, kStem;  // dimensionless

    if (DVI < 0) {
        // During emergence, the rhizome acts as a carbon source and the
        // coefficients are fixed; the root receives the remaining carbon
        kLeaf = kLeaf_emr;
        kRhizome = kRhizome_emr;
        kStem = kStem_emr;
        kRoot = 1.0 - kLeaf - kStem;
    } else {
        // Determine partitioning coefficients using multinomial logistic
        // equations from Osborne et al., 2015 JULES-crop
        // https://doi.org/10.5194/gmd-8-1139-2015

        // Calculate the sink strength of each tissue (relative to the rhizome)
        double const leaf_strength{exp(alphaLeaf + betaLeaf * DVI)};
        double const root_strength{exp(alphaRoot + betaRoot * DVI)};
        double const stem_strength{exp(alphaStem + betaStem * DVI)};
        double constexpr rhizome_strength{1};

        // Calculate the total sink strength
        double const total_strength =
            leaf_strength + root_strength + stem_strength + rhizome_strength;

        // The k values are the fraction of total demand from each tissue
        kLeaf = leaf_strength / total_strength;
        kRhizome = rhizome_strength / total_strength;
        kRoot = root_strength / total_strength;
        kStem = stem_strength / total_strength;
    }

    // Update the output quantities
    update(kGrain_op, 0.0);         // dimensionless
    update(kLeaf_op, kLeaf);        // dimensionless
    update(kRhizome_op, kRhizome);  // dimensionless
    update(kRoot_op, kRoot);        // dimensionless
    update(kShell_op, 0.0);         // dimensionless
    update(kStem_op, kStem);        // dimensionless
}

}  // namespace standardBML
#endif
