#ifndef POSITION_OSCILLATOR
#define POSITION_OSCILLATOR
#include "../modules.h"

class position_oscillator : public IModule {
    public:
        position_oscillator()
            : IModule("position_oscillator",
                    std::vector<std::string> {"velocity"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const;
};

#endif
