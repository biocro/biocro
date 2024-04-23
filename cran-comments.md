## R CMD check results
There were no ERRORs or WARNINGs.

There were 4 NOTEs:

- > checking CRAN incoming feasibility ... NOTE
  > Maintainer: 'McGrath Justin M. <jmcgrath@illinois.edu>'
  >
  > New submission

  BioCro is a new submission.

- > checking C++ specification ... NOTE
  > Specified C++11: please drop specification unless essential

  This specification is required in order to compile the included boost
  libraries on our minimum supported version of R (3.6.0)

- > checking top-level files ... NOTE
  > Non-standard file/directory found at top level:
  >   'inc'

  Having this directory at top level helps prevent overly long file paths in the
  tarball.

- > checking for GNU extensions in Makefiles ... NOTE
  > GNU make is a SystemRequirements.

  GNU make extensions are used in `src/Makevars` and `src/Makevars.win`.

## Downstream dependencies
There are currently no downstream dependencies for this package.
