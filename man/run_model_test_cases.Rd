\name{run_model_test_cases}

\alias{run_model_test_cases}

\title{Run BioCro model test cases}

\description{
  BioCro models can be tested using test cases, which are sets of known outputs
  that correspond to particular inputs. The \code{run_model_test_cases} function
  runs one or more of these tests.

  Note that \emph{model tests} are distinct from the \emph{module tests}
  described in \code{\link{module_testing}}.
}

\usage{
  run_model_test_cases(model_test_cases)
}

\arguments{
  \item{model_test_cases}{
    A list of module test cases, each of which should be created using
    \code{\link{model_test_case}}.
  }
}

\details{
  The \code{run_model_test_cases} function is a key part of the BioCro model
  testing system. See \code{\link{model_testing}} for more information.

  For each test case, the following checks will be performed:
  \itemize{
    \item The model definition must be valid according to
          \code{\link{validate_dynamical_system_inputs}}.
    \item The model will be run, which should not cause any errors or warnings.
  }

  For each test case where \code{check_outputs} was set to \code{TRUE}, the
  following additional checks comparing the new result to a saved result will be
  performed:
  \itemize{
    \item The new result should have the same number of rows as the old result.
    \item With the exception of any columns in \code{quantities_to_ignore}, all
          columns in the stored result should be included in the new result.
    \item With the exception of any columns in \code{quantities_to_ignore}, all
          columns in the stored result should have the same values in the new
          result (to within the specified tolerance). This check will be made
          using \code{\link{all.equal}} with \code{tolerance} set to
          \code{relative_tolerance}.
  }

  When comparing the values of each column, values will only be checked for
  every Nth row of the new result, where N is the value of \code{row_interval}
  specified when defining the test case.

  For each test case where \code{check_outputs} is \code{TRUE}, the stored
  result should be created using the \code{\link{update_stored_model_results}}
  function.

  If any of the above checks fail for any of the supplied test cases, an error
  will be thrown with a descriptive message.

  Besides the checks above, a warning message will also be sent to the user if
  there are columns in the new result that are not included in the saved result.
}

\value{
  If no issues are found, the function will return \code{TRUE}.
}

\seealso{
  \itemize{
    \item \code{\link{model_testing}}
    \item \code{\link{model_test_case}}
    \item \code{\link{update_stored_model_results}}
    \item \code{\link{compare_model_output}}
  }
}

\examples{
# Define and run a test case for the miscanthus model
miscanthus_test_case <- model_test_case(
    'miscanthus_x_giganteus',
    miscanthus_x_giganteus,
    get_growing_season_climate(weather$'2005'),
    FALSE
)

run_model_test_cases(
  list(
    miscanthus_test_case
  )
)
}
