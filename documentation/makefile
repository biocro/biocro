# Run "make help" to see useful Make targets, or read the section below.
#
# This makefile uses some GNU-make-specific features and therefore may need to
# be rewritten if included as part of a CRAN R package.


define HELPTEXT

Useful targets:

default:   Generate the HTML documentation for all files in src.  This is what
           you get if you just type "make" in the documentation directory.

all:       Generate both the HTML and the PDF documentation for all files in
           the src and src/module_library directories.

all-html:  Generate the HTML documentation for all files in the src and
           src/module_library directories.

all-pdf:   Generate the PDF documentation for all files in the src and
           src/module_library directories.

view:      View the HTML documentation (first generating it if needed)
           produced by the default target.

view-all:  View the HTML documentation (first generating it if needed) that
           includes documentation of the code in the src/module_library
           directory.

view-pdf:  View the PDF documentation, first generating it if needed.

clean:     Remove the html directory and its contents, preparing the way for a
           fresh default build.

clean-all: Remove the "complete" directory and its contents, preparing the way
           for a fresh build of the "all" target.

endef

export HELPTEXT


.PHONY: default all all-html all-pdf clean clean-all view view-all view-pdf

default: html
	@echo "Run \"make help\" to see a list of useful make targets."

html: ../src/*
	doxygen

all: complete

complete: ../src/* ../src/module_library/* ../src/system_solver_library/* ../src/utils/*
	# Override some of the settings in Doxyfile:
	( cat Doxyfile overrides/developer ) | doxygen -
	# Generate the PDF file from the LaTeX output:
	pushd complete/latex; make

all-html: complete/html

complete/html: ../src/* ../src/module_library/* ../src/system_solver_library/* ../src/utils/*
	# Override some of the settings in Doxyfile:
	( cat Doxyfile overrides/developer ; echo "GENERATE_LATEX = NO" ) | doxygen -

all-pdf: complete/latex

complete/latex: ../src/* ../src/module_library/* ../src/system_solver_library/* ../src/utils/*
	# Override some of the settings in Doxyfile:
	( cat Doxyfile overrides/developer ; echo "GENERATE_HTML = NO" ) | doxygen -
	# Generate the PDF file from the LaTeX output:
	pushd complete/latex; make

clean:
	rm -fr html

clean-all:
	rm -fr complete

view: html
	open html/index.html

view-all: all-html
	open complete/html/index.html

view-pdf: all-pdf
	open complete/latex/refman.pdf

# Make sure we don't try to compile any of the source code:
../src/* ../src/module_library/* ../src/system_solver_library/* ../src/utils/*:
	#

help:
	@echo "$$HELPTEXT"
