\name{default_ode_solvers}

\docType{data}

\alias{default_ode_solvers}

\title{Default ODE solver settings}

\description{
  A collection of reasonable settings to use with each ODE solver type. Users
  may need or wish to modify them for particular applications.
}

\usage{default_ode_solvers}

\format{
  A list of 6 named elements, where each name is one of the possible ODE solver
  types. Each element is itself a list of 5 named elements that can be passed to
  \code{\link{run_biocro}} as its \code{ode_solver} input argument.
}

\details{
  A full list of solver types can be obtained with the
  \code{\link{get_all_ode_solvers}} function.
}

\keyword{datasets}
