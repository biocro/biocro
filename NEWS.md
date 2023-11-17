<!--
This file should document all significant changes brought about by each new
release.

All changes related to a particular release should be collected under a heading
specifying the version number of that release, such as
"# CHANGES IN BioCro VERSION 2.0.0". The individual changes should be listed as
bullet points and categorized under "## MAJOR CHANGES", "## MINOR CHANGES",
or "## BUG FIXES" following the major.minor.patch structure of semantic
versioning. When applicable, entries should include direct links to the relevant
pull requests.

To facilitate this, when a feature on a feature branch is completed and a pull
request is being prepared, a new section should be added at the top of this file
under the heading "# UNRELEASED"; it should list all the important changes made
on the feature branch.

Then, when it comes time to merge the feature branch into `develop`, the new
"# UNRELEASED" section is transferred into the `develop` branch's version of
NEWS.md, or, if the `develop` branch already has an "# UNRELEASED" section in
its version of NEWS.md, the feature branch's "# UNRELEASED" section will be
integrated into the one on the `develop` branch. (This process of integrating
the two "# UNRELEASED" sections will likely be part of resolving an inevitable
merge conflict.)

Finally, when a new release is made, "# UNRELEASED" should be replaced by a
heading with the new version number, such as
"# CHANGES IN BioCro VERSION 2.0.0". This section will combine the draft release
notes for all features that have been added since the previous release.

In the case of a hotfix, a short section headed by the new release number should
be directly added to this file to describe the related changes.
-->

# UNRELEASED

- This version adds a description of the BioCro git branching model to
  `contribution_guidelines.Rmd` and clarifies the process of updating `NEWS.md`.

- Replaced instances of `sprintf` in boost files with `snprintf` to avoid
  `R CMD check` warnings

# CHANGES IN BioCro VERSION 3.0.2

## MINOR CHANGES

- This version adds several missing references to the main README.

# CHANGES IN BioCro VERSION 3.0.1

## MINOR CHANGES

- This version pertains only to the GitHub documentation workflow.  It
  changes the publication location to the
  `biocro/BioCro-documentation` repository, and it changes the
  triggers of the workflow so that automatic publication happens when
  a new release is published.  Additionally, a symlink is created to
  link the URL
  https://biocro.github.io/BioCro-documentation/latest/pkgdown/ to
  https://biocro.github.io/BioCro-documentation/<tag name>/pkgdown/,
  where <tag name> is the tag name for the new release.

# CHANGES IN BioCro VERSION 3.0.0

## MAJOR CHANGES

- This version introduces the concept of distinct module libraries, allowing
  users to develop modules in private and to create collections of related
  modules. There is an associated syntax change, where modules must now be
  specified using _fully-qualified names_ that include a module library name and
  the local name of a module within that library; for example, the module that
  was previously known as `thermal_time_linear` must now be referred to as
  `BioCro:thermal_time_linear`.

- Any R package representing a BioCro module library must now have four
  non-exported functions related to accessing its modules: `get_all_modules`,
  `get_all_quantities`, `module_creators`, and `framework_version`. When a
  fully-qualified module name such as `library_name:local_module_name` is passed
  to a function such as `module_info`, an internal call to
  `library_name:::module_creators(module_name)` will be made to retrieve a
  pointer to a module. Hence, `library_name` must be the same as the module
  library package name. This is a required part of a method for passing C
  objects from a module library to the core BioCro framework via R; the full
  details are not discussed here.

- The code in the `src` directory has been reorganized to reflect the division
  between framework code, module code, and "R-to-C" code discussed in the
  manuscript; now, `src/framework` contains the core C++ code,
  `src/module_library` contains the module code, and the "R-to-C" code can be
  found directly in `src`.

- The C++ framework code in `src/framework` has been moved to a separate
  repository and licensed under the GNU LGPL; it is included in the BioCro R
  package repository as a Git submodule. This allows us to use a permissive
  license for the BioCro R package while still protecting the code that
  assembles and solves models. Associated with this change, the BioCro R package
  is now licensed under the MIT license. See `LICENSE.note` for details.

## MINOR CHANGES

- The `soil_type_selector` module has been removed and replaced with a data
  object called `soil_parameters`. For crop models that previously set the
  `soil_type_indicator` to `6` to choose the soil parameter values, this
  parameter has been replaced with the values from `soil_parameters$clay_loam`.

- Crop model definitions are now stored as single lists rather than as multiple
  objects; for example, `soybean_parameters` and `soybean_initial_values` are
  now stored as two elements of the `soybean` list: `soybean$parameters` and
  `soybean$initial_values`.

- The function returned by `partial_run_biocro` can now properly respond to
  vectors and lists of named elements.

- Module testing functions have been added to the package namespace.

- Many small improvements have been made to the documentation and the module
  code in `src/module_library`; these changes are too numerous to list here.

## BUG FIXES

- The elements of the `arg_names` input to `partial_run_biocro` can now be in
  any order; previously, they were required to be supplied in the same order as
  the appear in the other inputs to `partial_run_biocro`, for example, the names
  of any initial values were required to come before the names of any
  parameters. If the arguments were supplied in the wrong order, then the inputs
  to the function returned by `partial_run_biocro` would be interpreted in the
  wrong order.

- A bug that sometimes caused the last time point of a simulation to contain NaN
  for all quantities has been fixed; this was related to an out-of-bounds error
  when accessing vector elements in the C++ function
  `dynamical_system::update_drivers`.

# CHANGES IN BioCro VERSION 2.0.0

## MAJOR CHANGES

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
  change log for this version and previous versions is not available in this
  document.
