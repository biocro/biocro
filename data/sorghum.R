# Some modules are included as named list elements so they can be easily changed
# on-the-fly to a different value, e.g.,
# CROP_steady_state_modules[['canopy_photosynthesis']] <- 'ten_layer_rue_canopy'
sorghum_steady_state_modules <- list(
    "soil_type_selector",
    stomata_water_stress = "stomata_water_stress_linear",
    "leaf_water_stress_exponential",
    "parameter_calculator",
    "soil_evaporation",
    canopy_photosynthesis = "c4_canopy",
    partitioning_coefficients = "partitioning_coefficient_selector",
    partitioning_growth_calculator = "partitioning_growth_calculator"
)

sorghum_derivative_modules <- list(
    senescence = "thermal_time_senescence",
    "partitioning_growth",
    thermal_time = "thermal_time_linear",
    soil_profile = "one_layer_soil_profile"
)

# Error tolerances greater than 1e-5 may cause problems with the regression test
sorghum_integrator <- list(
    type = 'auto',
    output_step_size = 1.0,
    adaptive_rel_error_tol = 1e-5,
    adaptive_abs_error_tol = 1e-5,
    adaptive_max_steps = 200
)

# Do the calculations inside an empty list so that temporary variables are not created in .Global.
sorghum_initial_values = with(list(), {
    datalines =
    "symbol                  value
    Grain                    0
    Leaf                     0.00001
    LeafLitter               0
    leaf_senescence_index    0
    Rhizome                  0.008886   # Page 130 of March 2014 lab book
    RhizomeLitter            0
    rhizome_senescence_index 0
    Root                     0.00001
    RootLitter               0
    root_senescence_index    0
    soil_water_content       0.32
    Stem                     0.00001
    StemLitter               0
    stem_senescence_index    0
    TTc                      0"

    data_frame = utils::read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})

# Do the calculations inside an empty list so that temporary variables are not created in .Global.
sorghum_parameters = with(list(), {
    datalines =
    "symbol                     value
    absorptivity_par            0.8
    alpha1                      0.04
    alphab1                     0
    atmospheric_pressure        101325
    b0                          0.08
    b1                          3
    beta                        0.93
    Catm                        400
    chil                        1.43    # An estimate for sorghum. Table 15.1, page 253 of Campbell and Norman. An introduction to environmental biophysics. 2nd edition.
    et_equation                 0
    Gs_min                      1e-3
    heightf                     3
    iSp                         1.7
    kd                          0.1
    kGrain1                     0
    kGrain2                     0
    kGrain3                     0
    kGrain4                     0
    kGrain5                     0
    kGrain6                     0
    kLeaf1                      0.8
    kLeaf2                      0.7
    kLeaf3                      0.49
    kLeaf4                      0.49
    kLeaf5                      0.49
    kLeaf6                      0.49
    kparm                       0.7
    kpLN                        0.2
    kRhizome1                   -0.0008
    kRhizome2                   0
    kRhizome3                   0
    kRhizome4                   0
    kRhizome5                   0
    kRhizome6                   0
    kRoot1                      0.1
    kRoot2                      0.01
    kRoot3                      0.01
    kRoot4                      0.01
    kRoot5                      0.01
    kRoot6                      0.01
    kStem1                      0.1
    kStem2                      0.29
    kStem3                      0.5
    kStem4                      0.5
    kStem5                      0.5
    kStem6                      0.5
    lat                         40
    LeafN                       2
    LeafN_0                     2
    leafwidth                   0.04
    lnb0                        -5
    lnb1                        18
    lnfun                       0
    lowerT                      3
    mrc1                        0.02
    mrc2                        0.03
    nalphab0                    0.02367
    nalphab1                    0.000488
    nileafn                     85
    nkln                        0.5
    nkpLN                       0.17
    nlayers                     10
    nlnb0                       -5
    nlnb1                       18
    nRdb0                       -4.5917
    nRdb1                       0.1247
    nvmaxb0                     -16.25
    nvmaxb1                     0.6938
    phi1                        0.01
    phi2                        10
    Rd                          0.8
    remobilization_fraction     0.6
    retrans                     0.9
    retrans_rhizome             1.0
    rsec                        0.2
    seneLeaf                    3000
    seneRhizome                 4000
    seneRoot                    4000
    seneStem                    3500
    soil_clod_size              0.04
    soil_depth                  1
    soil_reflectance            0.2
    soil_transmission           0.01
    soil_type_indicator         6
    specific_heat_of_air        1010
    Sp_thermal_time_decay       0
    stefan_boltzman             5.67e-8
    tbase                       0
    theta                       0.83
    timestep                    1
    tp1                         562
    tp2                         1312
    tp3                         2063
    tp4                         2676
    tp5                         3211
    upperT                      37.5
    vmax1                       39
    vmax_n_intercept            0
    water_stress_approach       1"

    data_frame = utils::read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})
