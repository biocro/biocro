#ifndef m_penman_monteith_transpiration
#define m_penman_monteith_transpiration
#include "../modules.h"

class penman_monteith_transpiration : public IModule {
    public:
        penman_monteith_transpiration()
            : IModule("penman_monteith_transpiration",
                    std::vector<std::string> {},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation (state_map const &s) const;
};

#endif
