# 1 "../kohzuCtl.st"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "../kohzuCtl.st"
program kohzuCtl ("P=nda, M_THETA=m9, M_Y=m10, M_Z=m11, GEOM=1")
# 44 "../kohzuCtl.st"
short kohzuCtlDebug;
assign kohzuCtlDebug to "{P}KohzuCtlDebug";
monitor kohzuCtlDebug;

string seqMsg1;
assign seqMsg1 to "{P}KohzuSeqMsg1SI";

string seqMsg2;
assign seqMsg2 to "{P}KohzuSeqMsg2SI";

short opAlert;
assign opAlert to "{P}KohzuAlertBO";

short opAck;
assign opAck to "{P}KohzuOperAckBO";
monitor opAck;





short putVals;
assign putVals to "{P}KohzuPutBO";
monitor putVals;





short autoMode;
assign autoMode to "{P}KohzuModeBO";
monitor autoMode;


short ccMode;
assign ccMode to "{P}KohzuMode2MO";
monitor ccMode;

short kohzuMoving;
assign kohzuMoving to "{P}KohzuMoving";
monitor kohzuMoving;

short kohzuDone;


double H;
assign H to "{P}BraggHAO";
monitor H;
evflag H_mon;
sync H H_mon;

double K;
assign K to "{P}BraggKAO";
monitor K;
evflag K_mon;
sync K K_mon;

double L;
assign L to "{P}BraggLAO";
monitor L;
evflag L_mon;
sync L L_mon;

double A;
assign A to "{P}BraggAAO";
monitor A;
evflag A_mon;
sync A A_mon;

double D;
assign D to "{P}Bragg2dSpacingAO";


double E;
assign E to "{P}BraggEAO";
monitor E;
evflag E_mon;
sync E E_mon;

double EHi;
assign EHi to "{P}BraggEAO.DRVH";

double ELo;
assign ELo to "{P}BraggEAO.DRVL";

double ERdbk;
assign ERdbk to "{P}BraggERdbkAO";

double lambda;
assign lambda to "{P}BraggLambdaAO";
monitor lambda;
evflag lambda_mon;
sync lambda lambda_mon;

double lambdaHi;
assign lambdaHi to "{P}BraggLambdaAO.DRVH";

double lambdaLo;
assign lambdaLo to "{P}BraggLambdaAO.DRVL";

double lambdaRdbk;
assign lambdaRdbk to "{P}BraggLambdaRdbkAO";

double theta;
assign theta to "{P}BraggThetaAO";
monitor theta;
evflag theta_mon;
sync theta theta_mon;

double thetaHi;
assign thetaHi to "{P}BraggThetaAO.DRVH";

double thetaLo;
assign thetaLo to "{P}BraggThetaAO.DRVL";

double thetaRdbk;
assign thetaRdbk to "{P}BraggThetaRdbkAO";







string thetaMotName;
assign thetaMotName to "{P}KohzuThetaPvSI";

string yMotName;
assign yMotName to "{P}KohzuYPvSI";

string zMotName;
assign zMotName to "{P}KohzuZPvSI";

double thetaMotCmdEcho;
assign thetaMotCmdEcho to "{P}KohzuThetaCmdAO";

double yMotCmdEcho;
assign yMotCmdEcho to "{P}KohzuYCmdAO";

double zMotCmdEcho;
assign zMotCmdEcho to "{P}KohzuZCmdAO";

double thetaMotRdbkEcho;
assign thetaMotRdbkEcho to "{P}KohzuThetaRdbkAI";

double yMotRdbkEcho;
assign yMotRdbkEcho to "{P}KohzuYRdbkAI";

double zMotRdbkEcho;
assign zMotRdbkEcho to "{P}KohzuZRdbkAI";

double thetaMotVelEcho;
assign thetaMotVelEcho to "{P}KohzuThetaVelAI";

double yMotVelEcho;
assign yMotVelEcho to "{P}KohzuYVelAI";

double zMotVelEcho;
assign zMotVelEcho to "{P}KohzuZVelAI";

short thetaDmovEcho;
assign thetaDmovEcho to "{P}KohzuThetaDmovBI";

short yDmovEcho;
assign yDmovEcho to "{P}KohzuYDmovBI";

short zDmovEcho;
assign zDmovEcho to "{P}KohzuZDmovBI";







short thetaMotStop;
assign thetaMotStop to "{P}{M_THETA}.STOP";

short yStop;
assign yStop to "{P}{M_Y}.STOP";

short zStop;
assign zStop to "{P}{M_Z}.STOP";

short thetaDmov;
assign thetaDmov to "{P}{M_THETA}.DMOV";
monitor thetaDmov;
evflag thetaDmov_mon;
sync thetaDmov thetaDmov_mon;

short yDmov;
assign yDmov to "{P}{M_Y}.DMOV";
monitor yDmov;
evflag yDmov_mon;
sync yDmov yDmov_mon;

short zDmov;
assign zDmov to "{P}{M_Z}.DMOV";
monitor zDmov;
evflag zDmov_mon;
sync zDmov zDmov_mon;


short thetaHls;
short thetaLls;
short yHls;
short yLls;
short zHls;
short zLls;



%% short kohzuCtl_LS_TEST = 0;
assign thetaHls to "{P}{M_THETA}.HLS";
assign thetaLls to "{P}{M_THETA}.LLS";
assign yHls to "{P}{M_Y}.HLS";
assign yLls to "{P}{M_Y}.LLS";
assign zHls to "{P}{M_Z}.HLS";
assign zLls to "{P}{M_Z}.LLS";
# 276 "../kohzuCtl.st"
monitor thetaHls;
monitor thetaLls;
monitor yHls;
monitor yLls;
monitor zHls;
monitor zLls;

double thetaMotDesired;
assign thetaMotDesired to "{P}KohzuThetaSetAO";
monitor thetaMotDesired;

double yMotDesired;
assign yMotDesired to "{P}KohzuYSetAO";
monitor yMotDesired;
double yMotDesiredHi;
assign yMotDesiredHi to "{P}KohzuYSetAO.DRVH";
double yMotDesiredLo;
assign yMotDesiredLo to "{P}KohzuYSetAO.DRVL";

double zMotDesired;
assign zMotDesired to "{P}KohzuZSetAO";
monitor zMotDesired;

double thetaMotHiLim;
assign thetaMotHiLim to "{P}{M_THETA}.HLM";
monitor thetaMotHiLim;
evflag thetaMotHiLim_mon;
sync thetaMotHiLim thetaMotHiLim_mon;

double thetaMotLoLim;
assign thetaMotLoLim to "{P}{M_THETA}.LLM";
monitor thetaMotLoLim;
evflag thetaMotLoLim_mon;
sync thetaMotLoLim thetaMotLoLim_mon;

double yMotHiLim;
assign yMotHiLim to "{P}{M_Y}.HLM";
monitor yMotHiLim;

double yMotLoLim;
assign yMotLoLim to "{P}{M_Y}.LLM";
monitor yMotLoLim;

double zMotHiLim;
assign zMotHiLim to "{P}{M_Z}.HLM";
monitor zMotHiLim;

double zMotLoLim;
assign zMotLoLim to "{P}{M_Z}.LLM";
monitor zMotLoLim;

double thetaMotCmd;
assign thetaMotCmd to "{P}{M_THETA}";

double yMotCmd;
assign yMotCmd to "{P}{M_Y}";

double zMotCmd;
assign zMotCmd to "{P}{M_Z}";

double thetaMotCmdMon;
assign thetaMotCmdMon to "{P}{M_THETA}";
monitor thetaMotCmdMon;
evflag thetaMotCmdMon_mon;
sync thetaMotCmdMon thetaMotCmdMon_mon;

double yMotCmdMon;
assign yMotCmdMon to "{P}{M_Y}";
monitor yMotCmdMon;
evflag yMotCmdMon_mon;
sync yMotCmdMon yMotCmdMon_mon;

double zMotCmdMon;
assign zMotCmdMon to "{P}{M_Z}";
monitor zMotCmdMon;
evflag zMotCmdMon_mon;
sync zMotCmdMon zMotCmdMon_mon;

double thTime;
double oldThSpeed;
double newThSpeed;
double thSpeed;
assign thSpeed to "{P}{M_THETA}.VELO";
monitor thSpeed;
evflag thetaMotVel_mon;
sync thSpeed thetaMotVel_mon;

double yTime;
double oldYSpeed;
double newYSpeed;
double ySpeed;
assign ySpeed to "{P}{M_Y}.VELO";
monitor ySpeed;
evflag yMotVel_mon;
sync ySpeed yMotVel_mon;

double zTime;
double oldZSpeed;
double newZSpeed;
double zSpeed;
assign zSpeed to "{P}{M_Z}.VELO";
monitor zSpeed;
evflag zMotVel_mon;
sync zSpeed zMotVel_mon;

double thetaMotRdbk;
assign thetaMotRdbk to "{P}{M_THETA}.RBV";
monitor thetaMotRdbk;
evflag thetaMotRdbk_mon;
sync thetaMotRdbk thetaMotRdbk_mon;
evflag thetaMotRdbkPseudo_mon;

double yMotRdbk;
assign yMotRdbk to "{P}{M_Y}.RBV";
monitor yMotRdbk;
evflag yMotRdbk_mon;
sync yMotRdbk yMotRdbk_mon;

double zMotRdbk;
assign zMotRdbk to "{P}{M_Z}.RBV";
monitor zMotRdbk;
evflag zMotRdbk_mon;
sync zMotRdbk zMotRdbk_mon;


short useSetMode;
assign useSetMode to "{P}KohzuUseSetBO";
monitor useSetMode;
evflag useSetMode_mon;
sync useSetMode useSetMode_mon;

short thetaMotSet;
assign thetaMotSet to "{P}{M_THETA}.SET";
monitor thetaMotSet;
evflag thetaMotSet_mon;
sync thetaMotSet thetaMotSet_mon;

short yMotSet;
assign yMotSet to "{P}{M_Y}.SET";
monitor yMotSet;
evflag yMotSet_mon;
sync yMotSet yMotSet_mon;

short zMotSet;
assign zMotSet to "{P}{M_Z}.SET";
monitor zMotSet;
evflag zMotSet_mon;
sync zMotSet zMotSet_mon;


double ETweakVal;
assign ETweakVal to "{P}BraggETweakAI";
monitor ETweakVal;

short ETweakInc;
assign ETweakInc to "{P}BraggEIncBO";
monitor ETweakInc;

short ETweakDec;
assign ETweakDec to "{P}BraggEDecBO";
monitor ETweakDec;

double LTweakVal;
assign LTweakVal to "{P}BraggLambdaTweakAI";
monitor LTweakVal;

short LTweakInc;
assign LTweakInc to "{P}BraggLambdaIncBO";
monitor LTweakInc;

short LTweakDec;
assign LTweakDec to "{P}BraggLambdaDecBO";
monitor LTweakDec;

double thTweakVal;
assign thTweakVal to "{P}BraggThetaTweakAI";
monitor thTweakVal;

short thTweakInc;
assign thTweakInc to "{P}BraggThetaIncBO";
monitor thTweakInc;

short thTweakDec;
assign thTweakDec to "{P}BraggThetaDecBO";
monitor thTweakDec;


double yOffset;
assign yOffset to "{P}Kohzu_yOffsetAO";
monitor yOffset;
evflag yOffset_mon;
sync yOffset yOffset_mon;

double yOffsetHi;
assign yOffsetHi to "{P}Kohzu_yOffsetAO.DRVH";

double yOffsetLo;
assign yOffsetLo to "{P}Kohzu_yOffsetAO.DRVL";

%%#include <string.h>
%%#include <math.h>

%%double kohzuCtl_calc2dSpacing();
%%char *kohzuCtl_pmacP;
%%char *kohzuCtl_pmacM;
%%char *kohzuCtl_pmacGEOM;
short geom;

short causedMove;
short initSeq;
short initSeqDone;
float avg;
int nint;
double hc;
double radConv;
double temp;

ss kohzuCtl {
state init {
 when () {
  initSeq = 0;
 } state initSequence
}

state initSequence {
 when (initSeq == 0) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:initSequence:initSeq=0\n");
  hc = 12.3984244;
  radConv = 57.2958;
  thetaMotStop = 1;
  yStop = 1;
  zStop = 1;
  putVals = 0;
  pvPut(putVals);
  autoMode = 0;
  pvPut(autoMode);
  opAck = 0;
  pvPut(opAck);
%{
  kohzuCtl_pmacP = seq_macValueGet(ssId,"P");
  kohzuCtl_pmacM = seq_macValueGet(ssId,"M_THETA");
  if (kohzuCtl_pmacP == 0 || kohzuCtl_pmacM == 0) {
   sprintf(thetaMotName,"??????");
  } else {
   strcpy(thetaMotName, kohzuCtl_pmacP);
   strcat(thetaMotName, kohzuCtl_pmacM);
  }
  kohzuCtl_pmacM = seq_macValueGet(ssId,"M_Y");
  if (kohzuCtl_pmacP == 0 || kohzuCtl_pmacM == 0) {
   sprintf(yMotName,"??????");
  } else {
   strcpy(yMotName, kohzuCtl_pmacP);
   strcat(yMotName, kohzuCtl_pmacM);
  }
  kohzuCtl_pmacM = seq_macValueGet(ssId,"M_Z");
  if (kohzuCtl_pmacP == 0 || kohzuCtl_pmacM == 0) {
   sprintf(zMotName,"??????");
  } else {
   strcpy(zMotName, kohzuCtl_pmacP);
   strcat(zMotName, kohzuCtl_pmacM);
  }
  kohzuCtl_pmacGEOM = seq_macValueGet(ssId,"GEOM");
  geom = 1;
  if (*kohzuCtl_pmacGEOM == '2') geom = 2;
}%
  if (geom == 1) {
   yOffsetHi = 17.5 + .000001;
   pvPut(yOffsetHi);
   yOffsetLo = 17.5 - .000001;
   pvPut(yOffsetLo);
   yOffset = 17.5;
   pvPut(yOffset);
   yMotDesiredHi = 0.;
   pvPut(yMotDesiredHi);
   yMotDesiredLo = -35;
   pvPut(yMotDesiredLo);
  } else {
   yMotDesiredHi = 60.;
   pvPut(yMotDesiredHi);
   yMotDesiredLo = 0.;
   pvPut(yMotDesiredLo);
  }
  pvPut(thetaMotName);
  pvPut(yMotName);
  pvPut(zMotName);
  initSeq = 1;
 } state dInputChanged

 when (initSeq == 1) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:initSequence:initSeq=1\n");
  initSeq = 2;
 } state chkMotorLimits

 when (initSeq == 2) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:initSequence:initSeq=2\n");
  initSeq = 3;
 } state updateRdbk

 when ((initSeq == 3) && delay(1.)) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:initSequence:initSeq=3\n");
  sprintf(seqMsg1, "Kohzu Control Ready");
  pvPut(seqMsg1);
  sprintf(seqMsg2, " ");
  pvPut(seqMsg2);
  initSeqDone=1;
  theta = thetaMotRdbk;
  pvPut(theta);
 } state thChanged
}

state waitForCmndEnter {
 when (initSeqDone && delay(.1)) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:waitForCmndEnter:initSeqDone\n");
  efClear(H_mon);
  efClear(K_mon);
  efClear(L_mon);
  efClear(A_mon);
  efClear(E_mon);
  efClear(lambda_mon);
  efClear(theta_mon);

  thetaMotRdbkEcho = thetaMotRdbk;
  pvPut(thetaMotRdbkEcho);
  yMotRdbkEcho = yMotRdbk;
  pvPut(yMotRdbkEcho);
  zMotRdbkEcho = zMotRdbk;
  pvPut(zMotRdbkEcho);
  thetaMotVelEcho = thSpeed;
  pvPut(thetaMotVelEcho);
  yMotVelEcho = ySpeed;
  pvPut(yMotVelEcho);
  zMotVelEcho = zSpeed;
  pvPut(zMotVelEcho);
  thetaMotCmdEcho = thetaMotCmdMon;
  pvPut(thetaMotCmdEcho);
  yMotCmdEcho = yMotCmdMon;
  pvPut(yMotCmdEcho);
  zMotCmdEcho = zMotCmdMon;
  pvPut(zMotCmdEcho);
  thetaDmovEcho = thetaDmov;
  pvPut(thetaDmovEcho);
  yDmovEcho = yDmov;
  pvPut(yDmovEcho);
  zDmovEcho = zDmov;
  pvPut(zDmovEcho);
  if (autoMode || useSetMode) {
   thetaRdbk = thetaMotRdbk;
   pvPut(thetaRdbk);
   lambdaRdbk = D*sin(thetaRdbk/radConv);
   pvPut(lambdaRdbk);
   ERdbk = hc/lambdaRdbk;
   pvPut(ERdbk);
  }
  opAck = 0;
  pvPut(opAck);
  if (kohzuCtlDebug) %%printf("kohzuCtl:waitForCmndEnter: kohzuMoving=%d; kohzuDone=%d\n", kohzuMoving, kohzuDone);
  if (kohzuMoving && kohzuDone) {
   if (kohzuCtlDebug) %%printf("kohzuCtl:waitForCmndEnter: asserting 'done'.\n");
   kohzuMoving=0; kohzuDone = 0;
   pvPut(kohzuMoving);
  }
 } state waitForCmnd

 when (!initSeqDone) {
 } state initSequence
}

state waitForCmnd {
 when (efTest(H_mon) || efTest(K_mon) || efTest(L_mon) || efTest(A_mon)) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:waitForCmnd:HKLA_mon\n");
  sprintf(seqMsg1, "Lattice Spacing Changed");
  pvPut(seqMsg1);
  sprintf(seqMsg2, " ");
  pvPut(seqMsg2);
  efClear(H_mon);
  efClear(K_mon);
  efClear(L_mon);
  efClear(A_mon);
 } state dInputChanged

 when (efTest(thetaMotHiLim_mon) || efTest(thetaMotLoLim_mon)) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:waitForCmnd:thetaMotXxLim_mon\n");
  sprintf(seqMsg1, "Theta Motor Soft Limits Changed");
  pvPut(seqMsg1);
  sprintf(seqMsg2, " ");
  pvPut(seqMsg2);
  efClear(thetaMotHiLim_mon);
  efClear(thetaMotLoLim_mon);
 } state thetaLimits

 when (efTestAndClear(thetaMotRdbk_mon)) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:waitForCmnd:thetaMotRdbk_mon\n");
  efSet(thetaMotRdbkPseudo_mon);
 } state updateRdbk

 when (efTestAndClear(E_mon)) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:waitForCmnd:E_mon\n");
 } state eChanged

 when (efTestAndClear(lambda_mon)) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:waitForCmnd:lambda_mon\n");
 } state lChanged

 when (efTestAndClear(theta_mon)) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:waitForCmnd:theta_mon\n");
 } state thChanged

 when (putVals && delay(.1)) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:waitForCmnd:putVals\n");
 } state moveKohzu

 when (ETweakInc||ETweakDec||LTweakInc||LTweakDec||thTweakInc||thTweakDec) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:waitForCmnd:Tweak\n");
 } state tweak

 when (efTestAndClear(yOffset_mon)) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:waitForCmnd:yOffset_mon\n");
  autoMode = 0;
  pvPut(autoMode);
  sprintf(seqMsg1, "y offset changed to %f", yOffset);
  pvPut(seqMsg1);
  sprintf(seqMsg2, "Set to Manual Mode");
  pvPut(seqMsg2);
 } state calcMovements

 when (opAck) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:waitForCmnd:opAck\n");
  opAlert = 0;
  pvPut(opAlert);
  sprintf(seqMsg1, " ");
  pvPut(seqMsg1);
  sprintf(seqMsg2, " ");
  pvPut(seqMsg2);
 } state waitForCmndEnter
}

state tweak {
 when (ETweakInc || ETweakDec) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:tweak:E+\n");
  temp = E + ETweakVal * (ETweakInc?1:-1);
  if ((temp>=ELo) && (temp<=EHi)) {
   E = temp;
   pvPut(E);
  } else {
   sprintf(seqMsg1, "E tweak exceeds limit");
   pvPut(seqMsg1);
   opAlert = 1;
   pvPut(opAlert);
  }
  if (ETweakInc) {
   ETweakInc = 0;
   pvPut(ETweakInc);
  } else {
   ETweakDec = 0;
   pvPut(ETweakDec);
  }
 } state eChanged

 when (LTweakInc || LTweakDec) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:tweak:L\n");
  temp = lambda + LTweakVal * (LTweakInc?1:-1);
  if ((temp>=lambdaLo) && (temp<=lambdaHi)) {
   lambda = temp;
   pvPut(lambda);
  } else {
   sprintf(seqMsg1, "Lambda tweak exceeds limit");
   pvPut(seqMsg1);
   opAlert = 1;
   pvPut(opAlert);
  }
  if (LTweakInc) {
   LTweakInc = 0;
   pvPut(LTweakInc);
  } else {
   LTweakDec = 0;
   pvPut(LTweakDec);
  }
 } state lChanged

 when (thTweakInc || thTweakDec) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:tweak:th\n");
  temp = theta + thTweakVal * (thTweakInc?1:-1);
  if ((temp>=thetaLo) && (temp<=thetaHi)) {
   theta = temp;
   pvPut(theta);
  } else {
   sprintf(seqMsg1, "Theta tweak exceeds limit");
   pvPut(seqMsg1);
   opAlert = 1;
   pvPut(opAlert);
  }
  if (thTweakInc) {
   thTweakInc = 0;
   pvPut(thTweakInc);
  } else {
   thTweakDec = 0;
   pvPut(thTweakDec);
  }
 } state thChanged
}

state dInputChanged {
 when () {
  if (kohzuCtlDebug) %%printf("kohzuCtl:dInputChanged\n");
  autoMode = 0;
  pvPut(autoMode);
  sprintf(seqMsg2, "Set to Manual Mode");
  pvPut(seqMsg2);
  D = kohzuCtl_calc2dSpacing();
  pvPut(D);
  pvPut(seqMsg1);
  lambdaHi = D*sin(thetaHi/radConv);
  pvPut(lambdaHi);
  lambdaLo = D*sin(thetaLo/radConv);
  pvPut(lambdaLo);
  EHi = hc/lambdaLo;
  pvPut(EHi);
  ELo = hc/lambdaHi;
  pvPut(ELo);
  pvPut(opAlert);
 } state thChanged
}

state thetaLimits {
 when () {
  if (kohzuCtlDebug) %%printf("kohzuCtl:thetaLimits\n");
  thetaHi = thetaMotHiLim;
  thetaLo = thetaMotLoLim;
  if (thetaHi > 89.0) {
   thetaHi = 89.0;
   sprintf(seqMsg2, "ThetaHiLim > 89 deg. (Using 89 deg.)");
   pvPut(seqMsg2);
  }
  if (thetaLo < 1.0) {
   thetaLo = 1.0;
   sprintf(seqMsg2, "ThetaLoLim < 1 deg. (Using 1 deg.)");
   pvPut(seqMsg2);
  }
  pvPut(thetaHi);
  pvPut(thetaLo);
  lambdaHi = D*sin(thetaHi/radConv);
  pvPut(lambdaHi);
  lambdaLo = D*sin(thetaLo/radConv);
  pvPut(lambdaLo);
  EHi = hc/lambdaLo;
  pvPut(EHi);
  ELo = hc/lambdaHi;
  pvPut(ELo);
 } state checkDone
}

state eChanged {
 when () {
  if (kohzuCtlDebug) %%printf("kohzuCtl:eChanged\n");
  lambda = hc/E;
  pvPut(lambda);
 } state lChanged
}

state lChanged {
 when (lambda > D) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:lChanged: lambda>D\n");

  sprintf(seqMsg1, "Wavelength > 2d spacing.");
  pvPut(seqMsg1);
  opAlert = 1;
  pvPut(opAlert);
 } state thChanged

 when () {
  if (kohzuCtlDebug) %%printf("kohzuCtl:lChanged\n");
  theta = radConv * asin(lambda/D);
  pvPut(theta);
 } state thChanged
}

state thChanged {
 when () {
  if (kohzuCtlDebug) %%printf("kohzuCtl:thChanged\n");
  if ((theta <= thetaLo) || (theta >= thetaHi)) {
   sprintf(seqMsg1, "Theta constrained to LIMIT");
   pvPut(seqMsg1);
   opAlert = 1;
   pvPut(opAlert);
   autoMode = 0;
   pvPut(autoMode);
   sprintf(seqMsg2, "Set to Manual Mode");
   pvPut(seqMsg2);
  }
  lambda = D*sin(theta/radConv);
  pvPut(lambda);
  E = hc/lambda;
  pvPut(E);


  thetaRdbk = thetaMotRdbk;
  lambdaRdbk = D * sin(thetaRdbk/radConv);
  ERdbk = hc/lambdaRdbk;
  pvPut(thetaRdbk);
  pvPut(lambdaRdbk);
  pvPut(ERdbk);
 } state calcMovements
}

state calcMovements {
 when () {
  if (kohzuCtlDebug) %%printf("kohzuCtl:calcMovements\n");


  thetaMotDesired = theta;
  if (geom == 1) {
   yMotDesired = -yOffset/cos(theta/radConv);
   zMotDesired = yOffset/sin(theta/radConv);
  } else {
   yMotDesired = yOffset/(2*cos(theta/radConv));
   zMotDesired = yOffset/(2*sin(theta/radConv));
  }
  if (ccMode != 1 && ccMode != 3 &&
     ((yMotDesired < yMotLoLim)||(yMotDesired > yMotHiLim))) {
   sprintf(seqMsg1, "Y will exceed soft limits");
   pvPut(seqMsg1);
   sprintf(seqMsg2, "Setting to Manual Mode");
   pvPut(seqMsg2);
   opAlert = 1;
   pvPut(opAlert);
   autoMode = 0;
   pvPut(autoMode);
  }
  if (ccMode != 1 && ccMode != 2 &&
     ((zMotDesired < zMotLoLim)||(zMotDesired > zMotHiLim))) {
   sprintf(seqMsg1, "Z will exceed soft limits");
   pvPut(seqMsg1);
   sprintf(seqMsg2, "Setting to Manual Mode");
   pvPut(seqMsg2);
   opAlert = 1;
   pvPut(opAlert);
   autoMode = 0;
   pvPut(autoMode);
  }
  pvPut(thetaMotDesired);
  pvPut(yMotDesired);
  pvPut(zMotDesired);


  oldThSpeed = thSpeed;
  oldYSpeed = ySpeed;
  oldZSpeed = zSpeed;
  thTime = fabs(theta-thetaMotRdbk)/thSpeed;
  yTime = fabs(yMotDesired-yMotRdbk)/ySpeed;
  zTime = fabs(zMotDesired-zMotRdbk)/zSpeed;

  if (ccMode == 1) {

   yTime = 0.0;
   zTime = 0.0;
  } else if (ccMode == 2) {

   zTime = 0.0;
  } else if (ccMode == 3) {

   yTime = 0.0;
  }

  if ((thTime >= yTime) && (thTime >= zTime)) {
   newThSpeed = thSpeed;
   newYSpeed = fabs(yMotDesired-yMotRdbk)/thTime;
   newZSpeed = fabs(zMotDesired-zMotRdbk)/thTime;
  } else if ((yTime >= thTime) && (yTime >= zTime)) {
   newThSpeed = fabs(theta-thetaMotRdbk)/yTime;
   newYSpeed = ySpeed;
   newZSpeed = fabs(zMotDesired-zMotRdbk)/yTime;
  } else {
   newThSpeed = fabs(theta-thetaMotRdbk)/zTime;
   newYSpeed = fabs(yMotDesired-yMotRdbk)/zTime;
   newZSpeed = zSpeed;
  }

 } state moveKohzu
}

state moveKohzu {
 when (autoMode || putVals || useSetMode) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:moveKohzu:auto|put\n");
  thSpeed = newThSpeed;
  ySpeed = newYSpeed;
  zSpeed = newZSpeed;
  pvPut(thSpeed);
  if (ccMode == 0 || ccMode == 2) {
   pvPut(ySpeed);
  }
  if (ccMode == 0 || ccMode == 3) {
   pvPut(zSpeed);
  }
  thetaMotCmd = thetaMotDesired;
  yMotCmd = yMotDesired;
  zMotCmd = zMotDesired;
  pvPut(thetaMotCmd);
  if (ccMode == 0) {
   pvPut(yMotCmd);
   pvPut(zMotCmd);
  } else if (ccMode == 1) {
   ;
  } else if (ccMode == 2) {
   pvPut(yMotCmd);
  } else if (ccMode == 3) {
   pvPut(zMotCmd);
  }
  putVals = 0;
  pvPut(putVals);
  causedMove = 1;
 } state updateRdbk

 when (1) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:moveKohzu\n");
 } state waitForCmndEnter
}

state checkDone {
 when (thetaDmov && yDmov && zDmov) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:checkDone: done\n");
  kohzuDone = 1;
 } state waitForCmndEnter

 when (!thetaDmov || !yDmov || !zDmov) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:checkDone: not done\n");
 } state updateRdbk

}

state updateRdbk {
 when (thetaMotRdbk == 0) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:updateRdbk:thetaMotRdbk==0\n");
  sprintf(seqMsg1, "Theta Motor Readback is 0 !");
  pvPut(seqMsg1);
  sprintf(seqMsg2, "Monochromator stopped");
  pvPut(seqMsg2);
  opAlert = 1;
  pvPut(opAlert);
 } state waitForCmndEnter

 when (thetaDmov && yDmov && zDmov) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:updateRdbk:*Dmov (done=%d,moving=%d)\n", kohzuDone, kohzuMoving);
  thetaRdbk = thetaMotRdbk;
  lambdaRdbk = D * sin(thetaRdbk/radConv);
  ERdbk = hc/lambdaRdbk;
  pvPut(thetaRdbk);
  pvPut(lambdaRdbk);
  pvPut(ERdbk);
  kohzuDone = 1;
  efSet(thetaMotRdbkPseudo_mon);
  efClear(thetaMotRdbk_mon);
  if (kohzuCtlDebug) %%printf("kohzuCtl:updateRdbk: done\n");
 } state thetaMotStopped


 when (thetaHls || thetaLls) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:updateRdbk: Th limit switch\n");
  sprintf(seqMsg1, "Theta Motor hit a limit switch!");
  pvPut(seqMsg1);
  opAlert = 1;
  pvPut(opAlert);
  autoMode = 0;
  pvPut(autoMode);
  sprintf(seqMsg2, "Setting to Manual Mode");
  pvPut(seqMsg2);
 } state stopKohzu


 when ((ccMode != 1) && (ccMode != 3) && (yHls || yLls)) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:updateRdbk: Y limit switch\n");
  sprintf(seqMsg1, "Y Motor hit a limit switch!");
  pvPut(seqMsg1);
  opAlert = 1;
  pvPut(opAlert);
  autoMode = 0;
  pvPut(autoMode);
  sprintf(seqMsg2, "Setting to Manual Mode");
  pvPut(seqMsg2);
 } state stopKohzu


 when ((ccMode != 1) && (ccMode != 2) && (zHls || zLls)) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:updateRdbk: Z limit switch\n");
  sprintf(seqMsg1, "Z Motor hit a limit switch!");
  pvPut(seqMsg1);
  opAlert = 1;
  pvPut(opAlert);
  autoMode = 0;
  pvPut(autoMode);
  sprintf(seqMsg2, "Setting to Manual Mode");
  pvPut(seqMsg2);
 } state stopKohzu

 when ((!thetaDmov || !yDmov || !zDmov) && delay(.1)) {
  if (kohzuCtlDebug>1) %%printf("kohzuCtl:updateRdbk:moving\n");
  thetaRdbk = thetaMotRdbk;
  lambdaRdbk = D * sin(thetaRdbk/radConv);
  ERdbk = hc/lambdaRdbk;
  pvPut(thetaRdbk);
  pvPut(lambdaRdbk);
  pvPut(ERdbk);
 } state updateRdbk
}

state thetaMotStopped {
 when (causedMove) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:thetaMotStopped:causedMove (done=%d,moving=%d)\n", kohzuDone, kohzuMoving);
  thSpeed = oldThSpeed;
  ySpeed = oldYSpeed;
  zSpeed = oldZSpeed;
  pvPut(thSpeed);
  pvPut(ySpeed);
  if (kohzuCtlDebug) %%printf("kohzuCtl:thetaMotStopped: ySpeed = %f\n", ySpeed);
  pvPut(zSpeed);
  causedMove = 0;
 } state waitForCmndEnter


 when (!causedMove) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:thetaMotStopped:!causedMove (done=%d,moving=%d)\n", kohzuDone, kohzuMoving);
  if (initSeqDone) {
   opAlert = 1;
   pvPut(opAlert);
   sprintf(seqMsg1, "Only Theta Motor Moved");
   pvPut(seqMsg1);
   sprintf(seqMsg2, "Hit MOVE to Adjust Y & Z");
   pvPut(seqMsg2);
   autoMode = 0;
   pvPut(autoMode);
  }
  theta = thetaMotRdbk;
  pvPut(theta);
  lambda = D*sin(theta/radConv);
  pvPut(lambda);
  E = hc/lambda;
  pvPut(E);
 } state thChanged
}

state chkMotorLimits {
 when () {
  if (kohzuCtlDebug) %%printf("kohzuCtl:chkMotorLimits\n");
  opAlert = 0;
  thetaRdbk = thetaMotRdbk;
  lambdaRdbk = D * sin(thetaRdbk/radConv);
  ERdbk = hc/lambdaRdbk;
  pvPut(thetaRdbk);
  pvPut(lambdaRdbk);
  pvPut(ERdbk);

  if (ccMode != 1 && ccMode != 3) {
   if (yMotRdbk == 0) {
    sprintf(seqMsg1, "Y Readback of 0 not reasonable");
    pvPut(seqMsg1);
    opAlert = 1;
   }
   if ((yMotRdbk < yMotLoLim) || (yMotRdbk > yMotHiLim)) {
    sprintf(seqMsg1, "Y Readback outside Motor Limits");
    pvPut(seqMsg1);
    opAlert = 1;
   }
  }
  if (ccMode != 1 && ccMode != 2) {
   if (zMotRdbk == 0) {
    sprintf(seqMsg1, "Z Readback of 0 not reasonable");
    pvPut(seqMsg1);
    opAlert = 1;
   }
   if ((zMotRdbk < zMotLoLim)||(zMotRdbk > zMotHiLim)) {
    sprintf(seqMsg1, "Z Readback outside Motor Limits");
    pvPut(seqMsg1);
    opAlert = 1;
   }
  }
  if (thetaMotRdbk == 0) {
   sprintf(seqMsg1, "Theta Readback of 0 not reasonable");
   pvPut(seqMsg1);
   opAlert = 1;
  }
  if ((thetaMotRdbk<thetaMotLoLim)||(thetaMotRdbk>thetaMotHiLim)) {
   sprintf(seqMsg1, "Theta Readback outside Motor Limits");
   pvPut(seqMsg1);
   opAlert = 1;
  }
  if (opAlert) {
   pvPut(opAlert);
   autoMode = 0;
   pvPut(autoMode);
   sprintf(seqMsg2, "Set to Manual Mode");
   pvPut(seqMsg2);
  }
 } state chkMotorLimitsExit
}

state chkMotorLimitsExit {
 when (!opAlert) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:chkMotorLimitsExit: OK\n");
  pvPut(opAlert);
  sprintf(seqMsg1, "All motors within limits");
  pvPut(seqMsg1);
 } state waitForCmndEnter

 when (opAlert && delay(2.)) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:chkMotorLimitsExit: opAlert\n");
  pvPut(opAlert);
  sprintf(seqMsg2, "Modify Motor Positions/Limits");
  pvPut(seqMsg2);
 } state chkMotorLimits
}

state stopKohzu {
 when () {
  if (kohzuCtlDebug) %%printf("kohzuCtl:stopKohzu\n");
  pvPut(thetaMotStop);
  pvPut(yStop);
  pvPut(zStop);
 } state stoppedKohzuWait
}

state stoppedKohzuWait {
 when(delay(1.)) {
  if (kohzuCtlDebug) %%printf("kohzuCtl:stoppedKohzuWait\n");
 } state updateRdbk
}

}







ss updatePsuedo {
state init {
 when () {
 } state update
}

state update {
 when (efTestAndClear(thetaMotRdbkPseudo_mon)) {
  thetaMotRdbkEcho = thetaMotRdbk;
  pvPut(thetaMotRdbkEcho);
 } state update

 when (efTestAndClear(yMotRdbk_mon)) {
  yMotRdbkEcho = yMotRdbk;
  pvPut(yMotRdbkEcho);
 } state update

 when (efTestAndClear(zMotRdbk_mon)) {
  zMotRdbkEcho = zMotRdbk;
  pvPut(zMotRdbkEcho);
 } state update

 when (efTestAndClear(thetaMotVel_mon)) {
  thetaMotVelEcho = thSpeed;
  pvPut(thetaMotVelEcho);
 } state update

 when (efTestAndClear(yMotVel_mon)) {
  yMotVelEcho = ySpeed;
  pvPut(yMotVelEcho);
 } state update

 when (efTestAndClear(zMotVel_mon)) {
  zMotVelEcho = zSpeed;
  pvPut(zMotVelEcho);
 } state update

 when (efTestAndClear(thetaMotCmdMon_mon)) {
  thetaMotCmdEcho = thetaMotCmdMon;
  pvPut(thetaMotCmdEcho);
 } state update

 when (efTestAndClear(yMotCmdMon_mon)) {
  yMotCmdEcho = yMotCmdMon;
  pvPut(yMotCmdEcho);
 } state update

 when (efTestAndClear(zMotCmdMon_mon)) {
  zMotCmdEcho = zMotCmdMon;
  pvPut(zMotCmdEcho);
 } state update

 when (efTestAndClear(thetaDmov_mon)) {
  thetaDmovEcho = thetaDmov;
  pvPut(thetaDmovEcho);
 } state update

 when (efTestAndClear(yDmov_mon)) {
  yDmovEcho = yDmov;
  pvPut(yDmovEcho);
 } state update

 when (efTestAndClear(zDmov_mon)) {
  zDmovEcho = zDmov;
  pvPut(zDmovEcho);
 } state update
}
}







ss updateSet {
state init {
 when () {
 } state update
}

state update {
 when (efTestAndClear(useSetMode_mon)) {
  if (thetaMotSet != useSetMode) {
   thetaMotSet = useSetMode;
   pvPut(thetaMotSet);
  }
  if (yMotSet != useSetMode) {
   yMotSet = useSetMode;
   pvPut(yMotSet);
  }
  if (zMotSet != useSetMode) {
   zMotSet = useSetMode;
   pvPut(zMotSet);
  }
 } state update

 when (efTestAndClear(thetaMotSet_mon)) {
  if (useSetMode != thetaMotSet) {
   useSetMode = thetaMotSet;
   pvPut(useSetMode);
  }
 } state update

 when (efTestAndClear(yMotSet_mon)) {
  if (useSetMode != yMotSet) {
   useSetMode = yMotSet;
   pvPut(useSetMode);
  }
 } state update

 when (efTestAndClear(zMotSet_mon)) {
  if (useSetMode != zMotSet) {
   useSetMode = zMotSet;
   pvPut(useSetMode);
  }
 } state update
}
}





%{


double kohzuCtl_calc2dSpacing()
{

 double result;

 result = (2*A)/sqrt(H*H + K*K + L*L);


 opAlert = 0;
 if (((fmod(H,2)) != (fmod(K,2))) || ((fmod(H,2)) != (fmod(L,2))))
  opAlert = 1;
 avg = (H+K+L)/2.0;
 nint = (int)((avg)>0 ? (avg)+0.5 : (avg)-0.5);
 if ((fabs(avg-nint) <= .25) && (fmod(nint,2)))
  opAlert = 1;
 if (opAlert) {
  sprintf(seqMsg1, "(H,K,L) is 'forbidden' reflection");
 } else {
  sprintf(seqMsg1, "New d spacing");
 }
 return(result);
}


}%
