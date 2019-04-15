#ifndef m_linear_vmax_from_leaf_n
#define m_linear_vmax_from_leaf_n
#include "../modules.h"

class linear_vmax_from_leaf_n : public IModule {
    public:
        linear_vmax_from_leaf_n()
            : IModule("linear_vmax_from_leaf_n",
                    std::vector<std::string> {"LeafN", "LeafN_0", "vmax_n_intercept", "vmax_n_slope"},
                    std::vector<std::string> {})
            {}
    private:
        state_map do_operation(state_map const &s) const
        {
            state_map result { {"vmax", (s.at("LeafN_0") - s.at("LeafN")) * s.at("vmax_n_intercept") + s.at("vmax1") } };
            return result;
        }
};

#endif
