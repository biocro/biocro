\name{plot.MCMCBioGro}
\alias{plot.MCMCBioGro}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{ Plotting function fo the MCMCBioGro class}
\description{
  Powerful plotting function to make a variety of plots regarding the
  \code{MCMCBioGro} class (output). It plots the residual sum of square
  progression, observed vs. fitted, residuals vs. fitted, trace of the
  coefficients and density.
}
\usage{
\method{plot}{MCMCBioGro}(x, x2 = NULL, x3 = NULL, plot.kind = c("rss", "OF", 
    "RF", "OFT", "trace", "density"), type = c("l", "p"), coef = 1, 
    cols = c("blue", "green", "red", "magenta", "black", "purple"), ...) 
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{x}{ Object of class \code{MCMCBioGro} }
  \item{x2}{ Optional object of class \code{MCMCBioGro} }
  \item{x3}{ Optional object of class \code{MCMCBioGro}}
  \item{plot.kind}{ Kind of plot. See details. }
  \item{type}{ Point of line as in \code{xyplot} }
  \item{coef}{ Whether to plot dry biomass partitioning coeficients (2)
    or Vmax and alpha (1).  }
  \item{cols}{ Colors. Modify if they don't suit you.}
  \item{\dots}{ Additional arguments passed to the underlying
    \code{xyplot} function. Some can be really useful. See details. }
}
\details{
  Kind of plots that can be produced

  rss: Residual Sum of Squares progression.
  OF: Observed vs. Fitted
  RF: Residual vs. Fitted
  OFT: Observed and Fitted with thermal time as the x-axis.
  trace: trace for the parameters
  density: density for the parameters

  % Important: if coef=2 is used (default) then the dry biomass
  % partitioning coefficients parameters will be used. If coef=1 then Vmax
  % and alpha will be plotted instead. At this point it is highly
  % recommended that Vmax and alpha are not optimized at this stage so
  % they shouldn't be plotted either (it will be a straight line if not
  % optimized).

  To choosing a subset of the 24 dry biomass partitioning coefficients
  use the subset option as you would in the xyplot using nams as the
  name. For example,
  \code{plot(x,plot.kind="trace",subset=nams=="kLeaf_1")} will select it
  for the leaf at the first phenological stage.
}
\value{
  A \code{lattice} plot.
}
%\references{ ~put references to the literature/web site here ~ }

\seealso{ \code{\link{MCMCBioGro}} }
\examples{
## See the MCMCBioGro function
}
\keyword{ hplot }

