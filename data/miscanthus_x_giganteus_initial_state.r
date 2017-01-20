# Do the calculations inside an NULL environment so that temporary variables are not created in .Global.
miscanthus_x_giganteus_initial_state = with(NULL, {
    datalines =
    "symbol	value
    Rhizome	7
    Leaf	7e-04
    Stem	0.007
    Root	0.007
    Grain	0
    waterCont	0.32
    StomataWS	1
    LeafN	2
    TTc	0
    LeafLitter	0
    RootLitter	0
    RhizomeLitter	0
    StemLitter	0
    LeafWS	0	
    leaf_senescence_index	0
    stem_senescence_index	0
    root_senescence_index	0
    rhizome_senescence_index	0"
    
    data_frame = read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})

