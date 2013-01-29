## Temporary function to debug soil model
tf <- function(x, layers) {
    ltmp <- 3
    ans <- numeric(50000)
    count <- 1
    tmp2 <- 0
    tmp <- capture.output(x)
    tmp1 <- lapply(tmp, function(x) strsplit(x, " "))
    ## Start loop
    for (i in 1:50000) {
        ltmp <- length(unlist(tryCatch(tmp1[[i]], error = function(err) return(0))))
        if (ltmp < 2) {
            break
        } else {
            tmp2 <- tmp2 + as.double(unlist(tmp1[[i]])[2])
            if (i%%layers == 0) {
                ans[count] <- tmp2
                tmp2 <- 0
                count <- count + 1
            }
        }
    }
    ans[1:count]
} 
