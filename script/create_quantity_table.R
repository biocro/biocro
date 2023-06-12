#!/usr/bin/env Rscript --vanilla

library(argparser)
library(memoise)

module_library_name <- 'BioCro'

## If we ever relocate this script, this path may change:
module_library_directory <- file.path('..', 'src', 'module_library')

test_list_size <- 30

## This test may not be foolproof but should work 99.9% of the time:
if (basename(getwd()) != "script" || !file.exists(module_library_directory)) {
    stop("This script should be run from the '<BioCro root>/script' directory.")
}

progress <- function(message, newline = TRUE) {
    if (arg_info[['verbose']]) {
        cat(message, ifelse(newline, "\n", ""), sep = "")
    }
}

## Create a memoized version of BioCro's get_all_quantities function:
get_all_quantities <- memoise(BioCro::get_all_quantities)

## Create a memoized version of BioCro's get_all_modules function:
get_all_modules <- memoise(BioCro::get_all_modules)


## Find the base name of the file defining the module class
get_defining_file_basename <- function(module_name) {

    ## TRUE if file exists and some line in file contains pattern
    grep_file <- function(pattern, file) {
        if (!file.exists(file)) {
            return(FALSE)
        }

        results <- grep(pattern, read_lines(file))

        length(results) > 0
    }

    class_pattern <- paste0("^ *class +", module_name, " ")

    ## First search files named for module:

    if (grep_file(class_pattern, file.path(module_library_directory, paste0(module_name, ".h"))) ||
        grep_file(class_pattern, file.path(module_library_directory, paste0(module_name, ".hpp"))))
    {
        return(module_name)
    }

    ## Search failed, so search ALL module files:

    ## Files to search:
    files <- dir(module_library_directory, full.names = TRUE)

    ## Limit to header files:
    files <- files[stri_detect_regex(files, 'h(pp)?$')]

    defining_files <- files[sapply(files, function(f) grep_file(class_pattern, f))]

    ## Should be exactly one defining file:
    if (length(defining_files) == 0) {
        cat("\nNo files matching pattern \"",
            class_pattern,
            "\".\n",
            sep = "")
        stop("Is your BioCro package installation up to date with your source code?")
    } else if (length(defining_files) > 1) {
        cat("multiple defining files: ")
        print(defining_files)
        stop("Is your BioCro package installation up to date with your source code?")
    }

    basename <- basename(stri_match(defining_files, regex = "^(.*)\\.h(pp)?$")[,2])

    return(basename)
}
## Make a memoized version so we don't have to search for the same
## module twice:
mem_get_defining_file_basename <- memoise(get_defining_file_basename)


## Creates (and returns) a dataframe showing quantity names and their
## corresponding units, either by scanning C++ source file, or
## possibly by reading in a previously-generated CSV file.  If
## multiple unit designations are found for a quantity, there will be
## a row for that quantity for each unit designation.
get_or_create_units_table <- function() {

    ## See if we can use an existing table:
    csv_file <- arg_info[['csv_filename']]
    if (file.exists(csv_file)) {
        if (arg_info[['preclean']]) {
            file.remove(csv_file)
        } else {
            if (arg_info[['goal']] == "csv") {
                stop("Nothing to do.")
            } else {
                progress(
                    sprintf(
                        "Getting units table by reading existing CSV file \"%s\".",
                        csv_file
                    )
                )
                return(read.csv(csv_file))
            }
        }
    }


    ## If we get here, we have to create or recreate the units table.


    ## readr is needed for read_file
    if (!requireNamespace("readr", quietly = TRUE)) {
        stop("Package readr must be installed to produce the units table.")
    } else {
        library(readr)
    }

    ## stringi is needed for stri detect, match, and replace functions.
    if (!requireNamespace("stringi", quietly = TRUE)) {
        stop("Package stringi must be installed to produce the units table.")
    } else {
        library(stringi)
    }

    ## Function to return the units if found in a comment in a source code file
    get_units <- function(quantity_name) {

        ## Make a pattern string for matching the "base name" of the
        ## quantity--the name of the quantity with the suffix removed
        ## and with or without the prefix:

        basename_pattern_string <- "(?x)         # allow white space and comments in regex

            ^                                    # anchor the beginning so that the pattern must
                                                 # match the whole quantity name

            (?<prefixoption>((shaded|sunlit)_)?) # match group for possible prefix

            (?<basename>.*?)                     # match group for the base name
                                                 # (without either prefix or suffix);
                                                 # use '*?' for non-greedy matching so that this
                                                 # group doesn't consume the suffix (if present)

            (?:_layer_[0-9]+)?                   # use a non-capturing match group for the possible suffix
                                                 # (since it's not used in the replacement)

            $                                    # anchor the end so that the pattern must match
                                                 # the whole quantity name
        "

        base_name_matcher = stri_replace_first_regex(
            quantity_name,
            basename_pattern_string,
            "(?:${prefixoption})?${basename}"      # if the quantity name had a prefix, match it optionally;
                                                   # use a non-capturing match group
        )

        # Only search files that define modules that use the quantity:
        q <- get_all_quantities(module_library_name)
        module_names <- q[q[['quantity_name']] == quantity_name,][['module_name']]
        # Strip off library name prefix, e.g., "BioCro:total_biomass" --> "total_biomass":
        module_names <- substring(module_names, stri_length(module_library_name) + 2)

        ## For certain special cases, also look at associated modules:
        associated_modules <- list(
            ten_layer_c3_canopy = c("c3_leaf_photosynthesis", "ten_layer_canopy_properties"),
            ten_layer_c4_canopy = c("c4_leaf_photosynthesis", "ten_layer_canopy_properties"),
            ten_layer_rue_canopy = c("rue_leaf_photosynthesis", "ten_layer_canopy_properties")
        )
        additional_names <- character(0)
        for (module_name in module_names) {
            if (!is.null(associated_modules[[module_name]])) {
                additional_names <- append(additional_names, associated_modules[[module_name]])
            }
        }
        ## "unique" may not be necessary here:
        module_names <- unique(append(module_names, additional_names))


        file_names <- sapply(module_names, mem_get_defining_file_basename, USE.NAMES = FALSE)

        ## unit_vector will find all the designations of units for
        ## quantity_name; ideally, they will all be identical, i.e.,
        ## there will be exactly one unique unit designation for a
        ## given quantity
        unit_vector <- character(0)

        ## a vector of a lines of all module files
        codelines <- character(0)


        ## Now check each file in which quantity_name is used to see
        ## if we can find a unit.
        for (file_name in file_names) {
            base_file_path <- file.path(module_library_directory, file_name)

            ## Source code files have .cpp, .h, or .hpp extensions
            suffixes <- list('.cpp', '.h', '.hpp')

            ## Check each suffix - if the file exists, read in the code:
            for (suffix in suffixes) {
                path <- paste0(base_file_path, suffix)
                if (file.exists(path)) {
                    ## append lines in file to codelines vector:
                    codelines <- c(codelines, read_lines(path))
                }
            }

            ## Look for lines that look like this:
            ##
            ##     "base_name (possibly with prefix)"(possible comma)    //  units
            ##

            pattern <- paste0('"', base_name_matcher, '",? *// *(?<units>.+) *$')

            matches <- stri_match_first_regex(codelines, pattern)

            ## "units" is the first match group, therefore in the
            ## second column.  (Unfortunately, we can't refer to it
            ## using the match-group name.)
            unit_vector <- matches[,2]

            ## Only keep non-NA items:
            unit_vector <- unit_vector[!is.na(unit_vector)]
        }
        if (length(unit_vector) == 0) {
            unit_vector[1] <- 'No units found'
        }

        return(unique(unit_vector))

    } # end get_units(quantity_name)

    ## Function to get units for all quantities
    get_all_units <- function() {

        all_quantity_names <-
            unique(get_all_quantities(module_library_name)[['quantity_name']])

        unit_dictionary <-
            data.frame(quantity_name = character(0), units = character(0))

        ## Maybe truncate list for testing:
        quantity_names <- head(all_quantity_names,
                               ifelse(arg_info[['test']],
                                      test_list_size,
                                      .Machine$integer.max))

        for (name in quantity_names) {
            units <- get_units(name)
            for (unit in units) {
                progress(".", newline = FALSE)
                unit_dictionary[nrow(unit_dictionary) + 1,] <-
                    list(quantity_name = name, units = unit)
            }
        }

        unit_dictionary <-
            unit_dictionary[order(unit_dictionary[['quantity_name']]),]

        return(unit_dictionary)

    } # end get_all_units()

    write_units_table <- function(units_table, output_file) {
        write.csv(units_table, file = output_file, row.names = FALSE)
    }

    progress("GENERATING UNITS TABLE ", FALSE)
    units_table <- get_all_units()
    progress(" DONE")

    ## Maybe write units table to a file:
    if (!arg_info[['clean']] || arg_info[['goal']] == "csv") {
        progress(
            sprintf(
                "Writing units table to file \"%s\".",
                csv_file
            )
        )
        write_units_table(units_table, csv_file)
    }

    ## invisible: Don't print return value when unassigned.
    return(invisible(units_table))

} # get_or_create_units_table


##############################################################################

get_or_create_xml <- function() {
    if (!requireNamespace("xml2", quietly = TRUE)) {
        stop("Package xml2 must be installed to construct the XML document.")
    } else {
        library(xml2)
    }

    ## stringi is needed for stri detect, match, and replace functions.
    if (!requireNamespace("stringi", quietly = TRUE)) {
        stop("Package stringi must be installed to produce the units table.")
    } else {
        library(stringi)
    }


    ## See if we can use an existing XML file:
    xml_file <- arg_info[['xml_filename']]
    if (file.exists(xml_file)) {
        if (arg_info[['preclean']]) {
            file.remove(xml_file)
        } else {
            if (arg_info[['goal']] == "xml") {
                stop("Nothing to do.")
            } else {
                progress(
                    sprintf(
                        "Getting XML document by reading existing XML file \"%s\".",
                        xml_file
                    )
                )
                return(read_xml(xml_file))
            }
        }
    }


    ## If we get here, we have to create or recreate the XML document.


    ## Construct a module information table showing, for each module,
    ## its type and whether it requires a fixed-step Euler integrator.
    make_module_info_table <- function() {
        ## Maybe truncate module list for testing:
        modules <- head(get_all_modules(module_library_name),
                        ifelse(arg_info[['test']],
                               test_list_size,
                               .Machine$integer.max))

        module_table <-
            lapply(modules, function(m) {
                BioCro::module_info(m, verbose = FALSE)
            })

        ## Unlike "modules", these are the names without the
        ## library-name prefix:
        names <- sapply(module_table, function(x) x$module_name)

        types <- sapply(module_table, function(x) x$type)

        euler_required <-
            sapply(
                module_table,
                function(x) {
                    x$euler_requirement == "requires a fixed-step Euler ode_solver"
                }
            )

        return(data.frame(
            module_name = names,
            type = types,
            euler_required = euler_required
        ))
    }


    ## Create the XML document in memory.
    create_xml <- function() {

        ##
        ## Get info as data frames.
        ##

        units_table <- get_or_create_units_table()

        progress("GENERATING XML DOCUMENT ...")

        ## Get a table (data frame) of all quantities showing the modules
        ## that use it and whether it is used as input or output.  If a
        ## quantity is used in multiple contexts, a row for each context
        ## is shown:
        all_quantities <- get_all_quantities(module_library_name)

        ## Maybe truncate quantity list for testing:
        quantities <- head(all_quantities,
                           ifelse(arg_info[['test']],
                                  test_list_size,
                                  .Machine$integer.max))

        module_info_table <- make_module_info_table()



        ##
        ## Build XML document
        ##

        doc <- xml_new_root("quantity_info")

        ## Add the module library element

        module_library_element <- xml_add_child(doc, "module_library")
        xml_set_attr(module_library_element, "name", module_library_name)

        ## Construct the modules element

        modules_element <- xml_add_child(doc, "modules")

        for (i in 1:nrow(module_info_table)) {
            xml_add_child(modules_element, "module", .where = 0)
        }

        module_nodes <- xml_find_all(doc, "/quantity_info/modules/module")


        progress("Adding attributes to module nodes ... ", FALSE)

        xml_set_attr(module_nodes, "name", module_info_table$module_name)
        xml_set_attr(module_nodes, "type", module_info_table$type)
        xml_set_attr(module_nodes, "euler_required",
                     module_info_table$euler_required)

        progress("done")

        ## Construct the quantities element

        quantities_element <- xml_add_child(doc, "quantities")

        progress("Adding quantity nodes ... ", FALSE)

        for (i in 1:nrow(units_table)) {
            xml_add_child(quantities_element, "quantity", .where = 0)
        }

        progress("done")

        quantity_nodes <- xml_find_all(doc, "/quantity_info/quantities/quantity")

        progress("Adding attributes to quantity nodes ... ", FALSE)

        xml_set_attr(quantity_nodes, "name", units_table$quantity_name)
        xml_set_attr(quantity_nodes, "units", units_table$units)

        progress("done")

        ## Construct usage info element

        usage_info_element <- xml_add_child(doc, "usage_info")

        progress("Adding quantity usage nodes ... ", FALSE)

        for (i in 1:nrow(quantities)) {
            xml_add_child(usage_info_element, "quantity", .where = 0)
        }

        progress("done")

        quantity_usage_nodes <- xml_find_all(doc, "/quantity_info/usage_info/quantity")

        progress("Adding attributes to quantity usage nodes ... ", FALSE)

        ## Strip off library name prefix, e.g., "BioCro:total_biomass" --> "total_biomass":
        simple_module_names <- substring(quantities$module_name, stri_length(module_library_name) + 2)

        xml_set_attr(quantity_usage_nodes, "module_name", simple_module_names)
        xml_set_attr(quantity_usage_nodes, "quantity_name",
                     quantities$quantity_name)
        xml_set_attr(quantity_usage_nodes, "quantity_type",
                     quantities$quantity_type)

        progress("done")

        progress("... DONE GENERATING XML DOCUMENT")

        return(doc)
    }

    write_quantities_xml <- function(doc, output_file) {
        if (!requireNamespace("xml2", quietly = TRUE)) {
            stop("Package xml2 must be installed to construct the XML document.")
        } else {
            library(xml2)
        }

        write_xml(doc, output_file)
    }

    quantities_xml <- create_xml()

    ## Maybe write XML document to a file:
    if (!arg_info[['clean']] || arg_info[['goal']] == "xml") {
        progress(
            sprintf(
                "Writing XML document to file \"%s\".",
                xml_file
            )
        )
        write_quantities_xml(quantities_xml, xml_file)
    }

    ## invisible: Don't print return value when unassigned.
    return(invisible(quantities_xml))

} # get_or_create_xml

##############################################################################

xml2html <- function() {

    ## See if there is an existing HTML file:
    html_file <- arg_info[['html_filename']]
    if (file.exists(html_file)) {
        if (arg_info[['preclean']]) {
            file.remove(html_file)
        } else {
            stop(
                sprintf(
                    "\n\n%s already exists!\n%s\n%s\n\n",
                    html_file,
                    "Remove it or supply a different HTML file name to",
                    "regenerate the HTML document.")
            )
        }
    }

    if (!requireNamespace("xslt", quietly = TRUE)) {
        stop("Package xslt must be installed to generate the HTML document.")
    } else {
        library(xslt)
    }

    quantities_xml <- get_or_create_xml()
    style <- read_xml("quantities.xsl")

    progress("GENERATING HTML DOCUMENT ... ", FALSE)
    html <- xml_xslt(quantities_xml, style)
    progress("DONE")

    ## If we build the HTML document, we always write it to a file:
    progress(
        sprintf(
            "Writing HTML file \"%s\".",
            html_file
        )
    )
    xml2::write_html(html, html_file)

    ## Don't print return value:
    invisible()
}


p <- arg_parser("Create Quantity Information Files", hide.opts = TRUE)
## TO-DO: Figure out how to get the opts option to work.

## Specify script options:
goal_help <- 'The ultimate goal file of this script.
              One of csv, xml, or html.'
p <- add_argument(p, "--goal", goal_help, default = "html")

preclean_help <- "Clean intermediate files from previous runs;
                  don't use them in building the goal.
                  Otherwise, the goal will be created from intermediate files
                  if they exist."
p <- add_argument(p, "--preclean", preclean_help, flag=TRUE)

p <- add_argument(p, "--clean", "Don't write intermediate files.", flag=TRUE)

csv_help <- "The name of the CSV file (if written or read from)."
p <- add_argument(p, "--csv_filename", csv_help,
                  default = "quantity_units.csv", short = '-u')

xml_help <- "The name of the XML file (if written or read from)."
p <- add_argument(p, "--xml_filename", xml_help,
                  default = "quantities.xml")

html_help <- "The name of the HTML file (if written or read from)."
p <- add_argument(p, "--html_filename", html_help,
                  default = "quantities.html", short = '-f')

p <- add_argument(p, "--verbose", "Show script progress.", flag=TRUE)

testing_message <- "Truncate the quantities, modules, and units lists for a quick build.
                    Use for testing out this script."
p <- add_argument(p, "--test", testing_message, flag=TRUE)

## Parse options:
arg_info <- parse_args(p)


## Now, get down to work:
goal <- arg_info[['goal']]

if (goal == 'csv') {
    get_or_create_units_table()
} else {
    csv_file <- arg_info[['csv_filename']]

    if (goal == 'xml') {
        get_or_create_xml()
        if (arg_info[['clean']] && file.exists(csv_file)) {
            file.remove(csv_file)
        }
    } else if (goal == 'html') {
        xml2html()
        if (arg_info[['clean']]) {
            if (file.exists(csv_file)) {
                file.remove(csv_file)
            }
            xml_file <- arg_info[['xml_filename']]
            if (file.exists(xml_file)) {
                file.remove(xml_file)
            }
        }
    } else {
        stop(sprintf("%s not a valid goal.  Use csv, xml, or html.", goal))
    }
}

