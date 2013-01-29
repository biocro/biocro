\name{Century}
\alias{Century}
\alias{CenturyC}

%- Also NEED an '\alias' for EACH other topic documented here.
\title{ This function implements the Century model from Parton. }
\description{
Calculates flows of soil organic carbon and nitrogen based on the
Century model. There are two versions one written in R (Century) and one
in C (CenturyC) they should give the same result. The C version only
runs at weekly timesteps.
}
\usage{
Century(LeafL, StemL, RootL, RhizL, smoist, stemp, precip, leachWater,
centuryControl = list(), verbose = FALSE)

CenturyC(LeafL, StemL, RootL, RhizL, smoist, stemp, precip, 
    leachWater, centuryControl = list(), soilType = 0) 
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{LeafL}{ Leaf litter.}
  \item{StemL}{ Stem litter. }
  \item{RootL}{ Root litter. }
  \item{RhizL}{ Rhizome litter. }
  \item{smoist}{ Soil moisture. }
  \item{stemp}{ Soil temperature. } 
  \item{precip}{ Precipitation. }
  \item{leachWater}{ Leached water. }
  \item{centuryControl}{ See \code{\link{centuryParms}}.}
  \item{verbose}{ Only used in the R version for debugging. }
  \item{soilType}{ See \code{\link{showSoilType}}.}
}
\details{
 Most of the details can be found in the papers by Parton et al. (1987, 1988, 1993)
}
\value{
  A list with,
  \item{SCs }{Soil carbon pools 1-9.}
  \item{SNs }{Soil nitrogen pools 1-9.}
  \item{MinN}{Mineralized nitrogen.}
  \item{Resp}{Soil respiration.}
}
\references{ ~put references to the literature/web site here ~ }
\author{ Fernando E. Miguez }
% \note{ ~~further notes~~ 

%  ~Make other sections like Warning with \section{Warning }{....} ~
% }
% \seealso{ ~~objects to See Also as \code{\link{help}}, ~~~ }
\examples{
% Add one or more standard keywords, see file 'KEYWORDS' in the
Century(0,0,0,0,0.3,5,2,2)$Resp
Century(0,0,0,0,0.3,5,2,2)$MinN

}
\keyword{ models }

