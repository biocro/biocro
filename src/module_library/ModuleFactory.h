#include <memory>
#include <string>
#include "../modules.h"
#include "../convergence_iteration.hpp"

template<typename T> std::unique_ptr<IModule> createModule() { return std::unique_ptr<IModule>(new T); }

class ModuleFactory {
    private:
        typedef std::map<std::string, std::unique_ptr<IModule>(*)()> module_map;  // A map of strings to function pointers.
        static module_map modules;

    public:
        std::unique_ptr<IModule> operator()(std::string const &module_name) const;
        template<class ModuleType>
            void register_module();
};

extern ModuleFactory module_factory;

template<class ModuleType>
void ModuleFactory::register_module() {
    ModuleType module;
    this->modules[module.get_name().c_str()] = &createModule<ModuleType>;
}

