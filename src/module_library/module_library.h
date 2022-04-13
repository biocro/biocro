#ifndef STANDARDBML_H
#define STANDARDBML_H

#include "../framework/module_creator.h"  // for module_creator and creator_map

// When creating a new module library R package, it will be necessary to modify
// the header guard and the namespace name in this file to reflect the new
// package name. For example, if the new package is to be called `myLib`, the
// header guard should become `MYLIB_H` and the namespace should become `mylib`.
// The doxygen-style comment describing the namespace should also be updated to
// reflect the purpose of the new library.

/**
 *  @brief This is the standard BioCro module library; it includes the essential
 *  modules used in typical BioCro crop growth simulations.
 */
namespace standardBML
{
class module_library
{
   public:
    static creator_map library_entries;
};

}  // namespace standardBML

#endif
