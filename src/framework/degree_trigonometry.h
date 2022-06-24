#ifndef DEGREE_TRIGONOMETRY_H
#define DEGREE_TRIGONOMETRY_H

#include <cmath>           // for trig functions
#include "constants.h"  // for pi

using math_constants::pi;

double sin_deg(double angle)
{
    return sin(angle * pi / 180.0);
}

double cos_deg(double angle)
{
    return cos(angle * pi / 180.0);
}

double tan_deg(double angle)
{
    return tan(angle * pi / 180.0);
}

double asin_deg(double arg)
{
    return asin(arg) * 180.0 / pi;
}

double acos_deg(double arg)
{
    return acos(arg) * 180.0 / pi;
}

double atan2_deg(double y, double x)
{
    return atan2(y, x) * 180.0 / pi;
}

#endif
