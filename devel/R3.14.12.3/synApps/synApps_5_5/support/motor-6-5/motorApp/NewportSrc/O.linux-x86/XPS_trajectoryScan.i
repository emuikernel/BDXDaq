# 1 "../XPS_trajectoryScan.st"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "../XPS_trajectoryScan.st"
program XPS_trajectoryScan("P=13BMC:,R=traj1,IPADDR=164.54.160.34,PORT=5001,"
                           "USERNAME=Administrator,PASSWORD=Administrator,"
                           "M1=m1,M2=m2,M3=m3,M4=m4,M5=m5,M6=m6,M7=m7,M8=m8,"
                           "GROUP=g1,"
                           "P1=p1,P2=p2,P3=p3,P4=p4,P5=p5,P6=p6,P7=p7,P8=p8")
# 19 "../XPS_trajectoryScan.st"
%% #include <string.h>
%% #include <stdio.h>
%% #include <math.h>
%% #include <stdarg.h>
%% #include "XPS_C8_drivers.h"
%% #include "Socket.h"
%% #include "xps_ftp.h"



option +r;
# 45 "../XPS_trajectoryScan.st"
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
# 46 "../XPS_trajectoryScan.st" 2
# 75 "../XPS_trajectoryScan.st"
int status;
int i;
int j;
int k;
int anyMoving;
int ncomplete;
int nextra;
int npoints;
int dir;
int pollSocket;
int driveSocket;
int abortSocket;
int positionSocket;
int xpsStatus;
int count;
double dtime;
double posActual;
double posTheory;
double expectedTime;
double initialPos[8];
double trajVel;
double preDistance[8];
double postDistance[8];
double motorLowLimit[8];
double motorHighLimit[8];
double motorMinPos[8];
double motorMaxPos[8];
double pulseTime;
double pulsePeriod;
char groupName[100];
char xpsAddress[100];
char *axisName[8];
char macroBuf[100];
char motorName[100];
char userName[100];
char password[100];
int xpsPort;





int motorCurrentIndex[8];
int epicsMotorDoneIndex[8];



unsigned long startTime;


%% static int getMotorPositions(SS_ID ssId, struct UserVar *pVar, double *pos);
%% static int getMotorMoving(SS_ID ssId, struct UserVar *pVar);
%% static int waitMotors(SS_ID ssId, struct UserVar *pVar);
%% static int getEpicsMotorMoving(SS_ID ssId, struct UserVar *pVar);
%% static int waitEpicsMotors(SS_ID ssId, struct UserVar *pVar);

%% static int getSocket(SS_ID ssId, struct UserVar *pVar, double timeout);
%% static void trajectoryExecute(SS_ID ssId, struct UserVar *pVar);
%% static void buildAndVerify(SS_ID ssId, struct UserVar *pVar);
%% static int currentElement(SS_ID ssId, struct UserVar *pVar);
%% static int getGroupStatus(SS_ID ssId, struct UserVar *pVar);
%% static void readGathering(SS_ID ssId, struct UserVar *pVar);
%% static int trajectoryAbort(SS_ID ssId, struct UserVar *pVar);
%% static void printMessage(const char *pformat, ...);


ss xpsTrajectoryScan {


    state init {
        when() {

            strcpy(groupName, macValueGet("GROUP"));
            strcpy(xpsAddress, macValueGet("IPADDR"));
            strcpy(userName, macValueGet("USERNAME"));
            strcpy(password, macValueGet("PASSWORD"));
            xpsPort = atoi(macValueGet("PORT"));
            if (debugLevel > 0) {
                printf("XPS_trajectoryScan: ss xpsTrajectoryScan: state init:"
                       " xpsAddress=%s groupName=%s\n",
                       xpsAddress, groupName);
            }
            for (i=0; i<numAxes; i++) {
                axisName[i] = malloc(100);
                sprintf(macroBuf, "P%d", i+1);
                sprintf(axisName[i], "%s.%s", groupName, macValueGet(macroBuf));
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
            xpsStatus = 0;
            ncomplete = 1;


            for (i=0; i<2000; i++) {
                for (j=0; j<8; j++) {
                    motorTrajectory[j][i] = 0.0;
                }
            }


            if (numAxes > 8) numAxes = 8;


            %%pVar->pollSocket = getSocket(ssId, pVar, 1.0);
            %%pVar->driveSocket = getSocket(ssId, pVar, 100000.);
            %%pVar->abortSocket = getSocket(ssId, pVar, 10.);

            for (j=0; j<numAxes; j++) {
                motorCurrentIndex[j] = pvIndex(motorCurrent[j]);
                epicsMotorDoneIndex[j] = pvIndex(epicsMotorDone[j]);
            }


            efClear(buildMon);
            efClear(executeMon);
            efClear(abortMon);
            efClear(readbackMon);
            efClear(nelementsMon);
        } state monitor_inputs
    }



    state monitor_inputs {
        entry {
            if (debugLevel > 0) {
                printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state monitor_inputs:"
                       " entry\n");
            }
        }
        when(efTestAndClear(buildMon) && (build==1)) {
        } state build

        when(efTestAndClear(executeMon) && (execute==1)
                && (buildStatus == 1)){
        } state execute

        when(efTestAndClear(readbackMon) && (readback==1)
                && (execStatus == 1)) {
        } state readback

        when(efTestAndClear(nelementsMon) && (nelements>=1)) {



            if (moveMode == 0)
                endPulses = nelements;
            else
                endPulses = nelements-1;
            pvPut(endPulses);
        } state monitor_inputs

    }



    state build {
        entry {
            if (debugLevel > 0) {
                printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state build:"
                       " entry\n");
            }
        }
        when() {

            buildState = 1;
            pvPut(buildState);
            buildStatus=0;
            pvPut(buildStatus);

            if (moveMode == 0)
                npoints = nelements;
            else
                npoints = nelements-1;


            if (timeMode == 0) {
                dtime = time/npoints;
                for (i=0; i<nelements; i++) timeTrajectory[i] = dtime;
                pvPut(timeTrajectory);
            }


            expectedTime=0;
            for (i=0; i<npoints; i++) {
                expectedTime += timeTrajectory[i];
            }

            %%buildAndVerify(ssId, pVar);


            for (j=0; j<numAxes; j++) {
                pvPut(motorMVA[j]);
                pvPut(motorMAA[j]);
            }

            buildState = 0;
            pvPut(buildState);
            pvPut(buildStatus);
            pvPut(buildMessage);


            build=0;
            pvPut(build);

            execStatus = 0;
            pvPut(execStatus);
            strcpy(execMessage, " ");

        } state monitor_inputs
    }


    state execute {
        entry {
            if (debugLevel > 0) {
                printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state execute:"
                       " entry\n");
            }
            %%waitMotors(ssId, pVar);


            for (j=0; j<numAxes; j++) initialPos[j] = epicsMotorPos[j];


            if (moveMode == 1) {
                for (j=0; j<numAxes; j++) {
                    if (moveAxis[j]) {
                        epicsMotorPos[j] = motorTrajectory[j][0] - preDistance[j];
                        pvPut(epicsMotorPos[j]);
                    }
                }
            } else {

                for (j=0; j<numAxes; j++) {
                    if (moveAxis[j]) {
                        epicsMotorPos[j] = epicsMotorPos[j] - preDistance[j];
                        pvPut(epicsMotorPos[j]);
                    }
                }
            }
            %%waitEpicsMotors(ssId, pVar);


            if (npulses > 0) {

                if (startPulses < 1) startPulses = 1;
                if (startPulses > nelements) startPulses = nelements;
                pvPut(startPulses);
                if (endPulses < startPulses) endPulses = startPulses;
                if (endPulses > nelements) endPulses = nelements;
                pvPut(endPulses);







                pulseTime=0;
                for (i=startPulses; i<=endPulses; i++) {
                    pulseTime += timeTrajectory[i-1];
                }
                pulsePeriod = pulseTime/npulses;
            } else {
                pulsePeriod = 0.;
            }


            for (j=0; j<numAxes; j++) {
                if (moveAxis[j]) {
                    status = PositionerUserTravelLimitsGet(pollSocket,
                                                           axisName[j],
                                                           &motorLowLimit[j],
                                                           &motorHighLimit[j]);
                    if ((epicsMotorPos[j] + motorMinPos[j]) < motorLowLimit[j]) {
                        execStatus = 2;
                        pvPut(execStatus);
                        sprintf(execMessage, "Low soft limit violation on motor %d", j);
                        pvPut(execMessage);
                    }
                    if ((epicsMotorPos[j] + motorMaxPos[j]) > motorHighLimit[j]) {
                        execStatus = 2;
                        pvPut(execStatus);
                        sprintf(execMessage, "High soft limit violation on motor %d", j);
                        pvPut(execMessage);
                    }
                }
            }
        }


        when (execStatus == 2) {


            if (debugLevel > 0) {
                printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state execute:"
                       " execStatus = STATUS_FAILURE\n");
            }
            execute=0;
            pvPut(execute);
        } state monitor_inputs


        when (execStatus != 2) {
            %%pVar->xpsStatus = getGroupStatus(ssId, pVar);

            if (debugLevel > 0) {
                printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state execute:"
                       " setting execState = EXECUTE_STATE_MOVE_START\n");
            }
            execState = 1;
            pvPut(execState);
            count = 0;
            while (execState != 2 &&
                   count < 100 ) {
                epicsThreadSleep(0.1);
                count++;
            }
            if (count == 100) {
                strcpy(execMessage, "Exec Timeout!");
                pvPut(execMessage);
                execStatus = 3;
                pvPut(execStatus);
            }
            count = 0;

            while (xpsStatus != 45 &&
                   count < 100 &&
                   execStatus != 2 ) {
                epicsThreadSleep(0.1);
                count++;
                %%pVar->xpsStatus = getGroupStatus(ssId, pVar);
            }
            if (count == 100) {
                strcpy(execMessage, "Exec Timeout!");
                pvPut(execMessage);
                execStatus = 3;
                pvPut(execStatus);
            }
            readStatus=0;
            pvPut(readStatus);
        } state wait_execute
    }



    state wait_execute {
        entry {
            if (debugLevel > 0) {
                printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state wait_execute:"
                       " entry\n");
            }
        }
        when (execStatus == 3) {
            if (debugLevel > 0) {
                printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state wait_execute:"
                       " execStatus = STATUS_ABORT\n");
            }



            execState = 0;
            pvPut(execState);


            execute=0;
            pvPut(execute);
        } state monitor_inputs

        when (delay(0.1) && execState==2) {

            %%getMotorPositions(ssId, pVar, pVar->motorCurrent);
            for (j=0; j<numAxes; j++) pvPut(motorCurrent[j]);

            %%pVar->xpsStatus = getGroupStatus(ssId, pVar);
            if (xpsStatus == 45) {
                %%pVar->ncomplete = currentElement(ssId, pVar);
                sprintf(execMessage, "Executing element %d/%d",
                        ncomplete, nelements);
                pvPut(execMessage);
            }
            else if (xpsStatus == 12) {

                execState = 3;
                execStatus = 1;
                strcpy(execMessage, " ");
            }
            else if (xpsStatus < 10) {

                execState = 3;
                execStatus = 2;
                sprintf(execMessage,"XPS Status Error=%d", xpsStatus);
            }

            if (difftime(time(0), startTime) > (expectedTime+10)) {
                execState = 3;
                execStatus = 4;
                strcpy(execMessage, "Timeout");
            }
        } state wait_execute

        when (execState==3) {
            if (debugLevel > 0) {
                printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state wait_execute:"
                       " execStatus = EXECUTE_STATE_FLYBACK\n");
            }
            pvPut(execState);
            pvPut(execStatus);
            pvPut(execMessage);


            if (execStatus == 1) {

                %%waitMotors(ssId, pVar);
                %%waitEpicsMotors(ssId, pVar);


                %%getMotorPositions(ssId, pVar, pVar->motorCurrent);
                for (j=0; j<numAxes; j++) pvPut(motorCurrent[j]);
                for (j=0; j<numAxes; j++) {
                    epicsMotorPos[j] = motorCurrent[j];
                    pvPut(epicsMotorPos[j]);
                }
                %%waitEpicsMotors(ssId, pVar);
            }

            execState = 0;
            pvPut(execState);


            execute=0;
            pvPut(execute);
        } state monitor_inputs
    }



    state readback {
        entry {
            if (debugLevel > 0) {
                printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state readback:"
                       " entry\n");
            }
        }
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

            %%readGathering(ssId, pVar);


            pvPut(nactual);


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






ss xpsTrajectoryAbort {
    state monitorAbort {
        when ((efTestAndClear(abortMon)) && (abort==1) &&
              (execState==2)) {

            execStatus = 3;
            if (debugLevel > 0) {
                printMessage("XPS_trajectoryScan: ss xpsTrajectoryAbort: state monitorAbort:"
                       " setting execStatus = STATUS_ABORT\n");
            }
            pvPut(execStatus);
            strcpy(execMessage, "Motion aborted");
            pvPut(execMessage);

            %%trajectoryAbort(ssId, pVar);



            abort=0;
            pvPut(abort);
        } state monitorAbort
    }
}




ss xpsTrajectoryRun {
    state asyncExecute {
        when (efTestAndClear(execStateMon) && (execState == 1)) {

            if (debugLevel > 0) {
                printMessage("XPS_trajectoryScan: ss xpsTrajectoryRun: state asyncExecute:"
                       " setting execState = EXECUTE_STATE_EXECUTING\n");
            }
            execState = 2;
            pvPut(execState);


            if (xpsStatus > 9 && xpsStatus < 20) {

                startTime = time(0);

                %%trajectoryExecute(ssId, pVar);
            } else {
                execStatus = 2;
                pvPut(execStatus);
            }
        } state asyncExecute
    }
}



ss xpsTrajectoryPosition {
    state positionUpdate {
        when (delay (1) && (execState == 0)) {
            %%getMotorPositions(ssId, pVar, pVar->motorCurrent);
            for (j=0; j<numAxes; j++) pvPut(motorCurrent[j]);
        } state positionUpdate
    }
}



%{




static int getMotorPositions(SS_ID ssId, struct UserVar *pVar, double *pos)
{
    int status;
    int j;
    int dir;



    status = GroupPositionCurrentGet(pVar->positionSocket,
                                     pVar->groupName,pVar->numAxes,pos);
    if (status != 0)
        printMessage("Error performing GroupPositionCurrentGet%i\n", status);

    for (j=0; j<pVar->numAxes; j++) {
        if (pVar->epicsMotorDir[j] == 0) dir=1; else dir=-1;
        pos[j] = pos[j]*dir + pVar->epicsMotorOff[j];
    }

    return(status);
}



static int getMotorMoving(SS_ID ssId, struct UserVar *pVar)
{
    int status;
    int moving=0;
    int groupStatus;



    status = GroupStatusGet(pVar->pollSocket,pVar->groupName,&groupStatus);
    if (status != 0)
        printMessage("Error performing GroupStatusGet %i\n",status);

    if (groupStatus > 42)
        moving = 1;

    return(moving);
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



static int waitMotors(SS_ID ssId, struct UserVar *pVar)
{
    int j;



    while(getMotorMoving(ssId, pVar)) {

        getMotorPositions(ssId, pVar, pVar->motorCurrent);
        for (j=0; j<pVar->numAxes; j++) seq_pvPut(ssId, pVar->motorCurrentIndex[j], 0);
        epicsThreadSleep(0.1);
    }
    getMotorPositions(ssId, pVar, pVar->motorCurrent);
    for (j=0; j<pVar->numAxes; j++) seq_pvPut(ssId, pVar->motorCurrentIndex[j], 0);
    return(0);
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


static int getSocket(SS_ID ssId, struct UserVar *pVar, double timeout)
{
    int sock = 0;

    sock = TCP_ConnectToServer(pVar->xpsAddress, pVar->xpsPort, timeout);
    if (sock < 0)
            printMessage("Error TCP_ConnectToServer, status=%d\n",sock);
    return (sock);
}



static void trajectoryExecute(SS_ID ssId, struct UserVar *pVar)
{
    int status;
    int j;
    char buffer[60 * 2 * 8];
    int eventId;

    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " entry\n");
    }



    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling GatheringReset(%d)\n", pVar->pollSocket);
    }
    status = GatheringReset(pVar->pollSocket);
    if (status != 0) {
        printMessage("Error performing GatheringReset, status=%d\n",status);
        return;
    }



    strcpy(buffer, "");
    for (j=0; j<pVar->numAxes; j++) {
        strcat (buffer, pVar->axisName[j]);
        strcat (buffer, ".SetpointPosition;");
        strcat (buffer, pVar->axisName[j]);
        strcat (buffer, ".CurrentPosition;");
    }



    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling GatheringConfigurationSet(%d, %d, %s)\n",
               pVar->pollSocket, pVar->numAxes*2, buffer);
    }
    status = GatheringConfigurationSet(pVar->pollSocket,
                                       pVar->numAxes*2, buffer);
    if (status != 0)
            printMessage("Error performing GatheringConfigurationSet, status=%d, buffer=%p\n",
                   status, buffer);
# 808 "../XPS_trajectoryScan.st"
    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling MultipleAxesPVTPulseOutputSet(%d, %s, %d, %d, %f)\n",
                                           pVar->pollSocket, pVar->groupName,
                                           pVar->startPulses+1,
                                           pVar->endPulses+1,
                                           pVar->pulsePeriod);
    }
    status = MultipleAxesPVTPulseOutputSet(pVar->pollSocket, pVar->groupName,
                                           pVar->startPulses+1,
                                           pVar->endPulses+1,
                                           pVar->pulsePeriod);


    sprintf(buffer, "Always;%s.PVT.TrajectoryPulse", pVar->groupName);
    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling EventExtendedConfigurationTriggerSet(%d, %d, %s, %s, %s, %s, %s)\n",
                                                  pVar->pollSocket, 2, buffer,
                                                  "", "", "", "");
    }
    status = EventExtendedConfigurationTriggerSet(pVar->pollSocket, 2, buffer,
                                                  "", "", "", "");
    if (status != 0) {
        printMessage("Error performing EventExtendedConfigurationTriggerSet, status=%d, buffer=%s\n",
               status, buffer);
        return;
    }


    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling EventExtendedConfigurationActionSet(%d, %d, %s, %s, %s, %s, %s)\n",
                                                 pVar->pollSocket, 1,
                                                 "GatheringOneData",
                                                 "", "", "", "");
    }
    status = EventExtendedConfigurationActionSet(pVar->pollSocket, 1,
                                                 "GatheringOneData",
                                                 "", "", "", "");
    if (status != 0) {
        printMessage("Error performing EventExtendedConfigurationActionSet, status=%d\n",
               status);
        return;
    }


    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling EventExtendedStart(%d, %p)\n",
                                            pVar->pollSocket, &eventId);
    }
    status= EventExtendedStart(pVar->pollSocket, &eventId);
    if (status != 0) {
        printMessage("Error performing EventExtendedStart, status=%d\n",status);
        return;
    }

    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling MultipleAxesPVTExecution(%d, %s, %s, %d)\n",
                                      pVar->driveSocket, pVar->groupName,
                                      pVar->trajectoryFile, 1);
    }
    status = MultipleAxesPVTExecution(pVar->driveSocket, pVar->groupName,
                                      pVar->trajectoryFile, 1);

    if ((status != 0) && (status != -27))
        printMessage("Error performing MultipleAxesPVTExecution, status=%d\n", status);


    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling EventExtendedRemove(%d, %d)\n", pVar->pollSocket, eventId);
    }
    status = EventExtendedRemove(pVar->pollSocket, eventId);
    if (status != 0) {
        printMessage("Error performing ExtendedEventRemove, status=%d\n",status);
        return;
    }


    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling GatheringStopAndSave(%d)\n", pVar->pollSocket);
    }
    status = GatheringStopAndSave(pVar->pollSocket);



    if ((status != 0) && (status != -30))
        printMessage("Error performing GatheringStopAndSave, status=%d\n", status);

    return;
}



static void buildAndVerify(SS_ID ssId, struct UserVar *pVar)
{
    FILE *trajFile;
    int i, j, status;
    int npoints;
    double trajVel;
    double D0, D1, T0, T1;
    int ftpSocket;
    char fileName[100];
    double distance;
    double maxVelocity[8], maxAcceleration[8];
    double minJerkTime[8], maxJerkTime[8];
    double preTimeMax, postTimeMax;
    double preVelocity[8], postVelocity[8];
    double time;
    int dir[8];

    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: buildAndVerify:"
               " entry\n");
    }
# 937 "../XPS_trajectoryScan.st"
    preTimeMax = 0.;
    postTimeMax = 0.;

    for (j=0; j<pVar->numAxes; j++) {
        preVelocity[j] = 0.;
        postVelocity[j] = 0.;
    }
    for (j=0; j<pVar->numAxes; j++) {
        if (!pVar->moveAxis[j]) continue;
        status = PositionerSGammaParametersGet(pVar->pollSocket, pVar->axisName[j],
                                               &maxVelocity[j], &maxAcceleration[j],
                                               &minJerkTime[j], &maxJerkTime[j]);
        if (status != 0) {
            printMessage("Error calling positionerSGammaParametersSet, status=%d\n",
                   status);
        }



        maxAcceleration[j] = 0.9 * maxAcceleration[j];



        if (pVar->moveMode == 0) {
            distance = pVar->motorTrajectory[j][0];
        } else {
            distance = pVar->motorTrajectory[j][1] - pVar->motorTrajectory[j][0];
        }
        preVelocity[j] = distance/pVar->timeTrajectory[0];
        time = fabs(preVelocity[j]) / maxAcceleration[j];
        if (time > preTimeMax) preTimeMax = time;
        if (pVar->moveMode == 0) {
            distance = pVar->motorTrajectory[j][pVar->nelements-1];
        } else {
            distance = pVar->motorTrajectory[j][pVar->nelements-1] -
                       pVar->motorTrajectory[j][pVar->nelements-2];
        }
        postVelocity[j] = distance/pVar->timeTrajectory[pVar->nelements-1];
        time = fabs(postVelocity[j]) / maxAcceleration[j];
        if (time > postTimeMax) postTimeMax = time;
    }



    for (j=0; j<pVar->numAxes; j++) {
        pVar->preDistance[j] = 0.5 * preVelocity[j] * preTimeMax;
        pVar->postDistance[j] = 0.5 * postVelocity[j] * postTimeMax;
    }


    trajFile = fopen (pVar->trajectoryFile, "w");



    for (j=0; j<pVar->numAxes; j++) {
        if (pVar->epicsMotorDir[j] == 0) dir[j]=1; else dir[j]=-1;
    }

    fprintf(trajFile,"%f", preTimeMax);
    for (j=0; j<pVar->numAxes; j++)
        fprintf(trajFile,", %f, %f", pVar->preDistance[j]*dir[j], preVelocity[j]*dir[j]);
    fprintf(trajFile,"\n");



    if (pVar->moveMode == 0)
        npoints = pVar->nelements;
    else
        npoints = pVar->nelements-1;
    for (i=0; i<npoints; i++) {
        T0 = pVar->timeTrajectory[i];
        if (i < npoints-1)
            T1 = pVar->timeTrajectory[i+1];
        else
            T1 = T0;
        for (j=0; j<pVar->numAxes; j++) {
            if (pVar->moveMode == 0) {
                D0 = pVar->motorTrajectory[j][i] * dir[j];
                if (i < npoints-1)
                    D1 = pVar->motorTrajectory[j][i+1] * dir[j];
                else
                    D1 = D0;
            } else {
                D0 = pVar->motorTrajectory[j][i+1] *dir[j] -
                           pVar->motorTrajectory[j][i] * dir[j];
                if (i < npoints-1)
                    D1 = pVar->motorTrajectory[j][i+2] * dir[j] -
                           pVar->motorTrajectory[j][i+1] * dir[j];
                else
                    D1 = D0;
            }


            trajVel = ((D0 + D1) / (T0 + T1));
            if (!(pVar->moveAxis[j])) {
                D0 = 0.0;
                trajVel = 0.0;
            }

            if (j == 0) fprintf(trajFile,"%f", pVar->timeTrajectory[i]);
            fprintf(trajFile,", %f, %f",D0,trajVel);
            if (j == (pVar->numAxes-1)) fprintf(trajFile,"\n");
        }
    }


    fprintf(trajFile,"%f", postTimeMax);
    for (j=0; j<pVar->numAxes; j++)
        fprintf(trajFile,", %f, %f", pVar->postDistance[j]*dir[j], 0.);
    fprintf(trajFile,"\n");
    fclose (trajFile);


    status = ftpConnect(pVar->xpsAddress, pVar->userName, pVar->password, &ftpSocket);
    if (status != 0) {
        printMessage("Error calling ftpConnect, status=%d\n", status);
        return;
    }
    status = ftpChangeDir(ftpSocket, "/Admin/public/Trajectories");
    if (status != 0) {
        printMessage("Error calling ftpChangeDir, status=%d\n", status);
        return;
    }
    status = ftpStoreFile(ftpSocket, pVar->trajectoryFile);
    if (status != 0) {
        printMessage("Error calling ftpStoreFile, status=%d\n", status);
        return;
    }
    status = ftpDisconnect(ftpSocket);
    if (status != 0) {
        printMessage("Error calling ftpDisconnect, status=%d\n", status);
        return;
    }


    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: buildAndVerify:"
               " calling MultipleAxesPVTVerification(%d, %s, %s)\n",
               pVar->pollSocket, pVar->groupName, pVar->trajectoryFile);
    }
    status = MultipleAxesPVTVerification(pVar->pollSocket, pVar->groupName,
                                         pVar->trajectoryFile);

    pVar->buildStatus = 2;
    if (status == 0) {
        strcpy(pVar->buildMessage, " ");
        pVar->buildStatus = 1;
    }
    else if (status == -69)
        strcpy(pVar->buildMessage, "Acceleration Too High");
    else if (status == -68)
        strcpy(pVar->buildMessage, "Velocity Too High");
    else if (status == -70)
        strcpy(pVar->buildMessage, "Final Velocity Non Zero");
    else if (status == -75)
        strcpy(pVar->buildMessage, "Negative or Null Delta Time");
    else
        sprintf(pVar->buildMessage, "Unknown trajectory verify error=%d", status);



    if (status == 0){
        pVar->buildStatus = 1;
    }
    if (1) {
        for (j=0; j<pVar->numAxes; j++) {
            status = MultipleAxesPVTVerificationResultGet(pVar->pollSocket,
                         pVar->axisName[j], fileName, &pVar->motorMinPos[j], &pVar->motorMaxPos[j],
                         &pVar->motorMVA[j], &pVar->motorMAA[j]);
            if (status != 0) {
                printMessage("Error performing MultipleAxesPVTVerificationResultGet for axis %s, status=%d\n",
                       pVar->axisName[j], status);
            }
        }
    } else {
        for (j=0; j<pVar->numAxes; j++) {
            pVar->motorMVA[j] = 0;
            pVar->motorMAA[j] = 0;
        }
    }

    return;
}



static int currentElement(SS_ID ssId, struct UserVar *pVar)
{
    int status;
    int number;
    char fileName[100];

    status = MultipleAxesPVTParametersGet(pVar->pollSocket,
                 pVar->groupName, fileName, &number);
    if (status != 0)
        printMessage("Error performing MultipleAxesPVTParametersGet, status=%d\n",
               status);
    return (number);
}

static int getGroupStatus(SS_ID ssId, struct UserVar *pVar)
{
    int status;
    int groupStatus;



    status = GroupStatusGet(pVar->pollSocket,pVar->groupName,&groupStatus);
    if (status != 0)
        printMessage("Error performing GroupStatusGet, status=%d\n", status);
    return(groupStatus);
}





static void readGathering(SS_ID ssId, struct UserVar *pVar)
{
    char buffer[60 * 2 * 8];
    FILE *gatheringFile;
    int i,j;
    int nitems;
    double setpointPosition, actualPosition;
    int ftpSocket;
    int status;
    int dir;

    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: readGathering:"
               " entry\n");
    }

    status = ftpConnect(pVar->xpsAddress, pVar->userName, pVar->password, &ftpSocket);
    if (status != 0) {
        printMessage("Error calling ftpConnect, status=%d\n", status);
        return;
    }
    status = ftpChangeDir(ftpSocket, "/Admin/public/");
    if (status != 0) {
        printMessage("Error calling ftpChangeDir, status=%d\n", status);
        return;
    }
    status = ftpRetrieveFile(ftpSocket, "Gathering.dat");
    if (status != 0) {
        printMessage("Error calling ftpRetrieveFile, status=%d\n", status);
        return;
    }
    status = ftpDisconnect(ftpSocket);
    if (status != 0) {
        printMessage("Error calling ftpDisconnect, status=%d\n", status);
        return;
    }

    gatheringFile = fopen("Gathering.dat", "r");


    for (i=0; i<2; i++) {
        fgets (buffer, 60 * 2 * 8, gatheringFile);
    }
# 1207 "../XPS_trajectoryScan.st"
    for (i=0; i<pVar->npulses; i++) {
        for (j=0; j<pVar->numAxes; j++) {


            nitems = fscanf(gatheringFile, "%lf %lf ",
                            &setpointPosition, &actualPosition);
            if (nitems != 2) {
                printMessage("Error reading Gathering.dat file, nitems=%d, should be %d\n",
                       nitems, 2);
                goto done;
            }
            pVar->motorError[j][i] = actualPosition - setpointPosition;

            if (pVar->epicsMotorDir[j] == 0) dir=1; else dir=-1;
            pVar->motorReadbacks[j][i] = actualPosition * dir + pVar->epicsMotorOff[j];
        }
    }

    done:
    fclose (gatheringFile);
    pVar->nactual = i;

    return;
}


static int trajectoryAbort(SS_ID ssId, struct UserVar *pVar)
{
    int status;

    status = GroupMoveAbort(pVar->abortSocket,pVar->groupName);
    if (status != 0)
        printMessage("Error performing GroupMoveAbort, status=%d\n", status);

    return (0);
}

static void printMessage(const char *pformat, ...)
{
    va_list pvar;
    FILE *fp = stdout;
    epicsTimeStamp now;
    char nowText[40];

    epicsTimeGetCurrent(&now);
    nowText[0] = 0;
    epicsTimeToStrftime(nowText,sizeof(nowText),
         "%Y/%m/%d %H:%M:%S.%03f",&now);
    fprintf(fp,"%s ",nowText);
    va_start(pvar, pformat);
    vfprintf(fp,pformat,pvar);
    va_end(pvar);
    fflush(fp);
}

}%
