# Contributing to BioCro

## Making Changes

### Discuss first
* Check the list of [GitHub issues](https://github.com/ebimodeling/biocro-dev/issues)<!-- CHANGE THIS URL WHEN MOVING FROM ebimodeling/biocro-dev TO ebimodeling/biocro!!! --> for a discussion of the issue. If there is not one, create an issue with a description of the problem and your proposed solution.

 By making changes without discussing it with the group, you risk spending time working on a solution that others may not accept. The members of the group also have diverse backgrounds and likely can give valuable design insights.

## Code style

(Most of what is discussed here pertains specifically to code for the BioCro C++ library.)

### Scientific considerations

#### Document sources and justification in the code
* Include citations to sources for equations and parameters used in the code. The citation should be sufficient to locate the article and relevant information within it. Include a table or figure reference if appropriate.
* Use [Doxygen](https://www.doxygen.nl/manual/docblocks.html)-style comment syntax for high-level documentation of functions and classes.  (See the documentation of the `solar_zenith_angle` class in the [Solar Zenith Angle module](../src/module_library/solar_zenith_angle.h) for a short example of a Doxygen-style comment.)
* Include reasoning and justification for the model, including assumptions that determine when use of the model is appropriate.  These descriptions should be succinct.
* See the [Ball-Berry model](../src/module_library/ball_berry.cpp) for an example.

#### Document units in the code
* After every [physical quantity](https://en.wikipedia.org/wiki/Physical_quantity), include a comment with the units. The idea is that every quantity will roughly be read as if it were written in normal text: ```double yield = 10  // Mg / ha``` is something like "the yield was 10 Mg / ha". Using dimensions instead of units is acceptable if the code is written with the expectation that [coherent units](https://en.wikipedia.org/wiki/Coherence_%28units_of_measurement%29) are used.

    ```c++
    // In function signatures
    double ball_berry(double assimilation,                   // mol / m^2 / s
                      double atmospheric_co2_concentration,  // mol / mol
                      double atmospheric_relative_humidity,  // Pa / Pa
                      double beta0,                          // mol / m^2 / s
                      double beta1)                          // dimensionless from [mol / m^2 / s] / [mol / m^2 / s]

    // In assignments
    double leaf_temperature = air_temperature - delta_t;  // K.

    // In return statements
    return assimilation_rate;  // micromoles / m^2 / s.

    // In tables
    const std::map<SoilType, soilText_str> soil_parameters =
    {
        //                        d = dimensionless
        //                        d     d     d     J kg^-1     d     J s m^-3     d     d      d     Mg m^-3
        //                        silt  clay  sand  air_entry   b     Ks           satur fieldc wiltp bulk_density
        { SoilType::sand,       { 0.05, 0.03, 0.92,      -0.7,  1.7,  5.8e-3,      0.87, 0.09,  0.03, 1.60 } },
        { SoilType::loamy_sand, { 0.12, 0.07, 0.81,      -0.9,  2.1,  1.7e-3,      0.72, 0.13,  0.06, 1.55 } },
    };
    ```

* If you would like to include other details, include the units in the same way, and include details **following** the units so that the variables are still read like regular text.  For example, write

    ```c++
    ✓ return gswmol * 1000;  // mmol / m^2 / s. Convert from mol to mmol.
    ```
    _not_
    ```c++
    ✗ return gswmol * 1000;  // Converting from mol / m^2 / s to mmol / m^2 / s.
    ```
* Use SI conventions for units and dimensions, including capitalization. Specifically, use "`degrees C`", not "`C`", to indicate &deg;C.
* Use full names when symbols are not available:
  * `micromoles / m^2`, _not_ `umol / m^2`
  * `degrees C`, _not_ ``*C``.
* Use `dimensionless` for dimensionless quantities, and include how the dimensions have canceled if that is informative.
* Use `^` to indicate exponentiation: `m^2`, _not_ `m2`.
* Prefer an asterisk to indicate multiplication; but indicating multiplication by juxtaposing units with exactly one space between them is acceptable. Prefer exactly one space on each side of the asterisk: `kg * m / s` or `kg m / s`.
* Either a solidus ("/") or negative exponent is acceptable to indicate division, but ensure that the solidus is used correctly if used multiple times. Prefer exactly one space on each side of the solidus.

#### Document parameters
* When adding models that require new parameters, document the parameters in the parameter table in [src/parameters.h](../src/parameters.h). Please keep the table well formatted.
* If you are working on a model with undocumented parameters, it would be nice if you added them to the table as you work through the issue.

### General coding considerations
* Do not use C-style arrays. Use an appropriate data type from the standard library instead.
* Use cmath, not math.h, for common mathematical functions.

* Be careful with **using**-directives (e.g. `using namespace std`) in
  a global scope; do not use them in global scope in a header file.
  Try to make **using**-declarations (e.g. `using std::string`) as
  local as possible.  Type aliases (e.g. `using string_vector =
  std::vector<std::string>`) are perfectly acceptable in the global
  scope of a header file.

* Strongly prefer the
  [coherent](https://en.wikipedia.org/wiki/Coherence_%28units_of_measurement%29)
  set of SI units. Doing so reduces code complexity remarkably as no
  conversions are necessary. Yes, no one publishes values with these
  units, but do the conversion in one place, the manuscript, instead
  of dozens of times in the code, constantly having to look up units
  for variables, and then spending hours debugging silly,
  difficult-to-find errors.  The coherent set of SI units consists of
  all the units without prefixes, except that kg is the coherent unit
  of mass, not g.

* Do not copy and paste code, changing only small parts. Choose a design that eliminates the duplication. Duplication is often the result of not separating control flow from data. Consider the following R code.

    ```r
    if (!("lattice" %in% installed.packages()[,"Package"])) {
      install.packages("lattice", repos="http://R-Forge.R-project.org", type="source")
    }

    if (!("BioCro" %in% installed.packages()[,"Package"])) {
      devtools::install_github("ebimodeling/biocro")
    }

    if (!("boot" %in% installed.packages()[,"Package"])) {
      devtools::install_url("http://cran.r-project.org/src/contrib/Archive/boot/boot_1.3-7.tar.gz")
    }
    ```

    The data and behavior can be separated.
    ```r
    required_packages = list(
        # package name  # installation function   # function arguments
        list("lattice", install.packages,         list("lattice", repos="http://R-Forge.R-project.org", type="source")),
        list("BioCro",  devtools::install_github, list("ebimodeling/biocro")),
        list("boot",    devtools::install_url,    list("http://cran.r-project.org/src/contrib/Archive/boot/boot_1.3-7.tar.gz"))
    )

    install_packages_if_missing = function(package_table) {
        installed_packages = installed.packages()[,"Package"]
        for (row in package_table) {
            if (!(row[[1]] %in% installed_packages)) {
                do.call(row[[2]], row[[3]])
            }
        }
    }

    install_packages_if_missing(required_packages)
    ```

    In the first example, the behavior is spread throughout all of the code, and there is not an obvious indication of what is being done. Once one understands what is being done, one must still read all of the code to be sure some different behavior is not hidden somewhere.

    In the second example, it is clear that the same task is performed repeatedly because control flow is in a single place, and the place has a meaningful name: `install_packages_if_missing`. For a long list, this is succinct, and easier to understand and maintain. If one wanted, one could devise a way to use the meaningful names of the columns of the `required_packages` table within the function.

* Make an effort to write unit tests.  (See the document "An Introduction to BioCro for Those Who Want to Add Models" for information about writing unit tests—specifically, writing unit tests for new modules.)
* Do not mix sweeping formatting changes with behavior changes. Large formatting changes should be committed separately, and the commit comment should indicate that only formatting was changed.
* The [C++ guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) have useful advice about [aspects of coding and design](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-naming).

### Formatting code

(Again, except in a few instances, this pertains specifically to C++ code.)

The most important aspect of formatting is that the code is easy to understand. Below are unenforced preferences.

* Regarding formatting (but not aspects of design), something similar to the [Google C++ style guide](https://google.github.io/styleguide/cppguide.html#Formatting) is preferred (with some differences; see [.clang-format](../.clang-format) for differences), except in cases where the code has been formatted in a more readable way, such as when aligning parts in a table. The program [clang-format](https://clang.llvm.org/docs/ClangFormat.html), using the [.clang-format](../.clang-format) file provided in the base directory of BioCro, produces code with the preferred formatting. **Do not apply clang-format to all files indiscriminately**, as that will ruin manually-aligned tables.
* One can install clang-format on Ubunutu using `sudo apt install clang-format` and on MacOS through Homebrew.
* Files can be formatted using
    ```
    clang-format file_name > new_file
    ```
    or edited in place using
    ```
    clang-format -i file_name
    ```
    If your editor has the ability to display differences between the original and revised versions of the file, it is a good idea to step through and inspect the proposed changes to ensure they are desirable.
* On Windows, MacOS, or Linux, the CodeLite IDE includes clang-format and provides an easy way to use it. First go to Plugins -> Source Code Formatter -> Options. In the C++ tab, select `use .clang-format file`. Now press `Ctrl-I` or click Plugins -> Source Code Formatter -> Format Current Source to format a file.
* The C++ guidelines offer some advice about [formatting conventions](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-naming) that are informative, particularly regarding the use of code comments, but that are not enforced here.
* Prefer `underscores_in_identifiers` not `CamelCaseInIdentifiers` and, in R, not `dots.in.identifiers`.  Prefer lowercase-only identifiers.  An exception may be made for commonly-recognized names used in a small scope, for example,
    ```c++
    I = V / R;
    F = m * a;
    E = m * (c * c);
    ```
* Avoid unnecessary parentheses. For example, use "`a * b / c`" instead of <nobr>"`(a * b) / c`"</nobr>  or  <nobr>"`a * (b / c)`"</nobr>.  But in cases where the order of operations affects the result, parentheses may be used to erase any doubt in the mind of the reader (or the programmer!) as to what that order is.  Thus, writing `(a / b) * c` instead of (the equivalent) `a / b * c` is acceptable.  Parentheses may also be used to group portions of a formula that are commonly considered as a subunit, where they provide some semantic value (see the previous bullet point).
Consider naming parts of a complicated expression in order to break it down into simpler ones.  For example,
    ```c++
    x = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
    ```
    may be rewritten in three lines as
    ```c++
    num = -b + sqrt(b * b - 4 * a * c);
    denom = 2 * a;
    x = num / denom;
    ```
    Note that in C++, unlike in R, return statements do not require parentheses around the returned expression.

* Restrict the line length of paragraph-like comments to 80
  characters, excepting a compelling reason to do otherwise. Lines in
  sections that are not paragraph-like could be somewhat longer if it
  facilitates presenting material in a more readable format.  In the
  following snippet from the module library documentation, for
  example, we have allowed slighly-longer lines in order to be able to
  maintain one line per interval:

    ```c++
    /*
    *  However, this definition is flexible. For example, for our soybean model
    *  (soybean_development_rate_calculator.h) we define the intervals as follows:
    *  -1 <= DVI < 0 : Sowing to Emergence
    *   0 <= DVI < 1  :  Emergence to R1 (Flowering) is broken into three stages.
    *       0 <= DVI < 0.333        : Emergence to V0 (Cotyledon stage)
    *       0.333 <= DVI < 0.667    : V0 (Cotyledon stage) to R0 (End of Floral Induction)
    *       0.667 <= DVI < 1        : R0 (End of Floral Induction) to R1 (Flowering)
    *  1 <= DVI < 2 : R1 (Flowering) to R7 (Maturity)
    */
    ```

  As for the code lines themselves, we point to the following advice
  from the Linux kernel project:<sup id="a1">[1](#f1)</sup>

  > The preferred limit on the length of a single line is 80 columns.

  > Statements longer than 80 columns should be broken into sensible
    chunks, unless exceeding 80 columns significantly increases
    readability and does not hide information.



## Running Unit Tests

The **BioCro** package contains a collection of R-based unit tests to help
maintain package integrity.  These tests should be run periodically,
and especially before committing changes to the C++ or R code.

There are a number of ways to run these tests:

1. In an R session using the **testthat** library functions.

    a. Ensure the **BioCro** library and the **testthat** library are both loaded.

    b. Determine the relative path to the <_BioCro package root_>`/tests/testthat` directory from the current working directory. (The current working directory may be discovered by calling `getwd()`).

    c. Call `test_dir('`<_path to testthat directory_>`')`.

    (`test_dir` can also take an absolute path.)

2. In an R session using the **devtools** package.

    If you have the **devtools** package installed, running all package tests is even simpler.

    a. Start an R session from any directory inside the BioCro
    package; or in an existing R session, use the `setwd()` function
    to switch to a directory inside the package if you aren't already
    inside such a directory.

    b. Load the **devtools** package: `library(devtools)`.

    c. Call `test()`.

3. In RStudio.

   This method assumes the **devtools** package is installed.  (It needn't be explicitly loaded.)

    a. Open the BioCro project in RStudio.

    b. From the **Build** menu, choose **Test Package**.  Or use the keyboard shortcut Ctrl + Shirt + T (Cmd + Shift + T on Mac).


While writing a file a tests, it is often useful to run only the tests in that one file.  This is done similarly to method 1 above, but using `test_file` instead of `test_dir`:

4. Running one file of tests in an R session.

    a. Ensure the **BioCro** library and the **testthat** library are both loaded.

    b. Determine the relative path from the current working directory to the test file.

    c. Call `test_file('`<_path to test file_>`')`.

    (`test_file` can also take an absolute path.)


---

<b id="f1">1</b> The Linux kernel project recently changed the default
length for code lines from 80 to 100 characters with the following
commit comment:

> Yes, staying withing 80 columns is certainly still _preferred_.  But
it's not the hard limit that the checkpatch warnings imply, and other
concerns can most certainly dominate.

> Increase the default limit to 100 characters.  Not because 100
characters is some hard limit either, but that's certainly a "what are
you doing" kind of value and less likely to be about the occasional
slightly longer lines. [↩](#a1)