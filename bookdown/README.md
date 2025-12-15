<!-- external references -->

[online docs]: https://biocro.org
  "Online documentation for BioCro" {target="_blank"}


## Compiling and viewing the __bookdown__ book

---

_Note_: A copy of the __bookdown__ BioCro developer's manual is
automatically generated on BioCro's GitHub documentation site at
[https://biocro.org][online docs] and may be viewed by clicking
on the _Developer's Manual_ menu-bar link found there.  What follows,
therefore, will mostly be of interest to developers working on
revising the documentation who want to check the results of their
revisions.

---

**To generate the _bookdown_ BioCro developer's manual**, do as
  follows:

1. Install _Pandoc_, if it is not already on your system.  See
https://pandoc.org/installing.html for instructions.  (Note to
_RStudio_ users: As mentioned in the _R Markdown Cookbook_
(https://bookdown.org/yihui/rmarkdown-cookbook/install-pandoc.html),
_RStudio_ comes with its own copy of _Pandoc_, so you may be able to
get by without installing it separately.)

2. Install the R __bookdown__ package, if it hasn't been installed
already.  These instructions are written for __bookdown__ version 0.22 or
greater but may work for other versions.

3. In the `bookdown` directory of your BioCro source tree, run
   ```sh
   Rscript -e "bookdown::render_book()"
   ```

   Note: If you wish to run `render_book` from other than the
`bookdown` directory, you may pass a _path_ argument:

   ```
   Rscript -e "bookdown::render_book(<path>)
   ```

   Here, `<path>` denotes the path from the current directory to the
`bookdown` directory.

   _This only works in __bookdown__ versions 0.22 and later!_ With earlier
versions, you can make use of the `xfun::in_dir` function:

   ```
   xfun::in_dir('<path>', bookdown::render_book())
   ```

   Again, `<path>` here denotes the path from the current directory to
the `bookdown` directory.

   Note: Because some sections of the book are contained in their own
files rather than being in a larger file comprising a complete
chapter, `render_book` will issue a warning such as the
following, which may be safely ignored:

   > "In split_chapters(output, gitbook_page, number_sections, split_by,  :
   >
   >   You have 13 Rmd input file(s) but only 7 first-level heading(s). Did you forget first-level headings in certain Rmd files?"

4. In a Web browser, open `bookdown/_book/index.html`.
