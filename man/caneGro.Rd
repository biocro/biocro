\name{caneGro}
\alias{caneGro}
\title{Simulation of cane, Growth, LAI, Photosynthesis and phenology}
\usage{
caneGro(WetDat, day1 = NULL, dayn = NULL, timestep = 1, lat = 40,
  iRhizome = 7, irtl = 1e-04, canopyControl = list(),
  seneControl = list(), photoControl = list(), phenoControl = list(),
  soilControl = list(), nitroControl = list(), canephenoControl = list(),
  centuryControl = list(), managementControl = list(),
  frostControl = list())
}
\arguments{
  \item{WetDat}{weather data as produced by the
  \code{\link{weach}} function.}

  \item{plant.day}{Planting date (format 0-365)}

  \item{emerge.day}{Emergence date (format 0-365)}

  \item{harvest.day}{Harvest date (format 0-365)}

  \item{plant.density}{Planting density (plants per meter
  squared, default = 7)}

  \item{timestep}{Simulation timestep, the default of 1
  requires houlry weather data. A value of 3 would require
  weather data every 3 hours.  This number should be a
  divisor of 24.}

  \item{lat}{latitude, default 40.}

  \item{canopyControl}{List that controls aspects of the
  canopy simulation. It should be supplied through the
  \code{canopyParms} function.

  \code{Sp} (specific leaf area) here the units are ha
  \eqn{Mg^{-1}}.  If you have data in \eqn{m^2} of leaf per
  kg of dry matter (e.g. 15) then divide by 10 before
  inputting this coefficient.

  \code{SpD} decrease of specific leaf area. Empirical
  parameter. Default 0. example value (1.7e-3).

  \code{nlayers} (number of layers of the canopy) Maximum
  50. To increase the number of layers (more than 50) the
  \code{C} source code needs to be changed slightly.

  \code{kd} (extinction coefficient for diffuse light)
  between 0 and 1.

  \code{mResp} (maintenance respiration) a vector of length
  2 with the first component for leaf and stem and the
  second component for rhizome and root.}

  \item{caneSeneControl}{List that controls aspects of
  senescence simulation. It should be supplied through the
  \code{caneSeneParms} function.

  \code{senLeaf} Thermal time at which leaf senescence will
  start.

  \code{senStem} Thermal time at which stem senescence will
  start.

  \code{senRoot} Thermal time at which root senescence will
  start.}

  \item{photoControl}{List that controls aspects of
  photosynthesis simulation. It should be supplied through
  the \code{canePhotoParms} function.

  \code{vmax} Vmax passed to the \code{\link{c4photo}}
  function.

  \code{alpha} alpha parameter passed to the
  \code{\link{c4photo}} function.

  \code{kparm} kparm parameter passed to the
  \code{\link{c4photo}} function.

  \code{theta} theta parameter passed to the
  \code{\link{c4photo}} function.

  \code{beta} beta parameter passed to the
  \code{\link{c4photo}} function.

  \code{Rd} Rd parameter passed to the
  \code{\link{c4photo}} function.

  \code{UPPERTEMP} UPPERTEMP parameter passed to the
  \code{\link{c4photo}} function.

  \code{LOWERTEMP} LOWERTEMP parameter passed to the
  \code{\link{c4photo}} function.

  \code{Catm} Catm parameter passed to the
  \code{\link{c4photo}} function.

  \code{b0} b0 parameter passed to the
  \code{\link{c4photo}} function.

  \code{b1} b1 parameter passed to the
  \code{\link{c4photo}} function.}

  \item{canePhenoControl}{argument used to pass parameters
  related to phenology characteristics %% ~~Describe
  \code{canePhenoControl} here~~}

  \item{soilControl}{%% ~~Describe \code{soilControl}
  here~~}

  \item{nitroControl}{%% ~~Describe \code{nitroControl}
  here~~}

  \item{centuryControl}{%% ~~Describe \code{centuryControl}
  here~~}
}
\value{
It currently returns a list with the following components

\item{DayofYear}{Day of the year (0-365)}

\item{Hour}{Hour of the day (0-23)}

\item{TTTc}{Accumulated thermal time}

\item{PhenoStage}{Phenological stage of the crop}

\item{CanopyAssim}{Hourly canopy assimilation, (Mg
\eqn{ha^-1} ground \eqn{hr^-1}).}

\item{CanopyTrans}{Hourly canopy transpiration, (Mg
\eqn{ha^-1} ground \eqn{hr^-1}).}

\item{LAI}{Leaf Area Index}

%% ~Describe the value returned %% If it is a LIST, use %%
\item{comp1 }{Description of 'comp1'} %% \item{comp2
}{Description of 'comp2'} %% ...
}
\description{
It takes weather data as input (hourly timesteps) and
several parameters and it produces phenology,
photosynthesis, LAI, etc.
}
\details{
The phenology follows the 'Corn Growth and Development'
Iowa State Publication. %% ~~ If necessary, more details
than the description above ~~
}
\note{
%% ~~further notes~~
}
\examples{
data(weather05)
res <- caneGro(weather05, plant.day = 110, emerge.day = 120, harvest.day=300,
                  canePhenoControl = canePhenoParms(R6 = 2000))
}
\author{
Fernando E Miguez
}
\references{
%% ~put references to the literature/web site here ~
}
\seealso{
\code{\link{BioGro}} %% ~~objects to See Also as
\code{\link{help}}, ~~~
}
\keyword{models}

