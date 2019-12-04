## Contributing to BioCro
### Making Changes
#### Discuss first
Check the issue tracker for a discussion of the issue. If there is not one, create an issue with a description of the problem and your proposed solution.

By making changes without discussing it with the group, you risk spending time working on a solution that others may not accept. The members of the group also have diverse backgrounds, and likely can give valuable design insights.

### Code style
#### Formatting
* Formatting should follow the format produced by [clang-format](https://clang.llvm.org/docs/ClangFormat.html) using the .clang-format file provided in the base directory of BioCro, except in cases where the code has been formatted in a more readable way, such as when aligning parts in a table. **Do not apply clang-format to the all files indiscriminately**, as that will ruin manually aligned tables.
* Use `underscores_in_identifiers` not `CamelCaseInIdentifiers`.
* Do not include unnessary parenthesis in return statements.

#### Scientific considerations

* After every [physical quantity](https://en.wikipedia.org/wiki/Physical_quantity), include a comment with the units. The idea is that every quantity will roughly be read as if it were written in normal text: ```double yield = 10  // Mg / ha``` is somthing like "the yield was 10 Mg / ha". Using dimensions instead of units is acceptable if the code is written with the expectation that coherent units are used.

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
    //                             d = dimensionless
    //                             d     d     d     J kg^-1     d     J s m^-3     d     d      d     Mg m^-3
    //                             silt  clay  sand  air_entry   b     Ks           satur fieldc wiltp bulk_density
    { SoilType::sand,            { 0.05, 0.03, 0.92,      -0.7,  1.7,  5.8e-3,      0.87, 0.09,  0.03, 1.60 } },
    { SoilType::loamy_sand,      { 0.12, 0.07, 0.81,      -0.9,  2.1,  1.7e-3,      0.72, 0.13,  0.06, 1.55 } },
};

```
If you would like to include other details, include the units in the same way, and include details **following** the units so that the variables are still read like regular text.

```c++
return gswmol * 1000;  // mmol / m^2 / s. Convert from mol to mmol.
```

not ~~```return gswmol * 1000;  // Converting from mol / m^2 / s to mmol / m^2 / s.```~~

Use SI conventions for units, including capitalization. Specifically, use ```degrees C```, not ```C```, to indicate &deg;C.

Use full names when symbols are not available: ```micromoles / m^2```, not ~~```umol / m^2```~~; ```degrees C``` not ~~```*C```~~.

Use ```dimensionless``` for dimensionless quantities, and if possible include how the dimensions have canceled if that is informative.

Use `^` to indicate exponentiation
```m^2``` not ~~```m2```~~.

Prefer an asterisk to indicate multiplication, but exactly one space is acceptable. Prefer exactly one space on each side of the asterisk.
```kg * m / s``` or ```kg m / s```

Either a solidus or negative exponents are acceptable to indicate division, but ensure that the solidus is used correctly if used multiple times. Prefer exactly one space on each side of the solidus.


#### General design
* Do not use C-style arrays. Use an appropriate data type from the standard library instead.

* Strongly prefer the [coherent](https://en.wikipedia.org/wiki/Coherence_%28units_of_measurement%29) set of SI units. Doing so reduces code complexity remarkably as no conversions are necessary. Yes, no one publishes values with these units, but do the conversion once, when you write the manuscript, instead of dozens of times when writing the code and then spending hours debugging silly, difficult-to-find, errors.

* Do not copy and paste code, changing only smalls parts. Choose a design that eliminates the duplication. Duplication is often the result of not separating control flow from data. Consider the following R code:

```r
if(!("lattice" %in% installed.packages()[,"Package"])) {
  install.packages("lattice", repos="http://R-Forge.R-project.org", type="source")
}

if(!("BioCro" %in% installed.packages()[,"Package"])) {
  devtools::install_github("ebimodeling/biocro")
}

if(!("boot" %in% installed.packages()[,"Package"])) {
  devtools::install_url("http://cran.r-project.org/src/contrib/Archive/boot/boot_1.3-7.tar.gz")
}
```

The data and behavior can be separated:
```r
required_packages = list(
    # package name  # installation function   # function arguments
    list("lattice", install.packages,         list("lattice", repos="http://R-Forge.R-project.org", type="source")),
    list("BioCro",  devtools::install_github, list("ebimodeling/biocro")),
    list("boot",    devtools::install_url,    list("http://cran.r-project.org/src/contrib/Archive/boot/boot_1.3-7.tar.gz"))
)

install_missing_packages = function(package_table) {
    installed_packages = installed.packages()[,"Package"]
    for (row in package_table) {
        if (!(row[[1]] %in% installed_packages)) {
            do.call(row[[2]], row[[3]])
        }
    }
}

install_missing_packages(required_packages)
```

In the first example, the behavior is spread throughout all of the code, and there is not an obvious indication of what is being done. Once one understands what is being one, one must still read all of the code to be sure some different behavior is not hidden somewhere.
In the second example, it is clear that the same task is performed repeatedly because control flow is in a single place, and the place has a meaningful name: `install_missing_packages`. For a long list, the second example is succint, and easier to understand and maintain. If one wanted, one could devise a way to use the meaningful names of the columns of the `required_package` table in the code.
