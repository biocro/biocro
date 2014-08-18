## Testing the different optimization methods 

data(weather05)

## Some coefficients
pheno.ll <- phenoParms(kLeaf1=0.48,kStem1=0.47,kRoot1=0.05,kRhizome1=-1e-4,
                       kLeaf2=0.14,kStem2=0.64,kRoot2=0.21, kRhizome2=-1e-4,
                       kLeaf3=0.01, kStem3=0.56, kRoot3=0.13, kRhizome3=0.3, 
                       kLeaf4=0.01, kStem4=0.56, kRoot4=0.13, kRhizome4=0.3,
                       kLeaf5=0.01, kStem5=0.56, kRoot5=0.13, kRhizome5=0.3,
                       kLeaf6=0.01, kStem6=0.56, kRoot6=0.13, kRhizome6=0.3)

system.time(ans <- BioGro(weather05, phenoControl = pheno.ll))

ans.dat <- as.data.frame(unclass(ans))
sel.rows <- seq(1,nrow(ans.dat),400)
simDat <- ans.dat[sel.rows,c(10,6,5,7:9)]
plot(ans,simDat)

## Residual sum of squares before the optimization

ans0 <- BioGro(weather05)
RssBioGro(simDat,ans0)

## Limited testing suggests that this method works best
## phen = 0 optimizes all phenological stages sequentially
## Some may not converge and can be re-run
## Can also try other methods
system.time(op <- OpBioGro(phen=0, WetDat=weather05, data = simDat)
## or
## system.time(cop <- constrOpBioGro(phen = 0, WetDat = weather05, data = simDat))
            
phenoP <- phenoParms()
phenoP[7:30] <- op$coefs[7:30-6]

ans <- BioGro(WetDat = weather05, phenoControl = phenoP)
plot(ans, simDat)




