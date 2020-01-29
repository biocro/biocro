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

## Controlling the effect of leaf nitrogen on photosynthethic parameters
lnParms <- function(LeafN = 2 , kpLN = 0.2, lnb0 = -5, lnb1 = 18, lnFun=c("none", "linear")){

  lnFun <- match.arg(lnFun)
  if(lnFun == "none"){
    lnFun <- 0
  }else{
    lnFun <- 1
  }

  list(LeafN = LeafN , kpLN = kpLN, lnb0 = lnb0, lnb1 = lnb1, lnFun=lnFun)

}

photoParms <- function(vmax=39, alpha=0.04, kparm=0.7, theta=0.83, beta=0.93, Rd=0.8, Catm=380, b0=0.08, b1=3, ws=c("gs","vmax"),uppertemp=37.5,lowertemp=3.0)
{
    ws <- match.arg(ws)
    if (ws == "gs") ws <- 1
    else ws <- 0

    list(vmax=vmax, alpha=alpha, kparm=kparm, theta=theta, beta=beta, Rd=Rd, Catm=Catm, b0=b0, b1=b1, ws=ws, uppertemp=uppertemp, lowertemp=lowertemp)

}

canenitroParms <- function(iLeafN=85, kLN=0.5, Vmax.b1=0.6938, Vmax.b0=-16.25,alpha.b1=0.000488,alpha.b0=0.02367,Rd.b1=0.1247,Rd.b0=-4.5917,
                       kpLN=0.17, lnb0 = -5, lnb1 = 18, lnFun=c("linear"),maxln=85,minln=57,daymaxln=60)
{
    lnFun <- match.arg(lnFun)
    if (lnFun == "none") {
        lnFun <- 0
    } else {
        lnFun <- 1
    }

    list(iLeafN=iLeafN, kLN=abs(kLN), Vmax.b1=Vmax.b1, Vmax.b0=Vmax.b0,alpha.b1=alpha.b1, alpha.b0=alpha.b0,Rd.b1=Rd.b1,Rd.b0=Rd.b0,kpLN=kpLN,
         lnb0 = lnb0, lnb1 = lnb1, lnFun = lnFun,maxln=maxln,minln=minln,daymaxln=daymaxln)
}

