#ifndef ED_MULTILAYER_CANOPY_PROPERTIES_H
#define ED_MULTILAYER_CANOPY_PROPERTIES_H

#include "../framework/module.h"
#include "../framework/state_map.h"

/**
 * @class ed_multilayer_canopy_properties
 *
 * @brief Calculates properties of each canopy layer using functions
 * found in AuxBioCro (for the most part). Also includes multiple
 * leaf classes (sunlit & shaded). Leaf class is added to output
 * parameters as a prefix, while layer number is added as a suffix.
 * Note that this module has a non-standard constructor, so it cannot
 * be created using the module_factory.
 *
 * For compatibility with the multilayer canopy photosynthesis module,
 * the outputs of this module must be split into the following categories:
 *  - multiclass_multilayer outputs: these outputs are different for each
 *    leaf class and canopy layer
 *  - pure_multilayer outputs: these outputs are different for each canopy
 *    layer but not for each leaf class
 *  - other outputs: these outputs do not depend on the leaf class or
 *    canopy layer (there are currently no outputs of this type)
 * The base names for the multiclass_multilayer and pure_multilayer outputs
 * will be used by the multilayer canopy photosynthesis module to pass them
 * to a leaf photosynthesis module.
 */
namespace standardBML
{
class ed_multilayer_canopy_properties : public direct_module
{
   public:
    ed_multilayer_canopy_properties(
        int const& nlayers,
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Store the number of layers
          nlayers(nlayers),

          // Get references to input quantities
          par_energy_fraction(get_input(input_quantities, "par_energy_fraction")),
          par_energy_content(get_input(input_quantities, "par_energy_content")),
          par_incident_direct(get_input(input_quantities, "par_incident_direct")),
          par_incident_diffuse(get_input(input_quantities, "par_incident_diffuse")),
          absorptivity_par(get_input(input_quantities, "absorptivity_par")),
          lai(get_input(input_quantities, "lai")),
          cosine_zenith_angle(get_input(input_quantities, "cosine_zenith_angle")),
          kd(get_input(input_quantities, "kd")),
          chil(get_input(input_quantities, "chil")),
          heightf(get_input(input_quantities, "heightf")),
          rh(get_input(input_quantities, "rh")),
          windspeed(get_input(input_quantities, "windspeed")),
          LeafN(get_input(input_quantities, "LeafN")),
          kpLN(get_input(input_quantities, "kpLN")),
          leaf_transmittance(get_input(input_quantities, "leaf_transmittance")),
          leaf_reflectance(get_input(input_quantities, "leaf_reflectance")),

          // Get pointers to output quantities
          sunlit_incident_par_ops(get_multilayer_op(output_quantities, nlayers, "sunlit_incident_par")),
          sunlit_collatz_PAR_flux_ops(get_multilayer_op(output_quantities, nlayers, "sunlit_collatz_PAR_flux")),
          sunlit_fraction_ops(get_multilayer_op(output_quantities, nlayers, "sunlit_fraction")),
          shaded_incident_par_ops(get_multilayer_op(output_quantities, nlayers, "shaded_incident_par")),
          shaded_collatz_PAR_flux_ops(get_multilayer_op(output_quantities, nlayers, "shaded_collatz_PAR_flux")),
          shaded_fraction_ops(get_multilayer_op(output_quantities, nlayers, "shaded_fraction")),
          incident_scattered_par_ops(get_multilayer_op(output_quantities, nlayers, "incident_scattered_par")),
          incident_average_par_ops(get_multilayer_op(output_quantities, nlayers, "incident_average_par")),
          solar_energy_absorbed_leaf_ops(get_multilayer_op(output_quantities, nlayers, "solar_energy_absorbed_leaf")),
          height_ops(get_multilayer_op(output_quantities, nlayers, "height")),
          rh_ops(get_multilayer_op(output_quantities, nlayers, "rh")),
          windspeed_ops(get_multilayer_op(output_quantities, nlayers, "windspeed")),
          LeafN_ops(get_multilayer_op(output_quantities, nlayers, "LeafN"))
    {
    }

   private:
    // Number of layers
    int const nlayers;

    // References to input quantities
    double const& par_energy_fraction;
    double const& par_energy_content;
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
    double const& leaf_transmittance;
    double const& leaf_reflectance;

    // Pointers to output quantities
    std::vector<double*> const sunlit_incident_par_ops;
    std::vector<double*> const sunlit_collatz_PAR_flux_ops;
    std::vector<double*> const sunlit_fraction_ops;
    std::vector<double*> const shaded_incident_par_ops;
    std::vector<double*> const shaded_collatz_PAR_flux_ops;
    std::vector<double*> const shaded_fraction_ops;
    std::vector<double*> const incident_scattered_par_ops;
    std::vector<double*> const incident_average_par_ops;
    std::vector<double*> const solar_energy_absorbed_leaf_ops;
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
string_vector ed_multilayer_canopy_properties::get_inputs(int /*nlayers*/)
{
    return {
        "par_energy_fraction",   // dimensionless
        "par_energy_content",    // J / micromol
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
        "leaf_transmittance",    // dimensionless
        "leaf_reflectance"       // dimensionless
    };
}

/**
 * @brief Define the different leaf classes included by the module
 */
string_vector ed_multilayer_canopy_properties::define_leaf_classes()
{
    return {
        "sunlit",  // these leaves receive diffuse, scattered, and direct solar radiation
        "shaded"   // these leaves receive diffuse and scattered solar radiation
    };
}

/**
 * @brief Define all outputs that have different values for each leaf class and layer
 */
string_vector ed_multilayer_canopy_properties::define_multiclass_multilayer_outputs()
{
    return {
        "incident_par",      // J / (m^2 leaf) / s
        "collatz_PAR_flux",  // J / (m^2 leaf) / s
        "fraction"           // dimensionless
    };
}

/**
 * @brief Define all outputs that have different values for each layer, not including
 * outputs that also depend on leaf class
 */
string_vector ed_multilayer_canopy_properties::define_pure_multilayer_outputs()
{
    return {
        "incident_scattered_par",      // J / (m^2 leaf) / s
        "incident_average_par",        // J / (m^2 leaf) / s
        "solar_energy_absorbed_leaf",  // J / (m^2 leaf) / s
        "height",                      // m
        "rh",                          // dimensionless from Pa / Pa
        "windspeed",                   // m / s
        "LeafN",                       // mmol / m^2 (?)
    };
}

/**
 * @brief Define all output names by appending leaf class prefixes and layer number suffixes.
 * Here we should make use of the `define_XXX` functions to reduce code duplication.
 */
string_vector ed_multilayer_canopy_properties::get_outputs(int nlayers)
{
    // Add leaf_class prefixes to the multiclass_multilayer outputs
    string_vector multilayer_outputs = generate_multiclass_quantity_names(
        ed_multilayer_canopy_properties::define_leaf_classes(),
        ed_multilayer_canopy_properties::define_multiclass_multilayer_outputs());

    // Add other multilayer outputs
    for (std::string const& name : ed_multilayer_canopy_properties::define_pure_multilayer_outputs()) {
        multilayer_outputs.push_back(name);
    }

    // Add layer number suffixes to all multilayer outputs
    // We can return the result immediately since there are no other outputs
    return generate_multilayer_quantity_names(nlayers, multilayer_outputs);
}

void ed_multilayer_canopy_properties::run() const
{
    // Calculate PAR levels throughout the canopy. Note that sunML specifies that its
    // `Idir` and `Idiff` arguments should have [micromol / m^2 / s] units. However,
    // we can leave these quantities as energy flux densities with [J / m^2 / s] units
    // because the two units are simply related by a multiplicative conversion factor
    // and the output of sunML is linear with respect to `Idir` and `Idiff`.
    struct Light_profile par_profile = sunML(
        par_incident_direct, par_incident_diffuse, lai, nlayers,
        cosine_zenith_angle, kd, chil, absorptivity_par, heightf,
        par_energy_content, par_energy_fraction, leaf_transmittance,
        leaf_reflectance);

    // Calculate relative humidity levels throughout the canopy
    double relative_humidity_profile[nlayers];
    RHprof(rh, nlayers, relative_humidity_profile);  // Modifies relative_humidity_profile.

    // Calculate windspeed throughout the canopy
    double wind_speed_profile[nlayers];
    WINDprof(windspeed, lai, nlayers, wind_speed_profile);  // Modifies wind_speed_profile.

    // Calculate leaf nitrogen throughout the canopy
    double leafN_profile[nlayers];
    LNprof(LeafN, lai, nlayers, kpLN, leafN_profile);  // Modifies leafN_profile.

    // Update layer-dependent outputs
    //
    // par_profile.sunlit_incident_ppfd and par_profile.shaded_incident_ppfd represent
    // incident PAR energy flux densities in units of J / m^2 / s. However, the
    // Collatz model expects a quantum flux in units of mol / m^2 / s. So we use
    // the `par_energy_content` quantity to convert from J to micromol, and then
    // an additional factor of 1e6 micromol / mol is required.
    //
    // par_profile.average_incident_ppfd represents some kind of wonky "average" incident
    // PAR flux on the leaves, but the evapotranspiration code expects a value for
    // total absorbed solar energy (including NIR). To convert, we use the method from
    // the Evapotrans2 code. First we use the `par_energy_fraction` quantity
    // to determine the total amount of solar energy incident on the leaf (including NIR).
    // Then we determine the amount of light absorbed by the leaf using the following factor:
    // (1 - LeafReflectance - tau) / (1 - tau)
    double const absorb_factor = (1 - leaf_reflectance - leaf_transmittance) / (1 - leaf_transmittance);
    for (int i = 0; i < nlayers; ++i) {
        update(sunlit_fraction_ops[i], par_profile.sunlit_fraction[i]);
        update(shaded_fraction_ops[i], par_profile.shaded_fraction[i]);
        update(height_ops[i], par_profile.height[i]);
        update(sunlit_incident_par_ops[i], par_profile.sunlit_incident_ppfd[i]);
        update(sunlit_collatz_PAR_flux_ops[i], par_profile.sunlit_incident_ppfd[i] / par_energy_content * 1e-6);
        update(incident_scattered_par_ops[i], par_profile.incident_ppfd_scattered[i]);
        update(shaded_incident_par_ops[i], par_profile.shaded_incident_ppfd[i]);
        update(shaded_collatz_PAR_flux_ops[i], par_profile.shaded_incident_ppfd[i] / par_energy_content * 1e-6);
        update(incident_average_par_ops[i], par_profile.average_incident_ppfd[i]);
        update(solar_energy_absorbed_leaf_ops[i], par_profile.average_incident_ppfd[i] / par_energy_fraction * absorb_factor);
        update(rh_ops[i], relative_humidity_profile[i]);
        update(windspeed_ops[i], wind_speed_profile[i]);
        update(LeafN_ops[i], leafN_profile[i]);
    }
}

////////////////////////////////////////
// TEN LAYER CANOPY PROPERTIES MODULE //
////////////////////////////////////////

/**
 * @class ed_ten_layer_canopy_properties
 *
 * @brief A child class of ed_multilayer_canopy_properties where the number of layers has been defined.
 * Instances of this class can be created using the module factory.
 */
class ed_ten_layer_canopy_properties : public ed_multilayer_canopy_properties
{
   public:
    ed_ten_layer_canopy_properties(
        state_map const& input_quantities,
        state_map* output_quantities)
        : ed_multilayer_canopy_properties(
              ed_ten_layer_canopy_properties::nlayers,
              input_quantities,
              output_quantities)  // Create the base class with the appropriate number of layers
    {
    }
    static string_vector get_inputs();
    static string_vector define_leaf_classes();
    static string_vector define_multiclass_multilayer_outputs();
    static string_vector define_pure_multilayer_outputs();
    static string_vector get_outputs();
    static std::string get_name() { return "ed_ten_layer_canopy_properties"; }

   private:
    // Number of layers
    static const int nlayers;
    // Main operation
    void do_operation() const;
};

const int ed_ten_layer_canopy_properties::nlayers = 10;  // Set the number of layers

string_vector ed_ten_layer_canopy_properties::get_inputs()
{
    return ed_multilayer_canopy_properties::get_inputs(ed_ten_layer_canopy_properties::nlayers);
}

string_vector ed_ten_layer_canopy_properties::define_leaf_classes()
{
    return ed_multilayer_canopy_properties::define_leaf_classes();
}

string_vector ed_ten_layer_canopy_properties::define_multiclass_multilayer_outputs()
{
    // Just call the parent class's multilayer output function
    return ed_multilayer_canopy_properties::define_multiclass_multilayer_outputs();
}

string_vector ed_ten_layer_canopy_properties::define_pure_multilayer_outputs()
{
    // Just call the parent class's multilayer output function
    return ed_multilayer_canopy_properties::define_pure_multilayer_outputs();
}

string_vector ed_ten_layer_canopy_properties::get_outputs()
{
    return ed_multilayer_canopy_properties::get_outputs(ed_ten_layer_canopy_properties::nlayers);
}

void ed_ten_layer_canopy_properties::do_operation() const
{
    ed_multilayer_canopy_properties::run();
}

}  // namespace standardBML
#endif
