## BioCro [![Build Status](https://magnum.travis-ci.com/ebimodeling/biocro-dev.svg?token=tjzJV6N6JPssJ6yfYPqH&branch=master)](https://magnum.travis-ci.com/ebimodeling/biocro-dev)

BioCro is a model that predicts plant growth over time given climate as input.


BioCro was adapted from WIMOVAC and models key physiological and biophysical processes underlying plant growth ([Humphries & Long, 1995]). BioCro has previously been used for predicting biomass yield and leaf area index of switchgrass and miscanthus ([Miguez et al., 2009]).

BioCro has also been integrated into a suite of tools which link the model directly with crop trait and yield data, streamlining the entire modeling and data collection workflow (LeBauer et al, 2013). The Predictive Ecosystem Analyzer [(PEcAn)](https://github.com/PecanProject/pecan) couples BioCro to the [Biofuel Ecophysiological Traits and Yields database](https://www.betydb.org), enabling model parameterization and optimization, targeted data collection optimized to reduce uncertainty in model predictions, and the management of informatics and HPC computing resources. 

### Inputs
* species-specific plant physiological traits
    * Photosynthetic parameters
    * Phenological stages
    * Biomass partitioning
* soil physical properties
    * Wwater holding capacity
    * wilting point
    * sand
    * silt
    * clay
    * organic matter
    * rooting depth 
* meteorological data (daily or hourly) 
    * precipitation
    * solar radiation
    * wind speed
    * temperature
    * humidity

### Outputs
* Yield
* Net primary production
* Biomass (leaf, root, storage)
* Evapotranspiration
* Leaf area index
* many more

### Software Documentation
* [R package][http://ebimodeling.github.io/biocro/documentation/r/]
* [C library][http://ebimodeling.github.io/biocro/documentation/doxygen/]

## References 
* Humphries S and Long SP (1995) WIMOVAC - a software package for modeling the dynamics of the plant leaf and canopy photosynthesis. Computer Applications in the Bioscience 11(4):361-371

* Miguez FE, Zhu XG, Humphries S, Bollero GA, Long SP (2009) A semimechanistic model predicting the growth and production of the bioenergy crop Miscanthus × giganteus: description, parameterization and validation.  Global Change Biology Bioenergy 1: 282-296

[Humphries & Long, 1995][https://academic.oup.com/bioinformatics/article-abstract/11/4/361/214034/WIMOVAC-a-software-package-for-modelling-the]
[Miguez et al., 2009][http://onlinelibrary.wiley.com/doi/10.1111/j.1757-1707.2009.01019.x/full]
