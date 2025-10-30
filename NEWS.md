<!--
This file should document all significant changes brought about by each new
release.

All changes related to a particular release should be collected under a heading
specifying the version number of that release, such as
"# Changes in BioCro Version 2.0.0". The individual changes should be listed as
bullet points and categorized under "## Major Changes", "## Minor Changes",
or "## Bug Fixes" following the major.minor.patch structure of semantic
versioning, or variants of these such as "## Minor User-Facing Changes".

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
"# Changes in BioCro Version 2.0.0". This section will combine the draft release
notes for all features that have been added since the previous release.

In the case of a hotfix, a short section headed by the new release number should
be directly added to this file to describe the related changes.
-->

# Unreleased

## Minor User-Facing Changes

- A simple multidimensional root solving library has been added. See header
  file `src/math/roots/multidim/zeros.h` for example usage. Currently, only one
  method is available: Broyden's method. This library is intended to support
  models requiring equation solving.

- BioCro's 1D root solving library has been moved to `src/math/roots/onedim` in
  anticipation that it will be moved to the `biocro/framework` repository. The
  code was refactored, but this refactoring has small changes for client code.
  See the header file  `src/math/roots/onedim/roots.h` for example usage
  and a list of available methods.

- Added a module `daylength_calculator` which compute day length from solar position.

- Added custom C++ library for the numerical approximation of the zeros of real
  valued 1D functions. See the header file ~~`roots_onedim.h`~~
  `src/math/roots/onedim/roots.h` for example usage and a list of available methods.

- Swapped fixed point iteration for the Dekker method in Ci calculation. To
  incorporate the effect of stomatal conductance, the Ball-Berry and FvCB model
  require solving for the correct intercellular CO2 concentrations by
  identifying the root of an equation. BioCro previously used fixed point
  iteration to find the root. However, the fixed point iteration method is known
  to be unstable. For more info, see Sun et al. (2012) "A numerical issue in
  calculating the coupled carbon and water fluxes in a climate model."
  *Journal of Geophysical Research* https://dx.doi.org/10.1029/2012JD018059.
  This change only affects the solution at low Ci, and does not modify the
  interface of any module.

- Swapped fixed point iteration for the Dekker method in energy balance
  calculations that are used to determine the leaf temperature.

- Swapped fixed point iteration for the Dekker method in the implementation of
  the Nikolov leaf boundary layer conductance model.

- Added a new model for leaf boundary layer conductance (from Campbell & Norman
  1998). This model is now used in place of the Nikolov model for energy balance
  calculations, since it can be solved more easily, and does not have multiple
  solutions.

- Changed how growth respiration is calculated (so the rate of growth
  respiration is zero whenever the base rate of carbon available for growth is
  negative) and made sure it is calculated consistently across different
  modules.

- Renamed several quantities to make their meanings more clear:

  - `mrc1` has been replaced by `grc_leaf` and `grc_stem` to better indicate
    that this is a growth respiration coefficient that was applied to two
    different tissues.

  - `mrc2` has been replaced by `grc_root` and `grc_rhizome` for similar
    reasons.

  - `mrc_root` has been split into `mrc_root` and `mrc_rhizome` instead of using
    one coefficient for both.

  - `mrc_grain` has been split into `mrc_grain` and `mrc_shell` instead of using
    one coefficient for both.

  - Molar fluxes (with units of micromol / m^2 / s) are now distinguished from
    mass fluxes (with units of Mg / ha / hr) when necessary by replacing `_rate`
    with `_molar_flux` in the quantity name. For example,
    `canopy_assimilation_molar_flux` is a molar flux and
    `canopy_assimilation_rate` is a mass flux. Previously the molar fluxes had a
    suffix of `_CO2`, so this necessitated renaming two quantities:
    `canopy_assimilation_rate_CO2` and `canopy_photorespiration_rate_CO2`.

  - `GrossAssim_CO2` was renamed to `canopy_gross_assimilation_molar_flux` for
    consistency with other canopy-level molar flux outputs such as
    `canopy_assimilation_molar_flux`.

  - `Rd` was renamed to `RL` and its definition was updated to "the rate of
    non-photorespiratory CO2 release in the light" when necessary, following
    https://doi.org/10.1093/plphys/kiab076 and https://doi.org/10.1111/pce.14153.
    This avoid the ambiguity of using a subscript `d` (which can refer to "day"
    or "dark" in different contexts) and it reflects and updated understanding
    of the metabolic origin of this term (it is not exclusively, or even
    primarily, due to mitochondrial respiration).

  - Several quantities representing a value at 25 degrees C were renamed to
    better reflect this: `jmax` -> `Jmax_at_25`, `Rd` -> `RL_at_25`,
    `tpu_rate_max` -> `Tp_at_25`, `vmax1` -> `Vcmax_at_25`,
    `vmax` -> `Vcmax_at_25`.

- The `BioCro:parameter_calculator` module no longer recalculates `Vcmax_at_25`.

- The rate of non-photorespiratory CO2 release in the light (`RL` or
  `canopy_non_photorespiratory_CO2_release`) is now included in the outputs of
  photosynthesis modules, including at the canopy level.

- The rate of whole-plant growth respiration is now included in the outputs of
  the canopy photosynthesis modules, including the C4 canopy, where whole-plant
  growth respiration had not been previously calculated.

- The `BioCro:maintenance_respiration` module was split into two modules
  (a differential module called `BioCro:maintenance_respiration` and a direct
  module called `BioCro:maintenance_respiration_calculator`) to make maintenance
  respiration rates available as outputs.

- Several changes were made to the partitioning growth calculator modules:

  - Now there are only two such modules: `BioCro:partitioning_growth_calculator`
    and `BioCro:partitioning_growth_calculator_leaf_costs`.

  - The partitioning growth calculator modules now include growth respiration
    rates in their outputs.

  - They have also been standardized so they apply growth respiration to all
    tissues, and they both incorporate reductions in leaf growth rate due to
    water stress.

- The `BioCro:partitioning_coefficient_logistic` module can now calculate the
  rhizome partitioning coefficient using the logistic model.

- Two new modules were added for calculating cumulative flows of CO2 and water:
  `BioCro:cumulative_carbon_dynamics` and `BioCro:cumulative_water_dynamics`.

- A new module was added for calculating the total amounts of carbon used for
  growth and maintenance respiration:
  `BioCro:total_growth_and_maintenance_respiration`.

- Simplified the radiation use efficiency (RUE) model such that net CO2
  assimilation is directly proportional to the incident quantum flux of photons.

- Altered `test_module` (and hence `test_module_library`) so that new module
  outputs produce a warning but don't cause an error on their own.

- Altered `evaluate_module`, `partial_evaluate_module`, and
  `module_response_curve` so that by default, module errors do not prevent
  response curve calculations from completing. The original behavior of these
  functions can be reproduced by setting `stop_on_exception` to `TRUE` when
  calling them.

- Following changes to the behavior of some modules, the soybean model was
  re-parameterized. The parameterization script is now included in the BioCro
  repository and package. See the help file for `soybean` for complete details.

- `catm_data` was updated to include the global average atmospheric CO2
  concentration in 2024.

- Links to the main BioCro documentation web site were changed from
  `https://biocro.github.io` to `https://biocro.org`

## Internal changes

- Renamed the `resp()` function to `growth_resp_Q10()` to better indicate its
  purpose, and moved it to a respiration header file. The new header file
  (`respiration.h`) also includes a new maintenance respiration function
  (`maintenance_respiration_Q10()`) and a simpler growth respiration function
  that does not apply a Q10 response (`growth_resp()`).

## Bug fixes

- Fixed a an issue where setting `adaptive_max_steps` to `NA` in R would cause
  `run_biocro` to fail at the first time point.

- Fixed a typo in the calculation of relative humidity just above the canopy
  (`RH_canopy`).

# Changes in BioCro version 3.2.0

## Minor User-Facing Changes

- Added maintenance respiration for each organ in a new module called
  `BioCro:maintenance_respiration`. Maintenance respiration is modelled by
  removing a fraction of dry biomass. The fraction removed is determined by an
  organ-specific "maintenance respiration coefficient" (such as `mrc_leaf`) and
  follows a Q10 temperature response. This differs from the existing growth
  respiration that is applied to the stem and root, and from a separate canopy
  growth respiration that can be used to rescale the canopy assimilation rate.

- Separated the specific leaf area (SLA) calculations from the
  `BioCro:parameter_calculator` module to enable alternate approaches to SLA.
  The original method is now available as the `BioCro:sla_linear` module, and a
  new logistic method has been added: `BioCro:sla_logistic`. The stored crop
  model definitions were updated to use the linear SLA module.

- Provided a new direct module for determining development index from thermal
  time: `BioCro:development_index_from_thermal_time`. This module is an
  alternative to the `BioCro:thermal_time_development_rate_calculator`
  differential module.

- C3 temperature response parameters are no longer hard-coded into `c3photoC()`:

  - There are now specialized structs for the temperature response parameters
    (`c3_temperature_response_parameters`) and the temperature-dependent values
    of key photosynthetic parameters (`c3_param_at_tleaf`).

  - There is also a dedicated function for calculating temperature-dependent
    parameter values: `c3_temperature_response()`.

  - The temperature response parameters are now inputs to several modules:
    `BioCro:c3_assimilation`, `BioCro:c3_canopy`,
    `BioCro:c3_leaf_photosynthesis`,

  - There is also a dedicated module for calculating values of C3 parameters at
    leaf temperature: `BioCro:c3_parameters`.

  - The `theta` parameter was renamed to `theta_0` for better consistency with
    the `polynomial_response()` function input argument names.

- Added a new vignette explaining key features of BioCro's multilayer canopy
  model, and made several changes to `sunML()` and related functions to ensure
  that the code matches the model description in the vignette:

  - Stopped calculating and using the "average" incident PPFD and absorbed
    shortwave radiation for leaves in the canopy.

  - Stopped using the "thick layer absorption" equation for determining the
    absorbed shortwave radiation within the canopy, replacing it with the thin
    layer absorption equation.

  - Used a simpler equation for calculating the fraction of sunlit leaves.

  - Used the same absorptivity value for direct and diffuse light.

  - Used separate leaf transmittance and reflectance values for PAR and NIR
    radiation within the canopy, rather than always assuming that light in the
    two bands are absorbed and scattered equally; in general, this caused a
    reduction in the absorbed shortwave energy for all leaves.

  - Started calculating absorptivity as `1 - R - T`, where `R` and `T` are the
    leaf reflectance and transmittance coefficients, respectively. This ensures
    that the constraint `A + R + T = 1` is always satisfied.

- Made several changes to BioCro's time handling:

  - The `time` variable is now required to be sequential and evenly spaced,
    where the time interval must be equal to `timestep`. A consequence is that
    `time` and `timestep` must have the same units.

  - With this change, it was necessary to change the definition of `time` used
    with the crop models. Now it is expected to be expressed as the (fractional)
    number of hours since midnight on January 1, rather than a fractional day of
    year.

  - There is a new module for calculating `doy` and `hour` from `time`, called
    `BioCro:format_time`. This module ensures that `doy` always takes integer
    values in the output from `run_biocro`.

  - In most cases, old scripts calling `run_biocro` will continue to function
    following these changes because `time` will be correctly computed from `doy`
    and `hour`, and `BioCro:format_time` will be automatically added to module
    lists.

  - The redefinition of `time` from days to hours may require changes to
    plotting commands or other operations using `time`. In most cases, instances
    of `time` in old scripts can be replaced by `fractional_doy`, which is
    equivalent to the definition of `time` used in previous versions of BioCro.

- Added a new function for generating C++ header files for new module classes:
  `module_write`.

- Added several functions to help with model regression tests:
  `compare_model_output`, `model_test_case`, `run_model_test_cases`, and
  `update_stored_model_results`. Previously, these were part of
  `tests/testthat/crop_model_testing_helper_functions.R`.

- The conversion of CO2 assimilation to biomass is no longer hard coded into
  the photosynthesis functions and modules, such as `c3CanAC()`, `CanAC()`,
  and `BioCro:ten_layer_multilayer_canopy_integrator`.

  - These functions and modules now produce canopy assimilation rates as
    molecular fluxes (with units of micromol CO2 / m^2 / s).

  - A new module called `BioCro:carbon_assimilation_to_biomass` now performs the
    conversion to rates of dry biomass acculumation (with units of
    Mg / ha / hr). A new parameter `dry_biomass_per_carbon` controls the
    conversion.

  - All affected models have the same behavior as before if the new module is
    used with `dry_biomass_per_carbon` set to 30.026 g / mol.

- The `soybean` model was re-parameterized following changes to module behavior.

## Other Changes

- Consolidated all temperature response functions into a single header file
  (`src/module_library/temperature_response_functions.h`) that now includes
  `arrhenius_exponential()`, `Q10_temperature_response()`,
  `johnson_eyring_williams_response()`, and `polynomial_response()`.

- The developer documentation was updated to include a section about pull
  requests.

## Bug fixes

- Fixed incorrect `year` column values in the weather data.

- Fixed a mistake where the CMI weather data for 2023 was a copy of the 2022
  data.

- The `ode_solver` input argument of `run_biocro` is now checked to ensure the
  essential list elements are provided.

# Changes in BioCro Version 3.1.3

- This is the first version of BioCro to be accepted by CRAN! Most of the
  changes since version 3.1.0 were needed to comply with CRAN policies and
  requirements.

- Several changes have been made to reduce the package size from over 20 MB to
  less than 5 MB:

  - Crop model regression tests only store 1 of every 24 rows (one time point
    from each day).

  - The stored weather data has been rounded to 3 or fewer significant digits:

    - The `solar` values have been rounded to the nearest integer.

    - The `rh` values have been rounded to 2 significant digits.

  - The stored crop model regression test data has been rounded to 5 significant
    digits.

  - All previously-existing vignettes were converted to "web only," meaning they
    will be available through the pkgdown website but not included with the
    package itself.

  - A new vignette has been added (`BioCro.Rmd`) that simply redirects readers
    to the documentation website.

- Moved the included boost libraries from `inc` to `src/inc` since CRAN will not
  allow a nonstandard top-level directory. Some paths were shortened during
  this move. The submodule repository was also renamed from `biocro/boost` to
  `biocro/inc`.

- Added the Boost organization to the authors as a copyright holder to comply
  with CRAN policies.

- Addressed a `missing-field-initializers` warning from the compiler by
  explicitly setting `iterations` to 0 in the output from
  `rue_leaf_photosynthesis`.

- Addressed a mistake in `thermal_time_and_frost_senescence.h` where the leaf
  death rate due to frost had been unintentionally set to 0 in all conditions.
  This mistake was caught by a compiler that reported a "ignoring return value
  of function declared with 'nodiscard' attribute" warning.

- Changed the minimum version of macOS checked by the R-CMD-check from
  3.6.0 to 4.2.0.

  - CRAN now only provides R versions 4.1.0 and above for Mac.

  - The `deSolve` package cannot be built on Mac for R versions below 4.2.0.

# Changes in BioCro Version 3.1.2

- Variable-length arrays were eliminated from the module library code
  and replaced with std::vector.

# Changes in BioCro Version 3.1.1

- The package date in its DESCRIPTION file was updated to meet CRAN submission
  requirements (must be less than one month old).

# Changes in BioCro Version 3.1.0

## Minor User-Facing Changes

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
  commonly used constant to convert PAR in W m^-2 to micromol m^-2 s^-1. Users
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

## Other Changes

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
  been replaced with links to https://biocro.org, with (in some
  cases) instructions on how to navigate to the particular section of
  the documentation of interest.  This decreases dependence on the
  precise layout of the online documentation.  Some other changes and
  clarifications to the documentation have been made as well.

- Addressed some `format-security` compiler warnings related to calling
  `Rf_error` and `Rprintf` without a format specifier; a format specifier of
  `"%s"` should always be used when printing the value of a string variable.

# Changes in BioCro Version 3.0.2

## Minor Changes

- This version adds several missing references to the main README.

# Changes in BioCro Version 3.0.1

## Minor Changes

- This version pertains only to the GitHub documentation workflow.  It
  changes the publication location to the
  `biocro/BioCro-documentation` repository, and it changes the
  triggers of the workflow so that automatic publication happens when
  a new release is published.  Additionally, a symlink is created to
  link the URL
  https://biocro.org/BioCro-documentation/latest/pkgdown/ to
  https://biocro.org/BioCro-documentation/<tag name>/pkgdown/,
  where <tag name> is the tag name for the new release.

# Changes in BioCro Version 3.0.0

## Major Changes

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

## Minor Changes

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

## Bug Fixes

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

# Changes in BioCro Version 2.0.0

## Major Changes

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
  [BioCro documentation website](https://biocro.org).

# BioCro Version 0.951

- This is the last release of the original version of BioCro, which was first
  described in [Miguez et al., 2009](https://doi.org/10.1111/j.1757-1707.2009.01019.x)
  and updated over the years to add more crop models and R functions. A full
  change log for this version and previous versions is not available in this
  document.
