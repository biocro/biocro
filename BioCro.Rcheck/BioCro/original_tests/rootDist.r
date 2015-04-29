## File to test the distribution of roots

data(weather05)
slayers <- 20
res <- BioGro(weather05, soilControl=soilParms(soilDepth=2,soilLayers=slayers))
nrdmat <- data.frame(TT=rep(res$ThermalT,each=slayers),Depth=rep(as.numeric(colnames(res$rdMat)),length(res$ThermalT)),root=c(t(res$rdMat)))
xyplot(-Depth ~ -root , data = nrdmat[nrdmat$TT %in% res$ThermalT[c(0,50,100,500,1000,2000,3000)],],type="o", groups=TT)
