#ifndef DEGREE_TRIGONOMETRY_H
#define DEGREE_TRIGONOMETRY_H

#include <cmath>           // for trig functions
#include "constants.h"  // for pi

using math_constants::pi;

template <typename T>
double sin_deg(T angle)
{
    return sin(angle * pi / 180.0);
}

template <typename T>
double cos_deg(T angle)
{
    return cos(angle * pi / 180.0);
}

template <typename T>
double tan_deg(T angle)
{
    return tan(angle * pi / 180.0);
}

template <typename T>
double asin_deg(T arg)
{
    return asin(arg) * 180.0 / pi;
}

template <typename T>
double acos_deg(T arg)
{
    return acos(arg) * 180.0 / pi;
}

template <typename T>
double atan2_deg(T y, T x)
{
    return atan2(y, x) * 180.0 / pi;
}

#endif
