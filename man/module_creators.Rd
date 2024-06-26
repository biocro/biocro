\name{module_creators}

\alias{module_creators}

\title{Create instances of modules}

\description{Creates pointers to module wrapper objects}

\usage{
  module_creators(module_names)
}

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

  Although the description of \code{externalptr} objects is sparse, they are
  briefly mentioned in the R documentation: \code{\link{externalptr-class}}.

  This function should not be used directly, and each module library package
  must have its own version. For these reasons, this function is not exported to
  the package namespace and can only be accessed using the package name via the
  \code{\link{:::}} operator.
}

\value{
  A vector of R \code{externalptr} objects that each point to a
  \code{module_creator} C++ object
}

\seealso{
  \itemize{
    \item \code{\link{run_biocro}}
    \item \code{\link{module_info}}
    \item \code{\link{evaluate_module}}
  }
}
