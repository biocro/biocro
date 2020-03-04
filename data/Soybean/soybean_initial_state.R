# Do the calculations inside an empty list so that temporary variables are not created in .Global.
soybean_initial_state = with(list(), {
    datalines =
    "symbol value
    ### Number of seeds per meter * weight per seed (in grams) / row spacing (in meters) (gives g/m2, divide by 100 for Mg/ha)
    ### Number of seeds per meter = 20 (Morgan et al., 2004, doi: https://doi.org/10.1104/pp.104.043968)
    ### weight per seed (in grams) = 0.15 grams (https://www.feedipedia.org/node/42, average of .12 to .18 grams)
    ### row spacing (in meters) = 0.38 meters (Morgan et al., 2004, doi: https://doi.org/10.1104/pp.104.043968)
    ### 20 * 0.15 / 0.38 = 7.89 g/m2 = 0.0789 Mg/ha
    Rhizome 0.0000001  #Mg/ha
    Leaf    0.06312   #Mg/ha, 80% of total seed mass per land area
    Stem    0.00789  #Mg/ha, 10% of total seed mass per land area
    Root    0.00789   #Mg/ha, 10% of total seed mass per land area
    Grain   0.00001 #Mg/ha # treating this as the Pod/reproductive (minus seed)
    soil_water_content 0.32
    LeafN 2
    TTc 0
    LeafLitter 0
    RootLitter 0
    RhizomeLitter 0
    StemLitter 0
    rhizome_senescence_index 0
    ## needed for two layer soil profile
    cws1    0.32 # equal to initial soil_water_content, from Justins sorghum parameters (see 10/3/19 email -mlm)
    cws2    0.32 # equal to initial soil_water_content, from Justins sorghum parameters (see 10/3/19 email -mlm)
    soil_evaporation_rate   0
    DVI -1
    "
    
    data_frame = read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})
