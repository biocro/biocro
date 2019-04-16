#ifndef PARTITIONING_COEFFICIENT_SELECTOR
#define PARTITIONING_COEFFICIENT_SELECTOR
#include "../modules.h"

class partitioning_coefficient_selector : public IModule {
    public:
        partitioning_coefficient_selector()
            : IModule("partitioning_coefficient_selector",
                    std::vector<std::string> { "kStem1", "kLeaf1", "kRoot1", "kRhizome1", "kGrain1",
                                               "kStem2", "kLeaf2", "kRoot2", "kRhizome2", "kGrain2",
                                               "kStem3", "kLeaf3", "kRoot3", "kRhizome3", "kGrain3",
                                               "kStem4", "kLeaf4", "kRoot4", "kRhizome4", "kGrain4",
                                               "kStem5", "kLeaf5", "kRoot5", "kRhizome5", "kGrain5",
                                               "kStem6", "kLeaf6", "kRoot6", "kRhizome6", "kGrain6" },
                       std::vector<std::string> {})
            {}
    private:
        mutable std::vector<double> coefficients;
        mutable std::vector<double> thermal_periods;

        state_map do_operation(state_map const &s) const
        {
            if (coefficients.size() == 0) {
                coefficients.insert(coefficients.begin(), {s.at("kStem1"), s.at("kLeaf1"), s.at("kRoot1"), s.at("kRhizome1"), s.at("kGrain1"),
                                        s.at("kStem2"), s.at("kLeaf2"), s.at("kRoot2"), s.at("kRhizome2"), s.at("kGrain2"),
                                        s.at("kStem3"), s.at("kLeaf3"), s.at("kRoot3"), s.at("kRhizome3"), s.at("kGrain3"),
                                        s.at("kStem4"), s.at("kLeaf4"), s.at("kRoot4"), s.at("kRhizome4"), s.at("kGrain4"),
                                        s.at("kStem5"), s.at("kLeaf5"), s.at("kRoot5"), s.at("kRhizome5"), s.at("kGrain5"),
                                        s.at("kStem6"), s.at("kLeaf6"), s.at("kRoot6"), s.at("kRhizome6"), s.at("kGrain6")});
                thermal_periods.insert(thermal_periods.begin(), {s.at("tp1"), s.at("tp2"), s.at("tp3"), s.at("tp4"), s.at("tp5"), s.at("tp6")});
            }

            // Find the interval that contains TTc.
            const int highest_interval = thermal_periods.size() - 1;
            int interval = highest_interval;
            for ( ; interval >= 0; --interval) {
                if (s.at("TTc") > thermal_periods[interval]) break;
            }

            constexpr int n_tissues = 5;
            int offset = std::min(interval + 1, highest_interval) * n_tissues;  // The thermal period variables mark the end of an interval. If the thermal time is larger than the last defined interval, then use the values in the last defined interval.

            state_map result {{"kStem",    coefficients.at(0 + offset)},
                              {"kLeaf",    coefficients.at(1 + offset)},
                              {"kRoot",    coefficients.at(2 + offset)},
                              {"kRhizome", coefficients.at(3 + offset)},
                              {"kGrain",   coefficients.at(4 + offset)}};

            return result;
        }
};

#endif
