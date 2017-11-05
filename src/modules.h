#ifndef MODULES_H
#define MODULES_H

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include "BioCro.h"
#include "c3photo.h"
#include "state_map.h"
#include <math.h>

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
            : ISoil_evaporation_module(std::vector<std::string> {"lai", "temp", "solar", "soil_water_content",
                    "FieldC", "WiltP", "windspeed", "rh", "rsec",
                    "CanopyT", "precip", "soil_depth", "phi1", "phi2",
                    "soil_type_indicator", "wsFun", "StomataWS", "LeafWS"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_map const &s) const;
};

class two_layer_soil_profile : public ISoil_evaporation_module {
    public:
        two_layer_soil_profile()
            : ISoil_evaporation_module(std::vector<std::string> {"precip", "CanopyT", "cws1", "cws2", "soil_depth1",
                    "soil_depth2", "soil_depth3", "FieldC", "WiltP", "phi1",
                    "phi2", "soil_type_indicator" /* Instead of soTexS */, "wsFun", "Root", "lai",
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

class utilization_senescence: public ISenescence_module {
    public:
        utilization_senescence()
            : ISenescence_module(std::vector<std::string> {"TTc", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                "Leaf", "Stem", "Root", "Rhizome", "Grain",
                "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                "KmLeaf", "KmStem", "KmRoot", "KmRhizome", "KmGrain",
                "resistance_leaf_to_stem",
                "resistance_stem_to_grain",
                "resistance_stem_to_root",
                "resistance_stem_to_rhizome"},
                std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
        virtual state_map do_operation(state_map const &s) const;
};

class utilization_growth_module : public IGrowth_module {
    public:
        utilization_growth_module()
            : IGrowth_module(std::vector<std::string> {"Assim",
                "Leaf", "Stem", "Root", "Rhizome", "Grain",
                "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                "KmLeaf", "KmStem", "KmRoot", "KmRhizome", "KmGrain",
                "resistance_leaf_to_stem",
                "resistance_stem_to_grain",
                "resistance_stem_to_root",
                "resistance_stem_to_rhizome"},
                std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
        virtual state_map do_operation(state_map const &s) const;
};

class utilization_growth_and_senescence_module: public ISenescence_module {
    public:
        utilization_growth_and_senescence_module()
            : ISenescence_module(std::vector<std::string> {"TTc", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                "Leaf", "Stem", "Root", "Rhizome", "Grain",
                "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                "KmLeaf", "KmStem", "KmRoot", "KmRhizome", "KmGrain",
                "resistance_leaf_to_stem",
                "resistance_stem_to_grain",
                "resistance_stem_to_root",
                "resistance_stem_to_rhizome",
                "rate_constant_leaf", "rate_constant_stem", "rate_constant_root", "rate_constant_rhizome",
                "remobilization_fraction", "grain_TTc"},
                std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

class utilization_growth_flowering_module: public ISenescence_module {
    public:
        utilization_growth_flowering_module()
            : ISenescence_module(std::vector<std::string> {"TTc", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                "Leaf", "Stem", "Root", "Rhizome", "Grain",
                "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                "KmLeaf", "KmStem", "KmRoot", "KmRhizome", "KmGrain",
                "resistance_leaf_to_stem",
                "resistance_stem_to_grain",
                "resistance_stem_to_root",
                "resistance_stem_to_rhizome",
                "GI_on", "GI_off", "FKF1_on", "FKF1_off", "CDF_on", "CDF_off", "FT_on", "FT_off", "FT_inhibition", "FKF1_timing", "CDF_timing", "dawn"},
                std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

class empty_senescence: public ISenescence_module {
    public:
        empty_senescence()
            : ISenescence_module(std::vector<std::string> {},
                std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
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

class ws_photo_linear : public IModule {
    public:
        ws_photo_linear()
            : IModule(std::vector<std::string> {"soil_water_content", "soil_field_capacity", "soil_wilting_point"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const
        {
            state_map result;
            double slope = 1 / (s.at("soil_field_capacity") - s.at("soil_wilting_point"));
            double intercept = 1 - s.at("soil_field_capacity") * intercept;
            result["wsPhoto"] = slope * s.at("soil_water_content");
            return result;
        };
};

class leaf_ws_exponential : public IModule {
    public:
        leaf_ws_exponential()
            : IModule(std::vector<std::string> {"soil_water_content", "soil_field_capacity", "phi1", "phi2"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const
        {
            state_map result;
            result["LeafWS"] = fmin(pow(s.at("soil_water_content") / s.at("soil_field_capacity"), s.at("phi2")), 1);
            return result;
        };
};

class one_layer_soil_profile_derivatives : public IModule {
    public:
        one_layer_soil_profile_derivatives()
            : IModule(std::vector<std::string> {"precipitation_rate", "soil_saturation_content", "soil_field_capacity", "soil_wilting_point", "soil_water_content",
                    "soil_depth", "soil_sand_content", "evapotranspiration", "soil_saturated_conductivity", "soil_air_entry",
                    "soil_b_coefficient", "acceleration_from_gravity"},
                    std::vector<std::string> {})
            {}
    private:
        state_map do_operation(state_map const &s) const
        {
            const double soil_water_content = s.at("soil_water_content"); // m^3 m^-3.
            const double soil_depth = s.at("soil_depth");

            double soil_water_potential = -exp(log(0.033) + ((log(s.at("soil_field_capacity")) - log(soil_water_content)) / (log(s.at("soil_field_capacity")) - log(s.at("soil_wilting_point"))) * (log(1.5) - log(0.033)))) * 1e3; // kPa.
            double hydraulic_conductivity = s.at("soil_saturated_conductivity") * pow(s.at("soil_air_entry") / soil_water_potential, 2 + 3 / s.at("soil_b_coefficient")); // Kg s m^-3.
            double J_w = -hydraulic_conductivity * (-soil_water_potential / (soil_depth * 0.5)) - s.at("acceleration_from_gravity") * hydraulic_conductivity; // kg m^-2 s^-1. Campbell, pg 129. I multiply soil depth by 0.5 to calculate the average depth.

            constexpr double density_of_water_at_20_celcius = 998.2; // kg m^-3

            double drainage = J_w / density_of_water_at_20_celcius;  // m s^-1.
            double precipitation_m_s = s.at("precipitation_rate") * 1e-3; // m s^-2.

            constexpr double second_per_hour = 3600;
            double runoff = (soil_water_content - s.at("soil_saturation")) * soil_depth / second_per_hour;  // m s^-1. The previous model drained everything in a single time-step. By default the time step was one hour, so use a rate that drains everything in one hour.
            double n_leach = runoff / 18 * (0.2 + 0.7 * s.at("soil_sand_content")) / second_per_hour;  // Base the rate on an hour for the same reason as was used with `runoff`.

            double evapotranspiration_volume = s.at("evapotranspiration") / density_of_water_at_20_celcius / 1e4 / second_per_hour;  // m^3 m^-2 s^-1


            state_map derivatives;
            derivatives["soil_water_content"] = (drainage + precipitation_m_s - runoff - evapotranspiration_volume) / soil_depth;
            derivatives["soil_n_content"] = -n_leach;
            return derivatives;
        }
};

class linear_vmax_from_leaf_n : public IModule {
    public:
        linear_vmax_from_leaf_n()
            : IModule(std::vector<std::string> {"LeafN", "LeafN_0", "vmax_n_intercept", "vmax_n_slope"},
                    std::vector<std::string> {})
            {}
    private:
        state_map do_operation(state_map const &s) const
        {
            state_map result;
            result["vmax"] = (s.at("LeafN_0") - s.at("LeafN")) * s.at("vmax_n_intercept") + s.at("vmax1");
            return result;
        }
};

class parameter_calculator : public IModule {
    public:
        parameter_calculator()
            : IModule(std::vector<std::string> {"iSp", "Sp_thermal_time_decay", "Leaf", "LeafN_0",
                    "vmax_n_intercept", "vmax1", "alphab1", "alpha1"},
                      std::vector<std::string> {})
            {}
    private:
        state_map do_operation(state_map const &s) const;
};

class soil_type_selector : public IModule {
    public:
        soil_type_selector()
            : IModule(std::vector<std::string> {"soil_type_identifier"},
                       std::vector<std::string> {})
            {}
    private:
        state_map do_operation(state_map const &s) const
        {
            soilText_str soil_properties = get_soil_properties(s.at("soil_type_indicator"));
            state_map result;
            result["soil_silt_content"] = soil_properties.silt;
            result["soil_clay_content"] = soil_properties.clay;
            result["soil_sand_content"] = soil_properties.sand;
            result["soil_air_entry"] = soil_properties.air_entry;
            result["soil_b_coefficient"] = soil_properties.b;
            result["soil_saturated_conductivity"] = soil_properties.Ks;
            result["soil_saturation_content"] = soil_properties.satur;
            result["soil_field_capacity"] = soil_properties.fieldc;
            result["soil_wilting_point"] = soil_properties.wiltp;
            result["soil_bulk_density"] = soil_properties.bulk_density;
            return result;
        }
};

class thermal_time_accumulator : public IModule {
    public:
        thermal_time_accumulator()
            : IModule(std::vector<std::string> {"TTc", "temp", "tbase"},
                       std::vector<std::string> {})
            {}
    private:
        state_map do_operation(state_map const &s) const
        {
            state_map result;
            double temp_diff = s.at("temp") - s.at("tbase");
            result["TTc"] = (temp_diff > 0) ? temp_diff / 24 : 0;
            return result;
        }
};

template<typename T> std::unique_ptr<IModule> createModule() { return std::unique_ptr<IModule>(new T); }
class ModuleFactory {
    private:
        typedef std::map<std::string, std::unique_ptr<IModule>(*)()> module_map;  // A map of strings to function pointers.
        const module_map modules
        {
            { "c4_canopy",                          &createModule<c4_canopy>},
            { "c3_canopy",                          &createModule<c3_canopy>},
            { "one_layer_soil_profile",             &createModule<one_layer_soil_profile>},
            { "two_layer_soil_profile",             &createModule<two_layer_soil_profile>},
            { "partitioning_growth",                &createModule<partitioning_growth_module>},
            { "no_leaf_resp_partitioning_growth",   &createModule<no_leaf_resp_partitioning_growth_module>},
            { "utilization_growth_and_senescence",  &createModule<utilization_growth_and_senescence_module>},
            { "utilization_growth",                 &createModule<utilization_growth_module>},
            { "utilization_senescence",             &createModule<utilization_senescence>},
            { "empty_senescence",                   &createModule<empty_senescence>},
            { "thermal_time_and_frost_senescence",  &createModule<thermal_time_and_frost_senescence>},
            { "thermal_time_senescence",            &createModule<thermal_time_senescence>},
            { "one_layer_soil_profile_derivatives", &createModule<one_layer_soil_profile_derivatives>},
            { "soil_type_selector",                 &createModule<soil_type_selector>},
            { "test_derivs",                        &createModule<test_derivs>},
            { "test_calc_state",                    &createModule<test_calc_state>},
            { "parameter_calculator",               &createModule<parameter_calculator>},
            { "thermal_time_accumulator",           &createModule<thermal_time_accumulator>},
            { "biomass_leaf_n_limitation",          &createModule<biomass_leaf_n_limitation>}
        };

    public:
        std::unique_ptr<IModule> operator()(std::string const &module_name) {
            try {
                return this->modules.at(module_name)();
            } catch (std::out_of_range const &oor) {
                throw std::out_of_range(std::string("\"") + module_name + std::string("\"") + std::string(" was given as a module name, but no module with that name could be found.\n"));
            }

        }
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


std::string join_string_vector(std::vector<std::string> const &state_keys);

bool any_key_is_duplicated(std::vector<std::vector<std::string>> const &keys);

# endif

