## BioCro/R/BioCro.R by Fernando Ezequiel Miguez Copyright (C) 2007-2010

##' Biomass crops growth simulation
##'
##' Simulates dry biomass growth during an entire growing season.  It
##' represents an integration of the photosynthesis function
##' \code{\link{c4photo}}, canopy evapo/transpiration \code{\link{CanA}}, the
##' multilayer canopy model \code{\link{sunML}} and a dry biomass partitioning
##' calendar and senescence. It also considers, carbon and nitrogen cycles and
##' water and nitrogen limitations.
##'
##'
##' @aliases BioGro print.BioGro soilParms nitroParms phenoParms photoParms
##' canopyParms seneParms centuryParms showSoilType SoilType
##' @param WetDat weather data as produced by the \code{\link{weach}} function.
##' @param day1 first day of the growing season, (1--365).
##' @param dayn last day of the growing season, (1--365, but larger than
##' \code{day1}). See details.
##' @param timestep Simulation timestep, the default of 1 requires houlry
##' weather data. A value of 3 would require weather data every 3 hours.  This
##' number should be a divisor of 24.
##' @param lat latitude, default 40.
##' @param iRhizome initial dry biomass of the Rhizome (Mg \eqn{ha^{-1}}).
##' @param irtl Initial rhizome proportion that becomes leaf. This should not
##' typically be changed, but it can be used to indirectly control the effect
##' of planting density.
##' @param canopyControl List that controls aspects of the canopy simulation.
##' It should be supplied through the \code{canopyParms} function.
##'
##' \code{Sp} (specific leaf area) here the units are ha \eqn{Mg^{-1}}.  If you
##' have data in \eqn{m^2} of leaf per kg of dry matter (e.g. 15) then divide
##' by 10 before inputting this coefficient.
##'
##' \code{nlayers} (number of layers of the canopy) Maximum 50. To increase the
##' number of layers (more than 50) the \code{C} source code needs to be
##' changed slightly.
##'
##' \code{kd} (extinction coefficient for diffuse light) between 0 and 1.
##'
##' \code{mResp} (maintenance respiration) a vector of length 2 with the first
##' component for leaf and stem and the second component for rhizome and root.
##' @param seneControl List that controls aspects of senescence simulation. It
##' should be supplied through the \code{seneParms} function.
##'
##' \code{senLeaf} Thermal time at which leaf senescence will start.
##'
##' \code{senStem} Thermal time at which stem senescence will start.
##'
##' \code{senRoot} Thermal time at which root senescence will start.
##'
##' \code{senRhizome} Thermal time at which rhizome senescence will start.
##' @param photoControl List that controls aspects of photosynthesis
##' simulation. It should be supplied through the \code{photoParms} function.
##'
##' \code{vmax} Vmax passed to the \code{\link{c4photo}} function.
##'
##' \code{alpha} alpha parameter passed to the \code{\link{c4photo}} function.
##'
##' \code{kparm} kparm parameter passed to the \code{\link{c4photo}} function.
##'
##' \code{theta} theta parameter passed to the \code{\link{c4photo}} function.
##'
##' \code{beta} beta parameter passed to the \code{\link{c4photo}} function.
##'
##' \code{Rd} Rd parameter passed to the \code{\link{c4photo}} function.
##'
##' \code{Catm} Catm parameter passed to the \code{\link{c4photo}} function.
##'
##' \code{b0} b0 parameter passed to the \code{\link{c4photo}} function.
##'
##' \code{b1} b1 parameter passed to the \code{\link{c4photo}} function.
##' @param phenoControl List that controls aspects of the crop phenology. It
##' should be supplied through the \code{phenoParms} function.
##'
##' \code{tp1-tp6} thermal times which determine the time elapsed between
##' phenological stages. Between 0 and tp1 is the juvenile stage. etc.
##'
##' \code{kLeaf1-6} proportion of the carbon that is allocated to leaf for
##' phenological stages 1 through 6.
##'
##' \code{kStem1-6} proportion of the carbon that is allocated to stem for
##' phenological stages 1 through 6.
##'
##' \code{kRoot1-6} proportion of the carbon that is allocated to root for
##' phenological stages 1 through 6.
##'
##' \code{kRhizome1-6} proportion of the carbon that is allocated to rhizome
##' for phenological stages 1 through 6.
##'
##' \code{kGrain1-6} proportion of the carbon that is allocated to grain for
##' phenological stages 1 through 6. At the moment only the last stage (i.e. 6
##' or post-flowering) is allowed to be larger than zero. An error will be
##' returned if kGrain1-5 are different from zero.
##' @param soilControl List that controls aspects of the soil environment. It
##' should be supplied through the \code{soilParms} function.
##'
##' \code{FieldC} Field capacity. This can be used to override the defaults
##' possible from the soil types (see \code{\link{showSoilType}}).
##'
##' \code{WiltP} Wilting point.  This can be used to override the defaults
##' possible from the soil types (see \code{\link{showSoilType}}).
##'
##' \code{phi1} Parameter which controls the spread of the logistic function.
##' See \code{\link{wtrstr}} for more details.
##'
##' \code{phi2} Parameter which controls the reduction of the leaf area growth
##' due to water stress. See \code{\link{wtrstr}} for more details.
##'
##' \code{soilDepth} Maximum depth of the soil that the roots have access to
##' (i.e. rooting depth).
##'
##' \code{iWatCont} Initial water content of the soil the first day of the
##' growing season. It can be a single value or a vector for the number of
##' layers specified.
##'
##' \code{soilType} Soil type, default is 6 (a more typical soil would be 3).
##' To see details use the function \code{\link{showSoilType}}.
##'
##' \code{soilLayer} Integer between 1 and 50. The default is 5. If only one
##' soil layer is used the behavior can be quite different.
##'
##' \code{soilDepths} Intervals for the soil layers.
##'
##' \code{wsFun} one of 'logistic','linear','exp' or 'none'. Controls the
##' method for the relationship between soil water content and water stress
##' factor.
##'
##' \code{scsf} stomatal conductance sensitivity factor (default = 1). This is
##' an empirical coefficient that needs to be adjusted for different species.
##'
##' \code{rfl} Root factor lambda. A Poisson distribution is used to simulate
##' the distribution of roots in the soil profile and this parameter can be
##' used to change the lambda parameter of the Poisson.
##'
##' \code{rsec} Radiation soil evaporation coefficient. Empirical coefficient
##' used in the incidence of direct radiation on soil evaporation.
##'
##' \code{rsdf} Root soil depth factor. Empirical coefficient used in
##' calculating the depth of roots as a function of root biomass.
##' @param nitroControl List that controls aspects of the nitrogen environment.
##' It should be supplied through the \code{nitrolParms} function.
##'
##' \code{iLeafN} initial value of leaf nitrogen (g m-2).
##'
##' \code{kLN} coefficient of decrease in leaf nitrogen during the growing
##' season. The equation is LN = iLeafN * (Stem + Leaf)^-kLN .
##'
##' \code{Vmax.b1} slope which determines the effect of leaf nitrogen on Vmax.
##'
##' \code{alpha.b1} slope which controls the effect of leaf nitrogen on alpha.
##' @param centuryControl List that controls aspects of the Century model for
##' carbon and nitrogen dynamics in the soil. It should be supplied through the
##' \code{centuryParms} function.
##'
##' \code{SC1-9} Soil carbon pools in the soil.  SC1: Structural surface
##' litter.  SC2: Metabolic surface litter.  SC3: Structural root litter.  SC4:
##' Metabolic root litter.  SC5: Surface microbe.  SC6: Soil microbe.  SC7:
##' Slow carbon.  SC8: Passive carbon.  SC9: Leached carbon.
##'
##' \code{LeafL.Ln} Leaf litter lignin content.
##'
##' \code{StemL.Ln} Stem litter lignin content.
##'
##' \code{RootL.Ln} Root litter lignin content.
##'
##' \code{RhizomeL.Ln} Rhizome litter lignin content.
##'
##' \code{LeafL.N} Leaf litter nitrogen content.
##'
##' \code{StemL.N} Stem litter nitrogen content.
##'
##' \code{RootL.N} Root litter nitrogen content.
##'
##' \code{RhizomeL.N} Rhizome litter nitrogen content.
##'
##' \code{Nfert} Nitrogen from a fertilizer source.
##'
##' \code{iMinN} Initial value for the mineral nitrogen pool.
##'
##' \code{Litter} Initial values of litter (leaf, stem, root, rhizome).
##'
##' \code{timestep} currently either week (default) or day.
##' @export
##' @return
##'
##' a \code{\link{list}} structure with components
##' \itemize{
##' \item DayofYear Day of the year
##' \item Hour Hour for each day
##' \item CanopyAssim Hourly canopy assimilation, (Mg \eqn{ha^-1} ground
##' \eqn{hr^-1}).
##' \item CanopyTrans Hourly canopy transpiration, (Mg \eqn{ha^-1} ground
##' \eqn{hr^-1}).
##' \item Leaf leaf dry biomass (Mg \eqn{ha^-1}).
##' \item Stem stem dry biomass(Mg \eqn{ha^-1}).
##' \item Root root dry biomass (Mg \eqn{ha^-1}).
##' \item Rhizome rhizome dry biomass (Mg \eqn{ha^-1}).
##' \item LAI leaf area index (\eqn{m^2} \eqn{m^-2}).
##' \item ThermalT thermal time (Celsius \eqn{day^-1}).
##' \item StomatalCondCoefs Coefficeint which determines the effect of
##' water stress on stomatal conductance and photosynthesis.
##' \item LeafReductionCoefs Coefficient which determines the effect of
##' water stress on leaf expansion reduction.
##' \item LeafNitrogen Leaf nitrogen.
##' \item AboveLitter Above ground biomass litter (Leaf + Stem).
##' \item BelowLitter Below ground biomass litter (Root + Rhizome).
##' \item VmaxVec Value of Vmax during the growing season.
##' \item AlphaVec Value of alpha during the growing season.
##' \item SpVec Value of the specific leaf area.
##' \item MinNitroVec Nitrogen in the mineral pool.
##' \item RespVec Soil respiration.
##' \item SoilEvaporation Soil Evaporation.
##' }
##' @keywords models
##' @examples
##'
##' \dontrun{
##' data(weather05)
##'
##' res0 <- BioGro(weather05)
##'
##' plot(res0)
##'
##' ## Looking at the soil model
##'
##' res1 <- BioGro(weather05, soilControl = soilParms(soilLayers = 6))
##' plot(res1, plot.kind='SW') ## Without hydraulic distribution
##' res2 <- BioGro(weather05, soilControl = soilParms(soilLayers = 6, hydrDist=TRUE))
##' plot(res2, plot.kind='SW') ## With hydraulic distribution
##'
##'
##' ## Example of user defined soil parameters.
##' ## The effect of phi2 on yield and soil water content
##'
##' ll.0 <- soilParms(FieldC=0.37,WiltP=0.2,phi2=1)
##' ll.1 <- soilParms(FieldC=0.37,WiltP=0.2,phi2=2)
##' ll.2 <- soilParms(FieldC=0.37,WiltP=0.2,phi2=3)
##' ll.3 <- soilParms(FieldC=0.37,WiltP=0.2,phi2=4)
##'
##' ans.0 <- BioGro(weather05,soilControl=ll.0)
##' ans.1 <- BioGro(weather05,soilControl=ll.1)
##' ans.2 <- BioGro(weather05,soilControl=ll.2)
##' ans.3 <-BioGro(weather05,soilControl=ll.3)
##'
##' lattice::xyplot(ans.0$SoilWatCont +
##'        ans.1$SoilWatCont +
##'        ans.2$SoilWatCont +
##'        ans.3$SoilWatCont ~ ans.0$DayofYear,
##'        type='l',
##'        ylab='Soil water Content (fraction)',
##'        xlab='DOY')
##'
##' ## Compare LAI
##'
##' lattice::xyplot(ans.0$LAI +
##'        ans.1$LAI +
##'        ans.2$LAI +
##'        ans.3$LAI ~ ans.0$DayofYear,
##'        type='l',
##'        ylab='Leaf Area Index',
##'        xlab='DOY')
##'
##'
##'
##' }
##' @export
CropGro <- function(WetDat, day1=NULL, dayn=NULL,
                   timestep=1,
                   lat=40,iRhizome=7,irtl=1e-4,
                   canopyControl=list(),
                   seneControl=list(),
                   photoControl=list(),
                   phenoControl=list(),
                   soilControl=list(),
                   nitroControl=list(),
                   SOMpoolsControl=list(),
                   SOMAssignParmsControl=list(),
                   GetCropCentStateVarParmsControl=list(),
                    GetSoilTextureParmsControl=list(),
                    GetBioCroToCropcentParmsControl=list(),
                    GetErosionParmsControl=list(),
                    GetC13ParmsControl=list(),
                    GetLeachingParmsControl=list(),
                    GetSymbNFixationParmsControl=list(),
                    centuryControl=list())
  {

    
## Trying to guess the first and last day of the growing season from weather data
    
    if(missing(day1)){
        half <- as.integer(dim(WetDat)[1]/2)
        WetDat1 <- WetDat[1:half,c(2,5)]
        if(min(WetDat1[,2]) > 0){
          day1 <- 90
        }else{
        WetDat1s <- WetDat1[which(WetDat1[,2]<0),]
        day1 <- max(WetDat1s[,1])
        if(day1 < 90) day1 <- 90
      }
     }
    if(missing(dayn)){
        half <- as.integer(dim(WetDat)[1]/2)
        WetDat1 <- WetDat[half:dim(WetDat)[1],c(2,5)]
        if(min(WetDat1[,2]) > 0){
          dayn <- 330
        }else{
          WetDat1s <- WetDat1[which(WetDat1[,2]<0),]
          dayn <- min(WetDat1s[,1])
          if(dayn > 330) dayn <- 330
        }
      }
    
#    if((day1<0) || (day1>365) || (dayn<0) || (dayn>365))
#      stop("day1 and dayn should be between 0 and 365")

    if(day1 > dayn)
      stop("day1 should be smaller than dayn")

    if( (timestep<1) || (24%%timestep != 0))
      stop("timestep should be a divisor of 24 (e.g. 1,2,3,4,6,etc.)")
   ##################################################
    ## SOM 
    SOMPools <- assignPoolsParms()
    SOMPools[names(SOMpoolsControl)] <-SOMpoolsControl
    sompools<-as.vector(unlist(SOMPools))
    
    ################assignParms
    SOMAssignParms <- assignParms ()
    SOMAssignParms[names(SOMAssignParmsControl)] <- SOMAssignParmsControl
    somassignparms <-as.vector(unlist(SOMAssignParms))
    
    ################GetCropCentStateVarParms
    GetCropCentStateVarParms <- GetCropCentStateVarParms()
    GetCropCentStateVarParms[names(GetCropCentStateVarParmsControl)] <- GetCropCentStateVarParmsControl
    getcropcentstatevarparms <- as.vector(unlist(GetCropCentStateVarParms))
    
    #################double *getsoiltexturefromR
    GetSoilTextureParms <- GetSoilTexture()
    GetSoilTextureParms[names(GetSoilTextureParmsControl)]<-GetSoilTextureParmsControl
    getsoiltextureparms <- as.vector(unlist(GetSoilTextureParms))
    
    #############*getbiocrotocropcentparmsfromR
    GetBioCroToCropcentParms <- GetBioCroToCropcentParms()
    GetBioCroToCropcentParms[names(GetBioCroToCropcentParmsControl)]<-GetBioCroToCropcentParmsControl
    getbiocrotocropcentparms <- as.vector(unlist(GetBioCroToCropcentParms))
    
    ##################*geterosionparmsfromR
    GetErosionParms <- GetErosionParms()
    GetErosionParms[names(GetErosionParmsControl)] <- GetErosionParmsControl
    geterosionparms <- as.vector(unlist(GetErosionParms))
    
    ##############getc13parmsfromR
    GetC13Parms <- GetC13Parms()
    GetC13Parms[names(GetC13ParmsControl)] <- GetC13ParmsControl
    getc13parms <- as.vector(unlist(GetC13Parms))
   
    ###########################getleachingparms
    GetLeachingParms <- GetLeachingParms()
    GetLeachingParms[names(GetLeachingParmsControl)]<-GetLeachingParmsControl
    getleachingparms <- as.vector(unlist(GetLeachingParms))
    
    #########################getsymbnfixationparmsfromR
    GetSymbNFixationParms <- GetSymbNFixationParms()
    GetSymbNFixationParms[names(GetSymbNFixationParmsControl)] <- GetSymbNFixationParmsControl
    getsymbnfixationparms <- as.vector(unlist(GetSymbNFixationParms))
    ################################################
    
    ## Getting the Parameters
    canopyP <- canopyParms()
    canopyP[names(canopyControl)] <- canopyControl


    ###########################################################################    
    ## Here I am creating default multi-layer soil input such that each layer is approximately 5 cm
    
    tmp<- soilParms()                              # default depth
    tmp[names(soilControl)]=soilControl            # depth from soilControl
    NumberofLayers=round(tmp$soilDepth*100/5.0, 0)    #Calculate Number of Layres
    soilP<-soilParms(FieldC = tmp$FieldC, WiltP = tmp$WiltP, phi1 = tmp$phi1, phi2 = tmp$phi2, 
            soilDepth = tmp$soilDepth, iWatCont = tmp$iWatCont, soilType = tmp$soilType, 
            soilLayers = NumberofLayers, wsFun = "linear", 
            scsf = tmp$scsf, transpRes = tmp$transpRes, leafPotTh = tmp$leafPotTh, 
            hydrDist =1, rfl = tmp$rfl, rsec = tmp$rsec, rsdf = tmp$rsdf)
   
    ######################################################################

    nitroP <- nitroParms()
    nitroP[names(nitroControl)] <- nitroControl
 ## this is to pass NNITROP for compatibility
    nnitroP <- canenitroParms()
    nnitroP<-as.vector(unlist(nnitroP))
    
    
    phenoP <- phenoParms()
    phenoP[names(phenoControl)] <- phenoControl

    photoP <- photoParms()
    photoP[names(photoControl)] <- photoControl

    seneP <- seneParms()
    seneP[names(seneControl)] <- seneControl

    centuryP <- centuryParms()
    centuryP[names(centuryControl)] <- centuryControl

    tint <- 24 / timestep
    vecsize <- (dayn - (day1-1)) * tint + 1
    indes1 <- (day1-1) * tint
    indesn <- (dayn) * tint
    
    doy <- WetDat[indes1:indesn,2]
    hr <- WetDat[indes1:indesn,3]
    solar <- WetDat[indes1:indesn,4]
    temp <- WetDat[indes1:indesn,5]
    rh <- WetDat[indes1:indesn,6]
    windspeed <- WetDat[indes1:indesn,7]
    precip <- WetDat[indes1:indesn,8]

    if(max(rh) > 1)
      stop("Rel Hum. should be 0 < rh < 1")
    if((min(hr) < 0) | (max(hr) > 23))
      stop("hr should be between 0 and 23")
    
    DBPcoefs <- valid_dbp(as.vector(unlist(phenoP)[7:31]))

    TPcoefs <- as.vector(unlist(phenoP)[1:6])

    SENcoefs <- as.vector(unlist(seneP))

    soilCoefs <- c(unlist(soilP[1:5]),mean(soilP$iWatCont),soilP$scsf, soilP$transpRes, soilP$leafPotTh)
    wsFun <- soilP$wsFun
    soilType <- soilP$soilType

    centCoefs <- as.vector(unlist(centuryP)[1:24])

    if(centuryP$timestep == "year"){
      stop("Not developed yet")
      centTimestep <- dayn - day1 ## This is really the growing season
    }
    if(centuryP$timestep == "week") centTimestep <- 7
    if(centuryP$timestep == "day") centTimestep <- 1
    
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
    upperT<-photoP$UPPERTEMP
    lowerT<-photoP$LOWERTEMP
    
    mResp <- canopyP$mResp
    kd <- canopyP$kd
    chi.l <- canopyP$chi.l
    Sp <- canopyP$Sp
    SpD <- canopyP$SpD
    heightF <- canopyP$heightFactor
    nlayers <- canopyP$nlayers
    
    res <- .Call("CropGro",
                 as.double(lat),
                 as.integer(doy),
                 as.integer(hr),
                 as.double(solar),
                 as.double(temp),
                 as.double(rh),
                 as.double(windspeed),
                 as.double(precip),
                 as.double(kd),
                 as.double(c(chi.l,heightF)),
                 as.integer(nlayers),
                 as.double(iRhizome),
                 as.double(irtl),
                 as.double(SENcoefs),
                 as.integer(timestep),
                 as.integer(vecsize),
                 as.double(Sp),
                 as.double(SpD),
                 as.double(DBPcoefs),
                 as.double(TPcoefs),
                 as.double(vmax),
                 as.double(alpha),
                 as.double(kparm),
                 as.double(theta),
                 as.double(beta),
                 as.double(Rd),
                 as.double(Catm),
                 as.double(b0),
                 as.double(b1),
                 as.integer(ws),
                 as.double(soilCoefs),
                 as.double(nitroP$iLeafN),
                 as.double(nitroP$kLN), 
                 as.double(nitroP$Vmax.b1),
                 as.double(nitroP$alpha.b1),
                 as.double(mResp),
                 as.integer(soilType),
                 as.integer(wsFun),
                 as.double(centCoefs),
                 as.integer(centTimestep),
                 as.double(centuryP$Ks),
                 as.integer(soilP$soilLayers),
                 as.double(soilP$soilDepths),
                 as.double(soilP$iWatCont),
                 as.integer(soilP$hydrDist),
                 as.double(c(soilP$rfl,soilP$rsec,soilP$rsdf)),
                 as.double(nitroP$kpLN),
                 as.double(nitroP$lnb0),
                 as.double(nitroP$lnb1),
                 as.integer(nitroP$lnFun),
                 as.double(upperT),
                 as.double(lowerT),
                 as.double(nnitroP),
                 as.double(sompools),
                 as.double(getsoiltextureparms),
                 as.double(getbiocrotocropcentparms),
                 as.double(geterosionparms),
                 as.double(getc13parms),
                 as.double(getleachingparms),
                 as.double(getsymbnfixationparms),
                 as.double(somassignparms),
                 as.double(getcropcentstatevarparms)
                 )
    
    res$cwsMat <- t(res$cwsMat)
    colnames(res$cwsMat) <- soilP$soilDepths[-1]
    res$rdMat <- t(res$rdMat)
    colnames(res$rdMat) <- soilP$soilDepths[-1]
    res$psimMat <- t(res$psimMat)
    colnames(res$psimMat) <- soilP$soilDepths[-1]
    res$waterfluxMat <- t(res$waterfluxMat)
    colnames(res$waterfluxMat) <- soilP$soilDepths[-1]
    structure(res,class="BioGro")
  }



