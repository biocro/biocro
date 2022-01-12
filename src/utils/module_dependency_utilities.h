#ifndef MODULE_DEPENDENCY_UTILITIES
#define MODULE_DEPENDENCY_UTILITIES

#include <boost/graph/topological_sort.hpp> // for not_a_dag

#include "../module_creator.h"  // For mc_vector

// Throws not_a_dag:
mc_vector get_evaluation_order(mc_vector mcs);

bool has_cyclic_dependency(mc_vector mcs);

bool order_ok(mc_vector mcs);

#endif
