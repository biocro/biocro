# Do the calculations inside an empty list so that temporary variables are not created in .Global.
soybean_parameters = with(list(), {
    datalines =
    "symbol                                 value
    # soil_type_selector module
    soil_type_indicator                     6

    # parameter_calculator module
    iSp                                     3.5         # 2002 average lai / leaf biomass, Dermody et al. 2006 (https://doi.org/10.1111/j.1469-8137.2005.01565.x), Morgan et al. 2005 (https://doi.org/10.1111/j.1365-2486.2005.001017.x)
    Sp_thermal_time_decay                   0           # not used in Soybean-BioCro (see Note 1 at end of file), but must be defined
    LeafN                                   2           # not used in Soybean-BioCro, but must be defined
    LeafN_0                                 2           # not used in Soybean-BioCro, but must be defined
    vmax_n_intercept                        0           # not used in Soybean-BioCro, but must be defined
    vmax1                                   110         # Bernacchi et. al. 2005 (https://doi.org/10.1007/s00425-004-1320-8), 2002 Seasonal average
    alphab1                                 0           # not used in Soybean-BioCro, but must be defined
    alpha1                                  0           # not used in Soybean-BioCro, but must be defined

    # soybean_development_rate_calculator module
    maturity_group                          3           # dimensionless; soybean cultivar maturity group
    Tbase_emr                               10          # degrees C
    TTemr_threshold                         60          # degrees C * days
    Rmax_emrV0                              0.1990      # day^-1; Setiyono et al., 2007 (https://doi.org/10.1016/j.fcr.2006.07.011), Table 2
    Tmin_emrV0                              5.0         # degrees C; Setiyono et al., 2007, Table 2
    Topt_emrV0                              31.5        # degrees C; Setiyono et al., 2007, Table 2
    Tmax_emrV0                              45.0        # degrees C; Setiyono et al., 2007, Table 2
    Tmin_R0R1                               5.0         # degrees C; Setiyono et al., 2007, Table 2 (used emrV0 values)
    Topt_R0R1                               31.5        # degrees C; Setiyono et al., 2007, Table 2 (used emrV0 values)
    Tmax_R0R1                               45.0        # degrees C; Setiyono et al., 2007, Table 2 (used emrV0 values)
    Tmin_R1R7                               0.0         # degrees C; Setiyono et al., 2007, Table 2
    Topt_R1R7                               21.5        # degrees C; Setiyono et al., 2007, Table 2
    Tmax_R1R7                               38.7        # degrees C; Setiyono et al., 2007, Table 2

    # partitioning_coefficient_logistic module
    alphaRoot                               34.39
    betaRoot                                -35.79
    alphaStem                               20.66
    betaStem                                -17.83
    alphaLeaf                               22.79
    betaLeaf                                -20.51
    kRhizome_emr                            0

    # soil_evaporation module
    rsec                                    0.2
    soil_clod_size                          0.04
    soil_reflectance                        0.2
    soil_transmission                       0.01
    specific_heat_of_air                    1010
    stefan_boltzman                         5.67e-8

    # solar_zenith_angle module
    lat                                     40

    # shortwave_atmospheric_scattering module
    atmospheric_pressure                    101325
    atmospheric_transmittance               0.85
    atmospheric_scattering                  0.3

    # incident_shortwave_from_ground_par module
    par_energy_fraction_of_sunlight         0.5
    par_energy_content                      0.235

    # ten_layer_canopy_properties module
    absorptivity_par                        0.8         # Campbell and Norman, An Introduction to Environmental Biophysics, 2nd Edition
    chil                                    0.81        # Campbell and Norman, An Introduction to Environmental Biophysics, 2nd Edition, Table 15.1, pg 253
    kd                                      0.7         # Estimated from Campbell and Norman, An Introduction to Environmental Biophysics, 2nd Edition, Figure 15.4, pg 254
    heightf                                 3           # m^-1
    kpLN                                    0           # not used in Soybean-BioCro (see Note 1 at end of file)
    lnfun                                   0           # not used in Soybean-BioCro

    # ten_layer_c3_canopy module
    jmax                                    195         # Bernacchi et al. 2005 (https://doi.org/10.1007/s00425-004-1320-8), 2002 Seasonal average
    electrons_per_carboxylation             4.5         # Bernacchi et al. 2003 (https://doi.org/10.1046/j.0016-8025.2003.01050.x)
    electrons_per_oxygenation               5.25        # Bernacchi et al. 2003 (https://doi.org/10.1046/j.0016-8025.2003.01050.x)
    tpu_rate_max                            13
    Rd                                      1.28        # Davey et al. 2004 (https://doi.org/10.1104/pp.103.030569), Table 3, cv Pana, co2 368 ppm
    Catm                                    372         # micromol / mol, CO2 level in 2002
    O2                                      210         # millimol / mol
    b0                                      0.008       # Leakey et al. 2006 (https://10.1111/j.1365-3040.2006.01556.x)
    b1                                      10.6        # Leakey et al. 2006 (https://10.1111/j.1365-3040.2006.01556.x)
    Gs_min                                  1e-3
    theta                                   0.76        # Bernacchi et al. 2003 (https://doi.org/10.1046/j.0016-8025.2003.01050.x)
    water_stress_approach                   1

    # ten_layer_canopy_integrator module
    growth_respiration_fraction             0

    # no_leaf_resp_partitioning_growth_calculator_negative_assimilation_partitioned module
    mrc1                                    0.0008333   # hour^-1, Bunce and Ziska 1996 (https://doi.org/10.1006/anbo.1996.0061)
    mrc2                                    0.000025    # 0.03 * mrc1, ratio used in previous BioCro crops

    # partitioning_growth module
    retrans                                 0.9         # previously hard-coded in the partitioning_growth module
    retrans_rhizome                         1.0         # previously hard-coded in the partitioning_growth module

    # senescence_coefficient_logistic module
    rateSeneLeaf                            0.01213
    rateSeneStem                            0.0005760
    rateSeneRoot                            0           # senescence of root not simulated in Soybean-BioCro (see Note 1 at end of file)
    rateSeneRhizome                         0           # no rhizome simulated in Soybean-BioCro
    alphaSeneLeaf                           30.05
    alphaSeneStem                           22.54
    alphaSeneRoot                           10          # senescence of root not simulated in Soybean-BioCro (rateSeneRoot=0)
    alphaSeneRhizome                        10          # no rhizome in Soybean-BioCro (rateSeneRhizome=0)
    betaSeneLeaf                            -17.83
    betaSeneStem                            -15.61
    betaSeneRoot                            -10         # senescence of root not simulated in Soybean-BioCro (rateSeneRoot=0)
    betaSeneRhizome                         -10         # no rhizome in Soybean-BioCro (rateSeneRhizome=0)

    # thermal_time_senescence_logistic module
    remobilization_fraction                 0.6

    # two_layer_soil_profile module
    soil_depth1                             0.0         # meters
    soil_depth2                             2.5         # meters
    soil_depth3                             10.0        # meters
    wsFun                                   2           # not used, but must be defined
    hydrDist                                0           # same as in sorghum parameter file
    rfl                                     0.2         # same as in sorghum parameter file
    rsdf                                    0.44        # same as in sorghum parameter file
    phi1                                    0.01
    phi2                                    1.5         # from Sugarcane-BioCro, Jaiswal et al. 2017 (https://doi.org/10.1038/nclimate3410)

    # thermal_time_linear module
    tbase                                   10          # degrees C

    # system parameters
    timestep                                1
    "

    data_frame = utils::read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})

# Note 1: Soybean-BioCro refers to the simulation scenarios defined by the the soybean
#         data files (soybean_initial_state, soybean_parameters, soybean_modules). See
#         Matthews et al. (doi: TBA) for more on the current version of Soybean-BioCro.

