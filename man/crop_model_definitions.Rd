\name{crop_model_definitions}

\docType{data}

\alias{crop_model_definitions}

\title{Crop model definitions}

\description{
  In BioCro, a crop model is defined by sets of direct modules, differential
  modules, initial values, and parameters, along with an ordinary differential
  equation (ODE) solver. To run a model, these values, along with a set of
  weather data, are passed to the \code{\link{run_biocro}} function. For
  convenience, several crop model definitions are included in the BioCro R
  package. A full list can be obtained by typing \code{??crop_models} into the R
  terminal.
}

\details{
  Each crop model definition is stored as a list with the following named
  elements:
  \itemize{
    \item \code{direct_modules}: A list of direct module names; can be passed to
    \code{\link{run_biocro}} as its \code{direct_module_names} argument.

    \item \code{differential_modules}: A list of differential module names; can
    be passed to \code{\link{run_biocro}} as its
    \code{differential_module_names} argument.

    \item \code{ode_solver}: A list specifying details of a numerical ODE
    solver; can be passed to \code{\link{run_biocro}} as its \code{ode_solver}
    argument.

    \item \code{initial_values}: A list of named quantity values; can be passed
    to \code{\link{run_biocro}} as its \code{initial_values} argument.

    \item \code{parameters}: A list of named quantity values; can be passed to
    \code{\link{run_biocro}} as its \code{parameters} argument, and also can be
    passed to \code{\link{evaluate_module}} and
    \code{\link{module_response_curve}} when investigating the behavior of one
    of the crop's modules.
  }

  These model definitions are not sufficient for running a simulation because
  \code{\link{run_biocro}} also requires drivers. The crop parameter lists also
  do not include soil parameter values; these are typically specified by
  combining the crop parameter list with one of the soil parameter lists from
  the \code{\link{soil_parameters}} dataset. For simulations in Champaign,
  Illinois, the \code{soil_parameters$clay_loam} set should be used.

  The \code{\link{soybean}} model is intended to be used along with the
  specialized soybean weather data (\code{\link{cmi_soybean_weather_data}}). The
  other crops should be used with the other weather data
  (\code{\link{cmi_weather_data}}).

  Some quantities in the crop model definitions, such as the values of
  photosynthetic parameters, would remain the same in any location; others, such
  as the latitude or longitude, would need to change when simulating crop growth
  in different locations. Care must be taken to understand each input quantity
  before attempting to run simulations in other places or for other cultivars.

  Typically, the modules in a crop model definition are defined as lists with
  some named elements; the names facilitate on-the-fly module swapping via the
  \code{\link{within}} function. For example, to change the soybean canopy
  photosynthesis module to the \code{ten_layer_rue_canopy} module, one could
  pass
  \code{within(soybean$direct_modules, {canopy_photosynthesis = "BioCro:ten_layer_rue_canopy"})}
  as the \code{direct_module_names} argument when calling
  \code{\link{run_biocro}} instead of \code{soybean$direct_modules}.

  Because each crop model definition is stored as a list with named elements,
  it is possible to use the \code{\link{with}} function to save some typing when
  calling \code{\link{run_biocro}}. For example, the following two commands are
  equivalent:
  \itemize{
    \item \code{
    result <- run_biocro(soybean$initial_values,
    c(soybean$parameters, soil_parameters$clay_loam), soybean_weather$'2002',
    soybean$direct_modules, soybean$differential_modules, soybean$ode_solver)
    }

    \item \code{
    result <- with(soybean, {run_biocro(initial_values,
    c(parameters, soil_parameters$clay_loam), soybean_weather$'2002',
    direct_modules, differential_modules, ode_solver)})
    }
  }
}

\seealso{
  \itemize{
    \item \code{\link{run_biocro}}
    \item \code{\link{modules}}
  }
}

\keyword{datasets}
