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
    ode_solver = BioCro::default_ode_solvers$homemade_euler,
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
  sensitivity analysis. Optimizers (such as \code{\link[stats]{optim}})
  typically require a function with a single input argument, so the partial
  application tools provided here help to create such functions.

  Both \code{partial_run_biocro} and \code{partial_evaluate_module} accept the
  same arguments as their "regular" counterparts (\code{\link{run_biocro}} and
  \code{\link{evaluate_module}}) with the addition of \code{arg_names}, which
  specifies the input quantities that should not be fixed.

  For \code{partial_run_biocro}, each element of \code{arg_names} must be the
  name of a quantity that is one of the \code{initial_values},
  \code{parameters}, or \code{drivers}. For \code{partial_evaluate_module}, each
  element of \code{arg_names} must be the name of one of the module's input
  quantities.

  When using one of the pre-defined crop growth models, it may be helpful to
  use the \code{with} command to pass arguments to \code{partial_run_biocro};
  see the documentation for \code{\link{crop_model_definitions}} for more
  information.
}

\value{
  \item{partial_run_biocro}{
    A function that calls \code{\link{run_biocro}} with all of the inputs
    (except those specified in \code{arg_names}) set to the values specified by
    the original call to \code{partial_run_biocro}. The new function has one
    input (\code{x}), which can be a vector or list specifying the values of the
    quantities in \code{arg_names}. If \code{x} has no names, its elements must
    be supplied in the same order as in the original \code{arg_names}. If
    \code{x} has names, they must be identical to the elements of
    \code{arg_names} but can be in any order. Elements of \code{x} corresponding
    to drivers must be vectors having the same length as the other drivers; they
    can be specified as a named element of a list or as sequential elements of
    a vector without names. The return value of the new function is a data frame
    as would be produced by \code{\link{run_biocro}}.
  }

  \item{partial_evaluate_module}{
    A function that calls \code{\link{evaluate_module}} with the input
    quantities (except those specified in \code{arg_names}) set to the values
    specified by the original call to \code{partial_evaluate_module}. The new
    function has one input (\code{x}), which can be a vector or list specifying
    the values of the quantities in \code{arg_names}. If \code{x} has no names,
    its elements must be supplied in the same order as in the original
    \code{arg_names}. If \code{x} has names, they must be identical to the
    elements of \code{arg_names} but can be in any order. The return value of
    the new function is a list with two elements (\code{inputs} and
    \code{outputs}), each of which is a list of named numeric elements
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
# Specify weather data to use in these examples
ex_weather <- get_growing_season_climate(weather$'2005')

# Example 1: varying the thermal time values at which senescence starts for
# different organs in a simulation; here we set them to the following values
# instead of the defaults:
#  - seneLeaf: 2000 degrees C * day
#  - seneStem: 2100 degrees C * day
#  - seneRoot: 2200 degrees C * day
#  - seneRhizome: 2300 degrees C * day
senescence_simulation <- partial_run_biocro(
  miscanthus_x_giganteus$initial_values,
  miscanthus_x_giganteus$parameters,
  ex_weather,
  miscanthus_x_giganteus$direct_modules,
  miscanthus_x_giganteus$differential_modules,
  miscanthus_x_giganteus$ode_solver,
  c('seneLeaf', 'seneStem', 'seneRoot', 'seneRhizome')
)
senescence_result <- senescence_simulation(c(2000, 2100, 2200, 2300))

# Example 2: a crude method for simulating the effects of climate change; here
# we increase the atmospheric CO2 concentration to 500 ppm and the temperature
# by 2 degrees C relative to 2005 temperatures. The commands below that call
# `temperature_simulation` all produce the same result.
temperature_simulation <- partial_run_biocro(
  miscanthus_x_giganteus$initial_values,
  miscanthus_x_giganteus$parameters,
  ex_weather,
  miscanthus_x_giganteus$direct_modules,
  miscanthus_x_giganteus$differential_modules,
  miscanthus_x_giganteus$ode_solver,
  c("Catm", "temp")
)
hot_result_1 <- temperature_simulation(c(500, ex_weather$temp + 2.0))
hot_result_2 <- temperature_simulation(list(Catm = 500, temp = ex_weather$temp + 2.0))
hot_result_3 <- temperature_simulation(list(temp = ex_weather$temp + 2.0, Catm = 500))

# Note that these commands will both produce errors:
# hot_result_4 <- temperature_simulation(c(Catm = 500, temp = ex_weather$temp + 2.0))
# hot_result_5 <- temperature_simulation(stats::setNames(
#   c(500, ex_weather$temp + 2.0),
#   c("Catm", rep("temp", length(ex_weather$temp)))
# ))

# Note that this command will produce a strange result where the first
# temperature value will be incorrectly interpreted as a `Catm` value, and the
# `Catm` value will be interpreted as the final temperature value.
# hot_result_6 <- temperature_simulation(c(ex_weather$temp + 2.0, 500))

# Example 3: varying the base and air temperature inputs to the
# 'thermal_time_linear' module from the 'BioCro' module library. The commands
# below that call `thermal_time_rate` all produce the same result.
thermal_time_rate <- partial_evaluate_module(
  'BioCro:thermal_time_linear',
  within(miscanthus_x_giganteus$parameters, {doy = 1}),
  c("temp", "tbase")
)
rate_result_1 <- thermal_time_rate(c(25, 10))
rate_result_2 <- thermal_time_rate(c(temp = 25, tbase = 10))
rate_result_3 <- thermal_time_rate(c(tbase = 10, temp = 25))
rate_result_4 <- thermal_time_rate(list(temp = 25, tbase = 10))
rate_result_5 <- thermal_time_rate(list(tbase = 10, temp = 25))
}
