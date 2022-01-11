#ifndef MODULE_DEPENDENCY_UTILITIES
#define MODULE_DEPENDENCY_UTILITIES

#include <boost/graph/topological_sort.hpp> // for not_a_dag

#include "../module_wrapper.h"  // For mwp_vector

// Throws not_a_dag:
mwp_vector get_evaluation_order(mwp_vector mwps);

bool has_cyclic_dependency(mwp_vector mwps);

bool order_ok(mwp_vector mwps);

#endif
