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

# CHANGES IN BioCro VERSION 3.1.3

- This is the first version of BioCro to be accepted by CRAN! Most of the
  changes since version 3.1.0 were needed to comply with CRAN policies and
  requirements.

- Several changes have been made to reduce the package size from over 20 MB to
  less than 5 MB:
  - Crop model regression tests only store 1 of every 24 rows (one time point
    from each day)
  - The stored weather data has been rounded to 3 significant digits
    - The `solar` values have been rounded to the nearest integer
    - The `rh` values have been rounded to 2 significant digits
  - The stored crop model regression test data has been rounded to 5 significant
    digits
  - All previously-existing vignettes were converted to "web only," meaning they
    will be available through the pkgdown website but not included with the
    package itself
  - A new vignette has been added (`BioCro.Rmd`) that simply redirects readers
    to the documentation website

- Moved the included boost libraries from `inc` to `src/inc` since CRAN will not
  allow a nonstandard top-level directory. Some paths were shortened during
  this move.

- Added the Boost organization to the authors as a copyright holder to comply
  with CRAN policies

- Addressed a `missing-field-initializers` warning from the compiler by
  explicitly setting `iterations` to 0 in the output from
  `rue_leaf_photosynthesis`

- Addressed a mistake in `thermal_time_and_frost_senescence.h` where the leaf
  death rate due to frost had been unintentionally set to 0 in all conditions.
  This mistake was caught by a compiler that reported a "ignoring return value
  of function declared with 'nodiscard' attribute" warning.

- Changed the minimum version of macOS checked by the R-CMD-check from
  3.6.0 to 4.2.0
  - CRAN now only provides R versions 4.1.0 and above for Mac
  - The `deSolve` package cannot be built on Mac for R versions below 4.2.0

# CHANGES IN BioCro VERSION 3.1.2

- Variable-length arrays were eliminated from the module library code
  and replaced with std::vector.

# CHANGES IN BioCro VERSION 3.1.1

- The package date in its DESCRIPTION file was updated to meet CRAN submission
  requirements (must be less than one month old)

# CHANGES IN BioCro VERSION 3.1.0

## MINOR USER-FACING CHANGES

- Another bug was corrected in `src/module_library/c3photoC.cpp`: The
  photorespiration value `Rp` is now calculated using the value of `Ci` from the
  current loop iteration (rather than the previous loop iteration).

- Modified some testing-related functions so that warnings due to mismatched
  framework versions do not trigger test failures: the `tryCatch` call in
  `test_module` now only catches errors (not warnings) when evaluating the
  module, and `test_plant_model` (in `crop_model_testing_helper_functions.R`)
  now uses `expect_no_error` instead of `expect_silent`.

- Changed the energy_par_content constant to 0.219. This is a conversion rate
  from photon flux density (in micromoles per square meter per second) to energy
  flux density (in joules per square meter per second or watts per square meter)
  for photosynthetically active radiation (PAR). It equals 1/4.57, 4.57 being a
  commonly used constant to convert PAR in W m^-2 to micromole m^-2 s^-1. Users
  should take care to ensure that if processing of radiation data is required to
  prepare it for use with BioCro, the same conversion factor is used. See more
  details in Plant Growth Chamber Handbook. CHAPTER 1 – RADIATION– John C. Sager
  and J. Craig McFarlane. Table 2, Pg 3
  (https://www.controlledenvironments.org/wp-content/uploads/sites/6/2017/06/Ch01.pdf)

- The C++ framework has been updated to v1.1.3. Since the framework is included
  as a git submodule, it will be necessary to use the `--recurse-submodule` flag
  when using `git pull`, `git checkout`, or `git switch` to update a local copy
  of the BioCro repository, or to move to or from this branch.

- Replaced the `inc/boost` directory with a submodule pointing to the new
  `biocro/boost` repository.

- The (unexported) `lightME` function has been removed from the R package, since
  its functionality can be reproduced using the
  `BioCro:solar_position_michalsky` and
  `BioCro:shortwave_atmospheric_scattering` modules.

## OTHER CHANGES

- All instances of `fabs` or unqualified `abs` have been replaced by `std::abs`.
  The use of unqualified `abs` in `src/module_library/c3photoC.cpp` had been
  causing test failures when running BioCro on Windows using R version 3.6.0.

- This version adds a description of the BioCro git branching model to
  `contribution_guidelines.Rmd` and clarifies the process of updating `NEWS.md`.

- The R-CMD-check workflow has been changed in the following ways:

  - When the check workflow is run manually, there are two new input
    options:

    - The user can now choose whether or not to run R CMD check with
      the --as-cran option.  Formerly, this was always used.

    - The user can choose whether and when to throw an error on R CMD
      check failures.  Formerly, an error was thrown whenever the R
      CMD check failure was either "warning" or "error".

  - Output that was formerly shown only on manual runs when the
    "debug" checkbox was selected is now always shown.  The "debug"
    option has been changed to "dry-run", which results in the debug
    output being shown but the actual check, and those set-up steps
    needed only to carry out the check, are skipped.

  - The debug output steps are grouped together when possible, and the
    output is shown earlier on in the workflow.

  - The R-CMD-check workflow has been modified to work around a
    problem with testing R version 3.6.0 on Windows.  And for all
    platforms, we now specify the tested minimum R version as 3.6.0
    rather than simply 3.6 in order to ensure that we are actually
    testing the minimum required R version specified in the
    DESCRIPTION file, rather than some later 3.6.x version such as
    version 3.6.3.

- Modified the R-CMD-check workflow so that the manual is not checked
  when the workflow runs automatically.  This has also been made the
  default when the workflow is run manually.

- GitHub workflows and actions in the repository have been updated to
  use the latest versions of all GitHub and 3rd-party actions.

- Updates related to changing the GitHub hosting organization from
  "ebimodeling" to "biocro":

  Most references to the ebimodeling GitHub organization have been
  removed; references to ebimodeling/biocro have been updated to point
  to biocro/biocro instead.  Most of these occurred in various places
  in the documentation.  Most links to the online documentation have
  been replaced with links to https://biocro.github.io, with (in some
  cases) instructions on how to navigate to the particular section of
  the documentation of interest.  This decreases dependence on the
  precise layout of the online documentation.  Some other changes and
  clarifications to the documentation have been made as well.

- Addressed some `format-security` compiler warnings related to calling
  `Rf_error` and `Rprintf` without a format specifier; a format specifier of
  `"%s"` should always be used when printing the value of a string variable.

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
  can be obtained online from the _Articles_ menu at the
  [BioCro documentation website](https://biocro.github.io).

# BioCro VERSION 0.951

- This is the last release of the original version of BioCro, which was first
  described in [Miguez et al., 2009](https://doi.org/10.1111/j.1757-1707.2009.01019.x)
  and updated over the years to add more crop models and R functions. A full
  change log for this version and previous versions is not available in this
  document.
