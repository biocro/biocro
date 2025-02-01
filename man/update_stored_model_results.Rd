\name{update_stored_model_results}

\alias{update_stored_model_results}

\title{Updated stored result for a BioCro model test case}

\description{
  BioCro models can be tested using test cases, which are sets of known outputs
  that correspond to particular inputs. The \code{update_stored_model_results}
  function stores the model outputs so they can be used for testing.

  Note that \emph{model tests} are distinct from the \emph{module tests}
  described in \code{\link{module_testing}}.
}

\usage{
  update_stored_model_results(mtc)
}

\arguments{
  \item{mtc}{
    A single module test case, which should be created using
    \code{\link{model_test_case}}.
  }
}

\details{
  The \code{update_stored_model_results} function is a key part of the BioCro
  model testing system. See \code{\link{model_testing}} for more information.

  This function will run the model with the supplied drivers and store the
  results in an appropriately-named CSV file in the specified directory.

  To save space, the values in the result will be rounded to 5 significant
  digits using \code{\link{signif}}. Also, only every Nth row will be retained,
  where N is the value of \code{row_interval} specified in the module test case.

  The saved result created by this function will be retrieved by
  \code{\link{run_model_test_cases}} when checking the test case.
}

\value{
  This function has no return value.
}

\seealso{
  \itemize{
    \item \code{\link{model_testing}}
    \item \code{\link{model_test_case}}
    \item \code{\link{run_model_test_cases}}
    \item \code{\link{compare_model_output}}
  }
}

\examples{
# Define a test case for the miscanthus model and save the model output to a
# temporary directory
miscanthus_test_case <- model_test_case(
    'miscanthus_x_giganteus',
    miscanthus_x_giganteus,
    get_growing_season_climate(weather$'2005'),
    TRUE,
    tempdir(),
    24,
    'soil_evaporation_rate',
    5e-3
)

update_stored_model_results(miscanthus_test_case)

# The output file's name will be based on the test case description
fpath <- file.path(tempdir(), 'miscanthus_x_giganteus_simulation.csv')

# Check that the output file exists and then load it
if (file.exists(fpath)) {
  saved_result <- read.csv(fpath)
  str(saved_result)
}
}
