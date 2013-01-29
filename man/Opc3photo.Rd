\name{Opc3photo}
\alias{Opc3photo}
\alias{plot.Opc3photo}
\alias{print.Opc3photo}
\alias{predict.Opc3photo}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{ Optimize parameters of the C3 photosynthesis model. }
\description{
  Applies the \code{optim} function to C3 photosynthesis.
}
\usage{
Opc3photo(data, ivcmax = 100, ijmax = 180, iRd = 1.1, Catm = 380, 
    O2 = 210, ib0 = 0.08, ib1 = 9.58, itheta = 0.7, op.level = 1, 
    op.method = c("optim", "nlminb"), response = c("Assim", "StomCond"), 
    level = 0.95, hessian = TRUE, curve.kind = c("Ci", "Q"), 
    op.ci = FALSE, ...)
}
%- maybe also 'usage' for other objects documented here.
\arguments{

For Opc3photo:
\item{data}{
    should be a \code{data.frame} or \code{matrix} with x columns

    col 1: measured assimilation (CO2 uptake)
    col 2: Incomming PAR (photosynthetic active radiation)
    col 3: Leaf temperature
    col 4: Relative humidity
    col 5: Intercellular CO2 (for A/Ci curves)
    col 6: Reference CO2 level

}
  \item{ivcmax}{ Initial value for \code{vcmax}.}
  \item{ijmax}{ Initial value for \code{jmax}.}
  \item{iRd}{ Initial value for \code{Rd}.}
  \item{Catm}{ Reference CO2.}
  \item{O2}{ Reference level of O2.}
  \item{ib0}{ Initial value for the intercept to the Ball-Berry model.}
  \item{ib1}{ Initial value for the slope to the Ball-Berry model.}
  \item{itheta}{ Initial value for \code{theta}.}
  \item{op.level}{ Level 1 will optimize \code{Vcmax} and \code{Jmax}
    and level 2 will optimize \code{Vcmax}, \code{Jmax} and \code{Rd}. }
  \item{op.method}{ optimization method. At the moment only optim is implemented.}
  \item{response}{ \code{"Assim"} for assimilation and \code{"StomCond"}
  for stomatal conductance.}
  \item{level}{ Confidence interval level.}
  \item{hessian}{Whether the hessian should be computed}
  \item{curve.kind}{ Whether an A/Ci curve is being optimized or an A/Q
    curve.}
  \item{op.ci}{whether to optimize intercellular CO2.}
  \item{\dots}{ Additioanl arguments to be passed to \code{\link{optim}}.}

}
%\details{
%  ~~ If necessary, more details than the description above ~~
%}
\value{

  An object of class \code{Opc3photo}.
  
  The following components can be extracted:
  \item{bestVmax }{optimized \code{vmax}.}
  \item{bestJmax }{optimized \code{jmax}.}
  \item{ReSumS}{Residual Sum of Squares.}
  \item{Convergence}{Convergence status.}
  \item{VarCov}{Variance-covariance matrix.}
  \item{df}{degress of freedom.}
  \item{ciVmax}{Confidence interval for \code{vmax}.}
  \item{ciJmax}{Confidence interval for \code{jmax}.}
  \item{corVJ}{correlation between \code{vmax} and \code{jmax}.}
  \item{level}{Confidence interval level.}
  \item{data}{Original data.}
  \item{xparms}{Additional parameters.}
  \item{curve.kind}{ A/Ci or A/Q curve.}
  \item{op.level}{ Level 1 means \code{vcmax} and \code{jmax} were optimized
    and level 2 \code{vcmax}, \code{jmax} and \code{Rd}. }
  \item{response}{\code{"Assim"} or \code{"StomCond"}.}
}
%\references{ ~put references to the literature/web site here ~ }
\author{ Fernando E. Miguez }
 \note{ ~~further notes~~ 
Additional notes about the assumptions.

 }
\seealso{ See Also \code{\link{mOpc3photo}} }
\examples{
## Load fabricated data
data(simA100)
## Look at it
head(simA100)

op <- Opc3photo(simA100[,1:5],Catm=simA100[,9], op.level = 2)

## If faced with a difficult problem
## This can give starting values
op100 <- Opc3photo(simA100[,1:5],Catm=simA100[,9],
                   op.level = 2, method="SANN",
                   hessian=FALSE)

op100 <- Opc3photo(simA100[,1:5],Catm = simA100[,9],
                   op.level = 2, 
                   ivcmax = op100$bestVmax,
                   ijmax = op100$bestJmax,
                   iRd = op100$bestRd)
op100
}
\keyword{ optimize }

