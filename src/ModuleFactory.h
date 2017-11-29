#include <memory>
#include <string>
#include "modules.h"

template<typename T> std::unique_ptr<IModule> createModule();

class ModuleFactory {
    private:
        typedef std::map<std::string, std::unique_ptr<IModule>(*)()> module_map;  // A map of strings to function pointers.
        static const module_map modules;

    public:
        std::unique_ptr<IModule> operator()(std::string const &module_name);
};

extern ModuleFactory module_factory;

