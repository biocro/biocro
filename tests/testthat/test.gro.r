library(BioCro)
data(sorghum_initial_state)
data(sorghum_parameters)
data(weather05)
sorghum_modules = list(canopy_module_name='c4_canopy', soil_module_name='one_layer_soil_profile', growth_module_name='partitioning_growth', senescence_module_name='thermal_time_senescence')

test = Gro(sorghum_initial_state, sorghum_parameters, weather05, sorghum_modules)

missing_parms = within(sorghum_parameters, alpha1<-NULL)

test = Gro(sorghum_initial_state, missing_parms, weather05, sorghum_modules)

wrong_modules = within(sorghum_modules, canopy_module_name<-'NONEXISTANTMODULE')
test = Gro(sorghum_initial_state, sorghum_parameters, weather05, wrong_modules)

