\name{idbp}
\alias{idbp}
\title{Initial Dry Biomass Partitioning Coefficients}
\usage{
    idbp(data, TPcoefs)
}
\arguments{
    \item{data}{Should have at least five columns with: ThermalT, Stem, Leaf, Root, Rhizome and Grain.}

    \item{phenoControl}{List that supplies mainly in this case the thrmal time periods that delimit the phenological stages.}
}
\value{
    It returns a vector of length 25 suitable for \code{BioGro},
    \code{OpBioGro}, \code{constrOpBioGro}, or \code{MCMCBioGro}.
}
\description{
    Attempts to guess good initial values for dry biomass coefficients
    that can be passed to \code{BioGro}, \code{OpBioGro}, \code{constrOpBioGro},
    or \code{MCMCBioGro}.  It is very fragile.
}
\details{
    This function will not accept missing values. It can be quite fragile and
    it is rather inflexible in what it expects in terms of data.
}
\note{
    It is highly recommended that the results of this function be tested
    with \code{\link{valid_dbp}}.
}
\examples{

}
\seealso{
    \code{\link{valid_dbp}}
}
\author{
    Fernando E. Miguez
}
\keyword{utilities}
