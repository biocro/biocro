context("Basic tests of WillowGro function")

test_that("WillowGro function runs with smoothed weather05 data",{
  data(weather05)
  res <- BioGro(weather05)
})

test_that("WillowGro function produces reasonable results",{
  ## 
  resmeans <- unlist(lapply(res, mean))
  for(output in c("LAI", "Leaf", "Root", "Stem")){
    expect_true(all(res[[output]]) > 0)
  }
  expect_true(max(res$LAI) < 10))
  expect_true(max(res$Leaf) < 25)  
})