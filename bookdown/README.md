## Compiling and viewing the _bookdown_ book

---

_Note_: A copy of the _bookdown_ BioCro development manual is
automatically generated on BioCro's GitHub documentation site at
https://ebimodeling.github.io/biocro-documentation/bookdown_book/index.html.
So what follows is likely primarily of interest to developers wishing
to revise this book who want to be able to easily view the result of
their revisions before committing them.

---

**To generate the *bookdown* BioCro development manual**, do as
  follows:

1. Install _Pandoc_, if it is not already on your system.  See
https://pandoc.org/installing.html for instructions.  (Note to
_RStudio_ users: As mentioned in the _R Markdown Cookbook_
(https://bookdown.org/yihui/rmarkdown-cookbook/install-pandoc.html),
_RStudio_ comes with its own copy of _Pandoc_, so you may be able to
get by without installing it separately.)

1. Install the R _bookdown_ package, if it hasn't been installed
already.  These instructions are written for bookdown version 0.22 or
greater but may work for other versions.

1. In the _bookdown_ directory of your BioCro source tree, run
   ```sh
   Rscript -e "bookdown::render_book()"
   ```

   Note: If you wish to run `render_book` from other than the
_bookdown_ directory, you may pass a _path_ argument:

   ```
   Rscript -e "bookdown::render_book(<path>)
   ```

   Here, `<path>` denotes the path from the current directory to the
_bookdown_ directory.

   _This only works in bookdown versions 0.22 and later!_ With earlier
versions, you can make use of the `xfun::in_dir` function:

   ```
   xfun::in_dir('<path>', bookdown::render_book())
   ```

   Again, `<path>` here denotes the path from the current directory to
the _bookdown_ directory.

   Note: Because some sections of the book are contained in their own
files rather than being in a larger file comprising a complete
chapter, `render_book` will issue a warning such as the
following, which may be safely ignored:

   > "In split_chapters(output, gitbook_page, number_sections, split_by,  :
   >
   >   You have 13 Rmd input file(s) but only 7 first-level heading(s). Did you forget first-level headings in certain Rmd files?"

4. In a Web browser, open `bookdown/_book/index.html`.
