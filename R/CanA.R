CanA <- function(lai, doy, hr, solar, temp, rh, windspeed,
                 lat=40, nlayers=8, kd=0.1, StomataWS=1,
                 chi.l=1, leafwidth=0.04,
                 heightFactor=3,
                 photoControl = list(),
                 lnControl = list())
  {
    ## Add error checking to this function
    if(length(c(lai, doy, hr, solar, temp, rh, windspeed)) != 7)
      stop("all input should be of length 1")

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
    upperT<-photoP$uppertemp
    lowerT<-photoP$lowertemp
    
    canenitroP <- canenitroParms()
    canenitroP [names(lnControl)] <- lnControl
    nnitroP<-as.vector(unlist(canenitroP))

    res <- .Call(CanA_sym, as.double(lai), as.integer(doy),
                 as.integer(hr), as.double(solar), as.double(temp),
                 as.double(rh), as.double(windspeed),
                 as.double(lat), as.integer(nlayers), as.double(StomataWS),
                 as.double(vmax), as.double(alpha), as.double(kparm),
                 as.double(theta), as.double(beta),
                 as.double(Rd), as.double(b0),
                 as.double(b1), as.double(Catm),
                 as.double(kd), as.double(heightFactor),
                 as.integer(ws), as.double(canenitroP$iLeafN),
                 as.double(canenitroP$kpLN), as.double(canenitroP$lnb0),
                 as.double(canenitroP$lnb1), as.integer(canenitroP$lnFun),
                 as.double(chi.l), as.double(upperT),
                 as.double(lowerT), as.double(nnitroP),
                 as.double(leafwidth))

    res$LayMat = t(res$LayMat)
    colnames(res$LayMat) <- c("IDir", "IDiff", "Leafsun",
                              "Leafshade", "TransSun", "TransShade",
                              "AssimSun", "AssimShade", "DeltaSun",
                              "DeltaShade", "CondSun", "CondShade",
                              "LeafN", "Vmax", "RH", "GrossAssimSun", "GrossAssimShade",
                              "Phi", "LeafN", "WindSpeed", "CanopyHeight")

    return(res)
  }
