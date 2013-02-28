\name{RssBioGro}
\alias{RssBioGro}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{ Residual sum of squares for BioGro.}
\description{
Computes residual sum of squares for the \code{\link{BioGro}} function.
}
\usage{
RssBioGro(obs, sim)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{obs}{ Observed data.}
  \item{sim}{ Simulated data.}
}
% \details{
%  This function is very simple   ~~ If necessary, more details than the description above ~~
% }
\value{
  Atomic vector with the residual sum of squares.
}
%\references{ ~put references to the literature/web site here ~ } 
\author{ Fernando E. Miguez }
% \note{ ~~further notes~~ 

%  ~Make other sections like Warning with \section{Warning }{....} ~
% }
\seealso{ See Also \code{\link{BioGro}}.}
\examples{
## A simple example
data(annualDB)
data(EngWea94i)
res <- BioGro(EngWea94i)
RssBioGro(annualDB,res)
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ models }
