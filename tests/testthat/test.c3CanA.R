context("Testing C3CanA")

library(BioCro)
library(testthat)


test_that("Testing c3CanA is sensitive to b0 Cutilular Conductance ",{

  
  a <- c3CanA(lai = 4, doy = 180, hr = 12, solar = 1000, temp = 30, rh = 0.1, windspeed =6,
              lat=40,nlayers=8,kd=0.1,
              heightFactor=3,
              c3photoControl = list(b0=0.1))
  b <- c3CanA(lai = 4, doy = 180, hr = 12, solar = 1000, temp = 30, rh = 0.1, windspeed =6,
              lat=40,nlayers=8,kd=0.1,
              heightFactor=3,
              c3photoControl = list(b0=1))
  a$CanopyAssim
  b$CanopyAssim
  
  a$CanopyTrans
  b$CanopyTrans
  
})


test_that("c3CanA is sensitive to b1",{
  
  c <- c3CanA(lai = 4, doy = 180, hr = 12, solar = 1000, temp = 30, rh = 0.1, windspeed =6,
              lat=40,nlayers=8,kd=0.1,
              heightFactor=3,
              c3photoControl = list(b1=4))
  d <- c3CanA(lai = 4, doy = 180, hr = 12, solar = 1000, temp = 30, rh = 0.1, windspeed =6,
              lat=40,nlayers=8,kd=0.1,
              heightFactor=3,
              c3photoControl = list(b1=10))
  expect_less_than(c$CanopyAssim, d$CanopyAssim)
  expect_less_than(c$CanopyTrans, d$CanopyTrans)
  
}) 


test_that("c3CanA is sensitive to vmax",{
  
  e <- c3CanA(lai = 4, doy = 180, hr = 12, solar = 1000, temp = 20, rh = 0.1, windspeed =6,
              lat=40,nlayers=8,kd=0.8,
              heightFactor=3,
              c3photoControl = willowphotoParms(vmax=50))
  f <- c3CanA(lai = 4, doy = 180, hr = 12, solar = 1000, temp = 20, rh = 0.1, windspeed =6,
              lat=40,nlayers=8,kd=0.8,
              heightFactor=3,
              c3photoControl = willowphotoParms(vmax=200))
  expect_less_than(e$CanopyAssim, f$CanopyAssim)
  
})


test_that("c3CanA is sensitive to jmax", {
  
  g <- c3CanA(lai = 4, doy = 180, hr = 12, solar = 1000, temp = 20, rh = 0.1, windspeed =6,
              lat=40,nlayers=8,kd=0.8,
              heightFactor=3,
              c3photoControl = willowphotoParms(jmax=50))
  h <- c3CanA(lai = 4, doy = 180, hr = 12, solar = 1000, temp = 20, rh = 0.1, windspeed =6,
              lat=40,nlayers=8,kd=0.8,
              heightFactor=3,
              c3photoControl = willowphotoParms(jmax=200))
  expect_less_than(g$CanopyAssim, h$CanopyAssim)
})

test_that("c3CanA is sensitive to kd", {
  i <- c3CanA(lai = 4, doy = 180, hr = 12, solar = 1000, temp = 20, rh = 0.1, windspeed =6,
              lat=40,nlayers=8,kd=0.2)
  j <- c3CanA(lai = 4, doy = 180, hr = 12, solar = 1000, temp = 20, rh = 0.1, windspeed =6,
              lat=40,nlayers=8,kd=0.8)
  expect_more_than(i$CanopyAssim, j$CanopyAssim)
  
})
