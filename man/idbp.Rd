\name{idbp} 
\Rdversion{1.1} 
\alias{idbp} 
  
\title{ Initial Dry Biomass Partitioning Coefficients }

\description{ Atempts to guess good initial vales for dry biomass coefficients
that can be passed to \code{BioGro}, \code{OpBioGro}, \code{constrOpBioGro}, or \code{MCMCBioGro}. 
It is very fragile.}

\usage{
idbp(data, phenoControl = list())
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{data}{
Should have at least five columns with: ThermalT, Stem, Leaf, Root, Rhizome and Grain.
}
  \item{phenoControl}{
list that supplies mainly in this case the thrmal time periods that delimit the phenological stages,

} } 

\details{ 
This function will not accept missing values. It can be quite
fragile and it is rather inflexible in what it expects in terms of data.  
}

\value{ 
It returns a vector of length 25 suitable for \code{BioGro}, \code{OpBioGro}, \code{constrOpBioGro}, or \code{MCMCBioGro}.

}

\author{ Fernando E. Miguez} 

\note{ It is highly recommended that the results of this function are tested with \code{\link{valid_dbp}}. }


\seealso{
 \code{\link{valid_dbp}}
}
\examples{
## See ?OpBioGro
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ utilities }
