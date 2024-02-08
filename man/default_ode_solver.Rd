\name{default_ode_solver}

\alias{default_ode_solver}

\title{Get a default ODE solver}

\description{
Returns default ODE solver settings for each solver type.
}

\usage{
default_ode_solver(type = 'homemade_euler')
}

\arguments{
  \item{type}{
    The name of a solver; a full list can be obtained with the
    \code{\link{get_all_ode_solvers}} function.
  }
}

\value{
A list of 5 named elements that can be passed to \code{\link{run_biocro}} as its
\code{ode_solver} input argument.
}

\examples{
# Get the default solver that is used by `run_biocro` when no other options are
# specified
default_ode_solver()
}
