\name{simultaneous_equations}

\alias{simultaneous_equations}

\alias{solve_simultaneous_equations}
\alias{validate_simultaneous_equations}
\alias{test_simultaneous_equations}
\alias{get_all_se_solvers}

\title{Simultaneous equations}

\description{
  These functions are only intended for use by Ed. This documentation is only
  here to avoid warnings from \code{R CMD check} and is not intended to actually
  describe what these functions do or how they should be used.
}

\usage{
  solve_simultaneous_equations(
    direct_modules,
    known_quantities,
    unknown_quantities,
    lower_bounds,
    upper_bounds,
    abs_error_tols,
    rel_error_tols,
    max_it = 50,
    solver_type = "newton_raphson_backtrack_boost",
    silent = FALSE
  )

  validate_simultaneous_equations(
    direct_modules,
    known_quantities,
    unknown_quantities,
    silent = FALSE
  )

  test_simultaneous_equations(
    direct_modules,
    known_quantities,
    unknown_quantities
  )

  get_all_se_solvers()
}

\arguments{
  \item{abs_error_tols}{Please ignore this.}
  \item{direct_modules}{Please ignore this.}
  \item{known_quantities}{Please ignore this.}
  \item{lower_bounds}{Please ignore this.}
  \item{max_it}{Please ignore this.}
  \item{rel_error_tols}{Please ignore this.}
  \item{silent}{Please ignore this.}
  \item{solver_type}{Please ignore this.}
  \item{unknown_quantities}{Please ignore this.}
  \item{upper_bounds}{Please ignore this.}
}

\value{
  \item{solve_simultaneous_equations}{Please ignore this.}
  \item{validate_simultaneous_equations}{Please ignore this.}
  \item{test_simultaneous_equations}{Please ignore this.}
  \item{get_all_se_solvers}{Please ignore this.}
}
