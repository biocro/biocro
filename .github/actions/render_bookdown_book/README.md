# The _Render Bookdown Book_ action

This action builds the _Bookdown_ documentation for the BioCro package.
The output files are written to the default location, the directory
`bookdown/_book` relative to the package root directory.

## Preconditions

* The BioCro repository must be checked out to the GitHub workspace

* R must be set up on the GitHub runner

* Package dependencies of the package being documented (BioCro) must
  be installed

* The _pkgdown_ R package must be installed

This action will take care of installing _Pandoc_ if it hasn't been
installed already.

## Inputs

### 'package_root'

This should be the relative path from the GitHub workspace directory
to the root directory of the BioCro source tree.  Default: '.'