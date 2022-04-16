\name{sorghum}

\docType{data}

\alias{sorghum}

\title{Sorghum model definition}

\description{
  Initial values, parameters, direct modules, differential modules, and
  a differential equation solver that can be used to run sorghum growth
  simulations in Champaign, Illinois and other locations. Note that this model
  has not been published.

  To represent sorghum growth in Champaign, IL, these values must be paired with
  the clay loam soil parameters (see \code{\link{soil_parameters}}) and the
  Champaign weather data (\code{\link{cmi_weather_data}}).

  Some specifications, such as the values of photosynthetic parameters, would
  remain the same in any location; others, such as the latitude or longitude,
  would need to change when simulating crop growth in different locations. Care
  must be taken to understand each input quantity before attempting to run
  simulations in other places or for other cultivars.
}

\usage{sorghum}

\format{
  A list of 5 named elements that are suitable for passing to
  \code{\link{run_biocro}}, as described in the help page for
  \code{\link{crop_model_definitions}}.
}

\seealso{
  \itemize{
    \item \code{\link{run_biocro}}
    \item \code{\link{modules}}
    \item \code{\link{crop_model_definitions}}
  }
}

\keyword{datasets}

\concept{crop_models}
