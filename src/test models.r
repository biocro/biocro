library(lattice)
library(BioCro)

system.time({
ngr = Gro(glycine_max_initial_state, glycine_max_parameters, get_growing_season_climate(weather05), within(glycine_max_modules, {growth_module_name<-'utilization_growth_and_senescence'; senescence_module_name<-'empty_senescence'}))
})

#save(gr, sr, wr, mr, file='old_results.rdata')
load('old_results.rdata')
identical(ngr, gr)

sr = Gro(sorghum_initial_state, sorghum_parameters, get_growing_season_climate(weather05), sorghum_modules)
wr = Gro(willow_initial_state, willow_parameters, get_growing_season_climate(weather05), willow_modules)
mr = Gro(miscanthus_x_giganteus_initial_state, miscanthus_x_giganteus_parameters, get_growing_season_climate(weather05), miscanthus_x_giganteus_modules)

#save(gr, sr, wr, mr, file='new_results.rdata')

x11() ;xyplot(Leaf + Stem + Root + Rhizome + Grain ~ TTc, gr, auto=TRUE, type='l')
x11() ;xyplot(Leaf + Stem + Root + Rhizome + Grain ~ TTc, wr, auto=TRUE, type='l')
x11() ;xyplot(Leaf + Stem + Root + Rhizome + Grain ~ TTc, ngr, auto=TRUE, type='l', ylim=c(-0.1, 8))

x11() ;xyplot(Leaf + Stem + Root + Rhizome + Grain ~ TTc, sr, auto=TRUE, type='l')
x11() ;xyplot(Leaf + Stem + Root + Rhizome + Grain ~ TTc, nsr, auto=TRUE, type='l')

x11() ;xyplot(Leaf + Stem + Root + Rhizome + Grain ~ seq_len(nrow(sr)), sr, auto=TRUE, type='l')
x11() ;xyplot(Leaf + Stem + Root + Rhizome + Grain ~ seq_len(nrow(nsr)), nsr, auto=TRUE, type='l')

x11() ;xyplot(soil_water_content + I(precip / 1000 + .3)~ time, cbind(get_growing_season_climate(weather05), gr), auto=TRUE, type='l')
x11() ;xyplot(soil_water_content ~ TTc, ngr, auto=TRUE, type='l')

with(subset(weather05, time > 2900 & time < 4000), cbind(cumsum(precip), time))

    x11() ;xyplot(LeafWS ~ TTc, gr, auto=TRUE, type='l')
    x11() ;xyplot(LeafWS ~ TTc, ngr, auto=TRUE, type='l')
all.equal(gr$LeafWS, ngr$LeafWS)

    x11() ;xyplot(LeafWS ~ TTc, sr, auto=TRUE, type='l')
    x11() ;xyplot(LeafWS ~ TTc, nsr, auto=TRUE, type='l')
all.equal(sr$LeafWS, nsr$LeafWS)

    x11() ;xyplot(LeafWS ~ TTc, wr, auto=TRUE, type='l')
    x11() ;xyplot(LeafWS ~ TTc, nwr, auto=TRUE, type='l')
all.equal(wr$LeafWS, nwr$LeafWS)

    x11() ;xyplot(LeafWS ~ TTc, mr, auto=TRUE, type='l')
    x11() ;xyplot(LeafWS ~ TTc, nmr, auto=TRUE, type='l')
all.equal(mr$LeafWS, nmr$LeafWS)

xyplot(LeafWS ~ seq_len(nrow(nsr)), nsr)
