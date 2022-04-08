\name{partial_application}

\alias{partial_application}
\alias{partial_run_biocro}
\alias{partial_evaluate_module}

\title{Convenience Functions for Partial Application}

\description{Convenience functions for using partial application with BioCro}

\usage{
partial_run_biocro(
    initial_values = list(),
    parameters = list(),
    drivers,
    direct_module_names = list(),
    differential_module_names = list(),
    ode_solver = BioCro:::default_ode_solver,
    arg_names,
    verbose = FALSE
)

partial_evaluate_module(module_name, input_quantities, arg_names)
}

\arguments{
  \item{arg_names}{
    A vector of strings specifying input quantities whose values should not be
    fixed when using partial application.
  }

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

  \item{ode_solver}{
    Identical to the corresponding argument from \code{\link{run_biocro}}.
  }

  \item{verbose}{
    Identical to the corresponding argument from \code{\link{run_biocro}}.
  }

  \item{module_name}{
    Identical to the corresponding argument from \code{\link{evaluate_module}}.
  }

  \item{input_quantities}{
    A list of named numeric elements representing any input quantities required
    by the module that are not included in \code{arg_names}; any extraneous
    quantities will be ignored by the module.
  }
}

\details{
  \emph{Partial application} is the technique of fixing some of the input
  arguments to a function, producing a new function with fewer inputs. In the
  context of BioCro, partial application can often be useful while varying some
  parameters, initial values, or drivers while performing optimization or
  sensitivity analysis.

  Both \code{partial_run_biocro} and \code{partial_evaluate_module} accept the
  same arguments as their "regular" counterparts (\code{\link{run_biocro}} and
  \code{\link{evaluate_module}}) with the addition of \code{arg_names}, which
  specifies the input quantities that should not be fixed.

  For \code{partial_run_biocro}, each element of \code{arg_names} must be the
  name of a quantity that is one of the \code{initial_values},
  \code{parameters}, or \code{drivers}. For \code{partial_evaluate_module}, each
  element of \code{arg_names} must be the name of one of the module's input
  quantities.
}

\value{
  \item{partial_run_biocro}{
    A function that calls \code{\link{run_biocro}} with all of the inputs
    (except those specified in \code{arg_names}) set as their default values.
    The only input argument of the new function is a numerical vector specifying
    the values of the quantities in \code{arg_names}. In this vector, the values
    must be supplied in the same order as in \code{arg_names}, and arguments
    corresponding to drivers must be vectors having the same length as the other
    drivers.
  }

  \item{partial_evaluate_module}{
    A function that calls \code{\link{evaluate_module}} with the input
    quantities (except those specified in \code{arg_names}) set as their default
    values. The only input argument of the new function is a numerical vector
    specifying the values of the quantities in \code{arg_names}. In this vector,
    the values must be supplied in the same order as in \code{arg_names}. The
    return value of the new function is a list with two elements (\code{inputs}
    and \code{outputs}), each of which is a list of named numeric elements
    representing the module's input and output values. (Note that this differs
    from the output of \code{evaluate_module}, which only returns the outputs.)
  }
}

\seealso{
  \itemize{
    \item \code{\link{run_biocro}}
    \item \code{\link{evaluate_module}}
  }
}

\examples{
# Example 1: varying the thermal time values at which senescence starts for
# different organs in a simulation; here we set them to the following values
# instead of the defaults:
#  - seneLeaf: 2000 degrees C * day
#  - seneStem: 2100 degrees C * day
#  - seneRoot: 2200 degrees C * day
#  - seneRhizome: 2300 degrees C * day
senescence_simulation <- partial_run_biocro(
    miscanthus_x_giganteus_initial_values,
    miscanthus_x_giganteus_parameters,
    get_growing_season_climate(weather2005),
    miscanthus_x_giganteus_direct_modules,
    miscanthus_x_giganteus_differential_modules,
    miscanthus_x_giganteus_ode_solver,
    c('seneLeaf', 'seneStem', 'seneRoot', 'seneRhizome')
)
senescence_result <- senescence_simulation(c(2000, 2100, 2200, 2300))

# Example 2: a crude method for simulating the effects of climate change; here we
# increase the atmospheric CO2 concentration to 500 ppm and the temperature by
# 2 degrees C relative to 2002 temperatures. The two commands below are
# equivalent, so `hot_result_1` is identical to `hot_result_2`.
temperature_simulation <- partial_run_biocro(
  soybean_initial_values,
  soybean_parameters,
  soybean_weather2002,
  soybean_direct_modules,
  soybean_differential_modules,
  soybean_ode_solver,
  c("Catm", "temp")
)
hot_result_1 <- temperature_simulation(c(500, soybean_weather2002$temp + 2.0))
hot_result_2 <- temperature_simulation(list(Catm = 500, temp = soybean_weather2002$temp + 2.0))

# Example 3: varying the temperature input to the 'thermal_time_linear' module
thermal_time_rate <- partial_evaluate_module(
  'BioCro:thermal_time_linear',
  within(miscanthus_x_giganteus_parameters, {time = 1}),
  c("temp")
)
rate_25 <- thermal_time_rate(25)
}
