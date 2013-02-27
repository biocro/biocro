## These functions justify the use of Arden Buck instead of
## Goff-Gratch
##
## References
##
## http://cires.colorado.edu/~voemel/vp.html


fnpsvp <- function(Tkelvin){
#   water boiling point = 373.16 oK

  u = Tkelvin / 373.16;
  v = 373.16 / Tkelvin;

  tmp <- -7.90298 * (v - 1) + 5.02808 * log10(v) 
          - 1.3816e-7 * (10^(11.344 * (1 - u)) - 1)   
          + 8.1328e-3 * (10^(-3.49149 * (v - 1)) - 1)
       + log10(1013.246);

  esat = 100 * 10^tmp;
  esat
}

## Arden Buck Equation

ardBuck <- function(Tcelsius){

  a <- (18.678 - Tcelsius/234.5)*Tcelsius
  b <- 257.14 + Tcelsius
  res <- (6.1121  * exp(a/b))/10
  #Why do I need to divide by 10 to get the same answer?
  res
}
