#ifndef MODULE_FACTORY_H
#define MODULE_FACTORY_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "module_wrapper.h"

class ModuleFactory {
    // This is a factory class that creates module wrapper objects.
    public:
        static std::unique_ptr<module_wrapper_base> create(std::string const &module_name);
        std::string get_description(std::string const &module_name) const;
        static std::vector<std::string> get_modules();
    private:
        // Map that links module names to wrapper constructors.
        const static std::unordered_map<std::string, module_wrapper_base*> modules;
};

#endif
