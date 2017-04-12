context("Basic tests of *Gro functions")
data(weather06, package = "BioCro")
data(weather05, package = "BioCro")

test_that("WillowGro runs in warm weather; refs bitbucket bug #7",{
    warmer <- transform(weather06, temp = temp + 20.0)
    res_warm <- willowGro(warmer)
    res_warm <- BioGro(warmer)
    data(warm) ## weather from Sapelo Island that caused problem
	warm$dayn = 365
    res_warm2 <- do.call(willowGro, warm)
})

test_that("WillowGro function produces reasonable results",{

    res <- BioGro(weather05)
    res <- willowGro(weather05)

  resmeans <- unlist(lapply(res, mean))
  for(output in c("LAI", "Leaf", "Root", "Stem")){
    expect_true(all(res[[output]] > 0))
  }
  expect_true(max(res$LAI) < 10)
  expect_true(max(res$Leaf) < 25)
})
# caneGro bug https://github.com/ebimodeling/biocro-dev/issues/45
# MaizeGro no roots:  https://github.com/ebimodeling/biocro-dev/issues/46
for (biocrofn in c("willowGro", "BioGro")){#, "caneGro" , "MaizeGro", "soyGro"
    print(biocrofn)
    res0 <- do.call(biocrofn, list(weather05))
    expect_is(res0, "BioGro")
   
    test_that("*Gro functions produce reasonable results",{
        
        for(output in c("LAI", "Leaf", "Root", "Stem")){
            print(paste(output, range(res0[[output]])))
            expect_true(min(res0[[output]]) >= 0)
            expect_true(max(res0[[output]]) < 50)
        }
    })
    
    
    res1 <- do.call(biocrofn, list(weather05, soilControl = soilParms(soilLayers = 6)))
    
    test_that("turning on soil layers increases aboveground productivity and reduces root allocation",{
        if(biocrofn != "soyGro"){# re-enable after soyGro has soil
          for(output in c("LAI", "Leaf", "Root", "Stem")){
            passed = expect_true(mean(res0[[output]]) < mean(res1[[output]]))
            print(paste(output, 'is greater when turning on soil layers:', passed))
          }          
        }
    })
    
    test_that(paste(biocrofn, "stem biomass is sensitive to key parameters "), {
      get.biomass <- function(...){
        ans <- do.call(biocrofn, list(weather05, ...))        
        return(max(ans$Stem, ans$Leaf, ans$Root))
      } 
      
      # willowGro insensitive to chi.l, b1 
      # pending implementation ebimodeling/biocro-dev#5
      
      if(biocrofn != "willowGro"){
        expect_gt(get.biomass(canopyControl = canopyParms(chi.l = 0.4, Sp = 5)),
                         get.biomass(canopyControl = canopyParms(chi.l = 9, Sp = 5)))
        expect_lt(get.biomass(photoControl = photoParms(b1 = 3)),
                         get.biomass(photoControl = photoParms(b1 = 10)))
      }
      if(biocrofn != "soyGro"){
        expect_gt(get.biomass(canopyControl = canopyParms(kd = 0.1)),
                         get.biomass(canopyControl = canopyParms(kd = 0.9)))
      }
      if(biocrofn == "soyGro"){
        expect_gt(get.biomass(photoControl = photoParms(vmax = 120)),
                         get.biomass(photoControl = photoParms(vmax = 80)))
      }
    })
	cat('\n')
}

test_that('willowGro() and Gro() produce similar results.', {
    # Check the difference between variables in the two data sets at each time point.
    # A relative difference is calculated for all time points,
    # and that difference must be less than a maximum allowed difference (md).

    md = 0.005  # The maximum allowed relative difference.

    # The data sets don't always use the same names, so make a list of the pairs of names that represent the same variable.
    name_pairs = list(
        c('TTc', 'ThermalT'), 
        c('lai', 'LAI'), 
        c('Stem', 'Stem'), 
        c('Leaf', 'Leaf'),
        c('Root', 'Root'),
        c('Rhizome', 'Rhizome'),
        c('canopy_assimilation', 'CanopyAssim'),
        c('canopy_transpiration', 'CanopyTrans')
    )

    d = function (g_var, w_var) {
        # Convenience function to get a relative difference between g$g_var and w$w_var.
        return(abs(sum(g[g_var] - w[w_var]) / sum(g[g_var])))
    }

    climate_like_willowGro = subset(weather06, doy >= 120 & doy <= 300)  # willowGro trims the climate data it is given, so you have to trim it before using it with Gro().

    w = willowGro(weather06)
    g = Gro(willow_initial_state, willow_parameters, climate_like_willowGro, modules = list(canopy_module_name='c3_canopy', soil_module_name='one_layer_soil_profile', growth_module_name='partitioning_growth', senescence_module_name='thermal_time_and_frost_senescence'))

    for (p in name_pairs) {
        expect_lt(d(p[1], p[2]), md, label=paste('willowGro vs Gro: relative difference of', p[1]), expected.label=md)
    }
})

