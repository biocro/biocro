## Documentation for the C/C++ code.

To generate documentation for the C/C++ code, we use
[_Doxygen_](https://www.doxygen.nl/index.html){target="_blank"
rel="noopener"}.

### Required software

- Doxygen (version 1.9.0 or higher)

- The Graph visualization toolkit ("GraphViz"; version 2.38 or higher)
  You can get by without this if you don't care about generating the
  dependency graphs.

- To generate the PDF version of the documentation, a TeX distribution
  is required.

- Gnu Make (version 4.3 or higher[^note_make_version]) if you want to take
  advantage of the Makefile recipes

[^note_make_version]: Although versions earlier than 4.3 will probably
mostly work, you will get a warning when you use them.

**Note: _None of this is required_** if you don't need documentation
corresponding to your latest code revisions, and if you are content
with one of the four HTML versions of the documentation provided by
default.  Simply visit
[https://ebimodeling.github.io/biocro-documentation/](https://ebimodeling.github.io/biocro-documentation/){target="_blank"
rel="noopener"} to get documentation of the latest version of the code
on the GitHub _master_ branch.

### Installation

Binary distributions of Doxygen for Linux x86-64, Windows (Vista and
later), and for macOS 10.14 and later[^note_macos_version_note] are
available on the [Doxygen Downloads
page](https://www.doxygen.nl/download.html){target="_blank"
rel="noopener"}.  This page also contains information about obtaining
a source distribution of Doxygen; a set of instructions for building
and installing Doxygen from a source distribution is on the [Doxygen
Manual's _Installation_
page](https://www.doxygen.nl/manual/install.html){target="_blank"
rel="noopener"}.

[^note_macos_version_note]: To launch Doxygen's GUI front-end on a Mac
by clicking on its _Application_ icon, you will need to be using a Mac
running macOS version _10.15_ or later unless you downgrade your
Doxygen installation to version 1.18.20.  (Version 10.14 of macOS
_does_ support Doxygen 1.9's `doxygen` command-line command, however,
and it even supports the GUI front-end if it is started from the
command-line using the command `doxywizard`.  See [Doxygen issue
#8334](https://github.com/doxygen/doxygen/issues/8334){target="_blank"
rel="noopener"}.)

    Note that if you downgrade, certain reference relations may not be
shown in the generated Doxygen documentation (see [Doxygen issue
#8102](https://github.com/doxygen/doxygen/issues/8102){target="_blank"
rel="noopener"}).  Note also that a [bug in version
1.18.19](https://github.com/doxygen/doxygen/issues/7975){target="_blank"
rel="noopener"} may render most of the recipes in the provided
Make-file (`doxygen/Makefile`) unusable.

If you use a package manager—*APT* on Ubuntu or *Homebrew* on macOS,
for example—this is a relatively easy alternative to installing a
binary distribution.  For example, running

```
sudo apt-get install doxygen graphviz ghostscript
```

on Ubuntu will get you not only Doxygen, but
Ghostscript[^note_ghostscript] and the Graph visualization toolkit as
well.  (To do something similar on a Mac with Homebrew, run `brew
install doxygen graphviz ghostscript`.)

[^note_ghostscript]: _Ghostscript_ is used to convert the PostScript
files that are generated for formulas in the documentation into
bitmaps.  But _MathJax_ provides an alternative method of rendering
formulas in the HTML documentation, and so Ghostscript is unneeded as
long as the Doxygen configuration variable `USE_MATHJAX` is set to
`YES`.

    If Ghostscript _is_ used, there may be some compatibility issues
between Ghostscript and Doxygen.  If you encounter problems, see
[Doxygen issue
#7290](https://github.com/doxygen/doxygen/issues/7290){target="_blank"
rel="noopener"} and [Doxygen issue
#8107](https://github.com/doxygen/doxygen/issues/8107){target="_blank"
rel="noopener"} for further information.

### Generating the Documentation

#### Using the canned Make-file recipes

If you have _Make_ installed, the easiest way to generate the Doxygen
documentation is to run one of the several recipes in
`doxygen/Makefile`.  There are various recipes for generating _all_ of
the documentation, only the module documentation or only the framework
documentation, for documenting only public members of classes, and for
automatically opening a viewer on the just-generated documentation.
Run `make help` in the `doxygen` directory to see information about
all of the available Make targets.

#### Using the doxygen command directly

You can build the Doxygen documentation directly, without using Make,
by running the command-line command `doxygen` in the `doxygen`
directory.  With no configuration-file argument provided, this will
default to using the provided BioCro configuration file
`doxygen/Doxyfile`.

#### Using the Doxygen GUI front-end

Doxygen provides a GUI front-end for documentation generation.  To use
it, simply click (or double-click) on the _Doxygen_ icon.
Alternatively, the GUI front-end can be started from the command line
by issuing the command `doxywizard` (in any directory).  You can
specify a configuration file by passing the file name as the
(optional) argument.  For example, if you are in BioCro's `doxygen`
directory, you can do `doxywizard Doxyfile` to use the default
configuration settings for BioCro.[^note_no_doxywizard_default] If you
didn't provide an argument when starting the GUI, or if you started it
by clicking its icon, you can select the configuration file from
within the GUI program by choosing _File/Open..._ from the application
menu (shortcut _Ctrl-O_, or _Cmd-O_ on Mac) and selecting the desired
configuration file in the pop-up file browser.

[^note_no_doxywizard_default]: The `doxywizard` command doesn't treat
`Doxyfile` as a default configuration file the way the `doxygen`
command does.

#### Using your own customizations of the provided Doxyfile

If you want to customize your Doxygen builds without having to write a
new Doxyfile from scratch, the existing BioCro Doxyfile provides for
including a _customization_ file to override the settings in Doxyfile.
This works as follows:

The `doxygen/customizations` directory contains a sample customization
file named `Doxyfile_customization_sample` containing some suggestions
for customizing your own Doxygen builds.  To use this as a template,
copy it to a file named `Doxyfile` (in the same directory) and modify
it as you see fit.  The settings you set here will override the
settings specified in `doxygen/Doxyfile`.  Here are some variables
whose settings you might want to consider overriding:

* INPUT

    If you are, for example, working on the Doxygen comment for one
    particular file, you might want to override the value of the INPUT
    variable so that only the documentation for that one partiuclar
    file will be generated.  This will minimize the generation time
    and thereby expedite your tweaking the documentation so that
    appears just as you want it to appear.

* OUTPUT_DIRECTORY

    If you are generating your own version of the documentation, you
    will likely want to override this setting so that existing
    (complete) documentation is not overwritten.

* HAVE_DOT

    Setting this to NO will reduce compilation time vastly, especially
    if you are compiling from scratch, from something on the order of
    five or ten minutes to closer to five or ten seconds.  So if you
    don't care about seeing various diagrams, consider setting
    HAVE_DOT to NO.

    Diagrams of various types may be enable or disabled selectively by
    leaving HAVE_DOT set to YES and using other, more specific
    settings.  For example, you could generate class inheritance
    diagrams but disable generation of call/caller graphs.  See the
    Doxygen documentation for details, specifically, the section
    [Configuration options related to the dot
    tool](https://www.doxygen.nl/manual/config.html#config_dot){target="_blank"
    rel="noopener"}.

* GENERATE_LATEX

    If you're interested in the PDF version of the documentation, you
    may want to set this to YES.

See the sample file for a few other ideas for custom settings.

Note: Although this method of customization works with any of `make`,
`doxygen`, or `doxywizard`, certain customizations will be ignored
when using the Make-file recipes.  In particular, customizations of
the following variables will be ignored: GENERATE_HTML,
GENERATE_TREEVIEW, GENERATE_LATEX, INPUT, OUTPUT_DIRECTORY,
EXTRACT_PRIVATE, HTML_COLORSTYLE_HUE.  (This may change in the future.)

### Viewing and using the documentation

Most of the Make-file targets for generating documentation have
corresponding targets that both generate the documentation and then
automatically open a view of the finished product once generation is
complete.

Alternatively, you can locate the primary documentation file and
manually open the file in the appropriate application (a browser or a
PDF-viewer application, as the case may be).

For HTML documentation, the file to open is
`<output_directory>/html/index.html`, where `<output_directory>` is
the value of the Doxygen configuration variable `OUTPUT_DIRECTORY`.
(This defaults to the current directory if unset.)  For the PDF
documentation, the file to open is
`<output_directory>/latex/refman.pdf`.  (If you didn't use one of the
Make-file recipes to generate the PDF documentation, you will have to
run `make` in the `<output_directory>/latex` directory after running
Doxygen in order to generate the PDF files from the LaTeX files that
Doxygen generates.)

#### A note on using the Doxygen documentation to browse source code {-}

The HTML version of the Doxygen documentation provides an alternative
to [Ctags](https://en.wikipedia.org/wiki/Ctags){target="_blank"
rel="noopener"}[^note_ctags] as a convenient way to browse C++ source
code: sections documenting classes and functions provide links to the
source code where the class or function is defined.  Within the source
code, names of functions or classes are usually hyper-linked to the
file and line where they are defined.

[^note_ctags]: One major advantage of using Ctags over Doxygen for
browsing source code is that you can actually make edits to the source
code as you are browsing it.

    If you _do_ use Ctags, a useful option is the flag
"--c++-kinds=+p", which will cause Ctags to create tags for symbols in
header files.

    Emacs users may want to use Etags instead.
