# Some modules are included as named list elements so they can be easily changed
# on-the-fly to a different value, e.g.,
# manihot_esculenta_direct_modules$canopy_photosynthesis <- 'BioCro:ten_layer_rue_canopy'

manihot_esculenta_direct_modules <- list(
    stomata_water_stress = "BioCro:stomata_water_stress_linear",
    "BioCro:leaf_water_stress_exponential",
    "BioCro:parameter_calculator",
    "BioCro:soil_evaporation",
    solar_coordinates = "BioCro:solar_position_michalsky",
    canopy_photosynthesis = "BioCro:c3_canopy",
    "BioCro:utilization_growth_calculator",
    "BioCro:utilization_senescence_calculator"
)

manihot_esculenta_differential_modules <- list(
    "BioCro:utilization_senescence",
    "BioCro:utilization_growth",
    thermal_time = "BioCro:thermal_time_linear",
    soil_profile = "BioCro:two_layer_soil_profile"
)

# Error tolerances greater than 1e-5 may cause problems with the regression test
manihot_esculenta_ode_solver <- list(
    type = 'auto',
    output_step_size = 1.0,
    adaptive_rel_error_tol = 1e-5,
    adaptive_abs_error_tol = 1e-5,
    adaptive_max_steps = 200
)

# Do the calculations inside an empty list so that temporary variables are not created in .Global.
manihot_esculenta_initial_values = with(list(), {
    datalines =
    "symbol                  value
    cws1                     0.32
    cws2                     0.32
    Grain                    0.0001
    Leaf                     0.02
    LeafLitter               0
    Rhizome                  0.99
    RhizomeLitter            0
    Root                     1
    RootLitter               0
    soil_water_content       0.32
    Stem                     0.99
    StemLitter               0
    substrate_pool_grain     0.00005    # No substrate pool can be zero with the utilization growth module
    substrate_pool_leaf      0.03       # No substrate pool can be zero with the utilization growth module
    substrate_pool_rhizome   0.00005    # No substrate pool can be zero with the utilization growth module
    substrate_pool_root      0.005      # No substrate pool can be zero with the utilization growth module
    substrate_pool_stem      0.005      # No substrate pool can be zero with the utilization growth module
    TTc                      0"

    data_frame = utils::read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})

# Do the calculations inside an empty list so that temporary variables are not created in .Global.
manihot_esculenta_parameters = with(list(), {
    datalines =
    "symbol                                value
    absorptivity_par                       0.8
    alpha1                                 0
    alphab1                                0
    atmospheric_pressure                   101325
    b0                                     0.08
    b1                                     5
    Catm                                   400
    chil                                   0.8
    electrons_per_carboxylation            4.5
    electrons_per_oxygenation              5.25
    grain_TTc                              950
    growth_respiration_fraction            0
    Gs_min                                 1e-3
    heightf                                3
    hydrDist                               0
    iSp                                    2.27
    jmax                                   226
    kd                                     0.37
    KmGrain                                0.836660027
    KmLeaf                                 0.632455532
    KmRhizome                              0.774596669
    KmRoot                                 0.774596669
    KmStem                                 0.707106781
    kpLN                                   0.2
    lat                                    40
    LeafN                                  2
    LeafN_0                                2
    leaf_reflectance                       0.2
    leaf_transmittance                     0.2
    lnb0                                   -5
    lnb1                                   18
    lnfun                                  0
    longitude                              -88
    minimum_gbw                            0.08
    nlayers                                10
    O2                                     210
    par_energy_content                     0.235
    par_energy_fraction                    0.5
    phi1                                   0.01
    phi2                                   2
    rate_constant_grain                    3
    rate_constant_leaf                     0.5
    rate_constant_leaf_senescence          0.0025
    rate_constant_rhizome                  0
    rate_constant_rhizome_senescence       0
    rate_constant_root                     0.8
    rate_constant_root_senescence          0.0018
    rate_constant_stem                     1.2
    rate_constant_stem_senescence          0.004
    Rd                                     3
    remobilization_fraction                0.6
    resistance_leaf_to_stem                0.16
    resistance_stem_to_grain               8
    resistance_stem_to_rhizome             1
    resistance_stem_to_root                0.16
    rfl                                    0.2
    rsdf                                   0.44
    rsec                                   0.2
    soil_clod_size                         0.04
    soil_depth1                            0.0
    soil_depth2                            2.5
    soil_depth3                            10.0
    soil_reflectance                       0.2
    soil_transmission                      0.01
    sowing_time                            0
    specific_heat_of_air                   1010
    Sp_thermal_time_decay                  0
    tbase                                  0
    theta                                  0.7
    timestep                               1
    tpu_rate_max                           23
    vmax1                                  133
    vmax_n_intercept                       0
    water_stress_approach                  1
    windspeed_height                       5
    wsFun                                  2"

    data_frame = utils::read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})
