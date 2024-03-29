<!-- external references -->

[online docs]: https://biocro.github.io
  "Online documentation for BioCro" {target="_blank"}

[biocro auto-document workflows]:
  https://github.com/biocro/biocro/actions/workflows/automatically-call-document.yml
  "Documentation workflow runs (automatic)" {target="_blank"}

[biocro documention workflows]:
  https://github.com/biocro/biocro/actions/workflows/document.yml
  "Documentation workflow runs (manual)" {target="_blank"}

[pkgdown]: https://pkgdown.r-lib.org/index.html "The pkgdown web site" {target="_blank"}

<!-- footnotes -->

[^latest_version]: If there is more than one run for a given pull
request, you will probably want the latest one.  If the Zip file has
expired, you may have to manually call the workflow to regenerate it.

    Manually-triggered workflow runs may be found at
[https://github.com/biocro/biocro/actions/workflows/document.yml][biocro
documention workflows]

## Note about the types of documentation in BioCro {.unnumbered #sec:documenation-types}

There several categories of documentation for BioCro:

* The top-level `README.md` is for general information about BioCro
  and is intended for _potential_ users of the package.

* The files in the `man` directory document the R functions and the
  data associated with the BioCro package.  This documentation is
  intended for _users_ of the package.

* The files in the `vignettes` directory generally contain _long-form_
  documentation, also intended for _users_ of the package.

* There is documentation generated by [doxygen] from comments in the C++
  source files.  _General users_ will be interested in the
  documentation of the _BioCro module library_ modules.  _Developers_
  will be interested in the implementation of the dynamical systems
  simulator.

* Finally, there are various `.md` and `.Rmd` files scattered
  throughout the package; these comprise the content of the
  _Developer's Manual_ (_this_ manual, if you are reading this page as
  part of the __bookdown__ book).  This documentation is targeted at
  BioCro developers and maintainers.

## The online documentation {-}

All of the documentation mentioned above is available online at
[https://biocro.github.io][online docs].

### When online documentation is generated {- #sec:generation-triggers}

Documentation is automatically generated and published when a new
release is created on GitHub.  It is also generated (but not
published) when a pull request is created or updated.  In the latter
case, the documentation is packaged as a Zip file available for
download.  To download a generated Zip file, go to the [page for the
__Automatically generate documentation__ workflow][biocro
auto-document workflows] and find a workflow run corresponding to the
pull request whose documentation you wish to view.  Click on it, and
then find the artifact to download near the bottom of the
page.[^latest_version]

In addition, one can manually trigger a workflow build of the
documentation for a specific branch.  To do this, go to the [page for
the __Generate Documentation__ workflow][biocro documention workflows]
and click on the _Run workflow_ button.  Choose which branch you want
to document and which types of documentation to generate and then
click the _Run workflow_ button inside the dropdown window.  (See the
introductory section about [types of
documentation](#sec:documenation-types) for discussion of the various
types of BioCro documentation, and [the following
section](#sec:documentation-layout) for a discussion of how this
documentation is packaged.)

Once the manually-triggered workflow has finished running, click on
the link for the workflow run and look for the artifact to download at
the bottom of the page.

### The layout of the online documentation {.unnumbered #sec:documentation-layout}

The layout of the online documentation is in the form of a [pkgdown]
document.  Each of the categories of documentation listed above
corresponds to a portion of this document as follows:

* The `README.md` document appears on the pkgdown home page (_BioCro_
  in the menu bar).

* The `man` pages appear under the menu item _References_.  (Note
  that although the page heading on the Reference page says _Function
  reference_, documentation of the various data sets is included as
  well.)

* The vignettes appear under the menu item _Articles_.

* The Doxygen documentation may be accessed from the menu bar item
  _C++ Library_.  Note that there are various forms of this
  documentation of varying concision and
  comprehensiveness.

* The developer documentation (this book!) may be accessed by using
  the menu bar item _Developer's
  Manual_.

Note that the Developer's Manual, the C++ documentation, and the
PDF-style vignettes are not well integrated into pkgdown, and the user
will have to click the browser _back_ button to return to pkgdown
proper to once again have access to the pkgdown menu items.

In addition to the documentation proper, the menu bar contains some
other useful links:

* The _About_ page gives useful information about the
  documentation---namely, which branch and commit version it
  corresponds to and the commit and generation dates.

* The _Changelog_ shows the BioCro version history.

* Finally, there is a search box at the right of the menu bar.  **Note
  that this will not search the portions of the documentation not
  integrated into pkgdown, that is, it will not search the Developer's
  Manual, the C++ documentation, or the PDF-style vignettes.** And the
  search box only functions when the pkgdown document is deployed on a
  server.
