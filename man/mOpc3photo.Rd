\name{mOpc3photo}
\alias{mOpc3photo}
\title{Multiple optimization of assimilation (or stomatal conductance) curves.}
\usage{
mOpc3photo(data, ID = NULL, iVcmax = 100, iJmax = 180, iRd = 1.1,
  op.level = 1, curve.kind = c("Ci", "Q"), verbose = FALSE, ...)
}
\arguments{
  \item{data}{should be a \code{data.frame} or
  \code{matrix} with x columns

  col 1: should be an ID for the different runs col 2:
  measured assimilation (CO2 uptake) col 3: Incomming PAR
  (photosynthetic active radiation) col 4: Leaf temperature
  col 5: Relative humidity col 6: Intercellular CO2 (for
  A/Ci curves) col 7: Reference CO2 level}

  \item{ID}{optional argument to include ids. should be of
  length equal to the number of runs.}

  \item{iVcmax}{Single value or vector of length equal to
  number of runs to supply starting values for the
  optimization of \code{vcmax}.}

  \item{iJmax}{Single value or vector of length equal to
  number of runs to supply starting values for the
  optimization of \code{jmax}.}

  \item{iRd}{Single value or vector of length equal to
  number of runs to supply starting values for the
  optimization of \code{Rd}.}

  \item{op.level}{Level 1 will optimize \code{Vcmax} and
  \code{Jmax} and level 2 will optimize \code{Vcmax},
  \code{Jmax} and \code{Rd}.}

  \item{curve.kind}{Whether an A/Ci curve is being
  optimized or an A/Q curve.}

  \item{verbose}{Whether to print information about
  progress.}

  \item{\dots}{Additional arguments to be passed to
  \code{\link{Opc3photo}}}
}
\value{
an object of class 'mOpc3photo'

if op.level equals 1 best Vcmax, Jmax and convergence

if op.level equals 2 best Vcmax, Jmax, Rd and convergence
%% ~Describe the value returned %% If it is a LIST, use %%
\item{comp1 }{Description of 'comp1'} %% \item{comp2
}{Description of 'comp2'} %% ...
}
\description{
It is a wrapper for Opc3photo which allows for optimization
of multiple runs of curves (A/Q or A/Ci).
}
\details{
Include more details about the data.
}
\examples{
data(simAssim)
simAssim <- cbind(simAssim[,1:6],Catm=simAssim[,10])
simAssim <- simAssim[simAssim[,1] < 11,]

plotAC(simAssim, trt.col=1)

op.all <- mOpc3photo(simAssim, op.level=1,
                      verbose=TRUE)

plot(op.all)
plot(op.all, parm='jmax')
}
\author{
Fernando E. Miguez
}
\seealso{
See also \code{\link{Opc3photo}} %% ~~objects to See Also
as \code{\link{help}}, ~~~
}
\keyword{optimize}

