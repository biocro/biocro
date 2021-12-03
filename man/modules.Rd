\name{modules}

\alias{module_info}
\alias{evaluate_module}
\alias{module_response_curve}
\alias{quantity_list_from_names}

\title{BioCro module functions}

\description{
  \code{module_info} returns essential information about a BioCro module.

  \code{evaluate_module} runs a BioCro module using a list of input quantity
  values.

  \code{module_response_curve} runs a BioCro module repeatedly with different
  input quantity values to produce a response curve.

  \code{quantity_list_from_names} initializes a list of named numeric elements
  from a set of names
}

\usage{
  module_info(module_name, verbose = TRUE)

  evaluate_module(module_name, input_quantities)

  module_response_curve(module_name, fixed_quantities, varying_quantities)

  quantity_list_from_names(quantity_names)
}

\arguments{
  \item{module_name}{
    A string representing the name of a BioCro module; only one name is allowed
    at the moment, but multiple modules will be supported in the future
  }

  \item{verbose}{
    A boolean indicating whether or not to print information to the R console
  }

  \item{input_quantities}{
    A list of named numeric elements representing the input quantities required
    by the module; any extraneous quantities will be ignored by the module
  }

  \item{fixed_quantities}{
    A list of named numeric elements representing input quantities required
    by the module whose values should be considered to be constant; any
    extraneous quantities will be ignored by the module
  }

  \item{varying_quantities}{
    A data frame where each column represents an input quantity required by the
    module whose value varies across the response curve
  }

  \item{quantity_names}{
    A vector of strings
  }
}

\details{
  By providing avenues for retrieving information about a module and evaluating
  a module's equations, the \code{module_info} and \code{evaluate_module}
  functions form the main interface to individual BioCro modules from within R.
  The \code{quantity_list_from_names} function is a convenience function for
  preparing suitable quantity lists to pass to \code{evaluate_module}.

  The \code{module_response_curve} function provides a convenient way to
  calculate a module response curve. To do this, a user must specify a module to
  use, the values of any fixed input quantities (\code{input_quantities}), and
  a sequence of values for other quantities that vary across the response curve
  (\code{varying_quantities}). Since each module input and output is included as
  a column in the resulting data frame, this function can only be applied to
  modules without any overlap between inputs and outputs.
}

\value{
  \item{module_info}{
    An \code{\link{invisible}} list of several named elements containing
    essential information about the module:
    \itemize{
      \item \code{module_name}: The module's name
      \item \code{inputs}: A character vector of the module's inputs
      \item \code{outputs}: A character vector of the module's outputs
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

  \item{module_response_curve}{
    A data frame where the first column is the name of the module that produced
    the response curve and the remaining columns are the module's input and
    output quantities. Each row corresponds to a row in the
    \code{varying_quantities}.
  }

  \item{quantity_list_from_names}{
    A list of named numeric elements, where the names are set by
    \code{quantity_names} and each value is initialized to 1.
  }
}

\seealso{
  \itemize{
    \item \code{\link{get_all_modules}}
    \item \code{\link{partial_evaluate_module}}
  }
}

\examples{
# Example 1: printing information about the 'c3_assimilation' module to the R
# console
module_info('c3_assimilation')

# Example 2: getting the inputs to the 'thermal_time_linear' module, generating
# a default input list, and using it to run the module
info <- module_info('thermal_time_linear', verbose = FALSE)
inputs <- quantity_list_from_names(info$inputs) # All inputs will be set to 1
outputs <- evaluate_module('thermal_time_linear', inputs)

# Example 3: calculating the temperature response of light saturated net
# assimilation at several values of relative humidity in the absence of water
# stress using the 'c3_assimilation' module and soybean parameters. Here, the
# leaf temperature and humidity values are independent of each other, so we use
# the `expand.grid` function to form a data frame of all possible combinations
# of their values.
rc <- module_response_curve(
  'c3_assimilation',
  within(soybean_parameters, {Qp = 2000; StomataWS = 1}),
  expand.grid(
    Tleaf = seq(from = 0, to = 40, length.out = 201),
    rh = c(0.2, 0.5, 0.8)
  )
)
lattice::xyplot(Assim ~ Tleaf, group = rh, data = rc, auto = TRUE, type = 'l')
}
