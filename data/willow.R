# Some modules are included as named list elements so they can be easily changed
# on-the-fly to a different value, e.g.,
# CROP_direct_modules[['canopy_photosynthesis']] <- 'ten_layer_rue_canopy'
willow_direct_modules <- list(
    "soil_type_selector",
    stomata_water_stress = "stomata_water_stress_linear",
    "leaf_water_stress_exponential",
    "parameter_calculator",
    "soil_evaporation",
    canopy_photosynthesis = "c3_canopy",
    partitioning_coefficients = "partitioning_coefficient_selector",
    partitioning_growth_calculator = "partitioning_growth_calculator"
)

willow_differential_modules <- list(
    senescence = "thermal_time_and_frost_senescence",
    "partitioning_growth",
    thermal_time = "thermal_time_linear",
    soil_profile = "two_layer_soil_profile"
)

# Error tolerances greater than 1e-5 may cause problems with the regression test
willow_ode_solver <- list(
    type = 'auto',
    output_step_size = 1.0,
    adaptive_rel_error_tol = 1e-5,
    adaptive_abs_error_tol = 1e-5,
    adaptive_max_steps = 200
)

# Do the calculations inside an empty list so that temporary variables are not created in .Global.
willow_initial_values = with(list(), {
    datalines =
    "symbol                  value
    cws1                     0.32
    cws2                     0.32
    Grain                    0
    Leaf                     0.02
    leafdeathrate            5
    LeafLitter               0
    Rhizome                  0.99
    RhizomeLitter            0
    rhizome_senescence_index 0
    Root                     1
    RootLitter               0
    root_senescence_index    0
    soil_water_content       0.32
    Stem                     0.99
    StemLitter               0
    stem_senescence_index    0
    TTc                      0"

    data_frame = utils::read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})

# Do the calculations inside an empty list so that temporary variables are not created in .Global.
willow_parameters = with(list(), {
    datalines =
    "symbol                        value
    absorptivity_par               0.8
    alpha1                         0
    alphab1                        0
    atmospheric_pressure           101325
    b0                             0.08
    b1                             5
    Catm                           400
    chil                           1
    electrons_per_carboxylation    4.5
    electrons_per_oxygenation      5.25
    growth_respiration_fraction    0.3
    Gs_min                         1e-3
    heightf                        3
    hydrDist                       0
    iSp                            1.1
    jmax                           180
    kd                             0.37
    kGrain1                        0
    kGrain2                        0
    kGrain3                        0
    kGrain4                        0
    kGrain5                        0
    kGrain6                        0
    kLeaf1                         0.98
    kLeaf2                         0.98
    kLeaf3                         0.15
    kLeaf4                         0.15
    kLeaf5                         1e-05
    kLeaf6                         1e-06
    kpLN                           0.2
    kRhizome1                      -8e-04
    kRhizome2                      0.007
    kRhizome3                      0.105
    kRhizome4                      0.105
    kRhizome5                      0.15
    kRhizome6                      0.15
    kRoot1                         0.01
    kRoot2                         0.003
    kRoot3                         0.045
    kRoot4                         0.045
    kRoot5                         0.15
    kRoot6                         0.15
    kStem1                         0.01
    kStem2                         0.01
    kStem3                         0.7
    kStem4                         0.7
    kStem5                         0.7
    kStem6                         0.7
    lat                            40
    LeafN                          2
    LeafN_0                        2
    leaf_reflectance               0.2
    leaf_transmittance             0.2
    lnb0                           -5
    lnb1                           18
    lnfun                          0
    mrc1                           0.02
    mrc2                           0.03
    nlayers                        10
    O2                             210
    par_energy_content             0.235
    par_energy_fraction            0.5
    phi1                           0.01
    phi2                           2
    Rd                             1.1
    remobilization_fraction        0.6
    retrans                        0.9
    retrans_rhizome                1.0
    rfl                            0.2
    rsdf                           0.44
    rsec                           0.2
    seneLeaf                       1600
    seneRhizome                    5500
    seneRoot                       5500
    seneStem                       5500
    soil_clod_size                 0.04
    soil_depth1                    0.0
    soil_depth2                    2.5
    soil_depth3                    10.0
    soil_reflectance               0.2
    soil_transmission              0.01
    soil_type_indicator            6
    sowing_time                    0
    specific_heat_of_air           1010
    Sp_thermal_time_decay          0
    tbase                          0
    Tfrosthigh                     5
    Tfrostlow                      0
    theta                          0.7
    timestep                       1
    tp1                            250
    tp2                            350
    tp3                            900
    tp4                            1200
    tp5                            3939
    tpu_rate_max                   23
    vmax1                          100
    vmax_n_intercept               0
    water_stress_approach          0
    wsFun                          2"

    data_frame = utils::read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})
