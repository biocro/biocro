## Testing whether my plots can be sourced
## Load fabricated data
data(simA100)
op <- Opc3photo(simA100[,1:5],Catm=simA100[,9], op.level = 2)

pdf("op.pdf")
plot(op)
dev.off()
