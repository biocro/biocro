#ifndef MULTILAYER_CANOPY_INTEGRATOR_H
#define MULTILAYER_CANOPY_INTEGRATOR_H

#include "../modules.h"
//#include "../module_helper_functions.h"

/**
 * @class multilayer_canopy_integrator
 * 
 * @brief Calculates properties of each canopy layer using functions
 * found in AuxBioCro (for the most part). Also includes multiple
 * leaf classes (sunlit & shaded). Leaf class is added to output
 * parameters as a prefix, while layer number is added as a suffix.
 */
class multilayer_canopy_integrator : public SteadyModule
{
   public:
    multilayer_canopy_integrator(
        const std::string& module_name,
        const int& nlayers,
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
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
    const std::vector<const double*> sunlit_fraction_ips;
    const std::vector<const double*> sunlit_Assim_ips;
    const std::vector<const double*> sunlit_GrossAssim_ips;
    const std::vector<const double*> sunlit_Gs_ips;
    const std::vector<const double*> sunlit_TransR_ips;
    const std::vector<const double*> shaded_fraction_ips;
    const std::vector<const double*> shaded_Assim_ips;
    const std::vector<const double*> shaded_GrossAssim_ips;
    const std::vector<const double*> shaded_Gs_ips;
    const std::vector<const double*> shaded_TransR_ips;
    // References to input parameters
    const double& lai;
    const double& growth_respiration_fraction;
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
    static std::vector<std::string> get_inputs(int nlayers);
    static std::vector<std::string> get_outputs(int nlayers);
};

/**
 * @brief Define all inputs required by the module, adding layer suffixes as required
 */
std::vector<std::string> multilayer_canopy_integrator::get_inputs(int nlayers)
{
    // Define the multilayer inputs
    std::vector<std::string> multilayer_inputs = {
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
    std::vector<std::string> all_inputs = generate_multilayer_quantity_names(nlayers, multilayer_inputs);

    // Add any other inputs
    all_inputs.push_back("lai");                          // dimensionless from m^2 / m^2
    all_inputs.push_back("growth_respiration_fraction");  // dimensionless

    return all_inputs;
}

/**
 * @brief Define all outputs produced by the module
 */
std::vector<std::string> multilayer_canopy_integrator::get_outputs(int /*nlayers*/)
{
    return {
        "canopy_assimilation_rate",   // Mg / ha / hr
        "canopy_transpiration_rate",  // Mg / ha / hr
        "canopy_conductance",         //
        "GrossAssim"                  //
    };
}

void multilayer_canopy_integrator::do_operation() const
{
    multilayer_canopy_integrator::run();
}

void multilayer_canopy_integrator::run() const
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

        canopy_assimilation_rate += *sunlit_Assim_ips[i] * sunlit_lai + *shaded_Assim_ips[i] * shaded_lai;
        canopy_transpiration_rate += *sunlit_TransR_ips[i] * sunlit_lai + *shaded_TransR_ips[i] * shaded_lai;
        canopy_conductance += *sunlit_Gs_ips[i] * sunlit_lai + *shaded_Gs_ips[i] * shaded_lai;
        GrossAssim += *sunlit_GrossAssim_ips[i] * sunlit_lai + *shaded_GrossAssim_ips[i] * shaded_lai;
    }
    
    // Modify net assimilation to account for respiration
    // Note: this was originally only done for the C3 canopy
    // Note: it seems like this should not be necessary since the assimilation model includes
    //       respiration
    canopy_assimilation_rate *= (1.0 - growth_respiration_fraction);

    // Convert assimilation values from micromol / m^2 / s to Mg / ha / hr using the following
    // conversion factors:
    // 3600 - seconds per hour
    // 1e-6 - moles per micromole
    // 30 - grams of C6H12O6 (glucose) incorporated into dry biomass per mole of CO2
    // 1e-6 - megagrams per gram
    // 10000 - meters squared per hectare

    // Convert transpiration values from mol / m^2 / s to Mg / ha / hr using the following
    // conversion factors:
    // 3600 - seconds per hour
    // 1e-3 - millimoles per mole
    // 18 - grams per mole for H2O
    // 1e-6 - megagrams per  gram
    // 10000 - meters squared per hectare

    update(canopy_assimilation_rate_op, canopy_assimilation_rate * 3600 * 1e-6 * 30 * 1e-6 * 10000);
    update(GrossAssim_op, GrossAssim * 3600 * 1e-6 * 30 * 1e-6 * 10000);
    update(canopy_transpiration_rate_op, canopy_transpiration_rate * 3600 * 1e-3 * 18 * 1e-6 * 10000);
    update(canopy_conductance_op, canopy_conductance);
}

////////////////////////////////////////
// TEN LAYER CANOPY INTEGRATOR MODULE //
////////////////////////////////////////

/**
 * @class ten_layer_canopy_integrator
 * 
 * @brief A child class of multilayer_canopy_integrator where the number of layers has been defined.
 * Instances of this class can be created using the module factory.
 */
class ten_layer_canopy_integrator : public multilayer_canopy_integrator
{
   public:
    ten_layer_canopy_integrator(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        : multilayer_canopy_integrator("ten_layer_canopy_integrator",
                                       ten_layer_canopy_integrator::nlayers,
                                       input_parameters,
                                       output_parameters)  // Create the base class with the appropriate number of layers
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_leaf_classes();
    static std::vector<std::string> get_multiclass_multilayer_outputs();
    static std::vector<std::string> get_pure_multilayer_outputs();
    static std::vector<std::string> get_outputs();

   private:
    // Number of layers
    static const int nlayers;
    // Main operation
    void do_operation() const;
};

const int ten_layer_canopy_integrator::nlayers = 10;  // Set the number of layers

std::vector<std::string> ten_layer_canopy_integrator::get_inputs()
{
    return multilayer_canopy_integrator::get_inputs(ten_layer_canopy_integrator::nlayers);
}

std::vector<std::string> ten_layer_canopy_integrator::get_outputs()
{
    return multilayer_canopy_integrator::get_outputs(ten_layer_canopy_integrator::nlayers);
}

void ten_layer_canopy_integrator::do_operation() const
{
    multilayer_canopy_integrator::run();
}

#endif
