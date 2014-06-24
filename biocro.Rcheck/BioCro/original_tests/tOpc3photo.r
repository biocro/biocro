## Testing the Opc3photo funciton
## Generate 300 A/Ci curves with different vmax, jmax, and Rd values

set.seed(12345)
vmaxs <- 100 + rnorm(10,sd=10)
jmaxs <- 200 + rnorm(10,sd=20)
Rds <- 2 + rnorm(3,sd=0.4)

parms <- expand.grid(vmax=vmaxs,jmax=jmaxs,Rd=Rds)

Catms <- c(0,5,10,20,30,50,75,100,200,300,500,1000)

mat <- matrix(ncol=10,nrow=nrow(parms)*length(Catms))
k <- 1
for(i in seq_along(parms[,1])){

  for(j in seq_along(Catms)){
    tmp <- c3photo(1500,25,0.7,vcmax=parms[i,1],jmax=parms[i,2],
                      Rd=parms[i,3],Catm=Catms[j])
    mat[k,1] <- i
    mat[k,2] <- tmp$Assim # Could add some error + rnorm(1,sd=0.75) 
    mat[k,3] <- 1500
    mat[k,4] <- 25
    mat[k,5] <- 0.7
    mat[k,6] <- tmp$Ci # Could add some error + rnorm(1,sd=0.75)
    mat[k,7] <- parms[i,1]
    mat[k,8] <- parms[i,2]
    mat[k,9] <- parms[i,3]
    mat[k,10] <- Catms[j]
    k <- k + 1
  }
}

colnames(mat) <- c("run","Assim","Qp","Tl","RH","Ci","Vcmax","Jmax","Rd","Catm")

## Plotting a sample
sst <- sample(1:300,20)
mats <- mat[mat[,1] %in% sst,]
matd <- as.data.frame(mats)
xyplot(Assim ~ Ci | factor(run), data = matd, type="o",
       ylab="Assim",xlab="Ci")

simAssim <- mat
## save(simAssim,file="../data/simAssim.rda")

## simA100 <- mat[mat[,1]==100,-1]
## simA100[,1] <- simA100[,1] + rnorm(1)
## save(simA100,file="../data/simA100.rda")


simAssim2 <- cbind(simAssim[,1:6],simAssim[,10])
## First run to get starting values
op.all <- mOpc3photo(simAssim2, op.level=2)
op.all

colm <- apply(op.all$mat[,2:4],2,mean,na.rm=TRUE)
ival <- op.all$mat[,2:4]
ival[is.na(ival[,1]),1] <- colm[1]
ival[is.na(ival[,2]),2] <- colm[2]
ival[is.na(ival[,3]),3] <- colm[3]

## Now we can run it again.
op.all2 <- mOpc3photo(simAssim2, iVcmax=ival[,1], iJmax=ival[,2], iRd=ival[,3], op.level=2)
op.all2

## all of them  converged
res <- op.all2$mat

par(mfrow=c(3,1))
plot(parms[,1],res[,2],xlab="Obs (true)",ylab="Sim (est)",main="Vcmax")
abline(0,1)
plot(parms[,2],res[,3],xlab="Obs (true)",ylab="Sim (est)",main="Jmax")
abline(0,1)
plot(parms[,3],res[,4],xlab="Obs (true)",ylab="Sim (est)",main="Rd")
abline(0,1)
par(mfrow=c(1,1))

## The ones that converged did find the true values almost exactly





