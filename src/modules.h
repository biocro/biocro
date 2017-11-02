#ifndef MODULES_H
#define MODULES_H

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include "BioCro.h"
#include "c3photo.h"

using std::vector;
using std::string;
using std::unordered_map;

typedef unordered_map<string, double> state_map;
typedef unordered_map<string, vector<double>> state_vector_map;

class IModule {
    public:
        IModule(vector<string> const &required_state, vector<string> const &modified_state) :
            _required_state(required_state),
            _modified_state(modified_state)
        {
            requirements_are_met = false;
        }
        vector<string> list_required_state() const;
        vector<string> list_modified_state() const;
        state_map run (state_map const &state) const;
        state_map run (state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &parameters) const;
        vector<string> state_requirements_are_met(state_map const &state) const;
        virtual ~IModule() = 0;  // Make the destructor a pure virtual function so that no objects can be made directly from this class.
    private:
        vector<string> const _required_state;
        vector<string> const _modified_state; 
        virtual state_map do_operation(state_map const &state) const {state_map derivs; return derivs;};
        virtual state_map do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &parameters) const;
        bool requirements_are_met;
};

inline IModule::~IModule() {}  // A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header.

class ICanopy_photosynthesis_module : public IModule {
    public:
        ICanopy_photosynthesis_module(const vector<string> &required_state, const vector<string> &modified_state)
            : IModule(required_state, modified_state)
        {}
};

class c4_canopy : public ICanopy_photosynthesis_module {
    public:
        c4_canopy()
            : ICanopy_photosynthesis_module(vector<string> {"lai", "doy", "hour", "solar", "temp",
                    "rh", "windspeed", "lat", "nlayers", "vmax",
                    "alpha", "kparm", "beta", "Rd", "Catm",
                    "b0", "b1", "theta", "kd", "chil",
                    "heightf", "LeafN", "kpLN", "lnb0", "lnb1",
                    "lnfun", "upperT", "lowerT", "leafwidth",
                    "et_equation", "StomataWS", "water_stress_approach",
                    "nileafn", "kln", "nvmaxb0", "nvmaxb1", "alphab1",
                    "nRdb1", "nRdb0", "kpLN"}, 
                    vector<string> {})
        {}
    private:
        virtual state_map do_operation (state_map const &s) const;
};

class c3_canopy : public ICanopy_photosynthesis_module {
    public:
        c3_canopy()
            : ICanopy_photosynthesis_module(vector<string> {"lai", "doy", "hour", "solar", "temp",
                    "rh", "windspeed", "lat", "nlayers", "vmax",
                    "jmax", "Rd", "Catm", "O2", "b0",
                    "b1", "theta", "kd", "heightf", "LeafN",
                    "kpLN", "lnb0", "lnb1", "lnfun", "chil", "StomataWS",
                    "water_stress_approach", "electrons_per_carboxylation", "electrons_per_oxygenation"},
                    vector<string> {})
        {}
    private:
        virtual state_map do_operation (state_map const &s) const;
};

class ISoil_evaporation_module : public IModule {
    public:
        ISoil_evaporation_module(const vector<string> &required_state, const vector<string> &modified_state)
            : IModule(required_state, modified_state)
        {}
};

class one_layer_soil_profile : public ISoil_evaporation_module {
    public:
        one_layer_soil_profile()
            : ISoil_evaporation_module(vector<string> {"lai", "temp", "solar", "waterCont",
                    "FieldC", "WiltP", "windspeed", "rh", "rsec",
                    "CanopyT", "precip", "soilDepth", "phi1", "phi2",
                    "soilType", "wsFun", "StomataWS", "LeafWS"},
                    vector<string> {})
        {}
    private:
        virtual state_map do_operation(state_map const &s) const;
};

class two_layer_soil_profile : public ISoil_evaporation_module {
    public:
        two_layer_soil_profile()
            : ISoil_evaporation_module(vector<string> {"precip", "CanopyT", "cws1", "cws2", "soilDepth1",
                    "soilDepth2", "soilDepth3", "FieldC", "WiltP", "phi1",
                    "phi2", "soilType" /* Instead of soTexS */, "wsFun", "Root", "lai",
                    "temp", "solar", "windspeed", "rh", "hydrDist",
                    "rfl", "rsec", "rsdf", "StomataWS", "LeafWS"},
                    vector<string> {})
        {}
    private:
        virtual state_map do_operation(state_map const &s) const;
};

class ISenescence_module : public IModule {
    public:
        ISenescence_module(const vector<string> &required_state, const vector<string> &modified_state)
            : IModule(required_state, modified_state)
        {}
};

class thermal_time_senescence : public ISenescence_module {
    public:
        thermal_time_senescence()
            : ISenescence_module(vector<string> {"TTc", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                "leaf_senescence_index", "stem_senescence_index", "root_senescence_index", "rhizome_senescence_index",
                "mrc1", "mrc2"},
                vector<string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

class thermal_time_and_frost_senescence : public ISenescence_module {
    public:
        thermal_time_and_frost_senescence()
            : ISenescence_module(vector<string> {"TTc", "leafdeathrate", "lat", "doy", "Tfrostlow", "Tfrosthigh", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                "leaf_senescence_index", "stem_senescence_index", "root_senescence_index", "rhizome_senescence_index"},
                vector<string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

class IGrowth_module : public IModule {
    public:
        IGrowth_module(const vector<string> &required_state, const vector<string> &modified_state)
            : IModule(required_state, modified_state)
        {}
};

class utilization_senescence: public ISenescence_module {
    public:
        utilization_senescence()
            : ISenescence_module(vector<string> {"TTc", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                "Leaf", "Stem", "Root", "Rhizome", "Grain",
                "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                "KmLeaf", "KmStem", "KmRoot", "KmRhizome", "KmGrain",
                "resistance_leaf_to_stem",
                "resistance_stem_to_grain",
                "resistance_stem_to_root",
                "resistance_stem_to_rhizome"},
                vector<string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

class utilization_growth_module : public IGrowth_module {
    public:
        utilization_growth_module()
            : IGrowth_module(vector<string> {"CanopyA",
                "Leaf", "Stem", "Root", "Rhizome", "Grain",
                "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                "KmLeaf", "KmStem", "KmRoot", "KmRhizome", "KmGrain",
                "resistance_leaf_to_stem",
                "resistance_stem_to_grain",
                "resistance_stem_to_root",
                "resistance_stem_to_rhizome"},
                vector<string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
        virtual state_map do_operation(state_map const &s) const;
};

class utilization_growth_and_senescence_module: public ISenescence_module {
    public:
        utilization_growth_and_senescence_module()
            : ISenescence_module(vector<string> {"TTc", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                "Leaf", "Stem", "Root", "Rhizome", "Grain",
                "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                "KmLeaf", "KmStem", "KmRoot", "KmRhizome", "KmGrain",
                "resistance_leaf_to_stem",
                "resistance_stem_to_grain",
                "resistance_stem_to_root",
                "resistance_stem_to_rhizome",
                "rate_constant_leaf", "rate_constant_stem", "rate_constant_root", "rate_constant_rhizome",
                "remobilization_fraction", "grain_TTc"},
                vector<string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

class utilization_growth_flowering_module: public ISenescence_module {
    public:
        utilization_growth_flowering_module()
            : ISenescence_module(vector<string> {"TTc", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                "Leaf", "Stem", "Root", "Rhizome", "Grain",
                "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                "KmLeaf", "KmStem", "KmRoot", "KmRhizome", "KmGrain",
                "resistance_leaf_to_stem",
                "resistance_stem_to_grain",
                "resistance_stem_to_root",
                "resistance_stem_to_rhizome",
                "GI_on", "GI_off", "FKF1_on", "FKF1_off", "CDF_on", "CDF_off", "FT_on", "FT_off", "FT_inhibition", "FKF1_timing", "CDF_timing", "dawn"},
                vector<string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

class empty_senescence: public ISenescence_module {
    public:
        empty_senescence()
            : ISenescence_module(vector<string> {},
                vector<string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};


class partitioning_growth_module : public IGrowth_module {
    public:
        partitioning_growth_module()
            : IGrowth_module(vector<string> {"TTc", "LeafWS", "temp", "CanopyA",
                "Leaf", "Stem", "Root", "Rhizome", "Grain",
                "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                "mrc1", "mrc2"},
                vector<string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

class no_leaf_resp_partitioning_growth_module : public IGrowth_module {
    public:
        no_leaf_resp_partitioning_growth_module()
            : IGrowth_module(vector<string> {"TTc", "LeafWS", "temp", "CanopyA",
                "Leaf", "Stem", "Root", "Rhizome", "Grain",
                "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                "mrc1", "mrc2"},
                vector<string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

state_map combine_state(state_map const &state_a, state_map const &state_b);

state_vector_map Gro(
        state_map const &initial_state,
        state_map const &invariant_parameters,
        state_vector_map const &varying_parameters,
        std::unique_ptr<IModule> const &canopy_photosynthesis_module,
        std::unique_ptr<IModule> const &soil_evaporation_module,
        std::unique_ptr<IModule> const &growth_module,
        std::unique_ptr<IModule> const &senescence_module,
		double (*leaf_n_limitation)(state_map const &model_state));

double biomass_leaf_nitrogen_limitation(state_map const &state);
double thermal_leaf_nitrogen_limitation(state_map const &state);

void output_map(state_map const &m);

state_map replace_state(state_map const &state, state_map const &newstate);

state_map update_state(state_map const &state, state_map const &change_in_state);

state_vector_map allocate_state(state_map const &m, size_t n);

void append_state_to_vector(state_map const &state, state_vector_map &state_vector);

std::unique_ptr<IModule> make_module(string const &module_name);

state_map at(state_vector_map const &vector_map, vector<double>::size_type const n);

state_map& operator+=(state_map &lhs, state_map const &rhs);

string join_string_vector(vector<string> const &state_keys);

vector<string> keys(state_map const &state);

vector<string> keys(state_vector_map const &state);

bool any_key_is_duplicated(vector<vector<string>> const &keys);

# endif

