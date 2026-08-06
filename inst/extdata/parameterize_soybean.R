# This script is used to parameterize a particular BioCro model called
# "Soybean-BioCro," which was originally published in Matthews et al. (2022)
# (https://doi.org/10.1093/insilicoplants/diab032). The model is included with
# the main BioCro R package and can be found in `data/soybean2.R`.
#
# In the original paper, the model was parameterized and tested using biomass
# data collected at the SoyFACE facility during the years 2002 - 2006. The
# parameterization procedure only used biomass from the ambient CO2 rings
# collected during 2002 and 2005. See the original publication for more details,
# including the cultivars that were used during those years.
#
# Over time, some of the modules that form Soybean-BioCro have changed their
# behavior, necessitating re-parameterizations. The optimization procedure has
# also been improved, such as the inclusion of LAI values. This script can be
# used to re-parameterize Soybean-BioCro using the latest modules and
# optimization approach, producing a model that is comparable to the one from
# the original paper.
#
# To be extra clear, this is not a general-purpose script for optimizing *any*
# BioCro model of soybean growth. It is specialized to Soybean-BioCro, as
# described above. It would need to be altered in order to use it with another
# data set.
#
# To use this script, it is first necessary to specify values of SEED (the seed
# to use for randomization) and NCORES (the number of processor cores to use for
# parallel operation). Then, the script can be run using `source`. For example,
# to use a single seed and all but one available core, type:
#
#   NCORES <- parallel::detectCores() - 1; SEED <- 1234; source('path/to/this_script.R')
#
# Or, to use 8 cores and run the script with two different seeds, type:
#
#   NCORES <- 8; SEED <- 1234; source('path/to/this_script.R'); SEED <- 3456; source('path/to/this_script.R')
#
# Several output files will produced in a directory called `outputs`, which will
# be created in the current working directory. Subdirectories corresponding to
# the value of SEED will also be created to avoid overwriting the outputs.
#
# If the results are satisfactory, copy the resulting `soybean2.R` file to the
# `data` directory of the BioCro repository.

###
### Preliminaries
###

# Load required libraries
library(BioCro)
library(BioCroValidation)
library(DEoptim)
library(dfoptim)
library(lattice)
library(parallel)
library(PhotoGEA)

# Check the BioCroValidation version
expected_version  <- '0.3.0-2'
installed_version <- as.character(packageVersion('BioCroValidation'))

if (compareVersion(expected_version, installed_version) != 0) {
  warning(
    'This script was written for BioCroValidation version ', expected_version,
    ' but version ', installed_version,
    ' is installed; this may cause unexpected errors to occur.'
  )
}

# Check for required variables
required_var <- c('NCORES', 'SEED')

var_exists <- sapply(required_var, exists)

if (any(!var_exists)) {
    missing_var <- required_var[!var_exists]
    stop(
        'The following variables are missing: ',
        paste(missing_var, collapse = ', '),
        '. See the script header for more information.'
    )
}

# Clear the workspace of everything except the required variables
rm(list = setdiff(ls(), required_var))

# Option to use this on biocluster
RUN_ON_BIOCLUSTER = FALSE

# Choose the number of DEoptim optimizer iterations
ITERMAX <- 2000

# Choose the tolerance for the nmkb optimizer
TOL <- 1e-7

# Decide whether to run optimization stages; setting these to FALSE can be
# useful if you just want to change some plotting parameters for the figures
MAKE_NEW_CALCULATIONS_EVOLUTIONARY <- TRUE
MAKE_NEW_CALCULATIONS_NELDER_MEAD  <- TRUE

# Make sure the output directory exists
BASE_OUTPUT_DIR <- 'outputs'

if (!dir.exists(BASE_OUTPUT_DIR)) {
    dir.create(BASE_OUTPUT_DIR)
}

OUTPUT_DIR <- file.path(BASE_OUTPUT_DIR, paste0('soybean_optim_seed_', SEED))

if (!dir.exists(OUTPUT_DIR)) {
    dir.create(OUTPUT_DIR)
}

# Specify log file names
ERROR_LOG_FILE      <- file.path(OUTPUT_DIR, 'error_log.md')            # a record of any BioCro errors
TRACE_LOG_FILE      <- file.path(OUTPUT_DIR, 'trace_log.md')            # a trace of the DEoptim optimizer
TRACE_LOG_FILE_NMKB <- file.path(OUTPUT_DIR, 'trace_log_nmkb.md')       # a trace of the nmkb optimizer
COMPARE_FILE        <- file.path(OUTPUT_DIR, 'parameter_comparison.md') # a comparison of parameter values
MODEL_FILE          <- file.path(OUTPUT_DIR, 'soybean2.R')              # a script defining the optimized model

# Specify Rdata file names
RDATA_FILE      <- file.path(OUTPUT_DIR, 'optim_res.Rdata')
RDATA_FILE_NMKB <- file.path(OUTPUT_DIR, 'optim_res_nmkb.Rdata')

# Get Catm values for 2002 and 2005
Catm_2002 <- with(BioCro::catm_data, {Catm[year == '2002']})
Catm_2004 <- with(BioCro::catm_data, {Catm[year == '2004']})
Catm_2005 <- with(BioCro::catm_data, {Catm[year == '2005']})
Catm_2006 <- with(BioCro::catm_data, {Catm[year == '2005']})

###
### Prepare inputs for `objective_function`
###

# Specify the base model definition
base_model_definition <- BioCro::soybean2

# Make sure the Euler solver is used
base_model_definition$ode_solver <- default_ode_solvers[['homemade_euler']]

# Make sure the seed respiration coefficients are zero
base_model_definition$parameters$grc_grain <- 0
base_model_definition$parameters$grc_shell <- 0
base_model_definition$parameters$mrc_grain <- 0
base_model_definition$parameters$mrc_shell <- 0

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

  # Find the value of standard deviation that produces a weight of 1 with the
  # "logarithm" method
  unity_weight_stdev <- 1 / exp(1) - 1e-5

  # Define new `Root_Mg_per_ha` column, which has just one non-NA value, which
  # occurs at the time point where the observed above-ground biomass is highest.
  row_to_use <- which(data_table$AGB_Mg_per_ha == max(data_table$AGB_Mg_per_ha, na.rm = TRUE))
  data_table$Root_Mg_per_ha <- NA # Initialize all values to NA

  if (length(row_to_use) != 1) {
    stop('row_to_use for Root was not successfully found')
  }

  if (type == 'biomass') {
    # Estimate a mass at one time point
    data_table[row_to_use, 'Root_Mg_per_ha'] <-
        0.17 * data_table[row_to_use, 'AGB_Mg_per_ha']
  } else {
    # Ensure the weights for Root and LAI will be unity
    data_table[row_to_use, 'Root_Mg_per_ha'] <- unity_weight_stdev
    data_table$LAI                           <- unity_weight_stdev
  }

  # Remove columns by setting them to NULL
  data_table$DOY              = NULL
  data_table$Rep_Mg_per_ha    = NULL
  data_table$Litter_Mg_per_ha = NULL
  data_table$AGB_Mg_per_ha    = NULL

  # Return the processed table
  data_table
}

# Define a helping function to replace precipitation values in weather data,
# where precipitation in df_new will replace precipitation in df_old
replace_precip <- function(df_orig, df_new) {
  # Make sure df_new does not extend outside the time range of df_orig
  df_new_to_keep <- df_new$doy >= df_orig$doy[1] & df_new$doy <= tail(df_orig$doy, 1)
  df_new <- df_new[df_new_to_keep, ]

  # Overwrite precip in the common time range and return
  df_orig_to_overwrite <- df_orig$doy >= df_new$doy[1] & df_orig$doy <= tail(df_new$doy, 1)
  df_orig$precip[df_orig_to_overwrite] <- df_new$precip
  df_orig
}

# For 2002, we can just use the default weather data
drivers_2002 <- soybean_weather[['2002']]

# For other years, we overwrite the precipitation values with values from
# Grey et al. (2016)
drivers_2004 <- replace_precip(soybean_weather[['2004']], soyface_precip[['2004']])
drivers_2005 <- replace_precip(soybean_weather[['2005']], soyface_precip[['2005']])
drivers_2006 <- replace_precip(soybean_weather[['2006']], soyface_precip[['2006']])

# Define the data-driver pairs
data_driver_pairs <- list(
  ambient_2002 = list(
    data       = process_table(soyface_biomass[['ambient_2002']],     'biomass'),
    data_stdev = process_table(soyface_biomass[['ambient_2002_std']], 'stdev'),
    drivers    = drivers_2002,
    parameters = list(Catm = Catm_2002),
    weight     = 1
  ),
  ambient_2005 = list(
    data       = process_table(soyface_biomass[['ambient_2005']],     'biomass'),
    data_stdev = process_table(soyface_biomass[['ambient_2005_std']], 'stdev'),
    drivers    = drivers_2005,
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
  Stem_Mg_per_ha      = 'Stem',
  LAI                 = 'lai'
)

# Define a list of independent arguments and their initial values
independent_arg_names <- c(
  # Partitioning for leaf, root, stem, and shell
  'alphaLeaf',
  'betaLeaf',
  'alphaRoot',
  'betaRoot',
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
  'mrc_root',

  # Specific leaf area
  'iSp'
)

independent_args <- base_model_definition$parameters[independent_arg_names]

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
  TotalLitter = 0.1,
  lai         = 1.0
)

# Define an extra penalty function
extra_penalty_function <- function(sim_res, long_form_data) {
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

  # Get the latest time when the observed seed mass is zero
  seed_obs      <- long_form_data[long_form_data[['quantity_name']] == 'Grain', ]
  last_seedless <- max(seed_obs[seed_obs[['quantity_value']] < 0.1, 'time'])

  # Return a penalty if necessary
  if (is.na(time_grain) | is.na(time_leaf) | is.na(time_shell) | is.na(time_stem)) {
    # One or more tissues is not growing
    return(PENALTY)
  } else if (abs(time_leaf - time_stem) > 5 * hpd) {
    # The starts of leaf and stem growth are more than 5 days apart
    return(PENALTY)
  } else if (time_grain < last_seedless - 14 * hpd) {
    # Seeds have started growing too early (more than 2 weeks before the last
    # seedless day in the observations)
    return(PENALTY)
  } else {
    # No problems were detected
    return(0.0)
  }
}

###
### Create the objective function
###

# Specify some bounds
aul <- 50   # Upper limit for alpha parameters
bll <- -50  # Lower limit for beta parameters
mll <- 1e-5 # Lower limit for mrc parameters
mul <- 1e-2 # Upper limit for mrc parameters

# Define a table with the bounds in the same order as `independent_args`
bounds <- bounds_table(
  independent_args,
  list(
    alphaLeaf     = c(0,      aul),
    alphaStem     = c(0,      aul),
    alphaShell    = c(0,      aul),
    alphaRoot     = c(0,      aul),
    alphaSeneLeaf = c(0,      aul),
    alphaSeneStem = c(0,      aul),
    betaLeaf      = c(bll,    0),
    betaStem      = c(bll,    0),
    betaShell     = c(bll,    0),
    betaRoot      = c(bll,    0),
    betaSeneLeaf  = c(bll,    0),
    betaSeneStem  = c(bll,    0),
    rateSeneLeaf  = c(0,      0.0125),
    rateSeneStem  = c(0,      0.005),
    mrc_leaf      = c(mll,    mul),
    mrc_root      = c(mll,    mul),
    grc_stem      = c(8e-4,   0.08),
    grc_root      = c(0.0025, 0.075),
    iSp           = c(2.5,    3.5)
  )
)

# Specify objective function settings
normalization_method <- 'mean_max'
stdev_weight_method  <- 'logarithm'
stdev_weight_param   <- 1e-5
regularization_method <- 'none'

# Create the objective function
obj_fun <- objective_function(
  base_model_definition,
  data_driver_pairs,
  independent_args,
  quantity_weights,
  data_definitions       = data_definitions,
  normalization_method   = normalization_method,
  stdev_weight_method    = stdev_weight_method,
  stdev_weight_param     = stdev_weight_param,
  regularization_method  = regularization_method,
  dependent_arg_function = dependent_arg_function,
  post_process_function  = post_process_function,
  extra_penalty_function = extra_penalty_function
)

###
### Use an evolutionary optimizer to get a good guess
###

if (MAKE_NEW_CALCULATIONS_EVOLUTIONARY) {
    # Remove any previous log files
    if (file.exists(ERROR_LOG_FILE)) {
        file.remove(ERROR_LOG_FILE)
    }

    if (file.exists(TRACE_LOG_FILE)) {
        file.remove(TRACE_LOG_FILE)
    }

    # Specify cores for parallel operation, and store any messages in a dedicated
    # log file
    cl = makeCluster(NCORES, outfile = ERROR_LOG_FILE)

    # Set a seed
    set.seed(SEED)

    parVars <- c(
        'base_model_definition',
        'data_driver_pairs',
        'independent_args',
        'quantity_weights',
        'data_definitions',
        'normalization_method',
        'stdev_weight_method',
        'regularization_method',
        'dependent_arg_function',
        'post_process_function',
        'extra_penalty_function'
    )

    if (RUN_ON_BIOCLUSTER) {
      # Broadcast the vars to cluster
      clusterExport(cl, parVars, envir = environment())
    }

    # Run the optimizer, storing its "trace" outputs in a dedicated log file
    sink(TRACE_LOG_FILE)

    optim_result <- DEoptim(
        fn = obj_fun,
        lower = bounds$lower,
        upper = bounds$upper,
        control = list(
            itermax = ITERMAX,
            parallelType = 1,
            parVar=parVars,
            cluster = cl,
            trace = 1
        )
    )

    # Stop the cluster; if the workers have already been terminated, this will
    # cause an error, so we wrap it in tryCatch
    tryCatch(
        stopCluster(cl),
        error = function(e) {}
    )

    sink()

    # Save the results
    save(optim_result, file = RDATA_FILE)
} else {
    load(RDATA_FILE)
}

optim_param <- optim_result$optim$bestmem

###
### Use a Nelder-Mead optimizer to improve on the best guess from the
### evolutionary optimizer
###

if (MAKE_NEW_CALCULATIONS_NELDER_MEAD) {
    # Remove any previous log files
    if (file.exists(TRACE_LOG_FILE_NMKB)) {
        file.remove(TRACE_LOG_FILE_NMKB)
    }

    # Run the optimizer, storing its "trace" outputs in a dedicated log file
    sink(TRACE_LOG_FILE_NMKB)

    optim_result_nmkb <- nmkb(
        as.numeric(optim_param),
        obj_fun,
        lower = bounds$lower,
        upper = bounds$upper,
        control = list(
            tol = TOL,
            maxfeval = 50000,
            restarts.max = 10,
            trace = TRUE
        ),
        debug_mode = FALSE # passed to obj_fun
    )

    sink()

    # Save the results
    save(optim_result_nmkb, file = RDATA_FILE_NMKB)
} else {
    load(RDATA_FILE_NMKB)
}

optim_param_nmkb <- optim_result_nmkb$par

###
### Check and record the new values
###

# Create a table of the various independent argument values
ind_arg_table <- data.frame(
  arg_name      = independent_arg_names,
  defaults      = as.numeric(independent_args),
  optimized     = optim_param_nmkb,
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
  base_model_definition,
  independent_args,
  optim_param_nmkb,
  dependent_arg_function = dependent_arg_function
)

# Convert the re-parameterized soybean2 model to an R command string
r_cmd_string <- with(soybean_reparam, write_model(
  'soybean2',
  direct_modules,
  differential_modules,
  initial_values,
  parameters,
  ode_solver
))

# Save the model definition as an R file in the output directory
writeLines(r_cmd_string, MODEL_FILE)

###
### Visualize results
###

# Define a helper function that runs a single model for a single year and adds
# a total litter column
run_soybean <- function(model_definition, drivers, Catm_year) {
  tmp_res <- with(model_definition, {run_biocro(
    initial_values,
    within(parameters, {Catm = Catm_year}),
    drivers,
    direct_modules,
    differential_modules,
    ode_solver
  )})

  within(tmp_res, {TotalLitter = LeafLitter + StemLitter})
}

# Run each model for 2002 and 2005 and combine the results by year
full_res_2002 <- rbind(
  within(run_soybean(base_model_definition, data_driver_pairs$ambient_2002$drivers, Catm_2002), {model = 'Default Soybean-BioCro'}),
  within(run_soybean(soybean_reparam,       data_driver_pairs$ambient_2002$drivers, Catm_2002), {model = 'Re-parameterized Soybean-BioCro'})
)

full_res_2005 <- rbind(
  within(run_soybean(base_model_definition, data_driver_pairs$ambient_2005$drivers, Catm_2005), {model = 'Default Soybean-BioCro'}),
  within(run_soybean(soybean_reparam,       data_driver_pairs$ambient_2005$drivers, Catm_2005), {model = 'Re-parameterized Soybean-BioCro'})
)

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

# Plot a comparison of the original and re-optimized versions of the model for
# the years used in the parameterization
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
    file = file.path(OUTPUT_DIR, 'soybean_validation_2002.pdf')
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
    file = file.path(OUTPUT_DIR, 'soybean_validation_2005.pdf')
)

# Helping function to run a model each year for ambient and elevated CO2
# conditions and plot the results
run_all_conditions <- function(model_def, model_name) {
    delta_eCO2 <- 180

    model_full_res <- rbind(
      within(run_soybean(model_def, drivers_2002, Catm_2002 + delta_eCO2), {model = 'elevated_2002'}),
      within(run_soybean(model_def, drivers_2002, Catm_2002),              {model = 'ambient_2002'}),
      within(run_soybean(model_def, drivers_2004, Catm_2004 + delta_eCO2), {model = 'elevated_2004'}),
      within(run_soybean(model_def, drivers_2004, Catm_2004),              {model = 'ambient_2004'}),
      within(run_soybean(model_def, drivers_2005, Catm_2005 + delta_eCO2), {model = 'elevated_2005'}),
      within(run_soybean(model_def, drivers_2005, Catm_2005),              {model = 'ambient_2005'}),
      within(run_soybean(model_def, drivers_2006, Catm_2006 + delta_eCO2), {model = 'elevated_2006'}),
      within(run_soybean(model_def, drivers_2006, Catm_2006),              {model = 'ambient_2006'})
    )

    PhotoGEA::pdf_print(
        lattice::xyplot(
            Leaf + Stem + Root + Grain + Shell + TotalLitter ~ fractional_doy | model,
            data = model_full_res,
            type = 'l',
            auto.key = list(space = 'top'),
            xlab = 'Day of year',
            ylab = 'Biomass (Mg / ha)',
            ylim = c(-0.5, 9.5),
            main = model_name,
            par.settings = list(
                superpose.line = list(col = cols)
            ),
            models = model_full_res$model,
            panel = function(...) {
                # Get info about this model
                args <- list(...)
                model <- args$models[args$subscripts][1]

                # Get the corresponding biomass data set
                bmass       <- process_table(soyface_biomass[[model]],                 'biomass')
                bmass_stdev <- process_table(soyface_biomass[[paste0(model, '_std')]], 'stdev')

                # Plot the measured data points
                plot_biomass_points(bmass$Leaf_Mg_per_ha,      bmass_stdev$Leaf_Mg_per_ha,      bmass$time, cols[1])
                plot_biomass_points(bmass$Stem_Mg_per_ha,      bmass_stdev$Stem_Mg_per_ha,      bmass$time, cols[2])
                plot_biomass_points(bmass$Root_Mg_per_ha,      bmass_stdev$Root_Mg_per_ha,      bmass$time, cols[3])
                plot_biomass_points(bmass$Seed_Mg_per_ha,      bmass_stdev$Seed_Mg_per_ha,      bmass$time, cols[4])
                plot_biomass_points(bmass$Shell_Mg_per_ha,     bmass_stdev$Shell_Mg_per_ha,     bmass$time, cols[5])
                plot_biomass_points(bmass$CumLitter_Mg_per_ha, bmass_stdev$CumLitter_Mg_per_ha, bmass$time, cols[6])

                # Plot the simulation results
                lattice::panel.xyplot(...)
            },
            layout = c(4, 2)
        ),
        width = 12,
        save_to_pdf = TRUE,
        file = file.path(OUTPUT_DIR, paste0('soybean_validation_', model_name, '.pdf'))
    )
}

# Run both models each year for ambient and elevated CO2 conditions
run_all_conditions(base_model_definition, 'original')
run_all_conditions(soybean_reparam,       'reparameterized')
