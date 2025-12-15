\name{test_module_library}

\alias{test_module_library}

\title{Run module test cases for an entire BioCro module library}

\description{
  Modules can be tested using test cases, which are sets of known outputs that
  correspond to particular inputs. The \code{test_module_library} function
  provides a way to run all test cases for all modules in a BioCro module
  library.

  Note that \emph{module tests} are distinct from the \emph{model tests}
  described in \code{\link{model_testing}}.
}

\usage{
  test_module_library(
    library_name,
    directory,
    modules_to_skip = c(),
    verbose = TRUE
  )
}

\arguments{
  \item{library_name}{The name of a BioCro module library.}

  \item{directory}{
    The directory where module test case files are stored, e.g.
    \code{file.path('tests', 'module_test_cases')}
  }

  \item{modules_to_skip}{
    A vector of local module name strings indicating any modules from the
    library that should not be tested. This feature should be used sparingly,
    since there are very few legitimate reasons to skip a module test.
  }

  \item{verbose}{
    To be passed to \code{\link{test_module}}.
  }
}

\details{
  For each CSV file in the specified directory, \code{test_module_library}
  determines the corresponding module name and checks to make sure it is part of
  the specified library. If there are test cases for modules not in the library,
  \code{test_module_library} throws an error with a message containing the
  "unexpected" module test cases.

  For each module in the specified library, \code{test_module_library} loads
  stored test cases from the specified directory and runs each test case,
  storing information about any test failures or other issues that may occur.
  If any problems are detected, \code{test_module_library} throws an error with
  a message describing the issues.

  For an example of how this function can be used along with the
  \code{\link[testthat]{testthat}} package, see
  \code{tests/testthat/test.Modules.R}.
}

\value{
  None
}

\seealso{
  \itemize{
    \item \code{\link{modules}}
    \item \code{\link{module_case_files}}
    \item \code{\link{module_testing}}
    \item \code{\link{test_module}}
  }
}

\examples{
# Here we will initialize a module test case file in a temporary directory, and
# then use `test_module_library` to test it. We will need to skip most of the
# modules in the library, since we only have a test case for one of them.

td <- file.path(tempdir(), 'module_test_cases')
dir.create(td, showWarnings = FALSE)

initialize_csv(
  'BioCro:thermal_time_linear',
  td,
  nonstandard_inputs = list(temp = -1),
  overwrite = TRUE
)

# Get a list of local module names, excluding the module that has a test case
all_modules <- get_all_modules('BioCro')
skip <- all_modules[all_modules != 'BioCro:thermal_time_linear']
skip <- gsub('BioCro:', '', skip)

test_module_library('BioCro', td, skip)

# If we attempt to test the entire library, we will get errors since only one
# module actually has an associated case file
tryCatch(
  {
    test_module_library('BioCro', td)
  },
  error = function(e) {print(e)}
)
}
