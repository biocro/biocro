\docType{data}
\name{aq}
\alias{aq}
\title{A/Q curves}
\format{A data frame with 64 observations on the following 6 variables.
    \describe{
        \item{list('ID')}{A numeric vector.}
        \item{list('trt')}{A factor with levels \code{mxg} \code{swg}.}
        \item{list('A')}{A numeric vector.  Assimilation.}
        \item{list('PARi')}{A numeric vector. Incident photosynthetic active radiation.}
        \item{list('Tleaf')}{A numeric vector. Temperature of the leaf.}
        \item{list('RH_S')}{A numeric vector. Relative humidity (fraction).}
    }
}
\source{
    Data based on measurements made by Dandan Wang.  ~~ reference to
    a publication or URL from which the data were obtained ~~
}
\description{
    Example of A/Q curves.
}
\details{
    \code{swg} stands for switchgrass (Panicum virgatum). \code{mxg} stands for
    miscanthus (Miscanthus x giganteus).
}
\examples{

data(aq)
lattice::xyplot(A ~ PARi | trt, data=aq, ylab='CO2 uptake', xlab='Quantum flux')

}
\references{
    Dandan Wang
}
\keyword{datasets}
