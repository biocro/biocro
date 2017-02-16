#ifndef RELATIVE_ERROR
#define RELATIVE_ERROR

void expect_near_rel(double A, double B,
                     double maxDiff, double maxRelDiff = FLT_EPSILON,
                     std::string message = "");



void assert_near_rel(double A, double B,
                     double maxDiff, double maxRelDiff = FLT_EPSILON,
                     std::string message = "");

void expect_near_rel_or_nan(double A, double B,
                            double maxDiff, double maxRelDiff = FLT_EPSILON,
                            std::string message = "");



void assert_near_rel_or_nan(double A, double B,
                            double maxDiff, double maxRelDiff = FLT_EPSILON,
                            std::string message = "");

void expect_near_or_nan(double val1, double val2, double tolerance,
                        std::string message);


void assert_near_or_nan(double val1, double val2, double tolerance,
                        std::string message);

#endif
