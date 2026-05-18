#ifndef ROOT_ONEDIM_TEST_H
#define ROOT_ONEDIM_TEST_H
#include <functional>  // for std::function
#include <map>         // std::map
#include <cmath>       // std::sin

#include "../framework/module.h"
#include "../framework/state_map.h"

#include "../math/roots/onedim/secant.h"
#include "../math/roots/onedim/bisection.h"
#include "../math/roots/onedim/regula_falsi.h"
#include "../math/roots/onedim/ridder.h"
#include "../math/roots/onedim/illinois.h"
#include "../math/roots/onedim/pegasus.h"
#include "../math/roots/onedim/newton.h"
#include "../math/roots/onedim/halley.h"
#include "../math/roots/onedim/steffensen.h"
#include "../math/roots/onedim/fixed_point.h"
#include "../math/roots/onedim/dekker.h"
#include "../math/roots/onedim/dekker_newton.h"
#include "../math/roots/onedim/anderson_bjorck.h"

namespace standardBML
{

/**
 * @brief Function object for Kepler's equation. Kepler's equation relates
 * the mean anomaly \f$y\f$ to the eccentric anomaly \f$x\f$ of an elliptical orbit of
 * eccentricity \f$\varepsilon\f$.
 *
 * \f[ y = x - \varepsilon \sin x \f]
 *
 * To calculate the position of an object at a certain time, one easily computes
 * the mean anomaly, then one must invert the above equation to find the eccentric
 * anomaly. Iterative numerical methods for root finding are ideal.
 *
 * We use this as a test example, because it is simple to compute the derivatives,
 * and because higher eccentricity orbits can cause problems for newton/secant
 * like updates. The function is almost the identity function at low eccentricity,
 * while at high eccentricity, the function can have cubic-roots at y=0 and ecc=1.
 *
 * Although Kepler's equation is only valid for orbits of eccentricity < 1, the
 * above equation can be extended to eccentricity > 1; however, there can be
 * three simple roots rather than a unique answer.
 *
 * For test purposes, we compute `y` from `x` so that correct root is known.
 *
 */
struct root_test_function {
    double epsilon;
    double answer;
    double y;

    root_test_function(double ep, double ans) : epsilon{ep}, answer{ans}
    {
        y = ans - ep * std::sin(ans);
    }

    double operator()(double x)
    {
        return x - epsilon * std::sin(x) - y;
    }

    double derivative(double x)
    {
        return 1 - epsilon * std::cos(x);
    }

    double second_derivative(double x)
    {
        return epsilon * std::sin(x);
    }
};

struct fixed_point_test_function {
    double epsilon;
    double answer;

    fixed_point_test_function(double ep, double a) : epsilon{ep}, answer{a} {}

    double operator()(double x)
    {
        return answer + epsilon * std::sin(x - answer);
    }
};

/**
 * @brief Function object for Kepler's equation. Kepler's equation relates
 * the mean anomaly \f$y\f$ to the eccentric anomaly \f$x\f$ of an elliptical orbit of
 * eccentricity \f$\varepsilon\f$.
 *
 * \f[ y = x - \varepsilon \sin x \f]
 *
 * To calculate the position of an object at a certain time, one easily computes
 * the mean anomaly, then one must invert the above equation to find the eccentric
 * anomaly. Iterative numerical methods for root finding are ideal.
 *
 * We use this as a test example, because it is simple to compute the derivatives,
 * and because higher eccentricity orbits can cause problems for newton/secant
 * like updates. The function is almost the identity function at low eccentricity,
 * while at high eccentricity, the function can have cubic-roots at y=0 and ecc=1.
 *
 * Although Kepler's equation is only valid for orbits of eccentricity < 1, the
 * above equation can be extended to eccentricity > 1; however, there can be
 * three simple roots rather than a unique answer.
 *
 * For test purposes, we compute `y` from `x=1` so that correct root is known. If
 * any root solving method returns something other than `x=1`, then it has failed
 * to find a root, or it has converged to a different root.
 *
 * Every method returns the `root`, `residual`, and `iteration` as well as a `flag`
 * indicating the reason for termination. If a method is functioning correctly,
 * then:
 *
 *  1. `root` = `answer`
 *  2. `residual` = 0  (residual is the value of the function).
 *  3. `iteration` < `max_iterations`; the number of iterations is proportional to
 *      the number of function evaluations. So fewer means faster.
 *  4. `flag` is 1, 2, or 3. `flag` = 0 should not occur. It indicates that
 *      the state is valid and iteration should continue. All other flags
 *      indicate failures.
 *
 *
 */
class root_onedim_test : public direct_module
{
    struct result {
        double* root_op;
        double* residual_op;
        double* iteration_op;
        double* flag_op;

        result(
            state_map* output_quantities,
            std::string&& name)
            : root_op{get_op(output_quantities, name + "_root")},
              residual_op{get_op(output_quantities, name + "_residual")},
              iteration_op{get_op(output_quantities, name + "_iteration")},
              flag_op{get_op(output_quantities, name + "_flag")}
        {
        }
    };

   public:
    root_onedim_test(
        state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          ecc{get_input(input_quantities, "ecc")},
          answer{get_input(input_quantities, "answer")},
          max_iterations{get_input(input_quantities, "max_iterations")},
          abs_tol{get_input(input_quantities, "abs_tol")},
          rel_tol{get_input(input_quantities, "rel_tol")},
          lower_bracket{get_input(input_quantities, "lower_bracket")},
          upper_bracket{get_input(input_quantities, "upper_bracket")},
          single_guess{get_input(input_quantities, "single_guess")},

          // Get pointers to output quantities
          secant_result{output_quantities, "secant"},
          fixed_point_result{output_quantities, "fixed_point"},
          newton_result{output_quantities, "newton"},
          halley_result{output_quantities, "halley"},
          steffensen_result{output_quantities, "steffensen"},
          bisection_result{output_quantities, "bisection"},
          regula_falsi_result{output_quantities, "regula_falsi"},
          ridder_result{output_quantities, "ridder"},
          illinois_result{output_quantities, "illinois"},
          pegasus_result{output_quantities, "pegasus"},
          anderson_bjorck_result{output_quantities, "anderson_bjorck"},
          dekker_result{output_quantities, "dekker"},
          dekker_newton_result{output_quantities, "dekker_newton"}

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "root_onedim_test"; }

   private:
    // Pointers to input quantities
    const double& ecc;
    const double& answer;
    const double& max_iterations;
    const double& abs_tol;
    const double& rel_tol;
    const double& lower_bracket;
    const double& upper_bracket;
    const double& single_guess;

    // Pointers to output quantities
    result secant_result;
    result fixed_point_result;
    result newton_result;
    result halley_result;
    result steffensen_result;
    result bisection_result;
    result regula_falsi_result;
    result ridder_result;
    result illinois_result;
    result pegasus_result;
    result anderson_bjorck_result;
    result dekker_result;
    result dekker_newton_result;

    // Main operation
    void do_operation() const;

    static string_vector make_qname(std::string& name)
    {
        return {
            name + "_root",
            name + "_residual",
            name + "_iteration",
            name + "_flag"};
    }

    void inline update_result(
        const result& r, root_finding::result_t& result) const
    {
        update(r.root_op, result.root);
        update(r.residual_op, result.residual);
        update(r.iteration_op, result.iteration);
        update(r.flag_op, static_cast<int>(result.flag));
    }
};

string_vector root_onedim_test::get_inputs()
{
    return {
        "ecc",
        "answer",
        "max_iterations",
        "abs_tol",
        "rel_tol",
        "lower_bracket",
        "upper_bracket",
        "single_guess"};
}

string_vector root_onedim_test::get_outputs()
{
    string_vector out;
    const string_vector methods = {
        "secant", "fixed_point", "newton", "halley", "steffensen",
        "bisection", "regula_falsi", "ridder",
        "illinois", "pegasus", "anderson_bjorck", "dekker",
        "dekker_newton"};
    for (auto name : methods) {
        string_vector sv = make_qname(name);
        out.insert(out.end(), sv.begin(), sv.end());
    }

    return out;
}

void root_onedim_test::do_operation() const
{
    // Collect inputs and make calculations
    using namespace root_finding;
    result_t result;
    root_test_function test{ecc, answer};

    fixed_point_test_function fix_pt_test{ecc, answer};
    size_t iter = static_cast<size_t>(max_iterations);

    result = secant(iter, abs_tol, rel_tol)
                 .solve(test, lower_bracket, upper_bracket);
    update_result(secant_result, result);

    result = fixed_point(iter, abs_tol, rel_tol)
                 .solve(fix_pt_test, single_guess);
    update_result(fixed_point_result, result);

    result = newton(iter, abs_tol, rel_tol)
                 .solve(test, single_guess);
    update_result(newton_result, result);

    result = halley(iter, abs_tol, rel_tol)
                 .solve(test, single_guess);
    update_result(halley_result, result);

    result = steffensen(iter, abs_tol, rel_tol)
                 .solve(test, single_guess);
    update_result(steffensen_result, result);

    result = bisection(iter, abs_tol, rel_tol)
                 .solve(test, lower_bracket, upper_bracket);
    update_result(bisection_result, result);

    result = regula_falsi(iter, abs_tol, rel_tol)
                 .solve(test, lower_bracket, upper_bracket);
    update_result(regula_falsi_result, result);

    result = ridder(iter, abs_tol, rel_tol)
                 .solve(test, lower_bracket, upper_bracket);
    update_result(ridder_result, result);

    result = illinois(iter, abs_tol, rel_tol)
                 .solve(test, lower_bracket, upper_bracket);
    update_result(illinois_result, result);

    result = pegasus(iter, abs_tol, rel_tol)
                 .solve(test, lower_bracket, upper_bracket);
    update_result(pegasus_result, result);

    result = anderson_bjorck(iter, abs_tol, rel_tol)
                 .solve(test, lower_bracket, upper_bracket);
    update_result(anderson_bjorck_result, result);

    result = dekker(iter, abs_tol, rel_tol)
                 .solve(test, lower_bracket, upper_bracket);
    update_result(dekker_result, result);

    result = dekker_newton(iter, abs_tol, rel_tol)
                 .solve(test, lower_bracket, upper_bracket);
    update_result(dekker_newton_result, result);
}

}  // namespace standardBML
#endif
