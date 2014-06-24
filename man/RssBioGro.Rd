\name{RssBioGro}
\alias{RssBioGro}
\title{Residual sum of squares for BioGro.}
\usage{
RssBioGro(obs, sim)
}
\arguments{
  \item{obs}{Observed data.}

  \item{sim}{Simulated data.}
}
\value{
Atomic vector with the residual sum of squares.
}
\description{
Computes residual sum of squares for the
\code{\link{BioGro}} function.
}
\examples{
## A simple example
data(annualDB)
data(EngWea94i)
res <- BioGro(EngWea94i)
RssBioGro(annualDB,res)
}
\author{
Fernando E. Miguez
}
\seealso{
See Also \code{\link{BioGro}}.
}
\keyword{models}

