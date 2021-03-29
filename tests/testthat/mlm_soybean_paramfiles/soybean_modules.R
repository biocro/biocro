# Do the calculations inside an empty list so that temporary variables are not created in .Global.
soybean_steadystate_modules <- c("soil_type_selector", "stomata_water_stress_linear", "leaf_water_stress_exponential",
                            "parameter_calculator", "soybean_development_rate_calculator", "partitioning_coefficient_logistic",
                            "soil_evaporation", "solar_zenith_angle", "shortwave_atmospheric_scattering",
                            "incident_shortwave_from_ground_par", "ten_layer_canopy_properties",
                            "ten_layer_c3_canopy", "ten_layer_canopy_integrator",
                            "no_leaf_resp_neg_assim_partitioning_growth_calculator",
                            "senescence_coefficient_logistic")

soybean_derivative_modules <- c("thermal_time_senescence_logistic", "partitioning_growth_negative_assimilation_partitioned",
                            "two_layer_soil_profile", "development_index", "thermal_time_linear")


