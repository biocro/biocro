\name{aci}
\Rdversion{1.1}
\alias{aci}
\docType{data}
\title{
A/Ci curves
}
\description{
Four A/Ci (assimilation vs. intercellular CO2) curves.
}
\usage{data(aci)}
\format{
  A data frame with 32 observations on the following 7 variables.
  \describe{
    \item{\code{ID}}{Identification for each curve. a numeric vector}
    \item{\code{Photo}}{Assimilation. a numeric vector}
    \item{\code{PARi}}{Incident Photosynthetic Active Radiation. a numeric vector}
    \item{\code{Tleaf}}{Temperature of the leaf. a numeric vector}
    \item{\code{RH_S}}{Realtive humidity. a numeric vector}
    \item{\code{Ci}}{Intercellular CO2. a numeric vector}
    \item{\code{CO2_R}}{Reference CO2. a numeric vector}
  }
}
\details{
  Measurements taken on Miscanthus x giganteus.
}
\source{
  Measurements taken by Dandan Wang.
}
\references{
  Dandan Wang
%%  ~~ possibly secondary sources and usages ~~
}
\examples{
data(aci)
plotAC(aci)
}
\keyword{datasets}
