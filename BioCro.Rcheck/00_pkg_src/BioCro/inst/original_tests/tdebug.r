## Temporary function to debug soil model

tf <- function(x,layers){

  ltmp <- 3
  ans <- numeric(5e4)
  count <- 1
  tmp2 <- 0
  tmp <- capture.output(x)
  tmp1 <- lapply(tmp,function(x) strsplit(x," "))
  ## Start loop
  for(i in 1:5e4){
    ltmp <- length(unlist(tryCatch(tmp1[[i]], error=function(err) return(0))))
    if(ltmp < 2){
      break
    }else{
      tmp2 <- tmp2 + as.double(unlist(tmp1[[i]])[2])
      if(i %% layers == 0){
        ans[count] <- tmp2
        tmp2 <- 0
        count <- count + 1
      }

    }
  }
  ans[1:count]
}
      
## Test
data(weather04)
##res <- tf(res <- BioGro(weather04, soilControl = soilParms(soilLayers=5,hydrDist=1)),5)
res0 <- BioGro(weather04, soilControl = soilParms(soilLayers=1,hydrDist=0,soilDepth=1))
plot(res0, plot.kind="SW")

res1 <- BioGro(weather04, soilControl = soilParms(soilLayers=5,hydrDist=0,soilDepth=1))
plot(res1, plot.kind="SW")

res2 <- BioGro(weather04, soilControl = soilParms(soilLayers=5,hydrDist=1,soilDepth=1))
plot(res2, plot.kind="SW")

xyplot(res0$SoilWatCont + res1$SoilWatCont + res2$SoilWatCont ~ res1$ThermalT,
       type = "l",
       auto.key=TRUE)
