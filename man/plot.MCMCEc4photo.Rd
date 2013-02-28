\name{plot.MCMCEc4photo}
\alias{plot.MCMCEc4photo}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{ Plottin function for MCMCEc4photo objects }
\description{
  By default it prints the trace of the four parameters being estimated
  by the \code{\link{MCMCEc4photo}} function. As an option the density
  can be plotted.
}
\usage{
\method{plot}{MCMCEc4photo}(x, x2 = NULL, x3 = NULL, type = c("trace", "density"), \dots)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{x}{ \code{\link{MCMCEc4photo}} object.}
  \item{x2}{ optional additional \code{link{MCMCEc4photo}} object.  }
  \item{x3}{ optional additional \code{link{MCMCEc4photo}} object.  }
  \item{type}{ "trace" plots the iteration history and "density"
    plots the kernel density. }
  \item{\dots}{ Optional arguments. }
}
\details{
  This function uses internally \code{\link[lattice]{xyplot}} and
  \code{\link[lattice]{densityplot}} both in the 'lattice' package.
}
\seealso{ \code{\link{MCMCEc4photo}} }
\keyword{ hplot }