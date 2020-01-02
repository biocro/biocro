# Adding the Boost libraries
## Why this is needed
BioCro uses software from the Boost C++ libraries. Boost does not assure backward compatibility, so changes to Boost could break BioCro. Thus, we don't want to link our code to a user supplied Boost installation, and we include a version with BioCro.
Boost is very large, so we want to include only the necessary parts. This document lists steps to extract the relevant parts and update files in BioCro to use them.

## How to extract parts of Boost
Use the bcp tool included with Boost to extract parts of the Boost library. bcp accepts a list of files or modules and extracts the relevant parts of the Boost library to a directory.
The packages BioCro uses are as follows:

| Package name                                 | Notes                                                                                                                       |
| -------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------- |
| `boost/units`                                | Used in `biocro_units.h`                                                                                                    |
| `boost/typeof/incr_registration_group.hpp`   | This is needed for `boost/units` but it is not exported properly and must be explicitly specificed as of Boost version 1.71 |
| `boost/algorithm`                            | Used in `parameters.h`                                                                                                      |
| `boost/graph`                                | Used in `dependency_graph.cpp`                                                                                              |
| `boost/numeric/ublas`                        | Used in `system.h`, among others                                                                                            |
| `boost/numeric/odeint.hpp`                   | Used in `Gro.cpp`                                                                                                           |

1. Run the following command:
   - `bcp --boost="PATH_TO_BOOST_ROOT_DIRECTORY" "boost/units" "boost/typeof/incr_registration_group.hpp" "boost/algorithm" "boost/numeric/odeint.hpp" "boost/numeric/ublas" "boost/graph" PATH_TO_TEMPORARY_DIRECTORY`
2. Copy `PATH_TO_TEMPORARY_DIRECTORY/boost` to the `biocro/boost_[version_number]` directory. Other files and directories my be created in `PATH_TO_TEMPORARY_DIRECTORY,` but they are not needed.
3. If necessary, update the `PKG_CPPFLAGS` line in `biocro/src/Makevars`: e.g., `PKG_CPPFLAGS=-I"../boost_1_71_0"`
4. Check that the Boost license at `biocro/boost_[version_num]` is correct for the version used, and update `biocro/LICENSE` if necssary.
5. Update the path to the Boost license in `biocro/LICENSE`.

### Notes for using bcp in Windows
First, follow the instructions in the "Getting Started on Windows" [Boost page](https://www.boost.org/doc/libs/1_71_0/more/getting_started/windows.html).
For V1.71.0, this entails the following:
 - Install the Visual Studio 2019 Developer Command Prompt ([see here](https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2019)).
 - Download the full Boost library, unzip it, and put it somewhere convenient: e.g., `C:\Program Files\boost\boost_1_71_0`.
 - Open a VS developer prompt and cd into the boost root directory: e.g., cd `C:\Program Files\boost\boost_1_71_0`.
 - Type bootstrap and press enter. This may take a minute or so to complete.
 - Type `.\b2` and press enter. This may also take a little while.

Now the Boost libraries have been built, and we are almost ready to use bcp. However, we need to explicitly build the bcp tool using the "Boost.Build" tool, which is cryptically named "b2.exe".
 - In a VS developer prompt, cd into the tools/bcp directory: e.g., `cd C:\Program Files\boost\boost_1_71_0\tools\bcp`.
 - Run b2.exe using its full path: e.g., type `C:\"Program Files"\boost\boost_1_71_0\b2` and press enter.
 - Now bcp.exe should be in `C:\Program Files\boost\boost_1_71_0\dist\bin`.

Finally, cd into the folder that contains bcp: e.g., `cd C:\Program Files\boost\boost_1_71_0\dist\bin`.

Now you should be able to run the command listed as "Step 1" above from a VS developer command prompt.
