#! /usr/bin/env Rscript

if (!requireNamespace("pkgdown", quietly = TRUE)) {
    cat("pkgdown must be installed to run this script.  Install now (~170M with dependencies)? (y/n) ")
    while (TRUE) {
        ans <- readLines(con = file("stdin"), n = 1)

        if (tolower(ans) == "y" || ans == "") {
            install.packages("pkgdown")
            break
        } else if (tolower(ans) == "n") {
            stop("Stopping.  pkgdown must be installed to run this script.")
        } else {
            cat(sprintf('"%s" is not a valid answer.  Please answer "y" or "n": ', ans))
        }

    }
}

setwd("../..")
pkgdown::build_site()
