# This script should be run by calling the following from an R session running
# in the BioCro R package root directory:
#
#   source("script/apply_styler.R")

if (!require(styler)) {
    stop(
        "Please install the `styler` package; this can be done from within R",
        " by typing `install.packages('styler')`"
    )
}

styler::style_pkg(indent_by = 4L)
