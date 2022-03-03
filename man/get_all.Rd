\name{get_all}

\alias{get_all_modules}
\alias{get_all_quantities}
\alias{get_all_ode_solvers}

\title{Getting lists of modules, quantities, and solvers}

\description{
  \code{get_all_modules} returns the names of all modules available in a BioCro
  module library package.

  \code{get_all_quantities} returns information about all quantities used as
  inputs or outputs by modules available in a BioCro module library package.

  \code{get_all_ode_solvers} returns the names of all ordinary differential
  equation (ODE) solvers available in BioCro.
}

\usage{
  get_all_modules(package_name)

  get_all_quantities(package_name)

  get_all_ode_solvers()
}

\arguments{None}

\details{
  These "get_all" functions return the modules, quantities, and ODE solvers
  available within the BioCro framework or a BioCro module library package.

  _Developer details_: the \code{get_all_modules} and \code{get_all_quantities}
  expect a module library package to include unexported functions called
  \code{get_all_modules_internal} and \code{get_all_quantities_internal},
  respectively. These functions should not have any input arguments, and their
  return values should follow the requirements described below for
  \code{get_all_modules} and \code{get_all_quantities}. Any module library
  package created by forking from the skeleton library will automatically
  include these functions without any modifications to the package's R code.
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
# Example 1: Getting a list of all unique quantities defined by modules in the
# `BioCro` module library. Doing this can be useful when writing a new module
# that is intended to work along with pre-existing modules.
all_quantities <- get_all_quantities('BioCro')
all_quantity_names <- all_quantities$quantity_name
all_unique_quantities <- unique(all_quantity_names)
all_unique_quantities <- sort(all_unique_quantities)

# Example 2: Getting a list of all modules in the `BioCro` module library that
# have "ci" as an input or output, using `tolower()` to account for any possible
# variations in capitalization.
all_quantities <- get_all_quantities('BioCro')
ci_modules <- subset(all_quantities, tolower(quantity_name) == "ci")
}
