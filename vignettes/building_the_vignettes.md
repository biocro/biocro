## Building package vignettes {#sec:build-vignettes}

### Required software

Unless you only want to build vignettes written in Markdown (".Rmd"
files), you will need a TeX installation of some sort.  Here are two
options:

- Visit the [CTAN starter page](https://ctan.org/starter) and choose
  and install a TeX distribution designed for your platform.

- Alternatively, if you mainly want a TeX installation for use in R,
  you can install the R `tinytex` package along with some extra needed
  LaTeX packages not included in TinyTeX by default by proceeding as
  follows:

  ```r
  install.packages('tinytex')
  tinytex::install_tinytex()
  # Install a few LaTeX packages needed by the vignettes but not
  # included in tinytex:
  tinytex::tlmgr_install(c('siunitx', 'babel-english',
                           'footnotebackref', 'listings',
                           'appendix', 'pgf'))
  ```

  (If you install TeX in this way, you will either need to build the
  vignettes using one of the _Alternative options_ given below or add
  TinyTex's bin directory to your shell path.  You can find the root
  of the TinyTex installation with the R function
  `tinytex::tinytex_root()`.)

  If you use the second alternative build option listed below, you
  will also need the R package `devtools`:
  ```r
  install.packages('devtools')
  ```

### Building all vignettes at once

The simplest (but not fastest) way to build _all_ the vignettes is to
use the `pkgdown` package.

From an R session running in the root directory of the BioCro source
tree, type `pkgdown::build_site()`. This will build a local copy of
the BioCro documentation web site, incuding all vignettes in the
`vignettes/web_only` directory. (The `bookdown` book will not
automatically be built, however.)

This method doesn't require that BioCro be installed. But it installs
BioCro in a temporary location and then builds _all_ the vignettes and
help pages, and thus it can be somewhat time consuming.

### Building individual vignettes

From an R session running in `biocro/vignettes`, type
`tools::buildVignette(XXX)`, where `XXX` is the name of the
particular vignette you wish to build.  (It should have extension
`.Rnw` or `.Rmd`.)  The resulting PDF or HTML file will appear in
`biocro/vignettes`.

This method is relatively fast and so is especially useful if you
are writing a new vignette or revising an existing one. It also enables
vignettes in `vignettes/web_only` to be built. If the vignette being
built uses any BioCro code, there must be a version of BioCro installed.

### Alternative options

Here are some alternative methods of building vignettes. The following
instructions assume that the root of the BioCro source tree is a
directory named `biocro`.

- Using `R CMD build`:

  - Build the package by running `R CMD build biocro` from the command line in
    the directory containing `biocro`. This includes building the vignettes, but
    will exclude any in `vignettes/web_only`, because this directory has been
    added to BioCro's `.Rbuildignore` file.

  - Then install using `R CMD INSTALL BioCro_xxx.tar.gz`, where `xxx` is the
    version number.

  - The vignettes should now be available as HTML or PDF files located
    in `path_to_rlib/BioCro/doc`, where `path_to_rlib` is the path to
    your R library directory.  An easy way to pull up an index to the
    vignettes in a web browser is to run the command
    `browseVignettes('BioCro')` in an R session.

- Using `devtools`:

  From an R session running in any directory of the BioCro source
  tree, type `devtools::build_vignettes()`.  (Alternatively, start R
  from anywhere and pass the path to BioCro source tree as the first
  ("`pkg`") argument to `build_vignettes()`.)  This method will modify
  `.Rbuildignore` and `.gitignore`, which may be annoying.  The
  resulting HTML and PDF files will appear in the `doc` directory,
  which will be created if it doesn't already exist.

  This method doesn't require that BioCro be installed. But it builds
  and installs BioCro in a temporary location, and thus it can be
  somewhat time consuming. Moreover, by default it gives very little
  indication of build progress, and so it may be useful to override
  this default and set `quiet = FALSE` in the function argument list.
  Note that `devtools::build-vignettes()` follows the same procedure
  as `R CMD build`, so it will not build any vignettes in
  `vignettes/web_only`.
