#ifndef SENESCENCE_COEFFICIENT_LOGISTIC_H
#define SENESCENCE_COEFFICIENT_LOGISTIC_H

#include "../modules.h"
#include "../state_map.h"
#include <cmath> // for exp

double ksene(double rate, double alpha, double beta, double DVI);

/**
 * @class senescence_coefficient_logistic
 *
 * @brief Calculates coefficients representing fraction of biomass senesced for
 * each organ; coefficents are represented as a logistic functions depending on
 * development index.
 *
 * Intended to be used with the `senescence_logistic` module.
 *
 * The senescence fraction \f$ s_i \f$ is computed according to this
 * formula:
 *
 * \f[ s_i = \frac{r_i}{1+\exp{(\alpha_i + \beta_i x)}}. \f]
 *
 * Where the subscript \f$ i \f$ indicates the plant tissue (leaf, stem, root,
 * rhizome), \f$ r_i \f$ is the maximum rate of senescence for that tissue, and
 * \f$ \alpha_i \f$ and \f$ \beta_i \f$ define the shape of the logistic curve.
 *
 * These parameters are represented in the code by the following input and output
 * parameters:
 *
 * - \f$ s_i \f$ : `kSene<plant organ>`
 * - \f$ r_i \f$ : `rateSene<plant organ>`
 * - \f$ \alpha_i \f$ : `alphaSene<plant organ>`
 * - \f$ \beta_i \f$ : `betaSene<plant organ>`
 * - \f$ x \f$ : `DVI`
 *
 * where <plant organ> is one of {Leaf, Stem, Root, Rhizome}.
 *
 * See Matthews et al. for more description of how this module was used in
 * Soybean-BioCro and for details on the parameter fitting to identify the
 * \f$ \alpha \text{ and } \beta \f$ parameters.
 *
 * ### References
 *
 * Matthews, M.L. et al. in preparation. "Soybean-BioCro: A semi-mechanistic
 * model of soybean growth"
 *
 */
class senescence_coefficient_logistic : public SteadyModule
{
   public:
    senescence_coefficient_logistic(
        const state_map* input_parameters,
        state_map* output_parameters)
        : SteadyModule{"senescence_coefficient_logistic"},

          // Get references to input parameters
          DVI{get_input(input_parameters, "DVI")},
          alphaSeneStem{get_input(input_parameters, "alphaSeneStem")},
          alphaSeneLeaf{get_input(input_parameters, "alphaSeneLeaf")},
          betaSeneStem{get_input(input_parameters, "betaSeneStem")},
          betaSeneLeaf{get_input(input_parameters, "betaSeneLeaf")},
          rateSeneLeaf{get_input(input_parameters, "rateSeneLeaf")},
          rateSeneStem{get_input(input_parameters, "rateSeneStem")},
          alphaSeneRoot{get_input(input_parameters, "alphaSeneRoot")},
          alphaSeneRhizome{get_input(input_parameters, "alphaSeneRhizome")},
          betaSeneRoot{get_input(input_parameters, "betaSeneRoot")},
          betaSeneRhizome{get_input(input_parameters, "betaSeneRhizome")},
          rateSeneRoot{get_input(input_parameters, "rateSeneRoot")},
          rateSeneRhizome{get_input(input_parameters, "rateSeneRhizome")},

          // Get pointers to output parameters
          kSeneStem_op{get_op(output_parameters, "kSeneStem")},
          kSeneLeaf_op{get_op(output_parameters, "kSeneLeaf")},
          kSeneRoot_op{get_op(output_parameters, "kSeneRoot")},
          kSeneRhizome_op{get_op(output_parameters, "kSeneRhizome")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // Refernces to input parameters
    const double& DVI;
    const double& alphaSeneStem;
    const double& alphaSeneLeaf;
    const double& betaSeneStem;
    const double& betaSeneLeaf;
    const double& rateSeneLeaf;
    const double& rateSeneStem;
    const double& alphaSeneRoot;
    const double& alphaSeneRhizome;
    const double& betaSeneRoot;
    const double& betaSeneRhizome;
    const double& rateSeneRoot;
    const double& rateSeneRhizome;

    // Pointers to output parameters
    double* kSeneStem_op;
    double* kSeneLeaf_op;
    double* kSeneRoot_op;
    double* kSeneRhizome_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};

string_vector senescence_coefficient_logistic::get_inputs()
{
    return {
        "DVI",               // dimensionless, development index
        "alphaSeneStem",     // dimensionless
        "alphaSeneLeaf",     // dimensionless
        "betaSeneStem",      // dimensionless
        "betaSeneLeaf",      // dimensionless
        "rateSeneLeaf",      // dimensionless, maximum percentage of leaf senesced at a given timestep
        "rateSeneStem",      // dimensionless, maximum percentage of stem senesced at a given timestep
        "alphaSeneRoot",     // dimensionless
        "alphaSeneRhizome",  // dimensionless
        "betaSeneRoot",      // dimensionless
        "betaSeneRhizome",   // dimensionless
        "rateSeneRoot",      // dimensionless, maximum percentage of root senesced at a given timestep
        "rateSeneRhizome"    // dimensionless, maximum percentage of rhizome senesced at a given timestep
    };
}

string_vector senescence_coefficient_logistic::get_outputs()
{
    return {
        "kSeneStem",    // dimensionless
        "kSeneLeaf",    // dimensionless
        "kSeneRoot",    // dimensionless
        "kSeneRhizome"  // dimensionless
    };
}

void senescence_coefficient_logistic::do_operation() const
{
    double kSeneStem = ksene(rateSeneStem, alphaSeneStem, betaSeneStem, DVI);
    double kSeneLeaf = ksene(rateSeneLeaf, alphaSeneLeaf, betaSeneLeaf, DVI);
    double kSeneRoot = ksene(rateSeneRoot, alphaSeneRoot, betaSeneRoot, DVI);
    double kSeneRhizome = ksene(rateSeneRhizome, alphaSeneRhizome, betaSeneRhizome, DVI);

    // Update the output parameters
    update(kSeneStem_op, kSeneStem);        // dimensionless
    update(kSeneLeaf_op, kSeneLeaf);        // dimensionless
    update(kSeneRoot_op, kSeneRoot);        // dimensionless
    update(kSeneRhizome_op, kSeneRhizome);  // dimensionless
}

double ksene(double rate, double alpha, double beta, double DVI)
{
    double kSene = rate / (1.0 + exp(alpha + beta * DVI));
    return kSene;  // dimensionless
}

#endif
