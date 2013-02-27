\name{SoilEvapo}
\alias{SoilEvapo}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{ Soil Evaporation }
\description{
  Calculates soil evaporation
}
\usage{
SoilEvapo(LAI, k, AirTemp, IRad, awc, FieldC, WiltP, winds, RelH) 
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{LAI}{ Leaf Area Index.}
  \item{k}{ ~~Describe \code{k} here~~ }
  \item{AirTemp}{ Air temperature. }
  \item{IRad}{ Incident radiation. }
  \item{awc}{ Available water content.}
  \item{FieldC}{ Field capacity.}
  \item{WiltP}{ Wilting point. }
  \item{winds}{ Wind speed. }
  \item{RelH}{ Relative humidty. }
}
\details{
 The style of the code is \code{C} like because this is a prototype for
 the underlying \code{C} (like so many other functions in this
 package). I leave it here for future development.
}
\value{
 Returns a single value of soil Evaporation in Mg H20 per hectare.
}
%\references{ ~put references to the literature/web site here ~ }
\author{ Fernando Miguez }
% \note{ ~~further notes~~ 

%  ~Make other sections like Warning with \section{Warning }{....} ~
% }
\seealso{ Source code :) }
\examples{

SoilEvapo(LAI=3,k=0.68,AirTemp=20,IRad=1000,awc=0.3,FieldC=0.4,WiltP=0.2,winds=3,RelH=0.8)
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ models }

