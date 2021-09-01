#ifndef BIOCRO_SIMULATION_H
#define BIOCRO_SIMULATION_H

#include <vector>
#include "state_map.h"
#include "dynamical_system.h"
#include "ode_solver.h"
#include "ode_solver_library/ode_solver_factory.h"

// Class that represents a BioCro simulation
class biocro_simulation
{
   public:
    biocro_simulation(
        // parameters passed to dynamical_system constructor
        std::unordered_map<std::string, double> const& initial_values,
        std::unordered_map<std::string, double> const& parameters,
        std::unordered_map<std::string, std::vector<double>> const& drivers,
        std::vector<std::string> const& direct_module_names,
        std::vector<std::string> const& differential_module_names,
        // parameters passed to ode_solver_factory::create
        std::string ode_solver_name,
        double output_step_size,
        double adaptive_rel_error_tol,
        double adaptive_abs_error_tol,
        int adaptive_max_steps)
    {
        // Create the system
        sys = std::shared_ptr<dynamical_system>(
            new dynamical_system(initial_values, parameters,
                       drivers, direct_module_names,
                       differential_module_names)
        );

        // Create the ode_solver that will be used to solve the system
        system_solver = ode_solver_factory::create(
            ode_solver_name, output_step_size, adaptive_rel_error_tol,
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
                  "\node_ODE solver description:\n" +
                      system_solver->generate_info_report() +
                  "\n\nThe ODE solver reports the following:\n" +
                      system_solver->generate_integrate_report() +
                  "\nThe dynamical system reports the following:\n" +
                      sys->generate_usage_report() +
                  "\n\n";
        return report;
    }

   private:
    std::shared_ptr<dynamical_system> sys;
    std::unique_ptr<ode_solver> system_solver;
};

#endif
