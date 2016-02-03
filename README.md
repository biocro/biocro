## BioCro

BioCro is a detailed model that scales from leaf-level photosynthesis to ecosystem level carbon and water balance. 


BioCro is a plant production model adapted from WIMOVAC, relying on the key physiological and biophysical processes underlying plant production (Humphries & Long, 1995). BioCro has previously been used for predicting biomass yield and LAI of switchgrass and  Miscanthus (Miguez et al., 2009). Sugarcane, coppice willow, arundo, and soy modules are under development and will be released following publication.

Recent development has coupled DayCent (Parton et al 1998) to BioCro, expanding the ability of BioCro to predict ecosystem functioning beyond yields, including the production of greenhouse gases and storage of carbon in the soil. BioCro has also been integrated into a suite of tools which link the model directly with crop trait and yield data, streamlining the entire modeling and data collection workflow (LeBauer et al, 2013). The Predictive Ecosystem Analyzer [(PEcAn)](https://github.com/PecanProject/pecan) couples BioCro to the [Biofuel Ecophysiological Traits and Yields database](https://www.betydb.org), enabling model parameterization and optimization, targeted data collection optimized to reduce uncertainty in model predictions, and the management of informatics and HPC computing resources. 


### Inputs:

* species-specific plant physiological traits 
** Photosynthetic Parameters
** Phenological stages
** Biomass partitioning
* soil physical properties:
** (water holding capacity, wilting point, sand, silt, clay, organic matter, rooting depth). 
* meteorological data (daily or hourly) 
** precipitation
** solar radiation
** wind speed
** temperature
** humidity

### Outputs

* Yield, NPP, Biomass (leaf, root, storage)
* Evapotranspiration
* Leaf Area Index
* many more

### Software Documentation

BioCro is an R package that provides a user-friendly interface to the underlying crop simulation model written in C. (A C standalone version is in development).

#### For R users,

To get started using the model, the quickest way to start is

```
library(devtools)
install_github("ebimodeling/biocro")
data(weather05)
head(weather05) # take a look
?weather05 # get info on data

ans <- BioGro(weather05) # uses default parameters
?BioGro
plot(ans)

```

#### For developers,

If you are interested in development please contact one of the package maintainers (see DESCRIPTION file). The development branch is generally open, but kept private to help with the coordination of research and publications.

## References 

Copies of these papers can be found in the [`documentation/publications`](https://github.com/ebimodeling/biocro/tree/master/documentation) folder.

Humphries S and Long SP (1995) WIMOVAC - a software package for modeling the dynamics of the plant leaf and canopy photosynthesis. Computer Applications in the Bioscience 11(4):361-371 [PDF](https://github.com/ebimodeling/biocro/raw/master/documentation/publications/migeuz2009smp.pdf)

Miguez FE, Zhu XG, Humphries S, Bollero GA, Long SP (2009) A semimechanistic model predicting the growth and production of the bioenergy crop Miscanthus × giganteus: description, parameterization and validation.  Global Change Biology Bioenergy 1: 282-296 [PDF]()
