library(BioCro)
library(ggplot2)
library(reshape2)
library(rstudioapi)

year <- '2002' # other options: '2004', '2005', '2006'

# Set working directory to folder where thid file is located
filepath <- rstudioapi::getActiveDocumentContext()$path
filename <- sub(".*/","",filepath)
work.dir <- gsub(pattern = filename, replacement = "", x = filepath)
setwd(work.dir)

## Load weather data for growing season from specified year
weather <- read.csv(file = paste0('weather/', year, '_Bondville_IL_daylength.csv'))
dates <- data.frame("year" = 2001:2006,"sow" = c(143,152,147,149,148,148), "harvest" = c(291, 288, 289, 280, 270, 270))

sowdate <- dates$sow[which(dates$year == year)] #143 #148 #149 #152
harvestdate <- dates$harvest[which(dates$year == year)] #291 #270 #280 #288
sd.ind <- which(weather$doy == sowdate)[1] + 9
hd.ind <- which(weather$doy == harvestdate)[24]

weather.growingseason <- weather[sd.ind:hd.ind,]

## Experimentally collected biomassees
ExpBiomass<-read.csv(file=paste0('biomasses/',year,'_ambient_biomass.csv'))
ExpBiomass.std<-read.csv(file=paste0('biomasses/',year,'_ambient_biomass_std.csv'))
colnames(ExpBiomass)<-c("DOY","Leaf","Stem","Grain","Seed","Litter","CumLitter")
colnames(ExpBiomass.std)<-c("DOY","Leaf","Stem","Grain","Seed","Litter","CumLitter")


# steady_state_modules
steady_state_modules <- c('soil_type_selector', 'stomata_water_stress_linear', 
                          'leaf_water_stress_exponential', 'parameter_calculator', 
                          'c3_canopy', 'soybean_development_rate_calculator', 
                          'partitioning_coefficient_logistic', 
                          'no_leaf_resp_partitioning_growth_calculator',
                          'senescence_coefficient_logistic')

# derivative_modules
derivative_modules <- c('thermal_time_senescence_logistic', 'partitioning_growth', 
                        'two_layer_soil_profile', 'development_index','thermal_time_accumulator')

# soybean_initial_state
source('soybean_initial_state.R')

# soybean_parameters
source('soybean_parameters.R')


solver_params <- list(
  type = 'Gro_rkck54',
  output_step_size = 1.0,
  adaptive_error_tol = 1e-6,
  adaptive_max_steps = 250)


result <- Gro_solver(soybean_initial_state, soybean_parameters, weather.growingseason, steady_state_modules, derivative_modules,solver = solver_params)


## Plot simulated and experimental biomasses
col.palette.muted <- c("#332288", "#117733", "#999933", "#882255")
size.title <- 24
size.axislabel <-18
size.axis <- 18
size.legend <- 18

r <- melt(result[,c("doy_dbl","Root","Leaf","Stem","Grain")],id.vars="doy_dbl")

r.exp <- melt(ExpBiomass[,c("DOY","Leaf","Stem","Grain")],id.vars = "DOY")
r.exp.std <- melt(ExpBiomass.std[,c("DOY","Leaf","Stem","Grain")],id.vars = "DOY")
r.exp.std$ymin<-r.exp$value-r.exp.std$value
r.exp.std$ymax<-r.exp$value+r.exp.std$value

f <- ggplot() + theme_classic()
f <- f + geom_point(data=r, aes(x=doy_dbl,y=value, colour=variable))
f <- f + geom_errorbar(data=r.exp.std, aes(x=DOY, ymin=ymin, ymax=ymax), width=4)
f <- f + geom_point(data=r.exp, aes(x=DOY, y=value, fill=variable), shape=22, size=3, show.legend = FALSE, stroke=1)
f <- f + labs(title=bquote(.(year)~Ambient~CO[2]), x=paste0('Day of Year (',year,')'),y='Biomass (Mg / ha)')
f <- f + scale_x_continuous(breaks = seq(150,275,30))
f <- f + theme(plot.title=element_text(size=size.title, hjust=0.5),axis.text=element_text(size=size.axis), axis.title=element_text(size=size.axislabel),legend.position = c(.15,.85), legend.title = element_blank(), legend.text=element_text(size=size.legend))
f <- f + guides(colour = guide_legend(override.aes = list(size=3)))
f <- f + scale_fill_manual(values = col.palette.muted[2:4])
f <- f + scale_colour_manual(values = col.palette.muted, labels=c('Root','Leaf','Stem','Pod'))
print(f)


