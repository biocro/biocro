#ifndef MODULE_FACTORY_H
#define MODULE_FACTORY_H

#include <string>
#include <unordered_map>
#include <vector>
#include "modules.h"
#include "module_wrapper.h"

class ModuleFactory {
	// This is a factory class that creates module objects and returns info about them
 public:
    static module_wrapper_base* create(std::string const &module_name);
    std::string get_description(std::string const &module_name) const;					// Returns a module's description
    static std::vector<std::string> get_modules();										// Returns a list of all available modules
 private:
    // Maps for linking module names to their constructors and descriptions
    const static std::unordered_map<std::string, module_wrapper_base*> modules;
};

#endif
