## Code provided by Corentin M. Barbu
## http://stackoverflow.com/a/28678741/513006
library("devtools")

#' devtools document function when using doxygen
#' @description Overwrites devtools::document() to include the treatment of 
#'    doxygen documentation in src/
#' @return The value returned by devtools::document()
#' @export
document <- function(){
  if(file.exists("src")){
    doxyFileName<-"documentation/Doxyfile"
    if(!file.exists(doxyFileName)){
      stop("can't find Doxyfile")#DoxInit()
    }
    system(paste("doxygen",doxyFileName))
  }
  devtools::document()
}
