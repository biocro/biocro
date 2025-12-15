\name{test_module}

\alias{test_module}

\alias{case}
\alias{cases_from_csv}

\title{Run BioCro module test cases}

\description{
  Modules can be tested using test cases, which are sets of known outputs that
  correspond to particular inputs. The functions here provide ways to create
  test cases and test modules.

  \code{test_module} runs one test case for a module, returning an error message
  if its output does not match the expected value.

  \code{case} helps define test cases for module testing by combining the
  required elements into a list with the correct names as required by
  \code{test_module}.

  \code{cases_from_csv} helps define test cases for module testing by creating a
  list of test cases from a \code{csv} file.

  Note that \emph{module tests} are distinct from the \emph{model tests}
  described in \code{\link{model_testing}}.
}

\usage{
  test_module(module_name, case_to_test, verbose = TRUE)

  case(inputs, expected_outputs, description)

  cases_from_csv(module_name, directory)
}

\arguments{
  \item{module_name}{
    A string specifying one BioCro module, formatted like
    \code{library_name:local_module_name}, where \code{library_name} is the name
    of a library that contains a module with local name
    \code{local_module_name}; such fully-qualified module names can be formed
    manually or with \code{\link{module_paste}}.
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
            not contain any newline characters.
    }
  }

  \item{verbose}{
    A logical value indicating whether to print information about failed tests
    to the R terminal. When \code{verbose} is \code{TRUE}, any module outputs
    that do not match the expected value will be printed.
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
    \code{file.path('tests', 'module_test_cases')}
  }

}

\details{
  The \code{test_module} function forms the basis for the BioCro module testing
  system. (See \code{\link{module_testing}} for more information.) The functions
  \code{case} and \code{cases_from_csv} are complementary to \code{test_module}
  because they help to pass suitably-formatted test cases to \code{test_module}.
}

\value{
  \item{test_module}{
    If the test passes, an empty string; otherwise, an informative message about
    what went wrong.
  }

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
    \item \code{\link{module_paste}}
    \item \code{\link{module_testing}}
    \item \code{\link{test_module_library}}
  }
}

\examples{
# Example 1: Defining an individual test case for the 'BioCro' module library's
# 'thermal_time_linear' module and running the test. This test will pass, so the
# return value will be an empty string: `character(0)`
test_module(
  'BioCro:thermal_time_linear',
  case(
    list(fractional_doy = 101, sowing_fractional_doy = 100, tbase = 20, temp = 44),
    list(TTc = 1.0),
    'temp above tbase'
  )
)

# Example 2: Defining an individual test case for the 'BioCro' module library's
# 'thermal_time_linear' module and running the test. This test will fail, so the
# return value will be a message indicating the failure. Because verbose is TRUE
# by default, information about the failure will also be printed.
test_module(
  'BioCro:thermal_time_linear',
  case(
    list(fractional_doy = 101, sowing_fractional_doy = 100, tbase = 20, temp = 44),
    list(TTc = 2.0),
    'temp above tbase'
  )
)

# Example 3: Loading a set of test cases from a file and running one of them.
# Note: here we use the `initialize_csv` function first to ensure that there is
# a properly defined test file in a temporary directory.

td <- tempdir()

module_name <- 'BioCro:thermal_time_linear'
initialize_csv(module_name, td)
cases <- cases_from_csv(module_name, td)
test_module(module_name, cases[[1]])
}
