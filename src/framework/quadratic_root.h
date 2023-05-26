#ifndef QUADRATIC_ROOT_H
#define QUADRATIC_ROOT_H

enum quadratic_root_type { plus,
                           minus,
                           larger,
                           smaller };

double quadratic_root(
    double a,
    double b,
    double c,
    quadratic_root_type root_type);

/**
 * @brief A wrapper for `quadratic_root()` that returns the "plus" root.
 */
inline double quadratic_root_plus(double a, double b, double c)
{
    return quadratic_root(a, b, c, plus);
}

/**
 * @brief A wrapper for `quadratic_root()` that returns the "minus" root.
 */
inline double quadratic_root_minus(double a, double b, double c)
{
    return quadratic_root(a, b, c, minus);
}

/**
 * @brief A wrapper for `quadratic_root()` that returns the larger root.
 */
inline double quadratic_root_max(double a, double b, double c)
{
    return quadratic_root(a, b, c, larger);
}

/**
 * @brief A wrapper for `quadratic_root()` that returns the smaller root.
 */
inline double quadratic_root_min(double a, double b, double c)
{
    return quadratic_root(a, b, c, smaller);
}

#endif
