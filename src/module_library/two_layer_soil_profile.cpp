#include "two_layer_soil_profile.hpp"
#include "BioCro.h"

state_map two_layer_soil_profile::do_operation(state_map const &s) const
{
    double cws[] = {s.at("cws1"), s.at("cws2")};
    double soil_depths[] = {s.at("soil_depth1"), s.at("soil_depth2"), s.at("soil_depth3")};

    struct soilML_str soilMLS = soilML(s.at("precip"), s.at("canopy_transpiration_rate"), cws, s.at("soil_depth3"), soil_depths,
            s.at("soil_field_capacity"), s.at("soil_wilting_point"), s.at("soil_saturation_capacity"), s.at("soil_air_entry"), s.at("soil_saturated_conductivity"),
            s.at("soil_b_coefficient"), s.at("soil_sand_content"), s.at("phi1"), s.at("phi2"), s.at("wsFun"),
            2 /* Always uses 2 layers. */, s.at("Root"), s.at("lai"), 0.68, s.at("temp"),
            s.at("solar"), s.at("windspeed"), s.at("rh"), s.at("hydrDist"), s.at("rfl"),
            s.at("rsec"), s.at("rsdf"), s.at("soil_clod_size"), s.at("soil_reflectance"), s.at("soil_transmission"),
            s.at("specific_heat"), s.at("stefan_boltzman"));

    double layer_one_depth = s.at("soil_depth2") - s.at("soil_depth1");
    double layer_two_depth = s.at("soil_depth3") - s.at("soil_depth2");
    double cws_mean = (soilMLS.cws[0] * layer_one_depth + soilMLS.cws[1] * layer_two_depth) / (layer_one_depth + layer_two_depth);

    state_map new_state {
        { "StomataWS", soilMLS.rcoefPhoto - s.at("StomataWS") },
        { "LeafWS",  soilMLS.rcoefSpleaf - s.at("LeafWS") },
        { "soilEvap", soilMLS.SoilEvapo },

        { "cws1", soilMLS.cws[0] - s.at("cws1") },
        { "cws2", soilMLS.cws[1] - s.at("cws2") },
        { "soil_water_content",  cws_mean - s.at("soil_water_content") }
    };

    return new_state;
}
