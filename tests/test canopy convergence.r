library(BioCro)

modules = c('water_vapor_properties_from_air_temperature', 'collatz_leaf')

parms = c(sorghum_parameters, weather05[13, ])
parms[c('vmax', 'alpha', 'k_Q10', 'layer_wind_speed', 'incident_irradiance', 'StomataWS', 'temp', 'solar',  'rh', 'doy', 'hour', 'windspeed', 'precip')] =
       c(39, 0.04, 2, 2.7073, 189.8702, 0.0002, 18.5395683, 521.8996203, 0.9141, 129, 9, 2.7281, 0.0)
#parms[c('vmax', 'alpha', 'k_Q10', 'layer_wind_speed', 'incident_irradiance', 'StomataWS', 'temp', 'solar',  'rh', 'doy', 'hour', 'windspeed', 'precip')] =
#       c(39, 0.04, 2, 0.9716, 0, 0.0002, 18.1667, 0, 0.8344, 153, 22, 0.9839, 0.0318)

Gro_ode(parms, modules, list())
result = run_modules(parms, modules)
result[c('leaf_assimilation_rate', 'leaf_temperature', 'temp', 'leaf_stomatal_conductance')]
