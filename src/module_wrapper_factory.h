#ifndef MODULE_WRAPPER_FACTORY_H
#define MODULE_WRAPPER_FACTORY_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "module_wrapper.h"

class module_wrapper_factory {
    // This is a factory class that creates module wrapper objects.
    public:
        static std::unique_ptr<module_wrapper_base> create(std::string const &module_name);
        static std::string get_description(std::string const &module_name);
        static std::vector<std::string> get_modules();
        static std::unordered_map<std::string, std::vector<std::string>> get_all_quantities();
    private:
        // Map that links module names to wrapper constructors.
        typedef std::unique_ptr<module_wrapper_base>(*f_ptr)();
        static std::unordered_map<std::string, f_ptr> modules;
};

#endif
