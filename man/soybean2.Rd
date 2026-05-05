\name{soybean2}

\docType{data}

\alias{soybean2}

\title{Soybean-BioCro model definition}

\description{
  Alternate version of the \code{\link{soybean}} model, where the main
  difference is that \code{soybean2} uses a new set of soil water modules.
}

\usage{soybean2}

\format{
  A list of 5 named elements that are suitable for passing to
  \code{\link{run_biocro}}, as described in the help page for
  \code{\link{crop_model_definitions}}.
}

\source{
  \strong{parameters}

  \itemize{
    \item Basal crop coefficient: Values of \code{kcbmax}, \code{kcbmin}, and
          \code{skc} for maize and cotton are reported in Table 3 of DeJonge &
          Thorp (2017) [\doi{10.13031/trans.12321}]. The two crops have the same
          values of \code{kcbmax} and \code{kcbmin} (1.15 and 0) and nearly
          indentical values of \code{skc} (0.5 and 0.6, respectively),
          indicating that these parameters do not change much for different
          crops. Here we use 1.15, 0, and 0.55 for soybean, where the \code{skc}
          value is the mean of the maize and cotton values.
    \item Stomatal water stress parameters: Values of \code{StomataWS_gradient}
          and \code{StomataWS_intercept} were determined by matching predicted
          and values of stomatal conductance with observed ones as reported in
          Gray \emph{et al}. 2016. A script for determining the values is
          included with the BioCro package; its location can be found by typing
          \code{system.file('extdata', 'optimize_StomataWS_linear.R', package = 'BioCro')}
          in an R session.
    \item Regarding soil types in Champaign County: according to 
          \href{https://websoilsurvey.nrcs.usda.gov/app/}{the Web Soil Survey}, 
          soils in this region are predominantly silty clay loam, loam, and silt loam,
          with variation across soil depth. In BioCro, soil types 4--7 correspond to
          loamy soils. Although this classification does not provide the most accurate
          representation of soil hydraulic properties, it is reasonable to constrain 
          our parameterization within this range for consistency. In our current setup,
          we assign type 7 (silty clay loam) to layers 3--6 and type 6 (clay loam) to 
          layers 1--2. This configuration produces reasonably good agreement with 
          observed soil water dynamics at SoyFACE.
  }
}

\seealso{
  \itemize{
    \item \code{\link{run_biocro}}
    \item \code{\link{modules}}
    \item \code{\link{crop_model_definitions}}
    \item \code{\link{soybean_clock}}
  }
}

\keyword{datasets}

\concept{crop_models}
