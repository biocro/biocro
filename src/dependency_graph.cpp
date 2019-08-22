// EBL is commenting out this entire file because the module class has changed significantly (2019-08-22)

/*
#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <map>
#include <string>
#include <vector>
#include <iterator>
//#include <boost/graph/graph_traits.hpp>
//#include <boost/graph/adjacency_list.hpp>

#include "modules.h"
#include "state_map.h"

//using namespace boost;
using std::string;
using std::vector;
using std::pair;
using std::cout;

std::map<string,string> get_provided_variables(std::vector<IModule*> modules)
{
    std::map<string,string> result;
    for(auto it = modules.begin(); it != modules.end(); ++it)
    {
        vector<string> provided = (*it)->list_modified_state();
        for(auto it2 = provided.begin(); it2 != provided.end(); ++it2)
        {
            result[*it2] = (*it)->list_module_name();
        }   
    }
    return result;
}

vector<pair<string,string>> get_edges(std::vector<IModule*> modules) //first module depends on the second
{
    std::map<string, string> provided = get_provided_variables(modules);
    vector<pair<string,string>> result;
    for(auto it = modules.begin(); it != modules.end(); ++it)
    {
        vector<string> required = (*it)->list_required_state();
        for(auto it2 = required.begin(); it2 != required.end(); ++it2)
        {
            string providing_module_name = provided[*it2];
            if (providing_module_name.length() != 0) {
                result.push_back(std::make_pair((*it)->get_name(), providing_module_name));
            }
        }    
    }
    return result;
}
*/