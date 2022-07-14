#! /usr/bin/env Rscript

if (requireNamespace("bookdown", quietly = TRUE)) {

    if (packageVersion("bookdown") >= "0.22") {

        ## Go to the bookdown directory:
        setwd("../../bookdown")
        bookdown::render_book()

    } else {
        print("version 0.22 or greater of the bookdown package is required")
    }
} else {
    print("The bookdown package must be installed.")
}
