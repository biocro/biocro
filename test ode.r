library(BioCro)

myparms = sorghum_parameters
myparms['Sp_thermal_time_decay'] = 0
myparms['LeafN'] = 0

#r = Gro_ode(sorghum_initial_state, myparms, weather05, steady_state_modules='c4_canopy', derivative_modules=c('one_layer_soil_profile', 'partitioning_growth', 'thermal_time_senescence'))
r = Gro_ode(sorghum_initial_state, myparms, weather05, steady_state_modules='c4_canopy', derivative_modules=c())



easy_parms = sorghum_parameters
easy_parms['Sp_thermal_time_decay'] = 0
easy_parms['LeafN'] = 0
easy_parms['LeafArea'] = 2
easy_parms['parameter'] = 5
easy_parms['PAR'] = 7
r = Gro_ode(sorghum_initial_state, easy_parms, weather05, steady_state_modules='test_calc_state', derivative_modules=c('test_derivs'))

soil_parms = sorghum_parameters
r = Gro_ode(sorghum_initial_state, soil_parms, weather05, steady_state_modules=c('soil_type_selector'), derivative_modules=('one_layer_soil_profile_derivatives'))

