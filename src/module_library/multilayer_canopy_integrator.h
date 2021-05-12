#ifndef MULTILAYER_CANOPY_INTEGRATOR_H
#define MULTILAYER_CANOPY_INTEGRATOR_H

#include "../state_map.h"
#include "../modules.h"

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
class multilayer_canopy_integrator : public SteadyModule
{
   public:
    multilayer_canopy_integrator(
        std::string const& module_name,
        int const& nlayers,
        state_map const* input_parameters,
        state_map* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule(module_name),

          // Store the number of layers
          nlayers(nlayers),

          // Get pointers to input parameters
          sunlit_fraction_ips(get_multilayer_ip(input_parameters, nlayers, "sunlit_fraction")),
          sunlit_Assim_ips(get_multilayer_ip(input_parameters, nlayers, "sunlit_Assim")),
          sunlit_GrossAssim_ips(get_multilayer_ip(input_parameters, nlayers, "sunlit_GrossAssim")),
          sunlit_Gs_ips(get_multilayer_ip(input_parameters, nlayers, "sunlit_Gs")),
          sunlit_TransR_ips(get_multilayer_ip(input_parameters, nlayers, "sunlit_TransR")),
          shaded_fraction_ips(get_multilayer_ip(input_parameters, nlayers, "shaded_fraction")),
          shaded_Assim_ips(get_multilayer_ip(input_parameters, nlayers, "shaded_Assim")),
          shaded_GrossAssim_ips(get_multilayer_ip(input_parameters, nlayers, "shaded_GrossAssim")),
          shaded_Gs_ips(get_multilayer_ip(input_parameters, nlayers, "shaded_Gs")),
          shaded_TransR_ips(get_multilayer_ip(input_parameters, nlayers, "shaded_TransR")),

          // Get references to input parameters
          lai(get_input(input_parameters, "lai")),
          growth_respiration_fraction(get_input(input_parameters, "growth_respiration_fraction")),

          // Get pointers to output parameters
          canopy_assimilation_rate_op(get_op(output_parameters, "canopy_assimilation_rate")),
          canopy_transpiration_rate_op(get_op(output_parameters, "canopy_transpiration_rate")),
          canopy_conductance_op(get_op(output_parameters, "canopy_conductance")),
          GrossAssim_op(get_op(output_parameters, "GrossAssim"))
    {
    }

   private:
    // Number of layers
    const int nlayers;

    // Pointers to input parameters
    std::vector<double const*> const sunlit_fraction_ips;
    std::vector<double const*> const sunlit_Assim_ips;
    std::vector<double const*> const sunlit_GrossAssim_ips;
    std::vector<double const*> const sunlit_Gs_ips;
    std::vector<double const*> const sunlit_TransR_ips;
    std::vector<double const*> const shaded_fraction_ips;
    std::vector<double const*> const shaded_Assim_ips;
    std::vector<double const*> const shaded_GrossAssim_ips;
    std::vector<double const*> const shaded_Gs_ips;
    std::vector<double const*> const shaded_TransR_ips;

    // References to input parameters
    double const& lai;
    double const& growth_respiration_fraction;

    // Pointers to output parameters
    double* canopy_assimilation_rate_op;
    double* canopy_transpiration_rate_op;
    double* canopy_conductance_op;
    double* GrossAssim_op;

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
        "sunlit_Assim",       // micromole / m^2 /s
        "sunlit_GrossAssim",  // micromole / m^2 /s
        "sunlit_Gs",          // mmol / m^2 / s
        "sunlit_TransR",      // mol / m^2 / s
        "shaded_fraction",    // dimensionless
        "shaded_Assim",       // micromole / m^2 /s
        "shaded_GrossAssim",  // micromole / m^2 /s
        "shaded_Gs",          // mmol / m^2 / s
        "shaded_TransR",      // mol / m^2 / s
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
        "canopy_assimilation_rate",   // Mg / ha / hr
        "canopy_transpiration_rate",  // Mg / ha / hr
        "canopy_conductance",         // mmol / m^2 / s
        "GrossAssim"                  // Mg / ha / hr
    };
}

void multilayer_canopy_integrator::do_operation() const
{
    multilayer_canopy_integrator::run();
}

void multilayer_canopy_integrator::run() const
{
    double const LAIc = lai / nlayers;
    double canopy_assimilation_rate = 0;
    double canopy_transpiration_rate = 0;
    double canopy_conductance = 0;
    double GrossAssim = 0;

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
    }

    // Modify net assimilation to account for respiration
    // Note: this was originally only done for the C3 canopy
    // Note: it seems like this should not be necessary since the assimilation
    //   model includes respiration
    canopy_assimilation_rate *= (1.0 - growth_respiration_fraction);

    // Convert assimilation values from micromol / m^2 / s to Mg / ha / hr using
    // the following conversion factors:
    // 3600 - seconds per hour
    // 1e-6 - moles per micromole
    // 30 - grams of glucose incorporated into dry biomass per mole of CO2
    // 1e-6 - megagrams per gram
    // 10000 - meters squared per hectare

    // Convert transpiration values from mol / m^2 / s to Mg / ha / hr using the
    // following conversion factors:
    // 3600 - seconds per hour
    // 1e-3 - millimoles per mole
    // 18 - grams per mole for H2O
    // 1e-6 - megagrams per  gram
    // 10000 - meters squared per hectare

    update(canopy_assimilation_rate_op, canopy_assimilation_rate *
                                            3600 * 1e-6 * 30 * 1e-6 * 10000);

    update(GrossAssim_op, GrossAssim *
                              3600 * 1e-6 * 30 * 1e-6 * 10000);

    update(canopy_transpiration_rate_op, canopy_transpiration_rate *
                                             3600 * 1e-3 * 18 * 1e-6 * 10000);

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
        state_map const* input_parameters,
        state_map* output_parameters)
        : multilayer_canopy_integrator(
              "ten_layer_canopy_integrator",
              ten_layer_canopy_integrator::nlayers,
              input_parameters,
              output_parameters)
    {
    }
    static string_vector get_inputs();
    static string_vector get_leaf_classes();
    static string_vector get_multiclass_multilayer_outputs();
    static string_vector get_pure_multilayer_outputs();
    static string_vector get_outputs();

   private:
    // Number of layers
    int static const nlayers;

    // Main operation
    void do_operation() const;
};

int const ten_layer_canopy_integrator::nlayers = 10;

string_vector ten_layer_canopy_integrator::get_inputs()
{
    return multilayer_canopy_integrator::get_inputs(ten_layer_canopy_integrator::nlayers);
}

string_vector ten_layer_canopy_integrator::get_outputs()
{
    return multilayer_canopy_integrator::get_outputs(ten_layer_canopy_integrator::nlayers);
}

void ten_layer_canopy_integrator::do_operation() const
{
    multilayer_canopy_integrator::run();
}

#endif
