# Do the calculations inside an empty list so that temporary variables are not created in .Global.
soybean_parameters = with(list(), {
    datalines =
    "symbol                                 value
    # soil_type_selector module
    soil_type_indicator                     6
    
    # leaf_water_stress_exponential module
    phi2                                    1.5         # Jaiswal 2017 #10
    
    # parameter_calculator module
    iSp                                     3.5         # 2002 approx. avg (lai /leaf biomass)     #2.7
    Sp_thermal_time_decay                   0
    LeafN_0                                 2
    vmax_n_intercept                        0
    vmax1                                   110         # Bernacchi 2005 (https://doi.org/10.1007/s00425-004-1320-8), 2002 Seasonal average
    alphab1                                 0
    alpha1                                  0
     
    # soybean_development_rate_calculator module
    maturity_group                          3           # dimensionless; soybean cultivar maturity group 
    Tbase_emr                               10          # degrees C
    TTemr_threshold                         60          # degrees C * days
    Rmax_emrV0                              0.1990      # day^-1; Setiyono et al., 2007, Table 2
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
    absorptivity_par                        0.8
    chil                                    0.81        # Campbell and Norman, An Introduction to Environmental Biophysics, 2nd Edition, Table 15.1, pg 253
    kd                                      0.7         # Estimated from Campbell and Norman, An Introduction to Environmental Biophysics, 2nd Edition, Figure 15.4, pg 254
    heightf                                 3
    kpLN                                    0
    lnfun                                   0
    
    # ten_layer_c3_canopy module
    jmax                                    195         # Bernacchi 2005 (https://doi.org/10.1007/s00425-004-1320-8), 2002 Seasonal average
    electrons_per_carboxylation             4.5
    electrons_per_oxygenation               5.25
    tpu_rate_max                            23
    Rd                                      1.28        # Davey 2004, Table 3, cv Pana, co2 368 ppm#1.2 # Dermody 2006, Table 1 2002 shaded leaves, ambient (https://doi.org/10.1111/j.1469-8137.2005.01565.x) #1.4 # https://soyface.illinois.edu/sites/soyface.illinois.edu/files/uploads/Rosenthal2010.pdf #1.5 # from venkats PlantGro 2008 and 2009 soyface files   #1.1 
    Catm                                    372         # Rogers et al., 2004, https://doi.org/10.1111/j.1365-3040.2004.01163.x
    O2                                      210 
    b0                                      0.008       # Leakey 2006 (doi: 10.1111/j.1365-3040.2006.01556.x)
    b1                                      10.6        # Leakey 2006 (doi: 10.1111/j.1365-3040.2006.01556.x)
    Gs_min                                  1e-3
    theta                                   0.76        # Bernacchi 2003, https://doi.org/10.1046/j.0016-8025.2003.01050.x
    water_stress_approach                   1
    
    # ten_layer_canopy_integrator module
    growth_respiration_fraction             0
    
    # no_leaf_resp_partitioning_growth_calculator_negative_assimilation_partitioned module
    mrc1                                    0.0008333   # 0.02 / 24, Bunce 1995, https://pdfs.semanticscholar.org/712d/93eed86a6e4df6254a3f944442fd052def70.pdf
    mrc2                                    0.000025    # 0.03 * mrc1
    
    # senescence_coefficient_logistic module
    rateSeneLeaf                            0.01213 
    rateSeneStem                            0.0005760 
    rateSeneRoot                            0           # senescence of root not simulated in soybean-biocro
    rateSeneRhizome                         0           # no rhizome simulated in soybean-biocro
    alphaSeneLeaf                           30.05 
    alphaSeneStem                           22.54
    alphaSeneRoot                           10          # dummyvalue, senescence of root not simulated in soybean-biocro (rateSeneRoot=0)
    alphaSeneRhizome                        10          # dummyvalue, no rhizome in soybean-biocro (rateSeneRhizome=0)
    betaSeneLeaf                            -17.83 
    betaSeneStem                            -15.61
    betaSeneRoot                            -10         # dummyvalue, senescence of root not simulated in soybean-biocro (rateSeneRoot=0)
    betaSeneRhizome                         -10         # dummyvalue, no rhizome in soybean-biocro (rateSeneRhizome=0)
    
    # thermal_time_senescence_logistic module
    remobilization_fraction                 0.6 
    
    # two_layer_soil_profile module
    soil_depth1                             0.0         # from Justins sorghum parameters (see 10/3/19 email -mlm)
    soil_depth2                             2.5         # from Justins sorghum parameters (see 10/3/19 email -mlm)
    soil_depth3                             10.0        # from Justins sorghum parameters (see 10/3/19 email -mlm)
    wsFun                                   2           # not actually used, but must be defined
    hydrDist                                0           # copied from sorghum parameter file
    rfl                                     0.2         # copied from sorghum parameter file
    rsdf                                    0.44        # copied from sorghum parameter file
    phi1                                    0.01  
     
    # thermal_time_linear module
    tbase                                   10
    
    # system parameters
    timestep                                1
    "

    data_frame = utils::read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})
