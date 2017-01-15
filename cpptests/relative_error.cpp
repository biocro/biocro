#include <cfloat>
#include <cmath>
#include <iostream>
#include "gtest/gtest.h"

// From https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
bool AlmostEqualRelativeAndAbs(double A, double B,
                               double maxDiff, double maxRelDiff = FLT_EPSILON,
                               std::string message = "")
{
    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.
    double diff = fabs(A - B);
    if (diff <= maxDiff)
        return true;

    A = fabs(A);
    B = fabs(B);
    double largest = (B > A) ? B : A;

    if (diff <= largest * maxRelDiff)
        return true;

    std::cout << message << std::endl;
    return false;
}


void expect_near_rel(double A, double B,
                     double maxDiff, double maxRelDiff = FLT_EPSILON,
                     std::string message = "") {

            EXPECT_PRED5(AlmostEqualRelativeAndAbs, A, B, maxDiff, maxRelDiff, message);
}


void assert_near_rel(double A, double B,
                      double maxDiff, double maxRelDiff = FLT_EPSILON,
                     std::string message = "") {

    ASSERT_PRED5(AlmostEqualRelativeAndAbs, A, B, maxDiff, maxRelDiff, message);
}
