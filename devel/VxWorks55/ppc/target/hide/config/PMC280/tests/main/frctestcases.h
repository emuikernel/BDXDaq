/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************
*                                                                             *
* Filename:frctestcaes.h                                                      *
*                                                                             *
* DESCRIPTION:	                                                              *
*       This file contains all function declarations ,global variables structures etc.      *
*                                                                          *
* DEPENDENCIES:	                                                              *
*       None.                                                                 *
*                                                                             *
******************************************************************************/

#ifndef __FRC_TESTCASES_H
#define __FRC_TESTCASES_H

#include "frctestconfig.h"


/*****************************Structures *************************************/

typedef union frc_frequency_structure
{
    unsigned char byte;
    struct
   {
    unsigned char freq:5;
    unsigned char junk:3;
   }BIT;
}FRC_FREQ_STRUCT;


typedef int (*TEST_FUNCPTR)(void);

typedef struct regression_test_info
{
   char testName[50];
   char funcName[30];
   TEST_FUNCPTR funcPtr;
   int  testStatus;
}REGR_TEST_INFO;

/******************Function declarations *************************************/
int getOption(void);
int frcTestDispFreq(void);
int frcTestWhoAmI(void);
int frcTestChkECC(void);
int frcTestIDMAScrubChk(unsigned int stAddr, unsigned int endAddr);


/*memory performance----mem_perf*/
int testmem_perf(void); /*got from tyne*/
int frcTestMemPerf(void);

/*Serial port*/
int frcTestSerialInfo();
int frcTestSerial(void);
int frcTestPCI(void);
int frcTestSubsysVendID(void);
int frcTestEthernet(void);
int frcTestIntLatency(void);
int frcTestBootFlash(void);

int frcTestDispReg(void);
int frcTestDMA(void);
int  frcTestDMATests(void);
int frcTestDualCPU(void);
int frcTestAuxClk(void);
int frcTestI2C_EEPROM(void);
int frcTestBIB(void);
int frcTestVPD(void);
int frcTestRTC(void);
int frcTestUserFlash(void);
int frcTestTAU(void);
int frcTestBaudRate(void);
int frcTestAllTests(void);
int frcTestSummary(void);
int frcTestIsMonarch(void);
int frcTestEnableInt_Non_monarch(void);
int frcTestMon_NonMon(void);
int  frcTestMACAddr(void);
STATUS frcTestHostOS ( void);



#endif /*__FRC_TESTCASES_H */
