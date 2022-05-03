# CHANGES IN BioCro VERSION 3.0.0

## NEW FEATURES

- This version introduces the concept of distinct module libraries, allowing
  users to develop modules in private and to create collections of related
  modules. There is an associated syntax change, where modules must now be
  specified using _fully-qualified names_ that include a module library name and
  the local name of a module within that library; for example, the module that
  was previously known as `thermal_time_linear` must now be referred to as
  `BioCro:thermal_time_linear`.

## MAJOR CHANGES

- Any R package representing a BioCro module library must now have three
  non-exported functions accessing its modules: `get_all_modules`,
  `get_all_quantities`, and `module_creators`. When a fully-qualified module
  name like `library_name:local_module_name` is passed to a function like
  `module_info`, an internal call to
  `library_name:::module_creators(module_name)` will be made to retrieve a
  pointer to a module. Here, `library_name` must be the same as the package
  name. This is a required part of a method for passing C objects from a module
  library to the core BioCro framework via R; the full details are not discussed
  here.

- The code in the `src` directory has been reorganized to reflect the division
  between framework code, module code, and "R-to-C" code discussed in the
  manuscript; now, `src/framework` contains the core C++ code,
  `src/module_library` contains the module code, and the "R-to-C" code can be
  found directly in `src`.

## MINOR CHANGES

- The `soil_type_selector` module has been removed and replaced with a data
  object called `soil_parameters`. For crop models that previously set the
  `soil_type_indicator` to `6` to choose the soil parameter values, this
  parameter has been replaced with the values from `soil_parameters$clay_loam`.

- Crop model definitions are now stored as single lists rather than as multiple
  objects; for example, `soybean_parameters` and `soybean_initial_values` are
  now stored as two elements of the `soybean` list: `soybean$parameters` and
  `soybean$initial_values`.

# CHANGES IN BioCro VERSION 2.0.0

## NEW FEATURES

- This version is a major update to the design of BioCro. In this version,
  subsets of a model are called _modules_. The design attempts to meet the
  following goals:
  - Make it easier to reuse modules between species, such as the C3
    photosynthesis modules.
  - Make it easier to swap related modules for comparison, for example,
    comparing the Farquhar-von-Caemmerer-Berry model to a radiation use
    efficiency model.
  - Simplify parameter optimization and sensitivity analysis by providing an
    interface readily useable by common optimizers and similar functions.

- More details can be found in the peer-reviewed publication in _in silico_
  Plants [Lochocki et al., 2022](https://doi.org/10.1093/insilicoplants/diac003)
  and in the vignettes included with the package:
  - _A Practical Guide to BioCro_
  - _Quantitative Comparison Between Two Photosynthesis Models_
  - _An Introduction to BioCro for Those Who Want to Add Models_

  PDF versions of these vignettes corresponding to the latest version of BioCro
  can be obtained online at the
  [BioCro vignette PDF website](https://ebimodeling.github.io/biocro-documentation/docs/articles/pdf_vignette_index.html).

# BioCro VERSION 0.951

- This is the last release of the original version of BioCro, which was first
  described in [Miguez et al., 2009](https://doi.org/10.1111/j.1757-1707.2009.01019.x)
  and updated over the years to add more crop models and R functions. A full
  change log for this version and previous versions is not available.
