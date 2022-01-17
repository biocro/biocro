\name{module_testing}

\alias{test_module}
\alias{case}
\alias{cases_from_csv}
\alias{initialize_csv}
\alias{add_csv_row}
\alias{update_csv_cases}

\title{BioCro module testing functions}

\description{
  \code{test_module} runs one test case for a module, throwing an error if its
  output does not match the expected value.

  \code{case} helps define test cases for module testing by combining the
  required elements into a list with the correct names as required by
  \code{test_module}.

  \code{cases_from_csv} helps define test cases for module testing by creating a
  list of test cases from a \code{csv} file.

  \code{initialize_csv} helps define test cases for module testing by
  initializing the \code{csv} file for one module based on either a set of
  default input values or user-supplied ones.

  \code{add_csv_row} helps define test cases for module testing by adding one
  test case to a module's \code{csv} file based on the user-supplied inputs and
  description.

  \code{update_csv_cases} helps define cases for module testing by updating the
  expected output values for each case stored in a module's csv file.
}

\usage{
  test_module(module_name, case_to_test)

  case(inputs, expected_outputs, description)

  cases_from_csv(module_name, directory)

  initialize_csv(
    module_name,
    directory,
    nonstandard_inputs = list(),
    nonstandard_description = ""
  )

  add_csv_row(module_name, directory, inputs, description)

  update_csv_cases(module_name, directory)
}

\arguments{
  \item{module_name}{
    A string representing the name of one BioCro module
  }

  \item{case_to_test}{
    A list with three named elements that describe a module test case:
    \itemize{
      \item \code{inputs}: a list of module inputs, i.e., a list of named
            numeric elements corresponding to the module's input quantities

      \item \code{expected_outputs}: a list of expected module outputs, i.e., a
            list of named numeric elements corresponding to the expected values
            of the module's output quantities

      \item \code{description}: a string describing the test case, e.g. "temp
            below tbase". The description should be succinct and not contain any
            newline characters. If the description contains a comma, it must be
            wrapped in quotes. This is not advised.
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

  \item{nonstandard_inputs}{
    An optional list of input quantities to be passed to the module in order to
    define the test case. If nonstandard inputs are not specified, all inputs
    will be set to 1 by default.
  }

  \item{nonstandard_description}{
    An optional string describing the case; see the \code{description} entry in
    \code{test_case} above. If a nonstandard case description is not specified,
    the description will be set to "automatically-generated test case" by
    default.
  }
}

\details{
  Together, \code{test_module}, \code{case}, \code{cases_from_csv},
  \code{initialize_csv}, \code{add_csv_row}, and \code{update_csv_cases} form a
  simple and convenient system for defining and running module test cases. Such
  tests form a critical component of BioCro's regression testing system, and
  test cases should be defined for all BioCro modules. These functions are not
  required to use the BioCro package, but are critical to understand when
  creating or modifying modules.

  Test cases are stored as \code{csv} files, where test cases for a module with
  name \code{module_name} must be stored in \code{module_name.csv}. The first
  row of a test case file must be the quantity names, the second row must be
  the quantity types (`input` or `output`), and the remaining rows must each
  specify input quantity values along with the expected output values they
  should produce. There must also be a \code{description} column containing
  short descriptions of the test cases. These formatting requirements will
  automatically be satisfied for any test case file produced by
  \code{initialize_csv} or modified by \code{add_csv_row} or
  \code{update_csv_cases}. Such files can be read from R using
  \code{cases_from_csv}, and the resulting case objects can be passed to
  \code{test_module}.

  The \code{initialize_csv} function will evaluate the module for a set of input
  quantities and store the results as a test case \code{csv} file as described
  above. Typically, both of its optional arguments can be omitted. However, some
  modules produce errors when all inputs are set to 1. In this case, it would be
  necessary to supply nonstandard inputs and (possibly) an alternate case
  description.

  The \code{add_csv_row} function will evaluate the module for a set of input
  quantities, define a test case from the resulting outputs and the description,
  and add it to the module's corresponding \code{csv} file.

  The \code{update_csv_cases} function will evaluate the module for all input
  values specified in its \code{csv} case file and update the stored values of
  the corresponding outputs. Note that it can be used to batch-initialize test
  cases. To do this, manually create a test case \code{csv} file with the proper
  name that only includes columns for the inputs and the description; now,
  calling \code{update_csv_cases} will automatically fill in the outputs for
  each case.

  Although it is possible, directly editing the case files is not recommended
  since \code{initialize_csv}, \code{add_csv_row}, and \code{update_csv_cases}
  are easier to use. There are several exceptions to this suggestion: (1) when a
  case must be deleted (2) when a module input must be added or removed, and (3)
  during the initialization of a test file, where a user may wish to batch
  initialized using \code{update_csv_cases} as described above.

  Case files can easily be viewed using Excel or other spreadsheet viewers, and
  are also nicely formatted when viewed on the GitHub website for the
  repository.
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

\seealso{\code{\link{modules}}}

\examples{
# Example: defining an individual test case and running it
test_module(
  'thermal_time_linear',
  case(
    list(time = 101, sowing_time = 100, tbase = 20, temp = 44),
    list(TTc = 1.0),
    'temp above tbase'
  )
)
}
