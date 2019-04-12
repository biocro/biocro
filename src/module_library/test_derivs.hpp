#ifndef TEST_DERIVS
#define TEST_DERIVS
#include "../modules.h"

class test_derivs : public IModule {
    public:
        test_derivs()
            : IModule("test_derivs",
                    std::vector<std::string> {"LeafArea", "PAR"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const;
};

#endif
