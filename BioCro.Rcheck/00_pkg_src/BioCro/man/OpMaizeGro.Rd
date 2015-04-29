\name{OpMaizeGro}
\alias{OpMaizeGro}
\title{Optimization of dry biomass partitioning coefficients.}
\usage{
OpMaizeGro(phen = 1, iCoef = NULL, cTT, WetDat, data, plant.day = NULL,
  emerge.day = NULL, harvest.day = NULL, plant.density = 7,
  timestep = 1, lat = 40, canopyControl = list(),
  MaizeSeneControl = list(), photoControl = list(),
  MaizePhenoControl = list(), MaizeCAllocControl = list(),
  laiControl = list(), soilControl = list(), MaizeNitroControl = list(),
  centuryControl = list(), op.method = c("optim", "nlminb"),
  verbose = FALSE, ...)
}
\arguments{
  \item{phen}{integer taking values 1 through 6 which
  indicate the phenological stage being optimized. If all
  of the phenological stages need to be optimized then use
  zero (0).}

  \item{iCoef}{initial vector of size 24 for the dry
  biomass partitioning coefficients.}

  \item{cTT}{}

  \item{WetDat}{Weather data.}

  \item{data}{observed data.}

  \item{plant.day}{}

  \item{emerge.day}{}

  \item{harvest.day}{}

  \item{plant.density}{}

  \item{canopyControl}{see \code{\link{MaizeGro}}}

  \item{seneControl}{see \code{\link{MaizeGro}}}

  \item{photoControl}{see \code{\link{MaizeGro}}}

  \item{phenoControl}{see \code{\link{MaizeGro}}}

  \item{soilControl}{see \code{\link{MaizeGro}}}

  \item{nitroControl}{see \code{\link{MaizeGro}}}

  \item{centuryControl}{see \code{\link{MaizeGro}}}

  \item{op.method}{Optimization method. Whether to use
  optim or nlminb}

  \item{verbose}{Displays additional information,
  originally used for debugging.}

  \item{\dots}{additional arguments passed to
  \code{\link{optim}} or \code{\link{constrOptim}}.}
}
\description{
Optimizes dry biomass partitioning coefficients using
constrained optimization (see below).
}
\details{
The optimization is done over the \code{\link{MaizeGro}}
function. The \code{OpMaizeGro} function is a wrapper for
\code{optim} and the \code{constrOpBioGro} is a wrapper for
\code{constrOptim}.
}

