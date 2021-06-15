#include "multilayer_canopy_properties.h"
#include "BioCro.h"     // for sunML, RHprof, WINDprof
#include "AuxBioCro.h"  // for LNprof

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
        update(sunlit_incident_par_ops[i], par_profile.incident_ppfd_direct[i]);
        update(incident_scattered_par_ops[i], par_profile.incident_ppfd_scattered[i]);
        update(shaded_incident_par_ops[i], par_profile.incident_ppfd_diffuse[i]);
        update(incident_average_par_ops[i], par_profile.incident_ppfd_average[i]);
        update(rh_ops[i], relative_humidity_profile[i]);
        update(windspeed_ops[i], wind_speed_profile[i]);
        update(LeafN_ops[i], leafN_profile[i]);
    }
}

////////////////////////////////////////
// TEN LAYER CANOPY PROPERTIES MODULE //
////////////////////////////////////////

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
