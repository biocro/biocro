\name{plot.MaizeGro}
\alias{plot.MaizeGro}
\title{Plotting function for MaizeGro objects}
\usage{
\method{plot}{MaizeGro}(x, obs = NULL, stem = TRUE, leaf = TRUE,
  root = TRUE, LAI = TRUE, grain = TRUE, xlab = NULL, ylab = NULL,
  pch = 21, lty = 1, lwd = 1, col = c("blue", "green", "red", "black",
  "purple"), x1 = 0.1, y1 = 0.8, plot.kind = c("DB", "SW", "LAI",
  "pheno"), ...)
}
\arguments{
  \item{x}{\code{\link{MaizeGro}} object.}

  \item{obs}{optional observed data object (format
  following the \code{\link{OpMaizeGro}} function .}

  \item{stem}{whether to plot simulated stem (default =
  TRUE).}

  \item{leaf}{whether to plot simulated leaf (default =
  TRUE).}

  \item{root}{whether to plot simulated root (default =
  TRUE).}

  \item{rhizome}{whether to plot simulated rhizome (default
  = TRUE).}

  \item{grain}{whether to plot simulated grain (default =
  TRUE).}

  \item{LAI}{whether to plot simulated LAI (default =
  TRUE).}

  \item{pch}{point character.}

  \item{lty}{line type.}

  \item{lwd}{line width.}

  \item{col}{Control of colors.}

  \item{x1}{position of the legend. x coordinate (0-1).}

  \item{y1}{position of the legend. y coordinate (0-1).}

  \item{plot.kind}{DB plots dry biomass and SW plots soil
  water.}

  \item{\dots}{Optional arguments.}
}
\description{
By default it plots stem, leaf, root, rhizome and LAI for a
\code{MaizeGro} object. Optionally, the observed data can
be plotted.
}
\details{
This function uses internally \code{\link[lattice]{xyplot}}
in the 'lattice' package.
}
\seealso{
\code{\link{MaizeGro}} \code{\link{OpMaizeGro}}
}
\keyword{hplot}

