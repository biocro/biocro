## BioCro

<!-- badges: start -->
[![CRAN status](https://www.r-pkg.org/badges/version/BioCro)](https://CRAN.R-project.org/package=BioCro)
[![](https://cranlogs.r-pkg.org/badges/grand-total/BioCro)](https://CRAN.R-project.org/package=BioCro)
[![Build Status](https://github.com/biocro/biocro/workflows/R-CMD-check/badge.svg)](https://github.com/biocro/biocro/actions?query=workflow%3AR-CMD-check)
<!-- badges: end -->

BioCro is a model that predicts plant growth over time given crop-specific
parameters and environmental data as input.

It uses models of key physiological and biophysical processes underlying plant
growth ([Humphries and Long, 1995]), and has previously been used for predicting
biomass yield and leaf area index of switchgrass and miscanthus
([Miguez et al., 2009]). In 2022, BioCro was reorganized to take a truly modular
approach to modeling ([Lochocki et al., 2022]) and a new soybean model was
developed ([Matthews et al., 2022]).

BioCro has also been integrated into a suite of tools that link the model
directly to crop trait and yield data ([LeBauer et al., 2013]). The Predictive
Ecosystem Analyzer ([PEcAn](https://github.com/PecanProject/pecan)) couples
BioCro to the [Biofuel Ecophysiological Traits and Yields
database](https://github.com/PecanProject/bety).

See [References](#references) below for a full list of scientific publications
using the BioCro framework.

### An example

The `run_biocro()` function accepts initial values, parameters, climate
variables, and sets of modules to run. It returns the results in a data frame.

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

There are parameters and modules for soybean (_Glycine max_), miscanthus
(_Miscanthus_ x _giganteus_), and willow (_Saliceae salix_).

### Simple installation

BioCro is available on CRAN, so it can be installed from R as follows:

```r
install.packages('BioCro')
```

### Installation from source

#### Requirements

- The [R environment](https://cran.r-project.org/) version 3.6.0 or greater.
- On Windows, a version of
  [Rtools](https://cran.r-project.org/bin/windows/Rtools/) appropriate for your
  version of R.
- On Linux, gcc and g++ version 4.9.3 or greater (consult documentation for your
  distribution for installation instructions).
- On MacOS, Xcode.

#### Installation steps

1. Obtain a local copy of this repository, making sure to include the Git
   submodule code. This can be accomplished using either of two methods:
   1. If you are new to Git, the easiest way to get a local copy is to install
      GitHub Desktop and use the "Open with GitHub Desktop" option in the "Code"
      dropdown on the GitHub page for this repository.
   2. Alternatively, clone the repository using Git on the command
      line in the usual fashion by running
      `git clone https://github.com/biocro/biocro` The repository
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

Please see the contribution guidelines before submitting changes.
These may be found in Chapter One of the _Developer's Manual_ on the
[public BioCro Documentation web site](https://biocro.org).

### Software Documentation

See the [public BioCro Documentation web
site](https://biocro.org).  There will be found not only the
usual package documentation, but also documentation of the C++ code,
including notes on the biological models used in BioCro and their
implementation.  Also included is documentation for BioCro package
developers and maintainers.

There is also a separate [page that documents all of the quantities
used by the Standard BioCro Module
Library](https://biocro.org/BioCro-documentation/quantity_docs/quantities.html).

### License

The `BioCro` R package is licensed under the MIT license, while the BioCro C++
framework is licensed under version 3 or greater of the GNU Lesser General
Public License (LGPL). This scheme allows people to freely develop models for
any use (public or private) under the MIT license, but any changes to the
framework that assembles and solves models must make source code changes
available to all users under the LGPL. See `LICENSE.note` for more details.

### Citing BioCro

Appropriate references for BioCro are
[Miguez et al. (2009)][Miguez et al., 2009] and
[Lochocki et al. (2022)][Lochocki et al., 2022], with details given below. To
cite the package itself, use `citation('BioCro')` in R to get details for the
current installed version.

### References

- [Humphries S and Long SP][Humphries and Long, 1995] (1995) WIMOVAC: a software package for modelling the dynamics of plant leaf and canopy photosynthesis. Computer Applications in the Biosciences 11(4): 361-371.
- [Miguez FE, Zhu XG, Humphries S, Bollero GA, Long SP][Miguez et al., 2009] (2009) A semimechanistic model predicting the growth and production of the bioenergy crop Miscanthus × giganteus: description, parameterization and validation.  Global Change Biology Bioenergy 1: 282-296.
- [LeBauer D, Wang D, Richter K, Davidson C, Dietze M][LeBauer et al., 2013] (2013) Facilitating feedbacks between field measurements and ecosystem models. Ecological Monographs 83(2): 133-154.
- [Wang D, Jaiswal D, Lebauer DS, Wertin TM, Bollero GA, Leakey ADB, Long SP][Wang et al., 2015] (2015) A physiological and biophysical model of coppice willow (Salix spp.) production yields for the contiguous USA in current and future climate scenarios. Plant, Cell & Environment 38(9): 1850-1865.
- [Larsen S, Jaiswal D, Bentsen NS, Wang D, Long SP][Larsen et al., 2015] (2015) Comparing predicted yield and yield stability of willow and Miscanthus across Denmark. GCB Bioenergy 8(6): 1061-1070.
- [Jaiswal D, de Souza AP, Larsen S, LeBauer D, Miguez FE, Sparovek G, Bollero G, Buckeridge MS, Long SP][Jaiswal et al., 2017] (2017) Brazilian sugarcane ethanol as an expandable green alternative to crude oil use. Nature Climate Change 7(11): 788-792.
- [Lochocki EB, Rohde S, Jaiswal D, Matthews ML, Miguez FE, Long SP, McGrath JM][Lochocki et al., 2022] (2022) BioCro II: a software package for modular crop growth simulations. _in silico_ Plants 4(1): diac003.
- [Matthews ML, Marshall-Colón A, McGrath JM, Lochocki EB, Long SP][Matthews et al., 2022] (2022) Soybean-BioCro: a semi-mechanistic model of soybean growth. _in silico_ Plants 4(1): diab032.
- [He Y, Jaiswal D, Liang XZ, Sun C, Long SP][He et al., 2022] (2022) Perennial biomass crops on marginal land improve both regional climate and agricultural productivity. GCB Bioenergy 14(5): 558-571.
- [He Y, Matthews ML][He & Matthews, 2023] (2023) Seasonal climate conditions impact the effectiveness of improving photosynthesis to increase soybean yield. Field Crops Research 296: 108907.
- [Holland B, Matthews ML, Bota P, Sweetlove LJ, Long SP, diCenzo GC][Holland et al., 2023] (2023) A genome-scale metabolic reconstruction of soybean and Bradyrhizobium diazoefficiens reveals the cost–benefit of nitrogen fixation. New Phytologist 240(2): 744-756.


[Humphries and Long, 1995]:https://doi.org/10.1093/bioinformatics/11.4.361
[Miguez et al., 2009]:https://doi.org/10.1111/j.1757-1707.2009.01019.x
[LeBauer et al., 2013]:https://doi.org/10.1890/12-0137.1
[Wang et al., 2015]:https://doi.org/10.1111/pce.12556
[Larsen et al., 2015]:https://doi.org/10.1111/gcbb.12318
[Jaiswal et al., 2017]:https://doi.org/10.1038/nclimate3410
[Lochocki et al., 2022]:https://doi.org/10.1093/insilicoplants/diac003
[Matthews et al., 2022]:https://doi.org/10.1093/insilicoplants/diab032
[He et al., 2022]:https://doi.org/10.1111/gcbb.12937
[He & Matthews, 2023]:https://doi.org/10.1016/j.fcr.2023.108907
[Holland et al., 2023]:https://doi.org/10.1111/nph.19203
