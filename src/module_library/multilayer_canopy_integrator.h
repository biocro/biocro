#ifndef MULTILAYER_CANOPY_INTEGRATOR_H
#define MULTILAYER_CANOPY_INTEGRATOR_H

#include "../framework/state_map.h"
#include "../framework/module.h"
#include "../framework/constants.h"  // for molar_mass_of_water, molar_mass_of_glucose

namespace standardBML
{
/**
 * @class multilayer_canopy_integrator
 *
 * @brief Calculates canopy-level values for assimilation and other quantities
 * by adding the individual values from the sunlit and shaded leaves in each
 * canopy layer, weighted by the relative fractions of sunlit and shaded leaves
 * in each layer.
 *
 * For more information about how multilayer modules work in BioCro, see the
 * documentation for the `multilayer_canopy_properties` and
 * `multilayer_canopy_photosynthesis` modules.
 */
class multilayer_canopy_integrator : public direct_module
{
   public:
    multilayer_canopy_integrator(
        int const& nlayers,
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Store the number of layers
          nlayers(nlayers),

          // Get pointers to input quantities
          sunlit_fraction_ips{get_multilayer_ip(input_quantities, nlayers, "sunlit_fraction")},
          sunlit_Assim_ips{get_multilayer_ip(input_quantities, nlayers, "sunlit_Assim")},
          sunlit_GrossAssim_ips{get_multilayer_ip(input_quantities, nlayers, "sunlit_GrossAssim")},
          sunlit_Gs_ips{get_multilayer_ip(input_quantities, nlayers, "sunlit_Gs")},
          sunlit_Rp_ips{get_multilayer_ip(input_quantities, nlayers, "sunlit_Rp")},
          sunlit_TransR_ips{get_multilayer_ip(input_quantities, nlayers, "sunlit_TransR")},
          shaded_fraction_ips{get_multilayer_ip(input_quantities, nlayers, "shaded_fraction")},
          shaded_Assim_ips{get_multilayer_ip(input_quantities, nlayers, "shaded_Assim")},
          shaded_GrossAssim_ips{get_multilayer_ip(input_quantities, nlayers, "shaded_GrossAssim")},
          shaded_Gs_ips{get_multilayer_ip(input_quantities, nlayers, "shaded_Gs")},
          shaded_Rp_ips{get_multilayer_ip(input_quantities, nlayers, "shaded_Rp")},
          shaded_TransR_ips{get_multilayer_ip(input_quantities, nlayers, "shaded_TransR")},

          // Get references to input quantities
          lai{get_input(input_quantities, "lai")},
          growth_respiration_fraction{get_input(input_quantities, "growth_respiration_fraction")},

          // Get pointers to output quantities
          canopy_assimilation_rate_op{get_op(output_quantities, "canopy_assimilation_rate")},
          canopy_transpiration_rate_op{get_op(output_quantities, "canopy_transpiration_rate")},
          canopy_conductance_op{get_op(output_quantities, "canopy_conductance")},
          GrossAssim_op{get_op(output_quantities, "GrossAssim")},
          canopy_photorespiration_rate_op{get_op(output_quantities, "canopy_photorespiration_rate")}
    {
    }

   private:
    // Number of layers
    int const nlayers;

    // Pointers to input quantities
    std::vector<double const*> const sunlit_fraction_ips;
    std::vector<double const*> const sunlit_Assim_ips;
    std::vector<double const*> const sunlit_GrossAssim_ips;
    std::vector<double const*> const sunlit_Gs_ips;
    std::vector<double const*> const sunlit_Rp_ips;
    std::vector<double const*> const sunlit_TransR_ips;
    std::vector<double const*> const shaded_fraction_ips;
    std::vector<double const*> const shaded_Assim_ips;
    std::vector<double const*> const shaded_GrossAssim_ips;
    std::vector<double const*> const shaded_Gs_ips;
    std::vector<double const*> const shaded_Rp_ips;
    std::vector<double const*> const shaded_TransR_ips;

    // References to input quantities
    double const& lai;
    double const& growth_respiration_fraction;

    // Pointers to output quantities
    double* canopy_assimilation_rate_op;
    double* canopy_transpiration_rate_op;
    double* canopy_conductance_op;
    double* GrossAssim_op;
    double* canopy_photorespiration_rate_op;

    // Main operation
    virtual void do_operation() const;

   protected:
    void run() const;

   public:
    static string_vector get_inputs(int nlayers);
    static string_vector get_outputs(int nlayers);
};

/**
 * @brief Define all inputs required by the module, adding layer suffixes as
 * required
 */
string_vector multilayer_canopy_integrator::get_inputs(int nlayers)
{
    // Define the multilayer inputs
    string_vector multilayer_inputs = {
        "sunlit_fraction",    // dimensionless
        "sunlit_Assim",       // micromol / m^2 /s
        "sunlit_GrossAssim",  // micromol / m^2 /s
        "sunlit_Gs",          // mmol / m^2 / s
        "sunlit_Rp",          // micromol / m^2 /s
        "sunlit_TransR",      // mmol / m^2 / s
        "shaded_fraction",    // dimensionless
        "shaded_Assim",       // micromol / m^2 /s
        "shaded_GrossAssim",  // micromol / m^2 /s
        "shaded_Gs",          // mmol / m^2 / s
        "shaded_Rp",          // micromol / m^2 /s
        "shaded_TransR",      // mmol / m^2 / s
    };

    // Get the full list by appending layer numbers
    string_vector all_inputs = generate_multilayer_quantity_names(nlayers, multilayer_inputs);

    // Add any other inputs
    all_inputs.push_back("lai");                          // dimensionless from m^2 / m^2
    all_inputs.push_back("growth_respiration_fraction");  // dimensionless

    return all_inputs;
}

/**
 * @brief Define all outputs produced by the module
 */
string_vector multilayer_canopy_integrator::get_outputs(int /*nlayers*/)
{
    return {
        "canopy_assimilation_rate",     // Mg / ha / hr
        "canopy_transpiration_rate",    // Mg / ha / hr
        "canopy_conductance",           // mmol / m^2 / s
        "GrossAssim",                   // Mg / ha / hr
        "canopy_photorespiration_rate"  // Mg / ha / hr
    };
}

void multilayer_canopy_integrator::do_operation() const
{
    multilayer_canopy_integrator::run();
}

void multilayer_canopy_integrator::run() const
{
    double const LAIc = lai / nlayers;
    double canopy_assimilation_rate{0.0};
    double canopy_transpiration_rate{0.0};
    double canopy_conductance{0.0};
    double GrossAssim{0.0};
    double canopy_photorespiration_rate{0.0};

    // Integrate assimilation, transpiration, and conductance throughout the
    // canopy
    for (int i = 0; i < nlayers; ++i) {
        double const sunlit_lai = *sunlit_fraction_ips[i] * LAIc;
        double const shaded_lai = *shaded_fraction_ips[i] * LAIc;

        canopy_assimilation_rate += *sunlit_Assim_ips[i] * sunlit_lai +
                                    *shaded_Assim_ips[i] * shaded_lai;

        canopy_transpiration_rate += *sunlit_TransR_ips[i] * sunlit_lai +
                                     *shaded_TransR_ips[i] * shaded_lai;

        canopy_conductance += *sunlit_Gs_ips[i] * sunlit_lai +
                              *shaded_Gs_ips[i] * shaded_lai;

        GrossAssim += *sunlit_GrossAssim_ips[i] * sunlit_lai +
                      *shaded_GrossAssim_ips[i] * shaded_lai;

        canopy_photorespiration_rate += *sunlit_Rp_ips[i] * sunlit_lai +
                                        *shaded_Rp_ips[i] * shaded_lai;
    }

    // Modify net assimilation to account for respiration
    // Note: this was originally only done for the C3 canopy
    // Note: it seems like this should not be necessary since the assimilation
    //   model includes respiration
    canopy_assimilation_rate *= (1.0 - growth_respiration_fraction);

    // For assimilation, we need to convert micromol / m^2 / s into
    // Mg / ha / hr, assuming that all carbon is converted into biomass in the
    // form of glucose (C6H12O6), i.e., six assimilated CO2 molecules contribute
    // one glucose molecule. Using the molar mass of glucose in kg / mol, the
    // conversion can be accomplished with the following factor:
    // (1 glucose / 6 CO2) * (3600 s / hr) * (1e-6 mol / micromol) *
    //     (1e-3 Mg / kg) * (1e4 m^2 / ha)
    // = 6e-3 s * mol * Mg * m^2 / (hr * micromol * kg * ha)
    const double cf = physical_constants::molar_mass_of_glucose * 6e-3;  // (Mg / ha / hr) / (micromol / m^2 / s)

    // For transpiration, we need to convert mmol / m^2 / s into Mg / ha / hr
    // using the molar mass of water in kg / mol, which can be accomplished by
    // the following conversion factor:
    // (3600 s / hr) * (1e-3 mol / mmol) * (1e-3 Mg / kg) * (1e4 m^2 / ha)
    // = 36 s * mol * Mg * m^2 / (hr * mmol * kg * ha)
    const double cf2 = physical_constants::molar_mass_of_water * 36;  // (Mg / ha / hr) / (mmol / m^2 / s)

    update(canopy_assimilation_rate_op, canopy_assimilation_rate * cf);

    update(GrossAssim_op, GrossAssim * cf);

    update(canopy_assimilation_rate_op, canopy_assimilation_rate * cf);

    update(canopy_transpiration_rate_op, canopy_transpiration_rate * cf2);

    update(canopy_conductance_op, canopy_conductance);
}

////////////////////////////////////////
// TEN LAYER CANOPY INTEGRATOR MODULE //
////////////////////////////////////////

/**
 * @class ten_layer_canopy_integrator
 *
 * @brief A child class of multilayer_canopy_integrator where the number of
 * layers has been defined. Instances of this class can be created using the
 * module factory, unlike the parent class `multilayer_canopy_integrator`.
 */
class ten_layer_canopy_integrator : public multilayer_canopy_integrator
{
   public:
    ten_layer_canopy_integrator(
        state_map const& input_quantities,
        state_map* output_quantities)
        : multilayer_canopy_integrator(
              ten_layer_canopy_integrator::nlayers,
              input_quantities,
              output_quantities)
    {
    }
    static string_vector get_inputs();
    static string_vector get_leaf_classes();
    static string_vector get_multiclass_multilayer_outputs();
    static string_vector get_pure_multilayer_outputs();
    static string_vector get_outputs();
    static std::string get_name() { return "ten_layer_canopy_integrator"; }

   private:
    // Number of layers
    int static const nlayers;

    // Main operation
    void do_operation() const;
};

int const ten_layer_canopy_integrator::nlayers = 10;

string_vector ten_layer_canopy_integrator::get_inputs()
{
    return multilayer_canopy_integrator::get_inputs(
        ten_layer_canopy_integrator::nlayers);
}

string_vector ten_layer_canopy_integrator::get_outputs()
{
    return multilayer_canopy_integrator::get_outputs(
        ten_layer_canopy_integrator::nlayers);
}

void ten_layer_canopy_integrator::do_operation() const
{
    multilayer_canopy_integrator::run();
}

}  // namespace standardBML
#endif
