#ifndef PARTITIONING_COEFFICIENT_LOGISTIC_H
#define PARTITIONING_COEFFICIENT_LOGISTIC_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include <cmath>  // for exp

namespace standardBML
{
/**
 * @class partitioning_coefficient_logistic
 *
 * @brief Calculates carbon partitioning coefficients based on logistic-based
 * functions and development index using the logistic-based functions from
 * Osborne et al 2015.
 *
 * Intended to be used with any of the following modules:
 * - `no_leaf_resp_neg_assim_partitioning_growth_calculator`
 * - `no_leaf_resp_partitioning_growth_calculator`
 * - `partitioning_growth_calculator`
 *
 * Using the following function, calculates the percentage of carbon allocated
 * to the root, stem, leaf, shell, and grain at a given development index.
 *
 * \f[ k_i = \frac{\exp{(\alpha_i+\beta_i x)}}  {\exp{(\alpha_R+\beta_R x)} +
 * \exp{(\alpha_S+\beta_S x)} + \exp{(\alpha_L+\beta_L x)} +
 * \exp{(\alpha_{Sh}+\beta_{Sh} x)} + 1}, \f]
 *
 * where \f$ i = {R, S, L, Sh} \f$ for root, stem, leaf, and shell respectively,
 * and \f$ x \f$ is the development index. For the grain,
 *
 * \f[ k_G = \frac{1}{\exp{(\alpha_R+\beta_R x)} + \exp{(\alpha_S+\beta_S x)} +
 * \exp{(\alpha_L+\beta_L x)} + \exp{(\alpha_{Sh}+\beta_{Sh})} + 1}. \f]
 *
 * See Matthews et al. for more description of how this module was used in
 * Soybean-BioCro and for details on the parameter fitting to identify the
 * \f$ \alpha \text{ and } \beta \f$ parameters. Note that the original model
 * did not include a shell component.
 *
 * Although it is not used in the soybean model, this module also includes an
 * option for a rhizome to contribute carbon to other organs during emergence.
 * See comments in the code for more details.
 *
 * ### References:
 *
 * [Matthews, M. L.et al. 2021. "Soybean-BioCro: a semi-mechanistic model of
 * soybean growth." in silico Plants 4, diab032.]
 * (https://doi.org/10.1093/insilicoplants/diab032)
 *
 * [Osborne, T. et al. 2015. "JULES-Crop: A Parametrisation of Crops in the Joint
 * UK Land Environment Simulator." Geoscientific Model Development 8(4): 1139–55.]
 * (https://doi.org/10.5194/gmd-8-1139-2015)
 */
double kcoeff(double alpha, double beta, double DVI, double denom);

class partitioning_coefficient_logistic : public direct_module
{
   public:
    partitioning_coefficient_logistic(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          DVI{get_input(input_quantities, "DVI")},
          alphaRoot{get_input(input_quantities, "alphaRoot")},
          alphaStem{get_input(input_quantities, "alphaStem")},
          alphaLeaf{get_input(input_quantities, "alphaLeaf")},
          alphaShell{get_input(input_quantities, "alphaShell")},
          betaRoot{get_input(input_quantities, "betaRoot")},
          betaStem{get_input(input_quantities, "betaStem")},
          betaLeaf{get_input(input_quantities, "betaLeaf")},
          betaShell{get_input(input_quantities, "betaShell")},
          kRhizome_emr{get_input(input_quantities, "kRhizome_emr")},

          // Get pointers to output quantities
          kRoot_op{get_op(output_quantities, "kRoot")},
          kStem_op{get_op(output_quantities, "kStem")},
          kLeaf_op{get_op(output_quantities, "kLeaf")},
          kShell_op{get_op(output_quantities, "kShell")},
          kGrain_op{get_op(output_quantities, "kGrain")},
          kRhizome_op{get_op(output_quantities, "kRhizome")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "partitioning_coefficient_logistic"; }

   private:
    // Pointers to input quantities
    const double& DVI;
    const double& alphaRoot;
    const double& alphaStem;
    const double& alphaLeaf;
    const double& alphaShell;
    const double& betaRoot;
    const double& betaStem;
    const double& betaLeaf;
    const double& betaShell;
    const double& kRhizome_emr;

    // Pointers to output quantities
    double* kRoot_op;
    double* kStem_op;
    double* kLeaf_op;
    double* kShell_op;
    double* kGrain_op;
    double* kRhizome_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};

string_vector partitioning_coefficient_logistic::get_inputs()
{
    return {
        "DVI",          // dimensionless, development index
        "alphaRoot",    // dimensionless
        "alphaStem",    // dimensionless
        "alphaLeaf",    // dimensionless
        "alphaShell",   // dimensionless
        "betaRoot",     // dimensionless
        "betaStem",     // dimensionless
        "betaLeaf",     // dimensionless
        "betaShell",    // dimensionless
        "kRhizome_emr"  // dimensionless
    };
}

string_vector partitioning_coefficient_logistic::get_outputs()
{
    return {
        "kRoot",    // dimensionless
        "kStem",    // dimensionless
        "kLeaf",    // dimesnionless
        "kShell",   // dimensionless
        "kGrain",   // dimensionless
        "kRhizome"  // dimensionless
    };
}

void partitioning_coefficient_logistic::do_operation() const
{
    // Determine partitioning coefficients using multinomial logistic equations
    // from Osborne et al., 2015 JULES-crop https://doi.org/10.5194/gmd-8-1139-2015

    // denominator term for kRoot, kStem, kLeaf, and kGrain
    double kDenom = exp(alphaRoot + betaRoot * DVI) +
                    exp(alphaStem + betaStem * DVI) +
                    exp(alphaLeaf + betaLeaf * DVI) +
                    exp(alphaShell + betaShell * DVI) + 1.0;  // dimensionless

    double kRoot = kcoeff(alphaRoot, betaRoot, DVI, kDenom);     // dimensionless
    double kStem = kcoeff(alphaStem, betaStem, DVI, kDenom);     // dimensionless
    double kLeaf = kcoeff(alphaLeaf, betaLeaf, DVI, kDenom);     // dimensionless
    double kShell = kcoeff(alphaShell, betaShell, DVI, kDenom);  // dimensionless
    double kGrain = 1.0 / kDenom;                                // dimensionless

    // Give option for rhizome to contribute to growth during the emergence stage,
    // kRhizome_emr is an input parameter and should be non-positive.
    // To ignore rhizome, set kRhizome_emr to 0 in input parameter file, and
    // adjust initial leaf, stem, and root biomasses accordingly.
    double kRhizome{DVI < 0 ? kRhizome_emr : 0};  // dimensionless

    // Update the output quantities
    update(kRoot_op, kRoot);        // dimensionless
    update(kStem_op, kStem);        // dimensionless
    update(kLeaf_op, kLeaf);        // dimensionless
    update(kShell_op, kShell);      // dimensionless
    update(kGrain_op, kGrain);      // dimensionless
    update(kRhizome_op, kRhizome);  // dimensionless
}

double kcoeff(double alpha, double beta, double DVI, double denom)
{
    return exp(alpha + beta * DVI) / denom;  // dimensionless
}

}  // namespace standardBML
#endif
