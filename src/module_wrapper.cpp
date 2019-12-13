#include "module_factory.h"

// Rather than the ModuleFactory wrapping the class and storing the parameters for the class, it returns the wrapper, and one interacts with that, without storing parameters.


/*
ModuleFactory module_factory;
module_wrapper_base * w = module_factory("c3_canopy");
auto the_inputs = w.get_inputs();
Module m = w.createModule(inputs, outputs);

// Instead of
ModuleFactory module_factory(inputs, outputs);
auto the_inputs = module_factory.get_inputs("c3_canopy");
Module m = module_factory.create("c3_canopy");
*/
