#include <math.h>
#include "BioCro.h"

// These are functions to calculate leaf N limitations.
// New functions can be written, and they must meet the following criteria:
// a) The argument list must be (double kLn, double leaf_n_0, struct Model_state current_state), even if they arguments are not used.
// b) It must return the current leaf N as type double.
//
// These are called from the Gro functions, and kLn, leaf_n_0 and current state are passed from the Gro functions.

double thermal_leaf_nitrogen_limitation(double kLn, double leaf_n_0, const struct Model_state &current_state)
{
	return(leaf_n_0 * exp(-kLn * current_state.thermal_time));
}

double biomass_leaf_nitrogen_limitation(double kLn, double leaf_n_0, const struct Model_state &current_state)
{
	double leaf_n = 0;
	leaf_n = leaf_n_0 * pow(current_state.leaf + current_state.stem, -kLn);
	return(leaf_n > leaf_n_0 ? leaf_n_0 : leaf_n);
}

