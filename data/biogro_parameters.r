biogro_parameters = utils::read.delim(textConnection("biogro symbol	description	how it is set	default value	units	has_derivative
iRhizome	initial dry mass of rhizomes	argument	7	Mg / ha	1
iLeaf	initial dry mass of leaves	argument	0.0007	Mg / ha	1
iStem	initial dry mass of stems	argument	0.007	Mg / ha	1
iRoot	initial dry mass of roots	argument	0.007	Mg / ha	1
Sp	specific leaf area	canopyControl	1.7		1
SpD		canopyControl	0		1
nlayers	the number of layers in the canopy model	canopyControl	10		0
kd		canopyControl	0.1		1
chi.l		canopyControl	1		1
mResp		canopyControl	0.02, 0.03		0
heightFactor		canopyControl	3		1
leafwidth		canopyControl	0.04		1
eteq		canopyControl	0		0
senLeaf		seneControl	3000		1
senStem		seneControl	3500		1
senRoot		seneControl	4000		1
senRhizome		seneControl	4000		1
vmax		photoControl	39		1
alpha		photoControl	0.04		1
kparm		photoControl	0.7		1
theta		photoControl	0.83		1
beta		photoControl	0.93		1
Rd		photoControl	0.8		1
Catm		photoControl	400		1
b0		photoControl	0.08		1
b1		photoControl	3		1
ws		photoControl	1		0
uppertemp		photoControl	37.5		1
lowertemp		photoControl	3		1
FieldC		soilControl	-1		1
WiltP		soilControl	-1		1
phi1		soilControl	0.01		1
phi2		soilControl	10		1
soil_depth		soilControl	1		1
iWatCont		soilControl	0.32		1
soilType		soilControl	6		0
soilLayers		soilControl	1		0
soil_depths		soilControl	0, 1		0
wsFun		soilControl	0		0
scsf		soilControl	1		1
transpRes		soilControl	5000000		1
leafPotTh		soilControl	-800		1
hydrDist		soilControl	0		1
rfl		soilControl	0.2		1
rsec		soilControl	0.2		1
rsdf		soilControl	0.44		1
iLeafN		nitroControl	2		1
kLN		nitroControl	0.5		1
Vmax.b1		nitroControl	0		1
alpha.b1		nitroControl	0		1
kpLN		nitroControl	0.2		1
lnb0		nitroControl	-5		1
lnb1		nitroControl	18		1
lnFun		nitroControl	0		0
SC1		centuryControl	1		1
SC2		centuryControl	1		1
SC3		centuryControl	1		1
SC4		centuryControl	1		1
SC5		centuryControl	1		1
SC6		centuryControl	1		1
SC7		centuryControl	1		1
SC8		centuryControl	1		1
SC9		centuryControl	1		1
LeafL.Ln		centuryControl	0.17		1
StemL.Ln		centuryControl	0.17		1
RootL.Ln		centuryControl	0.17		1
RhizL.Ln		centuryControl	0.17		1
LeafL.N		centuryControl	0.004		1
StemL.N		centuryControl	0.004		1
RootL.N		centuryControl	0.004		1
RhizL.N		centuryControl	0.004		1
Nfert		centuryControl	0, 0		0
iMinN		centuryControl	0		1
Litter		centuryControl	0, 0, 0, 0		0
timestep		centuryControl	day		0
Ks		centuryControl	3.9, 4.9, 7.3, 6, 14.8, 18.5, 0.2, 0.0045		0
tp1		phenoControl	562		1
tp2		phenoControl	1312		1
tp3		phenoControl	2063		1
tp4		phenoControl	2676		1
tp5		phenoControl	3211		1
tp6		phenoControl	7000		1
kStem1		phenoControl	0.37		1
kLeaf1		phenoControl	0.33		1
kRoot1		phenoControl	0.3		1
kRhizome1		phenoControl	-0.0008		1
kStem2		phenoControl	0.85		1
kLeaf2		phenoControl	0.14		1
kRoot2		phenoControl	0.01		1
kRhizome2		phenoControl	-0.0005		1
kStem3		phenoControl	0.63		1
kLeaf3		phenoControl	0.01		1
kRoot3		phenoControl	0.01		1
kRhizome3		phenoControl	0.35		1
kStem4		phenoControl	0.63		1
kLeaf4		phenoControl	0.01		1
kRoot4		phenoControl	0.01		1
kRhizome4		phenoControl	0.35		1
kStem5		phenoControl	0.63		1
kLeaf5		phenoControl	0.01		1
kRoot5		phenoControl	0.01		1
kRhizome5		phenoControl	0.35		1
kStem6		phenoControl	0.63		1
kLeaf6		phenoControl	0.01		1
kRoot6		phenoControl	0.01		1
kRhizome6		phenoControl	0.35		1
kGrain6		phenoControl	0		1"))
