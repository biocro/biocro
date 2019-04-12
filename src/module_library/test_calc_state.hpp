#ifndef TEST_CALC_STATE
#define TEST_CALC_STATE
#include "../modules.h"

class test_calc_state : public IModule {
    public:
        test_calc_state()
            : IModule("test_calc_state",
                    std::vector<std::string> {"LeafArea", "parameter"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const;
};

#endif
