#ifndef QUADRATIC_ROOT_H
#define QUADRATIC_ROOT_H

double quadratic_root(double a, double b, double c, unsigned int root_type);

/**
 * @brief A wrapper for `quadratic_root()` with `root_type` set to 1.
 */
inline double quadratic_root_plus(double a, double b, double c)
{
    return quadratic_root(a, b, c, 1);
}

/**
 * @brief A wrapper for `quadratic_root()` with `root_type` set to 2.
 */
inline double quadratic_root_minus(double a, double b, double c)
{
    return quadratic_root(a, b, c, 2);
}

/**
 * @brief A wrapper for `quadratic_root()` with `root_type` set to 3.
 */
inline double quadratic_root_max(double a, double b, double c)
{
    return quadratic_root(a, b, c, 3);
}

/**
 * @brief A wrapper for `quadratic_root()` with `root_type` set to 3.
 */
inline double quadratic_root_min(double a, double b, double c)
{
    return quadratic_root(a, b, c, 4);
}

#endif
