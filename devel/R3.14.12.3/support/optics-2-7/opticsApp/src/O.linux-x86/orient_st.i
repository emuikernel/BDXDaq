# 1 "../orient_st.st"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "../orient_st.st"


program orient ("P=xxx:orient1:,PM=xxx:,mTTH=m9,mTH=m10,mCHI=m11,mPHI=m12")
# 63 "../orient_st.st"
option +r;
# 90 "../orient_st.st"
char tmpstr[100];
char *cp;
# 107 "../orient_st.st"
short orientDebug; assign orientDebug to "{P}Debug"; monitor orientDebug;
string Msg; assign Msg to "{P}Msg";
short Alert; assign Alert to "{P}Alert";
short opAck; assign opAck to "{P}OperAck"; monitor opAck;


short Mode; assign Mode to "{P}Mode"; monitor Mode; evflag Mode_mon; sync Mode Mode_mon;
# 124 "../orient_st.st"
short motPut; assign motPut to "{P}motPut"; monitor motPut; evflag motPut_mon; sync motPut motPut_mon;
short motPut_Auto; assign motPut_Auto to "{P}motPut_Auto"; monitor motPut_Auto;
short load_motPut; assign load_motPut to "{P}load_motPut";







short motGet; assign motGet to "{P}motGet"; monitor motGet; evflag motGet_mon; sync motGet motGet_mon;
short motGet_Auto; assign motGet_Auto to "{P}motGet_Auto"; monitor motGet_Auto;


string motTTH_name; assign motTTH_name to "{P}motTTH_name"; monitor motTTH_name;
string motTH_name; assign motTH_name to "{P}motTH_name"; monitor motTH_name;
string motCHI_name; assign motCHI_name to "{P}motCHI_name"; monitor motCHI_name;
string motPHI_name; assign motPHI_name to "{P}motPHI_name"; monitor motPHI_name;

double motTTH; assign motTTH to "{PM}{mTTH}"; monitor motTTH; evflag motTTH_mon; sync motTTH motTTH_mon;
double motTH; assign motTH to "{PM}{mTH}"; monitor motTH; evflag motTH_mon; sync motTH motTH_mon;
double motCHI; assign motCHI to "{PM}{mCHI}"; monitor motCHI; evflag motCHI_mon; sync motCHI motCHI_mon;
double motPHI; assign motPHI to "{PM}{mPHI}"; monitor motPHI; evflag motPHI_mon; sync motPHI motPHI_mon;

double motTTH_RBV; assign motTTH_RBV to ""; monitor motTTH_RBV; evflag motRBV_mon; sync motTTH_RBV motRBV_mon;
double motTH_RBV; assign motTH_RBV to ""; monitor motTH_RBV; sync motTH_RBV motRBV_mon;
double motCHI_RBV; assign motCHI_RBV to ""; monitor motCHI_RBV; sync motCHI_RBV motRBV_mon;
double motPHI_RBV; assign motPHI_RBV to ""; monitor motPHI_RBV; sync motPHI_RBV motRBV_mon;
double rbv_angles[4];

int motTTH_stop; assign motTTH_stop to "";
int motTH_stop; assign motTH_stop to "";
int motCHI_stop; assign motCHI_stop to "";
int motPHI_stop; assign motPHI_stop to "";
int stopMotors; assign stopMotors to "{P}stopMotors"; monitor stopMotors;

int motTTH_Connected; assign motTTH_Connected to "{P}motTTH_Connected";
int motTH_Connected; assign motTH_Connected to "{P}motTH_Connected";
int motCHI_Connected; assign motCHI_Connected to "{P}motCHI_Connected";
int motPHI_Connected; assign motPHI_Connected to "{P}motPHI_Connected";
int motorsConnected;
int motorStopsConnected;


short OMTX_Calc; assign OMTX_Calc to "{P}OMTX_Calc"; monitor OMTX_Calc;




short Busy; assign Busy to "{P}Busy"; monitor Busy;


double H; assign H to "{P}H"; monitor H;
double H_event; assign H_event to "{P}H_event"; monitor H_event; evflag H_mon; sync H_event H_mon;
short H_busy; assign H_busy to "{P}H_busy"; monitor H_busy;
double loadH; assign loadH to "{P}loadH";
double H_RBV; assign H_RBV to "{P}H_RBV";

double K; assign K to "{P}K"; monitor K;
double K_event; assign K_event to "{P}K_event"; monitor K_event; evflag K_mon; sync K_event K_mon;
short K_busy; assign K_busy to "{P}K_busy"; monitor K_busy;
double loadK; assign loadK to "{P}loadK";
double K_RBV; assign K_RBV to "{P}K_RBV";

double L; assign L to "{P}L"; monitor L;
double L_event; assign L_event to "{P}L_event"; monitor L_event; evflag L_mon; sync L_event L_mon;
short L_busy; assign L_busy to "{P}L_busy"; monitor L_busy;
double loadL; assign loadL to "{P}loadL";
double L_RBV; assign L_RBV to "{P}L_RBV";

double hkl[3];
double rbv_hkl[3];


double TTH; assign TTH to "{P}TTH"; monitor TTH;
double TTH_event; assign TTH_event to "{P}TTH_event"; monitor TTH_event; evflag TTH_mon; sync TTH_event TTH_mon;
short TTH_busy; assign TTH_busy to "{P}TTH_busy"; monitor TTH_busy;
double loadTTH; assign loadTTH to "{P}loadTTH";

double TH; assign TH to "{P}TH"; monitor TH;
double TH_event; assign TH_event to "{P}TH_event"; monitor TH_event; evflag TH_mon; sync TH_event TH_mon;
short TH_busy; assign TH_busy to "{P}TH_busy"; monitor TH_busy;
double loadTH; assign loadTH to "{P}loadTH";

double CHI; assign CHI to "{P}CHI"; monitor CHI;
double CHI_event; assign CHI_event to "{P}CHI_event"; monitor CHI_event; evflag CHI_mon; sync CHI_event CHI_mon;
short CHI_busy; assign CHI_busy to "{P}CHI_busy"; monitor CHI_busy;
double loadCHI; assign loadCHI to "{P}loadCHI";

double PHI; assign PHI to "{P}PHI"; monitor PHI;
double PHI_event; assign PHI_event to "{P}PHI_event"; monitor PHI_event; evflag PHI_mon; sync PHI_event PHI_mon;
short PHI_busy; assign PHI_busy to "{P}PHI_busy"; monitor PHI_busy;
double loadPHI; assign loadPHI to "{P}loadPHI";

double angles[4];


double H1; assign H1 to "{P}H1"; monitor H1;
double K1; assign K1 to "{P}K1"; monitor K1;
double L1; assign L1 to "{P}L1"; monitor L1;
double TTH1; assign TTH1 to "{P}TTH1"; monitor TTH1;
double TH1; assign TH1 to "{P}TH1"; monitor TH1;
double CHI1; assign CHI1 to "{P}CHI1"; monitor CHI1;
double PHI1; assign PHI1 to "{P}PHI1"; monitor PHI1;
short refGet1; assign refGet1 to "{P}refGet1"; monitor refGet1;
short refPut1; assign refPut1 to "{P}refPut1"; monitor refPut1;

double H2; assign H2 to "{P}H2"; monitor H2;
double K2; assign K2 to "{P}K2"; monitor K2;
double L2; assign L2 to "{P}L2"; monitor L2;
double TTH2; assign TTH2 to "{P}TTH2"; monitor TTH2;
double TH2; assign TH2 to "{P}TH2"; monitor TH2;
double CHI2; assign CHI2 to "{P}CHI2"; monitor CHI2;
double PHI2; assign PHI2 to "{P}PHI2"; monitor PHI2;
short refGet2; assign refGet2 to "{P}refGet2"; monitor refGet2;
short refPut2; assign refPut2 to "{P}refPut2"; monitor refPut2;


double a; assign a to "{P}a"; monitor a; evflag xtalParm_mon; sync a xtalParm_mon;
double b; assign b to "{P}b"; monitor b; sync b xtalParm_mon;
double c; assign c to "{P}c"; monitor c; sync c xtalParm_mon;
double alpha; assign alpha to "{P}alpha"; monitor alpha; sync alpha xtalParm_mon;
double beta; assign beta to "{P}beta"; monitor beta; sync beta xtalParm_mon;
double Gamma; assign Gamma to "{P}gamma"; monitor Gamma; sync Gamma xtalParm_mon;


double lambda; assign lambda to "{P}lambda"; monitor lambda;
double lambda_event; assign lambda_event to "{P}lambda_event"; monitor lambda_event; evflag lambda_mon; sync lambda_event lambda_mon;
double load_lambda; assign load_lambda to "{P}load_lambda";

double energy; assign energy to "{P}energy"; monitor energy;
double energy_event; assign energy_event to "{P}energy_event"; monitor energy_event; evflag energy_mon; sync energy_event energy_mon;
double load_energy; assign load_energy to "{P}load_energy";

double hc;


double A0[3][3];
double A0_i[3][3];

double A0_11; assign A0_11 to "{P}A0_11"; monitor A0_11; evflag A0_mon; sync A0_11 A0_mon;
double A0_12; assign A0_12 to "{P}A0_12"; monitor A0_12; sync A0_12 A0_mon;
double A0_13; assign A0_13 to "{P}A0_13"; monitor A0_13; sync A0_13 A0_mon;

double A0_21; assign A0_21 to "{P}A0_21"; monitor A0_21; sync A0_21 A0_mon;
double A0_22; assign A0_22 to "{P}A0_22"; monitor A0_22; sync A0_22 A0_mon;
double A0_23; assign A0_23 to "{P}A0_23"; monitor A0_23; sync A0_23 A0_mon;

double A0_31; assign A0_31 to "{P}A0_31"; monitor A0_31; sync A0_31 A0_mon;
double A0_32; assign A0_32 to "{P}A0_32"; monitor A0_32; sync A0_32 A0_mon;
double A0_33; assign A0_33 to "{P}A0_33"; monitor A0_33; sync A0_33 A0_mon;


double OMTX[3][3];
double OMTX_i[3][3];

double OMTX_11; assign OMTX_11 to "{P}OMTX_11"; monitor OMTX_11; evflag OMTX_mon; sync OMTX_11 OMTX_mon;
double OMTX_12; assign OMTX_12 to "{P}OMTX_12"; monitor OMTX_12; sync OMTX_12 OMTX_mon;
double OMTX_13; assign OMTX_13 to "{P}OMTX_13"; monitor OMTX_13; sync OMTX_13 OMTX_mon;

double OMTX_21; assign OMTX_21 to "{P}OMTX_21"; monitor OMTX_21; sync OMTX_21 OMTX_mon;
double OMTX_22; assign OMTX_22 to "{P}OMTX_22"; monitor OMTX_22; sync OMTX_22 OMTX_mon;
double OMTX_23; assign OMTX_23 to "{P}OMTX_23"; monitor OMTX_23; sync OMTX_23 OMTX_mon;

double OMTX_31; assign OMTX_31 to "{P}OMTX_31"; monitor OMTX_31; sync OMTX_31 OMTX_mon;
double OMTX_32; assign OMTX_32 to "{P}OMTX_32"; monitor OMTX_32; sync OMTX_32 OMTX_mon;
double OMTX_33; assign OMTX_33 to "{P}OMTX_33"; monitor OMTX_33; sync OMTX_33 OMTX_mon;


double errAngle; assign errAngle to "{P}errAngle";
double errAngleThresh; assign errAngleThresh to "{P}errAngleThresh"; monitor errAngleThresh;

%%#include <string.h>
%%#include <math.h>
%%#include "matrix3.h"
%%#include "orient.h"


double v1_hkl[3];
double v1_angles[4];
double v2_hkl[3];
double v2_angles[4];
int i;


short newHKL; assign newHKL to "{P}newHKL";
short newAngles; assign newAngles to "{P}newAngles";
short newMotors; assign newMotors to "{P}newMotors";
short waiting4Mot; assign waiting4Mot to "{P}waiting4Mot";
short waiting4motTTH; assign waiting4motTTH to "{P}waiting4motTTH"; monitor waiting4motTTH;
short waiting4motTH; assign waiting4motTH to "{P}waiting4motTH"; monitor waiting4motTH;
short waiting4motCHI; assign waiting4motCHI to "{P}waiting4motCHI"; monitor waiting4motCHI;
short waiting4motPHI; assign waiting4motPHI to "{P}waiting4motPHI"; monitor waiting4motPHI;
int old_connect_state;

long A0_state; assign A0_state to "{P}A0_state"; monitor A0_state;
long OMTX_state; assign OMTX_state to "{P}OMTX_state"; monitor OMTX_state;

int weWrote_motTTH; assign weWrote_motTTH to "{P}weWrote_motTTH"; monitor weWrote_motTTH;
int weWrote_motTH; assign weWrote_motTH to "{P}weWrote_motTH"; monitor weWrote_motTH;
int weWrote_motCHI; assign weWrote_motCHI to "{P}weWrote_motCHI"; monitor weWrote_motCHI;
int weWrote_motPHI; assign weWrote_motPHI to "{P}weWrote_motPHI"; monitor weWrote_motPHI;






int startup;


char *orient_pmac_PM;
char *orient_pmac_mTTH;
char *orient_pmac_mTH;
char *orient_pmac_mCHI;
char *orient_pmac_mPHI;
# 351 "../orient_st.st"
ss orient {






 state init_wait {
  when (delay(1)) {
   if (A0_state == 2) {(A0_state) = (0); pvPut(A0_state);};
   if (OMTX_state == 2) {(OMTX_state) = (0); pvPut(OMTX_state);};
  } state init
 }







 state init {
  when (startup != 12345) {
   hc = 12.3984244;
   motorsConnected = 0;
   motorStopsConnected = 0;

   %%pVar->orient_pmac_PM = seq_macValueGet(ssId, "PM");
   %%pVar->orient_pmac_mTTH = seq_macValueGet(ssId, "mTTH");
   %%pVar->orient_pmac_mTH = seq_macValueGet(ssId, "mTH");
   %%pVar->orient_pmac_mCHI = seq_macValueGet(ssId, "mCHI");
   %%pVar->orient_pmac_mPHI = seq_macValueGet(ssId, "mPHI");
   if (orient_pmac_PM == 0) {
    {strcpy(Msg, ("motor prefix (PM) not specified")); pvPut(Msg); {(Alert) = (1); pvPut(Alert);}; printf("orient: %s\n", Msg);};
   } else {
    %%strcpy(pVar->motTTH_name, pVar->orient_pmac_PM);
    %%strcpy(pVar->motTH_name, pVar->orient_pmac_PM);
    %%strcpy(pVar->motCHI_name, pVar->orient_pmac_PM);
    %%strcpy(pVar->motPHI_name, pVar->orient_pmac_PM);
    if (orient_pmac_mTTH == 0) {
     {strcpy(Msg, ("orient: TTH motor name (mTTH) not specified")); pvPut(Msg); {(Alert) = (1); pvPut(Alert);}; printf("orient: %s\n", Msg);};
    } else {
     %%strcat(pVar->motTTH_name, pVar->orient_pmac_mTTH);
     pvPut(motTTH_name);

     %%strcpy(pVar->tmpstr, pVar->motTTH_name);
     %%if ((pVar->cp = strrchr(pVar->tmpstr, (int)'.'))) pVar->cp[0] = '\0';
     %%strcat(pVar->tmpstr, ".STOP");
     pvAssign(motTTH_stop, tmpstr);

     %%strcpy(pVar->tmpstr, pVar->motTTH_name);
     %%if ((pVar->cp = strrchr(pVar->tmpstr, (int)'.'))) pVar->cp[0] = '\0';
     %%strcat(pVar->tmpstr, ".RBV");
     pvAssign(motTTH_RBV, tmpstr);
    }
    if (orient_pmac_mTH == 0) {
     {strcpy(Msg, ("orient: TH motor name (mTH) not specified")); pvPut(Msg); {(Alert) = (1); pvPut(Alert);}; printf("orient: %s\n", Msg);};
    } else {
     %%strcat(pVar->motTH_name, pVar->orient_pmac_mTH);
     pvPut(motTH_name);
     %%strcpy(pVar->tmpstr, pVar->motTH_name);
     %%if ((pVar->cp = strrchr(pVar->tmpstr, (int)'.'))) pVar->cp[0] = '\0';
     %%strcat(pVar->tmpstr, ".STOP");
     pvAssign(motTH_stop, tmpstr);

     %%strcpy(pVar->tmpstr, pVar->motTH_name);
     %%if ((pVar->cp = strrchr(pVar->tmpstr, (int)'.'))) pVar->cp[0] = '\0';
     %%strcat(pVar->tmpstr, ".RBV");
     pvAssign(motTH_RBV, tmpstr);
    }
    if (orient_pmac_mCHI == 0) {
     {strcpy(Msg, ("orient: CHI motor name (mCHI) not specified")); pvPut(Msg); {(Alert) = (1); pvPut(Alert);}; printf("orient: %s\n", Msg);};
    } else {
     %%strcat(pVar->motCHI_name, pVar->orient_pmac_mCHI);
     pvPut(motCHI_name);
     %%strcpy(pVar->tmpstr, pVar->motCHI_name);
     %%if ((pVar->cp = strrchr(pVar->tmpstr, (int)'.'))) pVar->cp[0] = '\0';
     %%strcat(pVar->tmpstr, ".STOP");
     pvAssign(motCHI_stop, tmpstr);

     %%strcpy(pVar->tmpstr, pVar->motCHI_name);
     %%if ((pVar->cp = strrchr(pVar->tmpstr, (int)'.'))) pVar->cp[0] = '\0';
     %%strcat(pVar->tmpstr, ".RBV");
     pvAssign(motCHI_RBV, tmpstr);
    }
    if (orient_pmac_mPHI == 0) {
     {strcpy(Msg, ("orient: PHI motor name (mPHI) not specified")); pvPut(Msg); {(Alert) = (1); pvPut(Alert);}; printf("orient: %s\n", Msg);};
    } else {
     %%strcat(pVar->motPHI_name, pVar->orient_pmac_mPHI);
     pvPut(motPHI_name);
     %%strcpy(pVar->tmpstr, pVar->motPHI_name);
     %%if ((pVar->cp = strrchr(pVar->tmpstr, (int)'.'))) pVar->cp[0] = '\0';
     %%strcat(pVar->tmpstr, ".STOP");
     pvAssign(motPHI_stop, tmpstr);

     %%strcpy(pVar->tmpstr, pVar->motPHI_name);
     %%if ((pVar->cp = strrchr(pVar->tmpstr, (int)'.'))) pVar->cp[0] = '\0';
     %%strcat(pVar->tmpstr, ".RBV");
     pvAssign(motPHI_RBV, tmpstr);
    }
   }
   startup = 12345;
  } state init


  when (efTest(lambda_mon)||efTest(energy_mon)) {
   if (efTest(lambda_mon)) {
    energy = hc / lambda;

    {(load_energy) = (energy); pvPut(load_energy);};
   } else if (efTest(energy_mon)) {
    lambda = hc / energy;

    {(load_lambda) = (lambda); pvPut(load_lambda);};
   }
   efClear(lambda_mon); efClear(energy_mon);
   {(A0_state) = (0); pvPut(A0_state);};
  } state init

  when (A0_state == 0) {





   if (lambda > 1.0e-9) {
    energy = hc/lambda;

    {(load_energy) = (energy); pvPut(load_energy);};
    if (orientDebug) {strcpy(Msg, ("calculated energy")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
   } else if (energy > 1.0e-9) {
    lambda = hc/energy;

    {(load_lambda) = (lambda); pvPut(load_lambda);};
   }
   if ((a!=0) && (b!=0) && (c!=0) && (alpha!=0) && (beta!=0) && (Gamma!=0) && (lambda!=0)) {
    efClear(xtalParm_mon); efClear(lambda_mon); efClear(energy_mon);
    if (orientDebug) {strcpy(Msg, ("calling calc_A0")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
    %%pVar->i = calc_A0(pVar->a, pVar->b, pVar->c, pVar->alpha, pVar->beta, pVar->Gamma,
    %% pVar->lambda, pVar->A0, pVar->A0_i);

    %%pVar->A0_11 = pVar->A0[0][0]; pVar->A0_12 = pVar->A0[0][1]; pVar->A0_13 = pVar->A0[0][2];
    %%pVar->A0_21 = pVar->A0[1][0]; pVar->A0_22 = pVar->A0[1][1]; pVar->A0_23 = pVar->A0[1][2];
    %%pVar->A0_31 = pVar->A0[2][0]; pVar->A0_32 = pVar->A0[2][1]; pVar->A0_33 = pVar->A0[2][2];
    pvPut(A0_11); pvPut(A0_12); pvPut(A0_13);
    pvPut(A0_21); pvPut(A0_22); pvPut(A0_23);
    pvPut(A0_31); pvPut(A0_32); pvPut(A0_33);
    {(A0_state) = ((i==0 ? 1 : 2)); pvPut(A0_state);};
    if (A0_state == 1) {strcpy(Msg, ("Successful A0 calc")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
   } else {
    {(A0_state) = (2); pvPut(A0_state);};
   }
  } state init

  when (A0_state == 2) {

  } state init_wait

  when ((A0_state == 1) && (OMTX_state == 0)) {
# 518 "../orient_st.st"
   if ((H1!=0 || K1!=0 || L1!=0) && (H2!=0 || K2!=0 || L2!=0)) {
    if (orientDebug) {strcpy(Msg, ("calling calc_OMTX")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
    %%pVar->v1_hkl[H_INDEX] = pVar->H1; pVar->v1_hkl[K_INDEX] = pVar->K1; pVar->v1_hkl[L_INDEX] = pVar->L1;
    %%pVar->v1_angles[TTH_INDEX] = pVar->TTH1; pVar->v1_angles[TH_INDEX] = pVar->TH1;
    %%pVar->v1_angles[CHI_INDEX] = pVar->CHI1; pVar->v1_angles[PHI_INDEX] = pVar->PHI1;
    %%pVar->v2_hkl[H_INDEX] = pVar->H2; pVar->v2_hkl[K_INDEX] = pVar->K2; pVar->v2_hkl[L_INDEX] = pVar->L2;
    %%pVar->v2_angles[TTH_INDEX] = pVar->TTH2; pVar->v2_angles[TH_INDEX] = pVar->TH2;
    %%pVar->v2_angles[CHI_INDEX] = pVar->CHI2; pVar->v2_angles[PHI_INDEX] = pVar->PHI2;
    %%pVar->i = calc_OMTX(pVar->v1_hkl, pVar->v1_angles, pVar->v2_hkl, pVar->v2_angles,
    %% pVar->A0, pVar->A0_i, pVar->OMTX, pVar->OMTX_i);
    %%pVar->OMTX_11 = pVar->OMTX[0][0]; pVar->OMTX_12 = pVar->OMTX[0][1]; pVar->OMTX_13 = pVar->OMTX[0][2];
    %%pVar->OMTX_21 = pVar->OMTX[1][0]; pVar->OMTX_22 = pVar->OMTX[1][1]; pVar->OMTX_23 = pVar->OMTX[1][2];
    %%pVar->OMTX_31 = pVar->OMTX[2][0]; pVar->OMTX_32 = pVar->OMTX[2][1]; pVar->OMTX_33 = pVar->OMTX[2][2];
    pvPut(OMTX_11); pvPut(OMTX_12); pvPut(OMTX_13);
    pvPut(OMTX_21); pvPut(OMTX_22); pvPut(OMTX_23);
    pvPut(OMTX_31); pvPut(OMTX_32); pvPut(OMTX_33);
    if (OMTX_Calc) {(OMTX_Calc) = (0); pvPut(OMTX_Calc);};
    {(errAngle) = (checkOMTX(v2_hkl, v2_angles, A0, A0_i, OMTX_i)); pvPut(errAngle);};
   } else {
    i = -1;
   }






   if ((i==0) && (fabs(errAngle) < errAngleThresh)) {

    {(OMTX_state) = (1); pvPut(OMTX_state);};
    {strcpy(Msg, ("Successful OMTX calc")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
   } else {
    {(OMTX_state) = (2); pvPut(OMTX_state);};
    if (motPut_Auto != 0) {
     {(motPut_Auto) = (0); pvPut(motPut_Auto);};
     {strcpy(Msg, ("Bad OMTX calc; motPut_Auto set to Manual")); pvPut(Msg); {(Alert) = (1); pvPut(Alert);}; printf("orient: %s\n", Msg);};
    }
   }
  } state init

  when (OMTX_state == 2) {

  } state init_wait

  when (A0_state == 1 && (OMTX_state == 1)) {

  } state enter_monitor_all

 }





 state ref_show {
# 581 "../orient_st.st"
  when (delay(.5)) {
   {(refGet1) = (0); pvPut(refGet1);}; {(refGet2) = (0); pvPut(refGet2);};
   {(refPut1) = (0); pvPut(refPut1);}; {(refPut2) = (0); pvPut(refPut2);};
  } state monitor_all
 }

 state enter_monitor_all {
  when (delay(10)) {

   efClear(A0_mon); efClear(OMTX_mon);
   efClear(H_mon); efClear(K_mon); efClear(L_mon);
   efClear(TTH_mon); efClear(TH_mon); efClear(CHI_mon); efClear(PHI_mon);
   efClear(Mode_mon);
   {strcpy(Msg, ("Initialization complete; going live")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
  } state monitor_all
 }

 state monitor_all_delay {

  when (delay(1)) {
  } state monitor_all
 }


 state monitor_all {





  when (motorsConnected == 0) {
   {strcpy(Msg, ("Waiting for motor connection(s)")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
  } state monitor_all_delay

  when (motorStopsConnected == 0) {
   {strcpy(Msg, ("Waiting for motor-stop connection(s)")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
  } state monitor_all_delay


  when ((A0_state == 1) && OMTX_Calc) {

   if (orientDebug) {strcpy(Msg, ("Recalculating OMTX")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
   %%pVar->v1_hkl[H_INDEX] = pVar->H1; pVar->v1_hkl[K_INDEX] = pVar->K1; pVar->v1_hkl[L_INDEX] = pVar->L1;
   %%pVar->v1_angles[TTH_INDEX] = pVar->TTH1; pVar->v1_angles[TH_INDEX] = pVar->TH1;
   %%pVar->v1_angles[CHI_INDEX] = pVar->CHI1; pVar->v1_angles[PHI_INDEX] = pVar->PHI1;
   %%pVar->v2_hkl[H_INDEX] = pVar->H2; pVar->v2_hkl[K_INDEX] = pVar->K2; pVar->v2_hkl[L_INDEX] = pVar->L2;
   %%pVar->v2_angles[TTH_INDEX] = pVar->TTH2; pVar->v2_angles[TH_INDEX] = pVar->TH2;
   %%pVar->v2_angles[CHI_INDEX] = pVar->CHI2; pVar->v2_angles[PHI_INDEX] = pVar->PHI2;
   %%pVar->i = calc_OMTX(pVar->v1_hkl, pVar->v1_angles, pVar->v2_hkl, pVar->v2_angles,
   %% pVar->A0, pVar->A0_i, pVar->OMTX, pVar->OMTX_i);
   %%pVar->OMTX_11 = pVar->OMTX[0][0]; pVar->OMTX_12 = pVar->OMTX[0][1]; pVar->OMTX_13 = pVar->OMTX[0][2];
   %%pVar->OMTX_21 = pVar->OMTX[1][0]; pVar->OMTX_22 = pVar->OMTX[1][1]; pVar->OMTX_23 = pVar->OMTX[1][2];
   %%pVar->OMTX_31 = pVar->OMTX[2][0]; pVar->OMTX_32 = pVar->OMTX[2][1]; pVar->OMTX_33 = pVar->OMTX[2][2];
   pvPut(OMTX_11); pvPut(OMTX_12); pvPut(OMTX_13);
   pvPut(OMTX_21); pvPut(OMTX_22); pvPut(OMTX_23);
   pvPut(OMTX_31); pvPut(OMTX_32); pvPut(OMTX_33);
   {(OMTX_Calc) = (0); pvPut(OMTX_Calc);};
   errAngle = checkOMTX(v2_hkl, v2_angles, A0, A0_i, OMTX_i);
   pvPut(errAngle);
   if ((i==0) && (fabs(errAngle) < errAngleThresh)) {

    {(OMTX_state) = (1); pvPut(OMTX_state);};
   } else {
    {(OMTX_state) = (2); pvPut(OMTX_state);};
    {(motPut_Auto) = (0); pvPut(motPut_Auto);};
    {strcpy(Msg, ("Bad OMTX calc; motPut_Auto set to Manual")); pvPut(Msg); {(Alert) = (1); pvPut(Alert);}; printf("orient: %s\n", Msg);};
   }
  } state newOMTX


  when (efTest(xtalParm_mon) || efTest(lambda_mon) || efTest(energy_mon)) {
   if (efTest(lambda_mon)) {
    energy = hc / lambda;

    {(load_energy) = (energy); pvPut(load_energy);};
   } else if (efTest(energy_mon)) {
    lambda = hc / energy;

    {(load_lambda) = (lambda); pvPut(load_lambda);};
   }
   efClear(xtalParm_mon); efClear(lambda_mon); efClear(energy_mon);
   if (orientDebug) {strcpy(Msg, ("new XTAL parameters")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
   %%pVar->i = calc_A0(pVar->a, pVar->b, pVar->c, pVar->alpha, pVar->beta,
   %% pVar->Gamma, pVar->lambda, pVar->A0, pVar->A0_i);
   %%pVar->A0_11 = pVar->A0[0][0]; pVar->A0_12 = pVar->A0[0][1]; pVar->A0_13 = pVar->A0[0][2];
   %%pVar->A0_21 = pVar->A0[1][0]; pVar->A0_22 = pVar->A0[1][1]; pVar->A0_23 = pVar->A0[1][2];
   %%pVar->A0_31 = pVar->A0[2][0]; pVar->A0_32 = pVar->A0[2][1]; pVar->A0_33 = pVar->A0[2][2];
   pvPut(A0_11); pvPut(A0_12); pvPut(A0_13);
   pvPut(A0_21); pvPut(A0_22); pvPut(A0_23);
   pvPut(A0_31); pvPut(A0_32); pvPut(A0_33);
   {(A0_state) = ((i==0 ? 1 : 2)); pvPut(A0_state);};
   if (A0_state == 1) {
    {strcpy(Msg, ("Successful A0 calc")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
   } else {
    {strcpy(Msg, ("A0 calc failed")); pvPut(Msg); {(Alert) = (1); pvPut(Alert);}; printf("orient: %s\n", Msg);};
   }
  } state newHKL


  when (efTest(A0_mon)) {
   efClear(A0_mon);
   if (orientDebug) {strcpy(Msg, ("new A0 precursor(s)")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};

   %%pVar->A0[0][0] = pVar->A0_11; pVar->A0[0][1] = pVar->A0_12; pVar->A0[0][2] = pVar->A0_13;
   %%pVar->A0[1][0] = pVar->A0_21; pVar->A0[1][1] = pVar->A0_22; pVar->A0[1][2] = pVar->A0_23;
   %%pVar->A0[2][0] = pVar->A0_31; pVar->A0[2][1] = pVar->A0_32; pVar->A0[2][2] = pVar->A0_33;
   i = invertArray(A0, A0_i);
   if (i == -1) {
    {strcpy(Msg, ("Could not invert A0 array")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
    {(A0_state) = (2); pvPut(A0_state);};
   }
  } state newA0


  when (efTest(OMTX_mon)) {
   efClear(OMTX_mon);
   if (orientDebug) {strcpy(Msg, ("new OMTX element(s)")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};

   %%pVar->OMTX[0][0] = pVar->OMTX_11; pVar->OMTX[0][1] = pVar->OMTX_12; pVar->OMTX[0][2] = pVar->OMTX_13;
   %%pVar->OMTX[1][0] = pVar->OMTX_21; pVar->OMTX[1][1] = pVar->OMTX_22; pVar->OMTX[1][2] = pVar->OMTX_23;
   %%pVar->OMTX[2][0] = pVar->OMTX_31; pVar->OMTX[2][1] = pVar->OMTX_32; pVar->OMTX[2][2] = pVar->OMTX_33;
   i = invertArray(OMTX, OMTX_i);
   if (i == -1) {
    {strcpy(Msg, ("Could not invert OMTX array")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
    {(OMTX_state) = (2); pvPut(OMTX_state);};
   } else if (A0_state != 1) {
    {strcpy(Msg, ("Can't check or use OMTX array (No A0)")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
    {(OMTX_state) = (2); pvPut(OMTX_state);};
   } else {
    errAngle = checkOMTX(v2_hkl, v2_angles, A0, A0_i, OMTX_i);
    pvPut(errAngle);
    if (fabs(errAngle) < errAngleThresh) {

     {(OMTX_state) = (1); pvPut(OMTX_state);};
     if (orientDebug) {strcpy(Msg, ("User's OMTX passes test")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
    } else {
     {(OMTX_state) = (2); pvPut(OMTX_state);};
     if (orientDebug) {strcpy(Msg, ("User's OMTX fails test")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
    }
   }
  } state newOMTX


  when (efTest(H_mon)) {
   if (orientDebug) {printf("\norient: client wrote H\n");}
   if (!Busy) {(Busy) = (1); pvPut(Busy);};
   efClear(H_mon);
  } state newHKL

  when (efTest(K_mon)) {
   if (orientDebug) {printf("\norient: client wrote K\n");}
   if (!Busy) {(Busy) = (1); pvPut(Busy);};
   efClear(K_mon);
  } state newHKL

  when (efTest(L_mon)) {
   if (orientDebug) {printf("\norient: client wrote L\n");}
   if (!Busy) {(Busy) = (1); pvPut(Busy);};
   efClear(L_mon);
  } state newHKL



  when (efTest(TTH_mon)) {
   if (orientDebug) {printf("\norient: client wrote TTH\n");}
   if (!Busy) {(Busy) = (1); pvPut(Busy);};
   efClear(TTH_mon);
  } state newAngles

  when (efTest(TH_mon)) {
   if (orientDebug) {printf("\norient: client wrote TH\n");}
   if (!Busy) {(Busy) = (1); pvPut(Busy);};
   efClear(TH_mon);
  } state newAngles

  when (efTest(CHI_mon)) {
   if (orientDebug) {printf("\norient: client wrote CHI\n");}
   if (!Busy) {(Busy) = (1); pvPut(Busy);};
   efClear(CHI_mon);
  } state newAngles

  when (efTest(PHI_mon)) {
   if (orientDebug) {printf("\norient: client wrote PHI\n");}
   if (!Busy) {(Busy) = (1); pvPut(Busy);};
   efClear(PHI_mon);
  } state newAngles
# 795 "../orient_st.st"
  when (efTest(motTTH_mon) && waiting4motTTH && pvPutComplete(motTTH)) {
   if (orientDebug) {printf("orient: motTTH done moving\n");}
   efClear(motTTH_mon); {(waiting4motTTH) = (0); pvPut(waiting4motTTH);};






   if (weWrote_motTTH) {(weWrote_motTTH) = (0); pvPut(weWrote_motTTH);};
  } state checkDone
  when (efTest(motTTH_mon) && weWrote_motTTH) {
   if (orientDebug) {printf("orient: We wrote motTTH\n");}
   efClear(motTTH_mon); {(weWrote_motTTH) = (0); pvPut(weWrote_motTTH);};
  } state monitor_all
  when (efTest(motTTH_mon) && !weWrote_motTTH) {
   if (orientDebug) {printf("orient: client wrote motTTH\n");}
   efClear(motTTH_mon);
  } state newMotors

  when (efTest(motTH_mon) && waiting4motTH && pvPutComplete(motTH)) {
   if (orientDebug) {printf("orient: motTH done moving\n");}
   efClear(motTH_mon); {(waiting4motTH) = (0); pvPut(waiting4motTH);};
   if (weWrote_motTH) {(weWrote_motTH) = (0); pvPut(weWrote_motTH);};
  } state checkDone
  when (efTest(motTH_mon) && weWrote_motTH) {
   if (orientDebug) {printf("orient: We wrote motTH\n");}
   efClear(motTH_mon); {(weWrote_motTH) = (0); pvPut(weWrote_motTH);};
  } state monitor_all
  when (efTest(motTH_mon) && !weWrote_motTH) {
   if (orientDebug) {printf("orient: client wrote motTH\n");}
   efClear(motTH_mon);
  } state newMotors

  when (efTest(motCHI_mon) && waiting4motCHI && pvPutComplete(motCHI)) {
   if (orientDebug) {printf("orient: motCHI done moving\n");}
   efClear(motCHI_mon); {(waiting4motCHI) = (0); pvPut(waiting4motCHI);};
   if (weWrote_motCHI) {(weWrote_motCHI) = (0); pvPut(weWrote_motCHI);};
  } state checkDone
  when (efTest(motCHI_mon) && weWrote_motCHI) {
   if (orientDebug) {printf("orient: We wrote motCHI\n");}
   efClear(motCHI_mon); {(weWrote_motCHI) = (0); pvPut(weWrote_motCHI);};
  } state monitor_all
  when (efTest(motCHI_mon) && !weWrote_motCHI) {
   if (orientDebug) {printf("orient: client wrote motCHI\n");}
   efClear(motCHI_mon);
  } state newMotors

  when (efTest(motPHI_mon) && waiting4motPHI && pvPutComplete(motPHI)) {
   if (orientDebug) {printf("orient: motPHI done moving\n");}
   efClear(motPHI_mon); {(waiting4motPHI) = (0); pvPut(waiting4motPHI);};
   if (weWrote_motPHI) {(weWrote_motPHI) = (0); pvPut(weWrote_motPHI);};
  } state checkDone
  when (efTest(motPHI_mon) && weWrote_motPHI) {
   if (orientDebug) {printf("orient: We wrote motPHI\n");}
   efClear(motPHI_mon); {(weWrote_motPHI) = (0); pvPut(weWrote_motPHI);};
  } state monitor_all
  when (efTest(motPHI_mon) && !weWrote_motPHI) {
   if (orientDebug) {printf("orient: client wrote motPHI\n");}
   efClear(motPHI_mon);
  } state newMotors


  when (Busy && waiting4Mot && (waiting4motTTH==0) && (waiting4motTH==0) &&
    (waiting4motCHI==0) && (waiting4motPHI==0)) {
   {(waiting4Mot) = (0); pvPut(waiting4Mot);};
   {(Busy) = (0); pvPut(Busy);};
   if (H_busy) {(H_busy) = (0); pvPut(H_busy);};
   if (K_busy) {(K_busy) = (0); pvPut(K_busy);};
   if (L_busy) {(L_busy) = (0); pvPut(L_busy);};
   if (TTH_busy) {(TTH_busy) = (0); pvPut(TTH_busy);};
   if (TH_busy) {(TH_busy) = (0); pvPut(TH_busy);};
   if (CHI_busy) {(CHI_busy) = (0); pvPut(CHI_busy);};
   if (PHI_busy) {(PHI_busy) = (0); pvPut(PHI_busy);};
   if (orientDebug) {printf("motors done\n");}
  } state monitor_all


  when (efTest(Mode_mon)) {
   efClear(Mode_mon);
   if (orientDebug) {
    {strcpy(Msg, ("new HKL-to-Angles Mode")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
    printf("...Mode=%d\n", Mode);
   }
  } state newHKL


  when (motPut) {
   if (orientDebug) {printf("orient: motPut==%d\n", motPut);}
   {(weWrote_motTTH) = (1); pvPut(weWrote_motTTH);}; motTTH = TTH; {(waiting4motTTH) = (1); pvPut(waiting4motTTH);}; pvPut(motTTH, ASYNC);
   {(weWrote_motTH) = (1); pvPut(weWrote_motTH);}; motTH = TH; {(waiting4motTH) = (1); pvPut(waiting4motTH);}; pvPut(motTH, ASYNC);
   {(weWrote_motCHI) = (1); pvPut(weWrote_motCHI);}; motCHI = CHI; {(waiting4motCHI) = (1); pvPut(waiting4motCHI);}; pvPut(motCHI, ASYNC);
   {(weWrote_motPHI) = (1); pvPut(weWrote_motPHI);}; motPHI = PHI; {(waiting4motPHI) = (1); pvPut(waiting4motPHI);}; pvPut(motPHI, ASYNC);

   motPut = 0; {(load_motPut) = (motPut); pvPut(load_motPut);};
   {(waiting4Mot) = (1); pvPut(waiting4Mot);};
  } state monitor_all





  when (motGet) {
  } state newMotors


  when (refGet1) {
   {(H1) = (H); pvPut(H1);}; {(K1) = (K); pvPut(K1);}; {(L1) = (L); pvPut(L1);};
   {(TTH1) = (TTH); pvPut(TTH1);}; {(TH1) = (TH); pvPut(TH1);}; {(CHI1) = (CHI); pvPut(CHI1);}; {(PHI1) = (PHI); pvPut(PHI1);};
  } state ref_show

  when (refGet2) {
   {(H2) = (H); pvPut(H2);}; {(K2) = (K); pvPut(K2);}; {(L2) = (L); pvPut(L2);};
   {(TTH2) = (TTH); pvPut(TTH2);}; {(TH2) = (TH); pvPut(TH2);}; {(CHI2) = (CHI); pvPut(CHI2);}; {(PHI2) = (PHI); pvPut(PHI2);};
  } state ref_show


  when (refPut1) {
   {(H) = (H1); pvPut(H);}; {(K) = (K1); pvPut(K);}; {(L) = (L1); pvPut(L);};
   {(TTH) = (TTH1); pvPut(TTH);}; {(TH) = (TH1); pvPut(TH);}; {(CHI) = (CHI1); pvPut(CHI);}; {(PHI) = (PHI1); pvPut(PHI);};
  } state ref_show

  when (refPut2) {
   {(H) = (H2); pvPut(H);}; {(K) = (K2); pvPut(K);}; {(L) = (L2); pvPut(L);};
   {(TTH) = (TTH2); pvPut(TTH);}; {(TH) = (TH2); pvPut(TH);}; {(CHI) = (CHI2); pvPut(CHI);}; {(PHI) = (PHI2); pvPut(PHI);};
  } state ref_show


  when (opAck) {
   {strcpy(Msg, ("")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
   {(opAck) = (0); pvPut(opAck);};
   {(Alert) = (0); pvPut(Alert);};
  } state monitor_all
 }
# 937 "../orient_st.st"
 state newA0 {
  when (A0_state != 1) {
  } state monitor_all
  when (A0_state == 1) {
  } state newHKL
 }





 state newOMTX {
  when (OMTX_state == 1) {
  } state newHKL
  when (OMTX_state != 1) {
   {(motPut_Auto) = (0); pvPut(motPut_Auto);};
   {strcpy(Msg, ("Bad OMTX calc; motPut_Auto set to Manual")); pvPut(Msg); {(Alert) = (1); pvPut(Alert);}; printf("orient: %s\n", Msg);};
  } state monitor_all
 }
# 964 "../orient_st.st"
 state newHKL {
  when ((A0_state != 1) || (OMTX_state != 1)) {
   if (A0_state != 1) {
    {strcpy(Msg, ("We don't have a good A0 matrix")); pvPut(Msg); {(Alert) = (1); pvPut(Alert);}; printf("orient: %s\n", Msg);};
   } else {
    {strcpy(Msg, ("We don't have a good OMTX matrix")); pvPut(Msg); {(Alert) = (1); pvPut(Alert);}; printf("orient: %s\n", Msg);};
   }
  } state monitor_all
  when (delay(0.02)) {
   if (efTest(H_mon)) {efClear(H_mon); if (orientDebug) printf("orient:newHKL: client also wrote H\n");}
   if (efTest(K_mon)) {efClear(K_mon); if (orientDebug) printf("orient:newHKL: client also wrote K\n");}
   if (efTest(L_mon)) {efClear(L_mon); if (orientDebug) printf("orient:newHKL: client also wrote L\n");}

   %%pVar->hkl[H_INDEX] = pVar->H; pVar->hkl[K_INDEX] = pVar->K; pVar->hkl[L_INDEX] = pVar->L;
   %%pVar->angles[TTH_INDEX] = pVar->TTH; pVar->angles[TH_INDEX] = pVar->TH;
   %%pVar->angles[CHI_INDEX] = pVar->CHI; pVar->angles[PHI_INDEX] = pVar->PHI;
   %%pVar->i = HKL_to_angles(pVar->hkl, pVar->A0, pVar->OMTX, pVar->angles, (int)pVar->Mode);
   if (i == 0) {
    %%pVar->TTH = pVar->angles[TTH_INDEX];
    %%pVar->TH = pVar->angles[TH_INDEX];
    %%pVar->CHI = pVar->angles[CHI_INDEX];
    %%pVar->PHI = pVar->angles[PHI_INDEX];
    {(newAngles) = (1); pvPut(newAngles);};
    if (motPut_Auto) {(newMotors) = (1); pvPut(newMotors);};
   } else if (i) {
    {strcpy(Msg, ("Bad HKL2angles calc.")); pvPut(Msg); {(Alert) = (1); pvPut(Alert);}; printf("orient: %s\n", Msg);};
   }
  } state putAll
 }






 state newAngles {
  when ((A0_state != 1) || (OMTX_state != 1)) {
   if (A0_state != 1) {
    {strcpy(Msg, ("We don't have a good A0 matrix")); pvPut(Msg); {(Alert) = (1); pvPut(Alert);}; printf("orient: %s\n", Msg);};
   } else {
    {strcpy(Msg, ("We don't have a good OMTX matrix")); pvPut(Msg); {(Alert) = (1); pvPut(Alert);}; printf("orient: %s\n", Msg);};
   }
  } state monitor_all
  when (delay(0.02)) {
   if (efTest(TTH_mon)) {efClear(TTH_mon); if (orientDebug) printf("orient:newAngles: client also wrote TTH\n");}
   if (efTest(TH_mon)) {efClear(TH_mon); if (orientDebug) printf("orient:newAngles: client also wrote TH\n");}
   if (efTest(CHI_mon)) {efClear(CHI_mon); if (orientDebug) printf("orient:newAngles: client also wrote CHI\n");}
   if (efTest(PHI_mon)) {efClear(PHI_mon); if (orientDebug) printf("orient:newAngles: client also wrote PHI\n");}

   %%pVar->angles[TTH_INDEX] = pVar->TTH; pVar->angles[TH_INDEX] = pVar->TH;
   %%pVar->angles[CHI_INDEX] = pVar->CHI; pVar->angles[PHI_INDEX] = pVar->PHI;
   %%angles_to_HKL(pVar->angles, pVar->OMTX_i, pVar->A0_i, pVar->hkl);
   %%pVar->H = pVar->hkl[H_INDEX]; pVar->K = pVar->hkl[K_INDEX]; pVar->L = pVar->hkl[L_INDEX];
   {(newHKL) = (1); pvPut(newHKL);};
   if (motPut_Auto) {
    {(newMotors) = (1); pvPut(newMotors);};
   }
  } state putAll
 }

 state checkDone {

  when (Busy && waiting4Mot && (waiting4motTTH==0) && (waiting4motTH==0) &&
    (waiting4motCHI==0) && (waiting4motPHI==0)) {
   {(waiting4Mot) = (0); pvPut(waiting4Mot);};
   {(Busy) = (0); pvPut(Busy);};
   if (H_busy) {(H_busy) = (0); pvPut(H_busy);};
   if (K_busy) {(K_busy) = (0); pvPut(K_busy);};
   if (L_busy) {(L_busy) = (0); pvPut(L_busy);};
   if (TTH_busy) {(TTH_busy) = (0); pvPut(TTH_busy);};
   if (TH_busy) {(TH_busy) = (0); pvPut(TH_busy);};
   if (CHI_busy) {(CHI_busy) = (0); pvPut(CHI_busy);};
   if (PHI_busy) {(PHI_busy) = (0); pvPut(PHI_busy);};
   if (orientDebug) {printf("motors done\n");}
  } state newMotors
  when () {
  } state monitor_all
 }


 state newMotors {
  when () {
   if (orientDebug) {printf("orient:newMotors: motGet=%d\n", motGet);}
   if (motGet || (motGet_Auto && !waiting4Mot)) {

    if (orientDebug) {printf("orient:newMotors: getting values\n");}
    TTH = motTTH; TH = motTH; CHI = motCHI; PHI = motPHI;
    {(newAngles) = (1); pvPut(newAngles);};

    %%pVar->angles[TTH_INDEX] = pVar->TTH; pVar->angles[TH_INDEX] = pVar->TH;
    %%pVar->angles[CHI_INDEX] = pVar->CHI; pVar->angles[PHI_INDEX] = pVar->PHI;
    %%angles_to_HKL(pVar->angles, pVar->OMTX_i, pVar->A0_i, pVar->hkl);
    %%pVar->H = pVar->hkl[H_INDEX]; pVar->K = pVar->hkl[K_INDEX]; pVar->L = pVar->hkl[L_INDEX];
    {(newHKL) = (1); pvPut(newHKL);};
    if (motGet) {(motGet) = (0); pvPut(motGet);};
   }
  } state putAll
 }
# 1072 "../orient_st.st"
 state putAll {
  when () {
   if (newHKL) {
    if (orientDebug) {printf("orient:putAll: writing HKL\n");}

    {(loadH) = (H); pvPut(loadH);};
    {(loadK) = (K); pvPut(loadK);};
    {(loadL) = (L); pvPut(loadL);};
    {(newHKL) = (0); pvPut(newHKL);};
   }
   if (newAngles) {
    if (orientDebug) {printf("orient:putAll: writing Angles\n");}

    {(loadTTH) = (TTH); pvPut(loadTTH);};
    {(loadTH) = (TH); pvPut(loadTH);};
    {(loadCHI) = (CHI); pvPut(loadCHI);};
    {(loadPHI) = (PHI); pvPut(loadPHI);};
    {(newAngles) = (0); pvPut(newAngles);};
   }
   if (newMotors) {
    if (orientDebug) {printf("orient:putAll: comparing new/old Motor values\n");}
    if (motTTH_Connected && motTH_Connected && motCHI_Connected && motPHI_Connected) {
     if (orientDebug) printf("putAll:newMotors: H=%8.4f, K=%8.4f, L=%8.4f\n", H, K, L);
     if ((fabs((motTTH)-(TTH)) > 1.0e-9)) {
      if (orientDebug) printf("putAll:newMotors: TTH=%8.4f, motTTH=%8.4f\n", TTH, motTTH);
      {(weWrote_motTTH) = (1); pvPut(weWrote_motTTH);}; {(waiting4motTTH) = (1); pvPut(waiting4motTTH);};
      motTTH = TTH; pvPut(motTTH, ASYNC);
     }
     if ((fabs((motTH)-(TH)) > 1.0e-9)) {
      if (orientDebug) printf("putAll:newMotors:  TH=%8.4f,  motTH=%8.4f\n", TH, motTH);
      {(weWrote_motTH) = (1); pvPut(weWrote_motTH);}; {(waiting4motTH) = (1); pvPut(waiting4motTH);};
      motTH = TH; pvPut(motTH, ASYNC);
     }
     if ((fabs((motCHI)-(CHI)) > 1.0e-9)) {
      if (orientDebug) printf("putAll:newMotors: CHI=%8.4f, motCHI=%8.4f\n", CHI, motCHI);
      {(weWrote_motCHI) = (1); pvPut(weWrote_motCHI);}; {(waiting4motCHI) = (1); pvPut(waiting4motCHI);};
      motCHI = CHI; pvPut(motCHI, ASYNC);
     }
     if ((fabs((motPHI)-(PHI)) > 1.0e-9)) {
      if (orientDebug) printf("putAll:newMotors: PHI=%8.4f, motPHI=%8.4f\n", PHI, motPHI);
      {(weWrote_motPHI) = (1); pvPut(weWrote_motPHI);}; {(waiting4motPHI) = (1); pvPut(waiting4motPHI);};
      motPHI = PHI; pvPut(motPHI, ASYNC);
     }
     if (waiting4motTTH || waiting4motTH || waiting4motCHI || waiting4motPHI) {
      {(waiting4Mot) = (1); pvPut(waiting4Mot);};
      if (!Busy) {(Busy) = (1); pvPut(Busy);};
     }
     {(newMotors) = (0); pvPut(newMotors);};
    } else {
     {strcpy(Msg, ("Motors not connected")); pvPut(Msg); {(Alert) = (1); pvPut(Alert);}; printf("orient: %s\n", Msg);};
     {(newMotors) = (0); pvPut(newMotors);};
    }
   }

   if (!waiting4Mot) {
    if (Busy) {(Busy) = (0); pvPut(Busy);};
    if (H_busy) {(H_busy) = (0); pvPut(H_busy);};
    if (K_busy) {(K_busy) = (0); pvPut(K_busy);};
    if (L_busy) {(L_busy) = (0); pvPut(L_busy);};
    if (TTH_busy) {(TTH_busy) = (0); pvPut(TTH_busy);};
    if (TH_busy) {(TH_busy) = (0); pvPut(TH_busy);};
    if (CHI_busy) {(CHI_busy) = (0); pvPut(CHI_busy);};
    if (PHI_busy) {(PHI_busy) = (0); pvPut(PHI_busy);};
    if (orientDebug) {printf("orient:putAll: all complete (no motors)\n");}
   }
  } state monitor_all
 }
}



ss connections {
 state check {

  when (delay(1)) {
   old_connect_state = motTTH_Connected;
   motTTH_Connected = pvAssigned(motTTH) ? pvConnected(motTTH) : 0;
   if (old_connect_state != motTTH_Connected) pvPut(motTTH_Connected);

   old_connect_state = motTH_Connected;
   motTH_Connected = pvAssigned(motTH) ? pvConnected(motTH) : 0;
   if (old_connect_state != motTH_Connected) pvPut(motTH_Connected);

   old_connect_state = motCHI_Connected;
   motCHI_Connected = pvAssigned(motCHI) ? pvConnected(motCHI) : 0;
   if (old_connect_state != motCHI_Connected) pvPut(motCHI_Connected);

   old_connect_state = motPHI_Connected;
   motPHI_Connected = pvAssigned(motPHI) ? pvConnected(motPHI) : 0;
   if (old_connect_state != motPHI_Connected) pvPut(motPHI_Connected);

   motorsConnected = motTTH_Connected && motTH_Connected && motCHI_Connected &&
    motPHI_Connected;

   motorStopsConnected = pvAssigned(motTTH_stop) && pvConnected(motTTH_stop) &&
    pvAssigned(motTH_stop) && pvConnected(motTH_stop) &&
    pvAssigned(motCHI_stop) && pvConnected(motCHI_stop) &&
    pvAssigned(motPHI_stop) && pvConnected(motPHI_stop);
  } state check
 }
}


ss readback {
 state rbv_monitor_throttle {
  when (delay(.1)) {
  } state rbv_monitor
 }

 state rbv_monitor_wait {
  when ((A0_state == 1) && (OMTX_state == 1)) {
  } state rbv_monitor

  when (delay(5)) {
  } state rbv_monitor
 }

 state rbv_monitor {
  when ((A0_state != 1) || (OMTX_state != 1)) {
   if (H_RBV != 0) {(H_RBV) = (0); pvPut(H_RBV);};
   if (K_RBV != 0) {(K_RBV) = (0); pvPut(K_RBV);};
   if (L_RBV != 0) {(L_RBV) = (0); pvPut(L_RBV);};
  } state rbv_monitor_wait

  when (efTest(motRBV_mon)) {
   efClear(motRBV_mon);

   %%pVar->rbv_angles[TTH_INDEX] = pVar->motTTH_RBV; pVar->rbv_angles[TH_INDEX] = pVar->motTH_RBV;
   %%pVar->rbv_angles[CHI_INDEX] = pVar->motCHI_RBV; pVar->rbv_angles[PHI_INDEX] = pVar->motPHI_RBV;
   %%angles_to_HKL(pVar->rbv_angles, pVar->OMTX_i, pVar->A0_i, pVar->rbv_hkl);
   %%pVar->H_RBV = pVar->rbv_hkl[H_INDEX]; pVar->K_RBV = pVar->rbv_hkl[K_INDEX]; pVar->L_RBV = pVar->rbv_hkl[L_INDEX];
   pvPut(H_RBV); pvPut(K_RBV); pvPut(L_RBV);
  } state rbv_monitor_throttle
 }
}


ss stop {
 state stop_all {
  when (stopMotors) {
   {strcpy(Msg, ("stopping motors")); pvPut(Msg); if (orientDebug) printf("orient: %s\n", Msg);};
   if (pvAssigned(motTTH_stop) && pvConnected(motTTH_stop)) {(motTTH_stop) = (1); pvPut(motTTH_stop);};
   if (pvAssigned(motTH_stop) && pvConnected(motTH_stop)) {(motTH_stop) = (1); pvPut(motTH_stop);};
   if (pvAssigned(motCHI_stop) && pvConnected(motCHI_stop)) {(motCHI_stop) = (1); pvPut(motCHI_stop);};
   if (pvAssigned(motPHI_stop) && pvConnected(motPHI_stop)) {(motPHI_stop) = (1); pvPut(motPHI_stop);};
   {(stopMotors) = (0); pvPut(stopMotors);};
  } state stop_all
 }
}
