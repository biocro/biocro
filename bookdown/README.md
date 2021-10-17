## Compiling the bookdown book

To generate the *bookdown* developer manual, do as follows:

1. Install Pandoc, if it is not already on your system.  See
https://pandoc.org/installing.html for instructions.  (Note to RStudio
users: As mentioned in the _R Markdown Cookbook_
(https://bookdown.org/yihui/rmarkdown-cookbook/install-pandoc.html),
RStudio comes with its own copy of Pandoc, so you may be able to get
by without installing it separately.)

2. Start an R session.

3. Install the R "bookdown" package, if it hasn't been installed
already.  These instructions are written for bookdown version 0.22 or
greater but may work for other versions.

4. Call either `bookdown::render_book(<path>)` or
`rmarkdown::render_site(<path>)`, where `<path>` denotes the path from
the current directory to the _bookdown_ directory (i.e., *this*
directory).  If either command is run *from* this directory
(_bookdown_), the path argument may be omitted.

Note: The `bookdown::render_book()` function does not accept a
`<path>` argument in versions of bookdown below version 0.22.  To
compile the book with `render_book` in those versions, either run
`bookdown::render_book()` from the _bookdown_ directory or take
advantage of the "xfun::in_dir" function and call
`xfun::in_dir('<path>', bookdown::render_book())`, where `<path>`
denotes the path from the current directory to the bookdown directory
(*this* directory).

Alternatively, if you are using a Linux or macOS machine, you can take
advantage of the `run_bookdown.sh` script in the
_documentation/script_ directory: Simply go to that directory in a
terminal program and type `./run_bookdown.sh`.

Note: Because some sections of the book are contained in their own
files rather than being in a larger file comprising a complete
chapter, you will get a warning such as the following, which may be
safely ignored:

"In split_chapters(output, gitbook_page, number_sections, split_by,  :
  You have 11 Rmd input file(s) but only 7 first-level heading(s). Did you forget first-level headings in certain Rmd files?"

4. In a Web browser, open _bookdown/_book/index.html_.
