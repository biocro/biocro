context("Basic tests of *Gro functions")
data(weather06, package = "BioCro")
data(weather05, package = "BioCro")

test_that("WillowGro runs in warm weather; refs bitbucket bug #7",{
    warmer <- transform(weather06, Temp = Temp + 20.0)
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
for (biocrofn in c("soyGro", "willowGro", "BioGro")){#, "caneGro" , "MaizeGro"
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
            passed = expect_true(mean(res0[[output]]) < mean(res1[[output]]))$passed
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
        expect_more_than(get.biomass(canopyControl = canopyParms(chi.l = 0.4, Sp = 5)),
                         get.biomass(canopyControl = canopyParms(chi.l = 9, Sp = 5)))
        expect_less_than(get.biomass(photoControl = photoParms(b1 = 3)),
                         get.biomass(photoControl = photoParms(b1 = 10)))
      }
      if(biocrofn != "soyGro"){
        expect_more_than(get.biomass(canopyControl = canopyParms(kd = 0.1)),
                         get.biomass(canopyControl = canopyParms(kd = 0.9)))
      }
      if(biocrofn == "soyGro"){
        expect_more_than(get.biomass(photoControl = photoParms(vmax = 120)),
                         get.biomass(photoControl = photoParms(vmax = 80)))
      }
    })
	cat('\n')
}

