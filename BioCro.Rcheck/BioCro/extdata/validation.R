SelectPredictStem<-function(obs,pred,output,a,b)
{
  Stemo<-obs[(!is.na(obs$Stem)),]$Stem
  dayStemo<-obs[(!is.na(obs$Stem)),]$DAP
  n<-length(dayStemo)
  pred<-data.frame(DAP=DAP,Stem=res$Stem)
  Stemp<-as.vector(n)
  for (i in 1:n)
  {
    tmp<-pred[which(pred[,1]==dayStemo[i]),]$Stem
    Stemp[i]<-tmp
  }
  toadd<-data.frame(Observed=Stemo, Predicted=Stemp,Irrigated=rep(a,length(Stemp)),Plantcrop=rep(b,length(Stemp)))
  output=rbind(output,toadd)
}


SelectPredictLAI<-function(obs,pred,output,a,b)
{
  LAIo<-obs[(!is.na(obs$LAI)),]$LAI
  dayLAIo<-obs[(!is.na(obs$LAI)),]$DAP
  n<-length(dayLAIo)
  pred<-data.frame(DAP=DAP,LAI=res$LAI)
  LAIp<-as.vector(n)
  for (i in 1:n)
  {
    tmp<-pred[which(pred[,1]==dayLAIo[i]),]$LAI
    LAIp[i]<-tmp
  }
  toadd<-data.frame(Observed=LAIo, Predicted=LAIp,Irrigated=rep(a,length(LAIp)),Plantcrop=rep(b,length(LAIp)))
  output=rbind(output,toadd)
}

SelectPredictLeaf<-function(obs,pred,output,a,b)
{
  Leafo<-obs[(!is.na(obs$Leaf)),]$Leaf
  dayLeafo<-obs[(!is.na(obs$Leaf)),]$DAP
  n<-length(dayLeafo)
  pred<-data.frame(DAP=DAP,Leaf=res$Leaf)
  Leafp<-as.vector(n)
  for (i in 1:n)
  {
    tmp<-pred[which(pred[,1]==dayLeafo[i]),]$Leaf
    Leafp[i]<-tmp
  }
  toadd<-data.frame(Observed=Leafo, Predicted=Leafp,Irrigated=rep(a,length(Leafp)),Plantcrop=rep(b,length(Leafp)))
  output=rbind(output,toadd)
}

##############################################################################
# This function is to convert DayofYear from reults to DAP
#########################################################################

# This function is to change doy to date of Original EnCro
to.dap<-function(doy){
  start<-doy[1]  # this is the first date of planting
  nn<-length(doy) # determine length of doy vector
  dap<-numeric(nn)
  dap[1]=doy[1]-start # day after planting is zero in the beginning
  for (i in 0:nn){
    dap[i]=i
  }
  dap
}
