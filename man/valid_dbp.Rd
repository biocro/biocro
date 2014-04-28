\name{valid_dbp}
\alias{valid_dbp}
\title{Validate dry biomass partitioning coefficients}
\usage{
valid_dbp(x, tol = 0.001)
}
\arguments{
  \item{x}{Vector of length equal to 25 containing the dry
  biomass partitioning coefficients for the 6 phenological
  stages as for example produced by
  \code{\link{phenoParms}}.}

  \item{tol}{Numerical tolerance passed to the
  \code{\link{all.equal}} function.}
}
\value{
It will return the vector of coefficients unchanged if no
errors are detected.
}
\description{
It attempts to check the requirements of the dry biomass
partitioning coefficients.
}
\examples{
xx <- as.vector(unlist(phenoParms())[7:31])
valid_dbp(xx)
}
\author{
Fernando E. Miguez
}
\seealso{
\code{\link{BioGro}}
}
\keyword{utilities}

