\name{test_module_library}

\alias{test_module_library}

\title{Run module test cases for an entire BioCro module library}

\description{
  Modules can be tested using test cases, which are data sets of known outputs
  that correspond to particular inputs. The \code{test_module_library} function
  provides a way to run all test cases for all modules in a BioCro module
  library.
}

\usage{
test_module_library(library_name, directory, modules_to_skip = c())
}

\arguments{
  \item{library_name}{The name of a BioCro module library.}

  \item{directory}{
    The directory where module test case files are stored, e.g.
    \code{file.path('tests', 'module_test_cases')}
  }

  \item{modules_to_skip}{
    A vector of (not-fully-qualified) module name strings indicating any modules
    from the library that should not be tested. This feature should be used
    sparingly, since there are very few legitimate reasons to skip a module
    test.
  }
}

\details{
  For each module in the specified library, \code{test_module_library} loads
  stored test cases from the specified directory and runs each test case,
  storing information about any test failures or other issues that may occur.
  If any problems are detected, \code{test_module_library} throws an error with
  a message describing the issues.

  For an example of how this function can be used along with the
  \code{\link[testthat]{testthat}} package, see
  \code{tests/testthat/test.Modules.R}.
}

\seealso{
  \itemize{
    \item \code{\link{modules}}
    \item \code{\link{module_case_files}}
    \item \code{\link{module_testing}}
    \item \code{\link{test_module}}
  }
}
