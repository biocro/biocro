\name{module_case_files}

\alias{module_case_files}

\alias{initialize_csv}
\alias{add_csv_row}
\alias{update_csv_cases}

\title{Define and modify BioCro module test case files}

\description{
  Test cases for testing modules can be stored in files. The functions
  here provide ways to create and update those files.

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
  initialize_csv(
    module_name,
    directory,
    nonstandard_inputs = list(),
    description = "automatically-generated test case",
    overwrite = FALSE
  )

  add_csv_row(module_name, directory, inputs, description)

  update_csv_cases(module_name, directory)
}

\arguments{
  \item{module_name}{
    A string specifying one BioCro module, formatted like
    \code{library_name:local_module_name}, where \code{library_name} is the name
    of a library that contains a module with local name
    \code{local_module_name}; such fully-qualified module names can be formed
    manually or with \code{\link{module_paste}}.
  }

  \item{directory}{
    The directory where module test case files are stored, e.g.
    \code{file.path('tests', 'module_test_cases')}.
  }

  \item{inputs}{
    A list of module inputs, i.e., a list of named numeric elements
    corresponding to the module's input quantities.
  }

  \item{description}{
    A string describing the test case, e.g. \code{"temp above tbase"}. The
    description should be succinct and not contain any newline characters.
  }

  \item{nonstandard_inputs}{
    An optional list of input quantities whose values will override the default
    value of 1.0; see the \code{inputs} entry above.
  }

  \item{overwrite}{
    A logical value indicating whether an existing file should be overwritten.
  }
}

\details{
  Module test case files form a critical component of BioCro's regression
  testing system. For more details, see the help page for
  \code{\link{module_testing}}.

  The \code{initialize_csv} function will evaluate the module for a set of input
  quantities and store the results as a test case \code{csv} file. Typically,
  both of its optional arguments can be omitted. However, some modules produce
  errors when all inputs are set to 1.0. In this case, it would be necessary to
  supply some nonstandard inputs and (possibly) an alternate case description.

  The \code{add_csv_row} function will evaluate the module for a set of input
  quantities, define a test case from the resulting outputs and the description,
  and add it to the module's corresponding \code{csv} file. If no \code{csv}
  file exists, one will be initialized with the new case.

  The \code{update_csv_cases} function will evaluate the module for all input
  values specified in its \code{csv} case file and update the stored values of
  the corresponding outputs. Any output columns not present in the file will be
  added automatically and filled in with the correct values. Although the output
  columns are optional, the description column must exist in the \code{csv}
  file.

  If a module test fails and \code{update_csv_cases} is used to update the test,
  care should be taken to ensure that the new outputs are sensible. This
  function should not be used to blindly ensure that tests pass, since a test
  failure may indicate a real problem with a module.

  Note that \code{update_csv_cases} can be used to batch-initialize test cases.
  To do this, manually create a test case \code{csv} file with the proper name
  that only includes columns for the inputs and the description; now, calling
  \code{update_csv_cases} will automatically fill in the outputs for each case.
  With this method, care must be taken when manually specifying the values of
  the description column; the descriptions must be double quoted, and if they
  contain internal double quotes, those quotes must be doubled. Generally it is
  safest to simply avoid double quotes in the descriptions. (See \code{qmethod}
  in the help file for \code{\link{write.csv}} for more details about quoting.)
}

\value{
  A message indicating whether a file was created, overwritten, or not written.
}

\seealso{
  \itemize{
    \item \code{\link{modules}}
    \item \code{\link{module_paste}}
    \item \code{\link{module_testing}}
    \item \code{\link{test_module_library}}
    \item \code{\link{test_module}}
  }
}

\examples{
# First, we will initialize a test case file for the 'BioCro' library's
# 'thermal_time_linear' module, which will be saved in a temporary directory as
# 'BioCro_thermal_time_linear.csv'. Then, we will add a new case to the file.
# Finally, we will update the file. Note that the call to `update_csv_cases`
# will not actually modify the file unless it is manually edited beforehand to
# change an input or output value.

td <- tempdir()

initialize_csv(
  'BioCro:thermal_time_linear',
  td,
  nonstandard_inputs = list(temp = -1),
  overwrite = TRUE
)

writeLines(readLines(file.path(td, 'BioCro_thermal_time_linear.csv')))

add_csv_row(
  'BioCro:thermal_time_linear',
  td,
  list(doy = 101, sowing_time = 100, tbase = 20, temp = 44),
  'temp above tbase'
)

writeLines(readLines(file.path(td, 'BioCro_thermal_time_linear.csv')))

update_csv_cases('BioCro:thermal_time_linear', td)
}
