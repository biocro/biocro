# The _Build pkgdown site_ action

This action builds the _pkgdown_ documentation for the BioCro package
using the _pkgdown_ configuration file found in this directory.  The
output files are written to the default location, a subdirectory of
the package root called `docs`.

## Preconditions

* The BioCro repository must be checked out to the GitHub workspace

* R must be set up on the GitHub runner

* Package dependencies of the package being documented (BioCro) must
  be installed

* The _pkgdown_ R package must be installed

This action will take care of installing _Pandoc_ if it hasn't been
installed already.  It will also install the _tinytex_ R package along
with a corresponding TinyTex LaTeX installation and any extra LaTeX
packages needed by the vignettes.

## Inputs

### 'package_root'

**Optional** This should be the relative path from the GitHub
  workspace directory to the root directory of the BioCro source tree.
  Default: '.'