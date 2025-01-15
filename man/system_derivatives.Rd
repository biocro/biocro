\name{system_derivatives}

\alias{system_derivatives}

\title{Calculate Derivatives for Differential Quantities}

\description{
  Solving a BioCro model using one of R's available differential equation
  solvers
}

\usage{
  system_derivatives(
    parameters = list(),
    drivers,
    direct_module_names = list(),
    differential_module_names = list()
  )
}

\arguments{
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
}

\details{
  \code{system_derivatives} accepts the same input arguments as
  \code{\link{run_biocro}} with the exceptions of \code{ode_solver} and
  \code{initial_values}; this function is intended to be passed to an ODE solver
  in R, which will solve for the system's time dependence as its diffferential
  quantities evolve from their initial values, so \code{ode_solver} and
  \code{initial_values} are not required here.

  When using one of the pre-defined crop growth models, it may be helpful to
  use the \code{with} command to pass arguments to \code{system_derivatives};
  see the documentation for \code{\link{crop_model_definitions}} for more
  information.
}

\value{
  The return value of \code{system_derivatives} is a function with three inputs
  (\code{t}, \code{differential_quantities}, and \code{parms}) that returns
  derivatives for each of the differential quantities in the dynamical system
  determined by the original inputs (\code{parameters}, \code{drivers},
  \code{direct_module_names}, and \code{differential_module_names}).

  This function signature and the requirements for its inputs are set by the
  \code{LSODES} function from the \code{deSolve} package. The \code{t} input
  should be a single time value and the \code{differential_quantities} input
  should be a vector with the names of the differential quantities defined by
  the modules. \code{parms} is required by \code{LSODES}, but we don't use it
  for anything.

  This function can be passed to \code{LSODES} as an alternative integration
  method, rather than using one of BioCro's built-in solvers.
}

\seealso{
  \code{\link{run_biocro}}
}

\examples{
# Note: Example 3 below may take several minutes to run. Patience is required!

# Example 1: calculating a single derivative using a soybean model

soybean_system <- system_derivatives(
  soybean$parameters,
  soybean_weather$'2002',
  soybean$direct_modules,
  soybean$differential_modules
)

derivs <- soybean_system(0, unlist(soybean$initial_values), NULL)

# Example 2: a simple oscillator with only one module

times = seq(0, 5, by = 1) # times by spaced by `timestep`

oscillator_system_derivatives <- system_derivatives(
  list(
    timestep = 1,
    mass = 1,
    spring_constant = 1
  ),
  data.frame(time = times),
  c(),
  'BioCro:harmonic_oscillator'
)

result <- as.data.frame(deSolve::lsodes(
  c(position=0, velocity=1),
  times,
  oscillator_system_derivatives
))

lattice::xyplot(
  position + velocity ~ time,
  type='l',
  auto=TRUE,
  data=result
)

# Example 3: solving 500 hours of a soybean simulation. This will run slowly
# compared to a regular call to `run_biocro`.

\donttest{

soybean_system <- system_derivatives(
  soybean$parameters,
  soybean_weather$'2002',
  soybean$direct_modules,
  soybean$differential_modules
)

times = seq(from=0, to=500, by=1)

result <- as.data.frame(deSolve::lsodes(unlist(soybean$initial_values), times, soybean_system))

lattice::xyplot(Leaf + Stem ~ time, type='l', auto=TRUE, data=result)
}
}
