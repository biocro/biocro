#include <iostream>
#include "module_factory.h"
#include "module_wrapper.h"
#include "module_library/c3_canopy.hpp"

using namespace std;

int main() {


    auto list = ModuleFactory::get_modules();

    cout << "\nAvailable modules: \n" << endl;
    
    for (auto s : list) {
        cout << s << endl;
    }

    module_wrapper_base*  w = ModuleFactory::create("c3_canopy");
    auto inputs = w->get_inputs();

    cout << "\nInputs: \n" << endl;

    std::unordered_map<std::string, double> input;
    for (auto s : inputs) {
        cout << s << endl;
        input.insert({ s, 1.0 });
    }

    cout << "\nOutputs: \n" << endl;

    auto outputs = w->get_outputs();

    std::unordered_map<std::string, double> output;
    for (auto s : outputs) {
        cout << s << endl;
        output.insert({s, 1});
    }

    auto m = unique_ptr<c3_canopy>(dynamic_cast<c3_canopy*>((w->createModule(input, output)).get()));

    inputs = m->get_inputs();

    cout << "Inputs Again!!!: " << endl;

    for (auto s : inputs) {
        cout << s << endl;
    }

    cout << "\ngot here\n" << endl;

}
