##  BioCro/R/CanA.R by Fernando Ezequiel Miguez  Copyright (C) 2007-2010

##' Simulates canopy assimilation
##'
##' It represents an integration of the photosynthesis function
##' \code{\link{c4photo}}, canopy evapo/transpiration and the multilayer canopy
##' model \code{\link{sunML}}.
##'
##'
##' @aliases CanA lnParms
##' @param lai leaf area index.
##' @param doy day of the year, (1--365).
##' @param hr hour of the day, (0--23).
##' @param solar solar radiation (\eqn{\mu}{micro} mol \eqn{m^{-2}}{m-2}
##' \eqn{s^{-1}}{s-1}).
##' @param temp temperature (Celsius).
##' @param rh relative humidity (0--1).
##' @param windspeed wind speed (m \eqn{s^{-1}}{s-1}).
##' @param lat latitude.
##' @param nlayers number of layers in the simulation of the canopy (max
##' allowed is 50).
##' @param kd Ligth extinction coefficient for diffuse light.
##' @param StomataWS coefficient controlling the effect of water stress on
##' stomatal conductance and assimilation.
##' @param chi.l leaf angle as described by projection of horizontal to vertical leaf area
##' @param leafwidth leaf width in meters
##' @param heightFactor Height Factor. Divide LAI by this number to get the
##' height of a crop.
##' @param photoControl list that sets the photosynthesis parameters. See
##' \code{\link{BioGro}}.
##' @param lnControl list that sets the leaf nitrogen parameters.
##'
##' LeafN: Initial value of leaf nitrogen (g m-2).
##'
##' kpLN: coefficient of decrease in leaf nitrogen during the growing season.
##' The equation is LN = iLeafN * exp(-kLN * TTc).
##'
##' lnFun: controls whether there is a decline in leaf nitrogen with the depth
##' of the canopy. 'none' means no decline, 'linear' means a linear decline
##' controlled by the following two parameters.
##'
##' lnb0: Intercept of the linear decline of leaf nitrogen in the depth of the
##' canopy.
##'
##' lnb1: Slope of the linear decline of leaf nitrogen in the depth of the
##' canopy. The equation is 'vmax = leafN_lay * lnb1 + lnb0'.
##' @param units Whether to return units in kg/m2/hr or Mg/ha/hr. This is
##' typically run at hourly intervals, that is why the hr is kept, but it could
##' be used with data at finer timesteps and then convert the results.
##' @export
##' @return
##'
##' \code{\link{list}}
##'
##' returns a list with several elements
##'
##' CanopyAssim: hourly canopy assimilation (\eqn{kg m^{-2}}{kg/m2}
##' \eqn{hr^{-1}}{per hour}) or canopy assimilation (\eqn{Mg ha^{-1}}{Mg/ha}
##' \eqn{hr^{-1}}{per hour})
##'
##' CanopyTrans: hourly canopy transpiration (\eqn{kg m^{-2}}{kg/m2}
##' \eqn{hr^{-1}}{per hour}) or canopy transpiration (\eqn{Mg ha^{-1}}{Mg/ha}
##' \eqn{hr^{-1}}{per hour})
##'
##' CanopyCond: hourly canopy conductance (units ?)
##'
##' TranEpen: hourly canopy transpiration according to Penman (\eqn{kg m^{-2}}
##' \eqn{hr^{-1}}{per hour}) or canopy transpiration according to Penman
##' (\eqn{Mg ha^{-1}} \eqn{hr^{-1}}{per hour})
##'
##' TranEpen: hourly canopy transpiration according to Priestly (\eqn{kg
##' m^{-2}}{kg/m2} \eqn{hr^{-1}}{per hour}) canopy transpiration according to
##' Priestly (\eqn{Mg ha^{-1}}{Mg/ha} \eqn{hr^{-1}}{per hour})
##'
##' LayMat: hourly by Layer matrix containing details of the calculations by
##' layer (each layer is a row).  col1: Direct Irradiance col2: Diffuse
##' Irradiance col3: Leaf area in the sun col4: Leaf area in the shade col5:
##' Transpiration of leaf area in the sun col6: Transpiration of leaf area in
##' the shade col7: Assimilation of leaf area in the sun col8: Assimilation of
##' leaf area in the shade col9: Difference in temperature between the leaf and
##' the air (i.e. TLeaf - TAir) for leaves in sun.  col10: Difference in
##' temperature between the leaf and the air (i.e. TLeaf - TAir) for leaves in
##' shade.  col11: Stomatal conductance for leaves in the sun col12: Stomatal
##' conductance for leaves in the shade col13: Nitrogen concentration in the
##' leaf (g m^-2) col14: Vmax value as depending on leaf nitrogen
##' @keywords models
##' @examples
##'
##' \dontrun{
##' data(doy124)
##' tmp <- numeric(24)
##'
##' for(i in 1:24){
##'    lai <- doy124[i,1]
##'    doy <- doy124[i,3]
##'    hr  <- doy124[i,4]
##'  solar <- doy124[i,5]
##'   temp <- doy124[i,6]
##'     rh <- doy124[i,7]
##'     ws <- doy124[i,8]
##'
##'   tmp[i] <- CanA(lai,doy,hr,solar,temp,rh,ws)$CanopyAssim
##'
##' }
##'
##' plot(c(0:23),tmp,
##'             type='l',lwd=2,
##'             xlab='Hour',
##'             ylab=expression(paste('Canopy assimilation (kg  ',
##'             m^-2,' ',h^-1,')')))
##'
##' }
CanA <- function(lai,doy,hr,solar,temp,rh,windspeed,
                 lat=40,nlayers=8,kd=0.1,StomataWS=1,
                 chi.l=1, leafwidth=0.04,
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
    upperT<-photoP$uppertemp
    lowerT<-photoP$lowertemp
    
    canenitroP <- canenitroParms()
    canenitroP [names(lnControl)] <- lnControl
    nnitroP<-as.vector(unlist(canenitroP))

    res <- .Call(CanA_sym,as.double(lai),as.integer(doy),
                 as.integer(hr),as.double(solar),as.double(temp),
                 as.double(rh),as.double(windspeed),
                 as.double(lat),as.integer(nlayers),as.double(StomataWS),
                 as.double(vmax),as.double(alpha),as.double(kparm),
                 as.double(theta), as.double(beta),
                 as.double(Rd),as.double(b0),
                 as.double(b1),as.double(Catm),
                 as.double(kd), as.double(heightFactor),
                 as.integer(ws), as.double(canenitroP$iLeafN),
                 as.double(canenitroP$kpLN), as.double(canenitroP$lnb0),
                 as.double(canenitroP$lnb1), as.integer(canenitroP$lnFun),
                 as.double(chi.l),as.double(upperT),
                 as.double(lowerT), as.double(nnitroP),
                 as.double(leafwidth))

    if(units == "Mg/ha/hr"){
      res
    }else{
      ## Need to go from Mg to kg and from ha to m2
      cf <- 1e3 * 1e-4
      res$CanopyAssim <- res$CanopyAssim * cf
      res$GrossCanopyAssim<-res$GrossCanopyAssim * cf
      ## This is in kg of biomass per m2 per hour
      res$CanopyTrans <- res$CanopyTrans * cf
      ## This is in kg of water per m2 per hour
      res$TranEpen <- res$TranEpen * cf
      res$TranEpries <- res$TranEpries * cf
    }
    res
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
