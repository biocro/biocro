#include <vector>
#include "multilayer_canopy_properties.h"
#include "BioCro.h"     // for WINDprof
#include "AuxBioCro.h"  // for LNprof
#include "sunML.h"      // for sunML

using std::vector;
using standardBML::multilayer_canopy_properties;
using standardBML::ten_layer_canopy_properties;

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
        "windspeed",             // m / s
        "LeafN",                 // mmol / m^2 (?)
        "kpLN",                  // dimensionless
        "lnfun",                 // a dimensionless switch
        "par_energy_content",    // J / micromol
        "par_energy_fraction",   // dimensionless
        "leaf_transmittance",    // dimensionless
        "leaf_reflectance"       // dimensionless
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
        "incident_ppfd",       // micromol / (m^2 leaf) / s
        "absorbed_ppfd",       // micromol / (m^2 leaf) / s
        "absorbed_shortwave",  // J / (m^2 leaf) / s
        "fraction"             // dimensionless
    };
}

/**
 * @brief Define all outputs that have different values for each layer, not
 * including outputs that also depend on leaf class
 */
string_vector multilayer_canopy_properties::define_pure_multilayer_outputs()
{
    return {
        "incident_ppfd_scattered",     // micromol / (m^2 leaf) / s
        "average_incident_ppfd",       // J / (m^2 leaf) / s
        "average_absorbed_shortwave",  // J / (m^2 leaf) / s
        "height",                      // m
        "windspeed",                   // m / s
        "LeafN",                       // mmol / m^2 (?)
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
    multilayer_outputs = generate_multilayer_quantity_names(nlayers, multilayer_outputs);

    // Include outputs that do not depend on leaf class or number of layers.
    multilayer_outputs.push_back("canopy_direct_transmission_fraction");

    // Return the full list
    return multilayer_outputs;
}

void multilayer_canopy_properties::run() const
{
    // Calculate values of incident photosynthetically active photon flux
    // density (PPFD) and absorbed shortwave energy throughout the canopy. Note
    // that the `sunML` function expects input expects PPFD values, so we must
    // convert photosynthetically active radiation (PAR) to PPFD using the
    // energy content of light in the PAR band
    struct Light_profile light_profile = sunML(
        par_incident_direct / par_energy_content,   // micromol / (m^2 beam) / s
        par_incident_diffuse / par_energy_content,  // micromol / m^2 / s
        lai,
        nlayers,
        cosine_zenith_angle,
        kd,
        chil,
        absorptivity_par,
        heightf,
        par_energy_content,
        par_energy_fraction,
        leaf_transmittance,
        leaf_reflectance);

    // Calculate windspeed throughout the canopy
    vector<double> wind_speed_profile(nlayers);
    WINDprof(windspeed, lai, wind_speed_profile);  // Modifies wind_speed_profile

    // Calculate leaf nitrogen throughout the canopy
    vector<double> leafN_profile(nlayers);
    LNprof(LeafN, lai, kpLN, leafN_profile);  // Modifies leafN_profile

    // Don't calculate anything based on the nitrogen profile
    if (lnfun != 0) {
        throw std::logic_error("Thrown by the multilayer_canopy_properties module: lnfun != 0 is not yet supported.");
    }

    // Update layer-dependent outputs
    for (int i = 0; i < nlayers; ++i) {
        update(sunlit_fraction_ops[i], light_profile.sunlit_fraction[i]);
        update(sunlit_incident_ppfd_ops[i], light_profile.sunlit_incident_ppfd[i]);
        update(sunlit_absorbed_ppfd_ops[i], light_profile.sunlit_absorbed_ppfd[i]);
        update(sunlit_absorbed_shortwave_ops[i], light_profile.sunlit_absorbed_shortwave[i]);

        update(shaded_fraction_ops[i], light_profile.shaded_fraction[i]);
        update(shaded_incident_ppfd_ops[i], light_profile.shaded_incident_ppfd[i]);
        update(shaded_absorbed_ppfd_ops[i], light_profile.shaded_absorbed_ppfd[i]);
        update(shaded_absorbed_shortwave_ops[i], light_profile.shaded_absorbed_shortwave[i]);

        update(average_incident_ppfd_ops[i], light_profile.average_incident_ppfd[i]);
        update(average_absorbed_shortwave_ops[i], light_profile.average_absorbed_shortwave[i]);

        update(incident_ppfd_scattered_ops[i], light_profile.incident_ppfd_scattered[i]);
        update(height_ops[i], light_profile.height[i]);
        update(windspeed_ops[i], wind_speed_profile[i]);
        update(LeafN_ops[i], leafN_profile[i]);
    }

    // Update other outputs
    update(canopy_direct_transmission_fraction_op, light_profile.canopy_direct_transmission_fraction);
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
