#ifndef BIOCRO_MODULE_INTERFACE_MACROS_H
#define BIOCRO_MODULE_INTERFACE_MACROS_H

/*
 * Generic X-macro helpers for BioCro modules.
 *
 * Given a module named "my_module", here is example usage.
 *
 *   #define MY_MODULE_INPUTS(X) \
 *       X(foo, "foo")           \
 *       X(bar, "bar")
 *
 *   #define MY_MODULE_OUTPUTS(X) \
 *       X(baz_op, "baz")
 *
 *   DEFINE_MODULE_CLASS(my_module, direct_module,
 *                        MY_MODULE_INPUTS, MY_MODULE_OUTPUTS)
 *   #undef MY_MODULE_INPUTS
 *   #undef MY_MODULE_OUTPUTS
 *
 *   The names "MY_MODULE_INPUTS" and "MY_MODULE_OUTPUTS" are arbitrary here,
 *   but following that convention improves consistency.
 *
 *   In each row of the INPUTS and OUTPUTS lists, the first item is the variable
 *   name in the C++ code, and the second is the key in state_map.
 *
 *   After calling DEFINE_MODULE_CLASS, use `#undef` to undefine the INPUTS and
 *   OUTPUTS macros.
 */

/* Generic macros that expand for each entry in *_INPUTS and *_OUTPUTS */
#define DECLARE_INPUT_MEMBER(member, name) \
    const double& member;

#define DECLARE_OUTPUT_MEMBER(member, name) \
    double* member;

#define BIND_INPUT(member, name) \
    , member(get_input(input_quantities, name))

#define BIND_OUTPUT(member, name) \
    , member(get_op(output_quantities, name))

#define EMIT_INPUT_NAME(member, name) \
    name,

#define EMIT_OUTPUT_NAME(member, name) \
    name,

/*
 * Define a complete Module-derived class.
 *
 * Arguments:
 *   CLASS_NAME   The C++ class name
 *   BASE_CLASS   The Module base class: either direct_module or differential_module
 *   INPUTS       An X-macro list like MY_MODULE_INPUTS(X)
 *   OUTPUTS      An X-macro list like MY_MODULE_OUTPUTS(X)
 *
 * This defines a class that includes the following parts:
 *   - a constructor that binds inputs and outputs
 *   - static get_inputs() and get_outputs()
 *   - static get_name()
 *   - a declaration of do_operation() const
 *   - input reference members and output pointer members
 *
 * The author must define do_operation() const.
 */
#define DEFINE_MODULE_CLASS(CLASS_NAME, BASE_CLASS, INPUTS, OUTPUTS)  \
class CLASS_NAME : public BASE_CLASS                                  \
{                                                                     \
public:                                                               \
    CLASS_NAME(state_map const& input_quantities,                     \
               state_map* output_quantities)                          \
        : BASE_CLASS()                                                \
          INPUTS(BIND_INPUT)                                          \
          OUTPUTS(BIND_OUTPUT)                                        \
    {}                                                                \
                                                                      \
    static string_vector get_inputs()                                 \
    {                                                                 \
        return {                                                      \
            INPUTS(EMIT_INPUT_NAME)                                   \
        };                                                            \
    }                                                                 \
                                                                      \
    static string_vector get_outputs()                                \
    {                                                                 \
        return {                                                      \
            OUTPUTS(EMIT_OUTPUT_NAME)                                 \
        };                                                            \
    }                                                                 \
                                                                      \
    static std::string get_name()                                     \
    {                                                                 \
        return #CLASS_NAME;                                           \
    }                                                                 \
                                                                      \
    void do_operation() const override;                               \
                                                                      \
private:                                                              \
    INPUTS(DECLARE_INPUT_MEMBER)                                      \
    OUTPUTS(DECLARE_OUTPUT_MEMBER)                                    \
};

/* Here is an abbreviated example for c3_canopy.
namespace standardBML
{

#define C3_CANOPY_INPUTS(X) \
    X(absorbed_longwave, "absorbed_longwave")       // J / m^2 / s \
    X(ambient_temperature, "ambient_temperature")   // Pa \
    ...
    X(last_input, "last_input")

#define C3_CANOPY_OUTPUTS(X) \
    X(canopy_assimilation_molar_flux_op, "canopy_assimilation_molar_flux")  // micromol / m^2 / s \
    X(canopy_conductance_op, "canopy_conductance")                          // mol / m^2 / s \
    ...
    X(last_output_op, "last_output")

// Note that in the lists above, there is no continuation backslash after the last item.

DEFINE_MODULE_CLASS(c3_canopy, direct_module,
                     C3_CANOPY_INPUTS, C3_CANOPY_OUTPUTS)

#undef C3_CANOPY_INPUTS
#undef C3_CANOPY_OUTPUTS

// The do_operation() definition may go in the class header file or in a separate .cpp file.
void c3_canopy::do_operation() const
{
    ... Model calculations

    update(canopy_assimilation_molar_flux_op, can_result.Assim);                      // micromol / m^2 / s
    ... the rest of the update calls.

}

}  // namespace standardBML
*/
#endif
