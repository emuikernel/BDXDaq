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

*******************************************************************************
* watchDogTest.c -  This file is to test the functionality of watch Dog timer  
*                   It is for Testing purpose.   
******************************************************************************/

#include <stdio.h>
#include <taskLib.h>

#include "./../../coreDrv/vxWatchDog.h"
#include "./../../gtIntControl.h"
/*#include "./../../sysLib.h"*/

bool watchdogFlag = false;
int watchdogAppId =-1;
int wdgService=0;
unsigned int wdgCnt=0x0, wdgNMICnt =0x0;
int frcTestWdInitConnect=0;
int frcTestWdInitDisconnect =1;
  
IMPORT bool frcWatchdogLoad(unsigned int) ;
IMPORT bool frcWatchdogNMILoad(unsigned int);
IMPORT void frcWatchdogEnable();
IMPORT void frcWatchdogDisable();
IMPORT STATUS gtIntDisconnect(GT_INT_CAUSE cause);
IMPORT int getOption (void);
IMPORT int sysClkRateGet();

/******************************************************************************
* Function     : watchDogIsr
* Description  : This is ISR for watchDog Timer NMI (WD_NMI).
* Inputs       : Interrupt parameter
* Outputs      : None
* Returns      : None
******************************************************************************/
#undef WDG_TEST_DBG
void watchDogApp() {
    unsigned int numSec=0x0, numTicks=0x0, wdgCnt1=0x0;
    while(1)
    {
        wdgCnt1 = (wdgCnt << 8) + 0xff;
        numSec = ((wdgCnt1 - wdgNMICnt)/0x8000000); /*Convert the count difference into seconds*/
        if(numSec > 2)
        	numSec -=2;
        else
        {
        	printf("Watchdog period is too less, this can not be supported through this service\n"); 
        	taskDelete(taskIdSelf());
        }
        
        numTicks = numSec * sysClkRateGet();  /*Convert the seconds into ticks*/
        taskDelay(numTicks);
        
        #ifdef WDG_TEST_DBG
        printf("\nwdgCnt=%x",wdgCnt);
        printf("\nwdgCnt1=%x",wdgCnt1);
        printf("\nwdgNMICnt=%x",wdgNMICnt);
        printf("\ndelay in Seconds=%u",numSec);
        printf("\ndelay in Ticks=%u",numTicks);
        
        #endif /*WDG_TEST_DBG*/

        printf("Watchdog serviced\n");
        frcWatchdogService();
    }
}

void watchDogIsr(int i)
{
        printf("Watchdog serviced---Watchdog NMI ISR\n");
        frcWatchdogService();
}
/****************Application to service watchdog************************************/
	
void frcTestWatchDogAppStart(void)
{
      printf("\nWatchdog Service started\n");
       watchdogAppId = taskSpawn("frcTestWatchDogApplication", 5, 0, 10000, (FUNCPTR)watchDogApp, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}
void frcTestWatchDogAppStop(void)
{
    if(watchdogAppId != -1)
    {
    	taskDelete(watchdogAppId);
    	watchdogAppId =-1;
        wdgService =0;
       printf("\nWatchdog Service stopped\n");
    }
}

/******************************************************************************
* Function     : watchDogTest
* Description  : This is for testing the watchdog timer functionality.
*                It initialises the watchDog Timer, Loads the NMI and Counter
*                registers with proper values, and enables the Watch dog Timer
*                functionality.
*
* Inputs       : None
* Outputs      : None
* Returns      : None
******************************************************************************/

int watchdogTest (void) {

  unsigned int option = 0;

    /*Initialize watchdog*/
    if(!watchdogFlag) {
    	#if 0 /*modified as per code 0n 26th sept.03*/
        frcWatchdogInit ((VOIDFUNCPTR) watchDogIsr,0);
    	#endif
    	     frcWatchdogInit ();
      	     gtIntCtrlInit();
        watchdogFlag = true;
    }
    	
  while (1)
    {
      printf ("\n\n======Select the Option=====\n\n");
      printf ("Load Watchdog Preset Val  ======>1\n");
      printf ("Load Watchdog NMI Val     ======>2\n");
      printf ("Enable Watchdog           ======>3\n");
      printf ("Disable Watchdog          ======>4\n");
      printf ("Start Watchdog S/W Service    ======>5\n");
      printf ("Stop Watchdog S/W Service     ======>6\n");
      printf ("Enable NMI watchdog service ======>7\n");
      printf ("Disable NMI watchdog service ======>8\n");

      printf ("Return back to main menu      =====>0\n");

      option = getOption ();
      switch (option)
	{
	case 0:
	  return 0;

	case 1:
	  printf("Enter the Preset Val in Hex:");
	  scanf("%x",&wdgCnt);
	  
         frcWatchdogLoad(wdgCnt) ;
	  break;

	case 2:
	  printf("Enter the Watchdog NMI Val in Hex:");
	  scanf("%x",&wdgNMICnt);

         frcWatchdogNMILoad(wdgNMICnt);
	  break;

	case 3:
         if(wdgService)
             frcTestWatchDogAppStart();
         frcWatchdogEnable();
	  break;

	case 4:
          frcWatchdogDisable();
      	  frcTestWatchDogAppStop();
	  break;

	case 5:
         wdgService=1;
	  break;

	case 6:
         frcTestWatchDogAppStop();
	  break;

	case 7:
	  if(!frcTestWdInitConnect)
	  {
	       if( OK == gtIntConnect(WD_NMI, watchDogIsr, 0, 0))
              	printf("\nWatchdog NMI ISR connected\n");
              
              if( ERROR == gtIntEnable(WD_NMI))
              	printf("\nWatchdog NMI enable ERROR\n");
              else
              	printf("\nWatchdog NMI enabled\n");
              
              frcTestWdInitConnect =1;
              frcTestWdInitDisconnect=0;
	  }
	 case 8:
	  if(!frcTestWdInitDisconnect)
	  {
              gtIntDisconnect(WD_NMI);
              gtIntDisable(WD_NMI);
              frcTestWdInitConnect =0;
              frcTestWdInitDisconnect =1;
	  }
	  break;

	default:
	  printf ("Unknown option!! try again\n");
	  break;
	}

    }

    return OK;
}


void wdValueReg (UINT16 off) {
    UINT32 temp;
    
    GT_REG_READ(off,&temp);	
    printf("The value of  reg offset %x is %x \n",off,temp);
}

