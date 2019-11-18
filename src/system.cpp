#include "system.h"

// For integer time and std::vector state
void System::operator()(const std::vector<double>& x, std::vector<double>& dxdt, const int& t) {
    // Update the internally stored parameter list and use it to calculate a derivative
    ncalls ++;
    update_varying_params(t);
    update_state_params(x);
    run_steady_state_modules();
    run_derivative_modules(dxdt);
}

// For double time and std::vector state
void System::operator()(const std::vector<double>& x, std::vector<double>& dxdt, const double& t) {
    // Update the internally stored parameter list and use it to calculate a derivative
    ncalls ++;
    update_varying_params(t);
    update_state_params(x);
    run_steady_state_modules();
    run_derivative_modules(dxdt);
}

// For integer time and boost::numeric::ublas::vector state
void System::operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const int& t) {
    // Update the internally stored parameter list and use it to calculate a derivative
    ncalls ++;
    update_varying_params(t);
    update_state_params(x);
    run_steady_state_modules();
    run_derivative_modules(dxdt);
}

// For double time and boost::numeric::ublas::vector state
void System::operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const double& t) {
    // Update the internally stored parameter list and use it to calculate a derivative
    ncalls ++;
    update_varying_params(t);
    update_state_params(x);
    run_steady_state_modules();
    run_derivative_modules(dxdt);
}

void System::operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::matrix<double>& jacobi, const double& t, boost::numeric::ublas::vector<double>& dfdt) {
    // Numerically compute the Jacobian matrix
    //  The odeint Rosenbrock stepper requires the use of UBLAS vectors and matrices and the Jacobian is only required when using this
    //    stepper, so we can restrict the state vector type to be UBLAS
    // Discussion of step size from http://www.iue.tuwien.ac.at/phd/khalil/node14.html:
    //  "It is known that numerical differentiation is an unstable procedure prone to truncation and subtractive cancellation errors.
    //  Decreasing the step size will reduce the truncation error.
    //  Unfortunately a smaller step has the opposite effect on the cancellation error.
    //  Selecting the optimal step size for a certain problem is computationally expensive and the benefits achieved are not justifiable
    //    as the effect of small errors in the values of the elements of the Jacobian matrix is minor.
    //  For this reason, the sizing of the finite difference step is not attempted and a constant increment size is used in evaluating the gradient."
    // In BioCro, we fix a step size and only evaluate the forward perturbation to reduce calculation costs
    //  In other words:
    //    (1) We calculate dxdt using the input (x,t) (called dxdt_c for current)
    //    (2) We make a forward perturbation by adding h to one state variable and calculating the time derivatives (called dxdt_p for perturbation)
    //    (3) We calculate the rate of change for each state variable according to (dxdt_p[i] - dxdt_c[i])/h
    //    (4) We repeat steps (2) and (3) for each state variable
    //  The alternative method would be:
    //    (1) We make a backward perturbation by substracting h from one state variable and calculating the time derivatives (called dxdt_b for backward)
    //    (2) We make a forward perturbation by adding h to the same state variable and calculating the time derivatives (called dxdt_f for forward)
    //    (3) We calculate the rate of change for each state variable according to (dxdt_f[i] - dxdt_b[i])/(2*h)
    //    (4) We repeat steps (1) through (3) for each state variable
    //  In the simpler scheme, we make N + 1 derivative evaluations, where N is the number of state variables
    //  In the other scheme, we make 2N derivative evaluations
    //  The improvement in accuracy does not seem to outweigh the cost of additional calculations, since BioCro derivatives are expensive
    //  Likewise, higher-order numerical derivative calculations are also not worthwhile
    
    size_t n = x.size();
    
    // Make vectors to store the current and perturbed dxdt
    boost::numeric::ublas::vector<double> dxdt_c(n);
    boost::numeric::ublas::vector<double> dxdt_p(n);
    
    // Get the current dxdt
    operator()(x, dxdt_c, t);
    
    // Perturb each state variable and find the corresponding change in the derivative
    double h;
    boost::numeric::ublas::vector<double> xperturb = x;
    for(size_t i = 0; i < n; i++) {
        // Ensure that the step size h is close to eps_deriv but is exactly representable
        //  (see Numerical Recipes in C, 2nd ed., Section 5.7)
        h = eps_deriv;
        double temp = x[i] + h;
        h = temp - x[i];
        
        // Calculate the new derivatives
        xperturb[i] = x[i] + h;             // Add h to the ith state variable
        operator()(xperturb, dxdt_p, t);    // Calculate dxdt_p
        
        // Store the results in the Jacobian matrix
        for(size_t j = 0; j < n; j++) jacobi(j,i) = (dxdt_p[j] - dxdt_c[j])/h;
        
        // Reset the ith state variable
        xperturb[i] = x[i];                 // Reset the ith state variable
    }
    
    // Perturb the time and find the corresponding change in dxdt
    // Use a forward step whenever possible
    h = eps_deriv;
    double temp = t + h;
    h = temp - t;
    if(t + h <= (double)ntimes - 1.0) {
        operator()(x, dxdt_p, t + h);
        for(size_t j = 0; j < n; j++) dfdt[j] = (dxdt_p[j] - dxdt_c[j])/h;
    }
    else {
        operator()(x, dxdt_p, t - h);
        for(size_t j = 0; j < n; j++) dfdt[j] = (dxdt_c[j] - dxdt_p[j])/h;
    }
}

System::System(
    std::unordered_map<std::string, double> const& init_state,
    std::unordered_map<std::string, double> const& invariant_params,
    std::unordered_map<std::string, std::vector<double>> const &varying_params,
    std::vector<std::string> const &ss_module_names,
    std::vector<std::string> const &deriv_module_names,
    bool const verb,
    void (*print_fcn_ptr) (char const *format, ...)) :
    initial_state(init_state),
    invariant_parameters(invariant_params),
    varying_parameters(varying_params),
    steady_state_module_names(ss_module_names),
    derivative_module_names(deriv_module_names),
    verbose(verb),
    print_msg(print_fcn_ptr)
{
    // A note about variables:
    //  The system stores an unordered_map of variables, which includes several types:
    //   - The state variables that evolve according to differential equations and whose initial values are an input to the system
    //       (e.g. position and velocity for a mass-on-a-spring harmonic oscillator)
    //   - The invariant parameters that remain constant throughout the calculation
    //       (e.g. mass and spring constant for a mass-on-a-spring harmonic oscillator)
    //   - The varying parameters that change throughout the calculation in a way that is known beforehand
    //       (e.g. the temperature throughout a growing season)
    //   - The 'steady state' parameters that are calculated from other parameters at each time step
    //       (e.g. total biomass calculated by adding several state variables together)
    //
    // Rules for variables that are enforced while building the system:
    //  - The invariant parameters must include one called 'timestep'
    //  - The parameters in the initial state, invariant parameters, and varying parameters must be unique
    //  - A module's input parameters must be included in the state, invariant parameters, or varying parameters OR be output by a previous module
    //      (i.e., a module's input must be defined before it runs)
    //  - A steady state module's output parameters must not be included in the state, invariant parameters, or varying parameters AND must not be the output of a previous module
    //      (i.e., all steady state module outputs must be unique in the system)
    //  - A derivative module's output parameters must be included in the state
    //      (i.e., derivatives can only be defined for state variables)
    
    // Lists for collecting variables
    std::set<std::string> unique_steady_state_parameter_names;      // All parameters output by steady state modules
    std::set<std::string> unique_changing_parameters;               // All variables that change throughout a simulation (used for saving/returning results, since we shouldn't include invariant parameters)
    
    // A string for describing problems with the inputs
    std::string error_string;                                       // A message to send to the user about any issues that were discovered during the system setup
    
    if(verbose) print_msg("\nStarting to apply checks and build the system:\n");
    
    // Check through the input variables and modules, making sure everything is consistent.
    // If everything is okay, generate a central variable map, a module output map, lists
    //  of modules, and information about which variables will be changing throughout
    //  a simulation
    // This function will throw an exception if serious errors are found with the inputs
    process_variable_and_module_inputs(unique_steady_state_parameter_names, unique_changing_parameters);
    
    // Get pairs of pointers to important subsets of the variables, e.g.,
    //  pairs of pointers to the state variables in the main map and the
    //  module output map.
    // These pairs allow us to efficiently retrieve the output of each
    //  module and store it in the main variable map when running the system,
    //  to update the varying parameters at new time points, etc
    // Also get a pointer to the timestep variable
    get_pointer_pairs(unique_steady_state_parameter_names);
    
    // Fill in the initial values and test the modules
    test_all_modules(error_string);
    
    // If any errors have occurred, notify the user and throw an error to halt the operation,
    //  since the user needs to address these problems before continuing
    report_errors(error_string, verbose);
    
    // Get information that we will need when running a simulation and returning the results
    get_simulation_info(unique_changing_parameters);
    
    // Reset the derivative evaluation counter
    ncalls = 0;
    
    // Now we are done!
    if(verbose) print_msg("Done applying checks and building the system!\n\n");
}

void System::process_variable_and_module_inputs(std::set<std::string>& unique_steady_state_parameter_names, std::set<std::string>& unique_changing_parameters) {
    // Lists for collecting variables and modules
    std::set<std::string> unique_variable_names;                    // All variable names
    std::set<std::string> unique_steady_state_module_names;         // All steady state module names
    std::set<std::string> unique_derivative_module_names;           // All derivative module names
    std::set<std::string> unique_derivative_outputs;                // All state variables output by derivative modules
    std::set<std::string> unique_module_inputs;                     // All variables used as inputs to modules
    // Lists for describing problems with the inputs
    std::vector<std::string> duplicate_variable_names;              // A list of variable names that are duplicated in the initial state, invariant parameters, and varying parameters
    std::vector<std::string> duplicate_module_names;                // A list of module names that are duplicated
    std::vector<std::string> duplicate_output_variables;            // A list of variable names that are duplicated in the output of steady state modules
    std::vector<std::string> undefined_input_variables;             // A list of module input variable names that are not defined when the module runs
    std::vector<std::string> illegal_output_variables;              // A list of derivative module output variable names that are not state variables
    std::vector<std::string> incorrect_modules;                     // A list of mischaracterized modules, e.g., derivative modules included in the steady state module list
    std::string error_string;                                       // A message to send to the user about any issues that were discovered during the system setup
    
    // Make sure the inputs meet some basic requirements
    //  and create the "doy_dbl" varying parameter
    basic_input_checks();
    
    // Start collecting variable names from the input lists
    //  of state variables and parameters
    // Along the way, check for any duplicated variable names
    get_variables_from_input_lists(unique_variable_names, unique_changing_parameters, duplicate_variable_names);
    
    // Initialize a module factory, which will be used to get module I/O information
    //  and to create modules
    // Note: the inputs to the module factory have not been fully initialized yet. We can
    //  get the module input/output variables right now, but any attempt to create a module
    //  will fail
    ModuleFactory module_factory(&parameters, &module_output_map);
    
    // Continue collecting variable names from the modules
    // Along the way, check for any duplicated variable or module
    //  names, and any undefined module inputs
    get_variables_from_modules(module_factory,
        unique_steady_state_parameter_names,
        unique_derivative_outputs,
        unique_variable_names, unique_module_inputs, unique_changing_parameters,
        undefined_input_variables, duplicate_output_variables, duplicate_module_names, illegal_output_variables
    );
    unique_module_inputs.insert("timestep");    // Even when no modules explicitly use the timestep variable, include it here to prevent spurious warning messages about timestep being unused
    
    // Check the variable usage, i.e., check whether all state variables have
    //  associated derivatives, whether all invariant parameters are actually
    //  required, etc.
    // This information may be useful to the user but isn't serious enough to
    //  prevent the system from running
    if(verbose) check_variable_usage(unique_derivative_outputs, unique_module_inputs);
    
    // Collect information about any errors that may have occurred while checking the parameters
    process_errors(duplicate_variable_names, std::string("Some variables in the initial state, invariant parameters, and/or varying parameters were duplicated"), error_string, verbose, print_msg);
    process_errors(duplicate_module_names, std::string("Some modules were duplicated in the steady state and/or derivative module lists"), error_string, verbose, print_msg);
    process_errors(duplicate_output_variables, std::string("Some steady state module output parameters were already included in the initial state, invariant parameters, varying parameters, or previous steady state modules"), error_string, verbose, print_msg);
    process_errors(undefined_input_variables, std::string("Some modules required inputs variables that were not defined by the initial state, invariant parameters, varying parameters, or previous steady state modules"), error_string, verbose, print_msg);
    process_errors(illegal_output_variables, std::string("Some derivative modules returned derivatives for variables that were not included in the initial state"), error_string, verbose, print_msg);
    
    // If any errors have occurred, notify the user and throw an error to halt the operation,
    //  since the user needs to address these problems before continuing
    report_errors(error_string, verbose);
    
    // Initialize the module output map
    module_output_map = parameters;
    
    // Now that we have a complete list of variables, we can use the module factory to create the modules
    create_modules(module_factory, incorrect_modules);
    
    // Collect information about any errors that may have occurred while creating the modules
    process_errors(incorrect_modules, std::string("Some modules were mischaracterized in the input lists"), error_string, verbose, print_msg);
    
    // If any errors have occurred, notify the user and throw an error to halt the operation,
    //  since the user needs to address these problems before continuing
    report_errors(error_string, verbose);
}

void System::basic_input_checks() {    
    // Check to make sure at least one module was supplied
    if(verbose) print_msg("\nChecking to make sure at least one module was specified... ");
    if(steady_state_module_names.size() == 0 && derivative_module_names.size() == 0) {
        throw std::logic_error(std::string("No input modules were found! A system requires at least one module.\n"));
    }
    if(verbose) print_msg("done!\n\n");
    
    // Check to make sure the timing information has been properly supplied.
    // Here we expect to find "doy" and "hour" inputs, but we need to
    //  convert them into a continuous time parameter (called "doy_dbl").
    // We also check to make sure the time points are evenly spaced by the
    //  value set by the "timestep" parameter.
    if(verbose) print_msg("Checking to make sure the time parameters are properly defined... ");
    if(invariant_parameters.find("timestep") == invariant_parameters.end()) {
        throw std::logic_error(std::string("The 'timestep' parameter was not defined in the invariant parameters. This is a required parameter for any system.\n"));
    }
    if(varying_parameters.find("doy") == varying_parameters.end()) {
        throw std::logic_error(std::string("The 'doy' parameter was not defined in the varying parameters. This is a required parameter for any system.\n"));
    }
    if(varying_parameters.find("hour") == varying_parameters.end()) {
        throw std::logic_error(std::string("The 'hour' parameter was not defined in the varying parameters. This is a required parameter for any system.\n"));
    }
    if(varying_parameters.find("doy_dbl") != varying_parameters.end() || invariant_parameters.find("doy_dbl") != invariant_parameters.end() || initial_state.find("doy_dbl") != initial_state.end()) {
        throw std::logic_error(std::string("'doy_dbl' is a reserved parameter name, and it will be automatically calculated from the 'doy' and 'hour' parameters. Please remove or rename this parameter.\n"));
    }
    std::vector<double> hour_vec = varying_parameters.at("hour");
    for(size_t i = 1; i < hour_vec.size(); i++ ) {
        if(abs(hour_vec[i] - hour_vec[i-1] - 24.0 * floor((hour_vec[i] - hour_vec[i-1]) / 24.0) - invariant_parameters.at("timestep")) > 0.01) {
            throw std::logic_error(std::string("At least one pair of sequential values of 'hour' are not separated by the value of 'timestep'. Please check over the inputs.\n"));
        }
    }
    std::vector<double> doy_vec = varying_parameters.at("doy");
    std::vector<double> doy_dbl_vec(doy_vec.size());
    for(size_t i = 0; i < doy_dbl_vec.size(); i++) doy_dbl_vec[i] = hour_vec[i]/24.0 + doy_vec[i];      // Make a new parameter expressing the day of year as a double (note: might have trouble with multi-year simulations)
    varying_parameters["doy_dbl"] = doy_dbl_vec;
    varying_parameters.erase("doy");
    varying_parameters.erase("hour");
    if(verbose) print_msg("done!\n");
    
    // Check for any other prohibited parameter names
    if(varying_parameters.find("ncalls") != varying_parameters.end() || invariant_parameters.find("ncalls") != invariant_parameters.end() || initial_state.find("ncalls") != initial_state.end()) {
        throw std::logic_error(std::string("'ncalls' is a reserved parameter name, and it will be automatically filled with the number of derivative evaluations performed during a simulation. Please remove or rename this parameter.\n"));
    }
    
}

void System::get_variables_from_input_lists(
    std::set<std::string>& unique_variable_names,
    std::set<std::string>& unique_changing_parameters,
    std::vector<std::string>& duplicate_variable_names)
{
    // Go through the varying parameters
    if(verbose) print_msg("\nGetting variable names from the varying parameters:\n");
    for(auto x : varying_parameters) {
        if(unique_variable_names.find(x.first) == unique_variable_names.end()) {
            unique_variable_names.insert(x.first);
            unique_changing_parameters.insert(x.first);
            parameters[x.first] = x.second[0];
            if(verbose) print_msg("  %s[0] = %f\n", (x.first).c_str(), x.second[0]);
        }
        else duplicate_variable_names.push_back(std::string("Parameter '") + x.first + std::string("' from the varying parameters"));
    }
    
    // Go through the initial state
    if(verbose) print_msg("\nGetting variable names from the initial state:\n");
    for(auto x : initial_state) {
        if(unique_variable_names.find(x.first) == unique_variable_names.end()) {
            unique_variable_names.insert(x.first);
            unique_changing_parameters.insert(x.first);
            parameters[x.first] = x.second;
            if(verbose) print_msg("  %s[0] = %f\n", (x.first).c_str(), x.second);
        }
        else duplicate_variable_names.push_back(std::string("Parameter '") + x.first + std::string("' from the initial state"));
    }
    
    // Go through the invariant parameters
    if(verbose) print_msg("\nGetting variable names from the invariant parameters:\n");
    for(auto x : invariant_parameters) {
        if(unique_variable_names.find(x.first) == unique_variable_names.end()) {
            unique_variable_names.insert(x.first);
            parameters[x.first] = x.second;
            if(verbose) print_msg("  %s = %f\n", (x.first).c_str(), x.second);
        }
        else duplicate_variable_names.push_back(std::string("Parameter '") + x.first + std::string("' from the invariant parameters"));
    }
}

void System::get_variables_from_modules(
    ModuleFactory& module_factory,
    std::set<std::string>& unique_steady_state_parameter_names,
    std::set<std::string>& unique_derivative_outputs,
    std::set<std::string>& unique_variable_names,
    std::set<std::string>& unique_module_inputs,
    std::set<std::string>& unique_changing_parameters,
    std::vector<std::string>& undefined_input_variables,
    std::vector<std::string>& duplicate_output_variables,
    std::vector<std::string>& duplicate_module_names,
    std::vector<std::string>& illegal_output_variables)
{
    // Define containers for collecting important variable names
    std::set<std::string> unique_ss_outputs;
    std::set<std::string> deriv_module_outputs;
    
    // Collect variable names from the steady state modules
    if(verbose) print_msg("\nGetting variable names from the steady state modules:\n");
    get_variables_from_ss_modules(
        steady_state_module_names,
        module_factory,
        unique_module_inputs,
        unique_ss_outputs,
        duplicate_output_variables,
        duplicate_module_names
    );
    
    // Check for any steady state module outputs that were already defined by the initial state,
    //  invariant parameters, or varying parameters
    for(std::string p : unique_ss_outputs) {
        if(unique_variable_names.find(p) != unique_variable_names.end()) duplicate_output_variables.push_back(std::string("Variable '") + p + std::string("'"));
    }
    
    // Update the variable lists to include the steady state module output variables
    for(std::string p : unique_ss_outputs) {
        unique_variable_names.insert(p);
        unique_steady_state_parameter_names.insert(p);
        unique_changing_parameters.insert(p);
        parameters[p] = 0.0;
        if(verbose) print_msg("  %s\n", p.c_str());
    }
    
    // Collect variable names from the derivative modules
    if(verbose) print_msg("\nChecking the derivative module input and output parameters... ");
    get_variables_from_derivative_modules(
        derivative_module_names,
        module_factory,
        unique_module_inputs,
        unique_derivative_outputs,
        duplicate_module_names
    );
    if(verbose) print_msg("done!\n\n");
    
    // Check for any illegal derivative output variables
    for(std::string p : unique_derivative_outputs) {
        if(initial_state.find(p) == initial_state.end()) illegal_output_variables.push_back(std::string("Variable '") + p + std::string("'"));
    }
    
    // Check for any undefined input variables
    for(std::string p : unique_module_inputs) {
        if(unique_variable_names.find(p) == unique_variable_names.end()) undefined_input_variables.push_back(std::string("Variable '") + p + std::string("'"));
    }
}

void System::check_variable_usage(
    std::set<std::string> const& unique_derivative_outputs,
    std::set<std::string> const& unique_module_inputs) const
{
    // Check for state variables that lack derivatives
    if(unique_derivative_outputs.size() != initial_state.size()) {
        print_msg("No derivatives were supplied for the following state variables:\n");
        for(auto x : initial_state) {
            if(unique_derivative_outputs.find(x.first) == unique_derivative_outputs.end()) print_msg("  %s\n", (x.first).c_str());
        }
        print_msg("These variables will not change with time.\n");
        print_msg("You may want to consider adding one or more derivative modules that describe them,\n");
        print_msg("or remove them from the initial state.\n\n");
    }
    
    // Check for unused invariant parameters
    bool found_unused_invariant_parameter = false;
    for(auto x : invariant_parameters) {
        if(unique_module_inputs.find(x.first) == unique_module_inputs.end()) {
            if(found_unused_invariant_parameter == false) {
                print_msg("The following invariant parameters were not used as inputs to any module:\n  %s\n", (x.first).c_str());
                found_unused_invariant_parameter = true;
            }
            else print_msg("  %s\n", (x.first).c_str());
        }
    }
    if(found_unused_invariant_parameter) print_msg("You may want to consider removing them from the input list.\n\n");
    else print_msg("All invariant parameters were used as module inputs.\n\n");
    
    // Let the user know which parameters were used by at least one module
    print_msg("The following parameters were used as inputs to at least one module:\n");
    for(std::string p : unique_module_inputs) print_msg("  %s\n", p.c_str());
    print_msg("\n");
}

void System::create_modules(ModuleFactory& module_factory, std::vector<std::string>& incorrect_modules) {
    // Make a vector to store the names of modules that are incompatible with adaptive step size integrators
    std::vector<std::string> adaptive_step_size_incompat;
    
    // Create the steady state modules, checking to make sure they are properly categorized and whether they are
    //  compatible with adaptive step size integrators
    if(verbose) print_msg("Creating the steady state modules from the list and making sure the list only includes steady state modules... ");
    create_modules_from_names(
        steady_state_module_names,
        false,
        steady_state_modules,
        module_factory,
        incorrect_modules,
        adaptive_step_size_incompat,
        verbose,
        print_msg
    );
    if(verbose) print_msg("\n...done!\n\n");
    
    // Create the derivative modules, checking to make sure they are properly categorized and whether they are
    //  compatible with adaptive step size integrators
    if(verbose) print_msg("Creating the derivative modules from the list and making sure the list only includes derivative modules... ");
    create_modules_from_names(
        derivative_module_names,
        true,
        derivative_modules,
        module_factory,
        incorrect_modules,
        adaptive_step_size_incompat,
        verbose,
        print_msg
    );
    if(verbose) print_msg("\n...done!\n\n");
    
    // Let the user know about adaptive step size compatibility
    if(adaptive_step_size_incompat.size() != 0) {
        adaptive_compatible = false;
        if(verbose) {
            print_msg("The following modules are incompatible with adaptive step size integrators, so this system can only be solved with the fixed step size Euler method:\n");
            for(std::string s : adaptive_step_size_incompat) print_msg("%s\n", s.c_str());
            print_msg("\n");
        }
    }
    else {
        adaptive_compatible = true;
        if(verbose) print_msg("All modules are compatible with adaptive step size integrators, so this system can be solved using adaptive Runge-Kutta or Rosenbrock methods.\n\n");
    }
}

void System::get_pointer_pairs(std::set<std::string> const& unique_steady_state_parameter_names) {
    // Get a pointer to the timestep
    timestep_ptr = &parameters.at("timestep");
    
    // Get pointers to the state variables in the parameter and module output maps
    for(auto x : initial_state) {
        std::pair<double*, double*> temp(&parameters.at(x.first), &module_output_map.at(x.first));
        state_ptrs.push_back(temp);
    }
    
    // Get pointers to the steady state parameters in the parameter and module output maps
    for(std::string p : unique_steady_state_parameter_names) {
        std::pair<double*, double*> temp(&parameters.at(p), &module_output_map.at(p));
        steady_state_ptrs.push_back(temp);
    }
    
    // Get pointers to the varying parameters in the parameter and varying parameter maps
    for(auto x : varying_parameters) {
        std::pair<double*, std::vector<double>*> temp(&parameters.at(x.first), &varying_parameters.at(x.first));
        varying_ptrs.push_back(temp);
    }
}

void System::test_all_modules(std::string& total_error_string) {
    if(verbose) print_msg("Trying to run all the modules... ");
    
    // Test the steady state modules
    try {test_steady_state_modules();}
    catch (const std::exception& e) {total_error_string += e.what();}
    
    // Test the derivative modules
    std::vector<double> temp_vec(state_ptrs.size());
    try {test_derivative_modules(temp_vec);}
    catch (const std::exception& e) {total_error_string += e.what();}
    
    if(verbose) print_msg("done!\n\n");
}

void System::get_simulation_info(std::set<std::string> const& unique_changing_parameters) {
    // Get the number of time points
    ntimes = (varying_parameters.at("doy_dbl")).size();
    
    // Get the names of the state variables
    for(auto x : initial_state) state_parameter_names.push_back(x.first);
    
    // Create a vector of the names of variables that change throughout a simulation
    output_param_vector.resize(unique_changing_parameters.size());
    std::copy(unique_changing_parameters.begin(), unique_changing_parameters.end(), output_param_vector.begin());
    
    // Create a vector of pointers to the variables that change throughout a simulation
    output_ptr_vector.resize(unique_changing_parameters.size());
    for(size_t i = 0; i < output_param_vector.size(); i++) output_ptr_vector[i] = &parameters.at(output_param_vector[i]);
}

bool System::get_state_indx(int& state_indx, const std::string& parameter_name) const {
    for(size_t i = 0; i < state_parameter_names.size(); i++) {
        if(state_parameter_names[i] == parameter_name) {
            state_indx = i;
            return true;
        }
    }
    return false;
}

bool System::get_param(double& value, const std::string& parameter_name) const {
    if(parameters.find(parameter_name) != parameters.end()) {
        value = parameters.at(parameter_name);
        return true;
    }
    return false;
}

void System::reset() {
    // Put all parameters back to their original values
    int t = 0;
    update_varying_params(t);
    for(auto x : initial_state) parameters[x.first] = x.second;
    run_steady_state_modules();
}

void System::set_param(const double& value, const std::string& parameter_name) {
    if(parameters.find(parameter_name) != parameters.end()) parameters[parameter_name] = value;
    else throw std::logic_error(std::string("Thrown by System::set_param - could not find a parameter called ") + parameter_name);
}

void System::set_param(const std::vector<double>& values, const std::vector<std::string>& parameter_names) {
    if(values.size() != parameter_names.size()) throw std::logic_error("Thrown by System::set_param - input vector lengths are different\n");
    for(size_t i = 0; i < values.size(); i++) set_param(values[i], parameter_names[i]);
}

// For std::vector state
void System::get_state(std::vector<double>& x) const {
    x.resize(state_ptrs.size());
    for(size_t i = 0; i < x.size(); i++) x[i] = *(state_ptrs[i].first);
}

// For boost::numeric::ublas::vector state
void System::get_state(boost::numeric::ublas::vector<double>& x) const {
    x.resize(state_ptrs.size());
    for(size_t i = 0; i < x.size(); i++) x[i] = *(state_ptrs[i].first);
}

// For integer time and std::vector state
std::unordered_map<std::string, std::vector<double>> System::get_results(const std::vector<std::vector<double>>& x_vec, const std::vector<int>& times) {
    // Make the result map
    std::unordered_map<std::string, std::vector<double>> results;
    
    // Initialize the parameter names
    std::vector<double> temp(x_vec.size());
    for(std::string p : output_param_vector) results[p] = temp;
    
    // Store the derivative evaluation counter
    //  It will show up as a whole vector with the same value at every time point
    //  Maybe there is a better way to do this?
    std::fill(temp.begin(), temp.end(), ncalls);
    results["ncalls"] = temp;
    
    // Store the data
    for(size_t i = 0; i < x_vec.size(); i++) {
        // Unpack the latest time and state from the calculation results
        std::vector<double> current_state = x_vec[i];
        int current_time = times[i];
        // Get the corresponding parameter list
        update_varying_params(current_time);
        update_state_params(current_state);
        run_steady_state_modules();
        // Add the list to the results map
        for(size_t j = 0; j < output_param_vector.size(); j++) (results[output_param_vector[j]])[i] = parameters[output_param_vector[j]];
    }
    
    // Return the result map
    return results;
}

// For double time and boost::numeric::ublas::vector state
std::unordered_map<std::string, std::vector<double>> System::get_results(const std::vector<boost::numeric::ublas::vector<double>>& x_vec, const std::vector<double>& times) {
    // Make the result map
    std::unordered_map<std::string, std::vector<double>> results;
    
    // Initialize the parameter names
    std::vector<double> temp(x_vec.size());
    for(std::string p : output_param_vector) results[p] = temp;
    
    // Store the derivative evaluation counter
    //  It will show up as a whole vector with the same value at every time point
    //  Maybe there is a better way to do this?
    std::fill(temp.begin(), temp.end(), ncalls);
    results["ncalls"] = temp;
    
    // Store the data
    for(size_t i = 0; i < x_vec.size(); i++) {
        // Unpack the latest time and state from the calculation results
        boost::numeric::ublas::vector<double> current_state = x_vec[i];
        double current_time = times[i];
        // Get the corresponding parameter list
        update_varying_params(current_time);
        update_state_params(current_state);
        run_steady_state_modules();
        // Add the list to the results map
        for(size_t j = 0; j < output_param_vector.size(); j++) (results[output_param_vector[j]])[i] = parameters[output_param_vector[j]];
    }
    
    // Return the result map
    return results;
}

// For double time and std::vector state
std::unordered_map<std::string, std::vector<double>> System::get_results(const std::vector<std::vector<double>>& x_vec, const std::vector<double>& times) {
    // Make the result map
    std::unordered_map<std::string, std::vector<double>> results;
    
    // Initialize the parameter names
    std::vector<double> temp(x_vec.size());
    for(std::string p : output_param_vector) results[p] = temp;
    
    // Store the derivative evaluation counter
    //  It will show up as a whole vector with the same value at every time point
    //  Maybe there is a better way to do this?
    std::fill(temp.begin(), temp.end(), ncalls);
    results["ncalls"] = temp;
    
    // Store the data
    for(size_t i = 0; i < x_vec.size(); i++) {
        // Unpack the latest time and state from the calculation results
        std::vector<double> current_state = x_vec[i];
        double current_time = times[i];
        // Get the corresponding parameter list
        update_varying_params(current_time);
        update_state_params(current_state);
        run_steady_state_modules();
        // Add the list to the results map
        for(size_t j = 0; j < output_param_vector.size(); j++) (results[output_param_vector[j]])[i] = parameters[output_param_vector[j]];
    }
    
    // Return the result map
    return results;
}

// For integer time
void System::update_varying_params(int time_indx) {
    for(auto x : varying_ptrs) *(x.first) = (*(x.second))[time_indx];
}

// For double time
void System::update_varying_params(double time_indx) {
    // Find the two closest integers
    int t1 = (int)(time_indx + 0.5);
    int t2 = (t1 > time_indx) ? (t1 - 1) : (t1 + 1);
    // Make a linear interpolation
    for(auto x : varying_ptrs) *(x.first) = (*(x.second))[t1] + (time_indx - t1) * ((*(x.second))[t2] - (*(x.second))[t1]) / (t2 - t1);
}

template<class vector_type> void System::update_state_params(const vector_type& new_state) {
    for(size_t i = 0; i < new_state.size(); i++) *(state_ptrs[i].first) = new_state[i];
}

void System::run_steady_state_modules() {
    for(auto x : steady_state_ptrs) *x.second = 0.0;                                                        // Clear the module output map
    for(auto it = steady_state_modules.begin(); it != steady_state_modules.end(); ++it) {
        (*it)->run();                                                                                       // Run the module
        for(auto x : steady_state_ptrs) *x.first = *x.second;                                               // Store its output in the main parameter map
    }
}

template<class vector_type> void System::run_derivative_modules(vector_type& dxdt) {
    for(auto x : state_ptrs) *x.second = 0.0;                                                               // Reset the module output map
    std::fill(dxdt.begin(), dxdt.end(), 0);                                                                 // Reset the derivative vector
    for(auto it = derivative_modules.begin(); it != derivative_modules.end(); ++it) (*it)->run();           // Run the modules
    for(size_t i = 0; i < dxdt.size(); i++) dxdt[i] += *(state_ptrs[i].second)*(*timestep_ptr);             // Store the output in the derivative vector
}

void System::test_steady_state_modules() {
    // Identical to run_steady_state_modules except for a try-catch block
    for(auto x : steady_state_ptrs) *x.second = 0.0;
    for(auto it = steady_state_modules.begin(); it != steady_state_modules.end(); ++it) {
        try {(*it)->run();}
        catch (const std::exception& e) {
            throw std::logic_error(std::string("Steady state module '") + (*it)->get_name() + std::string("' generated an exception while calculating steady state parameters: ") + e.what() + std::string("\n"));
        }
        for(auto x : steady_state_ptrs) *x.first = *x.second;
    }
}

template<class vector_type> void System::test_derivative_modules(vector_type& dxdt) {
    // Identical to run_derivative_modules except for a try-catch block
    for(auto x : state_ptrs) *x.second = 0.0;
    std::fill(dxdt.begin(), dxdt.end(), 0);
    for(auto it = derivative_modules.begin(); it != derivative_modules.end(); ++it) {
        try {(*it)->run();}
        catch (const std::exception& e) {
            throw std::logic_error(std::string("Derivative module '") + (*it)->get_name() + std::string("' generated an exception while calculating time derivatives: ") + e.what() + std::string("\n"));
        }
    }
    for(size_t i = 0; i < dxdt.size(); i++) dxdt[i] += *(state_ptrs[i].second)*(*timestep_ptr);
}

template<class vector_type, class time_type> int System::speed_test(int n, const vector_type& x, vector_type& dxdt, const time_type& t) {
    // Run the system operator n times
    clock_t ct = clock();
    for(int i = 0; i < n; i++) operator()(x, dxdt, t);
    ct = clock() - ct;
    return (int)ct;
}

/*
template<class time_type> int System::speed_test(int n, const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::matrix<double>& jacobi, const time_type& t, boost::numeric::ublas::vector<double>& dfdt) {
    // Run the system operator n times
    clock_t ct = clock();
    for(int i = 0; i < n; i++) operator()(x, jacobi, t, dfdt);
    ct = clock() - ct;
    return (int)ct;
}
*/
