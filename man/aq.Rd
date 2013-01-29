\name{aq}
\Rdversion{1.1}
\alias{aq}
\docType{data}
\title{
A/Q curves
}
\description{
Example of A/Q curves which serves as a template for using the
\code{\link{Opc4photo}} and \code{\link{mOpc4photo}} functions. 
}
\usage{data(aq)}
\format{
  A data frame with 64 observations on the following 6 variables.
  \describe{
    \item{\code{ID}}{a numeric vector}
    \item{\code{trt}}{a factor with levels \code{mxg} \code{swg}}
    \item{\code{A}}{a numeric vector. Assimilation}
    \item{\code{PARi}}{a numeric vector. Photosynthetic Active Radiation
    (incident).}
    \item{\code{Tleaf}}{a numeric vector. Temperature of the leaf.}
    \item{\code{RH_S}}{a numeric vector. Relative humidity (fraction).}
  }
}
\details{

  \code{swg} stand for switchgrass (Panicum virgatum)
  \code{mxg} stands for miscanthus (Miscanthus x gignateus)
  
%%  ~~ If necessary, more details than the __description__ above ~~
}
\source{
  Data based on measurements made by Dandan Wang.
%%  ~~ reference to a publication or URL from which the data were obtained ~~
}
\references{
  Dandan Wang
%%  ~~ possibly secondary sources and usages ~~
}
\examples{
data(aq)
plotAQ(aq)
}
\keyword{datasets}
