# tl;dr
## Requirements
- The testthat package. From the R command line, run `install.packages('testthat')`.

## Running tests
Move to the "tests" directories within the BioCro source code directory, and run the following code:

```r
library(BioCro)
library(testthat)
test_dir('testthat')
```
# Running the _testthat_ Tests

BioCro's _testthat_ test suite is automatically run on GitHub as part
of the _R-CMD-check_ workflow every time a BioCro developer either pushes
code to GitHub or makes a new pull request to merge code into the
master branch.  Workflow results are viewable on GitHub under the
repository's _Actions_ tab.

There are (at least) two scenarios, however, under which you may want
to run tests manually:

1. You have revised the package's R code, stored data, or C++ code,
and you want to run the _testthat_ test suite against the changed code
before pushing that code to GitHub.

1. You are writing new tests, and you want to ensure that they work as
expected.

---

## Required R Packages
- `testthat` (To install, run `install.packages('testthat')` in R.)
- (optional) `devtools` (`install.packages('devtools')`)

(Note: Installing _devtools_ installs _testthat_ as a side-effect.)

## Running the Tests

There are two main ways to run the `testthat` tests:

- Run tests in the `tests/testthat` directory of a source tree against an installed version of BioCro
- Run tests in the `tests/testthat` directory of a source tree against the package code in that source tree

(A third scenario exists—running installed _testthat_ tests against the
installed package—but currently the BioCro package does not install
any of its tests.)

### Running the test suite on the installed version of BioCro

The simplest way to do this is as follows:

1. Start an R session.

1. Run `xfun::in_dir('<path to tests directory>',
source('testthat.R'))`, where `<path to tests directory>` is the path
to the `tests` directory of a BioCro source tree.  (Alternatively, use
`setwd()` to move to the `tests` directory (if you aren't there
already) and just run `source('testthat.R')`.)

This will run the _testthat_ tests in the same way that `R CMD check` would run them.

The default output of the test method used by `testthat.R` can be
exceedingly terse, however, and so it is highly desirable to tweak the
test output of that method for interactive use.  This is done by
overriding the default reporter of the testing function using the
_reporter_ argument.

A particularly useful and informative reporter is the _Summary_
reporter.  To use it, replace step 2 above with
```r
library(BioCro)
library(testthat)
xfun::in_dir('<path to tests directory>', test_check('BioCro', reporter = 'Summary'))
```

(Note: If you are _in_ the `tests` directory, the third line can be
simply
```r
test_check('BioCro', reporter = 'Summary')
```
And if you have already sourced `testthat.R` at least once, there
is no need to reload the _BioCro_ and _testthat_ packages!)


The _Summary_ reporter is useful in two ways: It clearly indicates
testing progress by printing a character to the screen each time a
test completes ("." for success, "S" for a skipped test, "W" for a
warning, and an (error) number for a failed test).  And it prints the
name of each test file run before printing that file's _context_
message.

Setting reporter to 'Progress' yields slightly less verbose output.
Crucially and inconveniently, it doesn't print the names of the test
files being run.  On the other hand, it gives a better numerical
summary of the test results—how many tests passed, failed, or were
skipped.[^comment]

### Running the test suite against the BioCro source code {#sec:testing-local}

If you are making changes to the R code or to the C++ code (or even to
the package data) and you want to test your changes to the source code
without installing (or _re_-installing) the BioCro package, the
function to use is `test_local`.  The steps are similar to the steps
above for running `test_check`:

1. Start an R session.

1. Load the _testthat_ library: `library(testthat)`.  (Note that we
_don't_ need to load the _BioCro_ library, and there needn't even be a
copy of it installed!)

1. Run `test_local('<path to the tests directory of some BioCro source tree>')`

If you are actually _in_ the `tests` directory (either because you
started R there or because you moved there with `setwd()`), you
needn't even supply the `path` argument; you can just run
`test_local()` because the path defaults to `'.'`.

Note that this function expects to find an up-to-date copy of the
BioCro C++ library file (`BioCro.so`, or `BioCro.dll` on Windows) in
the `src` directory.  If it doesn't find it (or if it is out of date
with respect to the C++ source files), it will try to re-create it.
This will happen even if none of the tests use any of the package
code. (_So be patient if the function seems to hang for several
minutes while it does this!_)

The default reporter for `test_local` is the _Progress_ reporter, but
if you prefer the _Summary_ reporter, which gives better progress
indication and prints the name of each test file it runs, you can
switch:

```r
test_local('<path to the tests directory of some BioCro source tree>', reporter = 'Summary')`
```

## Running individual test files

While writing new tests, it is useful to be able to run a single test
file rather than the whole test suite.  The test may be run either
against the installed version of the BioCro package, or against the
source code.

### Running an individual test file on the installed package

**Method 1**

1. Start an R session.
1. Load the _BioCro_ and _testthat_ packages:

    ```r
    library(BioCro)
    library(testthat)
    ```

1. Call the `test_file` function on the path to a test file:

    ```r
    test_file('<path to test file to run>')
    ```

Note the path passed to `test_file` can be either a relative or an
absolute path.  It doesn't matter what directory the R session was
started in or what the current R directory is as long as the path is
correct.  (Making the `testthat` directory the current directory,
however, will make for shorter path names.)

Once again, the default reporter ("CompactProgress", in this case) may
be overridden:

 ```r
 test_file('<path to test file to run>', reporter = 'Summary')
 ```

See the documentation for `testthat::Reporter` for a list of
reporters.

**Method 2**

This uses the `test_check` function we used earlier, but with a
"filter" option.

1. Start an R session.

1. Load the _BioCro_ and _testthat_ packages:

    ```r
    library(BioCro)
    library(testthat)
    ```

1. Call the `test_check` function with a filter option to select the
desired test file.  The filter pattern should be a regular expression
matching the main part of the test file.  For example, to run the
tests in `test.HarmonicOscillationModeling.R`, we could call
`test_check` as follows:

    ```r
    test_check('BioCro', filter = 'Harm')
    ```

The filter matching is performed on file names after they are stripped
of "`test-`" and "`.R`".

Again, a _reporter_ option may be specified.  For example,

```r
test_check('BioCro', filter = 'Harm', reporter = 'Summary')
```

Note that step 3 assumes you are in the BioCro `tests` directory when
`test_check` is called.  If you aren't, either use`setwd()` get there
first, or use the `xfun::in_dir` wrapper:

```r
xfun::in_dir('<path to tests directory>', test_check('BioCro', filter = 'Harm'))
```

### Running an individual test file against the package source code

Again, the `test_local` function is used.  The method is exactly the
same as specified above in section \@ref(sec:testing-local) except
that a _filter_ option is used to limit testing to matching file(s).

## Using devtools

If you don't mind installing and using the _devtools_ package, it
provides a particularly easy way to run tests against the package
source code:

1. Start an R session inside any directory in package source tree of
the source code you are testing (or `setwd()` to such a directory).[^in_dir-note]

1. Load the _devtools_ package: `library(devtools)`

1. Call `test()`.

Again, the _filter_ option may be used with this function to limit the
tests run, and the default reporter may be overridden with the
_reporter_ option.


[^comment]: The names "Progress" and "Summary" almost seem to me as if
they have been reversed!  After all, it is the _Summary_ reporter
which most clearly indicates how the testing is progressing and the
_Progress_ reporter that gives the best numerical summary of how many
tests failed.

[^in_dir-note]: Again, if the working directory is _not_ inside the
package source tree, you could use `xfun::in_dir` instead of `setwd`
to make things work:

    `xfun::in_dir('<path to some directory in the package source tree>', test())`
