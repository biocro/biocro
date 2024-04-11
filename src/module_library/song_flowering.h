#ifndef SONG_FLOWERING_H
#define SONG_FLOWERING_H

#include "../framework/module.h"
#include "../framework/state_map.h"

// This module is based on the photoperiodic flowering model described in
//  Song, Y. H., Smith, R. W., To, B. J., Millar, A. J. & Imaizumi, T. FKF1 Conveys Timing Information for CONSTANS Stabilization in Photoperiodic Flowering. Science 336, 1045–1049 (2012).
// The Song model is not the most up-to-date photoperiod model, but is considerably simpler than the newer one described in
//  Seaton, D. D. et al. Linked circadian outputs control elongation growth and flowering in response to photoperiod and temperature. Molecular Systems Biology 11, 776 (2015).
// The Song model also has the advantage of not requiring a "phase advanced" component
// The Song model uses the outputs from the Locke (2005) circadian clock model described in
//  Locke, J. C. W. et al. Extension of a genetic network model by iterative experimentation and mathematical analysis. Molecular Systems Biology 1, 2005.0013 (2005).
// The Locke model is included here in this module
// The models were implented in C++ by translating from the following files:
//  "Structured data 1" in the supplement to Locke (2005) as viewed with an online SBML viewer (https://sv.insysbio.com/online/)
//

namespace standardBML
{
class song_flowering : public differential_module
{
   public:
    song_flowering(state_map const& input_quantities, state_map* output_quantities)
        : differential_module{},

          // Get pointers to input quantities
          solar_ip{get_ip(input_quantities, "solar")},
          cLm_ip{get_ip(input_quantities, "cLm")},
          cLc_ip{get_ip(input_quantities, "cLc")},
          cLn_ip{get_ip(input_quantities, "cLn")},
          cTm_ip{get_ip(input_quantities, "cTm")},
          cTc_ip{get_ip(input_quantities, "cTc")},
          cTn_ip{get_ip(input_quantities, "cTn")},
          cXm_ip{get_ip(input_quantities, "cXm")},
          cXc_ip{get_ip(input_quantities, "cXc")},
          cXn_ip{get_ip(input_quantities, "cXn")},
          cYm_ip{get_ip(input_quantities, "cYm")},
          cYc_ip{get_ip(input_quantities, "cYc")},
          cYn_ip{get_ip(input_quantities, "cYn")},
          cPn_ip{get_ip(input_quantities, "cPn")},

          // Get pointers to output quantities
          cLm_op{get_op(output_quantities, "cLm")},
          cLc_op{get_op(output_quantities, "cLc")},
          cLn_op{get_op(output_quantities, "cLn")},
          cTm_op{get_op(output_quantities, "cTm")},
          cTc_op{get_op(output_quantities, "cTc")},
          cTn_op{get_op(output_quantities, "cTn")},
          cXm_op{get_op(output_quantities, "cXm")},
          cXc_op{get_op(output_quantities, "cXc")},
          cXn_op{get_op(output_quantities, "cXn")},
          cYm_op{get_op(output_quantities, "cYm")},
          cYc_op{get_op(output_quantities, "cYc")},
          cYn_op{get_op(output_quantities, "cYn")},
          cPn_op{get_op(output_quantities, "cPn")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "song_flowering"; }

   private:
    // Pointers to input quantities
    const double* solar_ip;
    const double* cLm_ip;
    const double* cLc_ip;
    const double* cLn_ip;
    const double* cTm_ip;
    const double* cTc_ip;
    const double* cTn_ip;
    const double* cXm_ip;
    const double* cXc_ip;
    const double* cXn_ip;
    const double* cYm_ip;
    const double* cYc_ip;
    const double* cYn_ip;
    const double* cPn_ip;

    // Pointers to output quantities
    double* cLm_op;
    double* cLc_op;
    double* cLn_op;
    double* cTm_op;
    double* cTc_op;
    double* cTn_op;
    double* cXm_op;
    double* cXc_op;
    double* cXn_op;
    double* cYm_op;
    double* cYc_op;
    double* cYn_op;
    double* cPn_op;

    // Main operation
    void do_operation() const;
};

string_vector song_flowering::get_inputs()
{
    return {
        "solar",
        "cLm",
        "cLc",
        "cLn",
        "cTm",
        "cTc",
        "cTn",
        "cXm",
        "cXc",
        "cXn",
        "cYm",
        "cYc",
        "cYn",
        "cPn"};
}

string_vector song_flowering::get_outputs()
{
    return {
        "cLm",
        "cLc",
        "cLn",
        "cTm",
        "cTc",
        "cTn",
        "cXm",
        "cXc",
        "cXn",
        "cYm",
        "cYc",
        "cYn",
        "cPn"};
}

void song_flowering::do_operation() const
{
    //////////////////////////////////////////
    // Collect inputs and make calculations //
    //////////////////////////////////////////

    // Unpack the solar radiantion
    double solar = *solar_ip;

    // Unpack the circadian clock components
    double cLm = *cLm_ip;
    double cLc = *cLc_ip;
    double cLn = *cLn_ip;
    double cTm = *cTm_ip;
    double cTc = *cTc_ip;
    double cTn = *cTn_ip;
    double cXm = *cXm_ip;
    double cXc = *cXc_ip;
    double cXn = *cXn_ip;
    double cYm = *cYm_ip;
    double cYc = *cYc_ip;
    double cYn = *cYn_ip;
    double cPn = *cPn_ip;

    // Define the circadian clock parameters
    double a = 3.3064;
    double b = 1.0258;
    double c = 1.0258;
    double d = 1.4422;
    double e = 3.6064;
    double f = 1.0237;
    double g1 = 0.876738488;
    double g2 = 0.036805783;
    double g3 = 0.26593318;
    double g4 = 0.538811228;
    double g5 = 1.17803247;
    double g6 = 0.064455137;
    double k1 = 1.817;
    double k10 = 1.7303;
    double k11 = 1.8258;
    double k12 = 1.8066;
    double k13 = 1.2;
    double k2 = 1.5644;
    double k3 = 1.2765;
    double k4 = 2.5734;
    double k5 = 2.7454;
    double k6 = 0.4033;
    double k7 = 6.5585;
    double k8 = 0.6632;
    double k9 = 17.1111;
    double m1 = 1.5283;
    double m10 = 0.2179;
    double m11 = 3.3442;
    double m12 = 4.297;
    double m13 = 0.1347;
    double m14 = 0.6114;
    double m15 = 1.2;
    double m2 = 20.44;
    double m3 = 3.6888;
    double m4 = 3.8231;
    double m5 = 0.0013;
    double m6 = 3.1741;
    double m7 = 0.0492;
    double m8 = 4.0424;
    double m9 = 10.1132;
    double n1 = 5.1694;
    double n2 = 3.0087;
    double n3 = 0.2431;
    double n4 = 0.0857;
    double n5 = 0.1649;
    double p1 = 0.8295;
    double p2 = 4.324;
    double p3 = 2.147;
    double p4 = 0.2485;
    double p5 = 0.5;
    double q1 = 2.4514;
    double q2 = 2.40178;
    double q3 = 1;
    double r1 = 16.8363;
    double r2 = 0.1687;
    double r3 = 0.3166;
    double r4 = 2.1509;
    double r5 = 1.0352;
    double r6 = 3.3017;
    double r7 = 2.2123;
    double r8 = 0.2002;

    // Check whether the plant is illuminated
    // Rather than basing L(t) on specified values for dusk and dawn
    //  as in the original Locke (2005) model, we use a logistic function
    //  based on solar radiation to determine when light is present.
    // This function is 0 for no sunlight and saturates to 1 when
    //  solar reaches ~150, which is well below the max on a typical day
    double L = 1.0 / (1.0 + exp(-0.058 * (solar - 100.0)));

    //////////////////////////////////////
    // Update the output quantity list //
    //////////////////////////////////////

    // Define some helpful lambdas
    auto hill = [](double s, double km, double n) { return pow(s, n) / (pow(km, n) + pow(s, n)); };
    auto mm = [](double s, double km) { return s / (km + s); };

    // Return the clock component derivatives
    update(cLm_op, L * q1 * cPn + n1 * hill(cXn, g1, a) - m1 * mm(cLm, k1));
    update(cLc_op, p1 * cLm - r1 * cLc + r2 * cLn - m2 * mm(cLc, k2));
    update(cLn_op, r1 * cLc - r2 * cLn - m3 * mm(cLn, k3));
    update(cTm_op, n2 * hill(cYn, g2, b) * hill(g3, cLn, c) - m4 * mm(cTm, k4));
    update(cTc_op, p2 * cTm - r3 * cTc + r4 * cTn - ((1.0 - L) * m5 + m6) * mm(cTc, k5));
    update(cTn_op, r3 * cTc - r4 * cTn - ((1.0 - L) * m7 + m8) * mm(cTn, k6));
    update(cXm_op, n3 * hill(cTn, g4, d) - m9 * mm(cXm, k7));
    update(cXc_op, p3 * cXm - r5 * cXc + r6 * cXn - m10 * mm(cXc, k8));
    update(cXn_op, r5 * cXc - r6 * cXn - m11 * mm(cXn, k9));
    update(cYm_op, (L * q2 * cPn + (L * n4 + n5) * hill(g5, cTn, e)) * hill(g6, cLn, f) - m12 * mm(cYm, k10));
    update(cYc_op, p4 * cYm - r7 * cYc + r8 * cYn - m13 * mm(cYc, k11));
    update(cYn_op, r7 * cYc - r8 * cYn - m14 * mm(cYn, k12));
    update(cPn_op, (1.0 - L) * p5 - m15 * mm(cPn, k13) - q3 * L * cPn);
}

}  // namespace standardBML
#endif
