writeLines("Running unit and regression tests...")
library(testthat)
library(BioCro)

test_check("BioCro")

