# Run "make help" to see useful Make targets, or read the section below.
#
# This makefile uses some GNU-make-specific features and therefore may
# need to be rewritten if included as part of a CRAN R package.  It
# also uses the "grouped-target" feature, introduced in GNU Make 4.3
# (19th Jan. 2020) but make work okay without it.
#
# BUGS
#
# There is not a separate rule for building latex/refman.pdf from the
# other files in the latex directory.  This means if it is
# accidentally deleted, it may not be rebuilt if the timestamp on the
# latex directory is later than the timestamps on the source files.

# Test for the presence of the "grouped-target" feature:
ifneq ($(findstring grouped-target,$(.FEATURES)),grouped-target)
    ifndef runanyway
        $(warning "This makefile uses the grouped-target feature introduced in GNU make version 4.3 and may not work as expected without it.")
        $(warning "To get rid of this warning when using earlier versions, set the variable runanyway=t on the command line.")
    endif
endif

################### Start of HELPTEXT definition. ####################
define HELPTEXT

Useful targets:

default:   Generate the HTML documentation for all files in src and its main
           subdirectories.  This is what you get if you just type "make" in
           the documentation directory.

all:       Generate both the HTML and the PDF documentation for all files in
           src and its main subdirectories.

all-html:  An alias for the default target.

all-pdf:   Generate the PDF documentation for all files in src and its main
           subdirectories.

view:      View the HTML documentation (first generating it if needed)
           produced for target all-html.

view-pdf:  View the PDF documentation (first generating it if needed)
           produced for target all-pdf.


Targets for module-only documentation:

module-docs:          Generate both the HTML and the PDF documentation for all
                      files in src/module_library.

module-docs-html:     Generate the HTML documentation for all files in
                      src/module_library.

module-docs-pdf:      Generate the PDF documentation for all files in
                      src/module_library.

view-module-docs:     View the HTML documentation (first generating it if
                      needed) produced for target module-docs-html.

view-module-docs-pdf: View the PDF documentation (first generating it if
                      needed) produced for target module-docs-pdf.


Targets for framework-only documentation:

framework-docs:          Generate both the HTML and the PDF documentation for
                         all source files other than those in
                         src/module_library.

framework-docs-html:     Generate the HTML documentation for all source files
                         other than those in src/module_library.

framework-docs-pdf:      Generate the PDF documentation for all source files
                         other than those in src/module_library.

view-framework-docs:     View the HTML documentation (first generating it if
                         needed) produced for target framework-docs-html.

view-framework-docs-pdf: View the PDF documentation (first generating it if
                         needed) produced for target framework-docs-pdf.


Targets to remove previous builds:

clean:                Remove the $(complete_docs_directory) directory and its contents.

clean-module-docs:    Remove the $(module_docs_directory) directory and its contents.

clean-framework-docs: Remove the $(framework_docs_directory) directory and its contents.

clean-all:            Remove all three build directories and their contents.

endef
#################### End of HELPTEXT definition. #####################

export HELPTEXT

#### Defaults that can be overridden on the command line:

generate_treeview = YES
color = 143
extract_private = YES

#### Defaults for target-specific variables:

source = $(all_source)
generate_html = YES
generate_latex = NO
output_directory = $(complete_docs_directory)

#### Fixed variables used in computing target-specific variables:

framework_source =  ../src ../src/framework ../src/framework_ed ../src/framework_ed/se_solver_library ../src/framework/ode_solver_library ../src/framework/utils markdown
module_source = ../src/module_library markdown
all_source = $(framework_source) $(module_source)
source_file_extensions = h hpp cpp md markdown dox
complete_docs_directory = doxygen_docs_complete
module_docs_directory = doxygen_docs_modules
framework_docs_directory = doxygen_docs_framework

#### Variables acting as functions for use with "call":

# Compute a list of prerequisite files from a list of source
# directories and the fixed list of source file extensions.
#
# arg 1 should be a list of source directories
source_prerequisites = $(wildcard $(foreach ext,$(source_file_extensions),$(patsubst %,%/*.$(ext),$(1))))

# Compute a list of output file subdirectories based on what Doxygen
# is generating.  The result generally either be 'html', 'latex', or
# 'html latex'.
#
# arg 1 should be YES if generating HTML and blank otherwise
# arg 2 should be YES if generating PDF and blank otherwise
target_subdirectories = $(strip $(if $(findstring YES,$(1)),html) $(if $(findstring YES,$(2)),latex))

# Compute a list of directory paths given a parent directory name and
# a list of subdirectory names.
#
# arg 1 should be the name of the output directory
# arg 2 should be a list of subdirectories (html, latex, or both)
target_directories = $(addprefix $(1)/,$(2))

#### Variables set from system calls:
git_hash = $(shell git show -s --format=%h)
git_date = $(shell git show -s --format=%ad)
git_status = $(shell git status | sed 's|^|<br>|' | sed 's|"|\&quot;|g')

# Canned recipe for build:

define build-recipe
( cat Doxyfile; \
  echo "GENERATE_HTML = $(generate_html)"; \
  echo "GENERATE_TREEVIEW = $(generate_treeview)"; \
  echo "GENERATE_LATEX = $(generate_latex)"; \
  echo "INPUT = $(source)"; \
  echo "ALIASES += git_hash=$(git_hash)"; \
  echo "ALIASES += git_date=\"$(git_date)\""; \
  echo "ALIASES += git_status=\"$(git_status)\""; \
  echo "OUTPUT_DIRECTORY = $(output_directory)"; \
  echo "EXTRACT_PRIVATE = $(extract_private)"; \
  echo "HTML_COLORSTYLE_HUE = $(color)" ) | doxygen -
if [ $(generate_latex) = YES ]; then \
    pushd $(output_directory)/latex; make; \
fi
endef

# The general strategy here is to control the Doxygen configuration
# using target-specific variables.  There are four Doxygen options we
# want to have be target-dependent:
#
# GENERATE_HTML    - whether or not to generate the HTML version of the
#                    documentation
#
# GENERATE_LATEX   - whether or not to generate the LaTeX (PDF) version
#                    of the documentation
#
# INPUT            - the set of source input files to document
#
# OUTPUT_DIRECTORY - where to output the generated files; generally,
#                    we want to use a different place for each
#                    different collection of input files
#
# Note that unfortunately, we can not use target-specific variable
# values to automatically set prerequiste lists, so there will be
# considerable duplication of code.
#
################################################################################

# Generally, these and only these targets should be used on the
# command line:
.PHONY: default all all-html all-pdf view view-pdf module-docs \
        module-docs-html module-docs-pdf view-module-docs \
        view-module-docs-pdf framework-docs framework-docs-html \
        framework-docs-pdf view-framework-docs view-framework-docs-pdf \
        clean clean-module-docs clean-framework-docs clean-all

default: all-html
	@echo "Run \"make help\" to see a list of useful make targets."

all: generate_latex = YES
all: $(call target_directories,$(complete_docs_directory),html latex)

# There are no target-specific variable values for this target because
# it uses the default value settings.
all-html: $(call target_directories,$(complete_docs_directory),html)

all-pdf: generate_html = NO
all-pdf: generate_latex = YES
all-pdf: $(call target_directories,$(complete_docs_directory),latex)

$(complete_docs_directory)/html $(complete_docs_directory)/latex &: $(call source_prerequisites,$(all_source))
	$(build-recipe)


view: all-html
	open $(output_directory)/html/index.html

view-pdf: all-pdf
	open $(output_directory)/latex/refman.pdf


# Targets for module-only documentation:

module-docs: source = $(module_source)
module-docs: generate_latex = YES
module-docs: output_directory = $(module_docs_directory)
module-docs: $(call target_directories,$(module_docs_directory),html latex)

module-docs-html: source = $(module_source)
module-docs-html: output_directory = $(module_docs_directory)
module-docs-html: $(call target_directories,$(module_docs_directory),html)

module-docs-pdf: source = $(module_source)
module-docs-pdf: generate_html = NO
module-docs-pdf: generate_latex = YES
module-docs-pdf: output_directory = $(module_docs_directory)
module-docs-pdf: $(call target_directories,$(module_docs_directory),latex)

$(module_docs_directory)/html $(module_docs_directory)/latex &: $(call source_prerequisites,$(module_source))
	$(build-recipe)


view-module-docs: output_directory = $(module_docs_directory)
view-module-docs: module-docs-html
	open $(output_directory)/html/index.html

view-module-docs-pdf: output_directory = $(module_docs_directory)
view-module-docs-pdf: module-docs-pdf
	open $(output_directory)/latex/refman.pdf

# Targets for framework-only documentation:

framework-docs: source = $(framework_source)
framework-docs: generate_latex = YES
framework-docs: output_directory = $(framework_docs_directory)
framework-docs: $(call target_directories,$(framework_docs_directory),html latex)

framework-docs-html: source = $(framework_source)
framework-docs-html: output_directory = $(framework_docs_directory)
framework-docs-html:  $(call target_directories,$(framework_docs_directory),html)

framework-docs-pdf: source = $(framework_source)
framework-docs-pdf: generate_html = NO
framework-docs-pdf: generate_latex = YES
framework-docs-pdf: output_directory = $(framework_docs_directory)
framework-docs-pdf: $(call target_directories,$(framework_docs_directory),latex)

$(framework_docs_directory)/html $(framework_docs_directory)/latex &:  $(call source_prerequisites,$(framework_source))
	$(build-recipe)

view-framework-docs: output_directory = $(framework_docs_directory)
view-framework-docs: framework-docs-html
	open $(output_directory)/html/index.html

view-framework-docs-pdf: output_directory = $(framework_docs_directory)
view-framework-docs-pdf: framework-docs-pdf
	open $(output_directory)/latex/refman.pdf


clean:
	rm -fr $(complete_docs_directory)

clean-module-docs:
	rm -fr $(module_docs_directory)

clean-framework-docs:
	rm -fr $(framework_docs_directory)

clean-all: clean clean-module-docs clean-framework-docs

# Make sure we don't try to compile any of the source code:
$(call source_prerequisites,$(all_source)):
	#

help:
	@echo "$$HELPTEXT" | less
