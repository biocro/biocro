#ifndef MODULES_H
#define MODULES_H

#include <map>
#include <string>
#include <vector>
#include <memory>
#include "BioCro.h"
#include "c3photo.h"

using std::vector;
using std::string;
using std::map;

typedef map<string, double> state_map;
typedef map<string, vector<double>> state_vector_map;

class IModule {
    public:
        IModule(const vector<string> required_state, const vector<string> modified_state) :
            _required_state(required_state),
            _modified_state(modified_state)
    {}
        vector<string> list_required_state();
        vector<string> list_modified_state();
        map<string, double> run (map<string, double> state);
        vector<string> state_requirements_are_met(map<string, double> state);
        virtual ~IModule() = 0;  // Make the destructor a pure virtual function so that no objects can be made directly from this class.
    private:
        const vector<string> _required_state;
        const vector<string> _modified_state; 
        virtual map<string, double> do_operation(map<string, double> state) = 0;
        bool requirements_are_met;
};

inline IModule::~IModule() {};  // A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header.

class Leaf_photosynthesis_module : public IModule {
    public:
        Leaf_photosynthesis_module(const vector<string> required_state, const vector<string> modified_state)
            : IModule(required_state, modified_state)
        {}
        virtual struct c3_str assimilation(struct Model_state) = 0;
};

class c3_leaf : public Leaf_photosynthesis_module {
    public:
        c3_leaf()
            : Leaf_photosynthesis_module(vector<string> {"Qp", "Tleaf"} , vector<string> {})
        {} 
        double run (map<string, double> s);
        struct c3_str assimilation(map<string, double> s);
};

class ICanopy_photosynthesis_module : public IModule {
    public:
        ICanopy_photosynthesis_module(const vector<string> required_state, const vector<string> modified_state)
            : IModule(required_state, modified_state)
        {}
};

class c4_canopy : public ICanopy_photosynthesis_module {
    public:
        c4_canopy()
            : ICanopy_photosynthesis_module(vector<string> {"lai", "doy", "hour", "solar", "temp",
                    "rh", "windspeed", "lat", "nlayers", "vmax1",
                    "alpha1", "kparm", "beta", "Rd", "Catm",
                    "b0", "b1", "theta", "kd", "chil",
                    "heightf", "LeafN", "kpLN", "lnb0", "lnb1",
                    "lnfun", "upperT", "lowerT", "leafwidth",
                    "et_equation", "StomataWS", "ws",
                    "ileafN", "kln", "Vmaxb0", "Vmaxb1", "alphab1",
                    "Rdb1", "Rdb0", "kpLN"}, 
                    vector<string> {})
        {}
    private:
        virtual map<string, double> do_operation (map<string, double> s);
};

class c3_canopy : public ICanopy_photosynthesis_module {
    public:
        c3_canopy()
            : ICanopy_photosynthesis_module(vector<string> {"lai", "doy", "hour", "solarr", "temp",
                   "rh", "windspeed", "lat", "nlayers", "vmax",
                   "jmax", "rd", "catm", "o2", "b0",
                   "b1", "theta", "kd", "heightf", "leafn",
                   "kpln", "lnb0", "lnb1", "lnfun", "stomataws",
                   "ws"},
                   vector<string> {})
        {}
    private:
        virtual map<string, double> do_operation (map<string, double> s);
};


state_map combine_state(state_map state, state_map invariant_parameters, state_vector_map varying_parameters, int timestep);

map<string, vector<double>> Gro(
        map<string, double> initial_state,
        map<string, double> invariant_parameters,
        map<string, vector<double>> varying_parameters,
        std::unique_ptr<IModule> const &canopy_photosynthesis_module,
		double (*leaf_n_limitation)(state_map model_state));

double biomass_leaf_nitrogen_limitation(state_map model_state);

void output_map(map<string, double> m);

state_map replace_state(state_map state, state_map newstate);

# endif

