\name{plotAC}
\Rdversion{1.1}
\alias{plotAC}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
Plotting function for A/Ci curves 
}
\description{
A small helper function that can be used to easily plot multiple A/Ci curves
}
\usage{
plotAC(data, fittd, id.col = 1, trt.col = 2, ylab = "CO2 Uptake", xlab = "Ci", by = c("trt", "ID"), type = c("p", "smooth"))
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{data}{
    Input data in the format needed for the \code{\link{mOpc4photo}}.
}
  \item{fittd}{
Optional fitted values.
}
  \item{id.col}{
Specify which column has the ids. Default is col 1.
}
\item{trt.col}{
  Specify which column has the treatments. Default is col 2. If no
  treatment is specified then use 1.
}
  \item{ylab}{
Label for the y-axis.
}
\item{xlab}{
  Label for the x-axis.
}
\item{by}{
  Whether to plot by id or by treatment.
}
\item{type}{
  this argument is passed to the \code{\link{xyplot}}. It changes the
  plotting symbols behavior.
}
}
% \details{
% %%  ~~ If necessary, more details than the description above ~~
% }
\value{
  NULL
%%  ~Describe the value returned
%%  If it is a LIST, use
%%  \item{comp1 }{Description of 'comp1'}
%%  \item{comp2 }{Description of 'comp2'}
%% ...
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
See Also \code{\link{xyplot}}.
}
\examples{
data(aci)
plotAC(aci, trt.col=1)

}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ hplot }

