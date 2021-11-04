\name{modules}

\alias{module_info}
\alias{evaluate_module}

\title{BioCro Module Functions}

\description{
  \code{module_info} returns essential information about a BioCro module.

  \code{evaluate_module} runs a BioCro module using a list of input quantity
  values.
}

\usage{
  info <- module_info(module_name, verbose = TRUE)

  outputs <- evaluate_module(module_name, input_quantities)
}

\arguments{
  \item{module_name}{A string representing the name of a BioCro module}

  \item{verbose}{
    A boolean indicating whether or not to print information to the R console
  }

  \item{input_quantities}{
    A list of named numeric elements representing the input quantities required
    by the module; any extraneous quantities will be ignored by the module.
  }
}

\details{
  By providing avenues for retrieving information about a module and evaluating
  a module's equations, these two functions form the main interface to
  individual BioCro modules from within R.
}

\value{
  \item{module_info}{
    A list of several named elements containing essential information about the
    module:
    \itemize{
      \item \code{module_name}: The module's name; only one name is allowed at
            the moment, but multiple modules will be supported in the future
      \item \code{inputs}: A list of named numeric elements representing the
            module's inputs, set to default values of 1
      \item \code{outputs}: A list of named numeric elements representing the
            module's outputs, set to default values of 1
      \item \code{type}: The module's type represented as a string (either
            'differential' or 'direct')
      \item \code{euler_requirement}: Indicates whether the module requires a
            fixed-step Euler ODE solver when used in a BioCro simulation
      \item \code{creation_error_message}: Describes any errors that occurred
            while creating an instance of the module
    }
  }

  \item{evaluate_module}{
    A list of named numeric elements representing the values of the module's
    outputs as calculated from the \code{input_quantities} according to the
    module's equations
  }
}

\seealso{
  \code{\link{get_all_modules}}
}

\examples{
# Example: getting the inputs to the 'thermal_time_linear' module and using them
# to run the module
info <- module_info('thermal_time_linear')
outputs <- evaluate_module('thermal_time_linear', info$inputs)
}
