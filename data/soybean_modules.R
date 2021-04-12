# Do the calculations inside an empty list so that temporary variables are not created in .Global.
soybean_steadystate_modules <- c("soil_type_selector",
                                 "stomata_water_stress_linear",
                                 "parameter_calculator",
                                 "soybean_development_rate_calculator",
                                 "partitioning_coefficient_logistic",
                                 "soil_evaporation",
                                 "solar_zenith_angle",
                                 "shortwave_atmospheric_scattering",
                                 "incident_shortwave_from_ground_par",
                                 "ten_layer_canopy_properties",
                                 "ten_layer_c3_canopy",
                                 "ten_layer_canopy_integrator",
                                 "no_leaf_resp_neg_assim_partitioning_growth_calculator",
                                 "senescence_coefficient_logistic")

soybean_derivative_modules <- c("senescence_logistic",
                                "partitioning_growth",
                                "two_layer_soil_profile",
                                "development_index",
                                "thermal_time_linear")


soybean_solver_params <- list(
  type = 'Gro_rkck54',
  output_step_size = 1.0,
  adaptive_rel_error_tol = 1e-4,
  adaptive_abs_error_tol = 1e-4,
  adaptive_max_steps = 200)
# Note: 'Gro_rsnbrk' and 'Gro' (which defaults to Gro_rsnbrk when an adaptive solver can be
# used, as in this case) should not be used as the solver type as the model will not simulate
# soybean growth unless the tolerances are stringent (e.g., output_step_size = 0.01,
# adaptive_rel_error_tol = 1e-9, adaptive_abs_error_tol = 1e-9)

