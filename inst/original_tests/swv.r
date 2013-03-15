## Test script for the relationship between temperature and saturated
## water vapor pressure

## This justifies the use of Arden Buck instead of Goff-Gratch which
## improves computational time by 30%

Temps <- seq(270,310)

resGG <- fnpsvp(Temps)

resAB <- ardBuck(I(Temps-273.15))

## Visualy the difference is minimal
xyplot(resGG + resAB ~ Temps, type="l",lwd=2)

## some differnece probably due to rounding error
dist(rbind(resGG,resAB))
