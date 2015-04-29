## Test script for the Century Model

iSoilP <- centuryParms(timestep="year")
for(j in 1:9) iSoilP[j] <- 100 

#nsim <- 52*1000 ## weekly
nsim <- 1000 ## yearly

mat <- matrix(nrow=nsim,ncol=9)
mat2 <- matrix(nrow=nsim,ncol=4)
mat3 <- matrix(nrow=nsim,ncol=9)
respVec <- numeric(nsim)
MinNVec <- numeric(nsim)
  
iLeafL <- 100
iStemL <- 200
iRootL <- 300
iRhizL <- 400

add <- TRUE
for(i in 1:nsim){

  mat2[i,] <- c(iLeafL,iStemL,iRootL,iRhizL) 
  
  aLeafL <- iLeafL * 0.1
  aStemL <- iStemL * 0.1
  aRootL <- iRootL * 0.1
  aRhizL <- iRhizL * 0.1

  res <- Century(aLeafL,aStemL,aRootL,aRhizL,0.23,15,10,50,centuryControl=iSoilP, verbose=FALSE)
#  res <- CenturyC(aLeafL,aStemL,aRootL,aRhizL,0.23,15,10,50,centuryControl=iSoilP,soilType=5)

  iLeafL <- iLeafL - aLeafL
  iStemL <- iStemL - aStemL
  iRootL <- iRootL - aRootL
  iRhizL <- iRhizL - aRhizL
  
  for(j in 1:9) iSoilP[j] <- res$SCs[j] 
  iSoilP$iMinN <- res$MinN
  
  mat[i,] <- res$SCs
  mat3[i,] <- res$SNs
  
  respVec[i] <- res$Resp
  MinNVec[i] <- res$MinN

if(add){  
    if(i %% 52 == 0){
      iLeafL <- 100
      iStemL <- 200
      iRootL <- 300
      iRhizL <- 400
    }
  }
}

## Plotting the different soil carbon pools
labs <- paste("SC",1:9,sep="")
xyplot(mat[,1] + mat[,2] + mat[,3] +
       mat[,4] + mat[,5] + mat[,6] +
       mat[,7] + mat[,8] + mat[,9] ~ 1:nsim,type="l",ylab="carbon",
       col=rainbow(9),
       lty=1, lwd=3,
       key=list(text=list(labs),lines=TRUE,col=rainbow(9)))

## plotting the total carbon accumulation
xyplot(rowSums(mat[,5:8])
       ~ 1:nsim,type="l",ylab="carbon",
       lty=1, lwd=3)

## Plotting the plant litter
xyplot(mat2[,1] + mat2[,2] + mat2[,3] + mat2[,4] ~ 1:nsim,
       type="l",ylab="carbon",col=1:4,
       key=list(text=list(c("Leaf","Stem","Root","Rhiz")),lines=TRUE,col=1:4))

xyplot(respVec ~ 1:nsim , ylab="Respiration")

xyplot(MinNVec ~ 1:nsim , ylab="Mineralized N") 


## Plotting the different nitrogen carbon pools
labs <- paste("SN",1:9,sep="")
xyplot(mat3[,1] + mat3[,2] + mat3[,3] +
       mat3[,4] + mat3[,5] + mat3[,6] +
       mat3[,7] + mat3[,8] + mat3[,9] ~ 1:nsim,type="l",ylab="nitrogen",
       col=rainbow(9),
       lty=1, lwd=3,
       key=list(text=list(labs),lines=TRUE,col=rainbow(9)))


## Testing another aspect of Century
data(weather05)

litter <- c(0,0,0,0)
iC <- rep(0.5,9) # These are in Mg ha^-1
nyears <- 300
Cmat <- matrix(ncol=9,nrow=nyears)
litterMat <- matrix(ncol=4, nrow=nyears)

iRhiz <- 0.06
Rhiz <- numeric(nyears)
yield <- numeric(nyears)

sene.ll <- seneParms(senRoot = 3500, senRhizome = 3500)

for(i in 1:nyears){

  century.ll <- centuryParms(SC1 = iC[1], SC2 = iC[2], SC3 = iC[3], SC4 = iC[4], SC5 = iC[5], SC6 = iC[6], SC7 = iC[7], SC8 = iC[8], SC9 = iC[9], Litter = litter)
  res <- BioGro(weather05, centuryControl = century.ll, iRhizome = iRhiz, seneControl = sene.ll)
  last <- length(res$Stem)
  litter <- c(res$AboveLitter[last]*0.3, res$AboveLitter[last]*0.7, res$BelowLitter[last]*0.7, res$BelowLitter[last]*0.3)
  litterMat[i,] <- litter
  iC <- res$SCpools
  Cmat[i,] <- iC
  iRhiz <- res$Rhizome[last]
  Rhiz[i] <- iRhiz
  yield[i] <- max(I(res$Stem + res$Leaf))
  
}

Cmat <- rbind(rep(1,9),Cmat)
litterMat <- rbind(rep(0,4),litterMat)

matplot(1:I(nyears+1),Cmat, type="l")

matplot(1:I(nyears+1),litterMat, type="l", ylab="litter")

xyplot(c(0.06,Rhiz) ~ c(0,1:nyears), type = "o")

xyplot(yield ~ 1:nyears , ylab = "Dry biomass", xlab = "years", type="o")
