#ifndef DAILYWILLOW_H
#define DAILYWILLOW_H

void createNULLwillow(struct c3tree *willow,int vecsize);

void UpdateWillowAfterEmergence(struct c3tree *willow,struct management *management);

void dailywillow(struct c3tree *willow,double coefs[25],double TherPrds[6], double TherTime, double Temp,double dailynetassim,
struct senthermaltemp *senparms, struct canopyparms *canopyparms, struct frostParms *frostparms, int N, double delTT,
struct respirationParms *RESP, int emergence);

#endif
