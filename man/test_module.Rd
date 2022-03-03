\name{test_module}

\alias{test_module}

\alias{case}
\alias{cases_from_csv}

\title{Run BioCro module test cases}

\description{
  Modules can be verified (a.k.a. tested) using test cases: data sets with known
  outputs for a given input. The functions here provide ways to create test cases
  and test modules.

  \code{test_module} runs one test case for a module, throwing an error if its
  output does not match the expected value.

  \code{case} helps define test cases for module testing by combining the
  required elements into a list with the correct names as required by
  \code{test_module}.

  \code{cases_from_csv} helps define test cases for module testing by creating a
  list of test cases from a \code{csv} file.
}

\usage{
  test_module(module_name, case_to_test)

  case(inputs, expected_outputs, description)

  cases_from_csv(module_name, directory)
}

\arguments{
  \item{module_name}{
    A string representing the name of one BioCro module.
  }

  \item{case_to_test}{
    A list with three named elements that describe a module test case:
    \itemize{
      \item \code{inputs}: A list of module inputs, i.e., a list of named
            numeric elements corresponding to the module's input quantities.

      \item \code{expected_outputs}: A list of expected module outputs, i.e., a
            list of named numeric elements corresponding to the expected values
            of the module's output quantities.

      \item \code{description}: A string describing the test case, e.g.
            \code{"temp below tbase"}. The description should be succinct and
            not contain any newline characters. If the description contains a
            comma, it must be wrapped in quotes. This is not advised.
    }
  }

  \item{inputs}{
    See the corresponding entry in \code{test_case} above.
  }

  \item{expected_outputs}{
    See the corresponding entry in \code{test_case} above.
  }

  \item{description}{
    See the corresponding entry in \code{test_case} above.
  }

  \item{directory}{
    The directory where module test case files are stored, e.g.
    \code{file.path('module_tests')}
  }

}

\details{
  The \code{test_module} function forms the basis for the BioCro module testing
  system. (See \code{\link{module_testing}} for more information.) The functions
  \code{case} and \code{cases_from_csv} are complementary to \code{test_module}
  because they help to pass suitably-formatted test cases to \code{test_module}.
}

\value{
  \item{case}{
    A list with three named elements (\code{inputs}, \code{expected_outputs},
    and \code{description}) formed from the input arguments.
  }

  \item{cases_from_csv}{
    A list of test cases created by the \code{case} function that are each
    suitable for passing to the \code{test_module} function.
  }
}

\seealso{
  \itemize{
    \item \code{\link{modules}}
    \item \code{\link{module_case_files}}
    \item \code{\link{module_testing}}
  }
}

\examples{
# Example 1: Defining an individual test case and running it.
test_module(
  'thermal_time_linear',
  case(
    list(time = 101, sowing_time = 100, tbase = 20, temp = 44),
    list(TTc = 1.0),
    'temp above tbase'
  )
)

# Example 2: Loading a set of test cases from a file and running one of them.
# Note: here we use the `initialize_csv` function first to ensure that there is
# a properly defined test file in the current directory.
\dontrun{
initialize_csv('thermal_time_linear', '.')
cases <- cases_from_csv('thermal_time_linear', '.')
test_module('thermal_time_linear', cases[[1]])
}
}
