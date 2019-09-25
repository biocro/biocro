library(BioCro)
library(lattice)

# Define multilayer canopy inputs
multilayer_canopy_inputs <- list(
	lat = 40,						# Urbana, IL
	atmospheric_pressure = 101592,	# 30 inches of mercury in Pascals (typical in Urbana)
	doy_dbl = 172.375,				# A bright Urbana morning in 2005
	rh = 0.5407,					# A bright Urbana morning in 2005
	solar = 1550.7,					# A bright Urbana morning in 2005
	windspeed = 1.967,				# A bright Urbana morning in 2005
	LeafN = 2,						# From sorghum parameters
	chil = 1.43,					# From sorghum parameters
	heightf = 3,					# From sorghum parameters
	kd = 0.1,						# From sorghum parameters
	kpLN = 0.2,						# From sorghum parameters
	lai = 2.8						# Value from a BioCro sorghum simulation
)

# Define multilayer canopy modules
multilayer_canopy_modules = c("solar_zenith_angle", "light_macro_environment", "ten_layer_canopy_properties")

# Run the modules
print("Starting to run the multilayer canopy modules")
multilayer_canopy_results <- test_standalone_ss(multilayer_canopy_modules, multilayer_canopy_inputs, verbose = FALSE)

# Extract some of the results

# cumulative lai
cum_lai_profile <- c(
	multilayer_canopy_results$layer_cum_lai_00,
	multilayer_canopy_results$layer_cum_lai_01,
	multilayer_canopy_results$layer_cum_lai_02,
	multilayer_canopy_results$layer_cum_lai_03,
	multilayer_canopy_results$layer_cum_lai_04,
	multilayer_canopy_results$layer_cum_lai_05,
	multilayer_canopy_results$layer_cum_lai_06,
	multilayer_canopy_results$layer_cum_lai_07,
	multilayer_canopy_results$layer_cum_lai_08,
	multilayer_canopy_results$layer_cum_lai_09
)

# rh
rh_profile <- c(
	multilayer_canopy_results$layer_rh_00,
	multilayer_canopy_results$layer_rh_01,
	multilayer_canopy_results$layer_rh_02,
	multilayer_canopy_results$layer_rh_03,
	multilayer_canopy_results$layer_rh_04,
	multilayer_canopy_results$layer_rh_05,
	multilayer_canopy_results$layer_rh_06,
	multilayer_canopy_results$layer_rh_07,
	multilayer_canopy_results$layer_rh_08,
	multilayer_canopy_results$layer_rh_09
)

# LeafN
LeafN_profile <- c(
	multilayer_canopy_results$layer_LeafN_00,
	multilayer_canopy_results$layer_LeafN_01,
	multilayer_canopy_results$layer_LeafN_02,
	multilayer_canopy_results$layer_LeafN_03,
	multilayer_canopy_results$layer_LeafN_04,
	multilayer_canopy_results$layer_LeafN_05,
	multilayer_canopy_results$layer_LeafN_06,
	multilayer_canopy_results$layer_LeafN_07,
	multilayer_canopy_results$layer_LeafN_08,
	multilayer_canopy_results$layer_LeafN_09
)

# windspeed
windspeed_profile <- c(
	multilayer_canopy_results$layer_windspeed_00,
	multilayer_canopy_results$layer_windspeed_01,
	multilayer_canopy_results$layer_windspeed_02,
	multilayer_canopy_results$layer_windspeed_03,
	multilayer_canopy_results$layer_windspeed_04,
	multilayer_canopy_results$layer_windspeed_05,
	multilayer_canopy_results$layer_windspeed_06,
	multilayer_canopy_results$layer_windspeed_07,
	multilayer_canopy_results$layer_windspeed_08,
	multilayer_canopy_results$layer_windspeed_09
)

# Iscat
Iscat_profile <- c(
	multilayer_canopy_results$layer_Iscat_00,
	multilayer_canopy_results$layer_Iscat_01,
	multilayer_canopy_results$layer_Iscat_02,
	multilayer_canopy_results$layer_Iscat_03,
	multilayer_canopy_results$layer_Iscat_04,
	multilayer_canopy_results$layer_Iscat_05,
	multilayer_canopy_results$layer_Iscat_06,
	multilayer_canopy_results$layer_Iscat_07,
	multilayer_canopy_results$layer_Iscat_08,
	multilayer_canopy_results$layer_Iscat_09
)

# Fsunlit
Fsun_profile <- c(
	multilayer_canopy_results$layer_Fsun_00,
	multilayer_canopy_results$layer_Fsun_01,
	multilayer_canopy_results$layer_Fsun_02,
	multilayer_canopy_results$layer_Fsun_03,
	multilayer_canopy_results$layer_Fsun_04,
	multilayer_canopy_results$layer_Fsun_05,
	multilayer_canopy_results$layer_Fsun_06,
	multilayer_canopy_results$layer_Fsun_07,
	multilayer_canopy_results$layer_Fsun_08,
	multilayer_canopy_results$layer_Fsun_09
)

# height
height_profile <- c(
	multilayer_canopy_results$layer_height_00,
	multilayer_canopy_results$layer_height_01,
	multilayer_canopy_results$layer_height_02,
	multilayer_canopy_results$layer_height_03,
	multilayer_canopy_results$layer_height_04,
	multilayer_canopy_results$layer_height_05,
	multilayer_canopy_results$layer_height_06,
	multilayer_canopy_results$layer_height_07,
	multilayer_canopy_results$layer_height_08,
	multilayer_canopy_results$layer_height_09
)

# Create some plots
rh_test <- xyplot(rh_profile ~ cum_lai_profile, type='b', pch=16, xlab="cumulative lai (integrated from canopy bottom)", ylab="residual humidity", main="residual humidity should decrease with cumulative lai")
LeafN_test <- xyplot(LeafN_profile ~ cum_lai_profile, type='b', pch=16, xlab="cumulative lai (integrated from canopy bottom)", ylab="leaf nitrogen", main="nitrogen should increase with cumulative lai")
windspeed_test <- xyplot(windspeed_profile ~ cum_lai_profile, type='b', pch=16, xlab="cumulative lai (integrated from canopy bottom)", ylab="windspeed", main="windspeed should increase with cumulative lai")
Iscat_test <- xyplot(Iscat_profile ~ cum_lai_profile, type='b', pch=16, xlab="cumulative lai (integrated from canopy bottom)", ylab="intensity of scattered light", main="scattered light should be lowest at low cum. lai")
Fsun_test <- xyplot(Fsun_profile ~ cum_lai_profile, type='b', pch=16, xlab="cumulative lai (integrated from canopy bottom)", ylab="fraction of sunlit leaves", main="the fraction of sunlit leaves should increase with cumulative lai")

# Help the user check to make sure that the output is sensible
print("Checking the output... see plot titles for instructions.")
x11(); print(rh_test)
x11(); print(LeafN_test)
x11(); print(windspeed_test)
x11(); print(Iscat_test)
x11(); print(Fsun_test)

# Run some similar calculations using the older c4_canopy code
c4_canopy_inputs <- get_module_info("c4_canopy")

c4_canopy_inputs$lai = 2.8
c4_canopy_inputs$doy_dbl = 172.375
c4_canopy_inputs$solar = 1550.7
c4_canopy_inputs$rh = 0.5407
c4_canopy_inputs$windspeed = 1.967
c4_canopy_inputs$lat = 40
c4_canopy_inputs$kd = 0.1
c4_canopy_inputs$chil = 1.43
c4_canopy_inputs$heightf = 3
c4_canopy_inputs$LeafN = 2
c4_canopy_inputs$kpLN = 0.2
c4_canopy_inputs$nlayers = 10

c4_canopy_results <- test_module("c4_canopy", c4_canopy_inputs)