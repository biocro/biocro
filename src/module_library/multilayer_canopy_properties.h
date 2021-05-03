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

/**
 * @brief Define all inputs required by the module
 */
string_vector multilayer_canopy_properties::get_inputs(int /*nlayers*/)
{
    return {
        "par_incident_direct",   // J / (m^2 beam) / s [area perpendicular to beam]
        "par_incident_diffuse",  // J / m^2 / s        [through any plane]
        "absorptivity_par",      // dimensionless
        "lai",                   // dimensionless from (m^2 leaf) / (m^2 ground). LAI of entire canopy.
        "cosine_zenith_angle",   // dimensionless
        "kd",                    // (m^2 ground) / (m^2 leaf)
        "chil",                  // dimensionless from m^2 / m^2
        "heightf",               // m^-1 from (m^2 / m^2) / m.  Leaf area density; LAI per height of canopy.
        "rh",                    // dimensionless from Pa / Pa
        "windspeed",             // m / s
        "LeafN",                 // mmol / m^2 (?)
        "kpLN",                  // dimensionless
        "lnfun",                 // a dimensionless switch
    };
}

/**
 * @brief Define the different leaf classes included by the module
 */
string_vector multilayer_canopy_properties::define_leaf_classes()
{
    return {
        "sunlit",  // these leaves receive diffuse, scattered, and direct solar radiation
        "shaded"   // these leaves receive diffuse and scattered solar radiation
    };
}

/**
 * @brief Define all outputs that have different values for each leaf class and
 * layer
 */
string_vector multilayer_canopy_properties::define_multiclass_multilayer_outputs()
{
    return {
        "incident_par",  // J / (m^2 leaf) / s
        "fraction"       // dimensionless
    };
}

/**
 * @brief Define all outputs that have different values for each layer, not
 * including outputs that also depend on leaf class
 */
string_vector multilayer_canopy_properties::define_pure_multilayer_outputs()
{
    return {
        "incident_scattered_par",  // J / (m^2 leaf) / s
        "incident_average_par",    // J / (m^2 leaf) / s
        "height",                  // m
        "rh",                      // dimensionless from Pa / Pa
        "windspeed",               // m / s
        "LeafN",                   // mmol / m^2 (?)
    };
}

/**
 * @brief Define all output names by appending leaf class prefixes and layer
 * number suffixes. Here we should make use of the `define_XXX` functions to
 * reduce code duplication.
 */
string_vector multilayer_canopy_properties::get_outputs(int nlayers)
{
    // Add leaf_class prefixes to the multiclass_multilayer outputs
    string_vector multilayer_outputs = generate_multiclass_quantity_names(
        multilayer_canopy_properties::define_leaf_classes(),
        multilayer_canopy_properties::define_multiclass_multilayer_outputs());

    // Add other multilayer outputs
    for (std::string const& name : multilayer_canopy_properties::define_pure_multilayer_outputs()) {
        multilayer_outputs.push_back(name);
    }

    // Add layer number suffixes to all multilayer outputs
    // We can return the result immediately since there are no other outputs
    return generate_multilayer_quantity_names(nlayers, multilayer_outputs);
}

void multilayer_canopy_properties::run() const
{
    // Calculate PAR levels throughout the canopy. Note that sunML specifies
    // that its `Idir` and `Idiff` arguments should have [micromol / m^2 / s]
    // units. However, we can leave these quantities as energy flux densities
    // with [J / m^2 / s] units because the two units are simply related by a
    // multiplicative conversion factor and the output of sunML is linear with
    // respect to `Idir` and `Idiff`.
    struct Light_profile par_profile = sunML(
        par_incident_direct,
        par_incident_diffuse,
        lai,
        nlayers,
        cosine_zenith_angle,
        kd,
        chil,
        absorptivity_par,
        heightf);

    // Calculate relative humidity levels throughout the canopy
    double relative_humidity_profile[nlayers];
    RHprof(rh, nlayers, relative_humidity_profile);  // Modifies relative_humidity_profile.

    // Calculate windspeed throughout the canopy
    double wind_speed_profile[nlayers];
    WINDprof(windspeed, lai, nlayers, wind_speed_profile);  // Modifies wind_speed_profile.

    // Calculate leaf nitrogen throughout the canopy
    double leafN_profile[nlayers];
    LNprof(LeafN, lai, nlayers, kpLN, leafN_profile);  // Modifies leafN_profile.

    // Don't calculate anything based on the nitrogen profile
    if (lnfun != 0) {
        throw std::logic_error("Thrown by the multilayer_canopy_properties module: lnfun != 0 is not yet supported.");
    }

    // Update layer-dependent outputs
    for (int i = 0; i < nlayers; ++i) {
        update(sunlit_fraction_ops[i], par_profile.sunlit_fraction[i]);
        update(shaded_fraction_ops[i], par_profile.shaded_fraction[i]);
        update(height_ops[i], par_profile.height[i]);
        update(sunlit_incident_par_ops[i], par_profile.direct_irradiance[i]);
        update(incident_scattered_par_ops[i], par_profile.scattered_irradiance[i]);
        update(shaded_incident_par_ops[i], par_profile.diffuse_irradiance[i]);
        update(incident_average_par_ops[i], par_profile.total_irradiance[i]);
        update(rh_ops[i], relative_humidity_profile[i]);
        update(windspeed_ops[i], wind_speed_profile[i]);
        update(LeafN_ops[i], leafN_profile[i]);
    }
}

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

int const ten_layer_canopy_properties::nlayers = 10;

string_vector ten_layer_canopy_properties::get_inputs()
{
    return multilayer_canopy_properties::get_inputs(ten_layer_canopy_properties::nlayers);
}

string_vector ten_layer_canopy_properties::define_leaf_classes()
{
    return multilayer_canopy_properties::define_leaf_classes();
}

string_vector ten_layer_canopy_properties::define_multiclass_multilayer_outputs()
{
    // Just call the parent class's multilayer output function
    return multilayer_canopy_properties::define_multiclass_multilayer_outputs();
}

string_vector ten_layer_canopy_properties::define_pure_multilayer_outputs()
{
    // Just call the parent class's multilayer output function
    return multilayer_canopy_properties::define_pure_multilayer_outputs();
}

string_vector ten_layer_canopy_properties::get_outputs()
{
    return multilayer_canopy_properties::get_outputs(ten_layer_canopy_properties::nlayers);
}

void ten_layer_canopy_properties::do_operation() const
{
    multilayer_canopy_properties::run();
}

#endif
