#ifndef CLOCK_TESTING_CALC_H
#define CLOCK_TESTING_CALC_H

#include "../module.hpp"

class clock_testing_calc : public SteadyModule {
	public:
		clock_testing_calc(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("clock_testing_calc"),
			// Get pointers to input parameters
			x_ip(get_ip(input_parameters, "x")),
			v_ip(get_ip(input_parameters, "v")),
			E_ip(get_ip(input_parameters, "E")),
			theta_ip(get_ip(input_parameters, "theta")),
			time_ip(get_ip(input_parameters, "time")),
			T_ip(get_ip(input_parameters, "T")),
			A_ip(get_ip(input_parameters, "A")),
			F_start_ip(get_ip(input_parameters, "F_start")),
			F_end_ip(get_ip(input_parameters, "F_end")),
			max_F_per_ip(get_ip(input_parameters, "max_F_per")),
			// Get pointers to output parameters
			F_op(get_op(output_parameters, "F")),
			ref_x_op(get_op(output_parameters, "ref_x")),
			ref_v_op(get_op(output_parameters, "ref_v")),
			F_per_op(get_op(output_parameters, "F_per")),
			F_time_op(get_op(output_parameters, "F_time")),
			phase_op(get_op(output_parameters, "phase")),
			energy_op(get_op(output_parameters, "energy")),
			theta_mod_op(get_op(output_parameters, "theta_mod")),
			N_per_op(get_op(output_parameters, "N_per")),
			N_time_op(get_op(output_parameters, "N_time"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* x_ip;
		const double* v_ip;
		const double* E_ip;
		const double* theta_ip;
		const double* time_ip;
		const double* T_ip;
		const double* A_ip;
		const double* F_start_ip;
		const double* F_end_ip;
		const double* max_F_per_ip;
		// Pointers to output parameters
		double* F_op;
		double* ref_x_op;
		double* ref_v_op;
		double* F_per_op;
		double* F_time_op;
		double* phase_op;
		double* energy_op;
		double* theta_mod_op;
		double* N_per_op;
		double* N_time_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> clock_testing_calc::get_inputs() {
	return {
		"x",
		"v",
		"E",
		"theta",
		"time",
		"T",
		"A",
		"F_start",
		"F_end",
		"max_F_per"
	};
}

std::vector<std::string> clock_testing_calc::get_outputs() {
	return {
		"F",
		"ref_x",
		"ref_v",
		"F_per",
		"F_time",
		"phase",
		"energy",
		"theta_mod",
		"N_per",
		"N_time"
	};
}

void clock_testing_calc::do_operation() const {
	//////////////////////////////////////////
	// Collect inputs and make calculations //
	//////////////////////////////////////////
	
	// Get the current reference oscillator state
	double E = *E_ip;
	double theta = *theta_ip;
	
	// Get the current oscillator state
	double x = *x_ip;
	double v = *v_ip;
	
	// Get the current time
	double time = *time_ip;
	
	// Get the driving force info
	double T = *T_ip;
	double A = *A_ip;
	double F_start = *F_start_ip;
	double F_end = *F_end_ip;
	double max_F_per = *max_F_per_ip;
	
	// Get the current period (of the natural cycle)
	int N_per = floor(time / (2 * 3.14159265358979323846));
	
	// Get the current time modulo the natural period
	double N_time = time - N_per * 2 * 3.14159265358979323846;
	
	// Calulate theta_mod
	double theta_mod = fmod(theta, 2 * 3.14159265358979323846);
	//if (theta_mod < -3.14159265358979323846) theta_mod += 2 * 3.14159265358979323846;
	if(theta_mod < 0) theta_mod += 2 * 3.14159265358979323846;
	
	// Get the current period (of the force)
	int F_per = floor(time / T);
	
	// Get the current time modulo the force's period
	double F_time = fmod(time, T);
	
	// Calculate the magnitude of the force
	double F = 0;
	//if(F_per <= max_F_per && F_time >= F_start && F_time < F_end) F = A;	// Apply a periodic force for a certain number of periods
	if(N_per == max_F_per && N_time >= (F_start - 1e-8) && N_time < (F_end + 1e-8)) {
		F = A;	// Apply a force once at a particular time, determined by the phase of the reference oscillator (theta)
		std::cout << "Applying force!\n";
	}
	
	//////////////////////////////////////
	// Update the output parameter list //
	//////////////////////////////////////
	
	update(F_op, F);
	update(ref_x_op, sqrt(E) * cos(theta));
	update(ref_v_op, sqrt(E) * sin(theta));
	update(F_per_op, F_per);
	update(F_time_op, (time - T * F_per) / T);
	update(phase_op, atan2(v, x));
	update(energy_op, x*x + v*v);
	update(theta_mod_op, theta_mod);
	update(N_per_op, N_per);
	update(N_time_op, N_time);
}

#endif
