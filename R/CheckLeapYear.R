##' This fuction checks if year is a leap year. If yes, then returns 1 or 0.
##' @param year the year in question
##' @export CheckLeapYear
##' @examples 
##' year==2000 => 1
##' year==1998 => 0 
CheckLeapYear<-function(year)
  {
    if( (year%%400==0) || ((year%%100 !=0) && (year%%4==0))) temp<-1 else temp<-0
    return (temp)
  }
