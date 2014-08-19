## This is the R version of the Century model
## At the bottom is the interface to the C version

##########################################################################################
#
#  TODO
#
#   1. The input should be in g m^-2 for the century model, so If I have
#      Mg ha^-1 I need to make the conversion: 1 Mg ha^-1 is 100 g m^-2. This is done in the C version
#   2. Add to the Nitrogen submodel the NO3 leaching, and the other destinations.
#   3. Need to add the N uptake, N2 loss and N2O and NOx losses
#
##########################################################################################
##' This function implements the Century model from Parton.
##'
##' Calculates flows of soil organic carbon and nitrogen based on the Century
##' model. There are two versions one written in R (Century) and one in C
##' (CenturyC) they should give the same result. The C version only runs at
##' weekly timesteps.
##'
##' Most of the details can be found in the papers by Parton et al. (1987,
##' 1988, 1993)
##'
##' @aliases Century CenturyC
##' @param LeafL Leaf litter.
##' @param StemL Stem litter.
##' @param RootL Root litter.
##' @param RhizL Rhizome litter.
##' @param smoist Soil moisture.
##' @param stemp Soil temperature.
##' @param precip Precipitation.
##' @param leachWater Leached water.
##' @param centuryControl See \code{\link{centuryParms}}.
##' @param verbose Only used in the R version for debugging.
##' @param soilType See \code{\link{showSoilType}}.
##' @export
##' @return A list with,
##' \itemize{
##' \item SCs Soil carbon pools 1-9.
##' \item SNs Soil nitrogen pools 1-9.
##' \item MinN Mineralized nitrogen.
##' \item Resp Soil respiration.
##' }
##' @author Fernando E. Miguez
##' @references ~put references to the literature/web site here ~
##' @keywords models
##' @examples
##'
##' Century(0,0,0,0,0.3,5,2,2)$Resp
##' Century(0,0,0,0,0.3,5,2,2)$MinN
##'
##'
Century <- function(LeafL, StemL, RootL, RhizL, smoist, stemp, precip, leachWater, centuryControl = list(),verbose=FALSE){

  ## I need the separate fractions of Leaf Litter and Stem Litter
  ## Because they have different lignin to N ratios
  ## Presumably, the rhizome and root are similar

  ## There is an issue here that I should be careful about I'm importing
  ## biomass, but need only carbon in some of these calculations

  centuryP <- centuryParms()
  centuryP[names(centuryControl)] <- centuryControl

  timestep <- centuryP$timestep

  if(verbose) cat("Ks :",centuryP$Ks, "\n");
  
  ## Additional nitrogen processes
  ## N deposition

  Na = 0.21 + 0.0028 * precip * 0.10 ; # precipitation is entered in mm
                                   # but it is needed in cm for this
                                   # equation. Idem below
  ## N fixation
  Nf = -0.18 + 0.014 * precip * 0.10 ; 
  ## The resulting N is in g N m^-2 yr^-1
  ## Conversions
  ## g => Mg : multiply by 1e-6
  ## m^2 = > ha : multiply by 1e4
  ## year to week : divide by 52
  if(timestep == 7){
    Na = Na * (1/52);
    Nf = Nf * (1/52);
  }else
  if(timestep == 1){
    Na = Na * (1/365);
    Nf = Nf * (1/365);
  }

  ## Nitrogen in the form of fertilizer
  Nfert = centuryP$Nfert[1]; # The input units should be in g N m^-2

  MinN = Na + Nf + Nfert + centuryP$iMinN;

  if(verbose) cat("MinN 0: ",MinN,"\n");
  ## I can calculate leaching by assuming that the excess water from
  ## my simple water budget contains NO3 and that this NO3 is lost
  
  T = 0.47; # silt plus clay content of the soil
  Ts = 0.53; # Sand content of the soil
  Tc = 0.4; # Clay content of the soil
  ## Termed T in the original paper

  ## The value 17% for lignin content corresponds to maize stover
  LeafL.Ln = centuryP$LeafL.Ln;
  StemL.Ln = centuryP$StemL.Ln;
  RootL.Ln = centuryP$RootL.Ln;
  RhizL.Ln = centuryP$RhizL.Ln;
  ## The value 0.4 % corresponds to maize stover
  LeafL.N = centuryP$LeafL.N;
  StemL.N = centuryP$StemL.N;
  RootL.N = centuryP$RootL.N;
  RhizL.N = centuryP$RhizL.N;
  
  ## Calculate the CtoN ratio for surface microbe

  PlantN = LeafL * LeafL.N + StemL * StemL.N;
  
  CN.structural = 150;
  CN.surface = 20 - PlantN * 5;
  CN.active = 15 - MinN * 6;
  CN.slow = 20 - MinN * 4;
  CN.passive = 10 - MinN * 1.5;

  if(PlantN > 2) CN.surface = 10;

  ## Here 2 is g m^-2
  if(MinN > 2){
    CN.active = 3;
    CN.passive = 7;
    CN.slow = 12;
  }

  SC1 = centuryP$SC1;
  SC2 = centuryP$SC2;
  SC3 = centuryP$SC3;
  SC4 = centuryP$SC4;
  SC5 = centuryP$SC5;
  SC6 = centuryP$SC6;
  SC7 = centuryP$SC7;
  SC8 = centuryP$SC8;
  SC9 = centuryP$SC9;


  SN1 = SC1 / CN.structural; 
  SN2 = SC2 / CN.surface;
  SN3 = SC3 / CN.structural;
  SN4 = SC4 / CN.active;
  SN5 = SC5 / CN.surface;
  SN6 = SC6 / CN.active;
  SN7 = SC7 / CN.slow;
  SN8 = SC8 / CN.passive;
  SN9 = SC9 / CN.passive;
  
  respC1.5 = 0.6;
  respC1.7 = 0.3;
  respC3.7 = 0.3;
  respC2.5 = 0.6;
  respC3.6 = 0.55;
  respC4.6 = 0.55;
  respC5.7 = 0.6;
  respC6 = 0.85 - 0.68 * T;
  respC7 = 0.55;
  respC8 = 0.55;
  
  Tm = 1 - 0.75 * T;

  if(verbose){
    cat("Tm :",Tm,"\n")
 } 
  ## Tm is the effect of soil texture on active SOM turnover
  
  ## The flow constants are taken from the paper Parton et al. 1987 SSSJ 51:1173
  ## and Parton et al. 1993 Global Biogeochemistry pg 785
  Ks <- centuryP$Ks

  if(timestep == "week"){
    Ks = Ks / 52; # The units are week^-1
  }else
  if(timestep == "day"){
    Ks = Ks / 365; # The units are day^-1 
    }
  
  ## Need to calculate the effect of temperature and moisture.
  if(stemp < 35){
    TempEff = 1.0087 / (1 + (46.2 * exp(-0.1899 * stemp)));
  }else{
    TempEff = -0.0826 * stemp + 3.84;
  }
  
  MoisEff = 1.0267 / (1 + 14.7 * exp(-6.5 * smoist));

  Abiot = TempEff * MoisEff ;

  ## Calculate Fm and Lc separately for each component
  FmLc.Leaf = FmLcFun(LeafL.Ln,LeafL.N);
  FmLc.Stem = FmLcFun(StemL.Ln,StemL.N);
  FmLc.Root = FmLcFun(RootL.Ln,RootL.N);
  FmLc.Rhiz = FmLcFun(RhizL.Ln,RhizL.N);

  ## Surface Metabolic Carbon
  SC2.Leaf = FmLc.Leaf$Fm * LeafL;
  SC2.Stem = FmLc.Stem$Fm * StemL;
  ## Root Metabolic Carbon
  SC4.Root = FmLc.Root$Fm * RootL;
  SC4.Rhiz = FmLc.Rhiz$Fm * RhizL;

  ## Surface Structural Carbon
  SC1.Leaf = (1 - FmLc.Leaf$Fm) * LeafL;
  SC1.Stem = (1 - FmLc.Stem$Fm) * StemL;

  ## Adding the extra arrows 12-11
  SC1.Leaf.Ln = SC1.Leaf * LeafL.Ln;
  SC1.Stem.Ln = SC1.Stem * StemL.Ln;
  SC1.Leaf = SC1.Leaf - SC1.Leaf.Ln;
  SC1.Stem = SC1.Stem - SC1.Stem.Ln;

  ## Root Structural Carbon
  SC3.Root = (1 - FmLc.Root$Fm) * RootL;
  SC3.Rhiz = (1 - FmLc.Rhiz$Fm) * RhizL;

  ## Adding the extra arrows 12-11
  SC3.Root.Ln = SC3.Root * RootL.Ln;
  SC3.Rhiz.Ln = SC3.Rhiz * RhizL.Ln;
  SC3.Root = SC3.Root - SC3.Root.Ln;
  SC3.Rhiz = SC3.Rhiz - SC3.Rhiz.Ln;
  
  ## T is silt plus clay content
  ## Ls is fraction of structural C that is lignin

  ## Structural Surface Litter C to Surface Microbe C
  ## 1 => 5
  ## 2 => 5
  ## dC1/dt = Ki * Lc * A * Ci
  ## Leaf
  SC1.Leaf = SC1.Leaf + 0.3 * SC1;
  SC2.Leaf = SC2.Leaf + 0.3 * SC2;
  C1.5.Leaf = flow(SC1.Leaf,CN.surface,Abiot,FmLc.Leaf$Lc,Tm,respC1.5,1,Ks,verbose);
  C2.5.Leaf = flow(SC2.Leaf,CN.surface,Abiot,FmLc.Leaf$Lc,Tm,respC1.5,5,Ks,verbose);
  ## Stem
  SC1.Stem = SC1.Stem + 0.7 * SC1;
  SC2.Stem = SC2.Stem + 0.7 * SC2;
  C1.5.Stem = flow(SC1.Stem,CN.surface,Abiot,FmLc.Stem$Lc,Tm,respC2.5,1,Ks,verbose);
  C2.5.Stem = flow(SC2.Stem,CN.surface,Abiot,FmLc.Stem$Lc,Tm,respC2.5,5,Ks,verbose);

  SC1.Leaf = C1.5.Leaf$SC;
  SC2.Leaf = C2.5.Leaf$SC;
  SC1.Stem = C1.5.Stem$SC;
  SC2.Stem = C2.5.Stem$SC;

  ## Adding the ligning content
  C1.7.Leaf.Ln = flow(SC1.Leaf.Ln,CN.slow,Abiot,FmLc.Leaf$Lc,Tm,respC1.7,1,Ks,verbose);
  C1.7.Stem.Ln = flow(SC1.Stem.Ln,CN.slow,Abiot,FmLc.Stem$Lc,Tm,respC1.7,1,Ks,verbose);

  SC7 = C1.7.Leaf.Ln$fC + C1.7.Stem.Ln$fC  + SC7;
##   SC1.Ln = C1.5.Leaf.Ln$SC + C1.5.Stem.Ln$SC;
  
  ## Collect respiration
  Resp = C1.5.Leaf$Resp + C2.5.Leaf$Resp +
         C1.5.Stem$Resp + C2.5.Stem$Resp +
         C1.7.Leaf.Ln$Resp + C1.7.Stem.Ln$Resp;
  ## Collect mineralized Nitrogen
  MinN = MinN +  C1.5.Leaf$MinN + C2.5.Leaf$MinN +
                 C1.5.Stem$MinN + C2.5.Stem$MinN +
                 C1.7.Leaf.Ln$MinN + C1.7.Stem.Ln$MinN;
                   

  if(verbose){
    cat("Resp:",Resp,"\n")
    cat("Min 1:",MinN,"\n")
  }

  ## Updating the Soil Carbon Pools 1 and 2 
  SC1 = C1.5.Leaf$SC + C1.5.Stem$SC + C1.7.Leaf.Ln$SC + C1.7.Stem.Ln$SC;
  SC2 = C2.5.Leaf$SC + C2.5.Stem$SC ;

  ## Updating the Nitrogen Carbon Pools 1 and 2
  SN1 = SC1 / CN.structural + SN1;
  SN2 = SC2 / CN.surface + SN2;
  
  ## Structural Root Litter C to Soil Microbe C
  ## 4 => 6
  ## 3 => 6
  ## Root
  SC3.Root = SC3.Root + 0.3 * SC3;
  SC4.Root = SC4.Root + 0.3 * SC4;
  C3.6.Root = flow(SC3.Root,CN.active,Abiot,FmLc.Root$Lc,Tm,respC3.6,2,Ks,verbose);
  C4.6.Root = flow(SC4.Root,CN.active,Abiot,FmLc.Root$Lc,Tm,respC3.6,6,Ks,verbose);
  ## Rhizome
  SC3.Rhiz = SC3.Rhiz + 0.7 * SC3;
  SC4.Rhiz = SC4.Rhiz + 0.7 * SC4;
  C3.6.Rhiz = flow(SC3.Rhiz,CN.active,Abiot,FmLc.Rhiz$Lc,Tm,respC4.6,2,Ks,verbose);
  C4.6.Rhiz = flow(SC4.Rhiz,CN.active,Abiot,FmLc.Rhiz$Lc,Tm,respC4.6,6,Ks,verbose);

  SC3.Root = C3.6.Root$SC;
  SC4.Root = C4.6.Root$SC;
  SC3.Rhiz = C3.6.Rhiz$SC;
  SC4.Rhiz = C4.6.Rhiz$SC;

  ## Adding the lignin content
  C3.7.Root.Ln = flow(SC3.Root.Ln,CN.slow,Abiot,FmLc.Root$Lc,Tm,respC3.7,2,Ks,verbose);
  C3.7.Rhiz.Ln = flow(SC3.Rhiz.Ln,CN.slow,Abiot,FmLc.Rhiz$Lc,Tm,respC3.7,2,Ks,verbose);

  SC7 = SC7 + C3.7.Root.Ln$fC + C3.7.Rhiz.Ln$fC ;
  ## Collect respiration
  Resp = Resp + C3.6.Root$Resp + C4.6.Root$Resp +
                C3.6.Rhiz$Resp + C4.6.Rhiz$Resp;

  ## Collect mineralized Nitrogen
  MinN = MinN + C3.6.Root$MinN + C4.6.Root$MinN +
                C3.6.Rhiz$MinN + C4.6.Rhiz$MinN +
                C3.7.Root.Ln$MinN + C3.7.Rhiz.Ln$Min;

  if(verbose){
    cat("Resp:",Resp,"\n")
    cat("MinN 2:",MinN,"\n")
  }
  
  ## Updating the Soil Carbon Pools 3 and 4
  SC3 = C3.6.Root$SC + C3.6.Rhiz$SC + C3.7.Root.Ln$SC + C3.7.Rhiz.Ln$SC ;
  SC4 = C4.6.Root$SC + C4.6.Rhiz$SC ;

  ## Updating the Nitrogen pools 3 and 4
  SN3 = SC3 / CN.structural + SN3;
  SN4 = SC4 / CN.active + SN4;

  ## Updating the Soil Carbon Pool 5
  SC5 = C1.5.Leaf$fC + C1.5.Stem$fC + C2.5.Leaf$fC + C2.5.Stem$fC + SC5;

  # Updating the Soil Nitrogen pool 5
  SN5 = SC5 / CN.surface + SN5;
  
  ## Updating the Soil Carbon Pool 6
  SC6 = C3.6.Root$fC + C3.6.Rhiz$fC + C4.6.Root$fC + C4.6.Rhiz$fC + SC6;

  ## Updating the Soil Carbon Pool 6
  SN6 = SC6 / CN.active + SN6;

  ## Surface Microbe C to Slow C
  ## 5 => 7
  C5.7 = flow(SC5,CN.slow,Abiot,0,0,respC5.7,4,Ks,verbose);

  Resp = Resp + C5.7$Resp;
  MinN = MinN + C5.7$MinN;

  if(verbose){
    cat("Resp:",Resp,"\n")
    cat("MinN 3:",MinN,"\n")
  }
  ## Updating Surface Microbe C (pool 5) and slow (pool 7)

  SC5 = C5.7$SC ;
  SC7 = C5.7$fC + SC7;

  ## Updating Surface Microbe N pool

  SN5 = SC5 / CN.surface;
   
  ## Soil Microbe C to intermediate stage C
  C6 = flow(SC6,CN.slow,Abiot,0,Tm,respC6,3,Ks,verbose);

  Resp = Resp + C6$Resp;
  MinN = MinN + C6$MinN;

  if(verbose) cat("Resp:",Resp,"\n")

  ## Updating carbon and soil pools 6
  SC6 = C6$SC;
  SN6 = SC6 / CN.active;
  
  C.ap = 0.003 + 0.032 * Tc;   
  C.al = leachWater/18 * (0.01 + 0.04 * Ts);

  if(verbose){
    cat("C.ap : ",C.ap,"\n")
    cat("C.al : ",C.al,"\n")
  }
  
  C6.8 = C6$fC * C.ap;
  C6.9 = C6$fC * C.al;
  C6.7 = C6$fC * (1 - C.ap - C.al);
  
  ## Updating the Soil Carbon Pool 7, 8 and 9

  SC7 = C6.7 + SC7;
  SC8 = C6.8 + SC8;
  SC9 = C6.9 + SC9;

  ## Updating the Soil Nitrogen Pool 7, 8 and 9

  SN7 = SC7 / CN.slow + SN7;
  SN8 = SC8 / CN.passive + SN8;
  SN9 = SC9 / CN.slow + SN9;
  
  ## Slow Carbon to intermediate stage

  C7 = flow(SC7,CN.slow,Abiot,0,0,respC7,7,Ks,verbose);

  Resp = Resp + C7$Resp;
  MinN = MinN + C7$MinN;
  
  if(verbose){
    cat("Resp ",Resp,"\n")
    cat("MinN 4:",MinN,"\n")
  }
  
  C.sp = 0.003 - 0.009 *Tc;
  C7.8 = C7$fC * C.sp;
  C7.6 = C7$fC * (1 - C.sp); # There is no need to subtract 0.55 since
                             # this was already taken into account in
                             # the flow equation

  ## Updating the Soil Carbon Pools 6 and 8

  SC6 = C7.6 + SC6;
  SC8 = C7.8 + SC8;

  ## Updating the Soil Nitrogen Pools 6 and 8

  SN6 = SC6 / CN.active;
  SN8 = SN8 / CN.passive;

  ## Passive Carbon to Soil Microbe C

  C8.6 = flow(SC8,CN.passive,Abiot,0,0,respC8,8,Ks,verbose);

  Resp = Resp + C8.6$Resp;
  MinN = MinN + C8.6$MinN;

  if(verbose){
    cat("Resp:",Resp,"\n")
    cat("MinN 5:",MinN,"\n")
  }

  ## Updating the Soil Microbe C 6 and 8
  
  SC8 = C8.6$SC ;
  SC6 = C8.6$fC + SC6;

  SN6 = SC6 / CN.active;
  SN8 = SC8 / CN.passive;

  SCs <- c(SC1,SC2,SC3,SC4,SC5,SC6,SC7,SC8,SC9)
  SNs <- c(SN1,SN2,SN3,SN4,SN5,SN6,SN7,SN8,SN9)
  list(SCs=SCs, SNs=SNs, MinN=MinN, Resp=Resp)
}


FmLcFun <- function(Lig,Nit){

  Fm = 0.85 - 0.018 *(Lig/Nit);
  Ls = Lig / (1 - Fm);
  Lc = exp(-3 * Ls);

  list(Lc=Lc,Fm=Fm)

}


flow <- function(SC,CNratio,A,Lc,Tm,resp,kno,Ks,verbose=FALSE){

  if(kno < 3){
    Kf = Ks[kno] * Lc * A 
    fC = Kf * SC;
  }else
  if(kno == 3){
    Kf = Ks[kno] * A * Tm
    fC = Kf * SC;
  }else
  if(kno > 3){
    Kf = Ks[kno] * A
    fC = Kf * SC;
  }

  if(is.na(Kf)) warning(paste("Kf is NA: ",kno))
  else if(Kf > 1) warning(paste("Kf greater than 1: ",Kf,A,Lc,kno))

  if(verbose){
    cat("Kf :",Kf," kno: ",kno,"\n")
  }
  
  Resp = fC * resp;
  if(Resp < 0) warning(paste("Resp less than zero",Resp))

  ## Mineralized N
  MinN = Resp / CNratio;

  SC = SC - fC;
  
  fC = fC - Resp;
#  fN = fC / CNratio;

## It is important to keep track of C emissions because
## I might need to validate it against Eddy flux data
  list(SC=SC, fC=fC, Resp=Resp, Kf=Kf, MinN=MinN)

}
  
centuryParms <- function(SC1=1,SC2=1,SC3=1,SC4=1,SC5=1,SC6=1,
                       SC7=1,SC8=1,SC9=1,
                       LeafL.Ln=0.17,StemL.Ln=0.17,RootL.Ln=0.17,RhizL.Ln=0.17,
                       LeafL.N=0.004,StemL.N=0.004,RootL.N=0.004,RhizL.N=0.004,
                       Nfert=c(0,0),iMinN=0, Litter = c(0,0,0,0),
                       timestep=c("day","week","year"),
                         Ks =  c(3.9, 4.9, 7.3, 6.0, 14.8, 18.5, 0.2, 0.0045)){

  timestep <- match.arg(timestep)

  if(length(Ks) != 8)
    stop("Length of Ks should be equal to 8")

  if(length(Nfert) != 2)
    stop("Nfert should be of length 2")
  
  list(SC1=SC1,SC2=SC2,SC3=SC3,
       SC4=SC4,SC5=SC5,SC6=SC6,SC7=SC7,SC8=SC8,SC9=SC9,
       LeafL.Ln=LeafL.Ln,StemL.Ln=StemL.Ln,RootL.Ln=RootL.Ln,RhizL.Ln=RhizL.Ln,
       LeafL.N=LeafL.N,StemL.N=StemL.N,RootL.N=RootL.N,RhizL.N=RhizL.N,
       Nfert=Nfert, iMinN=iMinN, Litter = Litter, timestep=timestep, Ks = Ks)

}


## The Century C version need to be rewritten

CenturyC <- function(LeafL, StemL, RootL, RhizL, smoist, stemp, precip, leachWater, centuryControl = list(),soilType=0){

  ## The C version accepts biomass in Mg ha^-1
  ## but I want the R version to accept biomass in g m^-2
  LeafL <- LeafL / 100
  StemL <- StemL / 100
  RootL <- RootL / 100
  RhizL <- RhizL / 100
  
  centuryP <- centuryParms()
  centuryP[names(centuryControl)] <- centuryControl

  timestep <- centuryP$timestep
  
  SCCs <- c(centuryP$SC1,centuryP$SC2,centuryP$SC3,centuryP$SC4,centuryP$SC5,centuryP$SC6,centuryP$SC7,centuryP$SC8,centuryP$SC9)

  SCCs <- SCCs / 100
  ## The value 17% for lignin content corresponds to maize stover
  LeafL.Ln <- centuryP$LeafL.Ln
  StemL.Ln <- centuryP$StemL.Ln
  RootL.Ln <- centuryP$RootL.Ln
  RhizL.Ln <- centuryP$RhizL.Ln
  ## The value 0.4 % corresponds to maize stover
  LeafL.N <- centuryP$LeafL.N
  StemL.N <- centuryP$StemL.N
  RootL.N <- centuryP$RootL.N
  RhizL.N <- centuryP$RhizL.N

  Ks <- centuryP$Ks
  
  if(timestep == "year") timestep <- 365
  if(timestep == "week") timestep <- 7
  if(timestep == "day") timestep <- 1

   res <- .Call(cntry,
                as.double(LeafL),             # 1
                as.double(StemL),             # 2
                as.double(RootL),             # 3
                as.double(RhizL),             # 4
                as.double(smoist),            # 5 
                as.double(stemp),             # 6
                as.integer(timestep),         # 7
                as.double(SCCs),              # 8
                as.double(leachWater),        # 9
                as.double(centuryP$Nfert[1]), # 10
                as.double(centuryP$iMinN),    # 11
                as.double(precip),            # 12
                as.double(LeafL.Ln),          # 13
                as.double(StemL.Ln),          # 14
                as.double(RootL.Ln),          # 15
                as.double(RhizL.Ln),          # 16
                as.double(LeafL.N),           # 17
                as.double(StemL.N),           # 18
                as.double(RootL.N),           # 19
                as.double(RhizL.N),           # 20
                as.integer(soilType),         # 21
                as.double(Ks))                # 22

  res$SCs <- res$SCs * 100
  res$SNs <- res$SNs * 100
  res

}
  
