# This script runs an optimization on the StomataWS's linear coeffcients
# to fit the observed Gs data by Gray, S., Dermody, O., Klein, S. et al.
# Intensifying drought eliminates the expected benefits of elevated carbon dioxide for soybean.
# Nature Plants 2, 16132 (2016). https://doi.org/10.1038/nplants.2016.132
#
# this has two steps: 1. we fit to each Gs point by adjusting the StomataWS value
# to achieve a "perfect" fit with module c3_leaf_photosynthesis;
# 2. we create a linear function between the optimized StomataWS and REW.
#
# When the script is done running, the new values of StomataWS_gradient and
# StomataWS_intercept will be printed to the R terminal. If necessary, the
# stored values in data/soybean2.R should be updated to the new ones, and a note
# about the reparameterization should be included in man/soybean2.R
#
# Last used with BioCro version with commit: db12f4a0

library(BioCro)
library(dplyr)
library(tidyr)
rm(list=ls())

## STEP 1: calibrate StomataWS to fit Gs data
# Define a function that runs the clock modules to determine the photoperiod
# length during a year's worth of weather data, adding it to the data so it can
# be used as a driver in future simulations
add_photoperiod_length <- function(weather_data) {
  clock_output <- with(soybean_clock, {run_biocro(
    initial_values,
    parameters,
    weather_data,
    direct_modules,
    differential_modules,
    ode_solver
  )})
  weather_data[['day_length']] <- clock_output[['day_length']]
  return(weather_data)
}

# Define the RMSE function
rmse <- function(observed, predicted) {
  # Check if the lengths of the observed and predicted vectors are the same
  if (length(observed) != length(predicted)) {
    stop("The lengths of observed and predicted values must be the same.")
  }

  # Calculate the squared differences
  squared_diff <- (observed - predicted)^2

  # Calculate the mean of the squared differences
  mean_squared_diff <- mean(squared_diff)

  # Calculate the square root of the mean squared differences (RMSE)
  rmse_value <- sqrt(mean_squared_diff)

  return(rmse_value)
}

CO2_cond = "AC" #ambient CO2 is used for optimization
#get observed Gs at different DOYs and Years
#YH: I collected and arranged the data from the paper
years = 2009:2011
obs_gs_mean   = read.csv("data/obs_gs_Vcmax_Jmax_2009_2011.csv")
obs_gs_mean   = obs_gs_mean[obs_gs_mean$CO2==CO2_cond,]

#set the search range to [0.2,1.5] for each StomataWS
x0   = rep(1,nrow(obs_gs_mean))
x_ub = x0*1.5
x_lb = x0*0.2
years = sort(unique(obs_gs_mean$year))
#observed Vcmax25 from 2009-2011: 135-aCO2, 119-eCO2
#observed Jmax25  from 2009-2011: 207-aCO2, 203-eCO2
#therefore, to make sure the means match our default (110 & 195),
#do a constant scaling of the dynamic Vcmax25 and Jmax25
Vcmax_sf = 1.22
Jmax_sf  = 1.06

#EL:I just checked some of my Licor log files. They have a gbw column that's always between 2.90 - 2.93 mol m-2 s-1.
#the module expects conductances in m/s.
#YH: assuming 30 C, the molar volume of air Vm=0.02487. So 2.9*0.02446 = 0.072 m/s
gbw_canopy = 0.072

#To drive c3_leaf_photosynthesis, we need the absorbed PPFD
#To match experimental condition, we also want "flat leaf" absorption
#therefore, run biocro for the 2009-2011 and extract midday conditions on the observed DOYs
obs_weather_Gray<-read.csv('data/soyFACE_weather_data_2004thru2011.csv') #Gray's weather data for determining growing season
results = list()

for (i in 1:length(years)){
    year = years[i]
    weatherData <- weather[[as.character(year)]]
    weatherData <- add_photoperiod_length(weatherData)
    #further subset from the start of obs DOY
    obs_weather_Gray_yeari = obs_weather_Gray[obs_weather_Gray$Year==year,]
    DOY_start = obs_weather_Gray_yeari$DOY[1]
    DOY_end = tail(obs_weather_Gray_yeari$DOY,1)
    weather_growing_season = weatherData[weatherData$doy>=DOY_start & weatherData$doy<=DOY_end,]

    #Use Gray's rainfall data for this calibration
    obs_weather_Gray_yeari_hourly <- obs_weather_Gray_yeari %>%
      # Add an hourly sequence per day
      uncount(weights = 24, .id = "hour") %>%
      # Adjust the hour (0 to 23)
      mutate(hour = hour - 1,
             precip = precip.mm. / 24)
    weather_growing_season$precip = obs_weather_Gray_yeari_hourly$precip

    parameters = soybean2$parameters
    parameters$Catm = catm_data$Catm[catm_data$year==year]
    parameters$chil = 1e8  #infinite chil means flat leaf

    #use the original linear function as the baseline
    direct_modules = soybean2$direct_modules
    direct_modules$stomata_water_stress = "BioCro:stomata_water_stress_linear_v2"
    parameters$StomataWS_gradient = 1
    parameters$StomataWS_intercept = 0

    results[[i]] <- run_biocro(
      soybean2$initial_values,
      parameters,
      weather_growing_season,
      direct_modules,
      soybean2$differential_modules
    )
}

#define obj function
obj_func<-function(x,return_df = FALSE){
  #get modelled and env conditions at the observed time
  obs_and_model = c()
  for (year in years){
    result_i = results[[which(years==year)]]
    obs_gs_mean_i = obs_gs_mean[obs_gs_mean$year==year,]
    result_daily_midday = result_i[result_i$hour == 12, ]
    result_match_obs    = result_daily_midday[result_daily_midday$doy %in% obs_gs_mean_i$DOY,]
    x_sub = x[which(obs_gs_mean$year==year)]
    obs_and_model = rbind(obs_and_model,data.frame(obs_gs_mean_i,
                                                   temp      = result_match_obs$temp,
                                                   StomataWS = x_sub,
                                                   swc       = result_match_obs$soil_water_content,
                                                   absorbed_longwave = result_match_obs$absorbed_longwave,
                                                   absorbed_ppfd      = result_match_obs$sunlit_absorbed_ppfd_layer_0,
                                                   absorbed_shortwave = result_match_obs$sunlit_absorbed_shortwave_layer_0,
                                                   height = result_match_obs$height_layer_0,
                                                   rh = result_match_obs$rh,
                                                   windspeed = 100,
                                                   temp_air  = result_match_obs$temp,
                                                   rh_air    = result_match_obs$rh,
                                                   fieldc    = result_match_obs$soil_field_capacity,
                                                   wiltp     = result_match_obs$soil_wilting_point
                                                   )
                          )
  }

  obs_and_model$ID = 1:nrow(obs_and_model)

  my_para = soybean2$parameters
  my_para$atmospheric_pressure = soybean$parameters$atmospheric_pressure
  my_para$gbw_canopy = gbw_canopy
  #for simplicity for c3_leaf_photosynthesis, just use average Catm from 2009-2011
  Current_Catm = mean(catm_data$Catm[catm_data$year<=2011 & catm_data$year>=2009])
  my_para$Catm = Current_Catm

  #these are updated parameters for soybean from Ed
  my_para$Gstar_at_25     = 37.99046
  my_para$Gstar_Ea        = 26.0038851874699e3
  my_para$Kc_at_25        = 359.408
  my_para$Kc_Ea           = 90.4762601430278e3
  my_para$Ko_at_25        = 446.7409
  my_para$Ko_Ea           = 16.5650822025287e3

  #use observed Vcmax25 and Jmax25
  my_para$Vcmax_at_25 = NULL
  my_para$Jmax_at_25 = NULL
  col2use = c("temp","StomataWS","absorbed_longwave","absorbed_ppfd","absorbed_shortwave","height","rh","windspeed","Vcmax25","Jmax25")
  my_df =  obs_and_model[,col2use]
  colnames(my_df)[colnames(my_df)=="Vcmax25"]="Vcmax_at_25"
  colnames(my_df)[colnames(my_df)=="Jmax25"]="Jmax_at_25"
  #scale their means
  my_df$Vcmax_at_25 = my_df$Vcmax_at_25 / Vcmax_sf
  my_df$Jmax_at_25  = my_df$Jmax_at_25  / Jmax_sf
  rc <- module_response_curve("BioCro:c3_leaf_photosynthesis",
                                my_para,
                                my_df)
  #time series
  df2out = cbind(obs_and_model,Vcmax_at_25 = rc$Vcmax_at_25, Jmax_at_25 = rc$Jmax_at_25, model=rc$Gs)
  names(df2out)[names(df2out) == "Cond"] <- "obs"
  # Logic to switch return type
  if (return_df) {
    return(df2out) # Returns the data frame for plotting
  } else {
    return(rmse(df2out$obs, df2out$model)) # Returns the number for optim
  }
}

opt_result<-optim(x0,obj_func,NULL, method = "L-BFGS-B",
                  lower = x_lb, upper = x_ub,control = list(trace = 3)
)
best_x = opt_result$par

# A quick plot to compare gs.
df_final <- obj_func(best_x, return_df = TRUE)

dev.new()
plot(df_final$obs, df_final$model,
     xlab = "Observed gs", ylab = "Modelled gs",
     main = "Optimization Results")
abline(0, 1, col = "red")

##END of STEP1

##STEP2: make a linear function between opt StomataWS and REW
df_final$bestx = best_x
#this is REW using modelled SWC
df_final$REW_model = (df_final$swc - df_final$wiltp)/(df_final$fieldc - df_final$wiltp)

#Here we estimate the observed SWC (root-depth-based average) to get "observed" REW
obs_data = readRDS("data/soil_moisture_data_rearranged.rds")
obs_data$sm = obs_data$sm/100
model_soil_depths = c(5,10,20,20,20,25)
df_final$swc_obs = NA
for (j in 1:nrow(obs_gs_mean)){
  year = obs_gs_mean$year[j]
  doy  = obs_gs_mean$DOY[j]
  result_j = results[[which(years==year)]]
  rooting_layer = result_j$max_rooting_layer[result_j$fractional_doy ==doy]
  depth_j = floor(sum(model_soil_depths[1:rooting_layer])/10)
  #top x-cm average for rings and depths
  obs_data_sub = obs_data[obs_data$depth<=depth_j,]
  obs_avg = aggregate(sm ~ doy+year, data = obs_data_sub, FUN = mean)
  subset_idx <- which(obs_avg$year == year)
  nearest_idx <- subset_idx[
    which.min(abs(obs_avg$doy[subset_idx] - doy))
  ]
  obs_avg_j <- obs_avg$sm[nearest_idx]
  df_final$swc_obs[j] = obs_avg_j
}

#this is REW using observed SWC
df_final$REW_obs = (df_final$swc_obs - df_final$wiltp)/(df_final$fieldc - df_final$wiltp)

#we use observed REW for the fitting
fit_linear <- lm(bestx ~ REW_obs, data = df_final)
print(summary(fit_linear))

# A quick plot to check bestx vs REW
# To get the predicted values for plotting
linear_pred <- predict(fit_linear)

dev.new()
plot(df_final$REW_obs, df_final$bestx,
     xlab = "REW", ylab = "StomataWS*")
lines(df_final$REW_obs,linear_pred,col='red')

##END of STEP2

# Print out new values to use
cat('New value of StomataWS_gradient:',  fit_linear$coefficients[2], '\n')
cat('New value of StomataWS_intercept:', fit_linear$coefficients[1], '\n')
