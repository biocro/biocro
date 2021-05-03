#ifndef MULTILAYER_CANOPY_PROPERTIES_H
#define MULTILAYER_CANOPY_PROPERTIES_H

#include "../state_map.h"
#include "../modules.h"

/**
 * @class multilayer_canopy_properties
 *
 * @brief Calculates properties of each canopy layer using functions found in
 * AuxBioCro (for the most part). Also includes multiple leaf classes (sunlit &
 * shaded). Leaf class is added to output parameters as a prefix, while layer
 * number is added as a suffix. Note that this module has a non-standard
 * constructor, so it cannot be created using the module_wrapper_factory.
 *
 * For compatibility with the multilayer canopy photosynthesis module, the
 * outputs of this module must be split into the following categories:
 *
 *  - multiclass_multilayer outputs: these outputs are different for each leaf
 *    class and canopy layer
 *
 *  - pure_multilayer outputs: these outputs are different for each canopy layer
 *    but not for each leaf class
 *
 *  - other outputs: these outputs do not depend on the leaf class or canopy
 *    layer (there are currently no outputs of this type)
 *
 * The base names for the multiclass_multilayer and pure_multilayer outputs will
 * be used by the multilayer canopy photosynthesis module to pass them to a leaf
 * photosynthesis module.
 */
class multilayer_canopy_properties : public SteadyModule
{
   public:
    multilayer_canopy_properties(
        const std::string& module_name,
        const int& nlayers,
        state_map const* input_parameters,
        state_map* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule(module_name),

          // Store the number of layers
          nlayers(nlayers),

          // Get references to input parameters
          par_incident_direct(get_input(input_parameters, "par_incident_direct")),
          par_incident_diffuse(get_input(input_parameters, "par_incident_diffuse")),
          absorptivity_par(get_input(input_parameters, "absorptivity_par")),
          lai(get_input(input_parameters, "lai")),
          cosine_zenith_angle(get_input(input_parameters, "cosine_zenith_angle")),
          kd(get_input(input_parameters, "kd")),
          chil(get_input(input_parameters, "chil")),
          heightf(get_input(input_parameters, "heightf")),
          rh(get_input(input_parameters, "rh")),
          windspeed(get_input(input_parameters, "windspeed")),
          LeafN(get_input(input_parameters, "LeafN")),
          kpLN(get_input(input_parameters, "kpLN")),
          lnfun(get_input(input_parameters, "lnfun")),

          // Get pointers to output parameters
          sunlit_incident_par_ops(get_multilayer_op(output_parameters, nlayers, "sunlit_incident_par")),
          sunlit_fraction_ops(get_multilayer_op(output_parameters, nlayers, "sunlit_fraction")),
          shaded_incident_par_ops(get_multilayer_op(output_parameters, nlayers, "shaded_incident_par")),
          shaded_fraction_ops(get_multilayer_op(output_parameters, nlayers, "shaded_fraction")),
          incident_scattered_par_ops(get_multilayer_op(output_parameters, nlayers, "incident_scattered_par")),
          incident_average_par_ops(get_multilayer_op(output_parameters, nlayers, "incident_average_par")),
          height_ops(get_multilayer_op(output_parameters, nlayers, "height")),
          rh_ops(get_multilayer_op(output_parameters, nlayers, "rh")),
          windspeed_ops(get_multilayer_op(output_parameters, nlayers, "windspeed")),
          LeafN_ops(get_multilayer_op(output_parameters, nlayers, "LeafN"))
    {
    }

   private:
    // Number of layers
    const int nlayers;

    // References to input parameters
    double const& par_incident_direct;
    double const& par_incident_diffuse;
    double const& absorptivity_par;
    double const& lai;
    double const& cosine_zenith_angle;
    double const& kd;
    double const& chil;
    double const& heightf;
    double const& rh;
    double const& windspeed;
    double const& LeafN;
    double const& kpLN;
    double const& lnfun;

    // Pointers to output parameters
    std::vector<double*> const sunlit_incident_par_ops;
    std::vector<double*> const sunlit_fraction_ops;
    std::vector<double*> const shaded_incident_par_ops;
    std::vector<double*> const shaded_fraction_ops;
    std::vector<double*> const incident_scattered_par_ops;
    std::vector<double*> const incident_average_par_ops;
    std::vector<double*> const height_ops;
    std::vector<double*> const rh_ops;
    std::vector<double*> const windspeed_ops;
    std::vector<double*> const LeafN_ops;

   protected:
    void run() const;
    static string_vector get_inputs(int nlayers);
    static string_vector define_leaf_classes();                   // required by derived modules for compatibility with the multilayer_canopy_photosynthesis module
    static string_vector define_multiclass_multilayer_outputs();  // required by derived modules for compatibility with the multilayer_canopy_photosynthesis module
    static string_vector define_pure_multilayer_outputs();        // required by derived modules for compatibility with the multilayer_canopy_photosynthesis module
    static string_vector get_outputs(int nlayers);
};

////////////////////////////////////////
// TEN LAYER CANOPY PROPERTIES MODULE //
////////////////////////////////////////

/**
 * @class ten_layer_canopy_properties
 *
 * @brief A child class of multilayer_canopy_properties where the number of
 * layers has been defined. Instances of this class can be created using the
 * module factory, unlike the parent class `multilayer_canopy_properties`.
 */
class ten_layer_canopy_properties : public multilayer_canopy_properties
{
   public:
    ten_layer_canopy_properties(
        state_map const* input_parameters,
        state_map* output_parameters)
        : multilayer_canopy_properties(
              "ten_layer_canopy_properties",
              ten_layer_canopy_properties::nlayers,
              input_parameters,
              output_parameters)
    {
    }
    static string_vector get_inputs();
    static string_vector define_leaf_classes();
    static string_vector define_multiclass_multilayer_outputs();
    static string_vector define_pure_multilayer_outputs();
    static string_vector get_outputs();

   private:
    // Number of layers
    int static const nlayers;

    // Main operation
    void do_operation() const;
};

#endif
