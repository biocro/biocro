#include <cmath>        // for std::abs, sqrt
#include <algorithm>    // for std::max, std::min
#include <stdexcept>    // for std::range_error, std::logic_error
#include "constants.h"  // for eps_zero
#include "quadratic_root.h"

using calculation_constants::eps_zero;
using std::abs;
using std::logic_error;
using std::max;
using std::min;
using std::range_error;

/**
 *  @brief Computes one of the roots of a quadratic equation.
 *
 *  In general, an equation of the type
 *
 *  \f[ a \cdot x^2 + b \cdot x + c = 0 \f]    [Equation `(1)`]
 *
 *  has two solutions given by
 *
 *  \f[ x = \frac{-b \pm \sqrt{b^2 - 4 \cdot a \cdot c}}{2 \cdot a} \f].    [Equation `(2)`]
 *
 *  These are referred to as the roots of a quadratic equation. Here, we refer
 *  to the separate roots as \f$ x_{plus} = (-b + \sqrt{b^2 - 4ac}) / (2a) \f$
 *  and \f$ x_{minus} = (-b - \sqrt{b^2 - 4ac}) / (2a) \f$.
 *
 *  In the special case where \f$ a = 0 \f$, Equation `(1)` reduces to
 *
 *  \f[ b \cdot x + c = 0 \f],    [Equation `(3)`]
 *
 *  which has a single solution given by \f$ x_{single} = -c / b \f$.
 *
 *  Another special case is where \f$ b^2 - 4 a c < 0\f$. In this case, the two
 *  roots are imaginary. Usually this is considered an error in BioCro, so this
 *  function will throw an error in that case.
 *
 *  If \f$ a = 0 \f$, this function returns \f$ x_{single} \f$. Otherwise, it
 *  returns either \f$ x_{plus} \f$ or \f$ x_{minus} \f$ depending on the value
 *  of the `root_type` input argument:
 *  - When `root_type` is `plus`, \f$ x_{plus} \f$ is returned.
 *  - When `root_type` is `minus`, \f$ x_{minus} \f$ is returned.
 *  - When `root_type` is `larger`, the larger root is returned.
 *  - When `root_type` is `smaller`, the smaller root is returned.
 *
 *  Convenience functions are also provided that fix `root_type` to a particular
 *  value:
 *  - `quadratic_root_plus()` sets `root_type` to `plus`.
 *  - `quadratic_root_minus()` sets `root_type` to `minus`.
 *  - `quadratic_root_max()` sets `root_type` to `larger`.
 *  - `quadratic_root_min()` sets `root_type` to `smaller`.
 *
 *  @param [in] a The value of \f$ a \f$.
 *
 *  @param [in] b The value of \f$ b \f$.
 *
 *  @param [in] c The value of \f$ c \f$.
 *
 *  @param [in] root_type A switch that determines the return value (see above).
 *
 *  @return One of the roots of \f$ a \cdot x^2 + b \cdot x + c = 0 \f$.
 */
double quadratic_root(
    double a,
    double b,
    double c,
    quadratic_root_type root_type)
{
    if (abs(a) < eps_zero) {
        return -c / b;
    } else {
        double const root_term = b * b - 4 * a * c;

        if (root_term < 0) {
            throw range_error("Thrown in quadratic_root: root_term is negative.");
        }

        double const root_plus = (-b + sqrt(root_term)) / (2 * a);
        double const root_minus = (-b - sqrt(root_term)) / (2 * a);

        switch (root_type) {
            case plus:
                return root_plus;
            case minus:
                return root_minus;
            case larger:
                return max(root_plus, root_minus);
            case smaller:
                return min(root_plus, root_minus);
            default:
                throw logic_error("Undefined root_type passed to quadratic_root");
        }
    }
}
