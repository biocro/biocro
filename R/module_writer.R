module.write = function(
        module.name , module.library, module.type,
        inputs,  outputs,
        input.units = NULL, output.units = NULL
){
    module.name.caps = toupper(module.name)
    module.library.caps = toupper(module.library)

    # check for duplicate inputs
    if(anyDuplicated(inputs)){
        e <- simpleError("Duplicate inputs detected.")
        stop(e)
    }

    # check for duplicate outputs
    if(anyDuplicated(outputs)){
        e <- simpleError("Duplicate outputs detected.")
        stop(e)
    }

    input.field = make_input_initializations(inputs)
    input.ptr = make_input_reference_list(inputs)
    input.get = make_get(inputs, input.units)

    output.field = make_output_initializations(outputs)
    output.ptr = make_output_pointer_list(outputs)
    output.get = make_get(outputs, output.units)



    update.template = make_update_template(outputs)

    sprintf(template,
            module.name , module.library, module.type,
            module.name.caps, module.library.caps,
            input.field, input.ptr, input.get,
            output.field, output.ptr, output.get,
            update.template )
}

template = "
#ifndef %5$s_%4$s_H
#define %5$s_%4$s_H

#include \"../framework/module.h\"
#include \"../framework/state_map.h\"

namespace %2$s
{
    /**
    * @class %1$s
    *
    * @brief Put Documentation Here.
    *
    */
    class %1$s : public %3$s_module
    {
        public:
            %1$s(
                state_map const& input_quantities,
                state_map* output_quantities)
        : %3$s_module(),

            // Get references to input quantities
            %6$s

            // Get pointers to output quantities
            %9$s
        {
        }
        static string_vector get_inputs();
        static string_vector get_outputs();
        static std::string get_name() { return \"%1$s\"; }

        private:
            // References to input quantities
            %7$s

            // Pointers to output quantities
            %10$s

        // Main operation
        void do_operation() const;
    };

    string_vector %1$s::get_inputs()
    {
        return {
            %8$s
        };
    }

    string_vector %1$s::get_outputs()
    {
        return {
            %11$s
        };
    }

    void %1$s::do_operation() const
    {
        //use update to set outputs as function of inputs
        %12$s
    }

}  // namespace %2$s
#endif
"

# newline and indent
indent = "    "
endl_dbl = paste0("\n",indent,indent)
endl_tpl = paste0("\n",indent,indent,indent)
comma_tpl = paste0(",",endl_tpl)


make_input_initializations = function(x){
    paste0(x, "{get_input(input_quantities, \"", x, "\")},",
           collapse=endl_tpl)
}

make_input_reference_list = function(x) {
    paste0('double const& ', x, ';',
           collapse=endl_tpl)
}

make_output_initializations = function(x){
    paste0(x, "_op{get_op(output_quantities, \"", x, "\")}",
           collapse=comma_tpl)
}

make_output_pointer_list = function(x) {
    paste0('double* ', x, '_op;',
           collapse=endl_tpl)
}

make_get = function(x, units=NULL) {
    if(is.null(units)){units_string = ''}
    else{
        units_string = paste(indent, indent, "//", units)
    }
    paste0('\"', x, '\",', units_string,
           collapse=endl_tpl)
}

make_update_template = function(x){
    paste0('update(', x, '_op, 0);',
           collapse=endl_dbl)
}
