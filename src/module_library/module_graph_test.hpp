#include "../modules.h"

class Module_1: public IModule {
    public:
        Module_1()
            : IModule ("Module_1",
                    std::vector<std::string> {},
                    std::vector<std::string> {"A", "B"})
        {}
    private:
        state_map do_operation (state_map const &s) const {
            return { {"A", 0}, {"B", 0} };
        };
};


class Module_2: public IModule {
    public:
        Module_2()
            : IModule ("Module_2",
                    std::vector<std::string> {"B"},
                    std::vector<std::string> {"C", "D"})
        {}
    private:
        state_map do_operation (state_map const &s) const {
            return { {"C", 0}, {"D", 0} };
        };
};


class Module_3: public IModule {
    public:
        Module_3()
            : IModule ("Module_3",
                    std::vector<std::string> {"A", "C"},
                    std::vector<std::string> {"E"})
        {}
    private:
        state_map do_operation (state_map const &s) const {
            return { {"E", 0} };
        };
};
