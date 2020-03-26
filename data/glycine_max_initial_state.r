# Do the calculations inside an empty list so that temporary variables are not created in .Global.
glycine_max_initial_state = with(list(), {
    datalines =
    "symbol value
    Rhizome 0.0001	# No tissue mass can be zero with the utilization growth module, so just leave Rhizome very small
    Leaf 0.06  # The total of Leaf, Stem and Root masses is 0.08, which comes from http://www.montana.edu/cpa/news/wwwpb-archives/ag/baudr182.html and https://www.extension.purdue.edu/extmedia/ay/ay-217-w.pdf.
    Stem 0.01
    Root 0.01
    Grain 0.0001
    soil_water_content 0.32
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
    substrate_pool_leaf 0.03		# No substrate pool can be zero with the utilization growth module
    substrate_pool_grain 0.00005	# No substrate pool can be zero with the utilization growth module
    substrate_pool_stem 0.005		# No substrate pool can be zero with the utilization growth module
    substrate_pool_root 0.005		# No substrate pool can be zero with the utilization growth module
    substrate_pool_rhizome 0.00005	# No substrate pool can be zero with the utilization growth module
    GI 0
    FKF1 0
    CDF 0
    FT 0
    dawn 100"
    
    data_frame = utils::read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})
