## R CMD check results
There were no ERRORs or WARNINGs.

There were 2 NOTEs:

- > checking C++ specification ... NOTE
  > Specified C++11: please drop specification unless essential

  This specification is required in order to compile the included boost
  libraries on our minimum supported version of R (3.6.0)

- > checking for GNU extensions in Makefiles ... NOTE
  > GNU make is a SystemRequirements.

  GNU make extensions are used in `src/Makevars` and `src/Makevars.win`.

## Downstream dependencies
There are currently no downstream dependencies for this package.
