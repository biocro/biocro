#! /usr/bin/env Rscript

if (requireNamespace("bookdown", quietly = TRUE)) {

    if (packageVersion("bookdown") >= "0.22") {

        ## Go to the bookdown directory:
        setwd("../../bookdown")

        ## This is automatically set to "true" when run by a GitHub action:
        if (Sys.getenv("GITHUB_ACTIONS") == "true") {
            bookdown::render_book(params = list(biocro_root = "https://github.com/biocro/biocro/tree/main"))
        } else {
            bookdown::render_book()
        }

    } else {
        print("version 0.22 or greater of the bookdown package is required")
    }
} else {
    print("The bookdown package must be installed.")
}
