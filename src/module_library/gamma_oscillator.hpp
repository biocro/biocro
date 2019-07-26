#ifndef GAMMA_OSCILLATOR
#define GAMMA_OSCILLATOR
#include "../modules.h"

class gamma_oscillator : public IModule {
    public:
        gamma_oscillator()
            : IModule("gamma_oscillator",
                    std::vector<std::string> {},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const;
};

#endif
