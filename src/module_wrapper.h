#ifndef MODULE_WRAPPER_H
#define MODULE_WRAPPER_H

#include <string>
#include <vector>
#include <memory>
#include "modules.h"

// R does not have the ability to create C++ objects directly, so we must create them via strings passed from R.
// We'd like to have a table mapping strings of names to something that will produce objects of the correct type.
// We can use a template class to wrap the Module class, so that we have access to whatever members we need.

// Since the table needs to have elements of all the same type, we need an abstract base class.
class module_wrapper_base {
 public:
    virtual std::vector<std::string> get_inputs() = 0;
    virtual std::vector<std::string> get_outputs() = 0;
    virtual std::unique_ptr<Module> createModule(const std::unordered_map<std::string, double>* input, std::unordered_map<std::string, double>* output) = 0;
    virtual std::string get_description() = 0;
    virtual ~module_wrapper_base() = 0;
};

inline module_wrapper_base::~module_wrapper_base() {}  // The destructor of the base class must always be implemented, even if it's pure virtual.

// Define a template class that wraps the behavior of Modules.
template<typename T>
class module_wrapper : public module_wrapper_base {
 public:
    std::vector<std::string> get_inputs()
        {
            return T::get_inputs();
        }

    std::vector<std::string> get_outputs()
        {
            return T::get_outputs();
        }

    std::unique_ptr<Module> createModule(const std::unordered_map<std::string, double>* input, std::unordered_map<std::string, double>* output)
        {
            return std::unique_ptr<Module>(new T(input, output));
        }

    std::string get_description() { return std::string("Descriptions no longer supported :("); }
};

#endif
