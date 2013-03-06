#c3photo predicts assimilation rate well based on the aci and aq curves measured on SALIX (WANG, 2010). The r2 is 0.94 and 0.9.
#however, the model did not do well on predicting gs

#TEST USING AQ CURVES MEASURED ON WILLOW, VCMAX=120 (2011), b1=6.5 (measured by katie)
willowaq2011 <- read.csv("~/biocro/data/willowaq2011.csv", header=T)
Qps <- willowaq2011$PARi
Tls <- willowaq2011$Tleaf
rhs <- willowaq2011$RH_R/100
dat1 <- data.frame(Qp=Qps,Tl=Tls,RH=rhs)
#dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))
model <- c3photo(dat1$Qp,dat1$Tl,dat1$RH,vcmax=120,Ca=370,b1=6.5)
modelassim<- cbind(model$Assim,willowaq2011)

df1=modelassim[,c("Photo","model$Assim")]
names(df1)=c("x","y")

lm_eqn = function(df1){
  m = lm(y ~ x, df1);
  eq <- substitute(italic(y) == a + b %.% italic(x)*","~~italic(r)^2~"="~r2,
                   list(a = format(coef(m)[1], digits = 2), 
                        b = format(coef(m)[2], digits = 2), 
                        r2 = format(summary(m)$r.squared, digits = 2)))
  #p = format(summary(m)$coefficients[8],digits=2))
  as.character(as.expression(eq));                 
}
library(ggplot2)
rassim <-ggplot(modelassim,aes(Photo,model$Assim))+geom_point(size=2.5,alpha=0.75)+stat_smooth(method="lm")+
  scale_x_continuous("Observed Assim") + # have tick marks for each session
  scale_y_continuous("Modelled Assim") +# rescale Y axis slightly
  theme_bw() + # make the theme black-and-white rather than grey (do this before font changes, or it overrides them)
  opts(#title = "Amax ~ Yield",
    #plot.title = theme_text(face="bold", size=14), # use theme_get() to see available options
    axis.title.x = theme_text(face="bold", size=12),
    axis.title.y = theme_text(face="bold", size=12, angle=90),
    panel.grid.major = theme_blank(), # switch off major gridlines
    panel.grid.minor = theme_blank() # switch off minor gridlines
    )
rassim <- rassim+geom_text(aes(x =5, y = 20, label = lm_eqn(df1)), parse = TRUE,colour="black")

modelgs<- cbind(model$Gs/1000,willowaq2011)
df2=modelgs[,c("Cond","model$Gs/1000")]
names(df2)=c("x","y")

lm_eqn = function(df2){
  m = lm(y ~ x, df2);
  eq <- substitute(italic(y) == a + b %.% italic(x)*","~~italic(r)^2~"="~r2,
                   list(a = format(coef(m)[1], digits = 2), 
                        b = format(coef(m)[2], digits = 2), 
                        r2 = format(summary(m)$r.squared, digits = 2)))
  #p = format(summary(m)$coefficients[8],digits=2))
  as.character(as.expression(eq));                 
}

rgs <-ggplot(modelgs,aes(Cond,model$Gs/1000))+geom_point(size=2.5,alpha=0.75)+stat_smooth(method="lm")+
  scale_x_continuous("Observed Gs") + # have tick marks for each session
  scale_y_continuous("Modelled Gs") +# rescale Y axis slightly
  theme_bw() + # make the theme black-and-white rather than grey (do this before font changes, or it overrides them)
  opts(#title = "Amax ~ Yield",
    #plot.title = theme_text(face="bold", size=14), # use theme_get() to see available options
    axis.title.x = theme_text(face="bold", size=12),
    axis.title.y = theme_text(face="bold", size=12, angle=90),
    panel.grid.major = theme_blank(), # switch off major gridlines
    panel.grid.minor = theme_blank() # switch off minor gridlines
   )
rgs <- rgs+geom_text(aes(x =0.2, y = 0.25, label = lm_eqn(df2)), parse = TRUE,colour="black")

modelci<- cbind(model$Ci,willowaq2011)
df3=modelci[,c("Ci","model$Ci")]
names(df3)=c("x","y")

lm_eqn = function(df3){
  m = lm(y ~ x, df3);
  eq <- substitute(italic(y) == a + b %.% italic(x)*","~~italic(r)^2~"="~r2,
                   list(a = format(coef(m)[1], digits = 2), 
                        b = format(coef(m)[2], digits = 2), 
                        r2 = format(summary(m)$r.squared, digits = 2)))
  #p = format(summary(m)$coefficients[8],digits=2))
  as.character(as.expression(eq));                 
}

rci <-ggplot(modelci,aes(Ci,model$Ci))+geom_point(size=2.5,alpha=0.75)+stat_smooth(method="lm")+
  scale_x_continuous("Observed Ci") + # have tick marks for each session
  scale_y_continuous("Modelled Ci") +# rescale Y axis slightly
  theme_bw() + # make the theme black-and-white rather than grey (do this before font changes, or it overrides them)
  opts(#title = "Amax ~ Yield",
    #plot.title = theme_text(face="bold", size=14), # use theme_get() to see available options
    axis.title.x = theme_text(face="bold", size=12),
    axis.title.y = theme_text(face="bold", size=12, angle=90),
    panel.grid.major = theme_blank(), # switch off major gridlines
    panel.grid.minor = theme_blank() # switch off minor gridlines
    )
rci <- rci+geom_text(aes(x =250, y = 380, label = lm_eqn(df3)), parse = TRUE,colour="black")

# testing using instaneous measurements on SALIX, this is not working, as the willow in the novel woody is not healthy enough

Photo <- read.csv("~/biocro/data/Photo.csv")
test2 <- Photo[,c(5,6,7,8,9,14,15,16,26,28,32,35,77)]
test2 <- test2[test2$sp=="SALIX",]
Qps <- test2$PARi
Tls <- test2$Tleaf
rhs <- test2$RH_R/100
dat2 <- data.frame(Qp=Qps,Tl=Tls,RH=rhs)
#dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))
model2 <- c3photo(dat2$Qp,dat2$Tl,dat2$RH,vcmax=120)
modelassim2<- cbind(model2$Assim,test2)

df4=modelassim2[,c("Photo","model2$Assim")]
names(df4)=c("x","y")

lm_eqn = function(df4){
  m = lm(y ~ x, df4);
  eq <- substitute(italic(y) == a + b %.% italic(x)*","~~italic(r)^2~"="~r2,
                   list(a = format(coef(m)[1], digits = 2), 
                        b = format(coef(m)[2], digits = 2), 
                        r2 = format(summary(m)$r.squared, digits = 2)))
  #p = format(summary(m)$coefficients[8],digits=2))
  as.character(as.expression(eq));                 
}

rassim2 <-ggplot(modelassim2,aes(Photo,model2$Assim))+geom_point(size=2.5,alpha=0.75)+stat_smooth(method="lm")+
  scale_x_continuous("Observed Assim") + # have tick marks for each session
  scale_y_continuous("Modelled Assim") +# rescale Y axis slightly
  theme_bw() + # make the theme black-and-white rather than grey (do this before font changes, or it overrides them)
  opts(#title = "Amax ~ Yield",
    #plot.title = theme_text(face="bold", size=14), # use theme_get() to see available options
    axis.title.x = theme_text(face="bold", size=12),
    axis.title.y = theme_text(face="bold", size=12, angle=90),
    panel.grid.major = theme_blank(), # switch off major gridlines
    panel.grid.minor = theme_blank() # switch off minor gridlines
   )
rassim2 <- rassim2+geom_text(aes(x =5, y = 20, label = lm_eqn(df4)), parse = TRUE,colour="black")

#testing using aci curves
aci <- read.csv("~/biocro/data/aci2010.csv")
acisalix <- aci[aci$run==66,]
Qps <- acisalix$Qp
Tls <- acisalix$TL
rhs <- acisalix$RH/100
Catms <- acisalix$X
dat3 <- data.frame(Qp=Qps,Tl=Tls,RH=rhs,Catm=Catms)
#dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))
model3 <- c3photo(dat3$Qp,dat3$Tl,dat3$RH,Catm=Catms,vcmax=100)
modelassim3<- cbind(model3$Assim,acisalix)

df5=modelassim3[,c("Assim","model3$Assim")]
names(df5)=c("x","y")

lm_eqn = function(df5){
  m = lm(y ~ x, df5);
  eq <- substitute(italic(y) == a + b %.% italic(x)*","~~italic(r)^2~"="~r2,
                   list(a = format(coef(m)[1], digits = 2), 
                        b = format(coef(m)[2], digits = 2), 
                        r2 = format(summary(m)$r.squared, digits = 2)))
  #p = format(summary(m)$coefficients[8],digits=2))
  as.character(as.expression(eq));                 
}

rassim3 <-ggplot(modelassim3,aes(Assim,model3$Assim))+geom_point(size=2.5,alpha=0.75)+stat_smooth(method="lm")+
  scale_x_continuous("Observed Assim") + # have tick marks for each session
  scale_y_continuous("Modelled Assim") +# rescale Y axis slightly
  theme_bw() + # make the theme black-and-white rather than grey (do this before font changes, or it overrides them)
  opts(#title = "Amax ~ Yield",
    #plot.title = theme_text(face="bold", size=14), # use theme_get() to see available options
    axis.title.x = theme_text(face="bold", size=12),
    axis.title.y = theme_text(face="bold", size=12, angle=90),
    panel.grid.major = theme_blank(), # switch off major gridlines
    panel.grid.minor = theme_blank() # switch off minor gridlines
  )
rassim3 <- rassim3+geom_text(aes(x =5, y = 20, label = lm_eqn(df5)), parse = TRUE,colour="black")
