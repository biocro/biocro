context("Run test cases for all modules")

source("module_testing_helper_functions.R") # for cases_from_csv and test_module

# Check test cases for all modules
lapply(
    get_all_modules(),
    function(module) {test_module(module, cases_from_csv(module))}
)
