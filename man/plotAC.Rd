\name{plotAC}
\alias{plotAC}
\title{Plotting function for A/Ci curves}
\usage{
  plotAC(data, fittd, id.col = 1, trt.col = 2,
    ylab = "CO2 Uptake", xlab = "Ci", by = c("trt", "ID"),
    type = c("p", "smooth"))
}
\arguments{
  \item{data}{Input data in the format needed for the
  \code{\link{mOpc4photo}}.}

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
  NULL %% ~Describe the value returned %% If it is a LIST,
  use %% \item{comp1 }{Description of 'comp1'} %%
  \item{comp2 }{Description of 'comp2'} %% ...
}
\description{
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

