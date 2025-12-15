# This script is used to parameterize a particular BioCro model called
# "Soybean-BioCro," which was originally published in Matthews et al. (2022)
# (https://doi.org/10.1093/insilicoplants/diab032). The model is included with
# the main BioCro R package and can be found in `data/soybean.R`.
#
# In the original paper, the model was parameterized and tested using biomass
# data collected at the SoyFACE facility during the years 2002 - 2006. The
# parameterization procedure only used biomass from the ambient CO2 rings
# collected during 2002 and 2005. See the original publication for more details,
# including the cultivars that were used during those years.
#
# Over time, some of the modules that form Soybean-BioCro have changed their
# behavior, necessitating re-parameterizations. This script can be used to
# re-parameterize Soybean-BioCro with the same data that was used in the
# original paper.
#
# To be extra clear, this is not a general-purpose script for optimizing *any*
# BioCro model of soybean growth. It is specialized to Soybean-BioCro, as
# described above. It would need to be altered in order to use it with another
# data set.
#
# To use this script, run it using `source`. Several output files will be
# created in the current working directory. If the results are satisfactory,
# copy the resulting `soybean.R` file to the `data` directory of the BioCro
# repository.

###
### Preliminaries
###

# Clear the workspace
rm(list=ls())

# Load required libraries
library(BioCro)
library(BioCroValidation)
library(DEoptim)
library(lattice)
library(parallel)
library(PhotoGEA)

# Specify some settings
NCORES  <- detectCores() - 1 # number of cores to use for parallel operation
ITERMAX <- 2000              # number of optimizer iterations

# Specify log file names
ERROR_LOG_FILE <- 'error_log.md'            # a record of any BioCro errors
TRACE_LOG_FILE <- 'trace_log.md'            # a trace of the optimizer
COMPARE_FILE   <- 'parameter_comparison.md' # a comparison of parameter values

# Get Catm values for 2002 and 2005
Catm_2002 <- with(BioCro::catm_data, {Catm[year == '2002']})
Catm_2005 <- with(BioCro::catm_data, {Catm[year == '2005']})

###
### Prepare inputs for `objective_function`
###

# Specify the base model definition
base_model_definition            <- soybean
base_model_definition$ode_solver <- default_ode_solvers[['homemade_euler']]

# Define a helping function for processing data tables
process_table <- function(data_table, type) {
  # Define new `time` column
  data_table$time <- (data_table$DOY - 1) * 24.0

  # Define new `Shell_Mg_per_ha` column
  data_table$Shell_Mg_per_ha <- if (type == 'biomass') {
    # The shell is all parts of the pod other than the seed
    data_table$Rep_Mg_per_ha - data_table$Seed_Mg_per_ha
  } else {
    # Add uncertainties in quadrature, a simple approach to error propagation
    sqrt(data_table$Rep_Mg_per_ha^2 + data_table$Seed_Mg_per_ha^2)
  }

  # Define new `AGB_Mg_per_ha` column (above-ground biomass)
  data_table$AGB_Mg_per_ha <-
    data_table$Leaf_Mg_per_ha + data_table$Stem_Mg_per_ha +
        data_table$Rep_Mg_per_ha

  # Define new `Root_Mg_per_ha` column, which has just one non-NA value, which
  # occurs at the time point where the observed above-ground biomass is highest.
  row_to_use <- which(data_table$AGB_Mg_per_ha == max(data_table$AGB_Mg_per_ha))
  data_table$Root_Mg_per_ha <- NA # Initialize all values to NA

  if (type == 'biomass') {
    # Estimate a mass at one time point
    data_table[row_to_use, 'Root_Mg_per_ha'] <-
        0.17 * data_table[row_to_use, 'AGB_Mg_per_ha']
  } else {
    # Estimate standard deviation at one time point
    data_table[row_to_use, 'Root_Mg_per_ha'] <- 1 / exp(1) - 1e-5
  }

  # Remove columns by setting them to NULL
  data_table$DOY              = NULL
  data_table$Rep_Mg_per_ha    = NULL
  data_table$Litter_Mg_per_ha = NULL
  data_table$AGB_Mg_per_ha    = NULL

  # Return the processed table
  data_table
}

# Define the data-driver pairs
data_driver_pairs <- list(
  ambient_2002 = list(
    data       = process_table(soyface_biomass[['ambient_2002']],     'biomass'),
    data_stdev = process_table(soyface_biomass[['ambient_2002_std']], 'stdev'),
    drivers    = BioCro::soybean_weather[['2002']],
    parameters = list(Catm = Catm_2002),
    weight     = 1
  ),
  ambient_2005 = list(
    data       = process_table(soyface_biomass[['ambient_2005']],     'biomass'),
    data_stdev = process_table(soyface_biomass[['ambient_2005_std']], 'stdev'),
    drivers    = BioCro::soybean_weather[['2005']],
    parameters = list(Catm = Catm_2005),
    weight     = 1
  )
)

# Define the post-processing function
post_process_function <- function(sim_res) {
  # Calculate the total litter as the sum of leaf and stem litter
  within(sim_res, {TotalLitter = LeafLitter + StemLitter})
}

# Define the data definition list, where the element names are columns in the
# observed data tables, and the element values are the corresponding column
# names in the model outputs
data_definitions <- list(
# Observed               Simulated
  CumLitter_Mg_per_ha = 'TotalLitter',
  Leaf_Mg_per_ha      = 'Leaf',
  Root_Mg_per_ha      = 'Root',
  Seed_Mg_per_ha      = 'Grain',
  Shell_Mg_per_ha     = 'Shell',
  Stem_Mg_per_ha      = 'Stem'
)

# Define a list of independent arguments and their initial values
independent_arg_names <- c(
  # Partitioning for leaf, stem, and shell
  'alphaLeaf',
  'betaLeaf',
  'alphaStem',
  'betaStem',
  'alphaShell',
  'betaShell',

  # Senescence for leaf and stem
  'alphaSeneLeaf',
  'betaSeneLeaf',
  'rateSeneLeaf',
  'alphaSeneStem',
  'betaSeneStem',
  'rateSeneStem',

  # Growth respiration for stem and root
  'grc_stem',
  'grc_root',

  # Maintenance respiration for leaf and root
  'mrc_leaf',
  'mrc_root'
)

independent_args <- soybean$parameters[independent_arg_names]

# Define a function that sets `mrc_stem` to the value of `mrc_leaf`
dependent_arg_function <- function(ind_args) {
  list(mrc_stem = ind_args[['mrc_leaf']])
}

# Specify the quantity weights; there is no systematic way to determine these,
# but the following weights have worked well in the past for Soybean-BioCro
quantity_weights <- list(
  Grain       = 1.0,
  Leaf        = 1.0,
  Root        = 0.1,
  Shell       = 0.5,
  Stem        = 1.0,
  TotalLitter = 0.1
)

# Define an extra penalty function
extra_penalty_function <- function(sim_res) {
  # Set the penalty value
  PENALTY <- 9999

  # Get the first times when each partitioning coefficient becomes non-zero
  k_thresh <- 0.01 # Threshold k value to decide when growth has started
  hpd      <- 24.0 # Hours per day

  time <- sim_res[['time']]

  time_grain <- time[sim_res[['kGrain']] > k_thresh][1]
  time_leaf  <- time[sim_res[['kLeaf']]  > k_thresh][1]
  time_shell <- time[sim_res[['kShell']] > k_thresh][1]
  time_stem  <- time[sim_res[['kStem']]  > k_thresh][1]

  # Return a penalty if necessary
  if (is.na(time_grain) | is.na(time_leaf) | is.na(time_shell) | is.na(time_stem)) {
    # One or more tissues is not growing
    return(PENALTY)
  } else if (abs(time_leaf - time_stem) > 5 * hpd) {
    # The starts of leaf and stem growth are more than 5 days apart
    return(PENALTY)
  } else if (time_leaf - time[1] > 20 * hpd | time_leaf - time[1] < 10 * hpd) {
    # The start of leaf growth is too late (more than 20 days after sowing) or
    # too early (fewer than 10 days after sowing)
    return(PENALTY)
  } else {
    # No problems were detected
    return(0.0)
  }
}

###
### Create the objective function
###

# Create the objective function
obj_fun <- objective_function(
  base_model_definition,
  data_driver_pairs,
  independent_args,
  quantity_weights,
  data_definitions       = data_definitions,
  normalization_method   = 'mean_max',
  stdev_weight_method    = 'logarithm',
  stdev_weight_param     = 1e-5,
  regularization_method  = 'none',
  dependent_arg_function = dependent_arg_function,
  post_process_function  = post_process_function,
  extra_penalty_function = extra_penalty_function
)

###
### Use an optimizer to choose parameter values
###

# Specify some bounds
aul <- 50   # Upper limit for alpha parameters
bll <- -50  # Lower limit for beta parameters
mll <- 1e-6 # Lower limit for mrc parameters
mul <- 1e-2 # Upper limit for mrc parameters

# Define a table with the bounds in the same order as `independent_args`
bounds <- bounds_table(
  independent_args,
  list(
    alphaLeaf     = c(0,      aul),
    alphaStem     = c(0,      aul),
    alphaShell    = c(0,      aul),
    alphaSeneLeaf = c(0,      aul),
    alphaSeneStem = c(0,      aul),
    betaLeaf      = c(bll,    0),
    betaStem      = c(bll,    0),
    betaShell     = c(bll,    0),
    betaSeneLeaf  = c(bll,    0),
    betaSeneStem  = c(bll,    0),
    rateSeneLeaf  = c(0,      0.0125),
    rateSeneStem  = c(0,      0.005),
    mrc_leaf      = c(mll,    mul),
    mrc_root      = c(mll,    mul),
    grc_stem      = c(8e-4,   0.08),
    grc_root      = c(0.0025, 0.075)
  )
)

# Specify cores for parallel operation, and store any messages in a dedicated
# log file
cl = makeCluster(NCORES, outfile = ERROR_LOG_FILE)

# Set a seed
set.seed(1234)

# Run the optimizer, storing its "trace" outputs in a dedicated log file
sink(TRACE_LOG_FILE)

optim_result <- DEoptim(
    fn = obj_fun,
    lower = bounds$lower,
    upper = bounds$upper,
    control = list(
        itermax = ITERMAX,
        parallelType = 1,
        cl = cl,
        trace = 1
    )
)

sink()

###
### Check and record the new values
###

# Create a table of the various independent argument values
ind_arg_table <- data.frame(
  arg_name      = independent_arg_names,
  defaults      = as.numeric(independent_args),
  optimized     = optim_result$optim$bestmem,
  stringsAsFactors = FALSE
)

# Add differences
ind_arg_table$optimized_diff <- with(ind_arg_table, {optimized - defaults})

# Print the comparison to a file
sink(COMPARE_FILE)
print(ind_arg_table)
sink()

# Get model definition lists for the re-parameterized version of Soybean-BioCro
soybean_reparam <- update_model(
  BioCro::soybean,
  independent_args,
  optim_result$optim$bestmem,
  dependent_arg_function = dependent_arg_function
)

# Define a helper function that runs a single model for a single year
run_soybean <- function(model_definition, year, Catm_year) {
  with(model_definition, {run_biocro(
    initial_values,
    within(parameters, {Catm = Catm_year}),
    soybean_weather[[year]],
    direct_modules,
    differential_modules,
    ode_solver
  )})
}

# Run each model for 2002 and 2005 and combine the results by year
full_res_2002 <- rbind(
  within(run_soybean(BioCro::soybean, '2002', Catm_2002), {model = 'Default Soybean-BioCro'}),
  within(run_soybean(soybean_reparam, '2002', Catm_2002), {model = 'Re-parameterized Soybean-BioCro'})
)

full_res_2005 <- rbind(
  within(run_soybean(BioCro::soybean, '2005', Catm_2005), {model = 'Default Soybean-BioCro'}),
  within(run_soybean(soybean_reparam, '2005', Catm_2005), {model = 'Re-parameterized Soybean-BioCro'})
)

# Add a total litter column
full_res_2002$TotalLitter <- full_res_2002$LeafLitter + full_res_2002$StemLitter
full_res_2005$TotalLitter <- full_res_2005$LeafLitter + full_res_2005$StemLitter

# Helper function for adding biomass values to plot
plot_biomass_points <- function(biomass, stdev, biocro_time, color) {
    day_of_year <- biocro_time / 24.0 + 1.0

    panel.points(
        biomass ~ day_of_year,
        pch = 16,
        col = color
    )

    panel.segments(
        day_of_year, biomass + stdev, day_of_year, biomass - stdev,
        col = color
    )
}

# Plot the results
cols <- PhotoGEA::multi_curve_colors()

PhotoGEA::pdf_print(
    lattice::xyplot(
        Leaf + Stem + Root + Grain + Shell + TotalLitter ~ fractional_doy | model,
        data = full_res_2002,
        type = 'l',
        auto.key = list(space = 'top'),
        xlab = 'Day of year (2002)',
        ylab = 'Biomass (Mg / ha)',
        par.settings = list(
            superpose.line = list(col = cols)
        ),
        panel = function(...) {
            bmass       <- data_driver_pairs$ambient_2002$data
            bmass_stdev <- data_driver_pairs$ambient_2002$data_stdev

            plot_biomass_points(bmass$Leaf_Mg_per_ha,      bmass_stdev$Leaf_Mg_per_ha,      bmass$time, cols[1])
            plot_biomass_points(bmass$Stem_Mg_per_ha,      bmass_stdev$Stem_Mg_per_ha,      bmass$time, cols[2])
            plot_biomass_points(bmass$Root_Mg_per_ha,      bmass_stdev$Root_Mg_per_ha,      bmass$time, cols[3])
            plot_biomass_points(bmass$Seed_Mg_per_ha,      bmass_stdev$Seed_Mg_per_ha,      bmass$time, cols[4])
            plot_biomass_points(bmass$Shell_Mg_per_ha,     bmass_stdev$Shell_Mg_per_ha,     bmass$time, cols[5])
            plot_biomass_points(bmass$CumLitter_Mg_per_ha, bmass_stdev$CumLitter_Mg_per_ha, bmass$time, cols[6])

            lattice::panel.xyplot(...)
        }
    ),
    width = 10,
    save_to_pdf = TRUE,
    file = 'soybean_validation_2002.pdf'
)


PhotoGEA::pdf_print(
    lattice::xyplot(
        Leaf + Stem + Root + Grain + Shell + TotalLitter ~ fractional_doy | model,
        data = full_res_2005,
        type = 'l',
        auto.key = list(space = 'top'),
        xlab = 'Day of year (2005)',
        ylab = 'Biomass (Mg / ha)',
        par.settings = list(
            superpose.line = list(col = cols)
        ),
        panel = function(...) {
            bmass       <- data_driver_pairs$ambient_2005$data
            bmass_stdev <- data_driver_pairs$ambient_2005$data_stdev

            plot_biomass_points(bmass$Leaf_Mg_per_ha,      bmass_stdev$Leaf_Mg_per_ha,      bmass$time, cols[1])
            plot_biomass_points(bmass$Stem_Mg_per_ha,      bmass_stdev$Stem_Mg_per_ha,      bmass$time, cols[2])
            plot_biomass_points(bmass$Root_Mg_per_ha,      bmass_stdev$Root_Mg_per_ha,      bmass$time, cols[3])
            plot_biomass_points(bmass$Seed_Mg_per_ha,      bmass_stdev$Seed_Mg_per_ha,      bmass$time, cols[4])
            plot_biomass_points(bmass$Shell_Mg_per_ha,     bmass_stdev$Shell_Mg_per_ha,     bmass$time, cols[5])
            plot_biomass_points(bmass$CumLitter_Mg_per_ha, bmass_stdev$CumLitter_Mg_per_ha, bmass$time, cols[6])

            lattice::panel.xyplot(...)
        }
    ),
    width = 10,
    save_to_pdf = TRUE,
    file = 'soybean_validation_2005.pdf'
)

# Convert the re-parameterized soybean model to an R command string
r_cmd_string <- with(soybean_reparam, write_model(
  'soybean',
  direct_modules,
  differential_modules,
  initial_values,
  parameters,
  ode_solver
))

# Save the model definition as an R file in the current working directory
writeLines(r_cmd_string, './soybean.R')
