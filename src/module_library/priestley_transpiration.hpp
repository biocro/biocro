#ifndef m_priestley_transpiration
#define m_priestley_transpiration
#include "../modules.h"

class priestley_transpiration : public IModule {
    public:
        priestley_transpiration()
            : IModule("priestley_transpiration",
                    std::vector<std::string> {},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation (state_map const &s) const;
};

#endif
