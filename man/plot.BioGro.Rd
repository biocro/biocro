\name{plot.BioGro}
\alias{plot.BioGro}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{ Plotting function for BioGro objects }
\description{
  By default it plots stem, leaf, root, rhizome and LAI for a
  \code{BioGro} object. Optionally, the observed data can be plotted.
  
}

\usage{
\method{plot}{BioGro}(x, obs = NULL, stem = TRUE, leaf = TRUE, root = TRUE, 
    rhizome = TRUE, LAI = TRUE, grain = TRUE, pch = 21, lty = 1,
    lwd =1, col = c("blue", "green", "red", "magenta", "black", "purple"),
    x1 = 0.1, y1 = 0.8, 
    plot.kind = c("DB", "SW"), \dots) 
}

%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{x}{ \code{\link{BioGro}} object.}
  \item{obs}{ optional observed data object (format following the
    \code{\link{OpBioGro}}
    function .  }
  \item{stem}{ whether to plot simulated stem (default = TRUE).  }
  \item{leaf}{ whether to plot simulated leaf (default = TRUE).  }
  \item{root}{ whether to plot simulated root (default = TRUE).  }
  \item{rhizome}{ whether to plot simulated rhizome (default = TRUE).  }
  \item{grain}{ whether to plot simulated grain (default = TRUE).  }
  \item{LAI}{ whether to plot simulated LAI (default = TRUE).  }
  \item{pch}{ point character.}
  \item{lty}{ line type. }
  \item{lwd}{line width.}
\item{col}{ Control of colors.}
\item{x1}{ position of the legend. x coordinate (0-1). }
\item{y1}{ position of the legend. y coordinate (0-1). }
\item{plot.kind}{ DB plots dry biomass and SW plots soil water. }
  \item{\dots}{ Optional arguments. }
}
\details{
  This function uses internally \code{\link[lattice]{xyplot}} 
 in the 'lattice' package.
}
\seealso{ \code{\link{BioGro}} \code{\link{OpBioGro}}}
\keyword{ hplot }