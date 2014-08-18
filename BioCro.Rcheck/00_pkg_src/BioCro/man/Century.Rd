\name{Century}
\alias{Century}
\alias{CenturyC}
\title{This function implements the Century model from Parton.}
\usage{
Century(LeafL, StemL, RootL, RhizL, smoist, stemp, precip, leachWater,
  centuryControl = list(), verbose = FALSE)
}
\arguments{
  \item{LeafL}{Leaf litter.}

  \item{StemL}{Stem litter.}

  \item{RootL}{Root litter.}

  \item{RhizL}{Rhizome litter.}

  \item{smoist}{Soil moisture.}

  \item{stemp}{Soil temperature.}

  \item{precip}{Precipitation.}

  \item{leachWater}{Leached water.}

  \item{centuryControl}{See \code{\link{centuryParms}}.}

  \item{verbose}{Only used in the R version for debugging.}

  \item{soilType}{See \code{\link{showSoilType}}.}
}
\value{
A list with,
}
\description{
Calculates flows of soil organic carbon and nitrogen based
on the Century model. There are two versions one written in
R (Century) and one in C (CenturyC) they should give the
same result. The C version only runs at weekly timesteps.
}
\details{
Most of the details can be found in the papers by Parton et
al. (1987, 1988, 1993)
}
\examples{
Century(0,0,0,0,0.3,5,2,2)$Resp
Century(0,0,0,0,0.3,5,2,2)$MinN
}
\author{
Fernando E. Miguez
}
\references{
~put references to the literature/web site here ~
}
\keyword{models}

