#ifndef EMPTY_SENESCENCE_H
#define EMPTY_SENESCENCE_H

#include "../state_map.h"
#include "../modules.h"

class empty_senescence : public differential_module {
	public:
		empty_senescence(state_map const& /*input_quantities*/, state_map* /*output_quantities*/) :
			// Define basic module properties by passing its name to its parent class
			differential_module("empty_senescence")
		{}
		static string_vector get_inputs();
		static string_vector get_outputs();
	private:
		// Main operation
		void do_operation() const;
};

string_vector empty_senescence::get_inputs() {
	return {
		// No inputs
	};
}

string_vector empty_senescence::get_outputs() {
	return {
		// No outputs
	};
}

void empty_senescence::do_operation() const {
	// Don't do anything
}

#endif
