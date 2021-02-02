# Do the calculations inside an empty list so that temporary variables are not created in .Global.
manihot_esculenta_initial_state = with(list(), {
    datalines =
    "symbol value
    Rhizome 0.99
    Leaf 0.02
    Stem 0.99
    Root 1
    Grain 0
    waterCont 0.32
    LeafN 2
    TTc 0
    LeafLitter 0
    RootLitter 0
    RhizomeLitter 0
    StemLitter 0
    leaf_senescence_index 0
    stem_senescence_index 0
    root_senescence_index 0
    rhizome_senescence_index 0
    substrate_pool_leaf 0
    substrate_pool_grain 0
    substrate_pool_stem 0
    substrate_pool_root 0
    substrate_pool_rhizome 0"
    
    data_frame = utils::read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})

