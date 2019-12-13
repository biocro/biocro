#include <iostream>
#include "module_factory.h"
#include "module_wrapper.h"

using namespace std;

int main() {

    
    auto list = ModuleFactory::get_modules();

    for (auto s : list) {
        cout << s << endl;
    }

    module_wrapper_base*  w = ModuleFactory::create("c3_canopy");
    auto inputs = w->get_inputs();

    cout << "Inputs: " << endl;
    
    for (auto s : inputs) {
        cout << s << endl;
    }

    cout << "Outputs: " << endl;

    auto outputs = w->get_outputs();

    for (auto s : outputs) {
        cout << s << endl;
    }

    //auto m = w->createModule(inputs, outputs);

    
}
