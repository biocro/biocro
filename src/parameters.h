#ifndef PARAMETERS_H
#define PARAMETERS_H

/*
#include <iostream>

#include <string>
#include <vector>
#include <unordered_map>
#include <boost/algorithm/string.hpp>

struct parameter {
    std::string description;
    std::string units;
};

struct parameter_list {
    typedef std::pair<std::string, parameter> parameter_entry;
    typedef std::unordered_map<std::string, parameter> parameter_entries;

    parameter_list(parameter_entries const &l = parameter_entries()) : _parameter_list(l) {};
    parameter_list(std::initializer_list<parameter_entries::value_type> const &l) : _parameter_list(l) {};

    void add(parameter_entry const &entry);
    void add(std::string const &s, char const delim, bool const trim);
    void add(parameter_entries const &l);
    parameter_entries _parameter_list;
};

inline void parameter_list::add(parameter_entry const &entry)
{
    this->_parameter_list[entry.first] = entry.second;
}

inline void parameter_list::add(parameter_entries const &l)
{
    for (auto const &entry : l) {
        this->add(entry);
    }
}

inline void parameter_list::add(std::string const &s, char const delim, bool const trim_tokens)
{
    std::vector<std::string> tokens;
    boost::split(tokens, s, [delim](char c){return c == delim;});
    if (tokens.size() != 3) throw std::invalid_argument("Each parameter must have exactly three values delimited by \"delim\".");
    if (trim_tokens) {
        for (auto &t : tokens) {
            boost::algorithm::trim(t);
        }
    }
    this->add(parameter_entry {tokens[0], {tokens[1], tokens[2]}});
}

parameter_list parameters {
  { "windspeed"                         , {"Wind speed at the top of the canopy."                                     ,"m / s"                        }},
  { "soilEvap"                          , {"Rate of evaporation of water from the soil."                              ,""                             }},
  { "solar"                             , {"Photosynthetically active radiation photon flux at the top of the canopy" ,"micromol / m^2 / s"           }},
  { "lat"                               , {"Latitude"                                                                 ,"degrees N"                    }},
  { "rh"                                , {"Relative humidity"                                                        ,"Pa / Pa"                      }},
  { "soil_bulk_density"                 , {"Mass of soil per volume of bulk soil."                                    ,"Mg / m^3"                     }},
  { "soil_water_content"                , {"Volume of water per volume of bulk soil."                                 ,"m^3 / m^3"                    }},
  { "soil_saturated_conductivity"       , {"Conductivity of soil when soil_saturation_capacity is 1"                  ,"kg / s / m^3"                 }},
  { "soil_saturation_capacity"          , {"The maximum soil_water_content that the soil can hold."                   ,"m^3 / m^3"                    }},
  { "Leaf"                              , {"Dry mass of leaves per area of ground."                                   ,"Mg / ha"                      }},
  { "Stem"                              , {"Dry mass of stems per area of ground."                                    ,"Mg / ha"                      }},
  { "Root"                              , {"Dry mass of roots per area of ground."                                    ,"Mg / ha"                      }},
  { "Sp"                                , {"Specific leaf area, one-sided leaf area per mass of leaf"                 ,"ha / Mg"                      }},
  { "Sp_thermal_time_decay"             , {"The rate at which Sp decreases"                                           ,"ha / Mg / degrees C / day"    }},  // See TTc parameter.
  { "iSp"                               , {"Initial value of specific leaf area when TTc is 0."                       ,"ha / Mg"                      }},
  { "TTc"                               , {"Thermal time as growing degree days."                                     ,"degrees C * day"              }},
  { "precipitation_rate"                , {"Precipitation per time."                                                  ,"mm / s"                       }},
  { "b0"                                , {"Ball-Berry model intercept."                                              ,"mol / m^2 / s"                }},
  { "b1"                                , {"Ball-Berry model slope."                                                  ,"dimensionless"                }},
  { "heightf"                           , {"The LAI per vertical height of the canopy"                                ,"m^2 / m^2 / m"                }},  // The units simplify to m^-1, but m^2 / m^2 is included for clarity with leaf area index.
  { "temp"                              , {"Air temperature"                                                          ,"degrees C"                    }},
  { "leafwidth"                         , {"Leaf width"                                                               ,"m"                            }},
  { "canopy_assimilation_rate"          , {"Canopy carbon assimilation rate on a ground-area basis"                   ,"Mg / ha / hr"                 }},
  { "canopy_transpiration_rate"         , {"Canopy water transpiation rate on a ground-area basis"                    ,"Mg / ha / hr"                 }},
  { "Leaf"                              , {"Mass of leaves per ground area"                                           ,"Mg / ha"                      }},
  { "Stem"                              , {"Mass of stems per ground area"                                            ,"Mg / ha"                      }},
  { "Root"                              , {"Mass of roots per ground area"                                            ,"Mg / ha"                      }},
  { "Grain"                             , {"Mass of grain per ground area"                                            ,"Mg / ha"                      }},
  { "Rhizome"                           , {"Mass of rhizomes per ground area"                                         ,"Mg / ha"                      }},
  { "acceleration_from_gravity"         , {"Accerlation from gravity"                                                 ,"m / s^2"                      }},
  { "lai"                               , {"Leaf area index"                                                          ,"m^2 / m^2"                    }},
  { "Catm"                              , {"Atmospheric CO2 concentration"                                            ,"micromol / mol"               }},
  { "Vcmax_at_25"                       , {"Maximum carboxylation rate of C4 photosynthesis."                         ,"micromol / m^2 / s"           }},
  { "alpha"                             , {"Quantum efficiency of CO2 assimiliation for incident radiation."          ,"mol / mol"                    }},  // This is for *incident* not absorbed radiation.
  { "kparm"                             , {"Slope of the roughly linear portion of an A-Ci curve."                    ,"mol / m^2 / s / Pa"           }},
  { "slope_water_vapor"                 , {"Slope of the saturation water vapor density vs air temperature curve."    ,"kg / m^3 / K"                 }},  // It is also kg / m^3 / degrees C since it is a change in temperature.
  { "latent_heat_vaporization_of_water" , {"Latent heat of vaporization of water."                                    ,"J / kg"                       }},
  { "saturation_water_vapor_pressure"   , {"Vapor pressure of water that saturates the air's capacity."               ,"Pa"                           }},
  { "vapor_density_deficit"             , {"The different between vapor density and saturated vapor density"          ,"kg / m^3"                     }},
  { "psychrometric_parameter"           , {"Parameter to relate change in air temperature to air density"             ,"kg / m^3 / K"                 }},
  { "soil_aba_concentration"            , {"The concentration of ABA in the soil."                                    ,""                             }},
  //{ "                                   , {"                                                                          ,"                              }},
  //{ "                                   , {"                                                                          ,"                              }},
};



/*
int main(int argc, char* argv[])
{
    try {
    parameters.add("windspeed   ,    The wind speed at the top of the canopy.  ,   m / s", ',', true);
    parameters.add("soilEvap, The rate of water evaporation from the soil.,   m / s", ',', true);
    std::cout << parameters._parameter_list.size() << '\n';
    for (auto &p : parameters._parameter_list) {
        std::cout << p.first << ", " << p.second.description << ", " << p.second.units << '\n';
    }
    } catch (std::exception const &e) {
        std::cout << "Exception thrown: " << e.what() << '\n';
    }
    return 0;
}
*/

# endif
