# Some modules are included as named list elements so they can be easily changed
# on-the-fly to a different value, e.g.,
# CROP_direct_modules[['canopy_photosynthesis']] <- 'ten_layer_rue_canopy'
zea_mays_direct_modules <- list(
    "soil_type_selector",
    stomata_water_stress = "stomata_water_stress_linear",
    "leaf_water_stress_exponential",
    "parameter_calculator",
    "thermal_time_development_rate_calculator",
    "soil_evaporation",
    solar_coordinates = "solar_position_michalsky",
    canopy_photosynthesis = "c4_canopy",
    partitioning_coefficients = "partitioning_coefficient_logistic",
    partitioning_growth_calculator = "partitioning_growth_calculator",
    "senescence_coefficient_logistic"
)

zea_mays_differential_modules <- list(
    senescence = "senescence_logistic",
    "partitioning_growth",
    "development_index",
    thermal_time = "thermal_time_bilinear",
    soil_profile = "two_layer_soil_profile"
)

# Error tolerances greater than 1e-5 may cause problems with the regression test
zea_mays_ode_solver <- list(
    type = 'auto',
    output_step_size = 1.0,
    adaptive_rel_error_tol = 1e-5,
    adaptive_abs_error_tol = 1e-5,
    adaptive_max_steps = 200
)

# Do the calculations inside an empty list so that temporary variables are not created in .Global.
zea_mays_initial_values = with(list(), {
    datalines =
    "symbol                     value
    cws1                        0.32
    cws2                        0.32
    DVI                         -1       # upon sowing
    Grain                       0
    Leaf                        0.00001
    LeafLitter                  0
    leaf_senescence_index       0
    Rhizome                     0.008886 # Page 130 of March 2014 lab book
    RhizomeLitter               0
    rhizome_senescence_index    0
    Root                        0.00001
    RootLitter                  0
    root_senescence_index       0
    soil_water_content          0.32
    Stem                        0.00001
    StemLitter                  0
    stem_senescence_index       0
    TTc                         0
    "

    data_frame = utils::read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})

# Do the calculations inside an empty list so that temporary variables are not created in .Global.
zea_mays_parameters = with(list(), {
    datalines =
    "symbol                     value
    # c4_canopy params
    absorptivity_par            0.8
    alpha1                      0.04
    alphab1                     0
    atmospheric_pressure        101325
    b0                          0.017    # Miner et al. 2017 (https://doi.org/10.1111/pce.12990)
    b1                          4.2      # Miner et al. 2017 (https://doi.org/10.1111/pce.12990)
    beta                        0.7      # Refered to as theta in Massad et al. 2007 (https://doi.org/10.1111/j.1365-3040.2007.01691.x)
    Catm                        400
    chil                        2.6      # An estimate for maize. Table 15.1, page 253 of Campbell and Norman. An introduction to environmental biophysics. 2nd edition.
    et_equation                 0
    Gs_min                      1e-3
    heightf                     2.5      # Leaf Area Density from Sinoquet et al. 1991. Fig 8. (https://doi.org/10.1016/0168-1923(91)90064-W)
    kd                          0.1
    kparm                       0.56     # Leakey et al. (https://doi.org/10.1104/pp.105.073957)
    kpLN                        0.2
    leafwidth                   0.04     # Bos, et al. 2000. Estimate of avg of 6 inbred North American lines in 4 temp conditions (https://doi.org/10.1016/S1573-5214(00)80013-5)
    leaf_reflectance            0.2
    leaf_transmittance          0.01
    lnfun                       0
    longitude                   -88
    lowerT                      3
    minimum_gbw                 0.34
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
    par_energy_content          0.235
    par_energy_fraction         0.5
    Rd                          0.8
    theta                       1        # Gilmanov et al. (DOI: 10.2111/REM-D-09-00072.1)
    upperT                      44
    vmax1                       36       # Yendrek et al. 2017. Fig 5E, Average of Amb 03 Inbred maize 2014 and 2015 (https://doi.org/10.1104/pp.16.01447)
    vmax_n_intercept            0
    water_stress_approach       1

    # partitioning_growth module
    retrans                     0.9
    retrans_rhizome             1.0

    # partitioning_coefficient_logistic module
    alphaLeaf                   13.0
    alphaRoot                   13.5     # Osborne et al. (doi:10.5194/gmd-8-1139-2015)
    alphaStem                   12.5
    betaLeaf                    -14.0
    betaRoot                    -15.5
    betaStem                    -12.5
    kRhizome_emr                0

    # senescence_coefficient_logistic module
    # TODO: must find development data to fit, plugging for now inspired by soy
    # a/b ratio seems ideal and not sensitive. Biggest impact is rate
    alphaSeneLeaf               46
    alphaSeneRhizome            10
    alphaSeneRoot               10
    alphaSeneStem               46
    betaSeneLeaf                -12
    betaSeneRhizome             -10
    betaSeneRoot                -10
    betaSeneStem                -12
    rateSeneLeaf                0.006
    rateSeneRhizome             0.0000001
    rateSeneRoot                0.0000001
    rateSeneStem                0.0002
    remobilization_fraction     0.65     # Ciampitti et al. 2013 (https://doi.org/10.2134/agronj2012.0467)

    # solar_zenith_angle module
    lat                         35

    # parameter_calculator module
    iSp                         3.5      # Amanullah et al. 2007. World Applied Sciences Journal averages and Massignam et al. 2011 (http://dx.doi.org/10.1071/CP11165)
    LeafN                       10000    # mmol m^-2, Massignam et al. 2011 (http://dx.doi.org/10.1071/CP11165)
    LeafN_0                     10000

    # soil_evaporation module
    hydrDist                    0        # same as in sorghum parameter file
    phi1                        0.01
    phi2                        10
    rfl                         0.2      # same as in sorghum parameter file
    rsdf                        0.44     # same as in sorghum parameter file
    rsec                        0.2
    soil_clod_size              0.04
    soil_depth1                 0.0
    soil_depth2                 2.5
    soil_depth3                 10.0
    soil_reflectance            0.2
    soil_transmission           0.01
    soil_type_indicator         6
    sowing_time                 0
    specific_heat_of_air        1010
    Sp_thermal_time_decay       0
    wsFun                       2        # not used, but must be defined

    # thermal_time_development_rate_calculator module
    tbase_emr                   10       # Maize Phasic Development Chapter 4 of Kiniry. Modeling Plant and Soil Systems. Vol 31. 1991
    TTemr                       45       # Maize Phasic Development Chapter 4 of Kiniry. Modeling Plant and Soil Systems. Vol 31. 1991
    TTrep                       1300     # Osborne et al. (doi:10.5194/gmd-8-1139-2015)
    TTveg                       900      # Osborne et al. (doi:10.5194/gmd-8-1139-2015)


    # thermal_time_bilinear module
    tbase                       8        # Maize Phasic Development Chapter 4 of Kiniry. Modeling Plant and Soil Systems. Vol 31. 1991
    tmax                        44       # Maize Phasic Development Chapter 4 of Kiniry. Modeling Plant and Soil Systems. Vol 31. 1991
    topt                        34       # Maize Phasic Development Chapter 4 of Kiniry. Modeling Plant and Soil Systems. Vol 31. 1991

    # required by run_biocro
    timestep                    1"

    data_frame = utils::read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})

# Note: Maize is photoperiod insensitive except for tassel initiation.
# The Osborne et al (doi:10.5194/gmd-8-1139-2015) model assumes
# absolutely no sensitivity, TODO: add photoperiod calculation to calculate TTveg.
# Tasseling triggered by longer night, day length < 15 critical, <10 optimal
#   pcrit_tassel                15      # Chen et al. (https://doi.org/10.1111/pce.12361)
#   popt_tassel                 10
