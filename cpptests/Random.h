#ifndef TEST_UTILITY_RANDOM
#define TEST_UTILITY_RANDOM

#include <random>

using namespace std;

class Rand_int {
public:
    Rand_int(int lo, int hi);
    int operator() () const;
private:
    function<int()> r;
    static int seed_offset;
};

class Rand_double {
public:
    Rand_double( double low, double high);
    double operator() () const;
private:
    function<double()> r;
    static int seed_offset;
};

#endif
