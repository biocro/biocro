\docType{data}
\name{annualDB}
\alias{annualDB}
\alias{annualDB2}
\title{Miscanthus dry biomass data.}
\format{Data frame of dimensions 5 by 6.}

\source{
    Clive Beale and Stephen Long. 1997. Seasonal dynamics of nutrient
    accumulation and partitioning in the perennial C4 grasses Miscanthus x
    giganteus and Spartina cynosuroides. \emph{Biomass and Bioenergy} 12 (6): 419--428.
}

\description{
    The first column is the thermal time. The second, third, fourth, and fifth
    columns are miscanthus stem, leaf, root, and rhizome dry biomass in Mg
    ha\ifelse{html}{\out{<sup>-1</sup>}}{\eqn{^{-1}}{-1}} (root is missing). The sixth column is the leaf area index.
    The \code{annualDB.c} version is altered so that root biomass is not
    missing and LAI is smaller. The purpose of this last modification is for
    testing some functions.
}

\keyword{datasets}
