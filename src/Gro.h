#ifndef GRO_H
#define GRO_H

#include <vector>
#include "state_map.h"
#include "system.h"

// General Gro function that automatically uses the Rosenbrock method when possible
std::unordered_map<std::string, std::vector<double>> Gro(
	std::unordered_map<std::string, double> const &initial_state,
	std::unordered_map<std::string, double> const &invariant_parameters,
	std::unordered_map<std::string, std::vector<double>> const &varying_parameters,
	std::vector<std::string> const &steady_state_module_names,
	std::vector<std::string> const &derivative_module_names,
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps,
	bool verbose,
	void (*print_msg) (char const *format, ...) = void_printf);

// Gro functions for Euler integration using a home-made integrator
std::unordered_map<std::string, std::vector<double>> Gro_euler(
	std::unordered_map<std::string, double> const &initial_state,
	std::unordered_map<std::string, double> const &invariant_parameters,
	std::unordered_map<std::string, std::vector<double>> const &varying_parameters,
	std::vector<std::string> const &steady_state_module_names,
	std::vector<std::string> const &derivative_module_names,
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps,
	bool verbose,
	void (*print_msg) (char const *format, ...) = void_printf);

std::unordered_map<std::string, std::vector<double>> Gro_euler_solve(
	std::shared_ptr<System> sys,
	bool verbose,
	void (*print_msg) (char const *format, ...) = void_printf);

// Gro functions for Euler integration using the ODEINT integrator
std::unordered_map<std::string, std::vector<double>> Gro_euler_odeint(
	std::unordered_map<std::string, double> const &initial_state,
	std::unordered_map<std::string, double> const &invariant_parameters,
	std::unordered_map<std::string, std::vector<double>> const &varying_parameters,
	std::vector<std::string> const &steady_state_module_names,
	std::vector<std::string> const &derivative_module_names,
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps,
	bool verbose,
	void (*print_msg) (char const *format, ...) = void_printf);

std::unordered_map<std::string, std::vector<double>> Gro_euler_odeint_solve(
	std::shared_ptr<System> sys,
    double output_step_size,
	bool verbose,
	void (*print_msg) (char const *format, ...) = void_printf);

// Gro functions for Rosenbrock integration
std::unordered_map<std::string, std::vector<double>> Gro_rsnbrk(
	std::unordered_map<std::string, double> const &initial_state,
	std::unordered_map<std::string, double> const &invariant_parameters,
	std::unordered_map<std::string, std::vector<double>> const &varying_parameters,
	std::vector<std::string> const &steady_state_module_names,
	std::vector<std::string> const &derivative_module_names,
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps,
	bool verbose,
	void (*print_msg) (char const *format, ...) = void_printf);

std::unordered_map<std::string, std::vector<double>> Gro_rsnbrk_solve(
	std::shared_ptr<System> sys,
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps,
	bool verbose,
	void (*print_msg) (char const *format, ...) = void_printf);

// Gro functions for 4th order Runge-Kutta integration
std::unordered_map<std::string, std::vector<double>> Gro_rk4(
	std::unordered_map<std::string, double> const &initial_state,
	std::unordered_map<std::string, double> const &invariant_parameters,
	std::unordered_map<std::string, std::vector<double>> const &varying_parameters,
	std::vector<std::string> const &steady_state_module_names,
	std::vector<std::string> const &derivative_module_names,
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps,
	bool verbose,
	void (*print_msg) (char const *format, ...) = void_printf);

std::unordered_map<std::string, std::vector<double>> Gro_rk4_solve(
	std::shared_ptr<System> sys,
    double output_step_size,
	bool verbose,
	void (*print_msg) (char const *format, ...) = void_printf);

// Gro functions for 5th order Runge-Kutta integration with 4th order error estimation
std::unordered_map<std::string, std::vector<double>> Gro_rkck54(
	std::unordered_map<std::string, double> const &initial_state,
	std::unordered_map<std::string, double> const &invariant_parameters,
	std::unordered_map<std::string, std::vector<double>> const &varying_parameters,
	std::vector<std::string> const &steady_state_module_names,
	std::vector<std::string> const &derivative_module_names,
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps,
	bool verbose,
	void (*print_msg) (char const *format, ...) = void_printf);

std::unordered_map<std::string, std::vector<double>> Gro_rkck54_solve(
	std::shared_ptr<System> sys,
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps,
	bool verbose,
	void (*print_msg) (char const *format, ...) = void_printf);

#endif
