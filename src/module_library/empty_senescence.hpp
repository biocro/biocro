#ifndef EMPTY_SENESCENCE_H
#define EMPTY_SENESCENCE_H

#include "../framework/state_map.h"
#include "../framework/module.h"

class empty_senescence : public differential_module
{
   public:
    empty_senescence(state_map const& /*input_quantities*/, state_map* /*output_quantities*/)
        : differential_module()
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "empty_senescence"; }

   private:
    // Main operation
    void do_operation() const;
};

string_vector empty_senescence::get_inputs()
{
    return {
        // No inputs
    };
}

string_vector empty_senescence::get_outputs()
{
    return {
        // No outputs
    };
}

void empty_senescence::do_operation() const
{
    // Don't do anything
}

#endif
