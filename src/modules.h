#ifndef MODULES_H
#define MODULES_H

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include "BioCro.h"
#include "c3photo.h"
#include "state_map.h"

class IModule {
    public:
        IModule(std::vector<std::string> const &required_state, std::vector<std::string> const &modified_state) :
            _required_state(required_state),
            _modified_state(modified_state)
        {
            requirements_are_met = false;
        }
        std::vector<std::string> list_required_state() const;
        std::vector<std::string> list_modified_state() const;
        state_map run (state_map const &state) const;
        state_map run (state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &parameters) const;
        std::vector<std::string> state_requirements_are_met(state_map const &state) const;
        virtual ~IModule() = 0;  // Make the destructor a pure virtual function so that no objects can be made directly from this class.
    private:
        std::vector<std::string> const _required_state;
        std::vector<std::string> const _modified_state; 
        virtual state_map do_operation(state_map const &state) const {throw std::logic_error("This module cannot call run().\n"); state_map derivs; return derivs;};
        virtual state_map do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &parameters) const;
        bool requirements_are_met;
};

inline IModule::~IModule() {}  // A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header.

class biomass_leaf_n_limitation : public IModule {
    public:
        biomass_leaf_n_limitation()
            : IModule(std::vector<std::string> {"LeafN_0", "Leaf", "Stem", "kln"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation (state_map const &s) const;

};

class ICanopy_photosynthesis_module : public IModule {
    public:
        ICanopy_photosynthesis_module(const std::vector<std::string> &required_state, const std::vector<std::string> &modified_state)
            : IModule(required_state, modified_state)
        {}
};

class c4_canopy : public ICanopy_photosynthesis_module {
    public:
        c4_canopy()
            : ICanopy_photosynthesis_module(std::vector<std::string> {"lai", "doy", "hour", "solar", "temp",
                    "rh", "windspeed", "lat", "nlayers", "vmax",
                    "alpha", "kparm", "beta", "Rd", "Catm",
                    "b0", "b1", "theta", "kd", "chil",
                    "heightf", "LeafN", "kpLN", "lnb0", "lnb1",
                    "lnfun", "upperT", "lowerT", "leafwidth",
                    "et_equation", "StomataWS", "water_stress_approach",
                    "nileafn", "kln", "nvmaxb0", "nvmaxb1", "alphab1",
                    "nRdb1", "nRdb0", "kpLN"}, 
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation (state_map const &s) const;
};

class c3_canopy : public ICanopy_photosynthesis_module {
    public:
        c3_canopy()
            : ICanopy_photosynthesis_module(std::vector<std::string> {"lai", "doy", "hour", "solar", "temp",
                    "rh", "windspeed", "lat", "nlayers", "vmax",
                    "jmax", "Rd", "Catm", "O2", "b0",
                    "b1", "theta", "kd", "heightf", "LeafN",
                    "kpLN", "lnb0", "lnb1", "lnfun", "chil", "StomataWS",
                    "water_stress_approach", "electrons_per_carboxylation", "electrons_per_oxygenation"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation (state_map const &s) const;
};

class ISoil_evaporation_module : public IModule {
    public:
        ISoil_evaporation_module(const std::vector<std::string> &required_state, const std::vector<std::string> &modified_state)
            : IModule(required_state, modified_state)
        {}
};

class one_layer_soil_profile : public ISoil_evaporation_module {
    public:
        one_layer_soil_profile()
            : ISoil_evaporation_module(std::vector<std::string> {"lai", "temp", "solar", "waterCont",
                    "FieldC", "WiltP", "windspeed", "rh", "rsec",
                    "CanopyT", "precip", "soilDepth", "phi1", "phi2",
                    "soilType", "wsFun", "StomataWS", "LeafWS"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_map const &s) const;
};

class two_layer_soil_profile : public ISoil_evaporation_module {
    public:
        two_layer_soil_profile()
            : ISoil_evaporation_module(std::vector<std::string> {"precip", "CanopyT", "cws1", "cws2", "soilDepth1",
                    "soilDepth2", "soilDepth3", "FieldC", "WiltP", "phi1",
                    "phi2", "soilType" /* Instead of soTexS */, "wsFun", "Root", "lai",
                    "temp", "solar", "windspeed", "rh", "hydrDist",
                    "rfl", "rsec", "rsdf", "StomataWS", "LeafWS"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_map const &s) const;
};

class ISenescence_module : public IModule {
    public:
        ISenescence_module(const std::vector<std::string> &required_state, const std::vector<std::string> &modified_state)
            : IModule(required_state, modified_state)
        {}
};

class thermal_time_senescence : public ISenescence_module {
    public:
        thermal_time_senescence()
            : ISenescence_module(std::vector<std::string> {"TTc", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                "leaf_senescence_index", "stem_senescence_index", "root_senescence_index", "rhizome_senescence_index",
                "mrc1", "mrc2"},
                std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

class thermal_time_and_frost_senescence : public ISenescence_module {
    public:
        thermal_time_and_frost_senescence()
            : ISenescence_module(std::vector<std::string> {"TTc", "leafdeathrate", "lat", "doy", "Tfrostlow", "Tfrosthigh", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                "leaf_senescence_index", "stem_senescence_index", "root_senescence_index", "rhizome_senescence_index"},
                std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

class IGrowth_module : public IModule {
    public:
        IGrowth_module(const std::vector<std::string> &required_state, const std::vector<std::string> &modified_state)
            : IModule(required_state, modified_state)
        {}
};

class partitioning_growth_module : public IGrowth_module {
    public:
        partitioning_growth_module()
            : IGrowth_module(std::vector<std::string> {"TTc", "LeafWS", "temp", "CanopyA",
                "Leaf", "Stem", "Root", "Rhizome", "Grain",
                "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                "mrc1", "mrc2"},
                std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

class no_leaf_resp_partitioning_growth_module : public IGrowth_module {
    public:
        no_leaf_resp_partitioning_growth_module()
            : IGrowth_module(std::vector<std::string> {"TTc", "LeafWS", "temp", "CanopyA",
                "Leaf", "Stem", "Root", "Rhizome", "Grain",
                "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                "mrc1", "mrc2"},
                std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

class test_derivs : public IModule {
    public:
        test_derivs()
            : IModule(std::vector<std::string> {"LeafArea", "PAR"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const;
};

class test_calc_state : public IModule {
    public:
        test_calc_state()
            : IModule(std::vector<std::string> {"LeafArea", "parameter"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const;
};

state_vector_map Gro(
        state_map const &initial_state,
        state_map const &invariant_parameters,
        state_vector_map const &varying_parameters,
        std::unique_ptr<IModule> const &canopy_photosynthesis_module,
        std::unique_ptr<IModule> const &soil_evaporation_module,
        std::unique_ptr<IModule> const &growth_module,
        std::unique_ptr<IModule> const &senescence_module,
		double (*leaf_n_limitation)(state_map const &model_state));

state_map Gro(
        state_map const &state,
        std::vector<std::unique_ptr<IModule>> const &steady_state_modules,
        std::vector<std::unique_ptr<IModule>> const &derivative_modules);

double biomass_leaf_nitrogen_limitation(state_map const &state);
double thermal_leaf_nitrogen_limitation(state_map const &state);


std::unique_ptr<IModule> make_module(std::string const &module_name);

std::string join_string_vector(std::vector<std::string> const &state_keys);

bool any_key_is_duplicated(std::vector<std::vector<std::string>> const &keys);

# endif

