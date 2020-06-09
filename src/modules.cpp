#include "modules.h"
#include <cmath>  // for fabs

// A small number to use when checking to see if a double is different
// than 0, positive, equal to another number, etc.
const double Module::eps = 1e-10;

// This is a helping function that returns the value pointed to by a
// pointer, throwing an error if NaN occurs.  This should only be used
// for debugging purposes since it will slow down the module
// execution.
double Module::get_val_debug(const double* ptr, const std::string name) const {
    double val = *ptr;
    //if(std::isnan(val)) throw std::logic_error(std::string("Found NaN when accessing parameter '") + name + ("'.\n"));    // What is wrong with this line??
    if (fabs(val) > 10000) {
        throw std::logic_error(
            std::string("Parameter '") +
            name +
            "' has a huge value: " +
            std::to_string(val) +
            "\n"
        );
    }
    return val;
}
