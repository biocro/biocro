# Some modules are included as named list elements so they can be easily changed
# on-the-fly to a different value, e.g.,
# CROP_direct_modules[['canopy_photosynthesis']] <- 'ten_layer_rue_canopy'
glycine_max_direct_modules <- list(
    "soil_type_selector",
    stomata_water_stress = "stomata_water_stress_linear",
    "leaf_water_stress_exponential",
    "parameter_calculator",
    "soil_evaporation",
    canopy_photosynthesis = "c3_canopy",
    "utilization_growth_calculator",
    "utilization_senescence_calculator"
)

glycine_max_derivative_modules <- list(
    "utilization_senescence",
    "utilization_growth",
    thermal_time = "thermal_time_linear",
    soil_profile = "one_layer_soil_profile"
)

# Error tolerances greater than 1e-5 may cause problems with the regression test
glycine_max_integrator <- list(
    type = 'auto',
    output_step_size = 1.0,
    adaptive_rel_error_tol = 1e-5,
    adaptive_abs_error_tol = 1e-5,
    adaptive_max_steps = 200
)

# Do the calculations inside an empty list so that temporary variables are not created in .Global.
glycine_max_initial_values = with(list(), {
    datalines =
    "symbol                  value
    Grain                    0.0001
    Leaf                     0.06   # The total of Leaf, Stem and Root masses is 0.08, which comes from http://www.montana.edu/cpa/news/wwwpb-archives/ag/baudr182.html and https://www.extension.purdue.edu/extmedia/ay/ay-217-w.pdf.
    LeafLitter               0
    Rhizome                  0.0001 # No tissue mass can be zero with the utilization growth module, so just leave Rhizome very small
    RhizomeLitter            0
    Root                     0.01
    RootLitter               0
    soil_water_content       0.32
    Stem                     0.01
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
glycine_max_parameters = with(list(), {
    datalines =
    "symbol                                value
    absorptivity_par                       0.8
    alpha1                                 32.5
    alphab1                                0
    atmospheric_pressure                   101325
    b0                                     0.048
    b1                                     5
    Catm                                   370
    chil                                   0.8
    electrons_per_carboxylation            4.5
    electrons_per_oxygenation              5.25
    grain_TTc                              950
    growth_respiration_fraction            0
    Gs_min                                 1e-3
    heightf                                3
    iSp                                    2.5
    jmax                                   213.2
    kd                                     0.1
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
    nlayers                                10
    O2                                     210
    par_energy_content                     0.235
    par_energy_fraction                    0.5
    phi1                                   0.01
    phi2                                   10
    rate_constant_grain                    3
    rate_constant_leaf                     0.5
    rate_constant_leaf_senescence          0.0025
    rate_constant_rhizome                  0
    rate_constant_rhizome_senescence       0
    rate_constant_root                     0.8
    rate_constant_root_senescence          0.0018
    rate_constant_stem                     1.2
    rate_constant_stem_senescence          0.004
    Rd                                     1.1
    remobilization_fraction                0.6
    resistance_leaf_to_stem                0.16
    resistance_stem_to_grain               8
    resistance_stem_to_rhizome             1
    resistance_stem_to_root                0.16
    rsec                                   0.2
    soil_clod_size                         0.04
    soil_depth                             1
    soil_reflectance                       0.2
    soil_transmission                      0.01
    soil_type_indicator                    6
    specific_heat_of_air                   1010
    Sp_thermal_time_decay                  0
    tbase                                  10
    theta                                  0.7
    timestep                               1
    tpu_rate_max                           23
    vmax1                                  111.2
    vmax_n_intercept                       0
    water_stress_approach                  1"

    data_frame = utils::read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})
