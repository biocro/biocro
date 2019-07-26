#ifndef TEST_MODULE
#define TEST_MODULE
#include "../modules.h"

class test_module : public IModule {
    public:
        test_module()
            : IModule("test_module",
                    std::vector<std::string> {},  // The first vector is state the module requires.
                    std::vector<std::string> {})  // The second vector is state the module creates or modifies.
            {}
    private:
        state_map do_operation (state_map const& s) const;
};

#endif
