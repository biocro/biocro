library(BioCro)

modules = c('water_vapor_properties_from_air_temperature', 'collatz_leaf')

parms = c(sorghum_parameters, weather05[13, ])
parms[c('vmax', 'alpha', 'k_Q10', 'layer_wind_speed', 'leaf_net_irradiance', 'StomataWS')] =
       c(39, 0.04, 1, 3, 0, 1)

Gro_ode(parms, modules, list())
result = run_modules(parms, modules)
result[c('leaf_assimilation_rate', 'leaf_temperature', 'temp', 'leaf_stomatal_conductance')]
