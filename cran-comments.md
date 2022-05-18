## R CMD check results
There were no ERRORs or WARNINGs.

There were 4 NOTEs:

- > checking CRAN incoming feasibility ... NOTE
  > Maintainer: 'McGrath Justin M. <jmcgrath@illinois.edu>'
  >
  > New submission

  BioCro is a new submission.

- > checking installed package size ... NOTE
  > installed size is  8.3Mb
  > sub-directories of 1Mb or more:
  >   data   3.5Mb
  >   doc    2.3Mb
  >   libs   2.2Mb

  No directories exceed 5 megabytes.

- > checking top-level files ... NOTE
  > Non-standard file/directory found at top level:
  >   'boost_1_71_0'

  Having this directory at top level helps prevent overly long file paths in the
  tarball.

- > checking for GNU extensions in Makefiles ... NOTE
  > GNU make is a SystemRequirements.

  GNU make extensions are used in `src/Makevars` and `src/Makevars.win`.

## Downstream dependencies
There are currently no downstream dependencies for this package.
