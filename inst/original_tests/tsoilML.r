## The R version has not been fully tested but it was used to prototype the C version

## Limited test for the R version
## Water infiltration without evapotranspiration
## Starting with a dry soil
## Without hydraulic distribution
data(weather05)
ans.bg <- BioGro(weather05)

## First scenario
## We have both precipitation and canopy transpiration
layers <- 10
precip <- weather05$precip[2952:7224]
cws <- rep(0.33,layers)
mat <- matrix(nrow=length(precip),ncol=layers)
mat2 <- matrix(nrow=length(precip),ncol=layers)
mat3 <- matrix(nrow=length(precip),ncol=layers)
for(i in seq_along(precip)){
  
  res <-  soilML(precip[i],ans.bg$CanopyTrans[i],cws,soilDepth=1.2,0.33,0.13,rootDB=10,soilLayers=layers,LAI=3,k=0.68,AirTemp=0,IRad=0,winds=3,RelH=0.8, soilType=6)

  cws <- res[,1]

  mat[i,] <- res[,1]
  mat2[i,] <- res[,5]
  mat3[i,] <- res[,6]
}

## Soil Moisture by layer
matplot(1:nrow(mat),mat, type="l")
plot(1:nrow(mat),apply(mat,1,mean),type="l")
## Canopy Transpiration as distributed on the soil layers
matplot(1:nrow(mat2),mat2, type="l")



## Previous scenario plus hydraulic distribution

layers <- 10
precip <- weather05$precip[2952:7224]
cws <- rep(0.33,layers)
mat <- matrix(nrow=length(precip),ncol=layers)
mat2 <- matrix(nrow=length(precip),ncol=layers)
mat3 <- matrix(nrow=length(precip),ncol=layers)
for(i in seq_along(precip)){
  
  res <-  soilML(precip[i],ans.bg$CanopyTrans[i],cws,soilDepth=1.5,0.33,0.13,rootDB=10,soilLayers=layers,LAI=3,k=0.68,AirTemp=0,IRad=0,winds=3,RelH=0.8, hydrDist=1, soilType=6)

  cws <- res[,1]

  mat[i,] <- res[,1]
  mat2[i,] <- res[,5]
  mat3[i,] <- res[,6]
}

## Soil Moisture by layer
matplot(1:nrow(mat),mat, type="l")
plot(1:nrow(mat),apply(mat,1,mean),type="l")
## Canopy Transpiration as distributed on the soil layers
matplot(1:nrow(mat2),mat2, type="l")


## Testing the overall model 
data(weather05)
wetdat <- weather05

soil.ll0.2.1L <- soilParms(wsFun="none", FieldC=0.34, WiltP=0.13 , soilDepth = 1.5, soilLayers=1) ## optimized soil depth
soil.ll0.2.2L <- soilParms(wsFun="none", FieldC=0.34, WiltP=0.13 , soilDepth = 1.5, soilLayers=2) ## optimized soil depth
soil.ll0.2.3L <- soilParms(wsFun="none", FieldC=0.34, WiltP=0.13 , soilDepth = 1.5, soilLayers=3) ## optimized soil depth
soil.ll0.2.4L <- soilParms(wsFun="none", FieldC=0.34, WiltP=0.13 , soilDepth = 1.5, soilLayers=4) ## optimized soil depth
soil.ll0.2.5L <- soilParms(wsFun="none", FieldC=0.34, WiltP=0.13 , soilDepth = 1.5, soilLayers=5) ## optimized soil depth
soil.ll0.2.6L <- soilParms(wsFun="none", FieldC=0.34, WiltP=0.13 , soilDepth = 1.5, soilLayers=6) ## optimized soil depth
soil.ll0.2.10L <- soilParms(wsFun="none", FieldC=0.34, WiltP=0.13 , soilDepth = 1.5, soilLayers=10) ## optimized soil depth
soil.ll0.2.36L <- soilParms(wsFun="none", FieldC=0.34, WiltP=0.13 , soilDepth = 1.5, soilLayers=36) ## optimized soil depth


sene.ll <- seneParms(senLeaf=2700)

ans0.2.1L <- BioGro(wetdat, soilControl = soil.ll0.2.1L, seneControl=sene.ll)
ans0.2.2L <- BioGro(wetdat, soilControl = soil.ll0.2.2L, seneControl=sene.ll)
ans0.2.3L <- BioGro(wetdat, soilControl = soil.ll0.2.3L, seneControl=sene.ll)
ans0.2.4L <- BioGro(wetdat, soilControl = soil.ll0.2.4L, seneControl=sene.ll)
ans0.2.5L <- BioGro(wetdat, soilControl = soil.ll0.2.5L, seneControl=sene.ll)
ans0.2.6L <- BioGro(wetdat, soilControl = soil.ll0.2.6L, seneControl=sene.ll)
ans0.2.10L <- BioGro(wetdat, soilControl = soil.ll0.2.10L, seneControl=sene.ll)
ans0.2.36L <- BioGro(wetdat, soilControl = soil.ll0.2.36L, seneControl=sene.ll)

xyplot(ans0.2.1L$SoilWatCont +
       ans0.2.2L$SoilWatCont +
       ans0.2.3L$SoilWatCont +
       ans0.2.4L$SoilWatCont +
       ans0.2.5L$SoilWatCont +
       ans0.2.6L$SoilWatCont +
       ans0.2.10L$SoilWatCont +
       ans0.2.36L$SoilWatCont ~ ans0.2.1L$DayofYear, type="l", auto.key=TRUE,
       ylab = "Soil Moisture")

xyplot(ans0.2.1L$SoilEvaporation +
       ans0.2.2L$SoilEvaporation +
       ans0.2.3L$SoilEvaporation +
       ans0.2.4L$SoilEvaporation +
       ans0.2.5L$SoilEvaporation +
       ans0.2.6L$SoilEvaporation +
       ans0.2.10L$SoilEvaporation +
       ans0.2.36L$SoilEvaporation ~ ans0.2.1L$DayofYear, type="l", auto.key=TRUE,
       ylab = "Soil Evaporation")

xyplot(ans0.2.1L$CanopyT + ans0.2.2L$CanopyT + ans0.2.3L$CanopyT +
       ans0.2.4L$CanopyT + ans0.2.5L$CanopyT + ans0.2.6L$CanopyT +
       ans0.2.10L$CanopyT ~ ans0.2.1L$ThermalT, type="l", auto.key=TRUE,
       ylab = "Canopy Transpiration")

xyplot(ans0.2.1L$Stem + ans0.2.2L$Stem + ans0.2.3L$Stem +
       ans0.2.4L$Stem + ans0.2.5L$Stem + ans0.2.6L$Stem +
       ans0.2.10L$Stem ~ ans0.2.1L$ThermalT, type="l", auto.key=TRUE,
       ylab = "Stem Biomass")

## More in detail looking at each depth

## This section depends on msummarize a function on a personal package
cwsMat <- ans0.2.10L$cwsMat
colnames(cwsMat) <- factor(soil.ll0.2.10L$soilDepths)[-1]
cws.dat <- data.frame(cwsMat,DOY=ans0.2.10L$DayofYear)
cws.datS <- msummarize(DOY ~ X0.15 + X0.3 + X0.45 + X0.6 + X0.75 + X0.9 + X1.05 + X1.2 + X1.35 + X1.5 ,FUN=mean, data = cws.dat)
xyplot(cws.datS[,2] + cws.datS[,3] + cws.datS[,4] + cws.datS[,5] + cws.datS[,6] + cws.datS[,7]
       + cws.datS[,8] + cws.datS[,9] + cws.datS[,10] ~ DOY, data = cws.datS, type="l",ylab="SWC")
cws.datST <- data.frame(DOY=rep(cws.datS$DOY,each=10),Depth=rep(-c(soil.ll0.2.10L$soilDepths[-1]),179),SWC=c(t(cws.datS[,-1])))
cws.datST2 <- cws.datST[cws.datST$DOY %in% seq(90,300,30),]
xyplot(Depth ~ SWC ,groups =factor(DOY), data = cws.datST2,  type="o", auto.key=TRUE)


## some code to help me understand darcy's law

## cws <- seq(0.2,0.32,0.01)
## ##cws <- rep(0.32, length.out=10)

## tmp2 <- SoilType(6)

## l.cws <- length(cws)
## dPsim <- numeric(l.cws)
## psim <- numeric(l.cws)
## K_psim <- numeric(l.cws)
## matric <- numeric(l.cws)
## gravit <- numeric(l.cws)
## J_w <- numeric(l.cws)

## FieldC <- tmp2$fieldc
## WiltP <- tmp2$wiltp
## theta_s <- tmp2$fieldc * 1.1
## soilLayers <- l.cws
## layerDepth <- 1.5/length(cws)
## drainage <- 0

## hours <- 10
## cwsMat <- matrix(nrow=length(cws),ncol=hours+1)
## cwsMat[,1] <- cws

## for(j in 1:hours){

##   for(i in length(cws):1){

##     psim[i] = tmp2$air.entry * (cws[i]/theta_s)^-tmp2$b ; # This is matric potential of current layer

##     if(i > 1){
##       psim[i-1] = tmp2$air.entry * (cws[i-1]/theta_s)^-tmp2$b ;#  This is matric potential of next layer 
##       dPsim[i] = psim[i] - psim[i-1];
##     }else{
##       dPsim[i] = 0;
##     }

##     K_psim[i] = tmp2$Ks * (tmp2$air.entry/psim[i])^(2+3/tmp2$b); # This is hydraulic conductivity
##     matric[i] <- K_psim[i] * (dPsim[i]/layerDepth) ## This is positive because it moves upward
##     gravit[i] <- - 9.8 * K_psim[i] ## This is negative because it moves downward
##     J_w[i] <- matric[i] +  gravit[i]
##     J <- J_w[i] * 3600 * 1e-3 * 0.98

## #    print(J)
##     if(i == l.cws){
##       if(J < 0){
##         cws[i] = ((cws[i] * layerDepth) +  J) / layerDepth;
##         drainage <- drainage - J
##       }else{
##         cws[i] = ((cws[i] * layerDepth) -  J) / layerDepth;
##         cws[i - 1] =  ((cws[i-1] * layerDepth) +  J) / layerDepth;
##       }
##     }else
##     if(i > 1){
##       cws[i] = ((cws[i] * layerDepth) -  J) / layerDepth;
##       cws[i - 1] =  ((cws[i-1] * layerDepth) +  J) / layerDepth;
##     }else{
##       if(J < 0){
##         cws[i] = ((cws[i] * layerDepth) +  J) / layerDepth;
##       }
##     }

##     ## if(cws[i] > FieldC) cws[i] = FieldC;
##     ## if(cws[i-1] > FieldC) cws[i-1] = FieldC;
##     ## if(cws[i] < WiltP) cws[i] = WiltP;
##     ## if(cws[i-1] < WiltP) cws[i-1] = WiltP;
    
##   }
##   cwsMat[,j+1] <- cws
## }

## ## xyplot(dPsim + psim1 + psim2 ~ cws, xlim=c(WiltP-0.02,max(cws)+0.02), ylim=c(-1e2,1e2), auto.key=TRUE)
## ## xyplot(K_psim ~ cws)
## ##xyplot(matric + gravit  + J_w ~ cws, auto.key=TRUE)
## ##matplot(seq_along(cws),cwsMat,type="l")
## dat <- data.frame(ws=c(t(cwsMat)),hour=rep(0:hours,l.cws),layer=rep(1:l.cws,hours+1))

## xyplot(ws ~ hour | factor(layer) , data = dat)

## xyplot(ws ~ layer | factor(hour) , data = dat)
