context("Basic tests of *Gro functions")

test_that("WillowGro function runs with smoothed weather05 data",{
})

test_that("WillowGro runs in warm weather; refs bitbucket bug #7",{
    data(weather06)
    warmer <- transform(weather06, Temp = Temp + 20.0)
    res_warm <- willowGro(warmer)
    res_warm <- BioGro(warmer)
    data(warm) ## weather from Sapelo Island that caused problem
    res_warm2 <- do.call(willowGro, warm)
})

test_that("WillowGro function produces reasonable results",{
    data(weather05)
    res <- BioGro(weather05)
    res <- willowGro(weather05)

  resmeans <- unlist(lapply(res, mean))
  for(output in c("LAI", "Leaf", "Root", "Stem")){
    expect_true(all(res[[output]] > 0))
  }
  expect_true(max(res$LAI) < 10)
  expect_true(max(res$Leaf) < 25)
})
    
for(biocrofn in c("willowGro", "BioGro", "caneGro")){
    ## print(biocrofn)
    res0 <- do.call(biocrofn, list(weather05))
    
    test_that("*Gro functions produce reasonable results",{
        
        for(output in c("LAI", "Leaf", "Root", "Stem")){
            ## print(paste(output, range(res0[[output]])))
            expect_true(min(res0[[output]]) > 0)
            expect_true(max(res0[[output]]) < 50)
        }
    })
    

    res1 <- do.call(biocrofn, list(weather05, soilControl = soilParms(soilLayers = 6,  hydrDist=TRUE)))

    test_that("turning on soil layers and hydrDist increases aboveground productivity and reduces root allocation",{
        
        for(output in c("LAI", "Leaf", "Root", "Stem")){
            expect_true(mean(res0[[output]]) < mean(res1[[output]]))
        }
    })

    if(!biocrofn == "caneGro"){
        test_that("increasing phi reduces productivity",{
            ll.0 <- soilParms(FieldC=0.37,WiltP=0.2,phi2=1)
            ## increase phi2 that controls water limitation
            ll.1 <- soilParms(FieldC=0.37,WiltP=0.2,phi2=4)
            
            ans.0 <- do.call(biocrofn, list(weather05,soilControl=ll.0))
            ans.1 <- do.call(biocrofn, list(weather05,soilControl=ll.1))
            
            b0 <- sum(ans.0[["Leaf"]], ans.0[["Root"]], ans.0[["Stem"]])
            b1 <- sum(ans.1[["Leaf"]], ans.1[["Root"]], ans.1[["Stem"]])
            print(biocrofn)
            print(b0)
            print(b1)
            expect_true(b0 < b1)
        })
    }
}

## test_that("lowering field capacity reduces yield",{
##   ll.0 <- soilParms(FieldC=0.4, WiltP=0.2, phi2=1)
##   ## increase phi2 that controls water limitation
##   ll.1 <- soilParms(FieldC=0.3, WiltP=0.2, phi2=1)
  
##   ans.0 <- do.call(biocrofn, list(weather05,soilControl=ll.0))
##   ans.1 <- do.call(biocrofn, list(weather05,soilControl=ll.1))
  
##   for(output in c("LAI", "Leaf", "Root", "Stem")){
##     expect_true(all(ans.0[[output]] >= ans.1[[output]]))
##   }
## })
