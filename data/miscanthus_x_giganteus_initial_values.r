# Do the calculations inside an empty list so that temporary variables are not created in .Global.
miscanthus_x_giganteus_initial_values = with(list(), {
    datalines =
    "symbol                     value
    Grain                       0
    Leaf                        7e-04
    LeafLitter                  0
    leaf_senescence_index       0
    Rhizome                     7
    RhizomeLitter               0
    rhizome_senescence_index    0
    Root                        0.007
    RootLitter                  0
    root_senescence_index       0
    soil_water_content          0.32
    Stem                        0.007
    StemLitter                  0
    stem_senescence_index       0
    TTc                         0"

    data_frame = utils::read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})
