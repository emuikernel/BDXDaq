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
* Filename:frctestcaes.c                                                      *
*                                                                             *
* DESCRIPTION:	                                                              *
*       All most all the test cases for testing PMC280, are implemented in        *
*       this file.                                                            *
* DEPENDENCIES:	                                                              *
*       None.                                                                 *
*                                                                             *
******************************************************************************/



#include <taskLib.h>
#include <sys/times.h>
#include <tickLib.h>
#include <sysLib.h>
#include <string.h>
#include "pciConfigLib.h"


#include <fcntl.h>



#include "frctestcases.h"
#include "memtest_drv.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <time.h>
#include <math.h>
#include "vxGppIntCtrl.h"

/*Includes for regparse*/
#include "regparse/Structs.h"
#include "gtSmp.h"
#include "gtDma.h"
#include "i2cDrv.h"

#include "frctestconfig.h"

IMPORT void vxTimeBaseSet (UINT32 tbu, UINT32 tbl);
IMPORT void vxTimeBaseGet (UINT32 * pTbu, UINT32 * pTbl);
IMPORT int frcLmbench (void);
IMPORT void vxHid1Set(UINT32);
IMPORT UINT32 vxHid1Get();


IMPORT unsigned int frcPci0ReadConfigReg (unsigned int regOffset,
					  unsigned int pciDevNum);
IMPORT void frcPciShow ();

IMPORT UINT32 vxHid1Get(void);
IMPORT int ipAttach ();
IMPORT UINT32  vxMsrGet    (void);

int frcTestEthLinkDetails (int portNum);

IMPORT int usrNetInit ();
IMPORT int ifAddrSet ();
IMPORT int pingLibInit ();
IMPORT void frcDbTestTaskEntry (unsigned int membase, unsigned int memsize);
IMPORT void frcSysAuxClkDrvInit (void);
IMPORT void frcSysAuxClkDisable (void);
IMPORT STATUS frcSysAuxClkRateSet (int ticksPerSecond);

IMPORT void frcEEPROMTestWrite (UINT8 devAdd);
IMPORT void frcEEPROMTestRead (UINT8 devAdd);
IMPORT void frcBibDataWrite (char *FTPSERVER_IP_ADRS, char *directory,
			     char *filename);
IMPORT STATUS frcBibDrvShow (int infoLvl);
IMPORT STATUS frcBibAttach ();
IMPORT void frcFlashCheck ();
IMPORT int frcFlashUnlock (unsigned int *flashBase);
IMPORT void frcTempShow (void);
IMPORT int settime(char *);
IMPORT void gettime (void);
IMPORT void frcBootFlashFile (char *FTPSERVER_IP_ADRS, char *DIRECTORY,
			      char *filename);
IMPORT STATUS frcProgramLocalVPD (char *FTPSERVER_IP_ADRS, char *directory,
				  char *filename);
IMPORT bool frcVPDStrataWrite (UINT32 bus, UINT32 dev, UINT32 fun);
IMPORT void frcVPDStrataRead (UINT32 bus, UINT32 dev, UINT32 fun,
			      UINT16 vpdAddr, UINT32 * vpdData);
IMPORT void gtUartBaudRateChange (int portNum, int baudRate);
IMPORT void frcDbTestStart (unsigned int membase, unsigned int memsize,
			    int loops);
IMPORT int frcFlashLock (unsigned int *flashBase);
IMPORT void frcGppIntCtrlInit (void);
IMPORT STATUS frcGppIntConnect (GPP_CAUSE cause, VOIDFUNCPTR routine,
				int parameter, int prio);
IMPORT STATUS frcGppIntDisable (GPP_CAUSE cause);
IMPORT STATUS frcGppIntEnable (GPP_CAUSE cause);
unsigned int testRdReg ();

double getPLLCoreMultiplier ();
IMPORT int frcTestEthPHYTestMode(int port, int mode);
/*IMPORT unsigned int frcWhoAmI(void);*/

#define _ON_TARGET

#ifdef _ON_TARGET
#include "./../../commDrv/ethernet.h"
#endif /*_ON_TARGET*/

#define MAX_PERF_SIZE (8*1024*1024)




extern void frcDmaTestStart (DMA_ENGINE dmaEngine, unsigned int xferSize,
			     char srcIf, char tgtIf, char dmaData,
			     unsigned int dmaDtl, int loopCnt);


extern void frcDmaTestStart0 (DMA_ENGINE dmaEngine, unsigned int xferSize,
			      char srcIf, char tgtIf, char dmaData,
			      unsigned int dmaDtl, int loopCnt);
extern void frcDmaTestStart1 (DMA_ENGINE dmaEngine, unsigned int xferSize,
			      char srcIf, char tgtIf, char dmaData,
			      unsigned int dmaDtl, int loopCnt);


/****************** Global variables  ************************/
unsigned char netWorkInitStatus = 1;

/******************Function definitions***********************/

/********************vxTimebase Timer routines start *************************/

/*vijay added*/
#ifndef DEC_CLK_TO_INC
#define DEC_CLK_TO_INC	4
#endif /*DEC_CLK_TO_INC */

#ifndef  DEFAULT_DEC_CLK_FREQ
#define	DEFAULT_DEC_CLK_FREQ	133000000
#endif /*8DEFAULT_DEC_CLK_FREQ */

#ifndef DEC_CLOCK_FREQ
#define DEC_CLOCK_FREQ  DEFAULT_DEC_CLK_FREQ
#endif /*#ifndef DEC_CLOCK_FREQ */

#define DIVI_SEC        (DEC_CLOCK_FREQ/DEC_CLK_TO_INC)
#define DIVI_USEC       (DEC_CLOCK_FREQ/(DEC_CLK_TO_INC*1000000))



void
frcTestTbReset (void)
{
  unsigned int tbStart = 0;

  vxTimeBaseSet (tbStart, tbStart);
}


/*dependent functions added by vijay*/
int
frcTestgettimeofday (struct timeval *tp, struct timezone *ptr)
{
  UINT32 tbLo, tbHi;
  double tv;

  vxTimeBaseGet (&tbHi, &tbLo);

  tv = tbLo + tbHi * (double) 0x100000000ULL;

  tp->tv_sec = tv / (double) DIVI_SEC;
  tp->tv_usec = (tv - ((double) tp->tv_sec * DIVI_SEC)) / (double) DIVI_USEC;

  return tp->tv_sec;
}

double
frcTestGettimeUsec (void)
{
  unsigned int tbLo, tbHi;
  double usecTemp, tvTemp;

  vxTimeBaseGet (&tbHi, &tbLo);
  tvTemp = tbLo + tbHi * (double) 0x100000000ULL;
  usecTemp = tvTemp / (double) DIVI_USEC;
  return usecTemp;
}

/********************vxTimebase Timer routines end ***************************/

int
getOption (void)
{

  char input[20], *end;
  UINT32 out;

  do
    {
      scanf ("%s", input);
      out = (UINT32) strtoul (input, &end, 0);
      if (*end != '\0')
	{
	  printf ("Invalid input:%s", end);
	  if (errno != 0)
	    perror (":");
	  printf ("\nEnter again:");
	}
      else
	break;
    }
  while (1);

  return (int) out;

  #if 0
				/*Vijay's code */
  char choice[50];
  int option;
  printf ("\n \t\t:");
  scanf ("%s", choice);
  option = atoi (choice);

  return option;
    #endif
}

LOCAL void
frcTestMemCaution (void)
{
  printf ("!!!!Caution!!!!\n");
  printf
    ("Execute the function showMap() or selct the option 24 from main Menu \n and get the safe range of memory on which these tests can be conducted\n");
}

int
prtMVReg (unsigned int stOffset, unsigned int endOffset)
{
  unsigned int i = 0;
  for (i = 0; i < (endOffset - stOffset); i++)
    printf ("MV register offset:0x%x \tData at this offset:0x%lx\n",
	    (stOffset + i), sysInLong (stOffset + i));

  return 0;
}
#ifdef WHO_AMI_TEST
IMPORT int frcWhoAmI();
#endif
#if 0
int
frcTestWhoAmI (void)
{
#ifdef WHO_AMI_TEST

  if (0 == frcWhoAmI ())
    {
      printf ("\t\t CPU:0\n");
    }
  else if (1 == frcWhoAmI ())
    {
      printf ("\t\t CPU:1\n");
    }
  else if (2 == frcWhoAmI ())
    {
      printf ("PCI agent\n");
    }
  else
    printf ("Unknown device\n");

#else

   printf ("\n\t CPU:0\n");

#endif 
  return TEST_SUCCESS;
}

#endif


int
frcTestWhoAmI (void)
{

#ifdef WHO_AMI_TEST
unsigned int option;



option = frcWhoAmI ();
      switch (option)
	{
	case 0:
	 printf ("\t\t CPU:0\n");
        break;
	case 1:
	 printf ("\t\t CPU:1\n");
	 
	  break;
	case 2:
	  printf ("PCI agent\n");
	break;
	default:
       printf ("Unknown device\n");
       return -1;
	
	}
#else

   printf ("\n\t CPU:0\n");

#endif 
	return TEST_SUCCESS;

}

int
frcTestChkECC (void)
{
#ifdef ECC_TEST
  unsigned int eccEnable = 0, eccCountReg = 0, eccErrReg = 0;
  frcTestMemCaution ();
  printf ("Inside ECC test\n");

  eccErrReg = testRdReg (0xf1001450);
  eccErrReg = eccErrReg & 0xfffffffe;
  sysOutLong (0xf1001450, eccErrReg);

  eccEnable = testRdReg (0xF1001400);
  eccEnable = eccEnable >> 18;

  /*Check weather ECC is enabled or not */
  if (eccEnable & 1)
    {
      printf ("ECC is enabled\n");
    }
  else
    {
      printf ("ECC is not enabled, returning from the test\n");
      return TEST_ERROR;
    }

  /*Conduct the memory tests */
  frcTestMemTest ();

  /*Check the number of errors */
  eccCountReg = testRdReg (0xF1001458);

  printf ("\n\t Number of single bit errors occured:%d\n", eccCountReg);

  eccErrReg = testRdReg (0xf1001450);
  if (eccErrReg & 1)
    printf ("Two bit error has occured\n");

  /*free(memBlock); */

#else
  printf ("This Test is not supported\n");
#endif /*ECC_TEST */
  return TEST_SUCCESS;
}

int
frcTestIDMAScrubChk (unsigned int stAddr, unsigned int endAddr)
{
#ifdef IDMA_SCRUB_TEST
  unsigned int *addrPtr = (unsigned int *) stAddr, i = 0, count = 0;
  printf ("IDMA Scrub test \n");
  frcTestMemCaution ();
  count = endAddr - stAddr;

  for (i = 0; i < count; i++)
    {
      if (*(addrPtr + i))
	{
	  printf ("IDMA Scrub error\n");
	  return TEST_ERROR;
	}
    }
  printf ("IDMA Scrub test -Success\n");

#else
  printf ("This Test is not supported\n");
#endif /*IDMA_SCRUB_TEST */
  return TEST_SUCCESS;
}


/*memory performance----mem_perf*/

int
testmem_perf (void)
{
#ifdef MEMORY_PERF_TEST
  unsigned long i, mcount;
  unsigned long the_tick;
  double d1, d2;
  long bufsize;
  unsigned long *pBufx, *pBufy;

  printf ("\nMemory Performance test program.\n");

  if ((pBufx = malloc (MAX_PERF_SIZE)) == NULL)
    {
      printf ("malloc error\n");
      return -1;
    }
  if ((pBufy = malloc (MAX_PERF_SIZE)) == NULL)
    {
      printf ("malloc error\n");
      return -1;
    }

  printf ("pBufx= %08lx\n", (unsigned long) pBufx);
  printf ("pBufy= %08lx\n", (unsigned long) pBufy);


  printf ("sysClkRateGet()= %d\n", sysClkRateGet ());

  printf ("     mcount,       bufsize,  ticks,   time [s], BW [MB/s]\n");
  taskDelay (10);

  bufsize = 1024;
  while (bufsize <= MAX_PERF_SIZE)
    {
      the_tick = 0;
      mcount = 1;
      while (the_tick < 100)
	{
	  mcount = 10 * mcount;
	  tickSet (0);
	  for (i = 0; i < mcount; i++)
	    {
	      memcpy (pBufy, pBufx, bufsize);
	      the_tick = tickGet ();
	    }
	}
      d1 = 2.0 * mcount * bufsize / 1024 / 1024;
      d2 = 1.0 / sysClkRateGet () * the_tick;
      printf ("%12ld, %12ld, %5ld, %8.2f, %10.2f\n", mcount, bufsize,
	      the_tick, d2, d1 / d2);
      taskDelay (10);
      bufsize = bufsize * 2;
    }
  free (pBufx);
  free (pBufy);

#else
  printf ("This Test is not supported\n");
#endif /*MEMORY_PERF_TEST */

  return 0;
}

int
frcTestMemPerf (void)
{
#ifdef MEMORY_PERF_TEST
  unsigned int option = 0;

  while (1)
    {
      printf ("\n\n======Select test to be performed=====\n\n");
      printf ("mem_perf - test          ======>1\n");
      printf ("lmbench  - test          ======>2\n");

      printf ("Return back to main menu ===>0\n");

      option = getOption ();
      switch (option)
	{
	case 0:
	  return 0;

	case 1:
	  printf ("option-> 1\n");

	  testmem_perf ();

	  break;
	case 2:
	  printf ("option-> 2\n");
#ifdef LMBENCH_TEST
	  frcLmbench ();
#else
	  printf ("This Test is not supported\n");
#endif /*LMBENCH_TEST */
	  break;

	default:
	  printf ("Unknown option!!!...try again\n");
	  break;
	}
    }
#else
  printf ("This test is not supported\n");
#endif

  return 0;
}


/********Serial ports*******/
#define _TEST_CLK_RATE 12500000

int
frcTestSerialInfo (void)
{

#ifdef SERIAL_PORT_TEST

  unsigned int regBCR0 = 0, regBCR1 = 0;	/*physical address has to be updatesd */
  unsigned int baudRate0 = 0, baudRate1 = 0;

  regBCR0 = testRdReg (0xF100b200);
  regBCR1 = testRdReg (0xF100b208);

#ifdef TEST_DEBUG
  printf ("for Port 0, Value of temp before shift:0x%x\n", regBCR0);
  printf ("for Port 1, Value of temp before shift:0x%x\n", regBCR1);
#endif /*TEST_DEBUG */

  regBCR0 = regBCR0 & 0xffff;
  regBCR1 = regBCR1 & 0xffff;

#ifdef TEST_DEBUG
  printf ("for Port 0, Value of temp before shift:0x%x\n", regBCR0);
  printf ("for Port 1, Value of temp before shift:0x%x\n", regBCR1);
  baudRate0 = _TEST_CLK_RATE / ((regBCR0 + 1) * 16);
  baudRate1 = _TEST_CLK_RATE / ((regBCR1 + 1) * 16);
#endif /*TEST_DEBUG */

  switch (regBCR0)
    {
    case 0x0050:
      baudRate0 = 9600;
      break;
    case 0x0027:
      baudRate0 = 19200;
      break;
    case 0x0013:
      baudRate0 = 38400;
      break;
    case 0x000c:
      baudRate0 = 57600;
      break;
    case 0x0006:
      baudRate0 = 115200;
      break;
    default:
      printf ("Unknown baud rate\n");
      return -1;
    }
  switch (regBCR1)
    {
    case 0x0050:
      baudRate1 = 9600;
      break;
    case 0x0027:
      baudRate1 = 19200;
      break;
    case 0x0013:
      baudRate1 = 38400;
      break;
    case 0x000c:
      baudRate1 = 57600;
      break;
    case 0x0006:
      baudRate1 = 115200;
      break;
    default:
      printf ("Unknown baud rate\n");
      return -1;
    }
  printf ("\t========Serial ports Settings==========\n");
  printf ("\tPort number:0     Baud rate set:%u\n", baudRate0);
  printf ("\tPort number:1     Baud rate set:%u\n", baudRate1);

#else
  printf ("his Test is not supported\n");
#endif /*SERIAL_PORT_TEST */
  return 0;
}



int
frcTestStressSerial (void)
{
unsigned int count;
int in, out,ch;
char file[20];
printf("\nEnter a file name:");
scanf("%s",file);
out = open (file, O_CREAT|O_RDWR,0777);

in = (int)stdin;/*stdin*/
fflush(stdin);
printf("\nSend the file");
count = 0;


while ((ch = getc((FILE*)in)) != EOF)
	{
	count ++;
	putc (ch, (FILE*) out);
	}
printf ("...received %u characters.",count);
return OK;

}

LOCAL int
frcTestBaudChg (void)
{
#ifdef BAUD_TEST
  int MpscPort, baudKey;

  printf ("\nEnter the Port  (0 or 1):");
  scanf ("%d", &MpscPort);

  printf ("\n------------------------------------------------\n");
  printf
    ("                 Enter Baud Rate Configuration                 \n");
  printf ("--------------------------------------------------\n");
  printf ("KEY            Baud Rate\n");
  printf ("......            ............\n");
  printf ("  1.                9600bps   \n");
  printf ("  2.               19200bps   \n");
  printf ("  3.               38400bps   \n");
  printf ("  4.               57600bps   \n");
  printf ("  5.              115200bps   \n");
  printf ("--------------------------------------------------\n");

  baudKey = getOption ();
  printf ("\n======Select the option=====\n");
  printf ("display serial port settings      ======>1\n");
  printf ("Baud rate change                  ======>2\n");
  printf ("Return back to main menu     ====>0\n\tOption:");

  taskDelay (60);

  switch (baudKey)
    {
    case 1:
      gtUartBaudRateChange (MpscPort, 9600);
      break;

    case 2:
      gtUartBaudRateChange (MpscPort, 19200);
      break;

    case 3:
      gtUartBaudRateChange (MpscPort, 38400);
      break;

    case 4:
      gtUartBaudRateChange (MpscPort, 57600);
      break;

    case 5:
      gtUartBaudRateChange (MpscPort, 115200);
      break;

    default:
      printf ("invalid number");
      break;
    }

#else
  printf ("Currently it sopports only one baud rate - 152000\n");
#endif
  return 0;
}

int
frcTestSerial (void)
{
  int option = 0;

  while (1)
    {
      	  printf ("\n\n======Select the option=====\n\n");
	  printf ("Display serial port settings      ======>1\n");
	  printf ("Baud rate change                  ======>2\n");
  	  printf ("Stress serial console             ======>3\n");
	  printf ("Return back to main menu     ====>0\n\tOption:");
	

      option = getOption();
      switch (option)
	{
	case 0:
	  return 0;

	case 1:
	  frcTestSerialInfo ();
	  break;

	case 2:
	  frcTestBaudChg ();
	  break;
	  
	case 3:
	  frcTestStressSerial ();
	  break;
	default:
	  printf ("Unknown option!! try again\n");
	  break;
	}
    }
  return 0;
}

typedef union subsystemIdandVendorId
{
  unsigned int ID;
  struct
  {
    unsigned int subsystemId:16;
    unsigned int vendorId:16;
  } SUBID;
} MVIDS_STRU;

int
frcTestSubsysVendID (void)
{

#ifdef SUB_ID_VEND_ID_TEST
  unsigned int deviceId = 0x00;
  MVIDS_STRU frcTestIDs;

  printf ("Enter the device number :");
  scanf ("%u", &deviceId);

  frcTestIDs.ID = frcPci0ReadConfigReg (0x2c, deviceId);
  printf ("Vendor  ID    :0x%x\n", frcTestIDs.SUBID.vendorId);
  printf ("Subsystem ID  :0x%x\n", frcTestIDs.SUBID.subsystemId);

#else
  printf ("This Test is not supported\n");
#endif /*SUB_ID_VEND_ID_TEST */
  return 0;
}
#if 0
int
frcTestPCI (void)
{

#ifdef PCI_TEST
  unsigned int option = 0;

  while (1)
    {
      printf ("\n\n======Select the option=====\n\n");
      printf ("Scan all buses                    ======>1\n");
      printf ("Return back to main menu          ======>0\n");

      option = getOption ();

      switch (option)
	{
	case 0:
	  return 0;

	case 1:
	  frcPciShow ();

	  break;
	default:
	  printf ("Unknown option!! try again\n");
	  break;
	}

    }
  return 0;

#else
  printf ("This Test is not supported\n");
#endif
  return 0;
}

#endif
/************************* Ethernet Tests Start ******************************/
/*Ethernet Link status*/
int
frcTestEthLinkDetails (int portNum)
{
#ifdef _ON_TARGET
  unsigned int data = 0, phyReg = 0x19;
  if (0 == ethPortReadSmiReg (portNum, phyReg, &data))
    {
      printf ("Error in reading the data\n");
      return TEST_ERROR;
    }
  else
    {
      /*parse the register */
      data = data >> 8;
      data = data & 0x7;
      switch (data)
	{
	case 0:
	  printf ("Auto negotiation is not complete\n");
	  break;

	case 1:
	  printf ("Port:%d is in the mode: 10 BASE-T half duplex\n", portNum);
	  break;

	case 2:
	  printf ("Port:%d is in the mode: 10 BASE-T full duplex\n", portNum);
	  break;

	case 3:
	  printf ("Port:%d is in the mode: 100 BASE-TX half duplex\n",
		  portNum);
	  break;

	case 4:
	  printf ("Port:%d is in the mode: 100 BASE-T4\n", portNum);
	  break;

	case 5:
	  printf ("Port:%d is in the mode: 100 BASE-TX full duplex\n",
		  portNum);
	  break;

	case 6:
	  printf ("Port:%d is in the mode: 1000 BASE-T half duplex\n",
		  portNum);
	  break;

	case 7:
	  printf ("Port:%d is in the mode: 1000 BASE-T full duplex\n",
		  portNum);
	  break;

	default:
	  printf ("Select the correct option\n");
	}

    }

  return TEST_SUCCESS;
#endif /*_ON_TARGET*/
}

int
frcTestConfigEthPorts (void)
{

#ifdef CFG_LAN_TEST
  int port = 0, type = 0, temp1 = 0, temp2 = 0;
  char ipAddr[15];

  printf ("Enter the type the port to be configured \n \t\t 0 ---------  On-Board Eths\n\t\t 1 ---------  Athena\n\t\t 2 ---------  82559\n\t:");
  scanf ("%d", &type);
port = 0;
if (( type == 0) &&( isPMC280TwoOnBoardEth() == OK))
{
  printf ("Enter the port number:");
  scanf ("%d",&port);
/* port = getOption();*/
       if ( port > 2 )
 	{
 	    printf("\nPMC280 supports max. 2 on-board Ethernet ports.");
 	    return ERROR;
 	}
}
  printf ("Enter the IP Address:");
  scanf ("%s", ipAddr);

  if (0 == type)
    {
      if (0 == port)
	{

	  temp1 = ipAttach (port, "mgi");
	  temp2 = ifAddrSet ("mgi0", ipAddr);

	}
      else if (1 == port)
	{

	  temp1 = ipAttach (port, "mgi");
	  temp2 = ifAddrSet ("mgi1", ipAddr);

	}
      else
	{

	  printf ("Invalid port number \n");
	  return -1;

	}
    }
  else if (1 == type)
    {
      if (0 == port)
	{

	  temp1 = ipAttach (port, "gei");
	  temp2 = ifAddrSet ("gei0", ipAddr);

	}
      else if (1 == port)
	{

	  temp1 = ipAttach (port, "gei");
	  temp2 = ifAddrSet ("gei1", ipAddr);

	}
      else if (2 == port)
	{

	  temp1 = ipAttach (port, "gei");
	  temp2 = ifAddrSet ("gei2", ipAddr);

	}
      else
	{
	  printf
	    ("Oh! God..Test tool doesn't support this port#, do manually\n");
	  return 0;
	}

    }
  else if (2 == type)
    {
      if (0 == port)
	{

	  temp1 = ipAttach (port, "fei");
	  temp2 = ifAddrSet ("fei0", ipAddr);

	}
      else if (1 == port)
	{

	  temp1 = ipAttach (port, "fei");
	  temp2 = ifAddrSet ("fei1", ipAddr);

	}
      else if (2 == port)
	{

	  temp1 = ipAttach (port, "fei");
	  temp2 = ifAddrSet ("fei2", ipAddr);

	}
      else
	{
	  printf
	    ("Oh! God..Test tool doesn't support this type, do manually\n");
	  return 0;
	}

    }
  else
    {
      printf ("Unknown port!! try again.\n");
      return 0;
    }
  if (OK == temp1 && OK == temp2)
    printf ("Port configuration successful\n");

#else
  printf ("This test is not supported\n");
#endif /*#define CFG_LAN_TEST */

  return 0;
}


LOCAL int
frcTestEthSet (unsigned short port, unsigned short duplex,
	       unsigned short mode, unsigned short autoNeg)
{
  unsigned short phyReg = 0x00;
  unsigned int temp;

  ethPortReadSmiReg (port, phyReg, &temp);

#ifdef TEST_DEBUG
  printf ("value read at the reg :%x for port:%d is:%d", phyReg, port, temp);
#endif /*#ifdef TEST_DEBUG */

  if (autoNeg)
    {
      temp = temp | 0x1000;
      if (ethPortWriteSmiReg (port, phyReg, temp))
	printf ("Set to Auto-Negotiation mode\n");
    }
  else
    {
      temp = temp & 0xefff;	/*Disable auto-neg */
      if (duplex)
	temp = temp | 0x0100;	/*Full duplex */
      else
	temp = temp & 0xfeff;	/*Half duplex */

      /*0---10Mbps, 1---100Mbps, 2---1000Mbps */
      if (0 == mode)
	{
	  temp = temp & 0xdfbf;
	}
      else if (1 == mode)
	{
	  temp = temp | 0x2000;
	  temp = temp & 0xffbf;
	}
      else if (2 == mode)
	{
	  temp = temp | 0x0040;
	  temp = temp & 0xdfff;

	}
      else
	{
	  printf ("Unknown mode \n");
	  return -1;
	}
#ifdef TEST_DEBUG
      printf ("value written at the reg :%x for port:%d  \n\tis:%x\n", phyReg,
	      port, temp);
#endif /*#ifdef TEST_DEBUG */
      if (ethPortWriteSmiReg (port, phyReg, temp))
	printf ("Mode setting is successful\n");

    }
  return 0;
}

int
frcTestEthModeSet (void)
{
  int port = 0, type = 0, option = 0;

  printf ("This support is implemented only for On-Board Ethernet ports\n");
  printf ("Enter the port number:");
  scanf ("%d", &port);
  /*
     printf("Enter the type the port to be configured \n \t\t 0 ---------  On-Board Eths\n\t\t 1 ---------  Athena\n\t\t 2 ---------  82559\n\t:");
     scanf("%d",&type);
   */
  type = 0;
  while (1)
    {
      printf ("Select the mode of operation\n");
      printf ("10 Mbps Half Duplex     ======>1\n");
      printf ("10 Mbps Full Duplex     ======>2\n");
      printf ("100 Mbps Half Duplex    ======>3\n");
      printf ("100 Mbps Full Duplex    ======>4\n");
      printf ("1000 Mbps Full Duplex   ======>5\n");
      printf ("Auto-Negotiation        ======>6\n");
      printf ("Exit   ======>0\n");

      option = getOption ();
      if (0 == type)
	{
	  printf ("Configuring the On-Board Ethernet ports\n");
	  printf ("Mode selected : %d\n", option);
	  switch (option)
	    {
	    case 0:
	      return 0;
	    case 1:
	      frcTestEthSet (port, 0, 0, 0);
	      break;
	    case 2:
	      frcTestEthSet (port, 1, 0, 0);
	      break;
	    case 3:
	      frcTestEthSet (port, 0, 1, 0);
	      break;
	    case 4:
	      frcTestEthSet (port, 1, 1, 0);
	      break;
	    case 5:
	      frcTestEthSet (port, 1, 2, 0);
	      break;
	    case 6:
	      frcTestEthSet (port, 0, 0, 1);
	      break;
	    default:
	      printf ("Unknown mode!! try again\n");
	      break;
	    }
	}
      else
	{
	  printf ("This type is not supported\n");
	  return 0;
	}
    }
  return 0;
}

int
frcTestEthernet (void)
{
#ifdef ETHERNET_TEST
  unsigned int option = 0, port = 0,mode;

  while (1)
    {
      printf ("\n\n======Select the option=====\n\n");
      printf ("Display Ethernet port settings     ======>1\n");
      printf ("Configure Ethernet ports           ======>2\n");
      printf ("Set the mode of operation          ======>3\n");
      printf ("Initialize pingLib                 ======>4\n");
       printf ("PHY test                          ======>5\n");
      printf ("Return back to main menu      ======>0\n");

      option = getOption ();

      switch (option)
	{
	case 0:
	  return 0;

	case 1:

	port = 0;
	if ( isPMC280TwoOnBoardEth() == OK)
		{
	    	printf("Enter the port number    :");
           		/*scanf("%u",&port);*/
           		port = getOption();
		}
	  frcTestEthLinkDetails (port);
	  break;

	case 2:
	  frcTestConfigEthPorts ();
	  break;

	case 3:
	  frcTestEthModeSet ();
	  break;

	case 4:
	  pingLibInit ();
	  break;
	case 5:
		port = 0;
	if ( isPMC280TwoOnBoardEth() == OK)
		{
	    	printf("Enter the port number    :");
           		/*scanf("%u",&port);*/
           		port = getOption();
		}

           /*Prompt the user for the required mode*/
           printf("*********************************************************\n");
           printf("\t\tSelect the Required Test mode\n");
           printf("*********************************************************\n");
           printf("\t       Test mode                            Option\n");
           printf("\tTest Mode(1) Transmit Wave form test     ======>1\n");
           printf("\tTest Mode(2) Master Transmit Jitter Test ======>2\n");
           printf("\tTest Mode(3) Slave Transmit Jitter Test  ======>3\n");
           printf("\tTest Mode(4) Tramsmitter Distortion Test ======>4\n");	   
           printf("\tSet back to Normal mode                  ======>5\n");
           printf("\tReturn           ======>0\n");
           mode = getOption();

           frcTestEthPHYTestMode(port, mode);
           break;   

	default:
	  printf ("Unknown option!! try again\n");
	  break;
	}

    }

#else
  printf ("This Test is not supported\n");
#endif /*ETHERNET_TEST */
  return 0;
}

/************************* Ethernet Tests End ********************************/


/***********************Interrupt Latency Start*******************************/
/*
There are two methods for the calculation of Interrupt Latency.
1. Through Software
2. Through Hardware.
*/
/*MPP 18 is used as input port*/
int mpp18Initialized = 0;
/*MPP 24 is used as input port*/
int mpp24Initialized = 0;
unsigned int ioTemp = 0;

static SEM_ID frcISRLatSem;
double IntLatencyTotal = 0;	/*Accumulated Latency */
double IntTimeBaseSt = 0;	/*Time base count Start */
double IntTimeBaseEnd = 0;	/*Time base count End */
double tbTempHi = 0x0, tbTempLo = 0x0;
int intConnectSoft = 0, intConnectHard = 0;
#define TEST_NUM_LOOPS  200
#undef INT_LATENCY_TEST_DBG
/*Interrupt Latency Calculation*/
void
frcTestMPP18ISR_hard (void)
{

  unsigned int data = 0;
#ifdef INT_LATENCY_TEST_DBG
  logMsg ("\nfrcDmaCompleteAppIntHandler[%d]: Entering...", 0, 0, 0, 0, 0, 0);
#endif /*INT_LATENCY_TEST_DBG */

  /*Set the pin 24 low */
  GT_REG_READ (0xf104, &data);
  data &= 0xfeffffff;
  GT_REG_WRITE (0xf104, data);


  GT_REG_READ (GPP_VALUE, &data);
  /* GPP[10] */
  if (data & 0x00000400)
    {
      /* Pulse low */
      data &= 0xFFFFFBFF;
      GT_REG_WRITE (GPP_VALUE, data);
    }
  else
    {
      /* Pulse high */
      data |= (0x1 << 10);
      GT_REG_WRITE (GPP_VALUE, data);
    }


  /* Give semaphore */
  semGive (frcISRLatSem);
#ifdef INT_LATENCY_TEST_DBG
  logMsg ("\nfrcDmaCompleteAppIntHandler[%d]: Exiting...", 0, 0, 0, 0, 0, 0);
#endif /*INT_LATENCY_TEST_DBG */

}

void
frcTestMPP18ISR_soft (void)
{
  unsigned int data = 0;

#ifdef INT_LATENCY_TEST_DBG
  printf ("ent\n");
  /*  logMsg("\nfrcDmaCompleteAppIntHandler[%d]: Entering...",0,0,0,0,0,0); */
#endif /*INT_LATENCY_TEST_DBG */

  /*Set the pin 24 low */
  GT_REG_READ (0xf104, &data);
  data &= 0xfeffffff;
  GT_REG_WRITE (0xf104, data);

  IntTimeBaseEnd = frcTestGettimeUsec ();

  /* Give semaphore */
  semGive (frcISRLatSem);

#ifdef INT_LATENCY_TEST_DBG
  printf ("exit\n");
  /* logMsg("\nfrcDmaCompleteAppIntHandler[%d]: Exiting...",0,0,0,0,0,0); */
#endif /*INT_LATENCY_TEST_DBG */

}

int
frcTestLatInitialization (void)
{
  unsigned int ioSet = 0;

  /* Create semaphore */
  frcISRLatSem = semBCreate (SEM_Q_FIFO, SEM_EMPTY);

  /*Initialize the MPP ports used */

  /* GPP[24] */
  printf ("Step-1\n");
  taskDelay (10);
  GT_REG_READ (0xf00c, &ioSet);
  ioSet &= 0xfffffff0;
  printf ("Step-2\n");
  taskDelay (10);
  GT_REG_WRITE (0xf00c, ioSet);

  printf ("Step-3\n");
  taskDelay (10);
  GT_REG_READ (0xf100, &ioSet);
  ioSet |= 0x01000000;
  printf ("Step-4\n");
  taskDelay (10);
  GT_REG_WRITE (0xf100, ioSet);

  printf ("Step-5\n");
  taskDelay (10);
  GT_REG_READ (0xf110, &ioSet);
  ioSet |= 0x01000000;
  printf ("Step-6\n");
  taskDelay (10);
  GT_REG_WRITE (0xf110, ioSet);

  /* GPP[18] */
  printf ("Step-7\n");
  taskDelay (10);
  GT_REG_READ (0xf008, &ioSet);
  ioSet &= 0xfffff0ff;
  printf ("Step-8\n");
  taskDelay (10);
  GT_REG_WRITE (0xf008, ioSet);

  printf ("Step-9\n");
  taskDelay (10);
  GT_REG_READ (0xf100, &ioSet);
  ioSet &= 0xfffbffff;
  printf ("Step-10\n");
  taskDelay (10);
  GT_REG_WRITE (0xf100, ioSet);

  printf ("Step-11\n");
  taskDelay (10);
  GT_REG_READ (0xf110, &ioSet);
  ioSet |= 0x00040000;
  printf ("Step-12\n");
  taskDelay (10);
  GT_REG_WRITE (0xf110, ioSet);

  /* GPP[10] */
  printf ("Step-13\n");
  taskDelay (10);
  GT_REG_READ (0xf004, &ioSet);
  ioSet &= 0xfffff0ff;
  printf ("Step-14\n");
  taskDelay (10);
  GT_REG_WRITE (0xf004, ioSet);	/*Changed from 0xf00c to 0xf004...by vijay on 14thOct */

  printf ("Step-15\n");
  taskDelay (10);
  GT_REG_READ (0xf100, &ioSet);
  ioSet |= 0x00000400;
  printf ("Step-16\n");
  taskDelay (10);
  GT_REG_WRITE (0xf100, ioSet);

  printf ("Step-17\n");
  taskDelay (10);
  frcGppIntCtrlInit ();

  return 0;
}

int
frcTestIntLatSoft ()
{
  unsigned int ioSet = 0, count = 0;

  frcTestLatInitialization ();
  printf ("Init complete\n");

  taskDelay (10);
  printf ("Connecting the ISR\n");
  if (!intConnectSoft)
    {
      frcGppIntConnect (18, frcTestMPP18ISR_soft, 0, 0);
      intConnectSoft = 1;
    }
  frcGppIntEnable (18);

  frcTestTbReset ();
  printf ("Reset vxTimebase reg\n");

  printf ("Going into loop\n");
  taskDelay (15);
  IntLatencyTotal = 0;
  while (count < TEST_NUM_LOOPS)
    {
      frcTestTbReset ();
      IntTimeBaseSt = 0;
      IntTimeBaseEnd = 0;

#ifdef INT_LATENCY_TEST_DBG
      printf ("st\n");
      taskDelay (10);
#endif /*#ifdef INT_LATENCY_TEST_DBG */

      IntTimeBaseSt = frcTestGettimeUsec ();

      /* Clear MPP[24] */
      GT_REG_READ (0xf104, &ioSet);
      ioSet |= 0x01000000;
      GT_REG_WRITE (0xf104, ioSet);

      semTake (frcISRLatSem, WAIT_FOREVER);

#ifdef INT_LATENCY_TEST_DBG
      printf ("cal\n");
      taskDelay (10);
#endif /*#ifdef INT_LATENCY_TEST_DBG */

      count++;
      taskDelay (5);

#ifdef  INT_LATENCY_TEST_DBG
      printf ("Int latency:%f\n", (IntTimeBaseEnd - IntTimeBaseSt));
#endif

      IntLatencyTotal += IntTimeBaseEnd - IntTimeBaseSt;

#ifdef INT_LATENCY_TEST_DBG
      printf ("IntTimeBaseStart   IntTimeBaseEnd:%f\t%f\n", IntTimeBaseSt,
	      IntTimeBaseEnd);
#endif /*TEST_DEBUG */

    }
  printf ("\n\n\t Interrupt Latency:%f usec\n",
	  ((IntLatencyTotal / (double) TEST_NUM_LOOPS) - 0.35));
  IntLatencyTotal = 0;
/*   semGive(frcISRLatSem);*/
  if (OK == semDelete (frcISRLatSem))
    {
#ifdef LATENCY_TEST_DBG
      printf ("Semaphore deleted successfully \n");
#endif /*#ifdef LATENCY_TEST_DBG */
    }
  frcGppIntDisable (18);
  return 0;
}

int
frcTestIntLatHard ()
{
  unsigned int ioSet = 0, count = 0;

  frcTestLatInitialization ();

  printf ("Init complete\n");

  printf ("Connecting the ISR\n");
  taskDelay (10);
  if (!intConnectHard)
    {
      frcGppIntConnect (18, frcTestMPP18ISR_hard, 0, 0);
      intConnectHard = 1;
    }
  frcGppIntEnable (18);
  printf
    ("Probe at MPP#24 to measure latency as soon as it goes into loop\n");
  taskDelay (120);
  printf ("Going into loop\n");
  taskDelay (50);

  /*Look the output at the GPP[24] */
  while (count < TEST_NUM_LOOPS)
    {
      if (count)
	semTake (frcISRLatSem, WAIT_FOREVER);

      /* Clear MPP[24] */
      GT_REG_READ (0xf104, &ioSet);
      ioSet |= 0x01000000;
      GT_REG_WRITE (0xf104, ioSet);

      count++;
      taskDelay (2);
    }
  semDelete (frcISRLatSem);
  return 0;
}

int
lat_h ()
{
  frcTestIntLatHard ();
  return 0;
}

int
frcTestIntLatency (void)
{
#ifdef INT_LATENCY_TEST

  unsigned int option = 0;
#ifdef PMC280_DUAL_CPU
  if (0 == frcWhoAmI ())
    {
#endif /*PMC280_DUAL_CPU */

      while (1)
	{
	  printf ("\n\n======Select the Method=====\n\n");
	  printf ("Through Software             ======>1\n");
	  printf ("Through Hardware             ======>2\n");

	  printf ("Return back to main menu  =====>0\n");

	  option = getOption ();

	  switch (option)
	    {
	    case 0:
	      return 0;

	    case 1:
	      frcTestIntLatSoft ();
	      break;

	    case 2:
	      frcTestIntLatHard ();
	      break;

	    default:
	      printf ("Unknown option!! try again\n");
	      break;
	    }

	}
#ifdef  PMC280_DUAL_CPU
    }
  else
    {
      printf ("This test has to be executed on CPU0 only\n");
      return 0;
    }
#endif /*PMC280_DUAL_CPU */

#else
  printf ("This test is not supported\n");
#endif /*INT_LATENCY_TEST */

  return 0;
}

/***********************Interrupt Latency End*********************************/

IMPORT void MV64360_regs_dump (MV64360_REGS * regs, char *p, char *filter);
IMPORT int frcTestDispAllPCI0Reg (void);

/***************MV64360 Register dump(reg parse tool) start*******************/
int
frcTestDispReg (void)
{
#ifdef REG_PARSE_TEST
  unsigned int option = 0;

  while (1)
    {
      printf ("\n\n======Select the Method=====\n\n");
      printf ("Register dump of MV64360          ======>1\n");
      printf ("Register dump of PCI0             ======>2\n");

      printf ("Return back to main menu  =====>0\n");

      option = getOption ();

      switch (option)
	{
	case 0:
	  return 0;

	case 1:
	  printf ("This displayes the contents of MV64360 registers\n");
	  MV64360_regs_dump ((MV64360_REGS *) INTERNAL_REG_ADRS, NULL, NULL);
	  break;

	case 2:
	  frcTestDispAllPCI0Reg ();
	  break;

	default:
	  printf ("Unknown option!! try again\n");
	  break;
	}

    }

#else
  printf ("This Test is not supported\n");
#endif /*REG_PARSE_TEST */
  return 0;
}

/***************MV64360 Register dump(reg parse tool) end*********************/

/********************************** DMA Tests Start **************************/
int
frcTestDMA (void)
{
#ifdef  DMA_TEST

  unsigned int dmaEngine, xferSize, mode;
  int src, dst, loopCnt;
/*if ( isPMC280DualCPU() == OK)
{*//*hide by vijay*/
#ifdef PMC280_DUAL_CPU
  printf ("Enter the DMA Engine# :");
  scanf ("%d", &dmaEngine);
  printf ("Enter the X'fer size in HEX:");
  scanf ("%x", &xferSize);
  printf
    ("Enter the Source \n\t 0 ----SDRAM \n\t 1 ----SRAM \n\t 2  ---- PCI Space\n\t\t:");
  scanf ("%d", &src);
  printf
    ("Enter the Destination \n\t 0 ----SDRAM \n\t 1 ----SRAM \n\t 2  ---- PCI Space\n\t\t:");
  scanf ("%d", &dst);
  printf
    ("Enter the mode of transfer\n\t 0 ---- 8 Bytes \n\t 1 ---- 16 Bytes \n\t 3 ---- 32 Bytes \n\t 4 --- 128 Bytes \n\t 7 ---- 64 Bytes \n\t\t\t:");
  scanf ("%u", &mode);

  printf ("Enter the number of loops, enter -1 to run for ever\t:");
  scanf ("%d", &loopCnt);
  frcDmaTestStart (dmaEngine, xferSize, src, dst, 'D', mode, loopCnt);
  taskDelay (300);
/*}*/
/*#endif*/ /*#ifdef DMA_TEST_DUALCPU */
/*
#ifdef DMA_TEST_SINGLECPU
  unsigned int dmaEngine, xferSize, mode;
  int src, dst, loopCnt;
*/
/*else
{*/
#else
  printf ("Enter details for first thread\n");
  printf ("Enter the DMA Engine# :");
  scanf ("%d", &dmaEngine);
  printf ("Enter the X'fer size in HEX:");
  scanf ("%x", &xferSize);
  printf
    ("Enter the Source \n\t 0 ----SDRAM \n\t 1 ----SRAM \n\t 2  ---- PCI Space\n\t\t:");
  scanf ("%d", &src);
  printf
    ("Enter the Destination \n\t 0 ----SDRAM \n\t 1 ----SRAM \n\t 2  ---- PCI Space\n\t\t:");
  scanf ("%d", &dst);
  printf
    ("Enter the mode of transfer\n\t 0 ---- 8 Bytes \n\t 1 ---- 16 Bytes \n\t 3 ---- 32 Bytes \n\t 4 --- 128 Bytes \n\t 7 ---- 64 Bytes \n\t\t\t:");
  scanf ("%u", &mode);

  printf ("Enter the number of loops, enter -1 to run for ever\t:");
  scanf ("%d", &loopCnt);

  frcDmaTestStart0 (dmaEngine, xferSize, src, dst, 'S', mode, loopCnt);
  taskDelay (300);

  printf ("Enter details for second thread\n");
  printf ("Caution!!! Choose a different engine\n");
  printf ("Enter the DMA Engine# :");
  scanf ("%d", &dmaEngine);
  printf ("Enter the X'fer size in HEX:");
  scanf ("%x", &xferSize);
  printf
    ("Enter the Source \n\t 0 ----SDRAM \n\t 1 ----SRAM \n\t 2  ---- PCI Space\n\t\t:");
  scanf ("%d", &src);
  printf
    ("Enter the Destination \n\t 0 ----SDRAM \n\t 1 ----SRAM \n\t 2  ---- PCI Space\n\t\t:");
  scanf ("%d", &dst);
  printf
    ("Enter the mode of transfer\n\t 0 ---- 8 Bytes \n\t 1 ---- 16 Bytes \n\t 3 ---- 32 Bytes \n\t 4 --- 128 Bytes \n\t 7 ---- 64 Bytes \n\t\t\t:");
  scanf ("%u", &mode);

  printf ("Enter the number of loops, enter -1 to run for ever\t:");
  scanf ("%d", &loopCnt);

  frcDmaTestStart1 (dmaEngine, xferSize, src, dst, 'S', mode, loopCnt);
  taskDelay (300);
/*}*/
#endif /*PMC280_DUAL_CPU*/
/*
#endif */ /*#define DMA_TEST_SINGLECPU */

#else
  printf ("This Test is not supported\n");
#endif /*DMA_TEST */

  return 0;
}

#if 0
int
frcTestDMATests (void)
{
#ifdef DMA_TEST
  unsigned int option = 0;
#if 0
#ifdef DMA_STRESS
  int numEng;
#endif /*#ifdef DMA_STRESS */
#endif

  while (1)
    {
      printf ("\n\n======Select the Option=====\n\n");
      printf ("IDMA Test                     ======>1\n");

      printf ("Return back to Main menu   ======>0\n");


      option = getOption ();
      switch (option)
	{
	case 0:
	  return 0;

	case 1:

	  break;

	default:
	  printf ("Unknown option!! try again\n");
	  break;
	}

    }
#else
  printf ("This test is not supported\n");
#endif /*#define DMA_TEST */
  return 0;
}
#endif
/********************************** DMA Tests End ****************************/

/********************************** Dual CPU Tests Start**********************/

/***********************  MV64360 Seamphore tests start **********************/
int
frcTestMV64360Semaphore (void)
{
#ifdef MV_SEMAPHORE_TEST
  unsigned int option = 0;
  unsigned int semNum = 0, timeoutOp = 0;

  while (1)
    {
      printf ("\n\n======Select the Option=====\n\n");
      printf ("Take Semaphore               ======>1\n");
      printf ("Give Semaphore               ======>2\n");
      printf ("Return back to main menu     ======>0\n");


      option = getOption ();
      switch (option)
	{
	case 0:
	  return 0;

	case 1:
	  printf ("Enter the Semaphore number\n");
	  scanf ("%d", &semNum);
	  printf
	    ("Enter the Timeout option < 1 -- WAIT_FOREVER, 2 -- NO_WAIT>\n");
	  scanf ("%d", &timeoutOp);
	  if (!
	      ((semNum >= 0 && semNum <= 7)
	       && (timeoutOp >= 1 && timeoutOp <= 2)))
	    {
	      printf ("Wrong options! Select the correct options\n");
	      option = 3;
	      break;
	    }
	  else
	    {
	      if (1 == timeoutOp)
		timeoutOp = MV64360_SMP_WAIT_FOREVER;
	      else
		timeoutOp = MV64360_SMP_NO_WAIT;

	      if (OK == frcMv64360semTake (semNum, timeoutOp))
		{
		  printf ("Semaphore taken successfully\n");
		}
	      else
		{
		  printf ("Semaphore is Locked\n");
		}
	    }
	  break;

	case 2:
	  printf ("Enter the Semaphore number\n");
	  scanf ("%d", &semNum);

	  if (!(semNum >= 0 && semNum <= 7))
	    {
	      printf ("Wrong options! Select the correct options\n");
	      option = 3;
	      break;
	    }
	  else
	    {
	      if (OK == frcMv64360semGive (semNum))
		{
		  printf ("Semaphore is given successfully\n");
		}
	      else
		{
		  printf ("Unable to give Semaphore \n");
		}
	    }
	  break;

	default:
	  printf ("Unknown option!! try again\n");
	  break;
	}

    }

#else
  printf ("This Test is not supported\n");
#endif /*MV_SEMAPHORE_TEST */

  return 0;
}

/***********************  MV64360 Seamphore tests start **********************/

int
frcTestDualCPU (void)
{
#ifdef DUAL_CPU_TEST
  unsigned int option = 0, vmMemBase = 0x0, memSize = 0x0;
  int loops = 0;

  if ( isPMC280DualCPU() == ERROR )
  	{
  	printf ( "\nPMC280 is in single CPU mode" );
  	return ERROR;
  	}
  else
  	{
  	printf ( "\nPMC280 is in dual CPU mode" );
  	}

  while (1)
    {
      printf ("\n\n======Select the Method=====\n\n");
      printf ("Doorbell Interrupts/Shared Memory  Test   ======>1\n");
      printf ("Semaphore Test                            ======>2\n");

      printf ("Return back to main menu  =====>0\n");


      option = getOption ();

      switch (option)
	{
	case 0:
	  return 0;

	case 1:
	  printf ("\nEnter the Shared memory Base in HEX:");
	  scanf ("%x", &vmMemBase);
	  printf ("\nEnter the Memory size in HEX:");
	  scanf ("%x", &memSize);
	  printf ("\nEnter the number of loops:");
	  scanf ("%d", &loops);
	  frcDbTestStart (vmMemBase, memSize, loops);
	  taskDelay (300);
	  break;

	case 2:
	  frcTestMV64360Semaphore ();
	  break;

	default:
	  printf ("Unknown option!! try again\n");
	  break;
	}

    }
#else
  printf ("This Test is not supported\n");
#endif /*DUAL_CPU_TEST */
  return 0;

}

/********************************** Dual CPU Tests End************************/

/********************************Auxiliry Clocks Tests Start *****************/
#ifdef AUX_CLK_TEST
int
frcTestAuxClk (void)
{

  unsigned int option = 0;
  int ticRate;

  while (1)
    {
      printf ("\n\n======Select the Option=====\n\n");
      printf ("Set Aux Clk rate                 ======>1\n");
      printf ("Initialize the Clock Driver      ======>2\n");
      printf ("Disable the Auxiliary Clock      ======>3\n");

      printf ("Return back to Main menu   ======>0\n");

      option = getOption ();
      switch (option)
	{
	case 0:
	  return 0;

	case 1:
	  printf ("Enter the number of tics per second\n");
	  scanf ("%d", &ticRate);
	  frcSysAuxClkRateSet (ticRate);
	  break;

	case 2:
	  frcSysAuxClkDrvInit ();
	  printf
	    ("On CPU0--output at MPP pin 14, On CPU1--output at MPP pin 15\n");
	  break;

	case 3:
	  frcSysAuxClkDisable ();
	  break;

	default:
	  printf ("Unknown option!! try again\n");
	  break;
	}

    }

}
#endif /*AUX_CLK_TEST */

/********************************Auxiliry Clocks Tests End *******************/

/********************************I2C & EEPROM Tests Start ********************/
int
frcTestI2C_EEPROM (void)
{
#ifdef I2C_EEPROM_TEST
  unsigned int option = 0, deviceAddr;

  while (1)
    {
      printf ("\n\n======Select the Option=====\n\n");
      printf ("EEPROM Write       ======>1\n");
      printf ("EEPROM Read        ======>2\n");

      printf ("Return back to Main menu   ======>0\n");


      option = getOption ();
      switch (option)
	{
	case 0:
	  return 0;

	case 1:
	  printf ("Enter the device address in HEX:");
	  scanf ("%x", &deviceAddr);
	  frcEEPROMTestWrite (deviceAddr);
	  break;

	case 2:
	  printf ("Enter the device address in HEX:");
	  scanf ("%x", &deviceAddr);
	  frcEEPROMTestRead (deviceAddr);
	  break;

	default:
	  printf ("Unknown option!! try again\n");
	  break;
	}

    }


#else
  printf ("This Test is not supported\n");
#endif /*I2C_EEPROM_TEST */

}

/*********************************I2C & EEPROM Tests End *********************/

/***********************************BIB Tests Start **************************/
int
frcTestBIB (void)
{

#ifdef BIB_TEST
  unsigned int option = 0, deviceAddr;
  char ipAddr[20], dirName[50], fileName[30];
  int levelParm;

#ifdef TEST_DEBUG
  printf ("Inside frcTestBIB function\n");
#endif /*TEST_DEBUG */
#if 0
printf ("Attach the BIB device        ======>2\n");
#endif
printf ("Enter the device address (probabaly a8) in HEX\n");
scanf ("%x", &deviceAddr);
if (frcBibAttach (deviceAddr) == ERROR)
	{
	printf("\n BIB attach failed.");
	return ERROR;
	};

  while (1)
    {
      printf ("\n\n======Select the option=====\n\n");
      printf ("BIB data write               ======>1\n");
      printf ("Show BIB info                ======>2\n");

      printf ("Return back to main menu  =====>0\n");


      option = getOption ();
      switch (option)
	{
	case 0:
	  return 0;

	case 1:
	  printf ("\nMake sure network is up\nEnter IP address:");
	  scanf ("%s", ipAddr);
	  printf ("Enter directory name:");
	  scanf ("%s", dirName);
	  printf ("Enter file name:");
	  scanf ("%s", fileName);
	  frcBibDataWrite (ipAddr, dirName, fileName);
	  break;

	case 2:

	  printf ("Enter the level of BIB display :");
	  scanf ("%d", &levelParm);
	  if ( frcBibDrvShow (levelParm) == ERROR)
	  	printf("\nError displaying BIB data.");
	  break;

	default:
	  printf ("Unknown option!! try again\n");
	  break;
	}

    }

#else
  printf ("This Test is not supported\n");
#endif /*BIB_TEST */

}

/*********************************BIB Tests  End *****************************/

/***********************************VPD Tests Start **************************/
#ifdef VPD_TEST
LOCAL int
frcTestGetVPDInfo (unsigned int *pciBusNo,
		   unsigned int *pciDeviceNo, unsigned int *pciFuncNo)
{
  printf ("\nBus No     : ");
  scanf ("%u", pciBusNo);
  if ( pciBusNo > MAX_BUS )
  	{
  	printf ( "\n %d PCI bus is not supported", pciBusNo);
  	return ERROR;
  	}
  printf ("\nDevice No  : ");
  scanf ("%u", pciDeviceNo);
    if ( pciDeviceNo >= 32 )
  	{
  	printf ( "\n Incorrect %d PCI device number", pciDeviceNo);
  	return ERROR;
  	}
  printf ("\nFunction No: ");
  scanf ("%u", pciFuncNo);
      if ( pciFuncNo >= 8 )
  	{
  	printf ( "\n Incorrect %d PCI function number", pciFuncNo);
  	return ERROR;
  	}

  return 0;
}
#endif /*#ifdef VPD_TEST */

int
frcTestVPD (void)
{
#ifdef VPD_TEST
  unsigned int option = 0;
  unsigned int pciBusNo, pciDeviceNo, pciFuncNo, vpdAddr, vpdData,vpdData2;
  char ipAddr[20], dirName[50], fileName[30];

#ifdef TEST_DEBUG
  printf ("Inside frcTestVPD function\n");
#endif /*TEST_DEBUG */

  while (1)
    {
      printf ("\n\n======Select the Option=====\n\n");
      printf ("Program VPD Structure         ======>1\n");
      printf ("VPD write-verify              ======>2\n");
      printf ("VPD Read                      ======>3\n");


      printf ("Return back to main menu  =====>0\n");

      option = getOption ();
      switch (option)
	{
	case 0:
	  return 0;

	case 1:
	  printf ("\nMake sure network is up\nEnter IP address:");
	  scanf ("%s", ipAddr);
	  printf ("Enter directory name:");
	  scanf ("%s", dirName);
	  printf ("Enter file name:");
	  scanf ("%s", fileName);

	  if ( frcProgramLocalVPD (ipAddr, dirName, fileName) ==OK)
	  	printf("\nSuccess");
	  else
	  	printf("\nFailed.");
	  break;

	case 2:
	if ( frcTestGetVPDInfo (&pciBusNo, &pciDeviceNo, &pciFuncNo) == OK)
		{

		  if (true == frcVPDStrataWrite (pciBusNo, pciDeviceNo, pciFuncNo))
			{
			printf ("\nVPD write successful");
			vpdData2 = 0;
			printf ("\nVerifying ");
	  		frcVPDStrataRead (pciBusNo, pciDeviceNo, pciFuncNo, vpdAddr, &vpdData1);
	  		if ( vpdData != vpdData1 )
	  			{
	  			printf ("..Error\nWrote :0x%x, Read : 0x%x",vpdData, vpdData1);
	  			}
			else
				printf("..success.");
		  	}
	  	else
	    		{
		      	printf ("VPD write unsuccessful\n");
		      	}
		}
	  break;

	case 3:
	  frcTestGetVPDInfo (&pciBusNo, &pciDeviceNo, &pciFuncNo);
	  printf ("\nEnter VPD offset in HEX: ");
	  scanf ("%x", &vpdAddr);
	  frcVPDStrataRead (pciBusNo, pciDeviceNo, pciFuncNo, vpdAddr,
			    &vpdData);
	  printf ("Read 0x%x: 0x%x\n", vpdAddr,vpdData);
	  break;

	default:
	  printf ("Unknown option!! try again\n");
	  break;
	}

    }

#else
  printf ("This Test is not supported\n");
#endif /*VPD_TEST */
  return 0;
}

/*********************************VPD Tests End ***************************/



/*****************************User Flash Tests End **************************/

/*******************************Boot Flash Tests Start ***********************/
#ifdef BOOT_FLASH_TEST
int
frcTestBootFlash (void)
{

  char ipAddr[20], dirName[50], fileName[30];

#ifdef TEST_DEBUG
  printf ("Inside frcTestBootFlash function\n");
#endif /*TEST_DEBUG */

if ( isPMC280BootFlashPresent() == ERROR)
	{
	printf("\n Boot Flash is not present.");
	}
	else
	{
	  printf ("Enter IP address of the Host:");
	  scanf ("%s", ipAddr);
	  printf ("Enter directory name:");
	  scanf ("%s", dirName);
	  printf ("Enter file name:");
	  scanf ("%s", fileName);
	  frcBootFlashFile (ipAddr, dirName, fileName);
	}
  return 0;
}

#else
    #warning "Boot Flash test is not included."
/******************************Boot Flash Tests End **************************/
#endif /*BOOT_FLASH_TEST */

/*********************************TAU Tests Start ****************************/
#ifdef TAU_TEST
int
frcTestTAU (void)
{

  unsigned int option = 0;

#ifdef TEST_DEBUG
  printf ("Inside frcTestTAU function\n");
#endif /*TEST_DEBUG */

  while (1)
    {
      printf ("\n\n======Select the Option=====\n\n");
      printf ("Display the CPU core temperature   ======>1\n");
      printf ("Return back to main menu      =====>0\n");

      option = getOption ();
      switch (option)
	{
	case 0:
	  return 0;

	case 1:
	  frcTempShow ();
	  break;

	default:
	  printf ("Unknown option!! try again\n");
	  break;
	}

    }




}
#endif /*TAU_TEST */

/********************************TAU Tests End *******************************/

/*******************************Baud Rate Tests Start ************************/
int
frcTestBaudRate (void)
{
#ifdef BAUD_TEST

#else
  printf ("This Test is not supported\n");
#endif /*BAUD_TEST */
  return 0;
}

/******************************Baud Rate Tests End ***************************/

/*******************************RTC Tests Start ************************/
int
frcTestRTC (void)
{

	char time[50];
	int temp;
#ifdef RTC_TEST
  unsigned int option = 0;

#ifdef TEST_DEBUG
  printf ("Inside frcTestRTC function\n");
#endif /*TEST_DEBUG */

  while (1)
    {
      printf ("\n\n======Select the Option=====\n\n");
      printf ("Set RTC time       ======>1\n");
      printf ("Get RTC time       =====>2\n");

      printf ("Return back to Main menu   =====>0\n");

      option = getOption ();

      switch (option)
	{
	case 0:
	  return 0;

	case 1:
	


          scanf("%s",time);
          temp=settime(time); 
	  break;

	case 2:
	  gettime ();
	  break;

	default:
	  printf ("Unknown option!! try again\n");
	  break;
	}

    }

#else
  printf ("This Test is not supported\n");
#endif /*RTC_TEST */

}

/******************************RTC Tests End ***************************/

/******************************All Tests Start *******************************/
REGR_TEST_INFO testInfo[15] = {
/*                               {"Who Am I","frcTestWhoAmI",frcTestWhoAmI,0 },
                               {"CPU Core Frequecy","frcTestDispFreq",frcTestDispFreq,0},
                               {"Serial port Settings","",NULL,0},
                               {"ECC test","",frcTestChkECC,0},
                               {"IDMA Test","",NULL,0},
                               {"Main memory tests","",frcTestMemTest,0},
                               {"Memory performance tests","",frcTestMemPerf,0},
                               {"PCI Scan","",NULL,0},
                               {"Display Subsystem ID & Vendor ID","",NULL,0},
                               {"Display LAN settings","",NULL,0},
*/
  {"Who Am I", "frcTestWhoAmI()", frcTestWhoAmI, 1},
  {"CPU Core Frequecy", "frcTestDispFreq()", NULL, 1},
  {"Serial port Settings", "", NULL, 1},
  {"ECC test", "", NULL, 1},
  {"IDMA Test", "", NULL, 1},
  {"Main memory tests", "", NULL, 1},
  {"Memory performance tests", "", NULL, 1},
  {"PCI Tests", "", NULL, 1},
  {"Display Subsystem ID & Vendor ID", "", NULL, 1},
  {"Display LAN settings", "", NULL, 1},
};
int
frcTestAllTests (void)
{
#ifdef ALL_TEST

  int i = 0;
  for (i = 0; i < TEST_NUM_TESTS; i++)
    {
      printf
	("=================================================================\n");
      printf (" Test Name    :%s\n", testInfo[i].testName);
      printf (" Function name:%s\n", testInfo[i].funcName);
/*    testInfo[i].testStatus = testInfo[i].funcPtr(); */
      printf (" Test Status  :");
      if (0 == testInfo[i].testStatus)
	printf ("Test passed\n");
      else if (-1 == testInfo[i].testStatus)
	printf ("Test passed\n");
      else
	printf ("Test not performed\n");
      printf
	("=================================================================\n");
    }

#else
  printf ("This test is not supported\n");
#endif /*ALL_TEST */
  return 0;
}

/******************************All Tests End *********************************/

/*****************************frcTestSummary start****************************/
int
frcTestSummary (void)
{
  printf ("\n\t============ Tests Summary ================\n");
  return 0;
}

/*****************************frcTestSummary end 88***************************/

/**************Monarch tests end**********************************************/

/***************Function to display monarch or Non_Monarch Start**************/
int
frcTestIsMonarch (void)
{
int mode;
  if ((mode = isPMC280Monarch()) == ERROR)
  	
    printf (" Card is Non-Monarch mode\n");
  else
    printf (" Card is Monarch mode\n");

  return mode;
}


#if 0
/***************Function to display monarch or Non_Monarch End****************/

/********************Function to enable Interrupts in Non-Monarch start*******/
int
frcTestEnableInt_Non_monarch (void)
{
  printf ("This is not yet supported\n\n");
  return 0;
}

/********************Function to test Monarch/Non-Monarch ********/
int
frcTestMon_NonMon (void)
{
#ifdef MONARCH_TEST
  unsigned int option = 0;

#ifdef TEST_DEBUG
  printf ("Inside Monarch/Non-Monarc tests\n");
#endif /*TEST_DEBUG */

  while (1)
    {
      printf ("\n\n======Select the Option=====\n\n");
      printf ("Monarch/Non-Monarch  Detection      ======>1\n");
#ifdef DISABLE_PCI_INTR_MONARCH
      printf ("Disable Interrupts in Monarch mode  ======>2\n");
#endif

      printf ("Return back to Main menu   ======>0\n");

      option = getOption ();

      switch (option)
	{
	case 0:
	  return 0;

	case 1:
	  frcTestIsMonarch ();
	  break;

#ifdef DISABLE_PCI_INTR_MONARCH
	case 2:
	  frcTestEnableInt_Non_monarch ();
	  break;
#endif

	default:
	  printf ("Unknown option!! try again\n");
	  break;
	}

    }

#else
  printf ("This Test is not supported\n");
#endif /*MONARCH_TEST */

}
#endif
/**************Monarch tests end**********************************************/


/*****************MAC address tests start ************************************/

int
atox (char str[])
{
  int temp = 0, i = 0, ones = 0;
  while (str[i] != '\0')
    {
      if (str[i] >= '0' && str[i] <= '9')
	ones = str[i] - '0';
      else if (str[i] == 'a' || str[i] == 'A')
	ones = 10;
      else if (str[i] == 'b' || str[i] == 'B')
	ones = 11;
      else if (str[i] == 'c' || str[i] == 'C')
	ones = 12;
      else if (str[i] == 'd' || str[i] == 'D')
	ones = 13;
      else if (str[i] == 'e' || str[i] == 'E')
	ones = 14;
      else if (str[i] == 'f' || str[i] == 'F')
	ones = 15;
      else
	{
	  printf ("Inavalid string\n");
	  return -1;
	}
      temp = (16 * temp) + ones;
      i++;
    }
  return temp;
}

LOCAL int
frcTestMACProgram ()
{
  char temp[40], arr[6][3], *st, *end;
  int portNo, i = 0, MAC[6];

  printf ("Enter the port number :");
  scanf ("%d", &portNo);

  if ((0 != portNo) && (1 != portNo))
    {
      printf ("Unknown port!!...\n");
      return -1;
    }

if (( isPMC280TwoOnBoardEth() == ERROR) && (portNo == 1))
	{
	printf("\nOnly one on-board Ethernet port is supported.");
	return ERROR;
	}

 printf    ("Enter MAC address in the following format in Hex \nBYTE0:BYTE1:BYTE2:BYTE3:BYTE4:BYTE5 \nexample:  fe:ff:ff:00:07:00 \n\n\t:");
  scanf ("%s", temp);

  st = temp;
  for (i = 0; i < 6; i++)
    {
#ifdef MAC_TEST_DBG
      printf ("byte %d\n", i);
#endif /*#ifdef MAC_TEST_DBG */
      if (i < 5)
	{
	  end = strchr (st, ':');
	  if (NULL == end)
	    {
	      printf ("Invalid format1\n");
	      return -1;
	    }
	}
      else
	{
	  end = strchr (st, '\0');
	  if (NULL == end)
	    {
	      printf ("Invalid format2\n");
	      return -1;
	    }

	}

#ifdef MAC_TEST_DBG
      printf ("end-st%d\n", end - st);
#endif /*#ifdef MAC_TEST_DBG */
      if ((end - st) > 3)
	{
	  printf ("Invalid format3\n");
	  return -1;
	}
      strncpy (arr[i], st, end - st);
      arr[i][end - st] = '\0';
      st = end + 1;
    }
  for (i = 0; i < 6; i++)
    {
      MAC[i] = atox (arr[i]);
      if (-1 == MAC[i])
	{
	  printf ("returning\n");
	  return -1;
	}
    }
#ifdef   MAC_TEST_DBG
  printf ("Going to invoke the function frcMACWrite\n");
#endif /*MAC_TEST_DBG */

  frcMACWrite ((unsigned char) portNo, (unsigned char) MAC[0],
	       (unsigned char) MAC[1], (unsigned char) MAC[2],
	       (unsigned char) MAC[3], (unsigned char) MAC[4],
	       (unsigned char) MAC[5]);

  return 0;
}

int
frcTestMACAddr (void)
{
#ifdef MAC_TEST
  unsigned int option = 0;
  unsigned char buffer[10];
  unsigned int i = 0;
  while (1)
    {
      printf ("\n\n======Select the Option=====\n\n");
      printf ("MAC Address Programming      ======>1\n");
      printf ("MAC Address Display          ======>2\n");

      printf ("Return back to Main menu   ======>0\n");

      option = getOption ();

      switch (option)
	{
	case 0:
	  return 0;

	case 1:
	  frcTestMACProgram ();
	  break;

	case 2:
	  frcMACRead (0, buffer);
	  printf ("\nMAC Address for the port0: ");
	  for (i = 0; i < 6; i++)
	    {
	      printf ("%x", buffer[i]);
	      if (5 != i)
		printf (":");
	    }
               if ( isPMC280TwoOnBoardEth() == OK)
               	{
               	frcMACRead (1, buffer);
	   	printf ("\nMAC Address for the port1: ");
	  	for (i = 0; i < 6; i++)
	    		{
	      		printf ("%x", buffer[i]);
	      		if (5 != i)
				printf (":");
	    		}
               	}
	  break;

	default:
	  printf ("Unknown option!! try again\n");
	  break;
	}

    }

#else
  printf ("This Test is not supported\n");
#endif /*MAC_TEST */

}

/*****************MAC address tests end **************************************/

int
frcTestDispFreq (void)
{
#ifdef DISP_FREQ_TEST
  double temp;
  temp = getPLLCoreMultiplier ();
  if (temp < 0)
    {
      printf ("\nCan't use PLL value to calculate core frequency.");
      return ERROR;
    }
  printf ("\n Core (CPU) frequency = %ld (Core multiplier = %d)",
	  (long)(SYSCLOCK * temp), (int)temp);

#else
  printf ("This feature is not supported by BSP\n");
#endif /*DISP_FREQ_TEST */
  return TEST_SUCCESS;
}

double
getPLLCoreMultiplier (void)
{
  unsigned int hid1, pll;
  double mult;
  pll = hid1 = vxHid1Get ();	/*big endian */
  printf ("\npll = %u", pll);
  pll = BYTE_SWAP_32 (pll);
  printf ("\npll = %u", pll);
  pll = pll >> 14;
  printf ("\npll = %u", pll);
  pll &= 0x1f;
  printf ("\npll = %u", pll);
/* Bus to core multiplier*/
  mult = 0;
  switch (pll)
    {
    case 8:
      mult = 2;
      break;
    case 16:
      mult = 3;
      break;
    case 20:
      mult = 4;
      break;
    case 22:
      mult = 5;
      break;
    case 18:
      mult = 5.5;
      break;
    case 26:
      mult = 6;
      break;
    case 10:
      mult = 6.5;
      break;
    case 4:
      mult = 7;
      break;
    case 2:
      mult = 7.5;
      break;
    case 24:
      mult = 8;
      break;
    case 12:
      mult = 8.5;
      break;
    case 15:
      mult = 9;
      break;
    case 14:
      mult = 9.5;
      break;
    case 21:
      mult = 10;
      break;
    case 17:
      mult = 10.5;
      break;
    case 19:
      mult = 11;
      break;
    case 0:
      mult = 11.5;
      break;
    case 23:
      mult = 12;
      break;
    case 31:
      mult = 12.5;
      break;
    case 11:
      mult = 13;
      break;
    case 28:
      mult = 13.5;
      break;
    case 25:
      mult = 14;
      break;
    case 3:
      mult = 15;
      break;
    case 27:
      mult = 16;
      break;
    case 1:
      mult = 17;
      break;
    case 5:
      mult = 18;
      break;
    case 7:
      mult = 20;
      break;
    case 9:
      mult = 21;
      break;
    case 13:
      mult = 24;
      break;
    case 29:
      mult = 28;
      break;
    case 6:
    case 30:
      mult = -1;
      break;
    default:
      printf ("\nUnknown %d value", pll);
    }
  return mult;
}

unsigned int
testRdReg (unsigned int address)
{
#ifdef TORNADO_1_0_1
#ifndef BIG_ENDIAN_SUPPORT
  return (sysInLong (address));
#else
  return (BYTE_SWAP_32 (sysInLong (address)));
#endif
#else
 return (sysInLong (address));
#endif
}




int frcTestAltivec(void)
{
printf("\nAltivec is");
if ( 0x02000000 & vxMsrGet() )
	printf("enabled");
else
	printf ("disabled");
printf(".");
return OK;
}



int frcTestPciScan(void)
{
    int devnum,busno,funcno,data;
    UINT32 deviceVendorId , temp,subDeviceVendorId;
	

	UINT32 udwMcpFlag=0,udwhid0;


	/*
     * Disable the Machine Check Before Reading this, because
     * if we read this , it generates the IACK cycle and it will
     * generate a Machine Check Exception
     */

    udwhid0 = vxHid1Get();
    if(udwhid0 & 0x30000000)
    {
	    vxHid1Set(udwhid0 & ~0xb0000000);
	    udwMcpFlag = TRUE;
    }
    GT_REG_READ(GPP_VALUE, &temp);
    if ((temp & 0x00000010) == 0x00000000)
    {
 	/* Monarch mode */
	printf("\nThe MV64360 is in Monarch mode\n");
    }
    else
	printf("\nThe MV64360 is in Non Monarch mode\n");		

#if 0
    /* Set the GT PCI0 agent as master on PCI 0 or PCI 1*/
    if(pciActive == PCI_NUM0)
        pci0SetConfigReg(PCI_BUS0,0,0,PCI0_STATUS_COMMAND_REG,0x2800007);
    else
    {
    }
#endif

    for(busno = 0;busno < MAX_BUSES ;busno++)
    {
        for(devnum = 0;devnum < 32;devnum++)
        {
            for(funcno = 0;funcno < 8;funcno++)
            {
                frcPciConfigRead(busno,devnum,funcno,0,&deviceVendorId);
                if(deviceVendorId == 0xffffffff)
		{
		    if(funcno == 0)	
                    	break;
		    else
			continue;	/* It's a multifunction device*/	
		}
                else
                {
                       frcPciConfigRead(busno,devnum,funcno,0x2c,&subDeviceVendorId);
                    printf("Bus no. 0x%x Device no. 0x%x Function 0x%x \n\tDevice Id 0x%x Vendor Id 0x%x\n\tSubsystem Id 0x%x Subsystem Vendor Id 0x%x",busno,devnum,funcno,(deviceVendorId >> 16),(deviceVendorId & 0xFFFF),(subDeviceVendorId >> 16),(subDeviceVendorId & 0xFFFF));
                }
                frcPciConfigRead(busno,devnum,funcno,0x0c,&data);
                if((data & 0x00800000) == 0x00000000)
                    break;
            }		
	}
    } 			
    
/* Enable Machine Check Exception */
    if(udwMcpFlag == TRUE)
    {
   	vxHid1Set(udwhid0 | 0xb0000000);
	udwMcpFlag = FALSE;
    }

return OK;
}




int frcTestEthPHYTestMode(int port, int mode)
{
#ifdef FRC_TEST_ETH
    unsigned short phyReg1=0x09,  phyReg2 = 0x00;
    unsigned int temp1=0x0, temp2=0x0;

    /*read the contents of Phy Control register*/
    ethPortReadSmiReg(port, phyReg2, &temp2);

#ifdef FRC_TEST_ETH_DBG
    printf("\nvalue read at the reg :%x for port:%d is:%d",phyReg2,port,temp2);
#endif /*#ifdef TEST_DEBUG*/

    /*disable auto negotiation...vijay*/
    temp2 = temp2 & 0xefff; /*Disable auto-neg*/
    if(ethPortWriteSmiReg(port, phyReg2,temp2))
       printf("\nAuto-Negotiation disabled\n");

#ifdef FRC_TEST_ETH_DBG
    printf("\nvalue set at the reg :%x for port:%d is:%d",phyReg2,port,temp2);
#endif /*#ifdef TEST_DEBUG*/
    
    /*read the contents 1000BASE-T Control register*/
    ethPortReadSmiReg(port, phyReg1, &temp1);
    temp1 = temp1 & 0x1fff;

#ifdef FRC_TEST_ETH_DBG
    printf("\nvalue read at the reg :%x for port:%d is:%d",phyReg1,port,temp1);
#endif /*#ifdef TEST_DEBUG*/
    	
    switch(mode)
    {
         case 0:
         	return 0;
         case 1:
         	temp1 = temp1 | 0x2000;        	/*  Value of 13,14,15 bits: 100*/
      	      break;

         case 2:
         	temp1 = temp1 | 0x4000;          	/*  Value of 13,14,15 bits: 010*/
     	      break;

         case 3:
         	temp1 = temp1 | 0x6000;           	/*  Value of 13,14,15 bits: 110*/
      	      break;

         case 4:
         	temp1 = temp1 | 0x8000;           	/*  Value of 13,14,15 bits: xx1*/
      	      break;

         case 5:
         	temp1 = temp1 & 0x1fff;         /*    Value of 13,14,15 bits: 000*/
         default:
      	      printf("Unknown option\n");
             return 0;
     }
        
    /*Set the mode*/
    if(ethPortWriteSmiReg(port, phyReg1,temp1))
            printf("\nTest Mode setting successful\n");
    #ifdef FRC_TEST_ETH_DBG
        printf("\nvalue set at the reg :%x for port:%d is:%d",phyReg1,port,temp1);
    #endif /*#ifdef TEST_DEBUG*/
#else
printf("\nThis test is not supported\n");
#endif
    return 0;
}
