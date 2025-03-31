#ifndef SECANT_METHOD
#define SECANT_METHOD

#include <cmath>      // for std::isfinite, std::abs
#include <stdexcept>  // for std::runtime_error

// Helper function declarations
inline bool is_close(double x, double y, double atol, double rtol);
inline bool is_zero(double x, double atol);

/**
 * @brief Finds the zero of a function using the secant method.
 *
 * @details Given a function \f$f : \mathbb{R} \to \mathbb{R}\f$,
 * this function tries to identify a zero \f$x^*\f$ such that \f$f(x^*) = 0\f$,
 * using the secant method. The secant method is similar the Newton method.
 *
 * To recall Newton's method, we approximate the unknown function about
 * a point \f$x\f$ as a linear polyomial \f$ f(x) = f(x_0)  + f'(x_0) (x - x_0 ) \f$.
 * Given a estimate of the root, this method then iterates the following function
 *
 * \f[ x \maptso x - \frac{f(x)}{f'(x)} \f]
 *
 * to produce a sequence of approximations of the root. Given a good guess,
 * the sequence will converge to the root rather quickly. The method fails
 * when \f$ f'(x) = 0 \f$ and the sequences of approximate roots can
 * diverge when close to a point where \f$ f'(x) = 0 \f$ nearly so.
 *
 * The method achieves its best performance when the root is a simple root:
 * that is when the intersection of the graph of \f$f(x)\f$ with \f$y=0\f$
 * is transversal and not tangent.
 *
 * For instance, Newton's method can identify the root of \f$ f(x) = x^2 \f$
 * but the intersection is not transveral, \f$f'(x) = 0\f$ at the root.
 * This pathology slows the method.
 *
 * The secant method uses the same logic as Newton's method but instead of
 * using the first derivative, we approximate \f$f\f$ using a secant, computed
 * from two points.
 *
 *  \f[ f(x) \approx f(x_1) + \left(\frac{f(x_1) - f(x_0)}{x_1 - x_0}\right) \left(x -x_1\right) \f]
 *
 * The root of this approximation is computed to produce to refine a guess for
 * the root. Let \f$y_n = f(x_n)\f$; the secant method's update rule is:
 *
 * \f[ x_{n+1} = x_n - y_n \left(\frac{x_n - x_{n-1}}{y_n - y_{n-1}}\right)\f]
 *
 * Under ideal conditions, this method approaches the rate of convergence
 * achieved in Newton's method. The same pathologies can also frustrate it.
 *
 * The secant method requires two starting guesses rather than one.
 * Good starting guesses are important to achieving good performance. It is also
 * important to identify potential pathologies.
 *
 * The order of initial guesses can affect convergence, so make the second guess
 * the closer of the two if possible.
 *
 * @param [in] fun A function or object with `double operator()(double)`
 *             signature. This routine is templated so any type with a call
 *             operator with the correct type signature will work.
 *
 * @param [in] x0 The first guess.
 *
 * @param [in] x1 The second guess.
 *
 * @param [in] max_iter The total number of iterations allowed. The secant
 *             method will terminate when this number is reached.
 *
 * @param [in] atol The absolute tolerance used to test for floating point
 *             number equality; primarily used to test if a zero has been found:
 *             Terminate if `abs(check) < atol`
 *
 * @param [in] rtol The relative tolerance used to test if iterates are not
 *             improving: Terminate if `abs(x0 - x1) <= atol + rtol * abs(x1)`
 *
 * @param [out] check The value of the function at its last evaluation. If a
 *              root has been found, this should equal zero to within the
 *              absolute tolerance.
 *
 * @param [out] counter The number of iterations completed at termination of the
 *              secant method. For example, a value of zero indicates that no
 *              iterations were needed, while a value of 10 indicates 10
 *              iterations were completed.
 *
 * @return Approximate root. If the method fails, it will return its last (and
 *         presumably best) guess for the root. If `check` is not zero, then the
 *         method has failed. This routine will terminate if NaN or non-finite
 *         guesses are encountered.
 */
template <typename F>
double find_root_secant_method(
    F const fun,
    double x0,
    double x1,
    size_t const max_iter,
    double const atol,
    double const rtol,
    double& check,
    size_t& counter)
{
    // Check inputs for trivial solutions
    double y0 = fun(x0);
    if (is_zero(y0, atol)) {
        counter = 0;
        check = y0;
        return x0;
    }

    // Guesses should be different
    if (is_close(x1, x0, atol, rtol)) {
        throw std::runtime_error(
            "Within tolerance, x0 == x1. Initial guesses should be distinct.");
    }

    // Check second solution
    double y1 = fun(x1);
    if (is_zero(y1, atol)) {
        counter = 0;
        check = y1;
        return x1;
    }

    for (size_t n = 1; n <= max_iter; ++n) {
        double secant_slope = (y1 - y0) / (x1 - x0);

        // Secant update formula
        double x2 = x1 - y1 / secant_slope;

        // Stop if the secant formula generates a non-finite value. This should
        // not occur, except when y1 == y0 but x1 != x0.
        if (not std::isfinite(x2)) {
            counter = n;
            check = y1;
            return x1;
        }

        double y2 = fun(x2);

        // Test convergence; if y2 == 0, then x2 is a root.
        if (is_zero(y2, atol)) {
            counter = n;
            check = y2;
            return x2;
        }
        // If no improvement in guesses, then terminate; prevents x1 == x0.
        if (is_close(x1, x2, atol, rtol)) {
            counter = n;
            check = y2;
            return x2;
        }

        // Update
        x0 = x1;
        x1 = x2;
        y0 = y1;
        y1 = y2;
    }

    counter = max_iter;
    check = y1;
    return x1;
}

// Helper function definitions
inline bool is_close(double x, double y, double atol, double rtol)
{
    return std::abs(x - y) <= atol + rtol * std::abs(y);
}

inline bool is_zero(double x, double atol)
{
    return std::abs(x) <= atol;
}

#endif
