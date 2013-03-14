
data(weather05)

## Some coefficients
pheno.ll <- phenoParms(kLeaf1=0.48,kStem1=0.47,kRoot1=0.05,kRhizome1=-1e-4,
                       kLeaf2=0.14,kStem2=0.65,kRoot2=0.21, kRhizome2=-1e-4,
                       kLeaf3=0.01, kStem3=0.56, kRoot3=0.13, kRhizome3=0.3, 
                       kLeaf4=0.01, kStem4=0.56, kRoot4=0.13, kRhizome4=0.3,
                       kLeaf5=0.01, kStem5=0.56, kRoot5=0.13, kRhizome5=0.3,
                       kLeaf6=0.01, kStem6=0.56, kRoot6=0.13, kRhizome6=0.3)

system.time(ans0 <- BioGro(weather05, phenoControl = pheno.ll))

ans0.dat <- as.data.frame(unclass(ans0)[1:20])
sel.rows <- seq(1,nrow(ans0.dat),400)
simDat <- ans0.dat[sel.rows,c("ThermalT","Stem","Leaf","Root","Rhizome","Grain","LAI")]
plot(ans0,simDat)

op.mc <- MCMCBioGro(phen=1, WetDat = weather05 , iCoef=idbp(simDat), niter = 20,
                    niter2 = 1000, data = simDat)

plot(op.mc, plot.kind="trace", subset=nams %in% c("kLeaf_1","kStem_1","kRoot_1"),layout=c(3,1))

