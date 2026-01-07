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
