\name{compare_model_output}

\alias{compare_model_output}

\title{Compare new and stored results for a BioCro model test case}

\description{
  BioCro models can be tested using test cases, which are sets of known outputs
  that correspond to particular inputs. The \code{compare_model_output}
  function facilitates manual comparisons between new and stored results.

  Note that \emph{model tests} are distinct from the \emph{module tests}
  described in \code{\link{module_testing}}.
}

\usage{
  compare_model_output(mtc, columns_to_keep = NULL)
}

\arguments{
  \item{mtc}{
    A single module test case, which should be created using
    \code{\link{model_test_case}}.
  }

  \item{columns_to_keep}{
    A vector of column names that should be included in the return value. If
    \code{columns_to_keep} is \code{NULL}, all columns that are in both the
    new and stored result will be included.
  }
}

\details{
  The \code{compare_model_output} function is a key part of the BioCro
  model testing system. See \code{\link{model_testing}} for more information.

  This function will run the model to get a new result, and load the stored
  result associated with the test case. The two data frames will be combined
  using \code{\link{rbind}}, where a new column named \code{version} indicates
  whether each row is from the \code{new} or \code{stored} result.

  It is intended that quantities from the resulting data frame will be plotted
  to visually look for changes in the model output.
}

\value{
  A data frame as described above.
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
    tempdir()
)

update_stored_model_results(miscanthus_test_case)

# Now we can use `compare_model_output` to compare the saved result to a new one
comparison_df <- compare_model_output(miscanthus_test_case)

# This will be a boring example because the new and stored results will be
# exactly the same
lattice::xyplot(
  Leaf + Stem + Root ~ time,
  group = version,
  data = comparison_df,
  type = 'l',
  auto = TRUE,
  grid = TRUE
)
}
