#include "module_dependency_utilities.h"

#include <boost/config.hpp> // put this first to suppress some VC++ warnings
#include <boost/graph/adjacency_list.hpp> // for adjacency_list
#include <boost/graph/topological_sort.hpp> // for dfs_visitor and not_a_dag

#include "../state_map.h" // for string_vector


// using declarations

using std::string;
using std::front_inserter;
using std::back_inserter;
using std::tie;
using boost::vertex_name;
using boost::vertex_index;


// type aliases

/**
 *  @brief Graph is the type of a directed Boost graph without
 *  multi-edges implemented as an adjacency list and having vertex
 *  name and vertex index properties. The "name" of a vertex here
 *  is actually a pointer to a module wrapper object.
 *
 *  For more information, see
 *  https://www.boost.org/doc/libs/1_71_0/libs/graph/doc/using_adjacency_list.html
 */
using Graph = boost::adjacency_list<
    boost::setS,      // the EdgeList type: don't allow multiedges
    boost::listS,     // the VertexList type; could use vecS instead
    boost::directedS, // cheaper than bidirectionalS, which provides features we don't need
    boost::property<boost::vertex_name_t, module_creator*,
                    boost::property<boost::vertex_index_t, std::size_t>>
    >;

/**
 *  @brief module_name_map_t is the type for the map mapping Graph
 *  vertex descriptors to names.
 *
 *  In our case, these will be pointers to module wrapper objects.
 */
using module_name_map_t = boost::property_map<Graph, boost::vertex_name_t>::type;

/**
 *  @brief index_map_t is the type for the map mapping Graph vertex
 *  descriptors to the vertex index.
 *
 *  (We wouldn't need this if we used vecS instead of listS for the
 *  VertexList adjacency_list template paramater.  But this allows us
 *  to use either.)
 */
using index_map_t = boost::property_map<Graph, boost::vertex_index_t>::type;

/**
 *  @brief Vertex is the type of the vertex used by Graph.
 */
using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

/**
 *  @brief vertex_list is a std::list containing Vertex objects.
 *
 *  Used for storing the module evaluation order.
 */
using vertex_list = std::list<Vertex>;

/**
 * @brief vertex_iterator is the type of an iterator that may be used
 * to iterate through any container of vertices from a graph of type
 * Graph.
 */
using vertex_iterator = boost::graph_traits<Graph>::vertex_iterator;


// Functions
//
// Only `mc_vector get_evaluation_order(mc_vector)` is
// declared in the header file.  The rest of the functions are
// auxiliary functions that it uses either directly or indirectly and
// that are not used outside of this file.

/**
 *  @brief Returns a list (presented as a sorted vector of strings) of
 *  all inputs for the module.
 *
 *  @param[in] w A pointer to a `module_creator` object.
 *
 *  @returns A list (presented as a sorted vector of strings) of all inputs for
 *           the module.
 */
string_vector get_module_inputs(module_creator* w) {
    string_vector inputs { w->get_inputs() };
    sort(inputs.begin(), inputs.end());
    return inputs;
}

/**
 *  @brief Returns a list (presented as a sorted vector of strings) of
 *  all outputs for the module.
 *
 *  @param[in] w A pointer to a `module_creator` object.
 *
 *  @returns A list (presented as a sorted vector of strings) of all
 *           outputs for the module.
 */
string_vector get_module_outputs(module_creator* w) {
    string_vector outputs { w->get_outputs() };
    sort(outputs.begin(), outputs.end());
    return outputs;
}


/**
 *  @brief Determines if the module m1 depends on the module
 *  m2, returning true if it does and false if not.
 *
 *  m1 depends on m2 if and only if some input of m1 corresponds to
 *  some output of m2.
 *
 *  @param[in] m1 A pointer to a `module_creator` object.
 *  @param[in] m2 A pointer to a `module_creator` object.
 *
 *  @returns true if the module named by m1 depends upon the module
 *           named by m2, false otherwise.
 *
 */
bool depends_on(module_creator* m1, module_creator* m2) {
    string_vector m1_inputs = get_module_inputs(m1);
    string_vector m2_outputs = get_module_outputs(m2);

    string_vector result;

    // set_intersection relies on both ranges being sorted, which they are.
    set_intersection(m1_inputs.begin(), m1_inputs.end(),
                     m2_outputs.begin(), m2_outputs.end(),
                     back_inserter(result));

    // If m1's inputs overlap with m2's outputs (i.e., their intersection is
    // non-empty), m1 depends on m2:
    return result.size() != 0;
}

/**
 *  @brief Creates and returns a graph that models the dependency
 *  structure of the modules listed in mcs.
 *
 *  There is a directed edge from the vertex for module A to the
 *  vertex for module B if A is depended upon by B, that is, if B
 *  depends on A.
 *
 *  @param mcs A list (presented as a vector of pointers to
 *                     `module_creator` objects) of modules.
 *
 *  @returns A Graph object modeling the dependency structure of the modules.
 */
Graph get_dependency_graph(mc_vector mcs) {

    int N = mcs.size();

    Graph g(N);

    module_name_map_t name_map = get(vertex_name, g);

    index_map_t index_map = get(vertex_index, g);


    vertex_iterator vi;
    vertex_iterator vi_end;
    string_vector::size_type i = 0;
    for (tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi, ++i) {
        name_map[*vi] = mcs[i];

        // If the adjacency_list VertexList parameter is vecS, then
        // index_map will be read-only; but it will be set properly so
        // that the body of this if statement is not executed.  On the
        // other hand, if it is listS, then we must set it manually:
        if (index_map[*vi] != i) {
            index_map[*vi] = i;
        }
    }

    vertex_iterator vi2;
    vertex_iterator vi2_end;
    for (tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi) {
        for (tie(vi2, vi2_end) = vertices(g); vi2 != vi2_end; ++vi2) {
            if (depends_on(name_map[*vi], name_map[*vi2])) {
                add_edge(*vi2, *vi, g);
            }
        }
    }

    return g;
}

/**
 *  @brief A class suitable to use as a visitor for detecting a cycle
 *  in a directed graph.
 *
 *  The code here was copied and adapted from the example at
 *  https://www.boost.org/doc/libs/1_72_0/libs/graph/example/file_dependencies.cpp.
 */
class cycle_detector : public boost::dfs_visitor<>
{
   public:
    cycle_detector(bool& has_cycle)
        : has_cycle(has_cycle) {}

    template <typename Edge, typename Graph>
    void back_edge(Edge, Graph&) const {
        has_cycle = true;
    }

   private:
    bool& has_cycle;
};

/**
 *  @brief Given a directed graph g, return true if g has a cycle,
 *  false otherwise.
 *
 *  @param g A graph object of type Graph.
 *
 *  @returns true if directed graph g has a cycle, false if it is
 *           acyclic.
 */
bool has_cycle(Graph g) {
    bool has_cycle = false;

    cycle_detector vis(has_cycle);
    depth_first_search(g, visitor(vis));

    return has_cycle;
}

/**
 *  @brief Given a list of modules (presented as a vector of module
 *  wrapper pointers), determine if the list can be put in a suitable
 *  order for evaluating the modules.
 *
 *  @param mcs A list (presented as a vector of module wrapper pointers) of
 *                      modules.
 *
 *  @returns `true` if there is a cyclic dependency among the given
 *           modules so that they cannot be suitably ordered; `false`
 *           otherwise.
 *
 */
bool has_cyclic_dependency(mc_vector mcs) {
    Graph g = get_dependency_graph(mcs);
    return has_cycle(g);
}

/**
 *  @brief Determines whether the modules in
 *  `mcs` need to be reordered before evaluation.
 *
 *  @param module_names A list (presented as a vector of strings) of
 *                      names of direct modules.
 *
 *  @returns `true` if the list of modules as given in `module_names`
 *           is a suitable evaluation order, `false` otherwise.
 *
 */
bool order_ok(mc_vector mcs) {
    for (mc_vector::size_type i = 0; i < mcs.size(); ++i) {
        for (mc_vector::size_type j = i; j < mcs.size(); ++j) {
            if (depends_on(mcs[i], mcs[j])) {
                return false;
            }
        }
    }

    return true;
}


/**
 *  @brief Given a directed acyclic graph g, return a ordered list of
 *  the vertices (presented as a vertex_list object) such that for
 *  each edge of the graph, the source vertex of the edge occurs in
 *  the list before the target vertex of the edge.
 *
 *  If g is not actually a directed acyclic graph, the function throws
 *  an exception of type
 *  boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::not_a_dag>>.
 *
 *  @param g A Graph object corresponding to a directed acyclic graph.
 *
 *  @return A list of the vertices of g (presented as a list of Vertex
 *          objects) ordered so the module associated with each vertex
 *          in the list depends only on modules associated with
 *          vertices occuring earlier in the list.
 *
 *  @throws boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::not_a_dag>>
 *              if g is not acyclic.
 *
 *  @note g is passed by (const) reference not merely for efficiency,
 *        but so that the returned vertex list will actually be a list
 *        of vertices of g, not a list of vertices of a copy of g.
 *        This matters when we try to get the names of the vertices in
 *        the list.
 */
vertex_list get_topological_ordering(const Graph& g) {
    vertex_list topological_order;
    topological_sort(g, front_inserter(topological_order));
    return topological_order;
}

/**
 *  @brief Given a list of modules (presented as a vector of pointers to
 *  `module_creator` objects), return the same list in a suitable order for
 *  evaluating the modules.
 *
 *  If no such ordering exists (due to a cyclic dependency), the
 *  function throws an exception of type
 *  boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::not_a_dag>>.
 *
 *  @param mcs A list (presented as a vector of module wrapper pointers) of
 *                      modules.
 *
 *  @return The same list given in `mcs`, but ordered so that
 *          each module in the list depends only on modules occuring
 *          earlier in the list.
 *
 *  @note Even if the input list `mcs` is already in a
 *        suitable evaluation order, the output list may nevertheless
 *        give the modules in a different order.
 *
 *  @throws boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::not_a_dag>>
 *              Thrown if there is no suitable evaluation order due to
 *              a cyclic dependency in the set of modules specified by
 *              `mcs`.
 */
mc_vector get_evaluation_order(mc_vector mcs) {
    Graph g = get_dependency_graph(mcs);

    vertex_list evaluation_order = get_topological_ordering(g);

    module_name_map_t name_map = get(vertex_name, g);

    mc_vector ordered_module_list;

    for (vertex_list::iterator i = evaluation_order.begin();
         i != evaluation_order.end();
         ++i)
    {
        ordered_module_list.push_back(name_map[*i]);
    }

    return ordered_module_list;
}
