## Function to simulate the multilayer drying of the soil
## This should probably take into account the distribution of
## root biomass in the profile

soilML <- function(precipt,CanopyT,cws,soilDepth,FieldC,WiltP,phi1=0.01,phi2 =10, wsFun = c("linear","logistic","exp","none"),rootDB,soilLayers=3,
                   LAI,k,AirTemp,IRad,winds,RelH,soilType=10,hydrDist=0,rfl=0.3){

  if(length(cws) !=soilLayers)
    stop("cws should be of length == soilLayers")

  wsFun <- match.arg(wsFun)
  if(wsFun == "linear")  wsFun <- 0
  else if(wsFun == "logistic") wsFun <- 1
  else if(wsFun =="exp") wsFun <- 2 
  else if(wsFun == "none") wsFun <- 3

  if(hydrDist > 0){
    mat <- matrix(nrow=soilLayers,ncol=12)
  }else{
    mat <- matrix(nrow=soilLayers,ncol=9)
  }

  tmp2 <- SoilType(soilType)
  
  oldEvapoTra <- 0
  oldWaterIn <- 0
  drainage <- 0
  theta_s <- FieldC*1.1
  FieldC <- tmp2$fieldc
  WiltP <- tmp2$wiltp
  psim <- numeric(soilLayers)
## rootDepth
  ## Crude empirical relationship between root biomass and rooting depth
  rootDepth <- rootDB * 0.44
  if(rootDepth > soilDepth) rootDepth = soilDepth;
  
  depths <- seq(0,soilDepth,length.out=I(soilLayers+1))
##  rprops <- dpois(1:soilLayers,0.3*soilLayers)/sum(dpois(1:soilLayers,0.3*soilLayers))
  rprops <- rootDist(soilLayers,rootDepth,depths,rfl)
  
## Precipitation enters in mm
  waterIn = precipt * 1e-3; ## This is now cubic meter of water
  
  for(i in I(length(depths)-1):1){

        ## First determine the layer depth
    if(i == 1){
      layerDepth <- depths[1+1]
    }else{
      layerDepth <- depths[i] - depths[i-1] ## These are in meters
    }

### There is redistribution of water in the profile. 
### For this section see Campbell and Norman "Environmental BioPhysics" Chapter 9
### First compute the matric potential
    if(hydrDist > 0){
      psim[i] = tmp2$air.entry * (cws[i]/theta_s)^-tmp2$b ; # This is matric potential of current layer 
      if(i > 1){
     	psim[i-1] = tmp2$air.entry * (cws[i-1]/theta_s)^-tmp2$b ;#  This is matric potential of next layer 
     	dPsim = psim[i] - psim[i-1];
      }else{
     	dPsim = 0;
      }
      K_psim = tmp2$Ks *  (tmp2$air.entry/psim[i])^(2+3/tmp2$b); # This is hydraulic conductivity 
      J_w = K_psim * (dPsim/layerDepth) - 9.8 * K_psim## Ignore gravitational effect - 10 * K_psim
      ## This is in kg (m2 * s)
      ## if(is.infinite(J_w) || is.na(J_w) || J_w > 1 || J_w < -1){
      ##   J_w <- 0
      ## }
# This flow is in kg / (m2 * s)
    if(is.na(J_w)){
      cat("K_psim: ",K_psim," psim[i]: ",psim[i]," psim[i-1]: ",psim[i-1]," i",i,"\n")
      stop("J_w is NA")
    }
    ## The model runs at hourly time steps so
    ## times 3600 to convert seconds to hours
    ## Density of water at 20C 0.9882 Mg /m3
    ## 1e-3 to convert from kg to Mg
      J_w = J_w * 3600 * (1/0.9882) * 1e-3; #/* This is flow in m3 /(m2 * hr)*/
      if(i == soilLayers && J_w < 0){
##          cws[i] = cws[i] +  J_w / layerDepth;
          drainage = drainage - J_w
      }else
      if(i > 1){
     	cws[i] = cws[i] -  J_w / layerDepth;
     	cws[i - 1] =  cws[i-1] +  J_w / layerDepth;
      }else{
##        if(J_w < 0){
          cws[i] = cws[i] -  J_w / layerDepth;
##        }
      }
    }

    if(cws[i] > FieldC) cws[i] = FieldC;
##    if(cws[i+1] > FieldC) cws[i+1] = FieldC;
    if(cws[i] < WiltP) cws[i] = WiltP;
##    if(cws[i+1] < WiltP) cws[i+1] = WiltP;
    
    aw = cws[i] * layerDepth; ## Volume of water in layer i currently

    if(waterIn > 0){
      aw = aw + waterIn / soilLayers + oldWaterIn; ## They are both in meters so it works
      ## This is likely to overflow for the first layer when there is plenty precipitation
      diffwc = FieldC*layerDepth - aw;
      
      if(diffwc < 0){
        ## This means that precipitation exceeded the capacity of the last layer
        ## Save this amount of water for the next layer
        oldWaterIn <- -diffwc
        aw <- FieldC * layerDepth
      }
     
    }

    ## Root Biomass
    rootATdepth <- rootDB*rprops[i]

    ## Plant available water is only between current water status and permanent wilting point 
    ## Plant available water 
    paw <- aw - WiltP * layerDepth;
    if(paw <0) paw = 0;
    

    ## The next step is to calculate the Evapo Transpiration I will
    ## assume that only the first layer of the soil profile has both
    ## evaporation and transpiration
    if(i == 1){
      ## This assumes that the Canopy Transpiration is distributed
      ## proportionally to the root distribution
       SoilEvap <- SoilEvapo(LAI=LAI,k=k,AirTemp=AirTemp,IRad=IRad,aw/layerDepth,FieldC=FieldC,WiltP=WiltP,winds=winds,RelH=RelH)
       CanopyTra <- CanopyT*rprops[i] 
       EvapoTra <- CanopyTra + SoilEvap
       ## These units are in Mg/ha so
       Newpawha <- (paw * 1e4) - EvapoTra
    }else{
      SoilEvap <- 0
      CanopyTra <- CanopyT*rprops[i] 
       EvapoTra <- CanopyTra + SoilEvap
       ## These units are in Mg/ha so
       Newpawha <- (paw * 1e4) - (EvapoTra + oldEvapoTra);
    }

    if(Newpawha < 0){
      oldEvapoTra = -Newpawha;
      paw = 0;
      ## If the Demand is not satisfied by the first layer. This will be stored and added to subsequent soilLayers
    }else{
      paw = Newpawha / 1e4 ;
    }

    awc <- paw / layerDepth + WiltP 
    cws[i] <- awc

    if(wsFun == 0){
      slp = 1/(FieldC - WiltP);
      intcpt = 1 - FieldC * slp;
      wsPhoto = slp * awc + intcpt ;
    }else
    if(wsFun == 1){
      phi10 = (FieldC + WiltP)/2;
      wsPhoto = 1/(1 + exp((phi10 - awc)/ phi1));
    }else
    if(wsFun == 2){
      slp = (1 - WiltP)/(FieldC - WiltP);
      intcpt = 1 - FieldC * slp;
      theta = slp * awc + intcpt ;
      wsPhoto = (1 - exp(-2.5 * (theta - WiltP)/(1 - WiltP))) / (1 - exp(-2.5));
    }else
    if(wsFun == 3){
      wsPhoto = 1;
    }

  if(wsPhoto <= 0 )
    wsPhoto = 1e-20; ## This can be mathematically lower than zero in some cases but I should prevent that. 

    wsSpleaf = awc^phi2 * 1/FieldC^phi2; 
    if(wsFun == 3){ 
      wsSpleaf = 1;
    }
    
    mat[i,1] <- cws[i]
    mat[i,2] <- rootATdepth
    mat[i,3] <- waterIn
    mat[i,4] <- layerDepth
    mat[i,5] <- CanopyTra
    mat[i,6] <- SoilEvap
    mat[i,7] <- wsPhoto
    mat[i,8] <- wsSpleaf
    mat[i,9] <- drainage
    if(hydrDist > 0){
      mat[i,10] <- J_w
      mat[i,11] <- K_psim
      mat[i,12] <- psim[i]
    }
    
    
  }

  if(waterIn > 0) drainage <- drainage + waterIn
  if(hydrDist > 0){
    colnames(mat) <- c("cws","rootATdepth","waterIn","layerDepth","CanopyTra","SoilEvap","wsPhoto","wsSpleaf","drainage","J_w","K_psim","psim")
  }else{
    colnames(mat) <- c("cws","rootATdepth","waterIn","layerDepth","CanopyTra","SoilEvap","wsPhoto","wsSpleaf","drainage")
  }
  mat
}


rootDist <- function(layers,rootDepth,depthsp,rfl){

  if(layers < 2)
    stop("layers should be at least 2")

  CumLayerDepth <- 0 
  CumRootDist <- 0
  ca <- 0
  rootDist <- numeric(layers)
  
#  for(i in 1:I(length(depthsp)-1)){
  for(i in seq_len(layers)){

    if(i == 1){
      layerDepth = depthsp[1+1];
    }else{
      layerDepth = depthsp[i] - depthsp[i-1];
    }

    CumLayerDepth = CumLayerDepth +  layerDepth;

    if(rootDepth > CumLayerDepth){
      CumRootDist = CumRootDist + 1
    }
    
  }

  for(j in seq_len(layers)){
    if(j < CumRootDist){
      a = dpois(j,CumRootDist*rfl)
      rootDist[j] = a
      ca = ca + a
    }else{
      rootDist[j] = 0
    }
  }

  rootDist = rootDist / ca
  rootDist
  
}


