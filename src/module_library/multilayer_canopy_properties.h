#ifndef MULTILAYER_CANOPY_PROPERTIES_H
#define MULTILAYER_CANOPY_PROPERTIES_H

#include "../framework/state_map.h"
#include "../framework/module.h"

namespace standardBML
{
/**
 * @class multilayer_canopy_properties
 *
 * @brief Calculates environmental properties for sunlit and shaded leaves in
 * each layer of a multilayer canopy, mostly using functions found in
 * `AuxBioCro.cpp`.
 *
 * In general, a canopy can be divided into different layers and leaf classes,
 * which could be based on factors such as angle or age. Here we only consider
 * sunlit and shaded leaves as separate classes. These leaf classes must be
 * specified by the `define_leaf_classes()` member function.
 *
 * Throughout the canopy, some calculated properties only vary across the layers
 * (such as air temperature), while others also depend on the leaf class (such
 * as incident photosynthetically active photon flux density). The names of
 * these output quantities are formed from a base name (e.g. `incident_ppfd`), a
 * prefix that indicates the leaf class (e.g. `sunlit_`), and a suffix that
 * indicates the layer number (e.g. `_layer_0`).
 *
 * For compatibility with the `multilayer_canopy_photosynthesis` module, the
 * outputs of this module must be split into categories according to their
 * dependence on canopy layer and/or leaf class, and the base names for the
 * quantities of each type must be specified:
 *
 *  - multiclass_multilayer outputs: these outputs are different for each leaf
 *    class and canopy layer and should be specified by the
 *    `define_multiclass_multilayer_outputs()` function.
 *
 *  - pure_multilayer outputs: these outputs are different for each canopy layer
 *    but not for each leaf class and should be specified by the
 *    `define_pure_multilayer_outputs()` function.
 *
 *  - other outputs: these outputs do not depend on the leaf class or canopy
 *    layer and do not need to be specified in a special way
 *
 * The base names for the multiclass_multilayer and pure_multilayer outputs will
 * be used by the `multilayer_canopy_photosynthesis` module to correctly pass
 * these quantities to a leaf photosynthesis module that represents one leaf
 * type (e.g. sunlit leaves in layer 1).
 *
 * Note that this module has a non-standard constructor, so it cannot be created
 * using the module_factory. Rather, it is expected that directly-usable
 * classes will be derived from this class.
 */
class multilayer_canopy_properties : public direct_module
{
   public:
    multilayer_canopy_properties(
        int const& nlayers,
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Store the number of layers
          nlayers(nlayers),

          // Get references to input quantities
          par_incident_direct{get_input(input_quantities, "par_incident_direct")},
          par_incident_diffuse{get_input(input_quantities, "par_incident_diffuse")},
          lai{get_input(input_quantities, "lai")},
          cosine_zenith_angle{get_input(input_quantities, "cosine_zenith_angle")},
          k_diffuse{get_input(input_quantities, "k_diffuse")},
          chil{get_input(input_quantities, "chil")},
          heightf{get_input(input_quantities, "heightf")},
          windspeed{get_input(input_quantities, "windspeed")},
          LeafN{get_input(input_quantities, "LeafN")},
          kpLN{get_input(input_quantities, "kpLN")},
          lnfun{get_input(input_quantities, "lnfun")},
          par_energy_content{get_input(input_quantities, "par_energy_content")},
          par_energy_fraction{get_input(input_quantities, "par_energy_fraction")},
          leaf_transmittance_nir{get_input(input_quantities, "leaf_transmittance_nir")},
          leaf_transmittance_par{get_input(input_quantities, "leaf_transmittance_par")},
          leaf_reflectance_nir{get_input(input_quantities, "leaf_reflectance_nir")},
          leaf_reflectance_par{get_input(input_quantities, "leaf_reflectance_par")},

          // Get pointers to output quantities
          sunlit_fraction_ops{get_multilayer_op(output_quantities, nlayers, "sunlit_fraction")},
          sunlit_incident_nir_ops{get_multilayer_op(output_quantities, nlayers, "sunlit_incident_nir")},
          sunlit_incident_ppfd_ops{get_multilayer_op(output_quantities, nlayers, "sunlit_incident_ppfd")},
          sunlit_absorbed_ppfd_ops{get_multilayer_op(output_quantities, nlayers, "sunlit_absorbed_ppfd")},
          sunlit_absorbed_shortwave_ops{get_multilayer_op(output_quantities, nlayers, "sunlit_absorbed_shortwave")},
          shaded_fraction_ops{get_multilayer_op(output_quantities, nlayers, "shaded_fraction")},
          shaded_incident_nir_ops{get_multilayer_op(output_quantities, nlayers, "shaded_incident_nir")},
          shaded_incident_ppfd_ops{get_multilayer_op(output_quantities, nlayers, "shaded_incident_ppfd")},
          shaded_absorbed_ppfd_ops{get_multilayer_op(output_quantities, nlayers, "shaded_absorbed_ppfd")},
          shaded_absorbed_shortwave_ops{get_multilayer_op(output_quantities, nlayers, "shaded_absorbed_shortwave")},
          height_ops{get_multilayer_op(output_quantities, nlayers, "height")},
          windspeed_ops{get_multilayer_op(output_quantities, nlayers, "windspeed")},
          LeafN_ops{get_multilayer_op(output_quantities, nlayers, "LeafN")},
          canopy_direct_transmission_fraction_op{get_op(output_quantities, "canopy_direct_transmission_fraction")}
    {
    }

   private:
    // Number of layers
    int const nlayers;

    // References to input parameters
    double const& par_incident_direct;
    double const& par_incident_diffuse;
    double const& lai;
    double const& cosine_zenith_angle;
    double const& k_diffuse;
    double const& chil;
    double const& heightf;
    double const& windspeed;
    double const& LeafN;
    double const& kpLN;
    double const& lnfun;
    double const& par_energy_content;
    double const& par_energy_fraction;
    double const& leaf_transmittance_nir;
    double const& leaf_transmittance_par;
    double const& leaf_reflectance_nir;
    double const& leaf_reflectance_par;

    // Pointers to output parameters
    std::vector<double*> const sunlit_fraction_ops;
    std::vector<double*> const sunlit_incident_nir_ops;
    std::vector<double*> const sunlit_incident_ppfd_ops;
    std::vector<double*> const sunlit_absorbed_ppfd_ops;
    std::vector<double*> const sunlit_absorbed_shortwave_ops;
    std::vector<double*> const shaded_fraction_ops;
    std::vector<double*> const shaded_incident_nir_ops;
    std::vector<double*> const shaded_incident_ppfd_ops;
    std::vector<double*> const shaded_absorbed_ppfd_ops;
    std::vector<double*> const shaded_absorbed_shortwave_ops;
    std::vector<double*> const height_ops;
    std::vector<double*> const windspeed_ops;
    std::vector<double*> const LeafN_ops;
    double* canopy_direct_transmission_fraction_op;

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
        state_map const& input_quantities,
        state_map* output_quantities)
        : multilayer_canopy_properties(
              ten_layer_canopy_properties::nlayers,
              input_quantities,
              output_quantities)
    {
    }
    static string_vector get_inputs();
    static string_vector define_leaf_classes();
    static string_vector define_multiclass_multilayer_outputs();
    static string_vector define_pure_multilayer_outputs();
    static string_vector get_outputs();
    static std::string get_name() { return "ten_layer_canopy_properties"; }

   private:
    // Number of layers
    int static const nlayers;

    // Main operation
    void do_operation() const;
};

}  // namespace standardBML
#endif
