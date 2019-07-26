\name{valid_dbp}
\alias{valid_dbp}
\title{Validate dry biomass partitioning coefficients}
\usage{
valid_dbp(x, tol = 0.001)
}
\arguments{
    \item{x}{Vector of length 25 containing the dry biomass partitioning coefficients for the 6 phenological stages.}

    \item{tol}{Numerical tolerance passed to the \code{\link{all.equal}} function.}
}
\value{
    It will return the vector of coefficients unchanged if no errors
    are detected.
}
\description{
    It attempts to check the requirements of the dry biomass partitioning
    coefficients.
}
\seealso{
}
\author{
    Fernando E. Miguez
}
\keyword{utilities}
