\name{plotAC}
\alias{plotAC}
\title{plot A/Ci curve}
\usage{
plotAC(data, fittd, id.col = 1, trt.col = 2, ylab = "CO2 Uptake",
  xlab = "Ci", by = c("trt", "ID"), type = c("p", "smooth"))
}
\arguments{
  \item{data}{Input data in the format needed for the
  \code{\link{mOpc4photo}}; assumed to have the following
  structure col 1: trt col 2 (optional): other treatment
  factor col 2: Assimilation col 3: Quantum flux col 4:
  Temperature col 5: Relative humidity col 6: Intercellular
  CO2 col 7: Reference CO2}

  \item{fittd}{Optional fitted values.}

  \item{id.col}{Specify which column has the ids. Default
  is col 1.}

  \item{trt.col}{Specify which column has the treatments.
  Default is col 2. If no treatment is specified then use
  1.}

  \item{ylab}{Label for the y-axis.}

  \item{xlab}{Label for the x-axis.}

  \item{by}{Whether to plot by id or by treatment.}

  \item{type}{this argument is passed to the
  \code{\link{xyplot}}. It changes the plotting symbols
  behavior.}
}
\value{
NULL, creates plot
}
\description{
Function to plot A/Ci curves
}
\details{
A small helper function that can be used to easily plot
multiple A/Ci curves
}
\examples{
data(aci)
plotAC(aci, trt.col=1)
}
\author{
Fernando E. Miguez
}
\seealso{
See Also \code{\link{xyplot}}.
}
\keyword{hplot}

