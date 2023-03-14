soybean <- list(
    direct_modules = list(
        stomata_water_stress = "BioCro:stomata_water_stress_linear",
        "BioCro:parameter_calculator",
        "BioCro:soybean_development_rate_calculator",
        leaf_water_stress = "BioCro:leaf_water_stress_exponential",
        partitioning_coefficients = "BioCro:partitioning_coefficient_logistic",
        "BioCro:soil_evaporation",
        solar_coordinates = "BioCro:solar_position_michalsky",
        "BioCro:shortwave_atmospheric_scattering",
        "BioCro:incident_shortwave_from_ground_par",
        "BioCro:ten_layer_canopy_properties",
        canopy_photosynthesis = "BioCro:ten_layer_c3_canopy",
        "BioCro:ten_layer_canopy_integrator",
        partitioning_growth_calculator = "BioCro:no_leaf_resp_neg_assim_partitioning_growth_calculator",
        "BioCro:senescence_coefficient_logistic"
    ),
    differential_modules = list(
        senescence = "BioCro:senescence_logistic",
        "BioCro:partitioning_growth",
        soil_profile = "BioCro:two_layer_soil_profile",
        "BioCro:development_index",
        thermal_time = "BioCro:thermal_time_linear"
    ),
    ode_solver = list(
        type = 'boost_rkck54',
        output_step_size = 1.0,
        adaptive_rel_error_tol = 1e-4,
        adaptive_abs_error_tol = 1e-4,
        adaptive_max_steps = 200
    ),
    initial_values = list(
        Leaf               = 0.06312,       # Mg / ha, 80% of total seed mass per land area
        Stem               = 0.00789,       # Mg / ha, 10% of total seed mass per land area
        Root               = 0.00789,       # Mg / ha, 10% of total seed mass per land area
        Grain              = 0.00001,       # Mg / ha, this is the seed part 
        Shell              = 0.00001,       # Mg / ha, this is the shell part 
        LeafLitter         = 0,             # Mg / ha
        RootLitter         = 0,             # Mg / ha
        StemLitter         = 0,             # Mg / ha
        soil_water_content = 0.32,          # dimensionless (m^3 / m^3), volume of water per volume of bulk soil
        cws1               = 0.32,          # dimensionless, current water status, soil layer 1
        cws2               = 0.32,          # dimensionless, current water status, soil layer 2
        DVI                = -1,            # Sowing date: DVI=-1
        TTc                = 0,             # degrees C * day, accumulated thermal time

        # Soybean does not have a rhizome, so these variables will not be used but must be defined
        Rhizome                = 0.0000001,     # Mg / ha
        RhizomeLitter          = 0              # Mg / ha
    ),
    parameters = list(
        # soil parameters (clay loam)
        soil_air_entry              = -2.6,
        soil_b_coefficient          = 5.2,
        soil_bulk_density           = 1.35,
        soil_clay_content           = 0.34,
        soil_field_capacity         = 0.32,
        soil_sand_content           = 0.32,
        soil_saturated_conductivity = 6.4e-05,
        soil_saturation_capacity    = 0.52,
        soil_silt_content           = 0.34,
        soil_wilting_point          = 0.2,

        # parameter_calculator module
        iSp                         = 3.5,         # 2002 average lai / leaf biomass, Dermody et al. 2006 (https://doi.org/10.1111/j.1469-8137.2005.01565.x), Morgan et al. 2005 (https://doi.org/10.1111/j.1365-2486.2005.001017.x)
        Sp_thermal_time_decay       = 0,           # not used in Soybean-BioCro, but must be defined
        LeafN                       = 2,           # not used in Soybean-BioCro, but must be defined
        LeafN_0                     = 2,           # not used in Soybean-BioCro, but must be defined
        vmax_n_intercept            = 0,           # not used in Soybean-BioCro, but must be defined
        vmax1                       = 110,         # Bernacchi et. al. 2005 (https://doi.org/10.1007/s00425-004-1320-8), 2002 Seasonal average
        alphab1                     = 0,           # not used in Soybean-BioCro, but must be defined
        alpha1                      = 0,           # not used in Soybean-BioCro, but must be defined

        # soybean_development_rate_calculator module
        maturity_group              = 3,           # dimensionless; soybean cultivar maturity group
        Tbase_emr                   = 10,          # degrees C
        TTemr_threshold             = 60,          # degrees C * days
        Rmax_emrV0                  = 0.1990,      # day^-1; Setiyono et al., 2007 (https://doi.org/10.1016/j.fcr.2006.07.011), Table 2
        Tmin_emrV0                  = 5.0,         # degrees C; Setiyono et al., 2007, Table 2
        Topt_emrV0                  = 31.5,        # degrees C; Setiyono et al., 2007, Table 2
        Tmax_emrV0                  = 45.0,        # degrees C; Setiyono et al., 2007, Table 2
        Tmin_R0R1                   = 5.0,         # degrees C; Setiyono et al., 2007, Table 2 (used emrV0 values)
        Topt_R0R1                   = 31.5,        # degrees C; Setiyono et al., 2007, Table 2 (used emrV0 values)
        Tmax_R0R1                   = 45.0,        # degrees C; Setiyono et al., 2007, Table 2 (used emrV0 values)
        Tmin_R1R7                   = 0.0,         # degrees C; Setiyono et al., 2007, Table 2
        Topt_R1R7                   = 21.5,        # degrees C; Setiyono et al., 2007, Table 2
        Tmax_R1R7                   = 38.7,        # degrees C; Setiyono et al., 2007, Table 2
        sowing_time                 = 0,           # Soybean-BioCro uses the weather data to set the sowing time

        # partitioning_coefficient_logistic module
        alphaRoot                   = 41.1547,
        betaRoot                    = -39.67512,
        alphaStem                   = 30.8649,
        betaStem                    = -24.0506,
        alphaLeaf                   = 29.6565,
        betaLeaf                    = -22.8073,
        alphaShell                  = 19.2903,
        betaShell                   = -14.2270,
        kRhizome_emr                = 0,

        # soil_evaporation module
        rsec                        = 0.2,
        soil_clod_size              = 0.04,
        soil_reflectance            = 0.2,
        soil_transmission           = 0.01,
        specific_heat_of_air        = 1010,

        # solar_position_michalsky module
        lat                         = 40,
        longitude                   = -88,

        # shortwave_atmospheric_scattering module
        atmospheric_pressure        = 101325,
        atmospheric_transmittance   = 0.85,
        atmospheric_scattering      = 0.3,

        # incident_shortwave_from_ground_par module
        par_energy_fraction         = 0.5,
        par_energy_content          = 0.235,

        # ten_layer_canopy_properties module
        absorptivity_par            = 0.8,         # Campbell and Norman, An Introduction to Environmental Biophysics, 2nd Edition
        chil                        = 0.81,        # Campbell and Norman, An Introduction to Environmental Biophysics, 2nd Edition, Table 15.1, pg 253
        kd                          = 0.7,         # Estimated from Campbell and Norman, An Introduction to Environmental Biophysics, 2nd Edition, Figure 15.4, pg 254
        heightf                     = 3,           # m^-1
        kpLN                        = 0,           # not used in Soybean-BioCro
        leaf_reflectance            = 0.2,
        leaf_transmittance          = 0.2,
        lnfun                       = 0,           # not used in Soybean-BioCro

        # ten_layer_c3_canopy module
        jmax                        = 195,         # Bernacchi et al. 2005 (https://doi.org/10.1007/s00425-004-1320-8), 2002 Seasonal average
        jmax_mature                 = 195,         # Needed in the varying_Jmax25 module  
        sf_jmax                     = 0.2,         # Scaling factor for jmax. Needed in the varying_Jmax25 module  
        electrons_per_carboxylation = 4.5,         # Bernacchi et al. 2003 (https://doi.org/10.1046/j.0016-8025.2003.01050.x)
        electrons_per_oxygenation   = 5.25,        # Bernacchi et al. 2003 (https://doi.org/10.1046/j.0016-8025.2003.01050.x)
        tpu_rate_max                = 13,          # Fitted value based on the A-Ci data measured at UIUC in 2019-08 by Delgrado (unpublished data)
        Rd                          = 1.28,        # Davey et al. 2004 (https://doi.org/10.1104/pp.103.030569), Table 3, cv Pana, co2 368 ppm
        Catm                        = 372.59,      # micromol / mol, CO2 level in 2002
        O2                          = 210,         # millimol / mol
        b0                          = 0.008,       # Leakey et al. 2006 (https://10.1111/j.1365-3040.2006.01556.x)
        b1                          = 10.6,        # Leakey et al. 2006 (https://10.1111/j.1365-3040.2006.01556.x)
        Gs_min                      = 1e-3,
        theta                       = 0.76,        # Bernacchi et al. 2003 (https://doi.org/10.1046/j.0016-8025.2003.01050.x)
        water_stress_approach       = 1,
        minimum_gbw                 = 0.08,
        windspeed_height            = 5,

        # ten_layer_canopy_integrator module
        growth_respiration_fraction = 0,

        # no_leaf_resp_partitioning_growth_calculator_negative_assimilation_partitioned module
        mrc1                        = 0.0008333,   # hour^-1, Bunce and Ziska 1996 (https://doi.org/10.1006/anbo.1996.0061)
        mrc2                        = 0.000025,    # 0.03 * mrc1, ratio used in previous BioCro crops

        # partitioning_growth module
        retrans                     = 0.9,         # previously hard-coded in the partitioning_growth module
        retrans_rhizome             = 1.0,         # previously hard-coded in the partitioning_growth module

        # senescence_coefficient_logistic module
        rateSeneLeaf                = 0.003384,
        rateSeneStem                = 0.0007075,
        rateSeneRoot                = 0,           # senescence of root not simulated in Soybean-BioCro
        rateSeneRhizome             = 0,           # no rhizome simulated in Soybean-BioCro
        alphaSeneLeaf               = 40.066,
        alphaSeneStem               = 17.7961,
        alphaSeneRoot               = 10,          # senescence of root not simulated in Soybean-BioCro (rateSeneRoot=0)
        alphaSeneRhizome            = 10,          # no rhizome in Soybean-BioCro (rateSeneRhizome=0)
        betaSeneLeaf                = -30.3006,
        betaSeneStem                = -10.9029,
        betaSeneRoot                = -10,         # senescence of root not simulated in Soybean-BioCro (rateSeneRoot=0)
        betaSeneRhizome             = -10,         # no rhizome in Soybean-BioCro (rateSeneRhizome=0)

        # thermal_time_senescence_logistic module
        remobilization_fraction     = 0.6,

        # two_layer_soil_profile module
        soil_depth1                 = 0.0,         # meters
        soil_depth2                 = 2.5,         # meters
        soil_depth3                 = 10.0,        # meters
        wsFun                       = 2,           # not used, but must be defined
        hydrDist                    = 0,           # same as in sorghum parameter file
        rfl                         = 0.2,         # same as in sorghum parameter file
        rsdf                        = 0.44,        # same as in sorghum parameter file
        phi1                        = 0.01,
        phi2                        = 1.5,         # from Sugarcane-BioCro, Jaiswal et al. 2017 (https://doi.org/10.1038/nclimate3410)

        # thermal_time_linear module
        tbase                       = 10,          # degrees C

        # litter_cover module
        # - This module is not always used, but we include this parameter for
        #   convenience.
        # - Based on 2021-2022 Energy Farm measurements, the final leaf litter
        #   is around 1.5 - 2.5 Mg / Ha and covers ~50% of the ground area near
        #   the plants.
        km_leaf_litter              = 2.0,      # Mg / ha

        # system parameters
        timestep                    = 1
    )
)
