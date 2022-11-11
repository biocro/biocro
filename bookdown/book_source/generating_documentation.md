[online docs]: https://ebimodeling.github.io/biocro-dev-documentation/
  "Online documentation for BioCro (dev version)" {target="_blank"}

[master branch online docs]: https://ebimodeling.github.io/biocro-dev-documentation/master/pkgdown/index.html
  "Online documentation for BioCro master branch (dev version)" {target="_blank"}

# Generating Documentation

## Online documentation {#sec:on-line_documentation}

Before we discuss generating documentation, we reiterate that you
rarely need to.  That is because the documentation is automatically
generated for you and made available online!

The documentation landing page at
[https://ebimodeling.github.io/biocro-dev-documentation/][online docs]
links to up-to-date documentation for the latest version of BioCro on
the _master_ branch (of the `ebimodeling/biocro-dev` GitHub
repository), plus select documentation for versions on other branches.

## When to generate documentation {#sec:when-to-generate-docs}

For the most part, developers should only need to generate
documentation when they wish to check how changes made to it will be
rendered, and whether that rendering is more or less correct.  Changes
a developer might make to the documentation include:

* Changes to Doxygen-style comments in the C++ source code files

* Changes to R function and data documentation (the files in the `man`
  directory)

* Changes to any of the Markdown (.md) or R Markdown (.Rmd) files
  included in the Bookdown book (_this_ book).  (A list of all files
  used in forming the Bookdown book is in the configuration file
  `bookdown/_bookdown.yml`.)

* Changes to the vignettes

In most other cases, developers can simply consult the
automatically-generated online version of the documentation.  