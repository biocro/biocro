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
	bool verbose,
	void (*print_msg) (char const *format, ...) = void_printf);

// Gro functions for Euler integration
std::unordered_map<std::string, std::vector<double>> Gro_euler(
	std::unordered_map<std::string, double> const &initial_state,
	std::unordered_map<std::string, double> const &invariant_parameters,
	std::unordered_map<std::string, std::vector<double>> const &varying_parameters,
	std::vector<std::string> const &steady_state_module_names,
	std::vector<std::string> const &derivative_module_names,
	bool verbose,
	void (*print_msg) (char const *format, ...) = void_printf);

std::unordered_map<std::string, std::vector<double>> Gro_euler_solve(
	std::shared_ptr<System> sys,
	bool verbose,
	void (*print_msg) (char const *format, ...) = void_printf);

// Gro functions for Rosenbrock integration
std::unordered_map<std::string, std::vector<double>> Gro_rsnbrk(
	std::unordered_map<std::string, double> const &initial_state,
	std::unordered_map<std::string, double> const &invariant_parameters,
	std::unordered_map<std::string, std::vector<double>> const &varying_parameters,
	std::vector<std::string> const &steady_state_module_names,
	std::vector<std::string> const &derivative_module_names,
	bool verbose,
	void (*print_msg) (char const *format, ...) = void_printf);

std::unordered_map<std::string, std::vector<double>> Gro_rsnbrk_solve(
	std::shared_ptr<System> sys,
	bool verbose,
	void (*print_msg) (char const *format, ...) = void_printf);

#endif
