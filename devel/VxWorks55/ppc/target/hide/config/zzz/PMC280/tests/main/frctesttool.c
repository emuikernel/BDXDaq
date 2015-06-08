/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************
*                                                                             *
* Filename    :frctesttool.c                                                  *
*                                                                             *
* DESCRIPTION :	                                                              *
*       This file implements menu for the tests.                              *
*                                                                             *
* DEPENDENCIES:	                                                              *
*       None.                                                                 *
*                                                                             *
******************************************************************************/

/*****************************Includes ****************************************/
#include "frctesttool.h"
#include "frctestcases.h"

#include "memtest_drv.h"
#include <stdlib.h>
#include <taskLib.h>
/********************Macros to enable/disable tests****************************/
#define MEMORY_TEST
#define BOOT_FLASH_TEST
IMPORT void *strcmp();
IMPORT void frcPrintBoardInfo();
IMPORT int watchdogTest (void);

 STATUS frcTestUserFlash(void);
STATUS frcTestDispMemMap(void);
STATUS frcDispBoardInfo ( void);
int frcTestPciScan(void);
int frcTestAltivec(void);


struct TEST_CMD
{
char summary[35];
int (*func)(void);
};
/*****************************Global Variables*********************************/

/*******************************************************************************
* Function:    frcSysTest 
* Description: This is a menu driven program for selecting and executing 
*              other tests.          
*
* Inputs:     None.
* Output:     A list of tests which can be used to select the test.
*******************************************************************************/

#if 0
int 
frcSysTest(void)
{
   unsigned int option  =0; /*to select the options from main list*/
   unsigned int idmaStAddr=0x0,idmaEndAddr=0x0;/*This is for IDMA Scrub Test*/
/*   unsigned char cpuOp  =0; *//*to select the options from CPU list*/
   char str[20];

  printf("!!!!!Caution!!!!!\n");
  printf("Wrong data entered for any Input, will take the control out of the current menu.\n");	

   while(1)
   {
       printf("\n\n===================Select test to be performed============\n\n");
#ifdef  WHOAMI      
printf("Who Am I                    ======>1   ");
#endif
#if 0
	   printf("Auxiliary Clock Tests       ======>14\n");
    
   printf("Display CPU frequency       ======>2   ");
#endif
	   printf("I2C/EEPROM Tests            ======>15\n");
       printf("Serial ports settings       ======>3   ");
	   printf("BIB Tests                   ======>16\n");
       printf("ECC Test                    ======>4   ");
#if 0
	   printf("VPD Tests                   ======>17\n");
#endif
       printf("IDMA Scrub Test             ======>5   ");
	
#if 0
   printf("User Flash Tests            ======>18\n");
#endif
	printf("\nMemory Tests                ======>6   ");
#if 0
	   printf("TAU Tests                   ======>19\n");
#endif
       printf("Memory perf Tests           ======>7   ");
	   printf("\nInterrupt Latency Tests     ======>20\n");
       printf("PCI Tests                   ======>8   ");
     
#ifndef PMC280_NORTEL_PASSPORT_INTEGRATION_CHANGE
  printf("Boot Flash Tests            ======>21\n");
#endif
	printf("SubsysID & subsys vendorID  ======>9  \n ");
       printf("RTC Tests                   ======>22\n");
       printf("Ethernet Tests              ======>10  ");
       printf("Monarch/Non-Moanrch Tests   ======>23\n");
       printf("Regparse tool(MV64360/PCI-0)======>11  ");
       printf("Display Memory Map          ======>24\n");
       printf("IDMA Tests                  ======>12  ");
       printf("MAC Programming/Display     ======>25\n");
       printf("Dual CPU tests              ======>13  ");

       printf("\n\nEXIT            ======> 0\n\nOption:");

       scanf("%s",str);
       option = atoi(str);

       switch(option)
       {
#ifdef WHOAMI
       case 1:
           printf("option-> 1\n");

           frcTestWhoAmI();

           break;
#endif
#if 0
       case 2:
           printf("option-> 2\n");

           frcTestDispFreq();

           break;
#endif
       case 3:
           printf("option-> 3\n");
           frcTestSerial();
           break;

       case 4:
           printf("option-> 4\n");
           frcTestChkECC();
           break;

       case 5:
           printf("option-> 5\n");
           printf("Enter the start address for IDMA Scrub test in HEX:");
           scanf("%x",&idmaStAddr);
           printf("Enter the End address for IDMA Scrub test in HEX:");
           scanf("%x",&idmaEndAddr);

           frcTestIDMAScrubChk(idmaStAddr,idmaEndAddr);
           break;

       case 6:
           printf("option-> 6\n");		  
#ifdef MEMORY_TEST
           printf("!!!!Caution!!!!\n");
           printf("Execute the function showMap() or selct the option 24 from main Menu \n and get the safe range of memory on which these tests can be conducted\n\n");\
           	
           frcTestMemTest();
           
#else
	   printf("This Test is not supported\n");
#endif
           break;

       case 7:
           printf("option-> 7\n");
           frcTestMemPerf();
           break;

       case 8:
           printf("option-> 8\n");		  
           frcTestPCI();
           break;

       case 9:
           printf("option-> 9\n");
           frcTestSubsysVendID();
           break;

       case 10:
           printf("option-> 10\n");		  
		   frcTestEthernet();
           break;

        case 11:
           printf("option-> 11\n");
           frcTestDispReg();
           break;

       case 12:
           printf("option-> 12\n");		  
           frcTestDMATests();
           break;

       case 13:
           printf("option-> 13\n"); 
           frcTestDualCPU();
           break;
#if 0
       case 14:
           printf("option-> 14\n");
           frcTestAuxClk();
           break;
#endif
       case 15:
           printf("option-> 15\n");
           frcTestI2C_EEPROM();
           break;

       case 16:
           printf("option-> 16\n");
           frcTestBIB();
           break;

       case 17:
           printf("option-> 17\n");
           frcTestVPD();
           break;

#if 0
       case 18:
           printf("option-> 18\n");
           frcTestUserFlash();
           break;

       case 19:
           printf("option-> 19\n");
           frcTestTAU();
           break;
#endif
       case 20:
           printf("option-> 20\n");
	       frcTestIntLatency();
           break;

#if 0
       case 21:   
           printf("option-> 21\n");
		   frcTestBootFlash();
           break;

#endif
       case 22:           
           printf("option-> 22\n");
		   frcTestRTC();
           break;

       case 23:           
           printf("option-> 23\n");
		   frcTestMon_NonMon();
           break;

       case 24:
           printf("option-> 24\n");
           showMap();
           break;

       case 25:
           printf("option-> 25\n");
           frcTestMACAddr();
           break;
	 
       default:
           printf("Please select correct option\n");
           break;          

       case 0:
           printf("option-> 0\n");		  
           return 0;

       }
     
    }
return 0;
}
#endif
int fst()
{
   frcSysTest();
   return 0;
}


int 
frcSysTest(void)
{
unsigned int i,option  =0; /*to select the options from main list*/
struct TEST_CMD test_cmd[] = {

  /*Host OS support */
  #ifdef HOST_OS_TEST 
{"Host OS",&frcTestHostOS},	
#endif
 /******/

  
 /*Board Initialization*/
#if 0 /*def DISP_FREQ_TEST*/
{"Display CPU frequency",&frcTestDispFreq},
#endif
{"PMC-280 board information",&frcDispBoardInfo },
{"Memory map/Partitioning",&frcTestDispMemMap},
/***********/


{"Register dispaly",&frcTestDispReg},
#if 0
{"Interrupt Routing",NULL},	
#endif
{"Sub-system ID",&frcTestSubsysVendID},
{"Monarch/Non-moanrch ",&frcTestIsMonarch },
#if 0
/*For Variant B and F Interrupt handling in Non-Monarch mode*/
{"Enable PCI interrupt handling in Non-Monarch",NULL },
#endif
{"ECC test",&frcTestChkECC},
{"PCI scanning",&frcTestPciScan },

{"Memory test",&frcTestMemTest},
{"Memory bench mark",&frcTestMemPerf},

#if 0
{"DDR Access from PCI",NULL},	
#endif

{"AltiVec support",&frcTestAltivec},	

/* MAC address test */
{"On-board MAC programming",&frcTestMACAddr},

/******/

	




{"Ethernet test",&frcTestEthernet},
#ifdef WATCHDOG_TEST
{"Watchdog test",&watchdogTest},
#endif
{"DMA test",&frcTestDMA },
{"Who Am I?",&frcTestWhoAmI},
{"Multiprocessor test",&frcTestDualCPU},
/******/



/* Extra tests*/

#if 0
{"Interrupt latency test", &frcTestIntLatency},
#endif
/*All set of test*/
{"Serial Port tests",&frcTestSerial},

#ifdef BOOT_FLASH_TEST 
{"Program Boot Flash", &frcTestBootFlash},
#endif
{"User Flash",&frcTestUserFlash},
{"BIB test",&frcTestBIB},
{"RTC test",&frcTestRTC},
{"VPD test",&frcTestVPD},


{"",NULL}
};



frcPrintBoardInfo();

   while(1)
   {
       printf("\n\n   ===================Select test to be performed========================\n\n");

	i = 0;
	while ( strcmp (test_cmd[ i].summary , "") != 0)
	{
		printf("     %-25s ===>  %d",test_cmd[i].summary,i+1);
		if(i%2)
			printf("\n");
		i++;
	} 
	
       printf ( "\n     Exit   :0");  
	printf("\nYour option:");
	/*scanf("%d" , &option);*/
option=getOption ();


	if ( option > i )
		{
		printf("\n Out of range test %d",option);
		continue;
		}
	else if ( option == 0)
		break;
	else
		{
		if ( test_cmd[option-1].func != NULL)
			(test_cmd[option-1].func)();
		else
			printf ("\nTool not supported in EAU release.");
		}

    }

return OK;
}


#if 0 
/**************************************************/
int frcTestIDMAScrub(void)
{
unsigned int idmaStAddr=0x0,idmaEndAddr=0x0;/*This is for IDMA Scrub Test*/
           printf("Enter the start address for IDMA Scrub test in HEX:");
           scanf("%x",&idmaStAddr);
           printf("Enter the End address for IDMA Scrub test in HEX:");
           scanf("%x",&idmaEndAddr);
return  frcTestIDMAScrubChk(idmaStAddr,idmaEndAddr);

}
#endif
