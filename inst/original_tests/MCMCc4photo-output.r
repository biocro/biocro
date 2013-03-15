## Analysis of MCMC output using the coda package

library(coda)

data(obsBea)

## Different starting values
resB1 <- MCMCc4photo(obsBea, 2e5, scale=1.5)
resB2 <- MCMCc4photo(obsBea, 2e5, ivmax=25, ialpha=0.1, scale=1.5)
resB3 <- MCMCc4photo(obsBea, 2e5, ivmax=45, ialpha=0.02, scale=1.5)

## Creating the mcmc objects
mc1 <- mcmc(resB1$resuMC[,1:2])
mc2 <- mcmc(resB2$resuMC[,1:2])
mc3 <- mcmc(resB3$resuMC[,1:2])

mcl <- mcmc.list(mc1,mc2,mc3)

acfplot(mcl)

## I apply a burn in and thinning

mc1 <- mcmc(resB1$resuMC[1e3:2e5,1:2], thin = 100)
mc2 <- mcmc(resB2$resuMC[1e3:2e5,1:2], thin = 100)
mc3 <- mcmc(resB3$resuMC[1e3:2e5,1:2], thin = 100)

mcl <- mcmc.list(mc1,mc2,mc3)

gelman.diag(mcl)

plot(mcl)

xyplot(mcl)

autocorr.diag(mcl)

summary(mcl)




