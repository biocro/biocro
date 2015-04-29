context("Basic tests of *Gro functions")
data(weather06, package = "BioCro")
data(weather05, package = "BioCro")

test_that("WillowGro runs in warm weather; refs bitbucket bug #7",{
    warmer <- transform(weather06, Temp = Temp + 20.0)
    res_warm <- willowGro(warmer)
    res_warm <- BioGro(warmer)
    data(warm) ## weather from Sapelo Island that caused problem
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
    
# soyGro: does not compile on linux https://github.com/ebimodeling/biocro-dev/issues/44
# caneGro bug https://github.com/ebimodeling/biocro-dev/issues/46
# MaizeGro no roots:  https://github.com/ebimodeling/biocro-dev/issues/46
for (biocrofn in c("willowGro", "BioGro")){#, "caneGro" , "MaizeGro"
    print(biocrofn)
    res0 <- do.call(biocrofn, list(weather05))
    
   
    test_that("*Gro functions produce reasonable results",{
        
        for(output in c("LAI", "Leaf", "Root", "Stem")){
            print(paste(output, range(res0[[output]])))
            expect_true(min(res0[[output]]) > 0)
            expect_true(max(res0[[output]]) < 50)
        }
    })
    
    
    res1 <- do.call(biocrofn, list(weather05, soilControl = soilParms(soilLayers = 6)))
    
    test_that("turning on soil layers increases aboveground productivity and reduces root allocation",{
        
        for(output in c("LAI", "Leaf", "Root", "Stem")){
            print(output)
            expect_true(mean(res0[[output]]) < mean(res1[[output]]))
        }
    })
    
    test_that("BioCro stem biomass is sensitive to key parameters ", {
      get.biomass <- function(...){
        ans <- do.call(biocrofn, list(weather05, ...))
        return(max(ans$Stem))
      } 
      
      
      if(biocrofn != 'willowGro'){# skip willowGro pending implementation ebimodeling/biocro-dev#5
        expect_more_than(get.biomass(canopyControl = canopyParms(chi.l = 0.37)),
                         get.biomass(canopyControl = canopyParms(chi.l = 9)))
        expect_less_than(get.biomass(photoControl = photoParms(b1=3)),
                         get.biomass(photoControl = photoParms(b1=10)))
      }
      
      expect_more_than(get.biomass(canopyControl = willowcanopyParms(kd = 0.37)),
                       get.biomass(canopyControl = willowcanopyParms(kd = 0.9)))
      

    })
}
