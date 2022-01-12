\name{module_creators}

\alias{module_creators}

\title{Create instances of modules}

\description{Creates pointers to module wrapper objects}

\usage{module_creators(module_names)}

\arguments{
  \item{module_names}{A vector of module names}
}

\details{
  This function is used internally by several other BioCro functions, where its
  purpose is to create instances of module wrapper pointers using BioCro's
  module library and return pointers to those wrappers. In turn, module wrappers
  can be used to obtain information about a module's inputs, outputs, and other
  properties, and can also be used to create a module instance. The \code{See
  Also} section contains a list of functions that directly rely on
  \code{module_creators}.

  The R documentation for \code{externalptr} objects is sparse, but some
  information can be found
  \href{https://stat.ethz.ch/R-manual/R-devel/library/methods/html/BasicClasses.html}{here}.
}

\value{
  A vector of R \code{externalptr} objects that each point to a
  \code{module_creator} C++ object
}

\seealso{
  \itemize{
    \item \code{\link{module_info}}
    \item \code{\link{evaluate_module}}
  }
}

\examples{
# Example: creating a vector of pointers to several module wrappers representing
# three options for thermal time calculations
module_pointers <- module_creators(c(
  'thermal_time_linear',
  'thermal_time_bilinear',
  'thermal_time_trilinear'
))
}
