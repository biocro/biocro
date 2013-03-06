test=as.matrix(aci2010)
mode(test)='numeric'
colnames(test)[4]="Tl"
test[,5]=test[,5]/100
op.woody2010 <- mOpc3photo(test[,1:7], op.level=2)
op.woody2010$mat
colm <- apply(op.woody2010$mat[,2:4],2,mean,na.rm=TRUE)
ival <- op.woody2010$mat[,2:4]
ival[is.na(ival[,1]),1] <- colm[1]
ival[is.na(ival[,2]),2] <- colm[2]
ival[is.na(ival[,3]),3] <- colm[3]

## Now we can run it again.
op.woody20102 <- mOpc3photo(test[,1:7], iVcmax=ival[,1], iJmax=ival[,2], iRd=ival[,3], op.level=2)
op.woody20102

## all of them  converged
res <- op.woody20102$mat

par(mfrow=c(3,1))
plot(parms[,1],res[,2],xlab="Obs (true)",ylab="Sim (est)",main="Vcmax")
abline(0,1)
plot(parms[,2],res[,3],xlab="Obs (true)",ylab="Sim (est)",main="Jmax")
abline(0,1)
plot(parms[,3],res[,4],xlab="Obs (true)",ylab="Sim (est)",main="Rd")
abline(0,1)
par(mfrow=c(1,1))
