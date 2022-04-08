\name{module_testing}

\alias{module_testing}

\title{The BioCro module testing system}

\description{
  BioCro provides several functions for defining, modifying, and running module
  test cases. These functions together allow module developers to easily create
  regression tests that ensure the modules continue to function correctly.
}

\details{
  Together, \code{\link{test_module_library}}, \code{\link{test_module}},
  \code{\link{case}}, \code{\link{cases_from_csv}},
  \code{\link{initialize_csv}}, \code{\link{add_csv_row}}, and
  \code{\link{update_csv_cases}} form a simple and convenient system for
  defining and running module test cases. Such tests form a critical component
  of BioCro's regression testing system, and test cases should be defined for
  all BioCro modules in all BioCro module libraries. These functions are not
  required to use the BioCro package, but are critical to understand when
  creating or modifying modules.

  A module test case consists of a set of module inputs, a set of module
  outputs, and a short description of the case. To run the test, the inputs are
  passed to the module, and then the calculated outputs are compared to the
  expected ones. If the outputs match, the test is passed; otherwise, it fails.
  This operation is handled by the \code{\link{test_module}} function.

  For simple on-the-fly testing, it is possible to define a test case using the
  \code{\link{case}} function and run it using \code{\link{test_module}}.
  However, a more robust method is available to facilitate regression testing,
  where module test cases are stored in suitably-formatted \code{csv} files,
  allowing multiple test cases to be defined for each module and easily checked
  afterwards. If test case files for each module in a module library are stored
  in a single directory, all the test cases can be checked with one call to
  \code{\link{test_module_library}}.

  In this system, test cases for a module with fully-qualified name
  \code{module_name} must be stored in \code{module_name.csv},
  where the colon in the module name has been replaced by an underscore; for
  example, the module named \code{BioCro:total_biomass} would be associated with
  \code{BioCro_total_biomass.csv}. The first row of a test case file must be the
  quantity types (\code{input} or \code{output}), the second row must be the
  quantity names, and the remaining rows must each specify input quantity values
  along with the expected output values they should produce. There must also be
  a \code{description} column (with \code{description} in the first row)
  containing short descriptions of the test cases. These formatting requirements
  will automatically be satisfied for any test case file produced by
  \code{\link{initialize_csv}} or modified by \code{\link{add_csv_row}} or
  \code{\link{update_csv_cases}}. Such files can be read from R using
  \code{\link{cases_from_csv}}, and the resulting case objects can be passed to
  \code{\link{test_module}}.

  Although it is possible, directly editing the case files is not recommended
  since \code{\link{initialize_csv}}, \code{\link{add_csv_row}}, and
  \code{\link{update_csv_cases}} are easier to use. There are several exceptions
  to this suggestion: (1) when a case must be deleted, (2) when a module input
  must be added or removed, and (3) during the initialization of a test file,
  where a user may wish to batch-initialize using \code{\link{update_csv_cases}}
  (see its documentation for an explanation of batch-initialization).

  Case files can easily be viewed using Excel or other spreadsheet viewers, and
  are also nicely formatted when viewed on the GitHub website for the
  repository.

  Examples of module test case files can be found in the
  \code{tests/module_test_cases} directory, while code that uses the
  \code{\link[testthat]{testthat}} package to automatically run all the defined
  test cases for the standard BioCro module library via
  \code{\link{test_module_library}} can be found in the
  \code{tests/testthat/test.Modules.R} file.
}

\seealso{
  \itemize{
    \item \code{\link{modules}}
    \item \code{\link{module_case_files}}
    \item \code{\link{test_module_library}}
    \item \code{\link{test_module}}
  }
}
