
default_out = "\n#ifndef MY_LIBRARY_MY_MODULE_H\n#define MY_LIBRARY_MY_MODULE_H\n    \n#include \"../framework/module.h\"\n#include \"../framework/state_map.h\"\n\nnamespace my_library\n{\n    /**\n    * @class my_module\n    *\n    * @brief Put Documentation Here.\n    *\n    */\n    class my_module : public direct_module\n    {\n        public:\n            my_module(\n                state_map const& input_quantities,\n                state_map* output_quantities)\n        : direct_module(),\n        \n        // Get references to input quantities \n        x{get_input(input_quantities, \"x\")},\n        \n        // Get pointers to output quantities \n        y_op{get_op(output_quantities, \"y\")}\n        {\n        }\n        static string_vector get_inputs();\n        static string_vector get_outputs();\n        static std::string get_name() { return \"my_module\"; }\n        \n        private:\n        // References to input quantities \n        double const& x;\n    \n        // Pointers to output quantities \n        double* y_op;\n        \n        // Main operation\n        void do_operation() const;\n    };\n    \n    string_vector my_module::get_inputs()\n    {\n        return {\n        \"x\",\n        };\n    }\n    \n    string_vector my_module::get_outputs()\n    {\n        return {\n        \"y\",\n        };\n    }\n    \n    void my_module::do_operation() const\n    {\n        //use update to set outputs as function of inputs\n        update(y_op, 0);\n    }\n    \n}  // namespace my_library\n#endif\n"

test_that("Default output hasn't changed", {
    expect_true(BioCro:::module.write() == default_out)
})

test_that("Duplicate input variables raise error",
    {
        expect_error(BioCro:::module.write(inputs = c('x', 'x')))
    }
)

test_that("Duplicate output variables raise error",
    {
        expect_error(BioCro:::module.write(output = c('y', 'y')))
    }
)