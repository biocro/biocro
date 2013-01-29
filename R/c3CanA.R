##
##  BioCro/R/CanA.R by Fernando Ezequiel Miguez  Copyright (C) 2007-2009
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

c3CanA <- function(lai,doy,hr,solar,temp,rh,windspeed,
                 lat=40,nlayers=8,kd=0.1,
                 heightFactor=3,
                 c3photoControl = list(),
                 lnControl = list())
  {
    ## Add error checking to this function
    if(length(c(lai,doy,hr,solar,temp,rh,windspeed)) != 7)
      stop("all input should be of length 1")

    photoP <- c3photoParms()
    photoP[names(c3photoControl)] <- c3photoControl

    photoPs <- as.vector(unlist(photoP))
    
    lnP <- lnParms()
    lnP[names(lnControl)] <- lnControl

    lnPs <- as.vector(unlist(lnP))
    
    res <- .Call(c3CanA_sym,as.double(lai),as.integer(doy),
                 as.integer(hr),as.double(solar),as.double(temp),
                 as.double(rh),as.double(windspeed),
                 as.double(lat),as.integer(nlayers),
                 as.double(photoPs),
                 as.double(kd), as.double(heightFactor),
                 as.double(lnPs))
    res$LayMat <- t(res$LayMat)
    colnames(res$LayMat) <- c("IDir","IDiff","Leafsun",
                              "Leafshade","TransSun","TransShade",
                              "AssimSun","AssimShade","DeltaSun",
                              "DeltaShade","CondSun","CondShade",
                              "LeafN", "Vmax")
    res
  }


c3photoParms <- function(vmax=100, jmax=180, Rd=1.1, Catm=380, O2 = 210, b0=0.08, b1=5, theta=0.7){

  ## ws <- match.arg(ws)
  ## if(ws == "gs") ws <- 1
  ## else ws <- 0
      
  list(vmax=vmax,jmax=jmax,Rd=Rd,Catm=Catm,O2=O2,b0=b0,b1=b1,theta=theta)

}
