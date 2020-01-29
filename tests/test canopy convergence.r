library(BioCro)

modules = c('water_vapor_properties_from_air_temperature', 'collatz_leaf')

parms = c(sorghum_parameters, weather05[13, ])
parms[c('vmax', 'alpha', 'k_Q10', 'layer_wind_speed', 'incident_irradiance', 'StomataWS', 'temp', 'solar',  'rh', 'doy', 'hour', 'windspeed', 'precip')] =
       c(39, 0.04, 2, 2.7073, 189.8702, 0.0002, 18.5395683, 521.8996203, 0.9141, 129, 9, 2.7281, 0.0)
#parms[c('vmax', 'alpha', 'k_Q10', 'layer_wind_speed', 'incident_irradiance', 'StomataWS', 'temp', 'solar',  'rh', 'doy', 'hour', 'windspeed', 'precip')] =
#       c(39, 0.04, 2, 0.9716, 0, 0.0002, 18.1667, 0, 0.8344, 153, 22, 0.9839, 0.0318)
parms$incident_par = parms$incident_irradiance * 0.5 / 0.253

#Gro_ode(parms, modules, list())	# this doesn't seem to do anything since we aren't storing its output... plus, neither of these modules return derivatives, so why use Gro_ode? (EBL)
result = test_standalone_ss(modules, parms)
result$temp = parms$temp
result[c('leaf_assimilation_rate', 'leaf_temperature', 'temp', 'leaf_stomatal_conductance')]