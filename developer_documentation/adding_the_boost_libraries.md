# Adding the Boost libraries

## Why this is needed
BioCro uses software from the Boost C++ libraries. Boost does not assure
backward compatibility, so changes to Boost could break BioCro. Thus, we don't
want to link our code to a user supplied Boost installation, and we include a
version with BioCro.

Boost is very large, so we want to include only the necessary parts. This
document lists steps to extract the relevant parts and update files in BioCro to
use them.

## How to extract parts of Boost
Use the `bcp` tool included with Boost to extract parts of the Boost library.
`bcp` accepts a list of files or modules and extracts the relevant parts of the
Boost library to a directory.

The boost files that BioCro uses are as follows:

| File    name                               | Notes                                                              |
| ------------------------------------------ | ------------------------------------------------------------------ |
| `boost/config.hpp`                         | Used in `module_dependency_utilities.cpp`                          |
| `boost/graph/adjacency_list.hpp`           | Used in `module_dependency_utilities.cpp`                          |
| `boost/graph/topological_sort.hpp`         | Used in `module_dependency_utilities.cpp`                          |
| `boost/math/constants/constants.hpp`       | Used in `constants.h`                                              |
| `boost/numeric/odeint.hpp`                 | Used in `ode_solver.h`                                             |
| `boost/numeric/ublas/io.hpp`               | Used in `newton_raphson_boost.h`                                   |
| `boost/numeric/ublas/lu.hpp`               | Used in `newton_raphson_boost.h`                                   |
| `boost/numeric/ublas/matrix.hpp`           | Used in `newton_raphson_boost.h`                                   |
| `boost/numeric/ublas/triangular.hpp`       | Used in `newton_raphson_boost.h`                                   |
| `boost/numeric/ublas/vector.hpp`           | Used in `boost_ode_solvers.h`                                      |
| `boost/numeric/ublas/vector_proxy.hpp`     | Used in `newton_raphson_boost.h`                                   |
| `boost/typeof/incr_registration_group.hpp` | This is needed for `boost/units` but is not exported properly      |
| `boost/graph/detail/empty_header.hpp`      | This is needed for `topological_sort` but is not exported properly |

Note that we do not actually use `boost/units`, but it is included whenever
`boost/numeric/odeint.hpp` is included, and BioCro will not compile on some
operating systems without it. Hence the need for
`boost/typeof/incr_registration_group`.

1. Run the following command, noting that the path to the temporary directory
   must exist:
   `bcp --boost="PATH_TO_BOOST_ROOT_DIRECTORY" "boost/config.hpp" "boost/graph/adjacency_list.hpp" "boost/graph/topological_sort.hpp" "boost/math/constants/constants.hpp" "boost/numeric/odeint.hpp" "boost/numeric/ublas/io.hpp" "boost/numeric/ublas/lu.hpp" "boost/numeric/ublas/matrix.hpp" "boost/numeric/ublas/triangular.hpp" "boost/numeric/ublas/vector.hpp" "boost/numeric/ublas/vector_proxy.hpp" "boost/typeof/incr_registration_group.hpp" "boost/graph/detail/empty_header.hpp" PATH_TO_TEMPORARY_DIRECTORY`

2. Copy `PATH_TO_TEMPORARY_DIRECTORY/boost` to the `inc` directory, overwriting
   any previous version of `boost`. Other files and directories my be created in
   `PATH_TO_TEMPORARY_DIRECTORY,` but they are not needed.

3. Check that the Boost license in `inc` is correct for the version used, and
   update the package `LICENSE` file if necessary.

4. Update the path to the Boost license in the package `LICENSE` file.

5. Run `R CMD check` and address any new warnings or errors related to the boost
   library. It is likely that the following issues will occur:

   - Some file paths will exceed the 100 character limit. Truncate any such file
     names, and be sure to update any associated `#include` directives that
     reference these files; otherwise, compilation errors will occur.

   - As of boost version 1.83, some of the boost libraries included with BioCro
     call `sprintf`, which is not allowed by CRAN. Replace any such instances of
     `sprintf` with `snprintf`.

### Notes for using bcp in Windows
First, follow the instructions in the "Getting Started on Windows"
[Boost page](https://www.boost.org/doc/libs/1_71_0/more/getting_started/windows.html).

For V1.71.0, this entails the following:
 - Install the Visual Studio 2019 Developer Command Prompt
   ([see here](https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2019)).

 - Download the full Boost library, unzip it, and put it somewhere convenient:
   e.g., `C:\Program Files\boost\boost_1_71_0`.

 - Open a VS developer prompt and cd into the boost root directory: e.g., cd
   `C:\Program Files\boost\boost_1_71_0`.

 - Type `bootstrap` and press enter. This may take a minute or so to complete.

 - Type `.\b2` and press enter. This may also take a little while.

Now the Boost libraries have been built, and we are almost ready to use `bcp`.
However, we need to explicitly build the `bcp` tool using the `Boost.Build`
tool, which is cryptically named `b2.exe`.

 - In a VS developer prompt, cd into the tools/bcp directory: e.g.,
   `cd C:\Program Files\boost\boost_1_71_0\tools\bcp`.

 - Run b2.exe using its full path: e.g., type
   `C:\"Program Files"\boost\boost_1_71_0\b2` and press enter.

 - Now bcp.exe should be in `C:\Program Files\boost\boost_1_71_0\dist\bin`.

Finally, cd into the folder that contains bcp: e.g.,
`cd C:\Program Files\boost\boost_1_71_0\dist\bin`.

Now you should be able to run the command listed as "Step 1" above from a VS
developer command prompt.
