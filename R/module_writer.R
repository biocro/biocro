module.write = function(
        name = 'my_module',
        lib = 'my_library',
        type = 'direct',
        inputs = 'x', 
        outputs = 'y'
){
    name_caps = toupper(name)
    lib_caps = toupper(lib)
    
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
    
    input.field = input_declare(inputs)
    input.ptr = input_pointer_list(inputs)
    input.units = names(inputs) # might be null
    input.get = get_list(inputs, input.units)
    
    output.field = output_declare(outputs)
    output.ptr = output_pointer_list(outputs)
    output.units = names(outputs) # might be null
    output.get = get_list(outputs, output.units)


    
    update.template = update_template(outputs)
    
    sprintf(template, 
            name, lib, type, 
            name_caps, lib_caps,
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
endl = "\n\t" 
comma_endl = ",\n\t" 

input_declare = function(x){
    paste0(x, "{get_input(input_quantities, \"", x, "\")},",  
           collapse=endl)    
}

input_pointer_list = function(x) {
    paste0('double const& ', x, ';',
           collapse=endl)
}

output_declare = function(x){
    paste0(x, "_op{get_op(output_quantities, \"", x, "\")}", 
           collapse=comma_endl)    
}

output_pointer_list = function(x) {
    paste0('double* ', x, '_op;', 
           collapse=endl)    
}

get_list = function(x, units=NULL) {
    if(is.null(units)){units_string = ''}
    else{
        units_string = paste("\t\t//", units)
    }
    paste0('\"', x, '\",', units_string,
           collapse=endl)    
}

update_template = function(x){
    paste0('update(', x, '_op, 0);', 
           collapse=endl)    
}
