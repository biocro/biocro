#ifndef PARTITIONING_COEFFICIENT_LOGISTIC_H
#define PARTITIONING_COEFFICIENT_LOGISTIC_H

#include "../modules.h"
#include "../state_map.h"
#include <cmath> // for exp

double kcoeff(double alpha, double beta, double DVI, double denom);

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
 * to the root, stem, leaf, and grain at a given development index.
 *
 * \f[ k_i = \frac{\exp{(\alpha_i+\beta_i x)}}  {\exp{(\alpha_R+\beta_R x)} +
 * \exp{(\alpha_L+\beta_L x)} + \exp{(\alpha_S+\beta_S x)} + 1}, \f]
 *
 * where \f$ i = {R, L, S} \f$ for root, leaf, and stem respectively, and
 * \f$ x \f$ is the development index. For the grain,
 *
 * \f[ k_G = \frac{1}{\exp{(\alpha_R+\beta_R x)} + \exp{(\alpha_L+\beta_L x)}
 * + \exp{(\alpha_S+\beta_S x)} + 1}. \f]
 *
 * See Matthews et al. for more description of how this module was used in
 * Soybean-BioCro and for details on the parameter fitting to identify the
 * \f$ \alpha \text{ and } \beta \f$ parameters.
 *
 * ### References:
 *
 * Matthews, M.L. et al. 2021. in preparation. "Soybean-BioCro: A
 * semi-mechanistic model of soybean growth"
 *
 * [Osborne, T. et al. 2015. “JULES-Crop: A Parametrisation of Crops in the Joint
 * UK Land Environment Simulator.” Geoscientific Model Development 8(4): 1139–55.]
 * (https://doi.org/10.5194/gmd-8-1139-2015)
 */
class partitioning_coefficient_logistic : public direct_module
{
   public:
    partitioning_coefficient_logistic(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{"partitioning_coefficient_logistic"},

          // Get references to input quantities
          DVI{get_input(input_quantities, "DVI")},
          alphaRoot{get_input(input_quantities, "alphaRoot")},
          alphaStem{get_input(input_quantities, "alphaStem")},
          alphaLeaf{get_input(input_quantities, "alphaLeaf")},
          betaRoot{get_input(input_quantities, "betaRoot")},
          betaStem{get_input(input_quantities, "betaStem")},
          betaLeaf{get_input(input_quantities, "betaLeaf")},
          kRhizome_emr{get_input(input_quantities, "kRhizome_emr")},

          // Get pointers to output quantities
          kStem_op{get_op(output_quantities, "kStem")},
          kLeaf_op{get_op(output_quantities, "kLeaf")},
          kRoot_op{get_op(output_quantities, "kRoot")},
          kRhizome_op{get_op(output_quantities, "kRhizome")},
          kGrain_op{get_op(output_quantities, "kGrain")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // Pointers to input quantities
    const double& DVI;
    const double& alphaRoot;
    const double& alphaStem;
    const double& alphaLeaf;
    const double& betaRoot;
    const double& betaStem;
    const double& betaLeaf;
    const double& kRhizome_emr;

    // Pointers to output quantities
    double* kStem_op;
    double* kLeaf_op;
    double* kRoot_op;
    double* kRhizome_op;
    double* kGrain_op;

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
        "betaRoot",     // dimensionless
        "betaStem",     // dimensionless
        "betaLeaf",     // dimensionless
        "kRhizome_emr"  // dimensionless
    };
}

string_vector partitioning_coefficient_logistic::get_outputs()
{
    return {
        "kStem",     // dimensionless
        "kLeaf",     // dimesnionless
        "kRoot",     // dimensionless
        "kRhizome",  // dimensionless
        "kGrain"     // dimensionless
    };
}

void partitioning_coefficient_logistic::do_operation() const
{
    // Determine partitioning coefficients using multinomial logistic equations
    // from Osborne et al., 2015 JULES-crop https://doi.org/10.5194/gmd-8-1139-2015

    // denominator term for kRoot, kStem, kLeaf, and kGrain
    double kDenom = exp(alphaRoot + betaRoot * DVI) + exp(alphaLeaf + betaLeaf * DVI)
                    + exp(alphaStem + betaStem * DVI) + 1.0; // dimensionless

    double kRoot = kcoeff(alphaRoot, betaRoot, DVI, kDenom); // dimensionless
    double kStem = kcoeff(alphaStem, betaStem, DVI, kDenom); // dimensionless
    double kLeaf = kcoeff(alphaLeaf, betaLeaf, DVI, kDenom); // dimensionless
    double kGrain = 1.0 / kDenom; // dimensionless

    // Give option for rhizome to contribute to growth during the emergence stage,
    // kRhizome_emr is an input parameter and should be non-positive.
    // To ignore rhizome, set kRhizome_emr to 0 in input parameter file, and
    // adjust initial leaf, stem, and root biomasses accordingly.
    double kRhizome;
    if (DVI < 0) {
        kRhizome = kRhizome_emr;  //dimensionless
    } else {
        kRhizome = 0.0;  // dimensionless
    }

    // Update the output quantities
    update(kStem_op, kStem);        //dimensionless
    update(kLeaf_op, kLeaf);        //dimensionless
    update(kRoot_op, kRoot);        //dimensionless
    update(kRhizome_op, kRhizome);  //dimensionless
    update(kGrain_op, kGrain);      //dimensionless
}

double kcoeff(double alpha, double beta, double DVI, double denom)
{
    double k = exp(alpha + beta * DVI) / denom;
    return k;  // dimensionless
}

#endif
