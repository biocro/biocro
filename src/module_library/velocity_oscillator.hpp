#ifndef VELOCITY_OSCILLATOR
#define VELOCITY_OSCILLATOR
#include "../modules.h"

class velocity_oscillator : public IModule {
    public:
        velocity_oscillator()
            : IModule("velocity_oscillator",
                    std::vector<std::string> {"position"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const;
};

#endif
