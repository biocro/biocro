#include <cmath>
#include <stdexcept>
#include <limits>

inline bool is_close(double x, double y, double atol, double rtol){
    return std::abs(x - y) <= atol + rtol * std::abs(y);
}

inline bool is_zero(double x, double atol){
    return std::abs(x) <= atol;
}

template<typename F>
double find_root_secant_method(
    F fun, double x0, double x1,
    size_t max_iter = 100,
    double atol = std::numeric_limits<double>::epsilon(),
    double rtol = std::numeric_limits<double>::epsilon()
){

    // check inputs for trivial solutions
    double y0 = fun(x0);
    if (is_zero(y0, atol)) { return x0; }

    // guesses should be different
    if (is_close(x1, x0, atol, rtol)) {
        throw std::runtime_error(
            "Within tolerance, x0 == x1. Initial guesses should be distinct. ");
    }

    double y1 = fun(x1);
    if (is_zero(y1, atol)) { return x1; }

    double x2;
    for (size_t n = 0; n < max_iter; ++n){

        double delta_y = y1 - y0;
        if (is_zero(delta_y, atol)) {
            throw std::runtime_error("Encountered secant = zero.");
        }

        // secant formula
        x2 = (y1 * x0 - y0 * x1)/delta_y;
        double y2 = fun(x2);

        // check convergence
        if (is_zero(y2, atol)) {
            return x2;
        }

        // update
        x0 = x1;
        x1 = x2;
        y0 = y1;
        y1 = y2;
    }

    throw std::runtime_error("Secant method exceeded max iterations.");
}

struct Interval {
    double a, b;
};

bool same_signs(double x, double y){
    return x * y > 0 ;
}

template<typename F, typename Method>
double find_root_bracketing(
    F fun, Interval x,
    Method update,
    size_t max_iter = 100,
    double atol = std::numeric_limits<double>::epsilon(),
    double rtol = std::numeric_limits<double>::epsilon()
){
    Interval y{fun(x.a), fun(x.b)};

    if (is_zero(y.a, atol)) { return x.a;}
    if (is_zero(y.b, atol)) { return x.b;}

    if (same_signs(y.a, y.b)){
        throw std::runtime_error("Must provided a valid starting bracket.");
    }

    for(size_t n =0; n < max_iter; ++n) {


        double c = update(x, y);
        double yc = fun(c);

        if (is_close(x.a, x.b)) { return c;}
        if (is_zero(yc, atol)) {return c;}

        if (same_signs(yc, y.a)) {
            x.a = c;
            y.a = yc;
        } else {
            x.b = c;
            y.b = yc;
        }
    }

    return 0.5 * (x.a + x.b);

}

double bisection(Interval x, Interval y){
    return 0.5 * (x.a + x.b);
}

double secant(Interval x, Interval y){
    return (y.b * x.a - y.a * x.b)/(y.b - y.a);
}
//(y1 * x0 - y0 * x1)/delta_y;

template<typename F>
double find_root_secant_method(
    F fun, double x0, double x1,
    size_t max_iter = 100,
    double atol = std::numeric_limits<double>::epsilon(),
    double rtol = std::numeric_limits<double>::epsilon()
){

    // check inputs for trivial solutions
    double y0 = fun(x0);
    if (is_zero(y0, atol)) { return x0; }

    // guesses should be different
    if (is_close(x1, x0, atol, rtol)) {
        throw std::runtime_error(
            "Within tolerance, x0 == x1. Initial guesses should be distinct. ");
    }

    double y1 = fun(x1);
    if (is_zero(y1, atol)) { return x1; }

    double x2;
    for (size_t n = 0; n < max_iter; ++n){

        double delta_y = y1 - y0;
        if (is_zero(delta_y, atol)) {
            throw std::runtime_error("Encountered secant = zero.");
        }

        // secant formula
        x2 = (y1 * x0 - y0 * x1)/delta_y;
        double y2 = fun(x2);

        // check convergence
        if (is_zero(y2, atol)) {
            return x2;
        }

        // update
        x0 = x1;
        x1 = x2;
        y0 = y1;
        y1 = y2;
    }

    throw std::runtime_error("Secant method exceeded max iterations.");
}
