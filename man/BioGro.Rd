\name{BioGro}
\alias{BioGro}
\alias{print.BioGro}
\alias{soilParms}
\alias{nitroParms}
\alias{phenoParms}
\alias{photoParms}
\alias{canopyParms}
\alias{seneParms}
\alias{centuryParms}
\alias{showSoilType}
\alias{SoilType}
\title{Biomass crops growth simulation}

\description{
  Simulates dry biomass growth during an entire growing
  season.  It represents an integration of the photosynthesis function
  \code{\link{c4photo}}, canopy evapo/transpiration \code{\link{CanA}},
  the multilayer canopy model \code{\link{sunML}} and a dry biomass
  partitioning calendar and senescence. It also considers, carbon and
  nitrogen cycles and water and nitrogen limitations.
}

\usage{
BioGro(WetDat, day1=NULL, dayn=NULL,
                   timestep=1,
                   lat=40,iRhizome=7,irtl=1e-4,
                   canopyControl=list(),
                   seneControl=list(),
                   photoControl=list(),
                   phenoControl=list(),
                   soilControl=list(),
                   nitroControl=list(),
                   centuryControl=list())
}

\arguments{
  
  \item{WetDat}{weather data as produced by the
    \code{\link{weach}} function.}

\item{day1}{first day of the growing season, (1--365).}

\item{dayn}{last day of the growing season, (1--365, but
  larger than \code{day1}). See details.}

\item{timestep}{Simulation timestep, the default of 1 requires houlry
  weather data. A value of 3 would require weather data every 3 hours.
  This number should be a divisor of 24.}

\item{lat}{latitude, default 40.}

\item{iRhizome}{initial dry biomass of the Rhizome (Mg \eqn{ha^{-1}}).}

\item{irtl}{Initial rhizome proportion that becomes leaf. This should not
typically be changed, but it can be used to indirectly control the effect of
planting density.}

\item{canopyControl}{List that controls aspects of the canopy simulation. It should be supplied through the \code{canopyParms} function. 

\code{Sp} (specific leaf area) here the units are ha \eqn{Mg^{-1}}.
  If you have data in \eqn{m^2} of leaf per kg of dry matter (e.g. 15)
  then divide by 10 before inputting this coefficient.

  \code{SpD} decrease of specific leaf area. Empirical
  parameter. Default 0. example value (1.7e-3).

\code{nlayers} (number of layers of the canopy) 
  Maximum 50. To increase the number of layers (more than 50) the \code{C}
  source code needs to be changed slightly.

\code{kd} (extinction coefficient for diffuse light) between 0 and 1. It is typically much lower than the extinction coefficient for direct light.

\code{mResp} (maintenance respiration) a vector of length 2 with the first component for leaf and stem and the second component for rhizome and root.

\code{heightFactor} (height factor) the cummulative LAI in the canopy model will
be divided by this number to obtain the height of the canopy. A canopy of LAI
equal to 3 will be one meter tall using the default.

}

\item{seneControl}{ List that controls aspects of senescence simulation. It should be supplied through the \code{seneParms} function.

\code{senLeaf} Thermal time at which leaf senescence will start.

\code{senStem} Thermal time at which stem senescence will start.

\code{senRoot} Thermal time at which root senescence will start.

\code{senRhizome} Thermal time at which rhizome senescence will start.

}

\item{photoControl}{  List that controls aspects of photosynthesis simulation. It should be supplied through the \code{photoParms} function.

\code{vmax} Vmax passed to the \code{\link{c4photo}} function.

\code{alpha} alpha parameter passed to the \code{\link{c4photo}}
  function.

\code{kparm} kparm parameter passed to the \code{\link{c4photo}}
function.

\code{theta} theta parameter passed to the \code{\link{c4photo}}
function.

\code{beta} beta parameter passed to the \code{\link{c4photo}}
function.

\code{Rd} Rd parameter passed to the \code{\link{c4photo}}
function.

\code{UPPERTEMP} UPPERTEMP parameter passed to the \code{\link{c4photo}}
function.

\code{LOWERTEMP} LOWERTEMP parameter passed to the \code{\link{c4photo}}
function.

\code{Catm} Catm parameter passed to the \code{\link{c4photo}}
  function.

\code{b0} b0 parameter passed to the \code{\link{c4photo}}
  function.

\code{b1} b1 parameter passed to the \code{\link{c4photo}}
  function.

\code{ws} water stress method. If \code{"gs"} is chosen the water stress factor
will be applied to the stomatal conductance. If \code{"vmax"} is chosen, it will
be applied to vmax.

}

\item{phenoControl}{ List that controls aspects of the crop phenology. It should be supplied through the \code{phenoParms} function.  

\code{tp1-tp6} thermal times which determine the time elapsed between phenological stages. Between 0 and tp1 is the juvenile stage. etc.

\code{kLeaf1-6} proportion of the carbon that is allocated to leaf for phenological stages 1 through 6.

\code{kStem1-6} proportion of the carbon that is allocated to stem for phenological stages 1 through 6.

\code{kRoot1-6} proportion of the carbon that is allocated to root for phenological stages 1 through 6.

\code{kRhizome1-6} proportion of the carbon that is allocated to rhizome
for phenological stages 1 through 6.

\code{kGrain6} proportion of the carbon that is allocated to grain for
phenological stage 6. An error will be
returned if kGrain6 less than zero.

}


\item{soilControl}{ List that controls aspects of the soil environment. It should be supplied through the \code{soilParms} function.  

\code{FieldC} Field capacity. This can be used to override the defaults
possible from the soil types (see \code{\link{showSoilType}}).

\code{WiltP} Wilting point.  This can be used to override the defaults
possible from the soil types (see \code{\link{showSoilType}}).

\code{phi1} Parameter which controls the spread of the logistic function.
See \code{\link{wtrstr}} for more details.

\code{phi2} Parameter which controls the reduction of the leaf area growth due to water stress.
See \code{\link{wtrstr}} for more details.

\code{soilDepths} Maximum depth of the soil that the roots have access to (i.e. rooting depth).

\code{hydrDist} default is 0 which means that hydraulic re distribution is not
computed. If a number other than 0 is used it will be interpreted as 'TRUE' and
the hydraulic redistribution in the profile will be calculated. This is only
relevant for a soil with multiple layers.

\code{iWatCont} Initial water content of the soil the first day of the
growing season. It can be a single value or a vector for the number of
layers specified.

\code{soilType} Soil type, default is 6 (a more typical soil would be 3). To see details use the function
\code{\link{showSoilType}}.

\code{soilLayers} Integer between 1 and 50. The default is 5. If only one
soil layer is used the behavior can be quite different.

\code{soilDepths} Intervals for the soil layers. As a default, the soil will be
divided in equal depths if more than one layer is specified, but this argument
can be used to change this. For example, three layers can be constructed by
supplying a vector such as \code{c(0,5,30,1)}.

\code{wsFun} one of "logistic","linear","exp", "none" or "lwp". Controls the
method for the relationship between soil water content and water stress
factor. See \code{\link{wtrstr}} for more details for the first four
options. The option "lwp" is more mechanistic and it attempts to calculate the
leaf and soil water potential.

\code{scsf} stomatal conductance sensitivity factor (default = 1). This is an
empirical coefficient that needs to be adjusted for different species. It only
becomes relevant if the "lwp" for wsFun is used. The equation is 1 - (leafPotTh -
LeafPsim)/1000 * scsf for the calculation of the water stress factor.

\code{transpRes} Resistance value for the equation used to calculate leaf water
potential. This is Psi_leaf = Psi_soil - Evapo * transpRes.

\code{leafPotTh} Leaf potential threshold at which water stress starts to occur.

\code{rfl} Root factor lambda. A Poisson distribution is used to
simulate the distribution of roots in the soil profile and this
parameter can be used to change the lambda parameter of the Poisson.

\code{rsec} Radiation soil evaporation coefficient. Empirical
coefficient used in the incidence of direct radiation on soil
evaporation.

\code{rsdf} Root soil depth factor. Empirical coefficient used in
calculating the depth of roots as a function of root biomass.

}

\item{nitroControl}{ List that controls aspects of the nitrogen environment. It should be supplied through the \code{nitrolParms} function.  

\code{iLeafN} initial value of leaf nitrogen (g m-2).

\code{kLN} coefficient of decrease in leaf nitrogen during the
growing season. The equation is LN = iLeafN * (Stem + Leaf)^-kLN .

\code{Vmax.b1} slope which determines the effect of leaf nitrogen on Vmax.

\code{alpha.b1} slope which controls the effect of leaf nitrogen on alpha.

}

\item{centuryControl}{ List that controls aspects of the Century model for carbon and nitrogen dynamics in the soil. It should be supplied through the \code{centuryParms} function.  

  \code{SC1-9} Soil carbon pools.

  SC1: Structural surface litter.

  SC2: Metabolic surface litter.

  SC3: Structural root litter.

  SC4: Metabolic root litter.

  SC5: Surface microbe.

  SC6: Soil microbe.

  SC7: Slow carbon.

  SC8: Passive carbon.

  SC9: Leached carbon.

\code{LeafL.Ln} Leaf litter lignin content. 

\code{StemL.Ln} Stem litter lignin content. 

\code{RootL.Ln} Root litter lignin content. 

\code{RhizomeL.Ln} Rhizome litter lignin content.

\code{LeafL.N} Leaf litter nitrogen content. 

\code{StemL.N} Stem litter nitrogen content. 

\code{RootL.N} Root litter nitrogen content. 

\code{RhizomeL.N} Rhizome litter nitrogen content.  

\code{Nfert} Nitrogen from a fertilizer source. Should be a vector of length
2. The first is the amount of N fertilizer (kg/ha) the second should be the doy
when it was applied.

\code{iMinN} Initial value for the mineral nitrogen pool.

\code{Litter} Initial values of litter (leaf, stem, root, rhizome).

\code{timestep} currently either week (default) or day.


}

}

\value{

  a \code{\link{list}} structure with components

  \item{DayofYear}{ Day of the year}

  \item{Hour}{Hour for each day}
  
  \item{CanopyAssim}{Hourly canopy assimilation,
    (Mg \eqn{ha^-1} ground \eqn{hr^-1}).}

    \item{CanopyTrans}{Hourly canopy transpiration,
      (Mg \eqn{ha^-1} ground \eqn{hr^-1}).}

  \item{Leaf}{leaf dry biomass (Mg \eqn{ha^-1}).}

  \item{Stem}{stem dry biomass(Mg \eqn{ha^-1}).} 

  \item{Root}{root dry biomass (Mg \eqn{ha^-1}).}

  \item{Rhizome}{rhizome dry biomass (Mg \eqn{ha^-1}).}

  \item{LAI}{leaf area index (\eqn{m^2} \eqn{m^-2}).}

  \item{ThermalT}{thermal time (Celsius \eqn{day^-1}).}

  \item{StomatalCondCoefs}{Coefficeint which determines the effect of water stress on stomatal conductance and photosynthesis (dimensionless 0-1).}

  \item{LeafReductionCoefs}{Coefficient which determines the effect of water stress on leaf expansion reduction (dimensionless 0-1).}

  \item{LeafNitrogen}{ Leaf nitrogen. (units ?, g/m2)}

  \item{AboveLitter}{ Above ground biomass litter (Leaf + Stem). Units, Mg/ha.}

  \item{BelowLitter}{ Below ground biomass litter (Root + Rhizome). Units, Mg/ha.}

  \item{VmaxVec}{ Value of Vmax during the growing season. Constant if no N stress is applied. For details see \code{c4photo}}

  \item{AlphaVec}{ Value of alpha during the growing season. Constant at the moment. For details see \code{c4photo}}

  \item{SpVec}{ Value of the specific leaf area. Constant if SpD is left at default of 0.}

  \item{MinNitroVec}{ Nitrogen in the mineral pool. It gives nonsense at the moment. Units ?}

  \item{RespVec}{ Soil respiration. It gives nonsense at the moment. Units ?}

  \item{SoilEvaporation}{ Soil Evaporation. Units ? Mg/ha}

  \item{cwsMat}{Matrix with 'current water status' for each of the specified layers in the soil. Units, fraction 0-1.}

  \item{psimMat}{Matrix with soil matric potential (kPa) for each layer of the soil.}

  \item{rdMat}{Matrix with root distribution by soil layer. Units, Mg/ha}

  \item{SCpools}{Vector of length 9 with soil carbon pools at the end of the simulation. The 9 carbon pools are described in the \code{centuryControl} above.}

  \item{SNpools}{Vector of length 9 with soil nitrogen pools at the end of the simulation. The 9 nitrogen pools are described in the \code{centuryControl} above.}		

  \item{LeafPsimVec}{Leaf water potential (kPa). It will be zero unless the simulation os water stress is \code{"lwp"}} 

}    

\examples{
\dontrun{
data(weather05)

res0 <- BioGro(weather05)

plot(res0)

## Looking at the soil model

res1 <- BioGro(weather05, soilControl = soilParms(soilLayers = 6))
plot(res1, plot.kind="SW") ## Without hydraulic distribution
res2 <- BioGro(weather05, soilControl = soilParms(soilLayers = 6, hydrDist=TRUE))
plot(res2, plot.kind="SW") ## With hydraulic distribution


## Example of user defined soil parameters.
## The effect of phi2 on yield and soil water content

ll.0 <- soilParms(FieldC=0.37,WiltP=0.2,phi2=1)
ll.1 <- soilParms(FieldC=0.37,WiltP=0.2,phi2=2)
ll.2 <- soilParms(FieldC=0.37,WiltP=0.2,phi2=3)
ll.3 <- soilParms(FieldC=0.37,WiltP=0.2,phi2=4)

ans.0 <- BioGro(weather05,soilControl=ll.0)
ans.1 <- BioGro(weather05,soilControl=ll.1)
ans.2 <- BioGro(weather05,soilControl=ll.2)
ans.3 <-BioGro(weather05,soilControl=ll.3)

xyplot(ans.0$SoilWatCont +
       ans.1$SoilWatCont +
       ans.2$SoilWatCont +
       ans.3$SoilWatCont ~ ans.0$DayofYear,
       type="l",		 
       ylab="Soil water Content (fraction)",
       xlab="DOY")

## Compare LAI

xyplot(ans.0$LAI +
       ans.1$LAI +
       ans.2$LAI +
       ans.3$LAI ~ ans.0$DayofYear,
       type="l",		 
       ylab="Leaf Area Index",
       xlab="DOY")



}
}
\keyword{models}