\name{miscanthus_x_giganteus}

\docType{data}

\alias{miscanthus_x_giganteus}

\title{Miscanthus model definition}

\description{
  Initial values, parameters, direct modules, differential modules, and
  a differential equation solver that can be used to run
  \emph{Miscanthus x giganteus} growth simulations in Champaign, Illinois and
  other locations.

  To represent miscanthus growth in Champaign, IL, these values must be paired
  with the clay loam soil parameters (see \code{\link{soil_parameters}}) and the
  Champaign weather data (\code{\link{cmi_weather_data}}).

  Some specifications, such as the values of photosynthetic parameters, would
  remain the same in any location; others, such as the latitude or longitude,
  would need to change when simulating crop growth in different locations. Care
  must be taken to understand each input quantity before attempting to run
  simulations in other places or for other cultivars.
}

\usage{miscanthus_x_giganteus}

\format{
  A list of 5 named elements that are suitable for passing to
  \code{\link{run_biocro}}, as described in the help page for
  \code{\link{crop_model_definitions}}.
}

\source{
  This model was originally described in Miguez \emph{et al.} (2009)
  [\doi{10.1111/j.1757-1707.2009.01019.x}] and Miguez \emph{et al.} (2012)
  [\doi{10.1111/j.1757-1707.2011.01150.x}]. Since its original parameterization,
  the behavior of several of its core modules has changed as bugs have been
  identified and fixed, so this model likely needs to be reparameterized before
  it can be used for realistic simulations.
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
