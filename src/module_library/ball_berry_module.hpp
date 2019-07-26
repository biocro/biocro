#ifndef BALL_BERRY_MODULE
#define BALL_BERRY_MODULE
#include "../modules.h"

class ball_berry_module : public IModule {
    public:
        ball_berry_module()
            : IModule("ball_berry_module",
                    std::vector<std::string> {},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation (state_map const &s) const;
};

#endif
