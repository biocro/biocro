module_write = function(
        module_name , module_library, module_type,
        inputs,  outputs, output_equations =NULL,
        input_units = NULL, output_units = NULL
){
    module_name_caps = toupper(module_name)
    module_library_caps = toupper(module_library)

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

    not_null = !is.null(input_units)
    inputs_not_same_length = length(inputs) != length(input_units)
    if (inputs_not_same_length && not_null){
        e <- simpleError("The arguments `inputs` and `input_units` do not have the same length.")
        stop(e)
    }

    not_null = !is.null(output_units)
    outputs_not_same_length = length(outputs) != length(output_units)
    if (outputs_not_same_length && not_null){
        e <- simpleError("The arguments `outputs` and `output_units` do not have the same length.")
        stop(e)
    }

    input_field = make_input_initializations(inputs)
    input_ptr = make_input_reference_list(inputs)
    input_get = make_get(inputs, input_units)

    output_field = make_output_initializations(outputs)
    output_ptr = make_output_pointer_list(outputs)
    output_get = make_get(outputs, output_units)



    update_template = make_update_template(outputs, output_equations)

    sprintf(template,
            module_name , module_library, module_type,
            module_name_caps, module_library_caps,
            input_field, input_ptr, input_get,
            output_field, output_ptr, output_get,
            update_template )
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

make_update_template = function(x, y=NULL){
    if (is.null(y)){
        out= paste0('update(', x, '_op, 0);',
            collapse=endl_dbl)
        return(out)
    }
    out= paste0('update(', x, '_op,', y, ');',
            collapse=endl_dbl)
    return(out)


}

tensor_string_vector = function(x, y, sep="_", order_by_left_first=FALSE){
    out = outer(x, y, FUN=paste, sep=sep)

    if (order_by_left_first){
        out = t(out)
    }

    as.vector(out)
}
