#ifndef SENESCENCE_COEFFICIENT_LOGISTIC_H
#define SENESCENCE_COEFFICIENT_LOGISTIC_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include <cmath>  // for exp

namespace standardBML
{
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
 * where `<plant organ>` is either `Leaf`, `Stem`, `Root`, or `Rhizome`.
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
double ksene(double rate, double alpha, double beta, double DVI);

class senescence_coefficient_logistic : public direct_module
{
   public:
    senescence_coefficient_logistic(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          DVI{get_input(input_quantities, "DVI")},
          alphaSeneStem{get_input(input_quantities, "alphaSeneStem")},
          alphaSeneLeaf{get_input(input_quantities, "alphaSeneLeaf")},
          betaSeneStem{get_input(input_quantities, "betaSeneStem")},
          betaSeneLeaf{get_input(input_quantities, "betaSeneLeaf")},
          rateSeneLeaf{get_input(input_quantities, "rateSeneLeaf")},
          rateSeneStem{get_input(input_quantities, "rateSeneStem")},
          alphaSeneRoot{get_input(input_quantities, "alphaSeneRoot")},
          alphaSeneRhizome{get_input(input_quantities, "alphaSeneRhizome")},
          betaSeneRoot{get_input(input_quantities, "betaSeneRoot")},
          betaSeneRhizome{get_input(input_quantities, "betaSeneRhizome")},
          rateSeneRoot{get_input(input_quantities, "rateSeneRoot")},
          rateSeneRhizome{get_input(input_quantities, "rateSeneRhizome")},

          // Get pointers to output quantities
          kSeneStem_op{get_op(output_quantities, "kSeneStem")},
          kSeneLeaf_op{get_op(output_quantities, "kSeneLeaf")},
          kSeneRoot_op{get_op(output_quantities, "kSeneRoot")},
          kSeneRhizome_op{get_op(output_quantities, "kSeneRhizome")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "senescence_coefficient_logistic"; }

   private:
    // Refernces to input quantities
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

    // Pointers to output quantities
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

    // Update the output quantities
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

}  // namespace standardBML
#endif
