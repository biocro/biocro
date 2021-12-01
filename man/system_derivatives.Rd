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
  The input arguments to \code{system_derivatives} are the same as the
  corresponding inputs to \code{\link{run_biocro}}.
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
# Example 1: calculating a single derivative using a soybean model

soybean_system <- system_derivatives(
  soybean_parameters,
  soybean_weather2002,
  soybean_direct_modules,
  soybean_differential_modules
)

derivs <- soybean_system(0, unlist(soybean_initial_values), NULL)

# Example 2: a simple oscillator with only one module (requires deSolve)

if (require(deSolve)) {

  times = seq(0, 5, length=100)

  oscillator_system_derivatives <- system_derivatives(
    list(
      timestep = 1,
      mass = 1,
      spring_constant = 1
    ),
    data.frame(time=times),
    character(),
    'harmonic_oscillator'
  )

  result <- as.data.frame(lsodes(
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
}

# Example 3: solving 500 hours of a soybean simulation. This requires the
# deSolve package and will run very slow compared to a regular call to
# `run_biocro`.
\donttest{
if (require(deSolve)) {
  soybean_system <- system_derivatives(
    soybean_parameters,
    soybean_weather2002,
    soybean_direct_modules,
    soybean_differential_modules
  )

  times = seq(from=0, to=500, by=1)

  result <- as.data.frame(lsodes(unlist(soybean_initial_values), times, soybean_system))

  lattice::xyplot(Leaf + Stem ~ time, type='l', auto=TRUE, data=result)
}
}
}
