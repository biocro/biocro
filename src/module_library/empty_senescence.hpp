#ifndef m_empty_senescence
#define m_empty_senescence
#include "../modules.h"

class empty_senescence: public IModule {
    public:
        empty_senescence()
            : IModule("empty_senescence",
                    std::vector<std::string> {},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

#endif
