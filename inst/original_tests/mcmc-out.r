## Script to analyze the output of MCMC object

data(weather05)

## Some coefficients
pheno.ll <- phenoParms(kLeaf1=0.48,kStem1=0.47,kRoot1=0.05,kRhizome1=-1e-4,
                       kLeaf2=0.14,kStem2=0.65,kRoot2=0.21, kRhizome2=-1e-4,
                       kLeaf3=0.01, kStem3=0.56, kRoot3=0.13, kRhizome3=0.3, 
                       kLeaf4=0.01, kStem4=0.56, kRoot4=0.13, kRhizome4=0.3,
                       kLeaf5=0.01, kStem5=0.56, kRoot5=0.13, kRhizome5=0.3,
                       kLeaf6=0.01, kStem6=0.56, kRoot6=0.13, kRhizome6=0.3)

system.time(ans <- BioGro(weather05, phenoControl = pheno.ll))

ans.dat <- as.data.frame(unclass(ans)[1:11])
sel.rows <- seq(1,nrow(ans.dat),400)
simDat <- ans.dat[sel.rows,c("ThermalT","Stem","Leaf","Root","Rhizome","Grain","LAI")]
plot(ans,simDat)

## Residual sum of squares before the optimization

ans0 <- BioGro(weather05)
RssBioGro(simDat,ans0)

pheno.ll2 <- phenoParms()
pheno.ll2[7:31] <- idbp(simDat)

ans01 <- BioGro(weather05, phenoControl = pheno.ll2)
RssBioGro(simDat,ans01)

## op <- OpBioGro(phen=0, WetDat=weather05,
##                      iCoef=idbp(simDat),
##                      data=simDat)

## pheno.op <- phenoParms()
## pheno.op[7:31] <- op$coefs

## bg.op <- BioGro(weather05, phenoControl = pheno.op)

## plot(bg.op, simDat)

## ## Second round
## op2 <- OpBioGro(phen=0, WetDat=weather05,
##                      iCoef=op$coefs,
##                      data=simDat)


## system.time(op1.mc <- MCMCBioGro(phen=1, niter=5,niter2=5,
##                      WetDat=weather05, saTemp=0,
##                      iCoef=as.vector(unlist(pheno.ll)[7:31]),
##                      data=simDat, sd = c(6e-18,1e-15)))


op1.mc <- MCMCBioGro(phen=1, niter=20,niter2=2e3,
                     WetDat=weather05, saTemp=0,
                     iCoef=idbp(simDat),
                     data=simDat, sd = c(0.6,1e-5))

plot(op1.mc)

plot(op1.mc, plot.kind="trace", subset = nams %in% c("kLeaf_1","kStem_1","kRoot_1"))

plot(op1.mc, plot.kind="density", subset = nams %in% c("kLeaf_1","kStem_1","kRoot_1"))

## Second stage

op2.mc <- MCMCBioGro(phen=2, niter=20,niter2=2e3,
                     WetDat=weather05, saTemp=0,
                     iCoef=idbp(simDat),
                     data=simDat, sd = c(0.6,1e-5))

plot(op2.mc)

plot(op2.mc, plot.kind="trace", subset = nams %in% c("kLeaf_1","kStem_1","kRoot_1"))

plot(op2.mc, plot.kind="density", subset = nams %in% c("kLeaf_1","kStem_1","kRoot_1"))


## Using the coda package

library(coda)

opmc <- mcmc(data = op1.mc$resMC[,1:3], thin = 50)

summary(opmc)
HPDinterval(opmc)
plot(opmc)

densityplot(opmc) 
traceplot(opmc)

## Diagnosis

## Crosscor

crosscorr(opmc)
##         kLeaf_1 kStem_1 kRoot_1
## kLeaf_1   1.000  -0.416  -0.370
## kStem_1  -0.416   1.000  -0.691
## kRoot_1  -0.370  -0.691   1.000

## Autocorrelation
autocorr.diag(opmc)
##          kLeaf_1 kStem_1 kRoot_1
## Lag 0     1.0000  1.0000  1.0000
## Lag 50    0.7672  0.8576  0.8613
## Lag 250   0.3831  0.5287  0.5437
## Lag 500   0.1528  0.2876  0.2776
## Lag 2500 -0.0323 -0.0195  0.0164

## Raftery
raftery.diag(opmc)
## Quantile (q) = 0.025
## Accuracy (r) = +/- 0.005
## Probability (s) = 0.95 

## You need a sample size of at least 3746 with these values of q, r and s

## Geweke
geweke.diag(opmc, frac1 = 0.5, frac2 = 0.5)
## Fraction in 1st window = 0.5
## Fraction in 2nd window = 0.5 

## kLeaf_1 kStem_1 kRoot_1 
##  -0.146   1.864  -1.807 

