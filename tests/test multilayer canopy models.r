library(BioCro)
library(lattice)

# Define multilayer canopy inputs
multilayer_canopy_inputs <- list(
	lat = 40,							# Urbana, IL
	atmospheric_pressure = 101592,		# 30 inches of mercury in Pascals (typical in Urbana)
	doy_dbl = 172.375,					# A bright Urbana morning in 2005
	rh = 0.5407,						# A bright Urbana morning in 2005
	solar = 1550.7,						# A bright Urbana morning in 2005
	windspeed = 1.967,					# A bright Urbana morning in 2005
	LeafN = 2,							# From sorghum parameters
	chil = 1.43,						# From sorghum parameters
	heightf = 3,						# From sorghum parameters
	kd = 0.1,							# From sorghum parameters
	kpLN = 0.2,							# From sorghum parameters
	alpha_scatter = 0.8,				# Used to be hard-coded into BioCro with value 0.8
	atmospheric_transmittance = 0.85,	# Used to be hard-coded into BioCro with value 0.85 (0.85 = super clear sky)
	lai = 2.8							# Value from a BioCro sorghum simulation
)

# Define multilayer canopy modules
multilayer_canopy_modules = c("solar_zenith_angle", "light_macro_environment", "ten_layer_canopy_properties")

# Run the modules
print("Starting to run the multilayer canopy modules")
multilayer_canopy_results <- test_standalone_ss(multilayer_canopy_modules, multilayer_canopy_inputs, verbose = FALSE)

# Extract some of the results

# cumulative lai
cum_lai_profile <- c(
	multilayer_canopy_results$layer_cum_lai_0,
	multilayer_canopy_results$layer_cum_lai_1,
	multilayer_canopy_results$layer_cum_lai_2,
	multilayer_canopy_results$layer_cum_lai_3,
	multilayer_canopy_results$layer_cum_lai_4,
	multilayer_canopy_results$layer_cum_lai_5,
	multilayer_canopy_results$layer_cum_lai_6,
	multilayer_canopy_results$layer_cum_lai_7,
	multilayer_canopy_results$layer_cum_lai_8,
	multilayer_canopy_results$layer_cum_lai_9
)

# height
height_profile <- c(
	multilayer_canopy_results$layer_height_0,
	multilayer_canopy_results$layer_height_1,
	multilayer_canopy_results$layer_height_2,
	multilayer_canopy_results$layer_height_3,
	multilayer_canopy_results$layer_height_4,
	multilayer_canopy_results$layer_height_5,
	multilayer_canopy_results$layer_height_6,
	multilayer_canopy_results$layer_height_7,
	multilayer_canopy_results$layer_height_8,
	multilayer_canopy_results$layer_height_9
)

# rh
rh_profile <- c(
	multilayer_canopy_results$layer_rh_0,
	multilayer_canopy_results$layer_rh_1,
	multilayer_canopy_results$layer_rh_2,
	multilayer_canopy_results$layer_rh_3,
	multilayer_canopy_results$layer_rh_4,
	multilayer_canopy_results$layer_rh_5,
	multilayer_canopy_results$layer_rh_6,
	multilayer_canopy_results$layer_rh_7,
	multilayer_canopy_results$layer_rh_8,
	multilayer_canopy_results$layer_rh_9
)

# LeafN
LeafN_profile <- c(
	multilayer_canopy_results$layer_LeafN_0,
	multilayer_canopy_results$layer_LeafN_1,
	multilayer_canopy_results$layer_LeafN_2,
	multilayer_canopy_results$layer_LeafN_3,
	multilayer_canopy_results$layer_LeafN_4,
	multilayer_canopy_results$layer_LeafN_5,
	multilayer_canopy_results$layer_LeafN_6,
	multilayer_canopy_results$layer_LeafN_7,
	multilayer_canopy_results$layer_LeafN_8,
	multilayer_canopy_results$layer_LeafN_9
)

# windspeed
windspeed_profile <- c(
	multilayer_canopy_results$layer_windspeed_0,
	multilayer_canopy_results$layer_windspeed_1,
	multilayer_canopy_results$layer_windspeed_2,
	multilayer_canopy_results$layer_windspeed_3,
	multilayer_canopy_results$layer_windspeed_4,
	multilayer_canopy_results$layer_windspeed_5,
	multilayer_canopy_results$layer_windspeed_6,
	multilayer_canopy_results$layer_windspeed_7,
	multilayer_canopy_results$layer_windspeed_8,
	multilayer_canopy_results$layer_windspeed_9
)

# Iscat
Iscat_profile <- c(
	multilayer_canopy_results$layer_Iscat_0,
	multilayer_canopy_results$layer_Iscat_1,
	multilayer_canopy_results$layer_Iscat_2,
	multilayer_canopy_results$layer_Iscat_3,
	multilayer_canopy_results$layer_Iscat_4,
	multilayer_canopy_results$layer_Iscat_5,
	multilayer_canopy_results$layer_Iscat_6,
	multilayer_canopy_results$layer_Iscat_7,
	multilayer_canopy_results$layer_Iscat_8,
	multilayer_canopy_results$layer_Iscat_9
)

# Idiff
Idiff_profile <- c(
	multilayer_canopy_results$layer_Idiff_0,
	multilayer_canopy_results$layer_Idiff_1,
	multilayer_canopy_results$layer_Idiff_2,
	multilayer_canopy_results$layer_Idiff_3,
	multilayer_canopy_results$layer_Idiff_4,
	multilayer_canopy_results$layer_Idiff_5,
	multilayer_canopy_results$layer_Idiff_6,
	multilayer_canopy_results$layer_Idiff_7,
	multilayer_canopy_results$layer_Idiff_8,
	multilayer_canopy_results$layer_Idiff_9
)

# Isun
Isun_profile <- c(
	multilayer_canopy_results$Isun,
	multilayer_canopy_results$Isun,
	multilayer_canopy_results$Isun,
	multilayer_canopy_results$Isun,
	multilayer_canopy_results$Isun,
	multilayer_canopy_results$Isun,
	multilayer_canopy_results$Isun,
	multilayer_canopy_results$Isun,
	multilayer_canopy_results$Isun,
	multilayer_canopy_results$Isun
)

# Itot
Itot_profile <- c(
	multilayer_canopy_results$layer_Itot_0,
	multilayer_canopy_results$layer_Itot_1,
	multilayer_canopy_results$layer_Itot_2,
	multilayer_canopy_results$layer_Itot_3,
	multilayer_canopy_results$layer_Itot_4,
	multilayer_canopy_results$layer_Itot_5,
	multilayer_canopy_results$layer_Itot_6,
	multilayer_canopy_results$layer_Itot_7,
	multilayer_canopy_results$layer_Itot_8,
	multilayer_canopy_results$layer_Itot_9
)

# Fsunlit
Fsun_profile <- c(
	multilayer_canopy_results$layer_Fsun_0,
	multilayer_canopy_results$layer_Fsun_1,
	multilayer_canopy_results$layer_Fsun_2,
	multilayer_canopy_results$layer_Fsun_3,
	multilayer_canopy_results$layer_Fsun_4,
	multilayer_canopy_results$layer_Fsun_5,
	multilayer_canopy_results$layer_Fsun_6,
	multilayer_canopy_results$layer_Fsun_7,
	multilayer_canopy_results$layer_Fsun_8,
	multilayer_canopy_results$layer_Fsun_9
)

# Create some plots
rh_test <- xyplot(rh_profile ~ cum_lai_profile, type='b', pch=16, xlab="cumulative lai (integrated from canopy bottom)", ylab="residual humidity", main="residual humidity should decrease with cumulative lai")
LeafN_test <- xyplot(LeafN_profile ~ cum_lai_profile, type='b', pch=16, xlab="cumulative lai (integrated from canopy bottom)", ylab="leaf nitrogen", main="nitrogen should increase with cumulative lai")
windspeed_test <- xyplot(windspeed_profile ~ cum_lai_profile, type='b', pch=16, xlab="cumulative lai (integrated from canopy bottom)", ylab="windspeed", main="windspeed should increase with cumulative lai")
Iscat_test <- xyplot(Iscat_profile ~ cum_lai_profile, type='b', pch=16, xlab="cumulative lai (integrated from canopy bottom)", ylab="intensity of scattered light", main="scattered light should be lowest at high cum. lai")
Fsun_test <- xyplot(Fsun_profile ~ cum_lai_profile, type='b', pch=16, xlab="cumulative lai (integrated from canopy bottom)", ylab="fraction of sunlit leaves", main="the fraction of sunlit leaves should increase with cumulative lai")
Idiff_test <- xyplot(Idiff_profile ~ cum_lai_profile, type='b', pch=16, xlab="cumulative lai (integrated from canopy bottom)", ylab="diffuse light flux", main="diffuse flux should be lowest at high cum. lai")
Icompare <- xyplot(Isun_profile + Idiff_profile + Itot_profile ~ cum_lai_profile, ylim=c(0,900), type='b', pch=16, auto=TRUE, xlab="cumulative lai (integrated from canopy bottom)", ylab="light flux", main="High sun with clear sky:\natmospheric_transmittance = 0.85, solar = 1550.7,\nzenith = 41 degrees (k = 0.73), kd=0.1\n\nComparison of:\nbeam flux hitting sunlit leaves (Isun),\ndiffuse flux hitting all leaves (Idiff),\nand the effective total flux (Itot)")

# Help the user check to make sure that the output is sensible
# Some plots will only be displayed if the user has defined an object called "plotall", e.g. plotall <- 1
print("Checking the output... see plot titles for details.")
if(exists("plotall")) {
	x11(); print(rh_test)
	x11(); print(LeafN_test)
	x11(); print(windspeed_test)
	x11(); print(Iscat_test)
	x11(); print(Fsun_test)
	x11(); print(Idiff_test)
}
x11(); print(Icompare)

# Run another simulation with some different parameters

multilayer_canopy_inputs_dusk <- list(
	lat = 40,							# Urbana, IL
	atmospheric_pressure = 101592,		# 30 inches of mercury in Pascals (typical in Urbana)
	doy_dbl = 172.75,					# A dim Urbana evening in 2005
	rh = 0.8049,						# A dim Urbana evening in 2005
	solar = 447.3,						# A dim Urbana evening in 2005
	windspeed = 1.967,					# A dim Urbana evening in 2005
	LeafN = 2,							# From sorghum parameters
	chil = 1.43,						# From sorghum parameters
	heightf = 3,						# From sorghum parameters
	kd = 0.1,							# From sorghum parameters
	kpLN = 0.2,							# From sorghum parameters
	alpha_scatter = 0.8,				# Used to be hard-coded into BioCro with value 0.8
	atmospheric_transmittance = 0.4,	# Used to be hard-coded into BioCro with value 0.85 (0.4 = overcast sky)
	lai = 3.1							# Value from a BioCro sorghum simulation
)

print("Starting to run the multilayer canopy modules with different parameters")
multilayer_canopy_results_dusk <- test_standalone_ss(multilayer_canopy_modules, multilayer_canopy_inputs_dusk, verbose = FALSE)

# Extract some of the results

# cumulative lai
cum_lai_profile_dusk <- c(
	multilayer_canopy_results_dusk$layer_cum_lai_0,
	multilayer_canopy_results_dusk$layer_cum_lai_1,
	multilayer_canopy_results_dusk$layer_cum_lai_2,
	multilayer_canopy_results_dusk$layer_cum_lai_3,
	multilayer_canopy_results_dusk$layer_cum_lai_4,
	multilayer_canopy_results_dusk$layer_cum_lai_5,
	multilayer_canopy_results_dusk$layer_cum_lai_6,
	multilayer_canopy_results_dusk$layer_cum_lai_7,
	multilayer_canopy_results_dusk$layer_cum_lai_8,
	multilayer_canopy_results_dusk$layer_cum_lai_9
)

# Idiff
Idiff_profile_dusk <- c(
	multilayer_canopy_results_dusk$layer_Idiff_0,
	multilayer_canopy_results_dusk$layer_Idiff_1,
	multilayer_canopy_results_dusk$layer_Idiff_2,
	multilayer_canopy_results_dusk$layer_Idiff_3,
	multilayer_canopy_results_dusk$layer_Idiff_4,
	multilayer_canopy_results_dusk$layer_Idiff_5,
	multilayer_canopy_results_dusk$layer_Idiff_6,
	multilayer_canopy_results_dusk$layer_Idiff_7,
	multilayer_canopy_results_dusk$layer_Idiff_8,
	multilayer_canopy_results_dusk$layer_Idiff_9
)

# Isun
Isun_profile_dusk <- c(
	multilayer_canopy_results_dusk$Isun,
	multilayer_canopy_results_dusk$Isun,
	multilayer_canopy_results_dusk$Isun,
	multilayer_canopy_results_dusk$Isun,
	multilayer_canopy_results_dusk$Isun,
	multilayer_canopy_results_dusk$Isun,
	multilayer_canopy_results_dusk$Isun,
	multilayer_canopy_results_dusk$Isun,
	multilayer_canopy_results_dusk$Isun,
	multilayer_canopy_results_dusk$Isun
)

# Iscat
Itot_profile_dusk <- c(
	multilayer_canopy_results_dusk$layer_Itot_0,
	multilayer_canopy_results_dusk$layer_Itot_1,
	multilayer_canopy_results_dusk$layer_Itot_2,
	multilayer_canopy_results_dusk$layer_Itot_3,
	multilayer_canopy_results_dusk$layer_Itot_4,
	multilayer_canopy_results_dusk$layer_Itot_5,
	multilayer_canopy_results_dusk$layer_Itot_6,
	multilayer_canopy_results_dusk$layer_Itot_7,
	multilayer_canopy_results_dusk$layer_Itot_8,
	multilayer_canopy_results_dusk$layer_Itot_9
)

# Create and print a plot
Icompare_dusk <- xyplot(Isun_profile_dusk + Idiff_profile_dusk + Itot_profile_dusk ~ cum_lai_profile_dusk, ylim=c(0,900), type='b', pch=16, auto=TRUE, xlab="cumulative lai (integrated from canopy bottom)", ylab="light flux", main="Low sun with overcast sky:\natmospheric_transmittance = 0.4, solar = 447.3,\nzenith = 75 degrees (k = 1.76), kd=0.1\n\nComparison of:\nbeam flux hitting sunlit leaves (Isun),\ndiffuse flux hitting all leaves (Idiff),\nand the effective total flux (Itot)")
x11(); print(Icompare_dusk)

if(FALSE) {
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
}