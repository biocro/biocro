# Do the calculations inside an empty list so that temporary variables are not created in .Global.
miscanthus_x_giganteus_parameters = with(list(), {
    datalines =
    "symbol	value
    lat	40
    iSp	1.7
    SpD	0
    nlayers	10
    kd	0.1
    chil	1
    heightf	3
    leafwidth	0.04
    et_equation	0
    seneLeaf	3000
    seneStem	3500
    seneRoot	4000
    seneRhizome	4000
    tbase	0
    vmax1	39
    alpha1	0.04
    kparm	0.7
    theta	0.83
    beta	0.93
    Rd	0.8
    Catm	380
    b0	0.08
    b1	3
    ws	1
    upperT	37.5
    lowerT	3
    FieldC	-1
    WiltP	-1
    phi1	0.01
    phi2	10
    soilDepth	1
    iWatCont	0.32
    soilType	6
    soilLayers	1
    wsFun	0
    scsf	1
    transpRes	5e+06
    leafPotTh	-800
    hydrDist	0
    rfl	0.2
    rsec	0.2
    rsdf	0.44
    SC1	1
    SC2	1
    SC3	1
    SC4	1
    SC5	1
    SC6	1
    SC7	1
    SC8	1
    SC9	1
    LeafL.Ln	0.17
    StemL.Ln	0.17
    RootL.Ln	0.17
    RhizL.Ln	0.17
    LeafL.N	0.004
    StemL.N	0.004
    RootL.N	0.004
    RhizL.N	0.004
    iMinN	0
    tp1	562
    tp2	1312
    tp3	2063
    tp4	2676
    tp5	3211
    tp6	7000
    kStem1	0.37
    kLeaf1	0.33
    kRoot1	0.3
    kRhizome1	-8e-04
    kStem2	0.85
    kLeaf2	0.14
    kRoot2	0.01
    kRhizome2	-5e-04
    kStem3	0.63
    kLeaf3	0.01
    kRoot3	0.01
    kRhizome3	0.35
    kStem4	0.63
    kLeaf4	0.01
    kRoot4	0.01
    kRhizome4	0.35
    kStem5	0.63
    kLeaf5	0.01
    kRoot5	0.01
    kRhizome5	0.35
    kStem6	0.63
    kLeaf6	0.01
    kRoot6	0.01
    kRhizome6	0.35
    kGrain6	0
    LeafN_0	2
    kln	0.5
    vmaxb1	0
    alphab1	0
    kpLN	0.2
    lnb0	-5
    lnb1	18
    lnfun	0
    nileafn	85
    nkln	0.5
    nvmaxb1	0.6938
    nvmaxb0	-16.25
    nalphab1	0.000488
    nalphab0	0.02367
    nRdb1	0.1247
    nRdb0	-4.5917
    nkpLN	0.17
    nlnb0	-5
    nlnb1	18
    timestep	1
    centTimestep	1
    doyNfert	0
    mrc1	0.02
    mrc2	0.03"
    
    data_frame = read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})

