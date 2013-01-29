\docType{data}
\name{aci}
\alias{aci}
\title{A/Ci curves}
\format{A data frame with 32 observations on the following 7 variables.
\describe{ \item{list('ID')}{Identification for each curve. a numeric
vector} \item{list('Photo')}{Assimilation. a numeric vector}
\item{list('PARi')}{Incident Photosynthetic Active Radiation. a numeric
vector} \item{list('Tleaf')}{Temperature of the leaf. a numeric vector}
\item{list('RH_S')}{Realtive humidity. a numeric vector}
\item{list('Ci')}{Intercellular CO2. a numeric vector}
\item{list('CO2_R')}{Reference CO2. a numeric vector} }}
\source{
  Measurements taken by Dandan Wang.
}
\description{
  Four A/Ci (assimilation vs. intercellular CO2) curves.
}
\details{
  Measurements taken on Miscanthus x giganteus.
}
\examples{
data(aci)
plotAC(aci)
}
\references{
  Dandan Wang
}
\keyword{datasets}

