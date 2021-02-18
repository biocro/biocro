# Do the calculations inside an empty list so that temporary variables are not created in .Global.
manihot_esculenta_parameters = with(list(), {
    datalines =
    "symbol                                value
    timestep                               1
    lat                                    40
    soil_clod_size                         0.04
    soil_reflectance                       0.2
    soil_transmission                      0.01
    specific_heat                          1010
    stefan_boltzman                        5.67e-8
    grain_TTc                              950
    iSp                                    2.27
    chil                                   0.8
    Sp_thermal_time_decay                  0
    nlayers                                10
    kd                                     0.37
    heightf                                3
    growth_respiration_fraction            0
    vmax1                                  133
    alpha1                                 0
    jmax                                   226
    tpu_rate_max                           23
    Rd                                     3
    Catm                                   400
    O2                                     210
    b0                                     0.08
    b1                                     5
    Gs_min                                 1e-3
    theta                                  0.7
    water_stress_approach                  1
    rate_constant_leaf                     0.5
    rate_constant_stem                     1.2
    rate_constant_root                     0.8
    rate_constant_rhizome                  0
    rate_constant_grain                    3
    KmLeaf                                 0.632455532
    KmStem                                 0.707106781
    KmRoot                                 0.774596669
    KmRhizome                              0.774596669
    KmGrain                                0.836660027
    resistance_leaf_to_stem                0.16
    resistance_stem_to_grain               8
    resistance_stem_to_root                0.16
    resistance_stem_to_rhizome             1
    rate_constant_leaf_senescence          0.0025
    rate_constant_stem_senescence          0.004
    rate_constant_root_senescence          0.0018
    rate_constant_rhizome_senescence       0
    remobilization_fraction                0.6
    tbase                                  0
    phi1                                   0.01
    phi2                                   10
    soil_depth                             1
    soil_type_indicator                    6
    rsec                                   0.2
    LeafN_0                                2
    LeafN                                  2
    vmax_n_intercept                       0
    alphab1                                0
    kpLN                                   0.2
    lnb0                                   -5
    lnb1                                   18
    lnfun                                  0
    electrons_per_carboxylation            4.5
    electrons_per_oxygenation              5.25"

    data_frame = utils::read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})
