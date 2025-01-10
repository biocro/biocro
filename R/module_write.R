module_write <- function(
    module_name,
    module_library,
    module_type,
    inputs,
    outputs,
    output_equations = NULL,
    input_units = NULL,
    output_units = NULL
)
{
    module_name_caps    <- toupper(module_name)
    module_library_caps <- toupper(module_library)

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

    # check input unit length
    not_null <- !is.null(input_units)
    inputs_not_same_length <- length(inputs) != length(input_units)
    if (inputs_not_same_length && not_null){
        e <- simpleError("The arguments `inputs` and `input_units` do not have the same length.")
        stop(e)
    }

    # check output unit length
    not_null <- !is.null(output_units)
    outputs_not_same_length <- length(outputs) != length(output_units)
    if (outputs_not_same_length && not_null){
        e <- simpleError("The arguments `outputs` and `output_units` do not have the same length.")
        stop(e)
    }

    # check for spaces
    if (any(grepl(' ', inputs))) {
        e <- simpleError("The values of `inputs` cannot have any spaces in them.")
        stop(e)
    }

    if (any(grepl(' ', outputs))) {
        e <- simpleError("The values of `outputs` cannot have any spaces in them.")
        stop(e)
    }

    # check for starting numbers
    if (any(grepl('^[[:digit:]]', inputs))) {
        e <- simpleError("The values of `inputs` cannot start with a number.")
        stop(e)
    }

    if (any(grepl('^[[:digit:]]', outputs))) {
        e <- simpleError("The values of `outputs` cannot start with a number.")
        stop(e)
    }

    # get customized parts of C++ code
    input_field <- make_input_initializations(inputs)
    input_ptr   <- make_input_reference_list(inputs)
    input_get   <- make_get(inputs, input_units)

    output_field <- make_output_initializations(outputs)
    output_ptr   <- make_output_pointer_list(outputs)
    output_get   <- make_get(outputs, output_units)

    update_template <- make_update_template(outputs, output_equations)

    # fill in header template (defined below)
    sprintf(
        module_header_template,
        module_name,         # %1
        module_library,      # %2
        module_type,         # %3
        module_name_caps,    # %4
        module_library_caps, # %5
        input_field,         # %6
        input_ptr,           # %7
        input_get,           # %8
        output_field,        # %9
        output_ptr,          # %10
        output_get,          # %11
        update_template      # %12
    )
}

module_header_template <- "#ifndef %5$s_%4$s_H
#define %5$s_%4$s_H

#include \"../framework/module.h\"
#include \"../framework/state_map.h\"

namespace %2$s
{
/**
 * @class %1$s
 *
 * @brief Put documentation here.
 *
 */
class %1$s : public %3$s_module
{
   public:
    %1$s(
        state_map const& input_quantities,
        state_map* output_quantities)
        : %3$s_module{},

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
    // Make calculations here

    // Use `update` to set outputs
    %12$s
}

}  // namespace %2$s
#endif
"

# newline and indent
indent <- "    "

endl_initializer_list <- paste0("\n  ", indent, indent)
endl_class_members    <- paste0("\n", indent)
endl_get              <- paste0("\n", indent, indent)
endl_do_operation     <- paste0("\n", indent)

# helping functions for generating C++ code
make_input_initializations <- function(x){
    paste0(x, "{get_input(input_quantities, \"", x, "\")},",
        collapse = endl_initializer_list)
}

make_output_initializations <- function(x){
    paste0(x, "_op{get_op(output_quantities, \"", x, "\")}",
        collapse = paste0(',', endl_initializer_list))
}

make_input_reference_list <- function(x) {
    paste0('double const& ', x, ';', collapse = endl_class_members)
}

make_output_pointer_list <- function(x) {
    paste0('double* ', x, '_op;', collapse = endl_class_members)
}

make_get <- function(x, units = NULL) {
    qnames_string <- paste0('\"', x, '\"')

    comma_string <- rep_len(',', length(x))
    comma_string[length(x)] <- ' '

    units_string <- if (is.null(units)) {
        paste(indent, indent, '// Put', x, 'units here')
    } else {
        paste(indent, indent, "//", units)
    }

    paste0(qnames_string, comma_string, units_string, collapse = endl_get)
}

make_update_template <- function(x, y = NULL){
    if (is.null(y)){
        paste0('update(', x, '_op, 0);', collapse = endl_do_operation)
    } else {
        paste0('update(', x, '_op,', y, ');', collapse = endl_do_operation)
    }
}
