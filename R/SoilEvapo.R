## Function to calculate evaporation directly from the soil
SoilEvapo <- function(LAI, k, AirTemp, IRad, awc, FieldC, WiltP, winds, RelH) {
    method = 0
    SoilClodSize = 0.04
    SoilReflectance = 0.2
    SoilTransmission = 0.01
    SpecificHeat = 1010
    kappa = 5.67e-08  # Stefan Boltzman Constant
    cf2 = 3600 * 0.001 * 18 * 1e-06 * 10000
    # /* Let us assume a simple way of calculating the proportion of the soil with
    # direct radiation */
    SoilArea = exp(-k * LAI)
    ### /* For now the temperature of the soil will be the same as the air.  At a
    ### later time this can be made more accurate. I looked at the equations for
    ### this and the issue is that it is strongly dependent on depth. Since the
    ### soil model now has a single layer, this cannot be implemented correctly at
    ### the moment . */
    SoilTemp = AirTemp
    # /* Let us use an idea of Campbell and Norman. Environmental Biophysics. */ /*
    # If relative available water content is */
    rawc = (awc - WiltP)/(FieldC - WiltP)
    # /* Page 142 */ /* Maximum Dimensionless Uptake Rate */
    Up = 1 - (1 + 1.3 * rawc)^-5
    # /* This is a useful idea because dry soils evaporate little water when dry*/
    # /* Total Radiation */ /*' Convert light assuming 1 µmol PAR photons = 0.235
    # J/s Watts*/
    IRad = IRad * 0.2
    TotalRadiation = IRad * 0.235
    DdryA = TempToDdryA(AirTemp)
    LHV = TempToLHV(AirTemp) * 1e+06
    # /* Here LHV is given in MJ kg-1 and this needs to be converted to Joules kg-1
    # */
    SlopeFS = TempToSFS(AirTemp) * 0.001
    SWVC = TempToSWVC(AirTemp) * 0.001
    PsycParam = (DdryA * SpecificHeat)/LHV
    DeltaPVa = SWVC * (1 - RelH/100)
    BoundaryLayerThickness = 0.004 * sqrt(SoilClodSize/winds)
    DiffCoef = 2.126e-05 * 1.48e-07 * SoilTemp
    SoilBoundaryLayer = DiffCoef/BoundaryLayerThickness
    Ja = 2 * TotalRadiation * ((1 - SoilReflectance - SoilTransmission)/(1 - SoilTransmission))
    rlc = 4 * kappa * (273 + SoilTemp)^3 * 0.5
    PhiN = Ja - rlc  #/* Calculate the net radiation balance*/
    if (PhiN < 0) 
        PhiN = 1e-07
    # /* Priestly-Taylor */
    if (method == 0) {
        Evaporation = 1.26 * (SlopeFS * PhiN)/(LHV * (SlopeFS + PsycParam))
    } else {
        # /* Penman-Monteith */
        Evaporation = (SlopeFS * PhiN + LHV * PsycParam * SoilBoundaryLayer * DeltaPVa)/(LHV * 
            (SlopeFS + PsycParam))
    }
    ## /* Report back the soil evaporation rate in Units mmoles/m2/s */ /*
    ## Evaporation = Evaporation * 1000: ' Convert Kg H20/m2/s to g H20/m2/s */ /*
    ## Evaporation = Evaporation / 18: ' Convert g H20/m2/s to moles H20/m2/s */ /*
    ## Evaporation = Evaporation * 1000: ' Convert moles H20/m2/s to mmoles
    ## H20/m2/s */ /* If Evaporation <= 0 Then Evaporation = 0.00001: ' Prevent any
    ## odd looking values which might get through at very low light levels */
    Evaporation = Evaporation * (1e+06/18)
    # /* Adding the area dependence and the effect of drying */ /* Converting from
    # m2 to ha (times 1e4) */ /* Converting to hour */
    Evaporation = Evaporation * SoilArea * Up * cf2
    if (Evaporation < 0) 
        Evaporation = 1e-06
    return(Evaporation)
}
TempToDdryA <- function(Temp) {
    DdryA = 1.295163636 + -0.004258182 * Temp
    return(DdryA)
}
TempToLHV <- function(Temp) {
    LHV = 2.501 + -0.002372727 * Temp
    return(LHV)
}
TempToSFS <- function(Temp) {
    SlopeFS = 0.338376068 + 0.011435897 * Temp + 0.001111111 * Temp^2
    return(SlopeFS)
}
TempToSWVC <- function(Temp) {
    SWVC = 4.90820192 + 0.06387253 * Temp + 0.02745742 * Temp^2
    return(SWVC)
} 
