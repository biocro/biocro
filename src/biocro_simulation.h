#ifndef BIOCRO_SIMULATION_H
#define BIOCRO_SIMULATION_H

#include <vector>
#include "state_map.h"
#include "system.h"
#include "integrator.h"
#include "integrator_library/integrator_factory.h"

// Class that represents a BioCro simulation
class biocro_simulation
{
   public:
    biocro_simulation(
        // parameters passed to System constructor
        std::unordered_map<std::string, double> const& initial_values,
        std::unordered_map<std::string, double> const& parameters,
        std::unordered_map<std::string, std::vector<double>> const& drivers,
        std::vector<std::string> const& steady_state_module_names,
        std::vector<std::string> const& derivative_module_names,
        // parameters passed to integrator_factory::create
        std::string integrator_name,
        double output_step_size,
        double adaptive_rel_error_tol,
        double adaptive_abs_error_tol,
        int adaptive_max_steps)
    {
        // Create the system
        sys = std::shared_ptr<System>(
            new System(initial_values, parameters,
                       drivers, steady_state_module_names,
                       derivative_module_names)
        );

        // Create the integrator that will be used to solve the system
        system_solver = integrator_factory::create(
            integrator_name, output_step_size, adaptive_rel_error_tol,
            adaptive_abs_error_tol, adaptive_max_steps
        );
    }

    std::unordered_map<std::string, std::vector<double>> run_simulation() {
        return system_solver->integrate(sys);
    }

    std::string generate_report() const
    {
        std::string report;
        report += "\nSystem startup information:\n" +
                      sys->generate_startup_report() +
                  "\nIntegrator description:\n" +
                      system_solver->generate_info_report() +
                  "\n\nThe integrator reports the following:\n" +
                      system_solver->generate_integrate_report() +
                  "\nThe system reports the following:\n" +
                      sys->generate_usage_report() +
                  "\n\n";
        return report;
    }

   private:
    std::shared_ptr<System> sys;
    std::unique_ptr<integrator> system_solver;
};

#endif
