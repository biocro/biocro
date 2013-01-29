\name{valid_dbp}
\Rdversion{1.1}
\alias{valid_dbp}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
Validate dry biomass partitioning coefficients
}
\description{
  It attempts to check the requirements of the dry biomass partitioning coefficients.
}
\usage{
valid_dbp(x, tol = 0.001)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{x}{
Vector of length equal to 25 containing the dry biomass partitioning
coefficients for the 6 phenological stages as for example produced by \code{\link{phenoParms}}.
}
\item{tol}{
  Numerical tolerance passed to the \code{\link{all.equal}} function.
}
}
% \details{
% %%  ~~ If necessary, more details than the description above ~~
% }
\value{
It will return the vector of coefficients unchanged if no errors are detected.
}
% \references{
% %% ~put references to the literature/web site here ~
% }
\author{
Fernando E. Miguez
}
% \note{
% %%  ~~further notes~~
% }

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
\code{\link{BioGro}}
}
\examples{
xx <- as.vector(unlist(phenoParms())[7:31])
valid_dbp(xx)
}
\keyword{ utilities }

