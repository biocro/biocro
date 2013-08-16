context("Basic tests of WillowGro function")

test_that("WillowGro function runs with smoothed weather05 data",{
  data(weather05)
  res <- BioGro(weather05)
})

test_that("WillowGro runs in warm weather",{
  data(warm)
  res <- willowGro(WetDat = warm$WetDat, canopyControl = warm$canopyControl,
                                         photoControl = warm$photoControl, 
                                         day1= warm$day1, 
                                         dayn = warm$dayn)
  })

test_that("WillowGro function produces reasonable results",{
  ## 
  resmeans <- unlist(lapply(res, mean))
  for(output in c("LAI", "Leaf", "Root", "Stem")){
    expect_true(all(res[[output]]) > 0)
  }
  expect_true(max(res$LAI) < 10)
  expect_true(max(res$Leaf) < 25)
})

test_that("WillowGro function produces expected results",{
  data(weather05)
  for(biocrofn in c("willowGro", "BioGro", "caneGro")){
    
    res0 <- do.call(biocrofn, list(weather05))
    res1 <- do.call(biocrofn, list(weather05, soilControl = soilParms(soilLayers = 6)))
    res2 <- do.call(biocrofn, list(weather05, soilControl = soilParms(soilLayers = 6, hydrDist=TRUE)))
    
    for(output in c("LAI", "Leaf", "Root", "Stem")){
      expect_true(all(res0[[output]] - res1[[output]]  <= 1)) 
      expect_true(all(res0[[output]] - res2[[output]]  <= 1))
    }

    ll.0 <- soilParms(FieldC=0.37,WiltP=0.2,phi2=1)
    ## increase phi2 that controls water limitation
    ll.1 <- soilParms(FieldC=0.37,WiltP=0.2,phi2=4)
    
    ans.0 <- do.call(biocrofn, list(weather05,soilControl=ll.0))
    ans.1 <- do.call(biocrofn, list(weather05,soilControl=ll.1))
    
    for(output in c("LAI", "Leaf", "Root", "Stem")){
      expect_true(all(ans.0[[output]] - ans.1[[output]] >= -1))
    }
  }
})

test_that("lowering field capacity reduces yield",{
  ll.0 <- soilParms(FieldC=0.4, WiltP=0.2, phi2=1)
  ## increase phi2 that controls water limitation
  ll.1 <- soilParms(FieldC=0.3, WiltP=0.2, phi2=1)
  
  ans.0 <- do.call(biocrofn, list(weather05,soilControl=ll.0))
  ans.1 <- do.call(biocrofn, list(weather05,soilControl=ll.1))
  
  for(output in c("LAI", "Leaf", "Root", "Stem")){
    expect_true(all(ans.0[[output]] >= ans.1[[output]]))
  }
})