## Testing the different optimization methods 

data(weather05)

## Some coefficients

Coefficientsjune <- c(0.842826471, 0.157173529, 0)# for stem, leaf, and root

Coefficientsjuly <- c(0.696176819, 0.134742302,0.169080879)

Coefficientsaug <- c(0.727832299, 0.148726144,0.123441557)

Coefficientssep <- c(0.695390626,0.116392593,0.188216781)
Coefficientsoct <- c(0.70511988,0.026656566,0.268223554)

pheno.ll <- phenoParms(kLeaf1=0.16,kStem1=0.79,kRoot1=0.05,kRhizome1=-1e-4,
                       kLeaf2=0.14,kStem2=0.69,kRoot2=0.17, kRhizome2=-1e-4,
                       kLeaf3=0.15, kStem3=0.73, kRoot3=0.06, kRhizome3=0.06, 
                       kLeaf4=0.12, kStem4=0.69, kRoot4=0.095, kRhizome4=0.095,
                       kLeaf5=0.03, kStem5=0.70, kRoot5=0.135, kRhizome5=0.135,
                       kLeaf6=0.0, kStem6=0.70, kRoot6=0.15, kRhizome6=0.15)


answillow <- willowGro(weather05)
plot(answillow)

system.time(ans <- BioGro(weather05, phenoControl = pheno.ll))
answillow <- willowGro(weather05, phenoControl = pheno.ll)
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




