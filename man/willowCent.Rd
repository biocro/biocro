\name{willowCent}
\alias{SoilType}
\alias{canopyParms}
\alias{centuryParms}
\alias{nitroParms}
\alias{phenoParms}
\alias{photoParms}
\alias{print.willowGro}
\alias{seneParms}
\alias{showSoilType}
\alias{soilParms}
\alias{willowCent}
\alias{willowGro}
\title{willowmass crops growth simulation}
\usage{
  willowCent(WetDat, day1 = 120, dayn = 300, timestep = 1,
    iRhizome = 1, lat = 40, iPlant = 1, irtl = 1e-04,
    canopyControl = list(), seneControl = list(),
    photoControl = list(), willowphenoControl = list(),
    soilControl = list(), nitroControl = list(),
    iPlantControl = list(), centuryControl = list())
}
\arguments{
  \item{WetDat}{weather data as produced by the
  \code{\link{weach}} function.}

  \item{day1}{first day of the growing season, (1--365).}

  \item{dayn}{last day of the growing season, (1--365, but
  larger than \code{day1}). See details.}

  \item{timestep}{Simulation timestep, the default of 1
  requires houlry weather data. A value of 3 would require
  weather data every 3 hours.  This number should be a
  divisor of 24.}

  \item{lat}{latitude, default 40.}

  \item{iRhizome}{initial dry biomass of the Rhizome (Mg
  \eqn{ha^{-1}}).}

  \item{irtl}{Initial rhizome proportion that becomes leaf.
  This should not typically be changed, but it can be used
  to indirectly control the effect of planting density.}

  \item{canopyControl}{List that controls aspects of the
  canopy simulation. It should be supplied through the
  \code{canopyParms} function.

  \code{Sp} (specific leaf area) here the units are ha
  \eqn{Mg^{-1}}.  If you have data in \eqn{m^2} of leaf per
  kg of dry matter (e.g. 15) then divide by 10 before
  inputting this coefficient.

  \code{nlayers} (number of layers of the canopy) Maximum
  50. To increase the number of layers (more than 50) the
  \code{C} source code needs to be changed slightly.

  \code{kd} (extinction coefficient for diffuse light)
  between 0 and 1.

  \code{mResp} (maintenance respiration) a vector of length
  2 with the first component for leaf and stem and the
  second component for rhizome and root.}

  \item{seneControl}{List that controls aspects of
  senescence simulation. It should be supplied through the
  \code{seneParms} function.

  \code{senLeaf} Thermal time at which leaf senescence will
  start.

  \code{senStem} Thermal time at which stem senescence will
  start.

  \code{senRoot} Thermal time at which root senescence will
  start.

  \code{senRhizome} Thermal time at which rhizome
  senescence will start.}

  \item{photoControl}{List that controls aspects of
  photosynthesis simulation. It should be supplied through
  the \code{photoParms} function.

  \code{vmax} Vmax passed to the \code{\link{c3photo}}
  function.

  \code{alpha} alpha parameter passed to the
  \code{\link{c3photo}} function.

  \code{theta} theta parameter passed to the
  \code{\link{c3photo}} function.

  \code{beta} beta parameter passed to the
  \code{\link{c3photo}} function.

  \code{Rd} Rd parameter passed to the
  \code{\link{c3photo}} function.

  \code{Catm} Catm parameter passed to the
  \code{\link{c3photo}} function.

  \code{b0} b0 parameter passed to the
  \code{\link{c3photo}} function.

  \code{b1} b1 parameter passed to the
  \code{\link{c3photo}} function.}

  \item{phenoControl}{List that controls aspects of the
  crop phenology. It should be supplied through the
  \code{phenoParms} function.

  \code{tp1-tp6} thermal times which determine the time
  elapsed between phenological stages. Between 0 and tp1 is
  the juvenile stage. etc.

  \code{kLeaf1-6} proportion of the carbon that is
  allocated to leaf for phenological stages 1 through 6.

  \code{kStem1-6} proportion of the carbon that is
  allocated to stem for phenological stages 1 through 6.

  \code{kRoot1-6} proportion of the carbon that is
  allocated to root for phenological stages 1 through 6.

  \code{kRhizome1-6} proportion of the carbon that is
  allocated to rhizome for phenological stages 1 through 6.

  \code{kGrain1-6} proportion of the carbon that is
  allocated to grain for phenological stages 1 through 6.
  At the moment only the last stage (i.e. 6 or
  post-flowering) is allowed to be larger than zero. An
  error will be returned if kGrain1-5 are different from
  zero.}

  \item{soilControl}{List that controls aspects of the soil
  environment. It should be supplied through the
  \code{soilParms} function.

  \code{FieldC} Field capacity. This can be used to
  override the defaults possible from the soil types (see
  \code{\link{showSoilType}}).

  \code{WiltP} Wilting point.  This can be used to override
  the defaults possible from the soil types (see
  \code{\link{showSoilType}}).

  \code{phi1} Parameter which controls the spread of the
  logistic function. See \code{\link{wtrstr}} for more
  details.

  \code{phi2} Parameter which controls the reduction of the
  leaf area growth due to water stress. See
  \code{\link{wtrstr}} for more details.

  \code{soilDepth} Maximum depth of the soil that the roots
  have access to (i.e. rooting depth).

  \code{iWatCont} Initial water content of the soil the
  first day of the growing season. It can be a single value
  or a vector for the number of layers specified.

  \code{soilType} Soil type, default is 6 (a more typical
  soil would be 3). To see details use the function
  \code{\link{showSoilType}}.

  \code{soilLayer} Integer between 1 and 50. The default is
  5. If only one soil layer is used the behavior can be
  quite different.

  \code{soilDepths} Intervals for the soil layers.

  \code{wsFun} one of 'logistic','linear','exp' or 'none'.
  Controls the method for the relationship between soil
  water content and water stress factor.

  \code{scsf} stomatal conductance sensitivity factor
  (default = 1). This is an empirical coefficient that
  needs to be adjusted for different species.

  \code{rfl} Root factor lambda. A Poisson distribution is
  used to simulate the distribution of roots in the soil
  profile and this parameter can be used to change the
  lambda parameter of the Poisson.

  \code{rsec} Radiation soil evaporation coefficient.
  Empirical coefficient used in the incidence of direct
  radiation on soil evaporation.

  \code{rsdf} Root soil depth factor. Empirical coefficient
  used in calculating the depth of roots as a function of
  root biomass.}

  \item{nitroControl}{List that controls aspects of the
  nitrogen environment. It should be supplied through the
  \code{nitrolParms} function.

  \code{iLeafN} initial value of leaf nitrogen (g m-2).

  \code{kLN} coefficient of decrease in leaf nitrogen
  during the growing season. The equation is LN = iLeafN *
  (Stem + Leaf)^-kLN .

  \code{Vmax.b1} slope which determines the effect of leaf
  nitrogen on Vmax.

  \code{alpha.b1} slope which controls the effect of leaf
  nitrogen on alpha.}

  \item{centuryControl}{List that controls aspects of the
  Century model for carbon and nitrogen dynamics in the
  soil. It should be supplied through the
  \code{centuryParms} function.

  \code{SC1-9} Soil carbon pools in the soil.  SC1:
  Structural surface litter.  SC2: Metabolic surface
  litter.  SC3: Structural root litter.  SC4: Metabolic
  root litter.  SC5: Surface microbe.  SC6: Soil microbe.
  SC7: Slow carbon.  SC8: Passive carbon.  SC9: Leached
  carbon.

  \code{LeafL.Ln} Leaf litter lignin content.

  \code{StemL.Ln} Stem litter lignin content.

  \code{RootL.Ln} Root litter lignin content.

  \code{RhizomeL.Ln} Rhizome litter lignin content.

  \code{LeafL.N} Leaf litter nitrogen content.

  \code{StemL.N} Stem litter nitrogen content.

  \code{RootL.N} Root litter nitrogen content.

  \code{RhizomeL.N} Rhizome litter nitrogen content.

  \code{Nfert} Nitrogen from a fertilizer source.

  \code{iMinN} Initial value for the mineral nitrogen pool.

  \code{Litter} Initial values of litter (leaf, stem, root,
  rhizome).

  \code{timestep} currently either week (default) or day.}
}
\value{
  a \code{\link{list}} structure with components
}
\description{
  Simulates dry biomass growth during an entire growing
  season.  It represents an integration of the
  photosynthesis function \code{\link{c3photo}}, canopy
  evapo/transpiration \code{\link{CanA}}, the multilayer
  canopy model \code{\link{sunML}} and a dry biomass
  partitioning calendar and senescence. It also considers,
  carbon and nitrogen cycles and water and nitrogen
  limitations.
}
\examples{
\dontrun{
data(weather05)

res0 <- willowGro(weather05)

plot(res0)

## Looking at the soil model

res1 <- willowGro(weather05, soilControl = soilParms(soilLayers = 6))
plot(res1, plot.kind='SW') ## Without hydraulic distribution
res2 <- willowGro(weather05, soilControl = soilParms(soilLayers = 6, hydrDist=TRUE))
plot(res2, plot.kind='SW') ## With hydraulic distribution


## Example of user defined soil parameters.
## The effect of phi2 on yield and soil water content

ll.0 <- soilParms(FieldC=0.37,WiltP=0.2,phi2=1)
ll.1 <- soilParms(FieldC=0.37,WiltP=0.2,phi2=2)
ll.2 <- soilParms(FieldC=0.37,WiltP=0.2,phi2=3)
ll.3 <- soilParms(FieldC=0.37,WiltP=0.2,phi2=4)

ans.0 <- willowGro(weather05,soilControl=ll.0)
ans.1 <- willowGro(weather05,soilControl=ll.1)
ans.2 <- willowGro(weather05,soilControl=ll.2)
ans.3 <-willowGro(weather05,soilControl=ll.3)

xyplot(ans.0$SoilWatCont +
       ans.1$SoilWatCont +
       ans.2$SoilWatCont +
       ans.3$SoilWatCont ~ ans.0$DayofYear,
       type='l',
       ylab='Soil water Content (fraction)',
       xlab='DOY')

## Compare LAI

xyplot(ans.0$LAI +
       ans.1$LAI +
       ans.2$LAI +
       ans.3$LAI ~ ans.0$DayofYear,
       type='l',
       ylab='Leaf Area Index',
       xlab='DOY')



}
}
\keyword{models}

