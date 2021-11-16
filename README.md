## BioCro [![Build Status](https://github.com/ebimodeling/biocro-dev/workflows/R-CMD-check/badge.svg)](https://github.com/ebimodeling/biocro-dev/actions?query=workflow%3AR-CMD-check)
BioCro is a model that predicts plant growth over time given crop-specific parameters and environmental data as input.

It uses models of key physiological and biophysical processes underlying plant growth ([Humphries and Long, 1995]), and has previously been used for predicting biomass yield and leaf area index of switchgrass and miscanthus ([Miguez et al., 2009]).

BioCro has also been integrated into a suite of tools that link the model directly to crop trait and yield data ([LeBauer et al., 2014]). The Predictive Ecosystem Analyzer ([PEcAn](https://github.com/PecanProject/pecan)) couples BioCro to the [Biofuel Ecophysiological Traits and Yields database](https://www.betydb.org).

### An example
The `run_biocro()` function accepts initial values, parameters, climate variables, and sets of modules to run. It returns the results in a data frame.

```r
library(BioCro)
library(lattice)

result = run_biocro(sorghum_initial_values, sorghum_parameters, get_growing_season_climate(weather2005), sorghum_direct_modules, sorghum_differential_modules, sorghum_ode_solver)
xyplot(Stem + Leaf ~ TTc, result, type='l')
```

There are parameters and modules for miscanthus (_Miscanthus_ x _giganteus_), sorghum (_Sorghum bicolor_) and willow (_Saliceae salix_).


### Installation
#### Requirements
- The [R environment](https://cran.r-project.org/) version 3.2.3 or greater.
- On Windows, [Rtools](https://cran.r-project.org/bin/windows/Rtools/) version 3.3 or higher (3.5 is not recommended as of 2019-12-05 since that version is not frozen).
- On Linux, gcc and g++ version 4.9.3 or greater (consult documentation for your distribution for installation instructions).
- On MacOS, Xcode.

#### Installation steps
Download the BioCro source code from GitHub, unzip the file, and install from the either the command line or from within R using one of the following sets of commands.

Do one of the sets of instruction below. These assume that the source files are in a directory named "biocro".

- From the command line
```
cd path_to_unzipped_directory
R CMD INSTALL biocro
```

- Or from within R
```
setwd('path_to_unzipped_directory')
install.packages('biocro', repos=NULL, type='SOURCE')
```

### Making contributions
Please see the [contribution guidelines](documentation/contribution_guidelines.md) before submitting changes.

### Software Documentation

See the [BioCro Documentation Web
Site](https://ebimodeling.github.io/biocro-documentation/).  There
will be found not only the standard package documentation, but also
documentation of the C++ code, including notes on the biological
models used in BioCro and their implementation.  Also included is
documentation for BioCro package developers and maintainers.


## References
- [Humphries S and Long SP][Humphries and Long, 1995] (1995) WIMOVAC - a software package for modeling the dynamics of the plant leaf and canopy photosynthesis. Computer Applications in the Bioscience 11(4):361-371.
- [Miguez FE, Zhu XG, Humphries S, Bollero GA, Long SP][Miguez et al., 2009] (2009) A semimechanistic model predicting the growth and production of the bioenergy crop Miscanthus × giganteus: description, parameterization and validation.  Global Change Biology Bioenergy 1: 282-296.
- [LeBauer D, Wang D, Richter K, Davidson C, Dietze M][LeBauer et al., 2014] (2014) Facilitating feedbacks between field measurements and ecosystem models. Ecological Monographs 83(2): 133-154.
- [Wang D, Jaiswal D, Lebauer DS, Wertin TM, Bollero GA, Leakey ADB, Long SP][Wang et al., 2015] (2015) A physiological and biophysical model of coppice willow (Salix spp.) production yields for the contiguous USA in current and future climate scenarios. Plant, Cell & Environment 38(9), 1850-1865.

[Humphries and Long, 1995]:https://academic.oup.com/bioinformatics/article-abstract/11/4/361/214034/WIMOVAC-a-software-package-for-modelling-the
[Miguez et al., 2009]:http://onlinelibrary.wiley.com/doi/10.1111/j.1757-1707.2009.01019.x/full
[Wang et al., 2015]:documentation/publications/wang2015pbm.pdf
[LeBauer et al., 2014]:https://esajournals.onlinelibrary.wiley.com/doi/full/10.1890/12-0137.1
