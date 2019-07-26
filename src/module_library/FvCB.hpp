#ifndef FVCB
#define FVCB
#include "../modules.h"

class FvCB : public IModule {
    // The Farquhar, von Caemmerer, Berry model.
    public:
        FvCB()
            : IModule("FvCB",
                    std::vector<std::string> {},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation (state_map const &s) const;
};

#endif
