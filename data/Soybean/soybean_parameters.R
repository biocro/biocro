# Do the calculations inside an empty list so that temporary variables are not created in .Global.
soybean_parameters = with(list(), {
    datalines =
    "symbol value
     lat           40
     soil_clod_size 0.04
     soil_reflectance 0.2
     soil_transmission 0.01
     specific_heat 1010
     stefan_boltzman 5.67e-8  
     iSp            3.5 # 2002 approx. avg (lai /leaf biomass)     #2.7      
     chil           0.81    # Campbell and Norman, An Introduction to Environmental Biophysics, 2nd Edition, Table 15.1, pg 253
     Sp_thermal_time_decay      0        
     nlayers            10       
     kd                 0.7 # Estimated from Campbell and Norman, An Introduction to Environmental Biophysics, 2nd Edition, Figure 15.4, pg 254   
     mrc1             0.0008333 # 0.02/24, Bunce 1995, https://pdfs.semanticscholar.org/712d/93eed86a6e4df6254a3f944442fd052def70.pdf #0.0005      #0.02
     mrc2             0.000025 #.03*mrc1 #0.000015    #0.03
     heightf       3        
     growth_respiration_fraction 0
     vmax1   110 # Bernacchi 2005 (https://doi.org/10.1007/s00425-004-1320-8), 2002 Seasonal average
     jmax   195 # Bernacchi 2005 (https://doi.org/10.1007/s00425-004-1320-8), 2002 Seasonal average
     Rd     1.28 # Davey 2004, Table 3, cv Pana, co2 368 ppm#1.2 # Dermody 2006, Table 1 2002 shaded leaves, ambient (https://doi.org/10.1111/j.1469-8137.2005.01565.x) #1.4 # https://soyface.illinois.edu/sites/soyface.illinois.edu/files/uploads/Rosenthal2010.pdf #1.5 # from venkats PlantGro 2008 and 2009 soyface files   #1.1 
     Catm   372 # Rogers et al., 2004, https://doi.org/10.1111/j.1365-3040.2004.01163.x
     O2     210 
     b0     0.008   # Leakey 2006 (doi: 10.1111/j.1365-3040.2006.01556.x)
     b1     10.6    # Leakey 2006 (doi: 10.1111/j.1365-3040.2006.01556.x)
     theta  0.76 #0.9 # Dermody 2006, Table 1 2002 shaded leaves (https://doi.org/10.1111/j.1469-8137.2005.01565.x) #0.76 # Bernacchi 2003, https://doi.org/10.1046/j.0016-8025.2003.01050.x
     water_stress_approach     1 
     maturity_group 3 # dimensionless; soybean cultivar maturity group 
     Tbase_emr  10 # degrees C
     TTemr_threshold    60 # degree C * days
     Rmax_emrV0 0.1990 # day^-1; Setiyono et al., 2007, Table 2
     Tmin_emrV0 5.0 # degrees C; Setiyono et al., 2007, Table 2
     Topt_emrV0 31.5 # degrees C; Setiyono et al., 2007, Table 2
     Tmax_emrV0 45.0 # degrees C; Setiyono et al., 2007, Table 2
     Tmin_R0R1 5.0 # degrees C; Setiyono et al., 2007, Table 2 (used emrV0 values)
     Topt_R0R1 31.5 # degrees C; Setiyono et al., 2007, Table 2 (used emrV0 values)
     Tmax_R0R1 45.0 # degrees C; Setiyono et al., 2007, Table 2 (used emrV0 values)
     Tmin_R1R7 0.0 # degrees C; Setiyono et al., 2007, Table 2
     Topt_R1R7 21.5 # degrees C; Setiyono et al., 2007, Table 2
     Tmax_R1R7 38.7 # degrees C; Setiyono et al., 2007, Table 2
     alphaRoot  25.04 
     betaRoot   -20.35
     alphaStem  24.44 
     betaStem   -19.16 
     alphaLeaf  21.38 
     betaLeaf   -15.88 
     kRhizome_emr 0
     alphaSeneLeaf  46.97 
     alphaSeneStem  21.67 
     betaSeneLeaf   -24.81 
     betaSeneStem   -10.33 
     rateSeneLeaf   0.01244 
     rateSeneStem   0.001592 
     remobilization_fraction    0
     tbase     10     
     phi1       0.01     
     phi2       1.5 #jaiswal 2017 #10       
     soil_depth  1        
     soil_type_indicator   6        
     rsec       0.2      
     LeafN_0   2   
     vmax_n_intercept  0   
     kpLN     0
     lnfun    0
     electrons_per_carboxylation    4.5
     electrons_per_oxygenation 5.25
     timestep   1
     alphab1    0
     alpha1     0
     lnb0   0
     lnb1   0
     ## params needed for two-layer-soil-profile
     soil_depth1    0.0 # from Justins sorghum parameters (see 10/3/19 email -mlm)
     soil_depth2    2.5 # from Justins sorghum parameters (see 10/3/19 email -mlm)
     soil_depth3    10.0 # from Justins sorghum parameters (see 10/3/19 email -mlm)
     wsFun  2 #exponential stomata water stress
     hydrDist   0 #copied from sorghum parameter file
     rfl    0.2 #copied from sorghum parameter file
     rsdf   0.44 #copied from sorghum parameter file
    "

    data_frame = read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})
