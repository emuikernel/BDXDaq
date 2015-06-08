# 1 "../MM4005_trajectoryScan.st"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "../MM4005_trajectoryScan.st"
program MM4005_trajectoryScan("P=13IDC:,R=traj1,M1=M1,M2=M2,M3=M3,M4=M4,M5=M5,M6=M6,M7=M7,M8=M8,PORT=serial1")
# 12 "../MM4005_trajectoryScan.st"
%% #include <string.h>
%% #include <stdio.h>
%% #include <epicsString.h>
%% #include <asynOctetSyncIO.h>


option +r;
# 38 "../MM4005_trajectoryScan.st"
# 1 "../trajectoryScan.h" 1
# 40 "../trajectoryScan.h"
int debugLevel; assign debugLevel to "{P}{R}DebugLevel.VAL";
monitor debugLevel;
int numAxes; assign numAxes to "{P}{R}NumAxes.VAL";
monitor numAxes;
int nelements; assign nelements to "{P}{R}Nelements.VAL";
monitor nelements;
int npulses; assign npulses to "{P}{R}Npulses.VAL";
monitor npulses;
int startPulses; assign startPulses to "{P}{R}StartPulses.VAL";
monitor startPulses;
int endPulses; assign endPulses to "{P}{R}EndPulses.VAL";
monitor endPulses;
int nactual; assign nactual to "{P}{R}Nactual.VAL";
int moveMode; assign moveMode to "{P}{R}MoveMode.VAL";
monitor moveMode;
double time; assign time to "{P}{R}Time.VAL";
monitor time;
double timeScale; assign timeScale to "{P}{R}TimeScale.VAL";
monitor timeScale;
int timeMode; assign timeMode to "{P}{R}TimeMode.VAL";
monitor timeMode;
double accel; assign accel to "{P}{R}Accel.VAL";
monitor accel;
int build; assign build to "{P}{R}Build.VAL";
monitor build;
int buildState; assign buildState to "{P}{R}BuildState.VAL";
int buildStatus; assign buildStatus to "{P}{R}BuildStatus.VAL";
string buildMessage;assign buildMessage to "{P}{R}BuildMessage.VAL";
int simMode; assign simMode to "{P}{R}SimMode.VAL";
monitor simMode;
int execute; assign execute to "{P}{R}Execute.VAL";
monitor execute;
int execState; assign execState to "{P}{R}ExecState.VAL";
monitor execState;
int execStatus; assign execStatus to "{P}{R}ExecStatus.VAL";
string execMessage; assign execMessage to "{P}{R}ExecMessage.VAL";
int abort; assign abort to "{P}{R}Abort.VAL";
monitor abort;
int readback; assign readback to "{P}{R}Readback.VAL";
monitor readback;
int readState; assign readState to "{P}{R}ReadState.VAL";
int readStatus; assign readStatus to "{P}{R}ReadStatus.VAL";
string readMessage; assign readMessage to "{P}{R}ReadMessage.VAL";
double timeTrajectory[2000];
assign timeTrajectory to "{P}{R}TimeTraj.VAL";
monitor timeTrajectory;
string trajectoryFile; assign trajectoryFile to "{P}{R}TrajectoryFile.VAL";
monitor trajectoryFile;

int moveAxis[8];
assign moveAxis to
        {"{P}{R}M1Move.VAL",
         "{P}{R}M2Move.VAL",
         "{P}{R}M3Move.VAL",
         "{P}{R}M4Move.VAL",
         "{P}{R}M5Move.VAL",
         "{P}{R}M6Move.VAL",
         "{P}{R}M7Move.VAL",
         "{P}{R}M8Move.VAL"};
monitor moveAxis;

double motorTrajectory[8][2000];
assign motorTrajectory to
        {"{P}{R}M1Traj.VAL",
         "{P}{R}M2Traj.VAL",
         "{P}{R}M3Traj.VAL",
         "{P}{R}M4Traj.VAL",
         "{P}{R}M5Traj.VAL",
         "{P}{R}M6Traj.VAL",
         "{P}{R}M7Traj.VAL",
         "{P}{R}M8Traj.VAL"};
monitor motorTrajectory;

double motorReadbacks[8][2000];
assign motorReadbacks to
        {"{P}{R}M1Actual.VAL",
         "{P}{R}M2Actual.VAL",
         "{P}{R}M3Actual.VAL",
         "{P}{R}M4Actual.VAL",
         "{P}{R}M5Actual.VAL",
         "{P}{R}M6Actual.VAL",
         "{P}{R}M7Actual.VAL",
         "{P}{R}M8Actual.VAL"};

double motorError[8][2000];
assign motorError to
        {"{P}{R}M1Error.VAL",
         "{P}{R}M2Error.VAL",
         "{P}{R}M3Error.VAL",
         "{P}{R}M4Error.VAL",
         "{P}{R}M5Error.VAL",
         "{P}{R}M6Error.VAL",
         "{P}{R}M7Error.VAL",
         "{P}{R}M8Error.VAL"};

double motorCurrent[8];
assign motorCurrent to
        {"{P}{R}M1Current.VAL",
         "{P}{R}M2Current.VAL",
         "{P}{R}M3Current.VAL",
         "{P}{R}M4Current.VAL",
         "{P}{R}M5Current.VAL",
         "{P}{R}M6Current.VAL",
         "{P}{R}M7Current.VAL",
         "{P}{R}M8Current.VAL"};

double motorMDVS[8];
assign motorMDVS to
        {"{P}{R}M1MDVS.VAL",
         "{P}{R}M2MDVS.VAL",
         "{P}{R}M3MDVS.VAL",
         "{P}{R}M4MDVS.VAL",
         "{P}{R}M5MDVS.VAL",
         "{P}{R}M6MDVS.VAL",
         "{P}{R}M7MDVS.VAL",
         "{P}{R}M8MDVS.VAL"};
monitor motorMDVS;

double motorMDVA[8];
assign motorMDVA to
        {"{P}{R}M1MDVA.VAL",
         "{P}{R}M2MDVA.VAL",
         "{P}{R}M3MDVA.VAL",
         "{P}{R}M4MDVA.VAL",
         "{P}{R}M5MDVA.VAL",
         "{P}{R}M6MDVA.VAL",
         "{P}{R}M7MDVA.VAL",
         "{P}{R}M8MDVA.VAL"};

int motorMDVE[8];
assign motorMDVE to
        {"{P}{R}M1MDVE.VAL",
         "{P}{R}M2MDVE.VAL",
         "{P}{R}M3MDVE.VAL",
         "{P}{R}M4MDVE.VAL",
         "{P}{R}M5MDVE.VAL",
         "{P}{R}M6MDVE.VAL",
         "{P}{R}M7MDVE.VAL",
         "{P}{R}M8MDVE.VAL"};

double motorMVA[8];
assign motorMVA to
        {"{P}{R}M1MVA.VAL",
         "{P}{R}M2MVA.VAL",
         "{P}{R}M3MVA.VAL",
         "{P}{R}M4MVA.VAL",
         "{P}{R}M5MVA.VAL",
         "{P}{R}M6MVA.VAL",
         "{P}{R}M7MVA.VAL",
         "{P}{R}M8MVA.VAL"};

int motorMVE[8];
assign motorMVE to
        {"{P}{R}M1MVE.VAL",
         "{P}{R}M2MVE.VAL",
         "{P}{R}M3MVE.VAL",
         "{P}{R}M4MVE.VAL",
         "{P}{R}M5MVE.VAL",
         "{P}{R}M6MVE.VAL",
         "{P}{R}M7MVE.VAL",
         "{P}{R}M8MVE.VAL"};

double motorMAA[8];
assign motorMAA to
        {"{P}{R}M1MAA.VAL",
         "{P}{R}M2MAA.VAL",
         "{P}{R}M3MAA.VAL",
         "{P}{R}M4MAA.VAL",
         "{P}{R}M5MAA.VAL",
         "{P}{R}M6MAA.VAL",
         "{P}{R}M7MAA.VAL",
         "{P}{R}M8MAA.VAL"};

int motorMAE[8];
assign motorMAE to
        {"{P}{R}M1MAE.VAL",
         "{P}{R}M2MAE.VAL",
         "{P}{R}M3MAE.VAL",
         "{P}{R}M4MAE.VAL",
         "{P}{R}M5MAE.VAL",
         "{P}{R}M6MAE.VAL",
         "{P}{R}M7MAE.VAL",
         "{P}{R}M8MAE.VAL"};



double epicsMotorPos[8];
assign epicsMotorPos to {"","","","","","","",""};
monitor epicsMotorPos;

double epicsMotorDir[8];
assign epicsMotorDir to {"","","","","","","",""};
monitor epicsMotorDir;

double epicsMotorOff[8];
assign epicsMotorOff to {"","","","","","","",""};
monitor epicsMotorOff;

double epicsMotorDone[8];
assign epicsMotorDone to {"","","","","","","",""};
monitor epicsMotorDone;


evflag buildMon; sync build buildMon;
evflag executeMon; sync execute executeMon;
evflag execStateMon; sync execState execStateMon;
evflag abortMon; sync abort abortMon;
evflag readbackMon; sync readback readbackMon;
evflag nelementsMon; sync nelements nelementsMon;
evflag motorMDVSMon; sync motorMDVS motorMDVSMon;
# 39 "../MM4005_trajectoryScan.st" 2
# 59 "../MM4005_trajectoryScan.st"
char stringOut[256];
char stringIn[256];
char *asynPort;
char *pasynUser;
int status;
int i;
int j;
int k;
double delay;
int anyMoving;
int ncomplete;
int nextra;
int npoints;
int dir;
double dtime;
double dpos;
double posActual;
double posTheory;
double expectedTime;
double initialPos[8];
char macroBuf[100];
char motorName[100];
char *p;
char *tok_save;



int motorCurrentIndex[8];
int epicsMotorDoneIndex[8];



unsigned long startTime;


%% static int writeOnly(SS_ID ssId, struct UserVar *pVar, char *command);
%% static int writeRead(SS_ID ssId, struct UserVar *pVar, char *command);
%% static int getMotorPositions(SS_ID ssId, struct UserVar *pVar, double *pos);
%% static int getMotorMoving(SS_ID ssId, struct UserVar *pVar);
%% static int getEpicsMotorMoving(SS_ID ssId, struct UserVar *pVar);
%% static int waitEpicsMotors(SS_ID ssId, struct UserVar *pVar);


ss trajectoryScan {


    state init {
        when() {

            if (numAxes > 8) numAxes = 8;
            for (i=0; i<numAxes; i++) {
                sprintf(macroBuf, "M%d", i+1);
                sprintf(motorName, "%s%s.VAL", macValueGet("P"), macValueGet(macroBuf));
                pvAssign(epicsMotorPos[i], motorName);
                sprintf(motorName, "%s%s.DIR", macValueGet("P"), macValueGet(macroBuf));
                pvAssign(epicsMotorDir[i], motorName);
                sprintf(motorName, "%s%s.OFF", macValueGet("P"), macValueGet(macroBuf));
                pvAssign(epicsMotorOff[i], motorName);
                sprintf(motorName, "%s%s.DMOV", macValueGet("P"), macValueGet(macroBuf));
                pvAssign(epicsMotorDone[i], motorName);
            }

            asynPort = macValueGet("PORT");
            %%pVar->status = pasynOctetSyncIO->connect(pVar->asynPort, 0,
            %% (asynUser **)&pVar->pasynUser,
            %% NULL);
            if (status != 0) {
               printf("trajectoryScan error in pasynOctetSyncIO->connect\n");
               printf("   status=%d, port=%s\n", status, asynPort);
            }
            for (j=0; j<numAxes; j++) {
                motorCurrentIndex[j] = pvIndex(motorCurrent[j]);
                epicsMotorDoneIndex[j] = pvIndex(epicsMotorDone[j]);
            }

            for (j=0; j<numAxes; j++) {
                sprintf(stringOut, "%dGC?", j+1);
                %%writeRead(ssId, pVar, pVar->stringOut);

                motorMDVS[j] = atof(stringIn+3);
                pvPut(motorMDVS[j]);
            }

            efClear(buildMon);
            efClear(executeMon);
            efClear(abortMon);
            efClear(readbackMon);
            efClear(nelementsMon);
            efClear(motorMDVSMon);
        } state monitor_inputs
    }



    state monitor_inputs {
        when(efTestAndClear(buildMon) && (build==1)) {
        } state build

        when(efTestAndClear(executeMon) && (execute==1)) {
        } state execute

        when(efTestAndClear(readbackMon) && (readback==1)) {
        } state readback

        when(efTestAndClear(nelementsMon) && (nelements>=1)) {



            endPulses = nelements;
            pvPut(endPulses);
        } state monitor_inputs

        when(efTestAndClear(motorMDVSMon)) {



            for (j=0; j<numAxes; j++) {
                sprintf(stringOut, "%dGC%f", j+1, motorMDVS[j]);
                %%writeOnly(ssId, pVar, pVar->stringOut);
            }
        } state monitor_inputs
    }



    state build {
        when() {

            buildState = 1;
            pvPut(buildState);
            buildStatus=0;
            pvPut(buildStatus);

            strcpy(stringOut, "NC");
            %%writeOnly(ssId, pVar, pVar->stringOut);

            for (i=0; i<numAxes; i++) {
                sprintf(stringOut, "%dDC%d", i+1, moveAxis[i]);
                %%writeOnly(ssId, pVar, pVar->stringOut);
            }

            sprintf(stringOut, "UC%f", accel);
            %%writeOnly(ssId, pVar, pVar->stringOut);


            if (timeMode == 0) {
                dtime = time/nelements;
                for (i=0; i<nelements; i++) timeTrajectory[i] = dtime;
                pvPut(timeTrajectory);
            }





            if (moveMode == 0) {
                npoints=nelements;
            } else {
                npoints=nelements-1;
            }
            nextra = (npoints % 4);
            if (nextra != 0) {
                nextra = 4-nextra;


                for (i=0; i<nextra; i++) {
                    timeTrajectory[npoints+i] = 0.1;
                    for (j=0; j<numAxes; j++) {
                        if (!moveAxis[j]) continue;
                        if (moveMode == 0) {
                            motorTrajectory[j][nelements+i] =
                                        motorTrajectory[j][nelements-1] *
                                        0.1 / timeTrajectory[nelements-1];
                        } else {
                            dpos = (motorTrajectory[j][nelements-1] -
                                            motorTrajectory[j][nelements-2]) *
                                        0.1 / timeTrajectory[nelements-2];
                            motorTrajectory[j][nelements+i] =
                                        motorTrajectory[j][nelements-1] +
                                                                dpos*(i+1);
                        }
                    }
                }
                nelements += nextra;
                npoints += nextra;
                pvPut(nelements);
                pvPut(timeTrajectory);

                for (j=0; j<numAxes; j++) {
                   pvPut(motorTrajectory[j]);
                }
            }

            expectedTime=0;
            for (i=0; i<npoints; i++)
                                expectedTime += timeTrajectory[i];

            for (i=0; i<npoints; i++) {
                sprintf(buildMessage, "Building element %d/%d", i+1, nelements);
                pvPut(buildMessage);
                sprintf(stringOut, "%dDT%f", i+1, timeTrajectory[i]);
                %%writeOnly(ssId, pVar, pVar->stringOut);
                for (j=0; j<numAxes; j++) {
                    if (!moveAxis[j]) continue;
                    if (moveMode == 0) {
                        dpos = motorTrajectory[j][i];
                    } else {
                        dpos = motorTrajectory[j][i+1] - motorTrajectory[j][i];
                    }

                    if (epicsMotorDir[j] == 0) dir=1; else dir=-1;
                    dpos = dpos*dir;
                    sprintf(stringOut, "%dDX%f", j+1, dpos);
                    %%writeOnly(ssId, pVar, pVar->stringOut);
                }



                if (((i+1) % 20) == 0) %%writeRead(ssId, pVar, "TB");
            }

            if (npulses > 0) {

                if (startPulses < 1) startPulses = 1;
                if (startPulses > npoints) startPulses = npoints;
                pvPut(startPulses);
                if (endPulses < startPulses) endPulses = startPulses;
                if (endPulses > npoints) endPulses = npoints;
                pvPut(endPulses);


                sprintf(stringOut, "MB%d,ME%d,MN%d",
                                startPulses, endPulses, npulses+1);
                %%writeOnly(ssId, pVar, pVar->stringOut);
            }

            strcpy(buildMessage, "Verifying trajectory");
            pvPut(buildMessage);
            strcpy(stringOut, "VC");
            %%writeOnly(ssId, pVar, pVar->stringOut);

            %%writeRead(ssId, pVar, "TB");

            if (stringIn[2] == '@') {
                buildStatus = 1;
                strcpy(buildMessage, " ");
            } else {
                buildStatus = 2;
                strncpy(buildMessage, stringIn, 40 -1);
            }

            for (j=0; j<numAxes; j++) {
                p = stringIn;


                if (!moveAxis[j]) continue;



                sprintf(stringOut, "%dRC1", j+1);
                %%writeRead(ssId, pVar, pVar->stringOut);
                motorMDVE[j] = atoi(p+3);
                pvPut(motorMDVE[j]);
                sprintf(stringOut, "%dRC2", j+1);
                %%writeRead(ssId, pVar, pVar->stringOut);
                motorMDVA[j] = atof(p+3);
                pvPut(motorMDVA[j]);

                sprintf(stringOut, "%dRC3", j+1);
                %%writeRead(ssId, pVar, pVar->stringOut);
                motorMVE[j] = atoi(p+3);
                pvPut(motorMVE[j]);
                sprintf(stringOut, "%dRC4", j+1);
                %%writeRead(ssId, pVar, pVar->stringOut);
                motorMVA[j] = atof(p+3);
                pvPut(motorMVA[j]);

                sprintf(stringOut, "%dRC5", j+1);
                %%writeRead(ssId, pVar, pVar->stringOut);
                motorMAE[j] = atoi(p+3);
                pvPut(motorMAE[j]);
                sprintf(stringOut, "%dRC6", j+1);
                %%writeRead(ssId, pVar, pVar->stringOut);
                motorMAA[j] = atof(p+3);
                pvPut(motorMAA[j]);
            }

            buildState = 0;
            pvPut(buildState);
            pvPut(buildStatus);
            pvPut(buildMessage);


            build=0;
            pvPut(build);
        } state monitor_inputs
    }


    state execute {
        when () {

            execState = 1;
            pvPut(execState);

            execStatus = 0;
            pvPut(execStatus);

            for (j=0; j<numAxes; j++) initialPos[j] = epicsMotorPos[j];

            if (moveMode == 1) {
                for (j=0; j<numAxes; j++) {
                    if (!moveAxis[j]) continue;
                    epicsMotorPos[j] = motorTrajectory[j][0];
                    pvPut(epicsMotorPos[j]);
                }
                %%waitEpicsMotors(ssId, pVar);
            }


            sprintf(stringOut, "LS,%dEC%f",simMode,timeScale);
            %%writeOnly(ssId, pVar, pVar->stringOut);

            startTime = time(0);
            execState = 2;
            pvPut(execState);

            epicsThreadSleep(0.1);
        } state wait_execute
    }



    state wait_execute {
        when (execStatus == 3) {



            execState = 0;
            pvPut(execState);


            execute=0;
            pvPut(execute);
        } state monitor_inputs

        when(execState==2) {

            %%getMotorPositions(ssId, pVar, pVar->motorCurrent);
            for (j=0; j<numAxes; j++) pvPut(motorCurrent[j]);

            %%writeRead(ssId, pVar, "XC1");

            ncomplete = atoi(&stringIn[2]);
            sprintf(execMessage, "Executing element %d/%d",
                                        ncomplete, nelements);
            pvPut(execMessage);
            %%pVar->anyMoving = getMotorMoving(ssId, pVar);
            if (!anyMoving) {
                execState = 3;
                execStatus = 1;
                strcpy(execMessage, " ");
            }

            if (difftime(time(0), startTime) > expectedTime*timeScale*2.) {
                execState = 3;
                execStatus = 4;
                strcpy(execMessage, "Timeout");
            }

            %%writeRead(ssId, pVar, "TB");


            if (stringIn[2] != '@') {
                execState = 3;
                execStatus = 2;
                strncpy(execMessage, stringIn, 40 -1);
            }
        } state wait_execute

        when(execState==3) {
            pvPut(execState);
            pvPut(execStatus);
            pvPut(execMessage);

            %%getMotorPositions(ssId, pVar, pVar->motorCurrent);
            for (j=0; j<numAxes; j++) pvPut(motorCurrent[j]);
            for (j=0; j<numAxes; j++) {
                if (!moveAxis[j]) continue;
                epicsMotorPos[j] = motorCurrent[j];
                pvPut(epicsMotorPos[j]);
            }
            %%waitEpicsMotors(ssId, pVar);
            execState = 0;
            pvPut(execState);


            execute=0;
            pvPut(execute);
        } state monitor_inputs
    }



    state readback {
        when() {

            readState = 1;
            pvPut(readState);
            readStatus=0;
            pvPut(readStatus);

            for (j=0; j<numAxes; j++) {
                for (i=0; i<2000; i++) {
                    motorReadbacks[j][i] = 0.;
                    motorError[j][i] = 0.;
                }
            }

            %%writeRead(ssId, pVar, "NQ");

            nactual = atoi(&stringIn[2]);
            pvPut(nactual);

            for (i=0; i<nactual; i++) {
                sprintf(readMessage, "Reading point %d/%d", i+1, nactual);
                pvPut(readMessage);
                sprintf(stringOut, "%dTQ", i+1);
                %%writeRead(ssId, pVar, pVar->stringOut);


                tok_save = 0;

                p = epicsStrtok_r(stringIn, ",", &tok_save);
                for (j=0; (j<numAxes && p!=0); j++) {
                    p = epicsStrtok_r(0, ",", &tok_save);
                    posTheory = atof(p+3);
                    p = epicsStrtok_r(0, ",", &tok_save);
                    if (epicsMotorDir[j] == 0) dir=1; else dir=-1;
                    posActual = atof(p+3);
                    motorError[j][i] = posActual-posTheory;

                    posActual = posActual*dir + epicsMotorOff[j];
                    motorReadbacks[j][i] = posActual;
                }
            }

            for (j=0; j<numAxes; j++) {
                pvPut(motorReadbacks[j]);
                pvPut(motorError[j]);
            }

            readState = 0;
            pvPut(readState);

            readStatus = 1;
            pvPut(readStatus);
            strcpy(readMessage, " ");
            pvPut(readMessage);


            readback=0;
            pvPut(readback);
        } state monitor_inputs
    }
}






ss trajectoryAbort {
    state monitorAbort {
        when (efTestAndClear(abortMon) && (abort==1)) {

            strcpy(stringOut,"AB");
            %%writeOnly(ssId, pVar, pVar->stringOut);
            execStatus = 3;
            pvPut(execStatus);
            strcpy(execMessage, "Motion aborted");
            pvPut(execMessage);


            abort=0;
            pvPut(abort);
        } state monitorAbort
    }
}



%{


static int writeOnly(SS_ID ssId, struct UserVar *pVar, char *command)
{
    asynStatus status;
    size_t nwrite;
    char buffer[256];


    strncpy(buffer, command, 256 -3);
    strcat(buffer, "\r");
    status = pasynOctetSyncIO->write((asynUser *)pVar->pasynUser, buffer,
                               strlen(buffer), 1.0, &nwrite);
    return(status);
}



static int writeRead(SS_ID ssId, struct UserVar *pVar, char *command)
{
    asynStatus status;
    size_t nwrite, nread;
    int eomReason;
    char buffer[256];


    strncpy(buffer, command, 256 -3);
    strcat(buffer, "\r");

    status = pasynOctetSyncIO->writeRead((asynUser *)pVar->pasynUser, buffer,
                               strlen(buffer), pVar->stringIn, 256,
                               30.0, &nwrite, &nread, &eomReason);
    return(status);
}



static int getMotorPositions(SS_ID ssId, struct UserVar *pVar, double *pos)
{
    char *p, *tok_save;
    int j;
    int dir;


    writeRead(ssId, pVar, "TP");


    tok_save = 0;
    p = epicsStrtok_r(pVar->stringIn, ",", &tok_save);
    for (j=0; (j<pVar->numAxes && p!=0); j++) {
        if (pVar->epicsMotorDir[j] == 0) dir=1; else dir=-1;
        pos[j] = atof(p+3)*dir + pVar->epicsMotorOff[j];
        p = epicsStrtok_r(0, ",", &tok_save);
    }
    return(0);
}





static int getMotorMoving(SS_ID ssId, struct UserVar *pVar)
{
    char *p, *tok_save;
    int j;
    int result=0, mask=0x01;


    writeRead(ssId, pVar, "MS");


    tok_save = 0;
    p = epicsStrtok_r(pVar->stringIn, ",", &tok_save);
    for (j=0; (j<pVar->numAxes && p!=0); j++) {

        if (*(p+3) & 0x01) result |= mask;
        mask = mask << 1;
        p = epicsStrtok_r(0, ",", &tok_save);
    }
    return(result);
}




static int getEpicsMotorMoving(SS_ID ssId, struct UserVar *pVar)
{
    int j;
    int result=0, mask=0x01;

    for (j=0; j<pVar->numAxes; j++) {
        seq_pvGet(ssId, pVar->epicsMotorDoneIndex[j], 0);
        if (pVar->epicsMotorDone[j] == 0) result |= mask;
        mask = mask << 1;
    }
    return(result);
}




static int waitEpicsMotors(SS_ID ssId, struct UserVar *pVar)
{
    int j;



    while(getEpicsMotorMoving(ssId, pVar)) {

        for (j=0; j<pVar->numAxes; j++) {
            pVar->motorCurrent[j] = pVar->epicsMotorPos[j];
            seq_pvPut(ssId, pVar->motorCurrentIndex[j], 0);
        }
        epicsThreadSleep(0.1);
    }
    for (j=0; j<pVar->numAxes; j++) {
        pVar->motorCurrent[j] = pVar->epicsMotorPos[j];
        seq_pvPut(ssId, pVar->motorCurrentIndex[j], 0);
    }
    return(0);
}

}%
