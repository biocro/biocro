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
        IModule(std::string const& module_name, std::vector<std::string> const &required_state, std::vector<std::string> const &modified_state) :
            _module_name(module_name),
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
        std::string const _module_name;
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
            : IModule(std::string("biomass_leaf_n_limitation"),
                    std::vector<std::string> {"LeafN_0", "Leaf", "Stem", "kln"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation (state_map const &s) const;

};

class ICanopy_photosynthesis_module : public IModule {
    public:
        ICanopy_photosynthesis_module(const std::string &module_name, const std::vector<std::string> &required_state, const std::vector<std::string> &modified_state)
            : IModule(module_name, required_state, modified_state)
        {}
};

class c4_canopy : public ICanopy_photosynthesis_module {
    public:
        c4_canopy()
            : ICanopy_photosynthesis_module("c4_canopy",
                    std::vector<std::string> {"lai", "doy", "hour", "solar", "temp",
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
            : ICanopy_photosynthesis_module("c3_canopy",
                    std::vector<std::string> {"lai", "doy", "hour", "solar", "temp",
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
        ISoil_evaporation_module(const std::string &module_name, const std::vector<std::string> &required_state, const std::vector<std::string> &modified_state)
            : IModule(module_name, required_state, modified_state)
        {}
};

class one_layer_soil_profile : public ISoil_evaporation_module {
    public:
        one_layer_soil_profile()
            : ISoil_evaporation_module("one_layer_soil_profile",
                    std::vector<std::string> {"lai", "temp", "solar", "soil_water_content",
                    "soil_field_capacity", "soil_wilting_point", "windspeed", "rh", "rsec",
                    "canopy_transpiration_rate", "precip", "soil_depth", "phi1", "phi2",
                    "StomataWS", "LeafWS", "soil_evaporation_rate"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_map const &s) const;
};

class two_layer_soil_profile : public ISoil_evaporation_module {
    public:
        two_layer_soil_profile()
            : ISoil_evaporation_module("two_layer_soil_profile",
                    std::vector<std::string> {"precip", "canopy_transpiration_rate", "cws1", "cws2", "soil_depth1",
                    "soil_depth2", "soil_depth3", "FieldC", "WiltP", "phi1",
                    "phi2", "soil_type_indicator", "wsFun", "Root", "lai",
                    "temp", "solar", "windspeed", "rh", "hydrDist",
                    "rfl", "rsec", "rsdf", "StomataWS", "LeafWS"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_map const &s) const;
};

class ISenescence_module : public IModule {
    public:
        ISenescence_module(const std::string &module_name, const std::vector<std::string> &required_state, const std::vector<std::string> &modified_state)
            : IModule(module_name, required_state, modified_state)
        {}
};

class thermal_time_senescence : public ISenescence_module {
    public:
        thermal_time_senescence()
            : ISenescence_module("thermal_time_senescence",
                    std::vector<std::string> {"TTc", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
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
            : ISenescence_module("thermal_time_and_frost_senescence",
                    std::vector<std::string> {"TTc", "leafdeathrate", "lat", "doy", "Tfrostlow", "Tfrosthigh", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                    "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                    "leaf_senescence_index", "stem_senescence_index", "root_senescence_index", "rhizome_senescence_index"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

class IGrowth_module : public IModule {
    public:
        IGrowth_module(const std::string &module_name, const std::vector<std::string> &required_state, const std::vector<std::string> &modified_state)
            : IModule(module_name, required_state, modified_state)
        {}
};

class utilization_senescence: public ISenescence_module {
    public:
        utilization_senescence()
            : ISenescence_module("utilization_senescence",
                    std::vector<std::string> {"TTc", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                    "Leaf", "Stem", "Root", "Rhizome", "Grain",
                    "rate_constant_leaf_senescence", "rate_constant_stem_senescence", "rate_constant_root_senescence", "rate_constant_rhizome_senescence", "rate_constant_grain_senescence",
                    "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                    "KmLeaf", "KmStem", "KmRoot", "KmRhizome", "KmGrain",
                    "resistance_leaf_to_stem", "resistance_stem_to_grain", "resistance_stem_to_root", "resistance_stem_to_rhizome",
                    "remobilization_fraction", "grain_TTc"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
        virtual state_map do_operation(state_map const &s) const;
};

class utilization_growth_module : public IGrowth_module {
    public:
        utilization_growth_module()
            : IGrowth_module("utilization_growth_module",
                    std::vector<std::string> {"canopy_assimilation_rate",
                    "Leaf", "Stem", "Root", "Rhizome", "Grain",
                    "rate_constant_leaf", "rate_constant_stem", "rate_constant_root", "rate_constant_rhizome", "rate_constant_grain",
                    "KmLeaf", "KmStem", "KmRoot", "KmRhizome", "KmGrain",
                    "resistance_leaf_to_stem", "resistance_stem_to_grain", "resistance_stem_to_root", "resistance_stem_to_rhizome",
                    "substrate_pool_leaf", "substrate_pool_stem", "substrate_pool_root", "substrate_pool_rhizome", "substrate_pool_grain"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
        virtual state_map do_operation(state_map const &s) const;
};

class utilization_growth_and_senescence_module: public ISenescence_module {
    public:
        utilization_growth_and_senescence_module()
            : ISenescence_module("utilization_growth_and_senescence_module",
                    std::vector<std::string> {"TTc", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                    "Leaf", "Stem", "Root", "Rhizome", "Grain",
                    "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                    "KmLeaf", "KmStem", "KmRoot", "KmRhizome", "KmGrain",
                    "resistance_leaf_to_stem", "resistance_stem_to_grain", "resistance_stem_to_root", "resistance_stem_to_rhizome",
                    "rate_constant_leaf", "rate_constant_stem", "rate_constant_root", "rate_constant_rhizome",
                    "remobilization_fraction", "grain_TTc"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
        virtual state_map do_operation(state_map const &s) const;
};

class utilization_growth_flowering: public ISenescence_module {
    public:
        utilization_growth_flowering()
            : ISenescence_module("utilization_growth_flowering",
                    std::vector<std::string> {"TTc", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                    "Leaf", "Stem", "Root", "Rhizome", "Grain",
                    "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                    "KmLeaf", "KmStem", "KmRoot", "KmRhizome", "KmGrain",
                    "resistance_leaf_to_stem", "resistance_stem_to_grain", "resistance_stem_to_root", "resistance_stem_to_rhizome",
                    "GI_on", "GI_off", "FKF1_on", "FKF1_off", "CDF_on", "CDF_off", "FT_on", "FT_off", "FT_inhibition", "FKF1_timing", "CDF_timing", "dawn"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

class empty_senescence: public ISenescence_module {
    public:
        empty_senescence()
            : ISenescence_module("empty_senescence",
                    std::vector<std::string> {},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};


class partitioning_growth_module : public IGrowth_module {
    public:
        partitioning_growth_module()
            : IGrowth_module("partitioning_growth_module",
                    std::vector<std::string> {"TTc", "LeafWS", "temp", "canopy_assimilation_rate",
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
            : IGrowth_module("no_leaf_resp_partitioning_growth_module",
                    std::vector<std::string> {"TTc", "LeafWS", "temp", "canopy_assimilation_rate",
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
            : IModule("test_derivs",
                    std::vector<std::string> {"LeafArea", "PAR"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const;
};

class test_calc_state : public IModule {
    public:
        test_calc_state()
            : IModule("test_calc_state",
                    std::vector<std::string> {"LeafArea", "parameter"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const;
};

class stomata_water_stress_linear : public IModule {
    public:
        stomata_water_stress_linear()
            : IModule("stomata_water_stress_linear",
                    std::vector<std::string> {"soil_water_content", "soil_field_capacity", "soil_wilting_point"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const
        {
            double slope = 1 / (s.at("soil_field_capacity") - s.at("soil_wilting_point"));
            double intercept = 1 - s.at("soil_field_capacity") * slope;
            state_map result { {"StomataWS", std::min(std::max(slope * s.at("soil_water_content") + intercept, 1e-10), 1.0)} };
            return result;
        };
};

class stomata_water_stress_sigmoid : public IModule {
    public:
        stomata_water_stress_sigmoid()
            : IModule("stomata_water_stress_sigmoid",
                    std::vector<std::string> {"soil_water_content", "soil_field_capacity", "soil_wilting_point", "phi1"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const
        {
            const double phi10 = (s.at("soil_field_capacity") + s.at("soil_wilting_point")) / 2;
            state_map result { {"StomataWS", std::min(std::max(1 / (1 + exp((phi10 - s.at("soil_water_content")) / s.at("phi1"))), 1e-10), 1.0)} };
            return result;
        };
};

class stomata_water_stress_exponential : public IModule {
    public:
        stomata_water_stress_exponential()
            : IModule("stomata_water_stress_exponential",
                    std::vector<std::string> {"soil_water_content", "soil_field_capacity", "soil_wilting_point"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const
        {
            double wilting_point = s.at("soil_wilting_point");
            double field_capacity = s.at("soil_field_capacity");

            double slope = (1 - wilting_point) / (field_capacity - wilting_point);
            double intercept = 1 - field_capacity * slope;
            double theta = slope * s.at("soil_water_content") + intercept;
            state_map result { {"StomataWS", std::min(std::max((1 - exp(-2.5 * (theta - wilting_point)/(1 - wilting_point))) / (1 - exp(-2.5)), 1e-10), 1.0)} };
            return result;
        };
};

class leaf_water_stress_exponential : public IModule {
    public:
        leaf_water_stress_exponential()
            : IModule("leaf_water_stress_exponential",
                    std::vector<std::string> {"soil_water_content", "soil_field_capacity", "phi2"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const
        {
            state_map result { {"LeafWS",  std::min(std::max(pow(s.at("soil_water_content") / s.at("soil_field_capacity"), s.at("phi2")), 1e-10), 1.0)} };
            return result;
        };
};

class one_layer_soil_profile_derivatives : public IModule {
    public:
        one_layer_soil_profile_derivatives()
            : IModule("one_layer_soil_profile_derivatives",
                    std::vector<std::string> {"precipitation_rate", "soil_saturation_capacity", "soil_field_capacity", "soil_wilting_point", "soil_water_content",
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
            double runoff = (soil_water_content - s.at("soil_saturation_capacity")) * soil_depth / second_per_hour;  // m s^-1. The previous model drained everything in a single time-step. By default the time step was one hour, so use a rate that drains everything in one hour.
            double n_leach = runoff / 18 * (0.2 + 0.7 * s.at("soil_sand_content")) / second_per_hour;  // Base the rate on an hour for the same reason as was used with `runoff`.

            double evapotranspiration_volume = s.at("evapotranspiration") / density_of_water_at_20_celcius / 1e4 / second_per_hour;  // m^3 m^-2 s^-1


            state_map derivatives;
            derivatives["soil_water_content"] = (drainage + precipitation_m_s - runoff - evapotranspiration_volume) / soil_depth;
            derivatives["soil_n_content"] = -n_leach;
            return derivatives;
        }
};

class soil_evaporation : public IModule {
    public:
        soil_evaporation()
            : IModule("soil_evaporation",
                    std::vector<std::string> {"lai", "temp", "solar", "soil_field_capacity", "soil_wilting_point", "soil_water_content",
                    "soil_sand_content", "soil_saturated_conductivity", "soil_air_entry", "soil_reflectance", "soil_transmission",
                    "soil_clod_size", "rsec"},
                    std::vector<std::string> {})
            {}
    private:
        state_map do_operation(state_map const &s) const
        {
            double soilEvap = SoilEvapo(s.at("lai"), 0.68, s.at("temp"), s.at("solar"), s.at("soil_water_content"),
                        s.at("soil_field_capacity"), s.at("soil_wilting_point"), s.at("windspeed"), s.at("rh"), s.at("rsec"), 
                        s.at("soil_clod_size"), s.at("soil_reflectance"), s.at("soil_transmission"),
                        s.at("specific_heat"), s.at("stefan_boltzman"));  // kg / m^2 / s.
            state_map result { {"soil_evaporation_rate", soilEvap } };
            return result;
        }
};

class bucket_soil_drainage : public IModule {
    public:
        bucket_soil_drainage()
            : IModule("bucket_soil_drainage",
                    std::vector<std::string> {"precipitation_rate", "soil_evaporation_rate", "soil_water_content", "soil_field_capacity", "soil_wilting_point",
                    "soil_saturation_capacity", "soil_depth", "soil_sand_content", "soil_saturated_conductivity", "soil_air_entry",
                    "soil_b_coefficient", "canopy_transpiration_rate"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const
        {
            constexpr double g = 9.8;  // m / s^2. Acceleration due to gravity.
            constexpr double density_of_water_at_20_celcius = 998.2;  // kg m^-3.

            double field_capacity = s.at("soil_field_capacity");  // m^3 / m^3.
            double wilting_point = s.at("soil_wilting_point");  // m^3 / m^3.
            double water_content = s.at("soil_water_content");  // m^3 / m^3.
            double saturation_capacity = s.at("soil_saturation_capacity");  // m^3 / m^3.
            double soil_depth = s.at("soil_depth"); // meters;
            double precipitation_rate = s.at("precipitation_rate") / 1e3;  // m / s.

            /* soil_matric_potential is calculated as per "Dynamic Simulation of Water Deficit Effects upon Maize Yield" R. F. Grant Agricultural Systems. 33(1990) 13-39. */
            double soil_matric_potential = -exp(log(0.033) + log(field_capacity / water_content) / log(field_capacity / wilting_point) * log(1.5 / 0.033)) * 1e3;  // This last term converts from MPa to kPa.
            double hydraulic_conductivity = s.at("soil_saturated_conductivity") * pow(s.at("soil_air_entry") / soil_matric_potential, 2 + 3 / s.at("soil_b_coefficient"));  // kg s / m^3.

            double drainage = - hydraulic_conductivity * g / density_of_water_at_20_celcius;  // m / s.

            constexpr double runoff_rate = 1 / 3600;  // Runoff 1 m^3 / hr.
            double runoff = std::min(0.0, water_content - saturation_capacity) * runoff_rate * soil_depth; // m / s.

            double transpiration_rate = s.at("canopy_transpiration_rate") / density_of_water_at_20_celcius * 1000 / 10000 / 3600;  // m / s.
            double evaporation_rate = s.at("soil_evaporation_rate") / density_of_water_at_20_celcius;  // m / s.

            state_map result { {"soil_water_content", (precipitation_rate - transpiration_rate - evaporation_rate - runoff - drainage) / soil_depth * 3600} };  // m^3 / m^3 / hr;
            return result;
        }
};

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

class parameter_calculator : public IModule {
    public:
        parameter_calculator()
            : IModule("parameter_calculator",
                    std::vector<std::string> {"iSp", "Sp_thermal_time_decay", "Leaf", "LeafN_0",
                    "vmax_n_intercept", "vmax1", "alphab1", "alpha1"},
                      std::vector<std::string> {})
            {}
    private:
        state_map do_operation(state_map const &s) const;
};

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

class soil_type_selector : public IModule {
    public:
        soil_type_selector()
            : IModule("soil_type_selector",
                    std::vector<std::string> {"soil_type_identifier"},
                       std::vector<std::string> {})
            {}
    private:
        state_map do_operation(state_map const &s) const
        {
            soilText_str soil_properties = get_soil_properties(s.at("soil_type_indicator"));
            state_map result {
                { "soil_silt_content",  soil_properties.silt },
                { "soil_clay_content",  soil_properties.clay },
                { "soil_sand_content",  soil_properties.sand },
                { "soil_air_entry",  soil_properties.air_entry },
                { "soil_b_coefficient",  soil_properties.b },
                { "soil_saturated_conductivity",  soil_properties.Ks },
                { "soil_saturation_capacity",  soil_properties.satur },
                { "soil_field_capacity",  soil_properties.fieldc },
                { "soil_wilting_point",  soil_properties.wiltp },
                { "soil_bulk_density",  soil_properties.bulk_density }
            };
            return result;
        }
};

class thermal_time_accumulator : public IModule {
    public:
        thermal_time_accumulator()
            : IModule("thermal_time_accumulator",
                    std::vector<std::string> {"TTc", "temp", "tbase"},
                       std::vector<std::string> {})
            {}
    private:
        state_map do_operation(state_map const &s) const
        {
            double temp_diff = s.at("temp") - s.at("tbase");
            state_map result { {"TTc", (temp_diff > 0) ? temp_diff / 24 : 0 } };
            return result;
        }
};

state_vector_map Gro(
        state_map const &initial_state,
        state_map const &invariant_parameters,
        state_vector_map const &varying_parameters,
        std::vector<std::unique_ptr<IModule>> const &steady_state_modules,
        std::vector<std::unique_ptr<IModule>> const &derivative_modules);

state_map Gro(
        state_map state,
        std::vector<std::unique_ptr<IModule>> const &steady_state_modules,
        std::vector<std::unique_ptr<IModule>> const &derivative_modules);

double biomass_leaf_nitrogen_limitation(state_map const &state);
double thermal_leaf_nitrogen_limitation(state_map const &state);


std::string join_string_vector(std::vector<std::string> const &state_keys);

bool any_key_is_duplicated(std::vector<std::vector<std::string>> const &keys);

# endif

