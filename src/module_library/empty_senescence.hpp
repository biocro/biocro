#ifndef EMPTY_SENESCENCE_H
#define EMPTY_SENESCENCE_H

#include "../modules.h"

class empty_senescence : public DerivModule {
	public:
		empty_senescence(const std::unordered_map<std::string, double>* /*input_parameters*/, std::unordered_map<std::string, double>* /*output_parameters*/) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("empty_senescence")
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Main operation
		void do_operation() const;
};

std::vector<std::string> empty_senescence::get_inputs() {
	return {
		// No inputs
	};
}

std::vector<std::string> empty_senescence::get_outputs() {
	return {
		// No outputs
	};
}

void empty_senescence::do_operation() const {
	// Don't do anything
}

#endif
