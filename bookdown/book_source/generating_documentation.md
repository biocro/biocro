# Generating Documentation

## When to generate documentation {#sec:when-to-generate-docs}

[online docs]: https://ebimodeling.github.io/biocro-dev-documentation/
  "On-line documentation for BioCro (dev version)" {target="_blank"}

[master branch online docs]: https://ebimodeling.github.io/biocro-dev-documentation/master/pkgdown/index.html
  "On-line documentation for BioCro master branch (dev version)" {target="_blank"}

[biocro-dev workflows]: https://github.com/ebimodeling/biocro-dev/actions/workflows/document.yml
  "Documentation workflow runs" {target="_blank"}

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
automatically-generated on-line version of the documentation.  The
documentation landing page at
[https://ebimodeling.github.io/biocro-dev-documentation/][online docs]
links to up-to-date documentation for the latest version of BioCro on
the master branch (of the `ebimodeling/biocro-dev` GitHub repository),
plus select documentation for versions on other branches.

More specifically, documentation is automatically generated when

* The master branch of the repository on GitHub is updated.

  This version of the documentation lives at
  [https://ebimodeling.github.io/biocro-dev-documentation/master/pkgdown/index.html][master
  branch online docs].

* A version of the repository's history is tagged, and that tag is
  pushed to the GitHub repository.

  Each tagged version's documentation lives at a URL of the form
  `https://ebimodeling.github.io/biocro-dev-documentation/<tag
  name>/pkgdown/index.html`.

* A pull request is created or updated.

  In this case, the documentation is generated and packaged as a Zip
  file available for download but is not deployed on-line.  To
  download such a file, go to
  [https://github.com/ebimodeling/biocro-dev/actions/workflows/document.yml][biocro-dev
  workflows] and find a workflow run corresponding to the branch and
  pull request whose documentation you wish to view.  Click on it, and
  then find the artifact to download near the bottom of the page.

In addition, one can always manually trigger a workflow build of the
documentation for a specific branch and either have it deployed, or
merely built for download.  One can also choose which types of
documentation to generate.  (See the introductory section about [types
of documentation](#sec:documenation-types) for discussion of the
various types of BioCro documentation.)

If you choose to deploy the branch documentation, the result will live
at a URL of the form
`https://ebimodeling.github.io/biocro-dev-documentation/<branch
name>/pkgdown/index.html`.
