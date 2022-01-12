#ifndef MODULE_LIBRARY_H
#define MODULE_LIBRARY_H

#include <string>
#include <map>
#include <unordered_map>
#include "../module_creator.h"

class module_library
{
   public:
    static module_creator* retrieve(std::string const& module_name);
    static string_vector get_all_modules();
    static std::unordered_map<std::string, string_vector> get_all_quantities();

   private:
    using creator_fcn = module_creator* (*)();
    using creator_map = std::map<std::string, creator_fcn>;
    static creator_map library_entries;
};

#endif
