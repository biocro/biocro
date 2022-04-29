\name{soybean_clock}

\docType{data}

\alias{soybean_clock}

\title{Soybean-BioCro circadian clock model definition}

\description{
  Initial values, parameters, direct modules, differential modules, and
  a differential equation solver that can be used to run soybean circadian clock
  simulations in Champaign, Illinois and other locations. Along with the soybean
  growth specifications (\code{\link{soybean}}), these values define the soybean
  growth model of Matthews \emph{et al.} (2022)
  [\doi{10.1093/insilicoplants/diab032}], which is commonly referred to as
  \emph{Soybean-BioCro}.

  To represent a soybean circadian clock in Champaign, Illinois, these values
  must be paired with the weather data from \code{\link{cmi_weather_data}}.
}

\usage{soybean_clock}

\format{
  A list of 5 named elements that are suitable for passing to
  \code{\link{run_biocro}}, as described in the help page for
  \code{\link{crop_model_definitions}}.
}

\source{
  This model is described in detail in Matthews \emph{et al.} (2022)
  [\doi{10.1093/insilicoplants/diab032}] and Lochocki & McGrath (2021)
  [\doi{10.1093/insilicoplants/diab016}].

  Here, we use initial phases for the dawn and dusk oscillators of \code{200.0}
  and \code{80.0} radians, respectively. These values are optimized for
  simulations beginning at midnight on January 1, and should require minimal
  time for transient signals to die down. These values were determined by
  running a simulation for one year starting on January 1, and recording the
  oscillator states at the end of December 31.
}

\seealso{
  \itemize{
    \item \code{\link{run_biocro}}
    \item \code{\link{modules}}
    \item \code{\link{crop_model_definitions}}
    \item \code{\link{soybean}}
  }
}

\keyword{datasets}

\concept{crop_models}
