#include <map>
#include <stdexcept>
#include <iostream>
#include <vector>
#include "../modules.h"
#include "module_library/module_graph_test.hpp"
#include "../state_map.h"

using std::string;
using std::vector;
using std::pair;
using std::map;
using std::cout;

// Advance declarations for functions in dependency_graph.cpp
vector<pair<string, string>> get_edges(vector<IModule*>);

int main(int argc, char** argv)
{
    try {
    vector<IModule*> test_modules;
    IModule *A = new Module_1;
    IModule *B = new Module_2;
    IModule *C = new Module_3;

    test_modules.push_back(A);
    test_modules.push_back(B);
    test_modules.push_back(C);

    vector<pair<string,string>> edges = get_edges(test_modules);

    for(auto it=edges.begin(); it!=edges.end(); ++it)
    {
    cout << it->first << " => " << it->second << '\n';
    }

    } catch (...) {
        cout << "Caught exception: \n";
    }
    return 0;
}


