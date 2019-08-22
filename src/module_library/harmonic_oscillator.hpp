#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "../modules.h"

class harmonic_oscillator : public DerivModule {
	public:
		harmonic_oscillator(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("harmonic_oscillator"),
			// Get pointers to input parameters
			mass_ip(get_ip(input_parameters, "mass")),
			sc_ip(get_ip(input_parameters, "spring_constant")),
			pos_ip(get_ip(input_parameters, "position")),
			vel_ip(get_ip(input_parameters, "velocity")),
			// Get pointers to output parameters
			pos_op(get_op(output_parameters, "position")),
			vel_op(get_op(output_parameters, "velocity"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* mass_ip;
		const double* sc_ip;
		const double* pos_ip;
		const double* vel_ip;
		// Pointers to output parameters
		double* pos_op;
		double* vel_op;
		void do_operation() const;
};

std::vector<std::string> harmonic_oscillator::get_inputs() {
	return {"mass", "spring_constant", "position", "velocity"};
}

std::vector<std::string> harmonic_oscillator::get_outputs() {
	return {"position", "velocity"};
}

void harmonic_oscillator::do_operation() const {
	update(pos_op, *vel_ip);
	update(vel_op, -1.0 * (*sc_ip) * (*pos_ip) / (*mass_ip));
}

class harmonic_energy : public SteadyModule {
	public:
		harmonic_energy(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("harmonic_energy"),
			// Get pointers to input parameters
			mass_ip(&((*input_parameters).at("mass"))),
			sc_ip(&((*input_parameters).at("spring_constant"))),
			pos_ip(&((*input_parameters).at("position"))),
			vel_ip(&((*input_parameters).at("velocity"))),
			// Get pointers to output parameters
			ke_op(&((*output_parameters).at("kinetic_energy"))),
			se_op(&((*output_parameters).at("spring_energy"))),
			te_op(&((*output_parameters).at("total_energy")))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* mass_ip;
		const double* sc_ip;
		const double* pos_ip;
		const double* vel_ip;
		// Pointers to output parameters
		double* ke_op;
		double* se_op;
		double* te_op;
		void do_operation() const;
};

std::vector<std::string> harmonic_energy::get_inputs() {
	return {"mass", "spring_constant", "position", "velocity"};
}

std::vector<std::string> harmonic_energy::get_outputs() {
	return {"kinetic_energy", "spring_energy", "total_energy"};
}

void harmonic_energy::do_operation() const {
	update(ke_op, 0.5 * (*mass_ip) * (*vel_ip) * (*vel_ip));
	update(se_op, 0.5 * (*sc_ip) * (*pos_ip) * (*pos_ip));
	update(te_op, *ke_op + *se_op);
}

#endif
