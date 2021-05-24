context("Run test cases for all modules")

source("module_testing_helper_functions.R")  # for cases_from_csv and test_module

# The manually-created list of modules can be replaced by a call to
# `get_all_modules()` when/if all modules have .csv files associated with them
lapply(
    c(
        "thermal_time_trilinear"
    ),
    function(module) {test_module(module, cases_from_csv(module))}
)
