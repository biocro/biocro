\name{get_all}

\alias{get_all_modules}
\alias{get_all_quantities}
\alias{get_all_ode_solvers}

\title{BioCro Functions for Getting Modules, Quantities, and Solvers}

\description{
  \code{get_all_modules} returns the names of all available BioCro modules.

  \code{get_all_quantities} returns information about all quantities used as
    inputs or outputs by BioCro modules.

  \code{get_all_ode_solvers} returns the names of all available ordinary
    differential equation (ODE) solvers available in BioCro.
}

\usage{
  View(get_all_modules())

  View(get_all_quantities())

  View(get_all_ode_solvers())
}

\arguments(None)

\details{
  These "get_all" functions return the modules, quantities, and ODE solvers
  available within the BioCro package
}

\value{
  \item{get_all_modules}{A character vector of module names}

  \item{get_all_quantities}{A data frame with three columns:
    \code{quantity_type}, \code{quantity_name}, and \code{module_name}, where
    each row represents one instance of a quantity associated with a module as
    either an input or an output}

  \item{get_all_ode_solvers}{A character vector of ODE solver names}
}

\examples{
# Example 1: getting a list of all unique quantities (which can be useful when
# writing a new module that is intended to work along with pre-existing modules)
all_quantities <- get_all_quantities()
all_quantity_names <- all_quantities$quantity_name
all_unique_quantities <- unique(all_quantity_names)
all_unique_quantities <- sort(all_unique_quantities)

# Example 2: getting a list of all modules that have "ci" or "Ci" as an input or
# output
all_quantities <- get_all_quantities()
ci_modules <- subset(all_quantities, quantity_name=="ci" | quantity_name=="Ci")
}
