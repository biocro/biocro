\name{dynamical_system}

\alias{dynamical_system}
\alias{validate_dynamical_system_inputs}

\title{Validating dynamical system inputs}

\description{
  Utility function for checking inputs to \code{run_biocro} without running it
}

\usage{
  validate_dynamical_system_inputs(
      initial_values = list(),
      parameters = list(),
      drivers,
      direct_module_names = list(),
      differential_module_names = list(),
      verbose = TRUE
  )
}

\arguments{
  \item{initial_values}{
    Identical to the corresponding argument from \code{\link{run_biocro}}.
  }

  \item{parameters}{
    Identical to the corresponding argument from \code{\link{run_biocro}}.
  }

  \item{drivers}{
    Identical to the corresponding argument from \code{\link{run_biocro}}.
  }

  \item{direct_module_names}{
    Identical to the corresponding argument from \code{\link{run_biocro}}.
  }

  \item{differential_module_names}{
    Identical to the corresponding argument from \code{\link{run_biocro}}.
  }

  \item{verbose}{
    Identical to the corresponding argument from \code{\link{run_biocro}}.
  }
}

\details{
  \code{validate_dynamical_system_inputs} accepts the same input arguments as
  \code{\link{run_biocro}} with the exception of \code{ode_solver} (which is
  not required to check the validity of a dynamical system).

  \code{validate_dynamical_system_inputs} checks a set of parameters, drivers,
  modules, and initial values to see if they can properly define a dynamical
  system and can therefore be used as inputs to \code{\link{run_biocro}}.
  Although the \code{\link{run_biocro}} function performs the same validity
  checks, the \code{validate_dynamical_system_inputs} includes additional
  information, such as a list of parameters whose values are not used as inputs
  by any modules, since in principle these parameters could be removed for
  clarity.

  When using one of the pre-defined crop growth models, it may be helpful to
  use the \code{with} command to pass arguments to
  \code{validate_dynamical_system_inputs}; see the documentation for
  \code{\link{crop_model_definitions}} for more information.
}

\value{
  A boolean indicating whether or not the inputs are valid.
}

\seealso{\code{\link{run_biocro}}}

\examples{
  weather_data <- add_time_to_weather_data(soybean_weather$'2002')
# Example 1: missing a parameter and an initial value
validate_dynamical_system_inputs(
  within(soybean$initial_values, rm(Leaf)),         # remove the initial `Leaf` value
  within(soybean$parameters, rm(leaf_reflectance)), # remove `leaf_reflectance`
   weather_data,
  soybean$direct_modules,
  soybean$differential_modules
)

# Example 2: a valid set of input arguments
validate_dynamical_system_inputs(
  soybean$initial_values,
  soybean$parameters,
  weather_data,
  soybean$direct_modules,
  soybean$differential_modules
)
}
