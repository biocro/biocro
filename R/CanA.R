##  BioCro/R/CanA.R by Fernando Ezequiel Miguez  Copyright (C) 2007-2010

##' Simulates canopy assimilation
##'
##' It represents an integration of the photosynthesis function
##' \code{\link{c4photo}}, canopy evapo/transpiration and the multilayer canopy
##' model \code{\link{sunML}}.
##'
##'
##' @aliases CanA lnParms
##' @param lai Leaf area index.
##' @param doy Day of the year (1--365).
##' @param hr Hour of the day (0--23).
##' @param solar Solar radiation (\eqn{\mu} mol \eqn{\mathrm{m}^{-2}}{m-2}
##' \eqn{s^{-1}}{s-1}).
##' @param temp Temperature (Celsius).
##' @param rh Relative humidity (0--1).
##' @param windspeed Wind speed (m \eqn{\mathrm{s}^{-1}}{s-1}).
##' @param lat Latitude (degrees north).
##' @param nlayers Number of layers in the simulation of the canopy (maximum
##' allowed is 50).
##' @param kd Light extinction coefficient for diffuse light.
##' @param StomataWS Coefficient controlling the effect of water stress on
##' stomatal conductance and assimilation.
##' @param chi.l Leaf angle as described by projection of horizontal to vertical leaf area.
##' @param Leafwidth Leaf width in meters.
##' @param heightFactor Height factor. Divide LAI by this number to get the
##' height of a crop.
##' @param photoControl List that sets the photosynthesis parameters. See
##' \code{\link{BioGro}}.
##' @param lnControl List that sets the leaf nitrogen parameters.
##'
##' \code{LeafN} Initial value of leaf nitrogen (g m-2).
##'
##' \code{kpLN} Coefficient of decrease in leaf nitrogen during the growing season.
##' The equation is LN = iLeafN * exp(-kLN * TTc).
##'
##' \code{lnFun} Controls whether there is a decline in leaf nitrogen with the depth
##' of the canopy. `none' means no decline, `linear' means a linear decline
##' controlled by the following two parameters.
##'
##' \code{lnb0} Intercept of the linear decline of leaf nitrogen in the depth of the
##' canopy.
##'
##' \code{lnb1} Slope of the linear decline of leaf nitrogen in the depth of the
##' canopy. The equation is \code{vmax = leafN_lay * lnb1 + lnb0}.
##'
##' @param units Whether to return units in kg/m2/hr or Mg/ha/hr. This is
##' typically run at hourly intervals, that is why the hr is kept, but it could
##' be used with data at finer timesteps and then convert the results.
##' @return
##'
##' \code{\link{list}}
##'
##' Returns a list with several elements.
##'
##' \item{\code{CanopyAssim}}{Hourly canopy assimilation (\eqn{\mathrm{kg\,m}^{-2}}{kg/m2}
##' \eqn{\mathrm{hr}^{-1}}{per hour}) or canopy assimilation (\eqn{\mathrm{Mg\,ha}^{-1}}{Mg/ha}
##' \eqn{\mathrm{hr}^{-1}}{per hour}).}
##'
##' \item{\code{CanopyTrans}}{Hourly canopy transpiration (\eqn{\mathrm{kg\,m}^{-2}}{kg/m2}
##' \eqn{\mathrm{hr}^{-1}}{per hour}) or canopy transpiration (\eqn{\mathrm{Mg\,ha}^{-1}}{Mg/ha}
##' \eqn{\mathrm{hr}^{-1}}{per hour}).}
##'
##' \item{\code{CanopyCond}}{Hourly canopy conductance (units ?).}
##'
##' \item{\code{TranEpen}}{Hourly canopy transpiration according to Penman (\eqn{\mathrm{kg\,m}^{-2}}{kg/m2}
##' \eqn{\mathrm{hr}^{-1}}{per hour}) or canopy transpiration according to Penman
##' (\eqn{\mathrm{Mg\,ha}^{-1}}{Mg/ha} \eqn{\mathrm{hr}^{-1}}{per hour}).}
##'
##' \item{\code{TranEpen}}{Hourly canopy transpiration according to Priestly (\eqn{\mathrm{kg\,
##' m}^{-2}}{kg/m2} \eqn{\mathrm{hr}^{-1}}{per hour}) canopy transpiration according to
##' Priestly (\eqn{\mathrm{Mg\,ha}^{-1}}{Mg/ha} \eqn{\mathrm{hr}^{-1}}{per hour})}
##'
##' \item{\code{LayMat}}{Hourly by layer matrix containing details of the calculations by
##' layer (each layer is a row).  \code{col1}: Direct irradiance. \code{col2}: Diffuse
##' irradiance. \code{col3}: Leaf area in the sun. \code{col4}: Leaf area in the shade. \code{col5}:
##' Transpiration of leaf area in the sun. \code{col6}: Transpiration of leaf area in
##' the shade. \code{col7}: Assimilation of leaf area in the sun. \code{col8}: Assimilation of
##' leaf area in the shade. \code{col9}: Difference in temperature between the leaf and
##' the air (i.e. TLeaf - TAir) for leaves in sun. \code{col10}: Difference in
##' temperature between the leaf and the air (i.e. TLeaf - TAir) for leaves in
##' shade.  \code{col11}: Stomatal conductance for leaves in the sun. \code{col12}: Stomatal
##' conductance for leaves in the shade. \code{col13}: Nitrogen concentration in the
##' leaf (\eqn{\mathrm{g\,m}^{-2}}{g m-2}). \code{col14}: Vmax value as depending on leaf nitrogen.}
##'
##' @keywords models
##' @examples
##'
##' \dontrun{
##' data(doy124)
##' tmp <- numeric(24)
##'
##' for(i in 1:24){
##'    lai <- doy124[i, 1]
##'    doy <- doy124[i, 3]
##'    hr  <- doy124[i, 4]
##'  solar <- doy124[i, 5]
##'   temp <- doy124[i, 6]
##'     rh <- doy124[i, 7]
##'     ws <- doy124[i, 8]
##'
##'   tmp[i] <- CanA(lai, doy, hr, solar, temp, rh, ws)$CanopyAssim
##'
##' }
##'
##' plot(c(0:23), tmp,
##'             type='l', lwd=2,
##'             xlab='Hour',
##'             ylab=expression(paste('Canopy assimilation (kg  ',
##'             m^-2, ' ', h^-1, ')')))
##'
##' }
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

