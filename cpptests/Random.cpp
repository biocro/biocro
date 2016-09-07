#include <random>
#include "Random.h"

using namespace std;

// Adapted from: Stroustrup, Bjarne (2013-07-10). The C++ Programming Language (Kindle Locations 45279-45284). Pearson Education. Kindle Edition.

Rand_int::Rand_int(int lo, int hi)
    :r(bind( uniform_int_distribution<>(lo, hi), default_random_engine(time(nullptr)))) { }


int Rand_int::operator() () const { return r(); }


Rand_double::Rand_double( double low, double high)
    :r(bind(uniform_real_distribution<>(low, high), default_random_engine(time(nullptr)))) { }

double Rand_double::operator() () const { return r(); }

