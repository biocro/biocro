\name{module_paste}

\alias{module_paste}

\title{Prepend library name to module names}

\description{
  Prepends a library name to a set of module names to create a
  suitably- formatted set of module specifications that can be passed to
  \code{\link{run_biocro}} or other BioCro functions.
}

\usage{module_paste(lib_name, module_names)}

\arguments{
  \item{lib_name}{A string specifying a module library name.}

  \item{module_names}{A vector or list of module name strings.}
}

\details{
  \code{module_paste} is a convenience function for specifying multiple modules
  from the same library; it prepends the library name to each module name,
  preserving the \code{names} and \code{class} of \code{module_names}.

  Note that a simple call to \code{paste0(lib_name, ':', module_names)} will
  produce a similar output with two important differences: (1)
  \code{\link{paste0}} will not preserve names if \code{module_names} has any
  named elements and (2) \code{\link{paste0}} will always return a character
  vector, even if \code{module_names} is a list.
}

\value{
  A vector or list of full module specification strings formatted like
  \code{lib_name:module_name}.
}

\seealso{
  \itemize{
    \item \code{\link{modules}}
    \item \code{\link{run_biocro}}
  }
}

\examples{
# Example: Specifying several modules from the `BioCro` module library.
full_module_names <- module_paste(
  'BioCro',
  list('total_biomass', canopy_photosynthesis = 'c3_canopy')
)

# Compare to the output from `paste0`
full_module_names2 <- paste0(
  'BioCro',
  ':',
  list('total_biomass', canopy_photosynthesis = 'c3_canopy')
)

str(full_module_names)
str(full_module_names2)

}
