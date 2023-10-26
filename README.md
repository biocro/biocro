## BioCro [![Build Status](https://github.com/ebimodeling/biocro/workflows/R-CMD-check/badge.svg)](https://github.com/ebimodeling/biocro/actions?query=workflow%3AR-CMD-check)
BioCro is a model that predicts plant growth over time given crop-specific parameters and environmental data as input.

It uses models of key physiological and biophysical processes underlying plant growth ([Humphries and Long, 1995]), and has previously been used for predicting biomass yield and leaf area index of switchgrass and miscanthus ([Miguez et al., 2009]).

BioCro has also been integrated into a suite of tools that link the model directly to crop trait and yield data ([LeBauer et al., 2014]). The Predictive Ecosystem Analyzer ([PEcAn](https://github.com/PecanProject/pecan)) couples BioCro to the [Biofuel Ecophysiological Traits and Yields database](https://www.betydb.org).

### An example
The `run_biocro()` function accepts initial values, parameters, climate variables, and sets of modules to run. It returns the results in a data frame.

```r
library(BioCro)
library(lattice)

result <- with(soybean, {run_biocro(
  initial_values,
  parameters,
  soybean_weather$'2002',
  direct_modules,
  differential_modules,
  ode_solver
)})

xyplot(Stem + Leaf ~ TTc, data = result, type='l', auto = TRUE)
```

There are parameters and modules for soybean (_Glycine max_), miscanthus (_Miscanthus_ x _giganteus_), and willow (_Saliceae salix_).


### Installation
#### Requirements
- The [R environment](https://cran.r-project.org/) version 3.5.0 or greater.
- On Windows, a version of [Rtools](https://cran.r-project.org/bin/windows/Rtools/) appropriate for your version of R.
- On Linux, gcc and g++ version 4.9.3 or greater (consult documentation for your distribution for installation instructions).
- On MacOS, Xcode.

#### Installation steps
1. Obtain a local copy of this repository, making sure to include the Git
   submodule code. This can be accomplished using either of two methods:
   1. If you are new to Git, the easiest way to get a local copy is to install
      GitHub Desktop and use the "Open with GitHub Desktop" option in the "Code"
      dropdown on the GitHub page for this repository.
   2. Alternatively, clone the repository using Git on the command
      line in the usual fashion by running
      `git clone <https://github.com/ebimodeling/biocro-dev>` The repository
      contains a Git submodule, so you will need to take the additional step of
      running `git submodule update --init` to obtain it.
2. Install the BioCro R package using one of the following sets of comands.
   These assume that the source files are in a directory named "biocro" residing
   in a parent directory located at "path_to_source_code_parent_directory".
   1. To install from the command line:
      ```
      cd path_to_source_code_parent_directory
      R CMD INSTALL biocro
      ```
   2. To install from within R:
      ```
      setwd('path_to_source_code_parent_directory')
      install.packages('biocro', repos=NULL, type='SOURCE')
      ```

### Making contributions

Please see the [contribution
guidelines](https://ebimodeling.github.io/biocro-dev-documentation/master/bookdown/contributing-to-biocro.html)
before submitting changes.

### Software Documentation

See the [BioCro Documentation Web
Site](https://ebimodeling.github.io/biocro-dev-documentation/master/pkgdown/index.html).  There
will be found not only the standard package documentation, but also
documentation of the C++ code, including notes on the biological
models used in BioCro and their implementation.  Also included is
documentation for BioCro package developers and maintainers.

There is also a separate [page that documents all of the quantities
used by the Standard BioCro Module
Library](https://ebimodeling.github.io/biocro-dev-documentation/quantity_docs/quantities.html).

### License

The `BioCro` R package is licensed under the MIT license, while the BioCro C++
framework is licensed under version 3 or greater of the GNU Lesser General
Public License (LGPL). This scheme allows people to freely develop models for
any use (public or private) under the MIT license, but any changes to the
framework that assembles and solves models must make source code changes
available to all users under the LGPL. See `LICENSE.note` for more details.

## References
- [Humphries S and Long SP][Humphries and Long, 1995] (1995) WIMOVAC - a software package for modeling the dynamics of the plant leaf and canopy photosynthesis. Computer Applications in the Bioscience 11(4):361-371.
- [Miguez FE, Zhu XG, Humphries S, Bollero GA, Long SP][Miguez et al., 2009] (2009) A semimechanistic model predicting the growth and production of the bioenergy crop Miscanthus × giganteus: description, parameterization and validation.  Global Change Biology Bioenergy 1: 282-296.
- [LeBauer D, Wang D, Richter K, Davidson C, Dietze M][LeBauer et al., 2014] (2014) Facilitating feedbacks between field measurements and ecosystem models. Ecological Monographs 83(2): 133-154.
- [Wang D, Jaiswal D, Lebauer DS, Wertin TM, Bollero GA, Leakey ADB, Long SP][Wang et al., 2015] (2015) A physiological and biophysical model of coppice willow (Salix spp.) production yields for the contiguous USA in current and future climate scenarios. Plant, Cell & Environment 38(9), 1850-1865.

[Humphries and Long, 1995]:https://doi.org/10.1093/bioinformatics/11.4.361
[Miguez et al., 2009]:https://doi.org/10.1111/j.1757-1707.2009.01019.x
[LeBauer et al., 2014]:https://doi.org/10.1890/12-0137.1
[Wang et al., 2015]:https://doi.org/10.1111/pce.12556
