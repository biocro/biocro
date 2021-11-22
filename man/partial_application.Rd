\name{partial_application}

\alias{partial_application}
\alias{partial_run_biocro}

\title{Convenience Functions for Partial Application}

\description{Convenience functions for using partial application with BioCro}

\usage{
partial_run_biocro(
    initial_values = list(),
    parameters = list(),
    drivers,
    direct_module_names = list(),
    differential_module_names = list(),
    ode_solver = default_ode_solver,
    arg_names,
    verbose = FALSE
)
}

\arguments{
  \code{partial_run_biocro} accepts the same input arguments as
  \code{\link{run_biocro}} with the addition of \code{arg_names}, which should
  be a vector of strings representing a subset of the named elements included in
  the \code{initial_values}, \code{parameters}, and \code{drivers}.
}

\details{
  \emph{Partial application} is the technique of fixing some of the input
  arguments to a function, producing a new function with fewer inputs. In the
  context of BioCro, partial application can often be useful while varying some
  parameters, initial values, or drivers while performing optimization or
  sensitivity analysis.
}

\value{
  A function that calls \code{\link{run_biocro}} with all of the inputs (except
  those specified in 'arg_names') set as their default values. The only
  input argument of the new function is a numerical vector specifying the values
  of the quantities in 'arg_names'.
}

\seealso{\code{\link{run_biocro}}}

\examples{
# Example: varying the thermal time values at which senescence starts for
# different organs in a simulation; here we set them to the following values
# instead of the defaults:
#  - seneLeaf: 2000 degrees C * day
#  - seneStem: 2100 degrees C * day
#  - seneRoot: 2200 degrees C * day
#  - seneRhizome: 2300 degrees C * day
senescence_simulation <- partial_run_biocro(
    sorghum_initial_values,
    sorghum_parameters,
    get_growing_season_climate(weather2005),
    sorghum_direct_modules,
    sorghum_differential_modules,
    sorghum_ode_solver,
    c('seneLeaf', 'seneStem', 'seneRoot', 'seneRhizome')
)
result <- senescence_simulation(c(2000, 2100, 2200, 2300))

# Example: adjusting water availability by supplying a constant value for
# precipitation; here we simulate drought by eliminating all rainfall
precip_simulation <- partial_run_biocro(
  soybean_initial_values,
  soybean_parameters,
  soybean_weather2002,
  soybean_direct_modules,
  soybean_differential_modules,
  soybean_ode_solver,
  "precip"
)
drought_result <- precip_simulation(0)
}
