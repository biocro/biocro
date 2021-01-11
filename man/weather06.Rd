\docType{data}
\name{weather06}
\alias{weather06}
\title{Weather data from 2006}
\format{A data frame with 8760 observations on the following 8 variables.
  \tabular{rll}{
    [,1] \tab year      \tab numeric \cr
    [,2] \tab doy       \tab numeric \cr
    [,3] \tab hour      \tab numeric \cr
    [,4] \tab solar     \tab numeric \cr
    [,5] \tab temp      \tab numeric \cr
    [,6] \tab rh        \tab numeric \cr
    [,7] \tab windspeed \tab numeric \cr
    [,8] \tab precip    \tab numeric
  }
}
\source{
}
\description{
    Weather data as produced by the \code{\link{weach}} function.  These are for 2006.
}
\examples{

data(weather06)
## maybe str(weather06) ; plot(weather06) ...

}
\keyword{datasets}
