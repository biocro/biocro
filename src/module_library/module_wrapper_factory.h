#ifndef MODULE_WRAPPER_FACTORY_H
#define MODULE_WRAPPER_FACTORY_H

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "../module_wrapper.h"

class module_wrapper_factory
{
   public:
    static std::unique_ptr<module_wrapper_base> create(std::string const& module_name);
    static std::string get_description(std::string const& module_name);
    static std::vector<std::string> get_modules();
    static std::unordered_map<std::string, std::vector<std::string>> get_all_quantities();

   private:
    using module_wrapper_creator = std::unique_ptr<module_wrapper_base> (*)();
    using module_wrapper_creator_map = std::map<std::string, module_wrapper_creator>;
    static module_wrapper_creator_map module_wrapper_creators;
};

#endif
