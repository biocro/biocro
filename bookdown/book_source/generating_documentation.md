<!--  external references -->

[online docs]: https://biocro.org
  "Online documentation for BioCro" {target="_blank"}

<!-- main text -->

# Generating Documentation

## Online documentation {#sec:on-line_documentation}

Before we discuss generating documentation, we reiterate that you
rarely need to.  That is because the documentation is automatically
generated for you and made available online!

The public documentation page at [https://biocro.org][online
docs] provides up-to-date documentation for the latest release of
BioCro.  (This documentation will provide links to documentation of
older releases.)

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
