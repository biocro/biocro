## Contributing to BioCro

### Making Changes

#### Discuss first
* Check the list of [GitHub issues](https://github.com/ebimodeling/biocro-dev/issues) for a discussion of the issue. If there is not one, create an issue with a description of the problem and your proposed solution.

 By making changes without discussing it with the group, you risk spending time working on a solution that others may not accept. The members of the group also have diverse backgrounds, and likely can give valuable design insights.

### Code style

#### Scientific considerations

##### Document sources and justification in the code
* Include citations to sources for equations and parameters in the code. The citation should be sufficient to locate the article and relevant information within it. Include a table or figure reference if appropriate.
* Include reasoning and justification for the model, including assumptions that determine when use of the model is appropriate.  These descriptions should be succint.
* See the [Ball-Berry model](../src/module_library/ball_berry.cpp) for an example.

##### Document units in the code
* After every [physical quantity](https://en.wikipedia.org/wiki/Physical_quantity), include a comment with the units. The idea is that every quantity will roughly be read as if it were written in normal text: ```double yield = 10  // Mg / ha``` is somthing like "the yield was 10 Mg / ha". Using dimensions instead of units is acceptable if the code is written with the expectation that [coherent units](https://en.wikipedia.org/wiki/Coherence_%28units_of_measurement%29) are used.

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
* If you would like to include other details, include the units in the same way, and include details **following** the units so that the variables are still read like regular text.

 ```c++
 return gswmol * 1000;  // mmol / m^2 / s. Convert from mol to mmol.
 ```
 not ~~`return gswmol * 1000;  // Converting from mol / m^2 / s to mmol / m^2 / s.`~~
* Use SI conventions for units and dimensions, including capitalization. Specifically, use `degrees C`, not `C`, to indicate &deg;C.
* Use full names when symbols are not available: `micromoles / m^2`, not ~~`umol / m^2`~~; `degrees C` not ~~``*C``~~.
* Use `dimensionless` for dimensionless quantities, and include how the dimensions have canceled if that is informative.
* Use `^` to indicate exponentiation: `m^2` not ~~`m2`~~.
* Prefer an asterisk to indicate multiplication, but exactly one space is acceptable. Prefer exactly one space on each side of the asterisk: `kg * m / s` or `kg m / s`.
* Either a solidus or negative exponents are acceptable to indicate division, but ensure that the solidus is used correctly if used multiple times. Prefer exactly one space on each side of the solidus.

##### Document paramaters
* When adding models that require new parameters, document the parameters in the parameter table in [src/parameters.h](../src/parameters.h). Please keep the table well formatted.
* If you are working on a model with undocumented parameters, it would be nice if you added them to the table as you work through the issue.

#### General design
* Do not use C-style arrays. Use an appropriate data type from the standard library instead.
* Use cmath, not math.h, for common mathematical functions.
* Do not use `using` statements in places that have nonlocal influence, such as in header files.
* Strongly prefer the [coherent](https://en.wikipedia.org/wiki/Coherence_%28units_of_measurement%29) set of SI units. Doing so reduces code complexity remarkably as no conversions are necessary. Yes, no one publishes values with these units, but do the conversion in one place, the manuscript, instead of dozens of times in the code, constantly having to look up units for variables, and then spending hours debugging silly, difficult-to-find, errors.
 * The coherent set of SI units consists of all the units without prefixes, except that kg is the coherent unit of mass, not g.
* Do not copy and paste code, changing only smalls parts. Choose a design that eliminates the duplication. Duplication is often the result of not separating control flow from data. Consider the following R code.

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

 In the first example, the behavior is spread throughout all of the code, and there is not an obvious indication of what is being done. Once one understands what is being one, one must still read all of the code to be sure some different behavior is not hidden somewhere.

 In the second example, it is clear that the same task is performed repeatedly because control flow is in a single place, and the place has a meaningful name: `install_packages_if_missing`. For a long list, this is succint, and easier to understand and maintain. If one wanted, one could devise a way to use the meaningful names of the columns of the `required_packages` table within the function.

* Make an effort to write unit tests.
* Do not mix sweeping formatting changes with behavior changes. Large formatting changes should be commited separately, and the comment should indicate that only formatting was changed.
* The C++ guidelines have useful advice about [aspects of design](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-naming).

#### Formatting
* The most important aspect of formatting is that the code is easy to understand. Below are unenforced preferences.
* Regarding formatting (but not aspects of design), something similar to the [Google C++ style guide](https://google.github.io/styleguide/cppguide.html) is preferred (with some differences; see [.clang-format](../.clang-format) for differences), except in cases where the code has been formatted in a more readable way, such as when aligning parts in a table. The program [clang-format](https://clang.llvm.org/docs/ClangFormat.html), using the [.clang-format](../.clang-format) file provided in the base directory of BioCro, produces code with the preferred formatting. **Do not apply clang-format to all files indiscriminately**, as that will ruin manually aligned tables.
 * One can install clang-format on Ubunutu using `sudo apt install clang-format` and on MacOS through Homebrew.
 * Files can be formatted using `clang-format file_name > new_file` or edited in place using `clang-format -i file_name`.
 * On Windows, MacOS, or Linux, the Codelite IDE includes clang-format and provides an easy way to use it. First go to Plugins -> Source Code Formatter -> Options. In the C++ tab, select `use .clang-format file`. Now press `Ctrl-I` or click Plugins -> Source Code Formatter -> Format Current Source to format a file.
* The C++ guidelines offer some advice about [formatting conventions](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-naming) that are informative, particulary regarding the use of code comments, but that are not enforced here.
* Prefer `underscores_in_identifiers` not `CamelCaseInIdentifiers` and, in R, not `dots.in.identifiers`.
* Avoid unnecessary parentheses. Use `a / b * c` instead of `(a / b) * c` or write the equation more sensibly, doing something like listing all variables in the numerator first followed by variables in the denominator for the most clarity, such as `z * q / d / e` instead of `z / d * q / e`. Of note is that return statements in C++ do not require parentheses, but in R they are required.
