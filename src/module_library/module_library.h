#ifndef BIOCRO_MODULE_LIBRARY_H
#define BIOCRO_MODULE_LIBRARY_H

#include "../framework/module_factory.h"  // for creator_map

// When creating a new module library R package, it will be necessary to modify
// the header guard and the class name in this file to reflect the new package
// name. For example, if the new package is to be called `myLib`, the header
// guard should become `MYLIB_MODULE_LIBRARY_H` and the class name should become
// `mylib_module_library`.

class biocro_module_library
{
   public:
    static creator_map library_entries;
};

#endif
