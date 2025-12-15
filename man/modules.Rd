\name{modules}

\alias{modules}
\alias{module_info}
\alias{evaluate_module}
\alias{module_response_curve}
\alias{quantity_list_from_names}

\title{BioCro module functions}

\description{
  BioCro modules are named sets of equations, and each module is available from
  a BioCro module library. Each module is identified by a
  \href{https://en.wikipedia.org/wiki/Fully_qualified_name}{fully-qualified name}
  that includes the name of its library and its local name within that library.
  The functions here provide ways to access information about modules and to
  calculate their output values from sets of input values.

  \code{module_info} returns essential information about a BioCro module.

  \code{quantity_list_from_names} initializes a list of named numeric elements
  from a set of names.

  \code{evaluate_module} runs a BioCro module using a list of input quantity
  values.

  \code{module_response_curve} runs a BioCro module repeatedly with different
  input quantity values to produce a response curve.
}

\usage{
  module_info(module_name, verbose = TRUE)

  quantity_list_from_names(quantity_names)

  evaluate_module(module_name, input_quantities, stop_on_exception = FALSE)

  module_response_curve(
    module_name,
    fixed_quantities,
    varying_quantities,
    stop_on_exception = FALSE
  )
}

\arguments{
  \item{module_name}{
    A string specifying one BioCro module, formatted like
    \code{library_name:local_module_name}, where \code{library_name} is the name
    of a library that contains a module with local name
    \code{local_module_name}; such fully-qualified module names can be formed
    manually or with \code{\link{module_paste}}.
  }

  \item{verbose}{
    A boolean indicating whether or not to print information to the R console.
  }

  \item{input_quantities}{
    A list of named numeric elements representing the input quantities required
    by the module; any extraneous quantities will be ignored by the module.
  }

  \item{quantity_names}{
    A vector of strings.
  }

  \item{fixed_quantities}{
    A list of named numeric elements representing input quantities required
    by the module whose values should be considered to be constant; any
    extraneous quantities will be ignored by the module.
  }

  \item{varying_quantities}{
    A data frame where each column represents an input quantity required by the
    module whose value varies across the response curve.
  }

  \item{stop_on_exception}{
    A boolean indicating whether to stop when a module throws an exception; see
    below for more details.
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
  (\code{varying_quantities}). The returned data frame includes all the
  information that would be required to reproduce the curve: the full-qualified
  module name, all inputs (including ones with constant values), and the
  outputs. Note: if one quantity \code{q} is both an input and output of the
  module, its input value will be stored in the \code{q} column of the returned
  data frame and its output value will be stored in the \code{q.1} column; this
  renaming is performed automatically by the \code{\link{make.unique}} function.
}

\value{
  \item{module_info}{
    An \code{\link{invisible}} list of several named elements containing
    essential information about the module:
    \itemize{
      \item \code{module_name}: The module's (not-fully-qualified) name
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

  \item{quantity_list_from_names}{
    A list of named numeric elements, where the names are set by
    \code{quantity_names} and each value is set to 1.
  }

  \item{evaluate_module}{
    A list of named numeric elements representing the values of the module's
    outputs as calculated from the \code{input_quantities} according to the
    module's equations. If the module threw an exception during evaluation and
    \code{stop_on_exception} is \code{FALSE}, the return value will instead be a
    list with a single element named \code{error_msg} that contains the content
    of the error message; otherwise, if \code{stop_on_exception} is \code{TRUE},
    an R error will be thrown.
  }

  \item{module_response_curve}{
    A data frame where the first column is the fully-qualified name of the
    module that produced the response curve and the remaining columns are the
    module's input and output quantities. Each row corresponds to a row in the
    \code{varying_quantities}. If the module throws an exception during
    evaluation for one set of inputs and \code{stop_on_exception} is
    \code{FALSE}, the outputs of the corresponding row will be set to \code{NA}
    and the error message will be recorded in the \code{error_msg} column;
    otherwise, if \code{stop_on_exception} is \code{TRUE}, an R error will be
    thrown and the response curve calculations will cease.
  }
}

\seealso{
  \itemize{
    \item \code{\link{get_all_modules}}
    \item \code{\link{module_paste}}
    \item \code{\link{module_testing}}
    \item \code{\link{partial_evaluate_module}}
  }
}

\examples{
# Example 1: printing information about the 'BioCro' module library's
# 'c3_assimilation' module to the R console
module_info('BioCro:c3_assimilation')

# Example 2: getting the inputs to the 'BioCro' module library's
# 'thermal_time_linear' module, generating a default input list, and using it to
# run the module
info <- module_info('BioCro:thermal_time_linear', verbose = FALSE)
inputs <- quantity_list_from_names(info$inputs) # All inputs will be set to 1
outputs <- evaluate_module('BioCro:thermal_time_linear', inputs)

# Example 3: calculating the temperature response of light saturated net
# assimilation at several values of relative humidity in the absence of water
# stress using the 'BioCro' module library's 'c3_assimilation' module and
# the default soybean parameters. Here, the leaf temperature and humidity values
# are independent of each other, so we use the `expand.grid` function to form a
# data frame of all possible combinations of their values. Then we set the
# ambient temperature equal to the leaf temperature.
rc <- module_response_curve(
  'BioCro:c3_assimilation',
  within(soybean$parameters, {Qabs = 2000; StomataWS = 1; gbw = 1.2}),
  within(
    expand.grid(
      Tleaf = seq(from = 0, to = 40, length.out = 201),
      rh = c(0.2, 0.5, 0.8)
    ),
    {temp = Tleaf}
  )
)

caption <- paste(
  "Response curves calculated with several RH\nvalues and Q =",
  unique(rc$Qp),
  "micromol / m^2 / s\nusing the",
  unique(rc$module_name),
  "module"
)

lattice::xyplot(
  Assim ~ Tleaf,
  group = rh,
  data = rc,
  auto = TRUE,
  type = 'l',
  main = caption
)
}
