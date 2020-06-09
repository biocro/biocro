#include "se_solver_helper_functions.h"
#include <random>  // for random number generation
#include <limits>  // for std::numeric_limits<double>::max()
#include <cmath>   // for std::nextafter

// Helping functions for adjust_bad_guess_random
namespace se_solver_rand
{
// A random_device to be used to obtain a seed for the random number engine
std::random_device rd;

// A standard mersenne_twister_engine seeded with rd()
std::mt19937 eng(rd());

// A uniform real distribution on the closed interval [0,1]
std::uniform_real_distribution<double> dist(0, std::nextafter(1, std::numeric_limits<double>::max()));

// A function that uses these objects to return a real number between 0 and 1
double rand_01()
{
    return dist(eng);
}

}  // namespace se_solver_rand
