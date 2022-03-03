\name{get_all}

\alias{get_all_modules}
\alias{get_all_quantities}
\alias{get_all_ode_solvers}

\title{Getting lists of modules, quantities, and solvers}

\description{
  \code{get_all_modules} returns the names of all modules available in the
  package.

  \code{get_all_quantities} returns information about all quantities used as
  inputs or outputs by modules available in the package.

  \code{get_all_ode_solvers} returns the names of all ordinary differential
  equation (ODE) solvers available in BioCro.
}

\usage{
  get_all_modules()

  get_all_quantities()

  get_all_ode_solvers()
}

\arguments{None}

\details{
  These "get_all" functions return the modules, quantities, and ODE solvers
  available within the package.

  Each module library package has its own version of \code{get_all_modules} and
  \code{get_all_quantities}. To avoid name conflicts, these functions are not
  included in the package namespaces and must be accessed using the package name
  via the \code{\link{:::}} operator.
}

\value{
  \item{get_all_modules}{A character vector of module names}

  \item{get_all_quantities}{A data frame with three columns:
    \code{quantity_type} (input or output), \code{quantity_name}, and
    \code{module_name}. A quantity will appear multiple times if it is used as
    both an input and an output, or if it is used by multiple modules.}

  \item{get_all_ode_solvers}{A character vector of ODE solver names}
}

\examples{
# Example 1: getting a list of all unique quantities (which can be useful when
# writing a new module that is intended to work along with pre-existing modules)
all_quantities <- BioCro:::get_all_quantities()
all_quantity_names <- all_quantities$quantity_name
all_unique_quantities <- unique(all_quantity_names)
all_unique_quantities <- sort(all_unique_quantities)

# Example 2: getting a list of all modules that have "ci" as an input or output,
# using `tolower()` to account for any possible variations in capitalization
all_quantities <- BioCro:::get_all_quantities()
ci_modules <- subset(all_quantities, tolower(quantity_name) == "ci")
}
