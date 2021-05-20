#ifndef ED_MULTILAYER_CANOPY_INTEGRATOR_H
#define ED_MULTILAYER_CANOPY_INTEGRATOR_H

#include "../modules.h"
#include "../state_map.h"

/**
 * @class ed_multilayer_canopy_integrator
 *
 * @brief Calculates properties of each canopy layer using functions
 * found in AuxBioCro (for the most part). Also includes multiple
 * leaf classes (sunlit & shaded). Leaf class is added to output
 * parameters as a prefix, while layer number is added as a suffix.
 */
class ed_multilayer_canopy_integrator : public SteadyModule
{
   public:
    ed_multilayer_canopy_integrator(
        const std::string& module_name,
        const int& nlayers,
        const state_map* input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule(module_name),
          // Store the number of layers
          nlayers(nlayers),
          // Get pointers to input quantities
          sunlit_fraction_ips(get_multilayer_ip(input_quantities, nlayers, "sunlit_fraction")),
          sunlit_assimilation_net_ips(get_multilayer_ip(input_quantities, nlayers, "sunlit_assimilation_net")),
          sunlit_assimilation_gross_ips(get_multilayer_ip(input_quantities, nlayers, "sunlit_assimilation_gross")),
          sunlit_conductance_stomatal_h2o_ips(get_multilayer_ip(input_quantities, nlayers, "sunlit_conductance_stomatal_h2o")),
          sunlit_transpiration_rate_ips(get_multilayer_ip(input_quantities, nlayers, "sunlit_transpiration_rate")),
          shaded_fraction_ips(get_multilayer_ip(input_quantities, nlayers, "shaded_fraction")),
          shaded_assimilation_net_ips(get_multilayer_ip(input_quantities, nlayers, "shaded_assimilation_net")),
          shaded_assimilation_gross_ips(get_multilayer_ip(input_quantities, nlayers, "shaded_assimilation_gross")),
          shaded_conductance_stomatal_h2o_ips(get_multilayer_ip(input_quantities, nlayers, "shaded_conductance_stomatal_h2o")),
          shaded_transpiration_rate_ips(get_multilayer_ip(input_quantities, nlayers, "shaded_transpiration_rate")),
          // Get references to input quantities
          lai(get_input(input_quantities, "lai")),
          growth_respiration_fraction(get_input(input_quantities, "growth_respiration_fraction")),
          // Get pointers to output quantities
          canopy_assimilation_rate_op(get_op(output_quantities, "canopy_assimilation_rate")),
          canopy_transpiration_rate_op(get_op(output_quantities, "canopy_transpiration_rate")),
          canopy_conductance_op(get_op(output_quantities, "canopy_conductance")),
          GrossAssim_op(get_op(output_quantities, "GrossAssim"))
    {
    }

   private:
    // Number of layers
    const int nlayers;
    // Pointers to input quantities
    const std::vector<const double*> sunlit_fraction_ips;
    const std::vector<const double*> sunlit_assimilation_net_ips;
    const std::vector<const double*> sunlit_assimilation_gross_ips;
    const std::vector<const double*> sunlit_conductance_stomatal_h2o_ips;
    const std::vector<const double*> sunlit_transpiration_rate_ips;
    const std::vector<const double*> shaded_fraction_ips;
    const std::vector<const double*> shaded_assimilation_net_ips;
    const std::vector<const double*> shaded_assimilation_gross_ips;
    const std::vector<const double*> shaded_conductance_stomatal_h2o_ips;
    const std::vector<const double*> shaded_transpiration_rate_ips;
    // References to input quantities
    const double& lai;
    const double& growth_respiration_fraction;
    // Pointers to output quantities
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
 * @brief Define all inputs required by the module, adding layer suffixes as required
 */
string_vector ed_multilayer_canopy_integrator::get_inputs(int nlayers)
{
    // Define the multilayer inputs
    string_vector multilayer_inputs = {
        "sunlit_fraction",                  // dimensionless
        "sunlit_assimilation_net",          // mol / m^2 /s
        "sunlit_assimilation_gross",        // mol / m^2 /s
        "sunlit_conductance_stomatal_h2o",  // mol / m^2 / s
        "sunlit_transpiration_rate",        // mol / m^2 / s
        "shaded_fraction",                  // dimensionless
        "shaded_assimilation_net",          // mol / m^2 /s
        "shaded_assimilation_gross",        // mol / m^2 /s
        "shaded_conductance_stomatal_h2o",  // mol / m^2 / s
        "shaded_transpiration_rate",        // mol / m^2 / s
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
string_vector ed_multilayer_canopy_integrator::get_outputs(int /*nlayers*/)
{
    return {
        "canopy_assimilation_rate",   // Mg / ha / hr
        "canopy_transpiration_rate",  // Mg / ha / hr
        "canopy_conductance",         // mmol / m^2 / hr
        "GrossAssim"                  // Mg / ha / hr
    };
}

void ed_multilayer_canopy_integrator::do_operation() const
{
    ed_multilayer_canopy_integrator::run();
}

void ed_multilayer_canopy_integrator::run() const
{
    const double LAIc = lai / nlayers;
    double canopy_assimilation_rate = 0;
    double canopy_transpiration_rate = 0;
    double canopy_conductance = 0;
    double GrossAssim = 0;

    // Integrate assimilation, transpiration, and conductance throughout the canopy
    for (int i = 0; i < nlayers; ++i) {
        const double sunlit_lai = *sunlit_fraction_ips[i] * LAIc;
        const double shaded_lai = *shaded_fraction_ips[i] * LAIc;

        canopy_assimilation_rate += *sunlit_assimilation_net_ips[i] * sunlit_lai + *shaded_assimilation_net_ips[i] * shaded_lai;
        canopy_transpiration_rate += *sunlit_transpiration_rate_ips[i] * sunlit_lai + *shaded_transpiration_rate_ips[i] * shaded_lai;
        canopy_conductance += *sunlit_conductance_stomatal_h2o_ips[i] * sunlit_lai + *shaded_conductance_stomatal_h2o_ips[i] * shaded_lai;
        GrossAssim += *sunlit_assimilation_gross_ips[i] * sunlit_lai + *shaded_assimilation_gross_ips[i] * shaded_lai;
    }

    // Modify net assimilation to account for respiration
    // Note: this was originally only done for the C3 canopy
    // Note: it seems like this should not be necessary since the assimilation model includes
    //       respiration
    canopy_assimilation_rate *= (1.0 - growth_respiration_fraction);

    // Convert assimilation values from mol / m^2 / s to Mg / ha / hr using the following
    // conversion factors:
    // 3600 - seconds per hour
    // 30 - grams of C6H12O6 (glucose) incorporated into dry biomass per mole of CO2
    // 1e-6 - megagrams per gram
    // 10000 - meters squared per hectare

    // Convert transpiration values from mol / m^2 / s to Mg / ha / hr using the following
    // conversion factors:
    // 3600 - seconds per hour
    // 18 - grams per mole for H2O
    // 1e-6 - megagrams per  gram
    // 10000 - meters squared per hectare

    // Convert conductance values from mol / m^2 / s to mmol / m^2 / s using 1e3 mmol / mol

    update(canopy_assimilation_rate_op, canopy_assimilation_rate * 3600 * 30 * 1e-6 * 10000);
    update(GrossAssim_op, GrossAssim * 3600 * 30 * 1e-6 * 10000);
    update(canopy_transpiration_rate_op, canopy_transpiration_rate * 3600 * 18 * 1e-6 * 10000);
    update(canopy_conductance_op, canopy_conductance * 1e3);
}

////////////////////////////////////////
// TEN LAYER CANOPY INTEGRATOR MODULE //
////////////////////////////////////////

/**
 * @class ed_ten_layer_canopy_integrator
 *
 * @brief A child class of ed_multilayer_canopy_integrator where the number of layers has been defined.
 * Instances of this class can be created using the module factory.
 */
class ed_ten_layer_canopy_integrator : public ed_multilayer_canopy_integrator
{
   public:
    ed_ten_layer_canopy_integrator(
        const state_map* input_quantities,
        state_map* output_quantities)
        : ed_multilayer_canopy_integrator("ed_ten_layer_canopy_integrator",
                                          ed_ten_layer_canopy_integrator::nlayers,
                                          input_quantities,
                                          output_quantities)  // Create the base class with the appropriate number of layers
    {
    }
    static string_vector get_inputs();
    static string_vector get_leaf_classes();
    static string_vector get_multiclass_multilayer_outputs();
    static string_vector get_pure_multilayer_outputs();
    static string_vector get_outputs();

   private:
    // Number of layers
    static const int nlayers;
    // Main operation
    void do_operation() const;
};

const int ed_ten_layer_canopy_integrator::nlayers = 10;  // Set the number of layers

string_vector ed_ten_layer_canopy_integrator::get_inputs()
{
    return ed_multilayer_canopy_integrator::get_inputs(ed_ten_layer_canopy_integrator::nlayers);
}

string_vector ed_ten_layer_canopy_integrator::get_outputs()
{
    return ed_multilayer_canopy_integrator::get_outputs(ed_ten_layer_canopy_integrator::nlayers);
}

void ed_ten_layer_canopy_integrator::do_operation() const
{
    ed_multilayer_canopy_integrator::run();
}

#endif
