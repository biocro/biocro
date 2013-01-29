##' This function checks if the current year is a leap year.
##'
##' @title Check Leap Year
##' @param year
##' @return returns 1 if leap year, else 0.
##' @author Fernando Miguez
CheckLeapYear <- function(year) {
    if ((year%%400 == 0) || ((year%%100 != 0) && (year%%4 == 0))) {
        temp <- TRUE
    } else {
        temp <- FALSE
    }
    return(temp)
} 
