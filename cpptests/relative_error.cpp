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

// Returns true if both values are NaN, both are positive infinity, or both are
// negative infinity.
bool equal_nan_or_infinite(double val1, double val2) {
    return ((isnan(val1) && isnan(val2))
            ||
            (isinf(val1) && (val1 == val2)));
}

void expect_near_or_nan(double val1, double val2, double tolerance,
                        std::string message) {

    if (equal_nan_or_infinite(val1, val2)) {
        return;
    }

  EXPECT_NEAR(val1, val2, tolerance) << message;
}


void assert_near_or_nan(double val1, double val2, double tolerance,
                        std::string message) {

    if (equal_nan_or_infinite(val1, val2)) {
        return;
    }

  ASSERT_NEAR(val1, val2, tolerance) << message;
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


void expect_near_rel_or_nan(double A, double B,
                            double maxDiff, double maxRelDiff = FLT_EPSILON,
                            std::string message = "") {

    if (equal_nan_or_infinite(A, B)) {
        return;
    }

    expect_near_rel(A, B, maxDiff, maxRelDiff, message);
}


void assert_near_rel_or_nan(double A, double B,
                            double maxDiff, double maxRelDiff = FLT_EPSILON,
                            std::string message = "") {

    if (equal_nan_or_infinite(A, B)) {
        return;
    }

    assert_near_rel(A, B, maxDiff, maxRelDiff, message);
}
