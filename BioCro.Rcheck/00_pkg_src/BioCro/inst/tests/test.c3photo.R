test_that("c3photo is sensitive to changes in vcmax",{
  # redmine issue #1478
  a <- c3photo(Qp = 1500, RH = 0.7, Tl = 10, vcmax = 100)
  c <- c3photo(Qp = 1500, RH = 0.7, Tl = 10, vcmax = 10)
  expect_false(a$Assim == c$Assim)
})

