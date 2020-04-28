#ifndef MODULE_DEPENDENCY_UTILITIES
#define MODULE_DEPENDENCY_UTILITIES

#include <boost/graph/topological_sort.hpp> // for not_a_dag

#include "../validate_system.h" // for string_vector

// Throws not_a_dag:
string_vector get_evaluation_order(string_vector module_names);

bool has_cyclic_dependency(string_vector module_names);

bool order_ok(string_vector module_names);

#endif
