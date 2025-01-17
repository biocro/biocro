soybean <- list(
    direct_modules = list(
        "BioCro:format_time",
        stomata_water_stress = "BioCro:stomata_water_stress_linear",
        specific_leaf_area = "BioCro:sla_linear",
        "BioCro:parameter_calculator",
        "BioCro:soybean_development_rate_calculator",
        leaf_water_stress = "BioCro:leaf_water_stress_exponential",
        partitioning_coefficients = "BioCro:partitioning_coefficient_logistic",
        "BioCro:soil_evaporation",
        solar_coordinates = "BioCro:solar_position_michalsky",
        "BioCro:shortwave_atmospheric_scattering",
        "BioCro:incident_shortwave_from_ground_par",
        "BioCro:height_from_lai",
        "BioCro:canopy_gbw_thornley",
        "BioCro:stefan_boltzmann_longwave",
        "BioCro:ten_layer_canopy_properties",
        canopy_photosynthesis = "BioCro:ten_layer_c3_canopy",
        "BioCro:ten_layer_canopy_integrator",
        partitioning_growth_calculator = "BioCro:no_leaf_resp_neg_assim_partitioning_growth_calculator",
        "BioCro:senescence_coefficient_logistic"
    ),
    differential_modules = list(
        senescence = "BioCro:senescence_logistic",
        "BioCro:maintenance_respiration",
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

        # sla_linear module
        iSp                         = 3.5,         # 2002 average lai / leaf biomass, Dermody et al. 2006 (https://doi.org/10.1111/j.1469-8137.2005.01565.x), Morgan et al. 2005 (https://doi.org/10.1111/j.1365-2486.2005.001017.x)
        Sp_thermal_time_decay       = 0,           # not used in Soybean-BioCro, but must be defined

        # parameter_calculator module
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
        alphaLeaf                   = 23.8895,
        alphaStem                   = 24.8248,
        betaLeaf                    = -18.0851,
        betaStem                    = -18.5214,
        alphaRoot                   = 36.9670,
        betaRoot                    = -40.1915,
        alphaShell                  = 13.5955,
        betaShell                   = -9.9809,
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
        atmospheric_transmittance   = 0.6,         # Campbell and Norman, An Introduction to Environmental Biophysics, 2nd Edition, Pg 173
        atmospheric_scattering      = 0.3,

        # incident_shortwave_from_ground_par module
        par_energy_fraction         = 0.5,
        par_energy_content          = 0.219,       # W * s / micromole. Also J / micromole. Conversion
                                                   # from photon flux density to to irradiance for
                                                   # PAR. Equals 1/4.57. Plant Growth Chamber
                                                   # Handbook. CHAPTER 1 – RADIATION– John C. Sager and
                                                   # J. Craig McFarlane. Table 2, Pg 3
                                                   # (https://www.controlledenvironments.org/wp-content/uploads/sites/6/2017/06/Ch01.pdf)

        # height_from_lai module
        heightf                     = 6,           # m^-1; LAI of 6 when canopy is 1 m tall

        # canopy_gbw_thornley module
        min_gbw_canopy              = 0.005,       # m / s

        # stefan_boltzmann_longwave module
        emissivity_sky              = 1,

        # ten_layer_canopy_properties module
        chil                        = 0.81,        # Campbell and Norman, An Introduction to Environmental Biophysics, 2nd Edition, Table 15.1, pg 253
        k_diffuse                   = 0.7,         # Estimated from Campbell and Norman, An Introduction to Environmental Biophysics, 2nd Edition, Figure 15.4, pg 254
        kpLN                        = 0,           # not used in Soybean-BioCro
        leaf_reflectance_nir        = 0.42,
        leaf_reflectance_par        = 0.10,
        leaf_transmittance_nir      = 0.42,
        leaf_transmittance_par      = 0.05,
        lnfun                       = 0,           # not used in Soybean-BioCro

        # ten_layer_c3_canopy module (temperature response)
        Gstar_c    = 19.02,       # Table 1 of Bernacchi et al. 2001 (https://doi.org/10.1111/j.1365-3040.2001.00668.x)
        Gstar_Ea   = 37.83e3,     # Table 1 of Bernacchi et al. 2001 (https://doi.org/10.1111/j.1365-3040.2001.00668.x)
        Jmax_c     = 17.57,       # Table 1 of Bernacchi et al. 2003 (https://doi.org/10.1046/j.0016-8025.2003.01050.x)
        Jmax_Ea    = 43.54e3,     # Table 1 of Bernacchi et al. 2003 (https://doi.org/10.1046/j.0016-8025.2003.01050.x)
        Kc_c       = 38.05,       # Table 1 of Bernacchi et al. 2001 (https://doi.org/10.1111/j.1365-3040.2001.00668.x)
        Kc_Ea      = 79.43e3,     # Table 1 of Bernacchi et al. 2001 (https://doi.org/10.1111/j.1365-3040.2001.00668.x)
        Ko_c       = 20.30,       # Table 1 of Bernacchi et al. 2001 (https://doi.org/10.1111/j.1365-3040.2001.00668.x)
        Ko_Ea      = 36.38e3,     # Table 1 of Bernacchi et al. 2001 (https://doi.org/10.1111/j.1365-3040.2001.00668.x)
        phi_PSII_0 = 0.352,       # Table 2 of Bernacchi et al. 2003 (https://doi.org/10.1046/j.0016-8025.2003.01050.x)
        phi_PSII_1 = 0.022,       # Table 2 of Bernacchi et al. 2003 (https://doi.org/10.1046/j.0016-8025.2003.01050.x)
        phi_PSII_2 = -3.4e-4,     # Table 2 of Bernacchi et al. 2003 (https://doi.org/10.1046/j.0016-8025.2003.01050.x)
        Rd_c       = 18.72,       # Table 1 of Bernacchi et al. 2001 (https://doi.org/10.1111/j.1365-3040.2001.00668.x)
        Rd_Ea      = 46.39e3,     # Table 1 of Bernacchi et al. 2001 (https://doi.org/10.1111/j.1365-3040.2001.00668.x)
        theta_0    = 0.76,        # Table 2 of Bernacchi et al. 2003 (https://doi.org/10.1046/j.0016-8025.2003.01050.x)
        theta_1    = 0.018,       # Table 2 of Bernacchi et al. 2003 (https://doi.org/10.1046/j.0016-8025.2003.01050.x)
        theta_2    = -3.7e-4,     # Table 2 of Bernacchi et al. 2003 (https://doi.org/10.1046/j.0016-8025.2003.01050.x)
        Tp_c       = 19.77399,    # Chosen so that Tp_norm = 1 at 25 degrees C
        Tp_Ha      = 62.99e3,     # Figure 7 of Yang et al. 2016 (https://doi.org/10.1007/s00425-015-2436-8)
        Tp_Hd      = 182.14e3,    # Figure 7 of Yang et al. 2016 (https://doi.org/10.1007/s00425-015-2436-8)
        Tp_S       = 0.588e3,     # Figure 7 of Yang et al. 2016 (https://doi.org/10.1007/s00425-015-2436-8)
        Vcmax_c    = 26.35,       # Table 1 of Bernacchi et al. 2001 (https://doi.org/10.1111/j.1365-3040.2001.00668.x)
        Vcmax_Ea   = 65.33e3,     # Table 1 of Bernacchi et al. 2001 (https://doi.org/10.1111/j.1365-3040.2001.00668.x)

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
        windspeed_height            = 5,
        beta_PSII                   = 0.5,         # Bernacchi et al. 2003 (https://doi.org/10.1046/j.0016-8025.2003.01050.x)
        leafwidth                   = 0.1,         # Large mature leaflets can reach 10 cm in width

        # ten_layer_canopy_integrator module
        growth_respiration_fraction = 0,

        # no_leaf_resp_partitioning_growth_calculator_negative_assimilation_partitioned module
        grc_stem                        = 0.02828,    # dimensionless, optimized. growth respiration coefficient
        grc_root                        = 0.00270,    # dimensionless, optimized

        # maintenance respiration module
        mrc_leaf                        = 0.00048836,    # kg / kg / hr maintenance respiration coefficient. optimized
        mrc_stem                        = 0.00048836,    # kg / kg / hr, assumed to be same as leaf
        mrc_root                        = 0.00001,       # kg / kg / hr, optimized
        mrc_grain                       = 1e-5,          # kg / kg / hr, assigned a small value to ignore this

        # partitioning_growth module
        retrans                     = 0.9,         # previously hard-coded in the partitioning_growth module
        retrans_rhizome             = 1.0,         # previously hard-coded in the partitioning_growth module

        # senescence_coefficient_logistic module
        rateSeneLeaf                = 0.012198,
        rateSeneStem                = 0.000117,
        rateSeneRoot                = 0,           # senescence of root not simulated in Soybean-BioCro
        rateSeneRhizome             = 0,           # no rhizome simulated in Soybean-BioCro
        alphaSeneLeaf               = 48.3200,
        alphaSeneStem               = 4.1937,
        alphaSeneRoot               = 10,          # senescence of root not simulated in Soybean-BioCro (rateSeneRoot=0)
        alphaSeneRhizome            = 10,          # no rhizome in Soybean-BioCro (rateSeneRhizome=0)
        betaSeneLeaf                = -29.5393,
        betaSeneStem                = -3.3124,
        betaSeneRoot                = -10,         # senescence of root not simulated in Soybean-BioCro (rateSeneRoot=0)
        betaSeneRhizome             = -10,         # no rhizome in Soybean-BioCro (rateSeneRhizome=0)

        # thermal_time_senescence_logistic module
        remobilization_fraction     = 0.6,

        # two_layer_soil_profile module
        soil_depth1                 = 0.0,         # meters
        soil_depth2                 = 2.5,         # meters
        soil_depth3                 = 10.0,        # meters
        wsFun                       = 2,           # not used, but must be defined
        hydrDist                    = 0,           # same as in miscanthus parameter file
        rfl                         = 0.2,         # same as in miscanthus parameter file
        rsdf                        = 0.44,        # same as in miscanthus parameter file
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
