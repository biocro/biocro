\name{manihot_esculenta}

\docType{data}

\alias{manihot_esculenta}

\title{Manihot esculenta model definition}

\description{
  Initial values, parameters, direct modules, differential modules, and
  a differential equation solver that can be used to run
  \emph{Manihot esculenta} (cassava) growth simulations in Champaign, Illinois
  and other locations. Note that this model has not been published or validated
  against experimental data.

  To represent cassava growth in Champaign, IL, these values must be paired with
  the Champaign weather data (\code{\link{cmi_weather_data}}). The parameters
  already include the \code{clay_loam} values from the
  \code{\link{soil_parameters}} dataset, which is the appropriate soil type for
  Champaign.

  Some specifications, such as the values of photosynthetic parameters, would
  remain the same in any location; others, such as the latitude or longitude,
  would need to change when simulating crop growth in different locations. Care
  must be taken to understand each input quantity before attempting to run
  simulations in other places or for other cultivars.
}

\usage{manihot_esculenta}

\format{
  A list of 5 named elements that are suitable for passing to
  \code{\link{run_biocro}}, as described in the help page for
  \code{\link{crop_model_definitions}}.
}

\details{
  For this model, error tolerances greater than \code{1e-5} may cause problems
  with the crop model regression tests, so
  \code{manihot_esculenta$ode_solver$adaptive_rel_error_tol} and
  \code{manihot_esculenta$ode_solver$adaptive_abs_error_tol} are both set to
  \code{1e-5}.
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
