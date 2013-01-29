##
##  BioCro/R/CanA.R by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation; either version 2 or 3 of the License
##  (at your option).
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  A copy of the GNU General Public License is available at
##  http://www.r-project.org/Licenses/
##
##

CanA <- function(lai,doy,hr,solar,temp,rh,windspeed,
                 lat=40,nlayers=8,kd=0.1,StomataWS=1,
                 heightFactor=3,
                 photoControl = list(),
                 lnControl = list(),
                 units=c("kg/m2/hr","Mg/ha/hr"))
  {
    ## Add error checking to this function
    if(length(c(lai,doy,hr,solar,temp,rh,windspeed)) != 7)
      stop("all input should be of length 1")

    units <- match.arg(units)

    photoP <- photoParms()
    photoP[names(photoControl)] <- photoControl

    vmax <- photoP$vmax
    alpha <- photoP$alpha
    kparm <- photoP$kparm
    theta <- photoP$theta
    beta <- photoP$beta
    Rd <- photoP$Rd
    Catm <- photoP$Catm
    b0 <- photoP$b0
    b1 <- photoP$b1
    ws <- photoP$ws
    
    lnP <- lnParms()
    lnP[names(lnControl)] <- lnControl

    res <- .Call(CanA_sym,as.double(lai),as.integer(doy),
                 as.integer(hr),as.double(solar),as.double(temp),
                 as.double(rh),as.double(windspeed),
                 as.double(lat),as.integer(nlayers),as.double(StomataWS),
                 as.double(vmax),as.double(alpha),as.double(kparm),
                 as.double(theta), as.double(beta),
                 as.double(Rd),as.double(b0),
                 as.double(b1),as.double(Catm),
                 as.double(kd), as.double(heightFactor),
                 as.integer(ws), as.double(lnP$LeafN),
                 as.double(lnP$kpLN), as.double(lnP$lnb0),
                 as.double(lnP$lnb1), as.integer(lnP$lnFun))
    res$LayMat <- t(res$LayMat)
    colnames(res$LayMat) <- c("IDir","IDiff","Leafsun",
                              "Leafshade","TransSun","TransShade",
                              "AssimSun","AssimShade","DeltaSun",
                              "DeltaShade","CondSun","CondShade",
                              "LeafN", "Vmax")
    if(units == "Mg/ha/hr"){
      res
    }else{
      ## Need to go from Mg to kg and from ha to m2
      cf <- 1e3 * 1e-4
      res$CanopyAssim <- res$CanopyAssim * cf
      ## This is in kg of biomass per m2 per hour
      res$CanopyTrans <- res$CanopyAssim * cf
      ## This is in kg of water per m2 per hour
      res$TranEpen <- res$TranEpen * cf
      res$TranEpries <- res$TranEpries * cf
      res$LayMat[,5:8] <- res$LayMat[,5:8] * cf
    }
  }

## Controlling the effect of leaf nitrogen on photosynthethic parameters
lnParms <- function(LeafN = 2 , kpLN = 0.2, lnb0 = -5, lnb1 = 18, lnFun=c("none","linear")){

  lnFun <- match.arg(lnFun)
  if(lnFun == "none"){
    lnFun <- 0
  }else{
    lnFun <- 1
  }

  list(LeafN = LeafN , kpLN = kpLN, lnb0 = lnb0, lnb1 = lnb1, lnFun=lnFun)

}
