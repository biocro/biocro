\docType{data}
\name{aq}
\alias{aq}
\title{A/Q curves}
\format{A data frame with 64 observations on the following 6 variables.
\describe{ \item{list('ID')}{a numeric vector} \item{list('trt')}{a factor
with levels \code{mxg} \code{swg}} \item{list('A')}{a numeric vector.
Assimilation} \item{list('PARi')}{a numeric vector. Photosynthetic Active
Radiation (incident).} \item{list('Tleaf')}{a numeric vector. Temperature
of the leaf.} \item{list('RH_S')}{a numeric vector. Relative humidity
(fraction).} }}
\source{
Data based on measurements made by Dandan Wang. %% ~~
reference to a publication or URL from which the data were
obtained ~~
}
\description{
Example of A/Q curves which serves as a template for using
the \code{\link{Opc4photo}} and \code{\link{mOpc4photo}}
functions.
}
\details{
\code{swg} stand for switchgrass (Panicum virgatum)
\code{mxg} stands for miscanthus (Miscanthus x gignateus)

%% ~~ If necessary, more details than the __description__
above ~~
}
\examples{
data(aq)
plotAQ(aq)
}
\references{
Dandan Wang %% ~~ possibly secondary sources and usages ~~
}
\keyword{datasets}

