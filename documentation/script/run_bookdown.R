#! /usr/bin/env Rscript

if (requireNamespace("bookdown", quietly = TRUE)) {

    if (packageVersion("bookdown") >= "0.22") {

        ## Go to the bookdown directory:
        setwd("../../bookdown")
        if (Sys.getenv("REFERENCE_ONLINE_BIOCRO_FILES")=="TRUE") {
            bookdown::render_book(params = list(base_src_url = "https://github.com/ebimodeling/biocro-dev/tree/master/src"))
        } else {
            bookdown::render_book()
        }

    } else {
        print("version 0.22 or greater of the bookdown package is required")
    }
} else {
    print("The bookdown package must be installed.")
}
