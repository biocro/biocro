library(BioCro)

modules = c('water_vapor_properties_from_air_temperature', 'collatz_leaf')

parms = c(sorghum_parameters, weather05[13, ])
parms[c('vmax', 'alpha', 'k_Q10', 'wind_speed', 'StomataWS')] =
       c(39, 0.04, 1, 3, 1)

parms$incident_par = parms$solar
fraction_of_irradiance_in_PAR = 0.5;  # dimensionless.
joules_per_micromole_PAR = 0.235;   # J / micromole. For the wavelengths that make up PAR in sunlight, one mole of photons has, on average, approximately 2.35 x 10^5 joules:
parms$incident_irradiance = parms$incident_par * joules_per_micromole_PAR / fraction_of_irradiance_in_PAR

Gro_ode(parms, modules, list())
result = run_modules(parms, modules)
result[c('leaf_assimilation_rate', 'leaf_temperature', 'temp', 'leaf_stomatal_conductance')]
