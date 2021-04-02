# Do the calculations inside an empty list so that temporary variables are not created in .Global.

soybean_initial_state = with(list(), {
    datalines =
    "symbol                  value
    Leaf                     0.06312       # Mg / ha, 80% of total seed mass per land area (see comment at end of file)
    Stem                     0.00789       # Mg / ha, 10% of total seed mass per land area
    Root                     0.00789       # Mg / ha, 10% of total seed mass per land area
    Grain                    0.00001       # Mg / ha, We treat this as the soybean pod and refer to this variable as Pod in Matthews et al.
    LeafLitter               0             # Mg / ha
    RootLitter               0             # Mg / ha
    StemLitter               0             # Mg / ha
    soil_water_content       0.32          # dimensionless (m^3 / m^3), volume of water per volume of bulk soil
    cws1                     0.32          # dimensionless, current water status, soil layer 1
    cws2                     0.32          # dimensionless, current water status, soil layer 2
    LeafN                    2             # mmol / m^2, specific leaf nitrogen
    DVI                      -1            # Sowing date: DVI=-1 
    TTc                      0             # degrees C, accumulated thermal time
    
    # Soybean does not have a rhizome, so these variables will not be used but must be defined
    Rhizome                  0.0000001     # Mg / ha
    RhizomeLitter            0             # Mg / ha
    rhizome_senescence_index 0
    "
    
    data_frame = utils::read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})

# For the initial total seed mass per land area, we use the following equation:
# Number of seeds per meter * weight per seed * 1 / row spacing
#
# Number of seeds per meter = 20 (Morgan et al., 2004, https://doi.org/10.1104/pp.104.043968)
# weight per seed = 0.15 grams / seed (https://www.feedipedia.org/node/42, average of .12 to .18 grams)
# row spacing = 0.38 meters (Morgan et al., 2004)
# 
# (20 seeds / meter) * (0.15 grams / seed) * (1 / 0.38 meter) = 7.89 g / m^2 = 0.0789 Mg / ha
# This value is used to determine the initial Leaf, Stem, and Root biomasses

