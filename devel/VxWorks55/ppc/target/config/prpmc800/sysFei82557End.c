/* sysFei82557End.c - system configuration module for fei82557End */
 
/* Copyright 1984 - 1999 Wind River Systems, Inc. */
/* Copyright 1999-2001 Motorola, Inc. All Rights Reserved */
 
/*
modification history
--------------------
01j,07mar02,kab  SPR 70817: *EndLoad returns NULL on failure
01i,17jan02,dtr  Fixing diab warnings.
01h,06dec01,dtr  Mod to distinguish between SECONDARY and TERTIARY_ENET.
01g,06dec01,dtr  Fix for CARRIER_1 board.
01f,15oct01,scb  shared memory fixes for new prpmc800 window mapping.
01e,28sep01,srr  Add PrPMC Adapter-specific definitions.
01d,10may01,pch  Add IMPORT declaration for sysUsDelay() to fix
		 compiler warning; use macro for PCI device ID.
01c,28jan00,jkf  enabling 557 interrupt after connecting ISR, SPR#30132.
01b,29apr99,jkf  merged with T2
01a,01apr99,jkf  written 
*/


/*
DESCRIPTION
This is the WRS-supplied configuration module for the VxWorks 
fei82557End (fei) END driver.  It performs the dynamic parameterization 
of the fei82557End driver.  This technique of 'just-in-time' 
parameterization allows driver parameter values to be declared 
as something other than static strings. 
*/


 
/* includes */

#include "vxWorks.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "end.h"
#include "config.h"
#include "prpmc800.h" /* PCI_ID_I82559ER */

/* mattr */
#include "logLib.h"

#include "drv/end/fei82557End.h"	

#if (defined(INCLUDE_FEI_END) && defined (INCLUDE_NETWORK)	\
     && defined (INCLUDE_END))

END_OBJ * sysFei82557EndLoad
    (
    char * pParamStr,   /* ptr to initialization parameter string */
    void * unused       /* unused optional argument */
    );


 
/* imports */

IMPORT FUNCPTR feiEndIntConnect;
IMPORT FUNCPTR feiEndIntDisconnect;
IMPORT END_OBJ * fei82557EndLoad (char *);
IMPORT void   sysUsDelay (UINT32);
IMPORT BOOL     sysMonarchMode;  /* TRUE if Monarch */


/* defines */

#ifdef I82557_DEBUG
#   undef       LOCAL
#   define      LOCAL
#endif  /* I82557_DEBUG */
 

/* Intel EtherExpress PRO100B LAN Adapter type */
 
#define TYPE_PRO100B_PCI        1       /* Intel EtherExpress PRO-100B PCI */
 
/* EEPROM control bits */
 
#define EE_SK           0x01            /* shift clock */
#define EE_CS           0x02            /* chip select */
#define EE_DI           0x04            /* chip data in */
#define EE_DO           0x08            /* chip data out */
 
/* EEPROM opcode */
 
#define EE_CMD_WRITE    0x05            /* WRITE opcode, 101 */
#define EE_CMD_READ     0x06            /* READ  opcode, 110 */
#define EE_CMD_ERASE    0x07            /* ERASE opcode, 111 */
 
/* EEPROM misc. defines */
 
#define EE_CMD_BITS     3               /* number of opcode bits */
#define EE_ADDR_BITS    6               /* number of address bits */
#define EE_DATA_BITS    16              /* number of data bits */
#define EE_SIZE         0x40            /* 0x40 words */
#define EE_CHECKSUM     0xbaba          /* checksum */
 
/* unknown values */

#define UNKNOWN         -1
 
/* Adapter-specific definitions */

#if (CARRIER_TYPE != PRPMC_G) && defined(INCLUDE_SECONDARY_ENET)
#   define NUM_END_DEVICES		2 
#else
#   define NUM_END_DEVICES		1
#endif /* (CARRIER_TYPE != PRPMC_G) && defined(INCLUDE_SECONDARY_ENET) */

#if (CARRIER_TYPE == PRPMC_CARRIER_1) && defined(INCLUDE_TERTIARY_ENET)
#   undef  NUM_END_DEVICES
#   define NUM_END_DEVICES		3 
#endif /* (CARRIER_TYPE == PRPMC_CARRIER_1) && defined(INCLUDE_TERTIARY_ENET) */
 
 
/* FEI driver access routines */
 
/* typedefs */
 
typedef struct feiResource              /* FEI_RESOURCE */
    {
    UINT32      membaseCsr;             /* Base Address Register 0 */
    UINT32      iobaseCsr;              /* Base Address Register 1 */
    UINT32      membaseFlash;           /* Base Address Register 2 */
    char        irq;                    /* Interrupt Request Level */
    UINT32      configType;             /* type of configuration - unused */
    UINT32      boardType;              /* type of LAN board this unit is */
    UINT32      pciBus;                 /* PCI Bus number */
    UINT32      pciDevice;              /* PCI Device number */
    UINT32      pciFunc;                /* PCI Function number */
    UINT32      eeprom[0x40];           /* Ethernet Address of this unit */
    INT32       timeout;                /* timeout for the self-test */
    INT32       str[6];                 /* storage for the self-test result */
    INT32       *pResults;              /* pointer to the self-test result */
    UINT        memLength;              /* required memory size */
    UINT        initialStateMask;       /* mask parameter to vmStateSet */
    UINT        initialState;           /* state parameter to vmStateSet */
    } FEI_RESOURCE;
 
 
/* locals */

LOCAL UINT32 sys557LocalToPciBusAdrs (int unit, UINT32 adrs);
LOCAL UINT32 sys557PciBusToLocalAdrs (int unit, UINT32 adrs);
LOCAL UINT32 feiUnits;                  /* number of FEIs we found */
LOCAL BOOL sys557PciInitdone=FALSE;
 
LOCAL FEI_RESOURCE feiResources [FEI_MAX_UNITS] =
    {
    {UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
     UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, {UNKNOWN}, UNKNOWN, {UNKNOWN}, NULL,
     UNKNOWN, UNKNOWN, UNKNOWN},
    {UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
     UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, {UNKNOWN}, UNKNOWN, {UNKNOWN}, NULL,
     UNKNOWN, UNKNOWN, UNKNOWN},
    {UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
     UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, {UNKNOWN}, UNKNOWN, {UNKNOWN}, NULL,
     UNKNOWN, UNKNOWN, UNKNOWN},
    {UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
     UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, {UNKNOWN}, UNKNOWN, {UNKNOWN}, NULL,
     UNKNOWN, UNKNOWN, UNKNOWN},
     };
 
LOCAL const char *phys[] =
    {
     "None", "i82553-A/B", "i82553-C", "i82503",
     "DP83840", "80c240", "80c24", "unknown"
     };
 
enum phy_chips
    {
    NonSuchPhy=0, I82553AB, I82553C, I82503,
    DP83840, S80C240, S80C24, UndefinedPhy
    };
 
LOCAL const char *connectors[] = {" RJ45", " BNC", " AUI", " MII"};

/* globals */
 
                                              /* i82559 buffer in low memory */
UCHAR lowMemBufFei[0x50000 + PPC_PAGE_SIZE];
#ifdef INCLUDE_SECONDARY_ENET
UCHAR lowMemBufFei1[0x50000 + PPC_PAGE_SIZE];
#  ifdef INCLUDE_TERTIARY_ENET
UCHAR lowMemBufFei2[0x50000 + PPC_PAGE_SIZE];
#  endif 
#endif /* INCLUDE_SECONDARY_ENET */


/* forward declarations */

UINT16    sys557eepromRead (int unit, int location);
LOCAL UINT16    sys557mdioRead   (int unit, int phyId, int location);
LOCAL UINT16    sys557mdioWrite  (int unit, int phyId, int location, int value);
LOCAL int       sys557IntAck     (int unit);
LOCAL STATUS sys557IntDisable (int unit);
LOCAL STATUS sys557IntEnable (int unit);
void sys557PciInit (void);
void sys557Show(int unit            /* unit number */);
STATUS sys557Init(int unit, FEI_BOARD_INFO *pBoard);

/******************************************************************************
*
* sysFei82557EndLoad - load fei82557 (fei) device.
*
* This routine loads the fei device with initial parameters. 
*
* RETURNS: pointer to END object or NULL.
*
* SEE ALSO: fei82557EndLoad()
*/
 
END_OBJ * sysFei82557EndLoad
    (
    char * pParamStr,   /* ptr to initialization parameter string */
    void * unused       /* unused optional argument */
    )
{
#ifdef PCI_AUTO_DEBUG
  char txt[1000];
#endif


    /*
     * The fei82557End driver END_LOAD_STRING should be:
     * "<memBase>:<memSize>:<nTfds>:<nRfds>:<flags>"
     *
     */
    
    char * cp; 			
    char paramStr [END_INIT_STR_MAX];   /* from end.h */
    END_OBJ * pEnd;

#ifdef PCI_AUTO_DEBUG
    sprintf(txt,"sysFei82557EndLoad reached.\r\n");
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

    /* read PCI configuration and initialize endDevices[] */
#ifndef SLAVE_OWNS_ETHERNET

#ifdef PCI_AUTO_DEBUG
    sprintf(txt,"sysFei82557EndLoad: SLAVE_OWNS_ETHERNET undefined\r\n");
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

    if(!sysMonarchMode)
    {
#ifdef PCI_AUTO_DEBUG
      sprintf(txt,"sysFei82557EndLoad: sysMonarchMode undefined\r\n");
      sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
      return ((END_OBJ *)NULL);
	}

#endif /* SLAVE_OWNS_ETHERNET */

    if(sys557PciInitdone == FALSE)
    {
#ifdef PCI_AUTO_DEBUG
      sprintf(txt,"sysFei82557EndLoad: calls sys557PciInit()\r\n");
      sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
      sys557PciInit();
      sys557PciInitdone = TRUE;
    }
 
    if(strlen (pParamStr) == 0)
    {
#ifdef PCI_AUTO_DEBUG
      sprintf(txt,"sysFei82557EndLoad: calls 1 fei82557EndLoad()\r\n");
      sysDebugMsg(txt,CONTINUE_EXECUTION);
	  /*
      sprintf(txt,"sysFei82557EndLoad: pParamStr >%60.60s<\r\n",(char *)pParamStr);
      sysDebugMsg(txt,CONTINUE_EXECUTION);
	  */
#endif

      /* 
       * muxDevLoad() calls us twice.  If the string is
       * zero length, then this is the first time through
       * this routine, so we just return.
       */

      pEnd = fei82557EndLoad (pParamStr);

    }
    else
	{
      /*
       * On the second pass though here, we actually create 
       * the initialization parameter string on the fly.   
       * Note that we will be handed our unit number on the 
       * second pass through and we need to preserve that information.
       * So we use the unit number handed from the input string.
       */
      cp = strcpy (paramStr, pParamStr); /* cp points to paramStr */

#ifdef PCI_AUTO_DEBUG
	  /*
      sprintf(txt,"sysFei82557EndLoad: after 'cpy' paramStr >%60.60s<\r\n",(char *)paramStr);
      sysDebugMsg(txt,CONTINUE_EXECUTION);
	  */
#endif

      /* Now, we advance cp, by finding the end the string */

      cp += strlen (paramStr);
#ifdef PCI_AUTO_DEBUG
	  /*
      sprintf(txt,"sysFei82557EndLoad: finally paramStr >%60.60s<\r\n",(char *)paramStr);
      sysDebugMsg(txt,CONTINUE_EXECUTION);
	  */
#endif
        
      /* align to cache boundary */
	  if(strcmp(pParamStr,"0:")==0) 
	  {
	    /* align to cache boundary */
	    sprintf (cp, "0x%x:0x%x:0x20:0x20:0x0",
                   ENET_BUF_ALIGN (lowMemBufFei),
                   sizeof(lowMemBufFei) -
                   (ENET_BUF_ALIGN (lowMemBufFei)
                    - (UINT32)(lowMemBufFei)));
#ifdef PCI_AUTO_DEBUG
      sprintf(txt,"sysFei82557EndLoad: align to cache boundary\r\n");
      sysDebugMsg(txt,CONTINUE_EXECUTION);
		/*
      sprintf(txt,"sysFei82557EndLoad: align to cache boundary >%60.60s<\r\n",(char *)paramStr);
      sysDebugMsg(txt,CONTINUE_EXECUTION);
		*/
#endif
	  }
#ifdef INCLUDE_SECONDARY_ENET
      else if(strcmp(pParamStr,"1:")==0) 
	  {
	    /* align to cache boundary */
	    sprintf (cp, "0x%x:0x%x:0x20:0x20:0x0",
                   ENET_BUF_ALIGN (lowMemBufFei1),
                   sizeof(lowMemBufFei1) -
                   (ENET_BUF_ALIGN (lowMemBufFei1)
                    - (UINT32)(lowMemBufFei1)));
	  }
#ifdef INCLUDE_TERTIARY_ENET
	  else if(strcmp(pParamStr,"2:")==0) 
	  {
	    /* align to cache boundary */
	    sprintf (cp, "0x%x:0x%x:0x20:0x20:0x0",
                     ENET_BUF_ALIGN (lowMemBufFei2),
                     sizeof(lowMemBufFei2) -
                     (ENET_BUF_ALIGN (lowMemBufFei2)
                      - (UINT32)(lowMemBufFei2)));
	  }
#endif
#endif
	  else
      {
        ;
#ifdef PCI_AUTO_DEBUG
        sprintf(txt,"sysFei82557EndLoad: Bad Unit.\r\n");
        sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
      }


#ifdef PCI_AUTO_DEBUG
      sprintf(txt,"sysFei82557EndLoad: calls 2 fei82557EndLoad.\r\n");
      sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

      if ((pEnd = fei82557EndLoad (paramStr)) == (END_OBJ *)NULL)
	  {
        logMsg("sysFei82557EndLoad: ERROR: device failed at fei82557EndLoad routine.\n",
          0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: ERROR: device failed at fei82557EndLoad routine.\n",
          0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: ERROR: device failed at fei82557EndLoad routine.\n",
          0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: ERROR: device failed at fei82557EndLoad routine.\n",
          0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: ERROR: device failed at fei82557EndLoad routine.\n",
          0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: ERROR: device failed at fei82557EndLoad routine.\n",
          0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: ERROR: device failed at fei82557EndLoad routine.\n",
          0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: ERROR: device failed at fei82557EndLoad routine.\n",
          0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: ERROR: device failed at fei82557EndLoad routine.\n",
          0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: ERROR: device failed at fei82557EndLoad routine.\n",
          0,0,0,0,0,0);
#ifdef PCI_AUTO_DEBUG
        sprintf(txt,"sysFei82557EndLoad: ERROR: device failed at fei82557EndLoad routine.\r\n");
        sysDebugMsg(txt,EXIT_TO_SYSTEM_MONITOR);
#endif
	  }
	  else
      {
        /*Sergey: print many times, otherwise it will not get through */
#ifdef MVME5500_DEBUG
        logMsg("sysFei82557EndLoad: device installed at fei82557EndLoad routine.\n",0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: device installed at fei82557EndLoad routine.\n",0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: device installed at fei82557EndLoad routine.\n",0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: device installed at fei82557EndLoad routine.\n",0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: device installed at fei82557EndLoad routine.\n",0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: device installed at fei82557EndLoad routine.\n",0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: device installed at fei82557EndLoad routine.\n",0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: device installed at fei82557EndLoad routine.\n",0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: device installed at fei82557EndLoad routine.\n",0,0,0,0,0,0);
        logMsg("sysFei82557EndLoad: device installed at fei82557EndLoad routine.\n",0,0,0,0,0,0);
#endif

#ifdef PCI_AUTO_DEBUG
        sprintf(txt,"sysFei82557EndLoad: device installed at fei82557EndLoad routine.\r\n");
        sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

      }
	}

  return (pEnd);
}



/*******************************************************************************
*
* sys557PciInit - prepare LAN adapter for 82557 initialization
*
* This routine find out the PCI device, and map its memory and IO address.
* It must be done prior to initializing the 82557, sys557Init().  Also
* must be done prior to MMU initialization, usrMmuInit().
*
* RETURNS: N/A
*/
 
void
sys557PciInit (void)
{
  FEI_RESOURCE *pReso;
  int pciBus[NUM_END_DEVICES];
  int pciDevice[NUM_END_DEVICES];
  int unit;
  UINT32 membaseCsr;
  UINT32 iobaseCsr;
  UINT32 membaseFlash;
  char irq;
  int no_of_feiUnits;
/*#ifdef PCI_AUTO_DEBUG1*/
  char txt[100];
/*#endif*/

  feiUnits=0;
  unit=0;
  pciBus[0]=0;

#ifdef PCI_AUTO_DEBUG
  sprintf(txt,"sys557PciInit reached\r\n");
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

    /* limit search to bus 0 */
     
#if (CARRIER_TYPE == PRPMC_CARRIER_1)
#ifdef PCI_AUTO_DEBUG
  sprintf(txt,"sys557PciInit: CARRIER_TYPE == PRPMC_CARRIER_1\r\n");
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
  pciBus[0] = PCI_CARRIER_1_PRI_BUS;
#endif /* (CARRIER_TYPE == PRPMC_CARRIER_1) */

  for(unit=0; unit<NUM_END_DEVICES; unit++)
  {
#ifdef PCI_AUTO_DEBUG
    sprintf(txt,"sys557PciInit: unit # %d :::\r\n",unit);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
    if(unit==0) 
    {
      if(sysMonarchMode)
      {
        pciDevice[0] = PCI_IDSEL_PRI_LAN;
#ifdef PCI_AUTO_DEBUG
        sprintf(txt,"sys557PciInit: Monarch mode: pciDevice[0]=0x%08x\r\n",pciDevice[0]);
        sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
      }
#if (CARRIER_TYPE == PRPMC_BASE)
      else
      {
        pciDevice[0] = PCI_IDSEL_SLAVE_LAN;
#ifdef PCI_AUTO_DEBUG
        sprintf(txt,"sys557PciInit: Slave mode: pciDevice[0]=0x%08x\r\n",pciDevice[0]);
        sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
      }
#endif
#if (CARRIER_TYPE == PRPMC_CARRIER_1) /* Sergey */
      else
      {
        pciDevice[0] = PCI_IDSEL_SLAVE_LAN;
#ifdef PCI_AUTO_DEBUG
        sprintf(txt,"sys557PciInit: Slave mode: pciDevice[0]=0x%08x\r\n",pciDevice[0]);
        sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
      }
#endif
    }

#ifdef INCLUDE_SECONDARY_ENET

    if(unit == 1)
    {
# if (CARRIER_TYPE==PRPMC_BASE)
	  /* not fei */
	  unit++;
      feiUnits--;
# else
#  if (CARRIER_TYPE==PRPMC_CARRIER_1)
	  pciBus[feiUnits] = 1;
#  endif
      pciDevice[feiUnits] = PCI_IDSEL_SEC_LAN;
# endif
    }

# if (CARRIER_TYPE == PRPMC_CARRIER_1)
#  ifdef INCLUDE_TERTIARY_ENET
	if(unit == 2)
    {
      pciDevice[feiUnits] = PCI_IDSEL_TER_LAN;
      pciBus[feiUnits] = 1;
    }
#  endif /* INCLUDE_TERTIARY_ENET */
# endif /* (CARRIER_TYPE == PRPMC_CARRIER_1) */

#endif /* INCLUDE_SECONDARY_ENET */

    feiUnits++;

  }

  no_of_feiUnits = feiUnits;
#ifdef PCI_AUTO_DEBUG
  sprintf(txt,"sys557PciInit: no_of_feiUnits = %d\r\n",no_of_feiUnits);
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
     
  for(feiUnits=0; feiUnits<no_of_feiUnits; feiUnits++)
  {
    pReso = &feiResources [feiUnits];
    pReso->pciBus    = pciBus[feiUnits];
    pReso->pciDevice = pciDevice[feiUnits];
    pReso->pciFunc   = 0;

#ifdef PCI_AUTO_DEBUG1
	/*
    logMsg("sys557PciInit: pciBus=%d pciDevice=%d pciFunc=%d\r\n",
      pReso->pciBus,pReso->pciDevice,pReso->pciFunc,0,0,0);
	*/
    sprintf(txt,"sys557PciInit: pciBus=%d pciDevice=%d pciFunc=%d\r\n",
      pReso->pciBus,pReso->pciDevice,pReso->pciFunc);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
	
#endif

    /* get memory base address and IO base address */	
    pciConfigInLong (pReso->pciBus, pReso->pciDevice, pReso->pciFunc,
                     PCI_CFG_BASE_ADDRESS_0, &membaseCsr);
    pciConfigInLong (pReso->pciBus, pReso->pciDevice, pReso->pciFunc,
                     PCI_CFG_BASE_ADDRESS_1, &iobaseCsr);
    pciConfigInLong (pReso->pciBus, pReso->pciDevice, pReso->pciFunc,
                     PCI_CFG_BASE_ADDRESS_2, &membaseFlash);
    pciConfigInByte (pReso->pciBus, pReso->pciDevice, pReso->pciFunc,
                     PCI_CFG_DEV_INT_LINE, &irq);

#ifdef PCI_AUTO_DEBUG1
	/*
    logMsg("sys557PciInit: membaseCsr=0x%08x iobaseCsr=0x%08x membaseFlash=0x%08x irq=0x%08x(%d)\r\n",
       membaseCsr,iobaseCsr,membaseFlash,irq,irq,0);
	*/
    sprintf(txt,"sys557PciInit: membaseCsr=0x%08x iobaseCsr=0x%08x membaseFlash=0x%08x irq=0x%08x(%d)\r\n",
       membaseCsr,iobaseCsr,membaseFlash,irq,irq);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
	
#endif

#ifdef SLAVE_OWNS_ETHERNET

#ifdef MVME5500

	/* Sergey: in mvme5500's interrupt routing table it set to 0x49, so
	   we have to change it to 0x1a; it is also possible to change
       mvme5500 table setting 0x1a there, and it also works
       (I do not quite understand all this ... ) */

	/*#ifdef PCI_AUTO_DEBUG*/
    /*sprintf(txt,"sys557PciInit: Sergey: set irq to 0x1a\r\n");
	  sysDebugMsg(txt,CONTINUE_EXECUTION);*/
	/*#endif*/
    irq = 0x1a;

#else
  if(irq==0)
  {
#ifdef PCI_AUTO_DEBUG
    sprintf(txt,"sys557PciInit: Sergey: irq==0, set it to 26\r\n");
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
    irq = 26;
  }
#endif

#endif

    membaseCsr   &= PCI_MEMBASE_MASK;
    iobaseCsr    &= PCI_IOBASE_MASK;
    membaseFlash &= PCI_MEMBASE_MASK;
 
    /* over write the resource table with read value */
    pReso->membaseCsr   = membaseCsr;
    pReso->iobaseCsr    = iobaseCsr; 
    pReso->membaseFlash = membaseFlash;
    pReso->irq          = irq;


#ifdef PCI_AUTO_DEBUG1
	/*
    logMsg("sys557PciInit: membaseCsr=0x%08x iobaseCsr=0x%08x membaseFlash=0x%08x irq=%d\r\n",
      membaseCsr,iobaseCsr,membaseFlash,irq,0,0);
	*/
    sprintf(txt,"sys557PciInit: membaseCsr=0x%08x iobaseCsr=0x%08x membaseFlash=0x%08x irq=%d\r\n",
      membaseCsr,iobaseCsr,membaseFlash,irq);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
	
#endif

    /* enable mapped memory and IO addresses */
    pciConfigOutWord (pReso->pciBus, pReso->pciDevice, pReso->pciFunc,
                      PCI_CFG_COMMAND, PCI_CMD_IO_ENABLE |
                      PCI_CMD_MEM_ENABLE | PCI_CMD_MASTER_ENABLE);
  }
  feiUnits=no_of_feiUnits;
#ifdef PCI_AUTO_DEBUG
  sprintf(txt,"sys557PciInit: feiUnits = %d\r\n",feiUnits);
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

  /* specify the interrupt connect/disconnect routines to be used */
  feiEndIntConnect = (FUNCPTR) intConnect;
  feiEndIntDisconnect = (FUNCPTR) intDisable;
}
 

/*******************************************************************************
*
* sys557Init - prepare LAN adapter for 82557 initialization
*
* This routine is expected to perform any adapter-specific or target-specific
* initialization that must be done prior to initializing the 82557.
*
* The 82557 driver calls this routine from the driver attach routine before
* any other routines in this library.
*
* This routine returns the interrupt level the <pIntLvl> parameter.
*
* Input parameters: 
*    unit - unit number
*    pBoard - board information for the end driver
*
* RETURNS: OK or ERROR if the adapter could not be prepared for initialization.
*/

STATUS
sys557Init(int unit, FEI_BOARD_INFO *pBoard)
{
  FEI_RESOURCE *pReso = &feiResources [unit];
  UINT16 sum          = 0;
  int ix;
  int iy;
  UINT16 value;
#ifdef PCI_AUTO_DEBUG
  char txt[100];
#endif
    
#ifdef PCI_AUTO_DEBUG
  sprintf(txt,"Inside sys557Init %d\r\n",unit);
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

  if(unit >= feiUnits)
  {
#ifdef PCI_AUTO_DEBUG
	sprintf(txt,"Bmattr: %d\r\n",unit);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
    return (ERROR);
  }
 
  /* locate the 82557 based adapter. PRO100B and XXX */
 
  if (pReso->boardType == TYPE_PRO100B_PCI)   /* only setup once */
  {
    ;
  }
  else
  {
    /* read the configuration in EEPROM */
    for(ix = 0; ix < EE_SIZE; ix++)
    {
      value = sys557eepromRead (unit, ix);
      pReso->eeprom[ix] = value;
      sum += value;
    }
 
    if(sum != EE_CHECKSUM)
    {
      logMsg("i82557(%d): Invalid EEPROM checksum 0x%04x != 0x%04x\r\n",
        unit, sum, EE_CHECKSUM,0,0,0);
#ifdef PCI_AUTO_DEBUG1
      sprintf(txt,"i82557(%d): Invalid EEPROM checksum 0x%04x != 0x%04x\r\n",
        unit, sum, EE_CHECKSUM);
      sysDebugMsg(txt,CONTINUE_EXECUTION);
	  
#endif
    }
    else
    {
#ifdef MVME5500_DEBUG
      logMsg("i82557(%d): Check EEPROM checksum: 0x%04x == 0x%04x\r\n",
        unit, sum, EE_CHECKSUM,0,0,0);
#endif

#ifdef PCI_AUTO_DEBUG1
      sprintf(txt,"i82557(%d): Check EEPROM checksum: 0x%04x == 0x%04x\r\n",
        unit, sum, EE_CHECKSUM);
      sysDebugMsg(txt,CONTINUE_EXECUTION);
      
#endif
    }

    /* DP83840 specific setup */
    if(((pReso->eeprom[6]>>8) & 0x3f) == DP83840)
    {
      int reg23 = sys557mdioRead (unit, pReso->eeprom[6] & 0x1f, 23);
      sys557mdioWrite (unit, pReso->eeprom[6] & 0x1f, 23, reg23 | 0x0420);
    }
 
    /* perform a system self-test. */
    pReso->timeout     = 16000;             /* Timeout for set-test.  */
    pReso->pResults    = (int *) ((((int) pReso->str) + 0xf) & ~0xf);
    pReso->pResults[0] = 0;
    pReso->pResults[1] = -1;

    sysOutLong (PCI_MEMIO2LOCAL ((pReso->membaseCsr + SCB_PORT)),
                    (int)pReso->pResults | 1 | PCI_SLV_MEM_BUS );

    do
    {
      sysUsDelay (1000);        /* delay for one IO READ cycle */
    } while ((pReso->pResults[1] == -1)  &&  (--pReso->timeout >= 0));

    pReso->boardType = TYPE_PRO100B_PCI;

  }
 
    /* initializes the board information structure */
 
    pBoard->vector        = pReso->irq + INT_NUM_IRQ0;
    pBoard->baseAddr      = pReso->membaseCsr;
    
    for (ix = 0, iy = 0; ix < 3; ix++)
        {
        pBoard->enetAddr[iy++] = pReso->eeprom[ix] & 0xff;
        pBoard->enetAddr[iy++] = (pReso->eeprom[ix] >> 8) & 0xff;
        }
 
    /*
    pBoard->intEnable     = sysLanIntEnable;
    pBoard->intDisable    = sysLanIntDisable;
    */
    pBoard->intEnable     = sys557IntEnable;
    pBoard->intDisable    = sys557IntDisable;
    pBoard->intAck        = sys557IntAck;
    pBoard->sysLocalToBus = sys557LocalToPciBusAdrs;
    pBoard->sysBusToLocal = sys557PciBusToLocalAdrs;

#ifdef MVME5500_DEBUG
    logMsg("vector = 0x%x baseAddr = 0x%x\r\n", pBoard->vector, pBoard->baseAddr,0,0,0,0);
    logMsg("intEnable = 0x%x intAck = 0x%x\r\n", pBoard->intEnable, pBoard->intAck,0,0,0,0);
    logMsg("sysLocalToBus = 0x%x sysBusToLocal = 0x%x\r\n",
      pBoard->sysLocalToBus, pBoard->sysBusToLocal,0,0,0,0);
    logMsg("CPU PCI BAse Address 0x%x\r\n",PCI_SLV_MEM_BUS,0,0,0,0,0); 
#endif

#ifdef PCI_AUTO_DEBUG1
    sprintf(txt,"vector = 0x%x baseAddr = 0x%x\r\n", pBoard->vector, pBoard->baseAddr);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    sprintf(txt,"intEnable = 0x%x intAck = 0x%x\r\n", pBoard->intEnable, pBoard->intAck);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    sprintf(txt,"sysLocalToBus = 0x%x sysBusToLocal = 0x%x\r\n",
      pBoard->sysLocalToBus, pBoard->sysBusToLocal);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    sprintf(txt,"CPU PCI BAse Address 0x%x\r\n",PCI_SLV_MEM_BUS); 
    sysDebugMsg(txt,CONTINUE_EXECUTION);
	
#endif
 
#ifdef FEI_10MB
    pBoard->phySpeed      = NULL;
    pBoard->phyDpx        = NULL;
#endif
 
    return (OK);
    }
 
/*******************************************************************************
*
* sys557IntAck - acknowledge an 82557 interrupt
*
* This routine performs any 82557 interrupt acknowledge that may be
* required.  This typically involves an operation to some interrupt
* control hardware.
*
* This routine gets called from the 82557 driver's interrupt handler.
*
* This routine assumes that the PCI configuration information has already
* been setup.
*
* RETURNS: OK, or ERROR if the interrupt could not be acknowledged.
*/
 
LOCAL STATUS sys557IntAck
(
  int unit            /* unit number */
)
{
  FEI_RESOURCE *pReso = &feiResources [unit];
  /*
  logMsg("sys557IntAck reached for unit # %d\n",unit,0,0,0,0,0);
  */
  switch (pReso->boardType)
  {
    case TYPE_PRO100B_PCI: /* handle PRO100B LAN Adapter */
                           /* no addition work necessary for the PRO100B */
	  /*
      logMsg("sys557IntAck: no addition work necessary for the PRO100B\n",0,0,0,0,0,0);
	  */
      break;
 
    default:
      return (ERROR);
  }
 
  return (OK);
}
 
 
/*******************************************************************************
*
* sys557eepromRead - read a word from the 82557 EEPROM
*
* RETURNS: the EEPROM data word read in.
*/
 
UINT16 sys557eepromRead
(
  int unit,           /* unit number */
  int location        /* address of word to be read */
)
{
  UINT32 membase;
  UINT32 retval = 0;
  UINT32 dataval;
  volatile UINT32 dummy;
  int ix;
#ifdef PCI_AUTO_DEBUG
  char txt[100];
#endif

  membase = PCI_MEMIO2LOCAL (feiResources[unit].membaseCsr);
#ifdef PCI_AUTO_DEBUG
  sprintf(txt,"Emattr: 0x%08x >> 0x%08x, location=0x%08x, SCB_EEPROM=0x%08x, EE_CS=0x%08x\r\n",
    feiResources[unit].membaseCsr, membase, location, SCB_EEPROM, EE_CS);
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

#ifdef PCI_AUTO_DEBUG
  dummy = sysInWord (membase + SCB_EEPROM);
   sprintf(txt,"dummy1=0x%08x\r\n",dummy);
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
 
  sysOutWord (membase + SCB_EEPROM, EE_CS);    /* enable EEPROM */
 
#ifdef PCI_AUTO_DEBUG
  dummy = sysInWord (membase + SCB_EEPROM);
   sprintf(txt,"dummy2=0x%08x\r\n",dummy);
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

  /* write the READ opcode */
 
  for(ix = EE_CMD_BITS - 1; ix >= 0; ix--)
  {
    dataval = (EE_CMD_READ & (1 << ix)) ? EE_DI : 0;
 
    sysOutWord (membase + SCB_EEPROM, EE_CS | dataval);
 
    sysUsDelay (1000);    /* delay for one IO READ cycle */
 
    sysOutWord (membase + SCB_EEPROM, EE_CS | dataval | EE_SK);
 
    sysUsDelay (1000);    /* delay for one IO READ cycle */
  }
 
  /* write the location */
 
  for(ix = EE_ADDR_BITS - 1; ix >= 0; ix--)
  {
    dataval = (location & (1 << ix)) ? EE_DI : 0;
    sysOutWord (membase + SCB_EEPROM, EE_CS | dataval);
    sysUsDelay (1000);    /* delay for one IO READ cycle */
    sysOutWord (membase + SCB_EEPROM, EE_CS | dataval | EE_SK);
    sysUsDelay (1000);    /* delay for one IO READ cycle */
    sysOutWord (membase + SCB_EEPROM, EE_CS | dataval);
    sysUsDelay (1000);    /* delay for one IO READ cycle */
    dummy = sysInWord (membase + SCB_EEPROM);
  }
 
  if ((dummy & EE_DO) == 0)           /* dummy read */
  {
    ;
  }

  /* read the data */
 
  for(ix = EE_DATA_BITS - 1; ix >= 0; ix--)
  {
    sysOutWord (membase + SCB_EEPROM, EE_CS | EE_SK);
 
    sysUsDelay (1000);    /* delay for one IO READ cycle */
 
    retval = (retval << 1) |
                 ((sysInWord (membase + SCB_EEPROM) & EE_DO) ? 1 : 0);
 
    sysOutWord (membase + SCB_EEPROM, EE_CS);
 
    sysUsDelay (1000);    /* delay for one IO READ cycle */
  }
 
  sysOutWord (membase + SCB_EEPROM, 0x00);     /* disable EEPROM */
 
  return(retval);
}
 
/*******************************************************************************
*
* sys557mdioRead - read MDIO
*
* RETURNS: read value
*/
 
LOCAL UINT16 sys557mdioRead
    (
    int unit,           /* unit number */
    int phyId,          /* PHY ID */
    int location        /* location to read */
    )
    {
    UINT32 membase;
    int timeout   = 64*4;       /* <64 usec. to complete, typ 27 ticks */
    int val;
    membase = PCI_MEMIO2LOCAL((feiResources[unit].membaseCsr));
     
    sysOutLong (membase + SCB_MDI, 0x08000000 | (location<<16) | (phyId<<21));
 
    do {
        sysUsDelay (1000);    /* delay for one IO READ cycle */
        val = sysInLong (membase + SCB_MDI);
        if (--timeout < 0)
            printf ("sys557mdioRead() timed out with val = %8.8x.\n", val);
        } while (! (val & 0x10000000));
 
    return (val & 0xffff);
    }
 
/*******************************************************************************
*
* sys557mdioWrite - write MDIO
*
* RETURNS: write value
*/
 
LOCAL UINT16 sys557mdioWrite
    (
    int unit,           /* unit number */
    int phyId,          /* PHY ID */
    int location,       /* location to write */
    int value           /* value to write */
    )
    {
    UINT32 membase = PCI_MEMIO2LOCAL((feiResources[unit].membaseCsr));
    int timeout   = 64*4;       /* <64 usec. to complete, typ 27 ticks */
    int val;
 
    sysOutLong (membase + SCB_MDI,
                0x04000000 | (location<<16) | (phyId<<21) | value);
    do {
        sysUsDelay (1000);    /* delay for one IO READ cycle */
        val = sysInLong (membase + SCB_MDI);
        if (--timeout < 0)
            printf ("sys557mdioWrite() timed out with val = %8.8x.\n", val);
        } while (! (val & 0x10000000));
 
    return (val & 0xffff);
    }
 
/*******************************************************************************
*
* sys557Show - shows 82557 configuration
*
* this routine shows (Intel Pro Express 100) configuration
*
* RETURNS: N/A
*/
 
void sys557Show
    (
    int unit            /* unit number */
    )
{
    FEI_RESOURCE *pReso = &feiResources [unit];
    UINT32 membase        = PCI_MEMIO2LOCAL (pReso->membaseCsr);
    UCHAR etheraddr[6];
    int ix;
    int iy;
    if (unit >= feiUnits)
        {
        printf ("none\n");
        return;
        }
 
    for (ix = 0, iy = 0; ix < 3; ix++)
        {
        etheraddr[iy++] = pReso->eeprom[ix];
        etheraddr[iy++] = pReso->eeprom[ix] >> 8;
        }
 
    printf ("82557(%d): Intel EtherExpress Pro 10/100 at %#3x ", unit, membase);
 
    for (ix = 0; ix < 5; ix++)
        printf ("%2.2X:", etheraddr[ix]);
 
    printf ("%2.2X\n", etheraddr[ix]);
 
    printf ("CSR mem base address = 0x%x, Flash mem base address = 0x%x\n",
            pReso->membaseCsr, pReso->membaseFlash);
    printf ("PCI bus no. = 0x%x, device no. = 0x%x"
            ", function no. = 0x%x, IRQ = %d\n",
            pReso->pciBus, pReso->pciDevice, pReso->pciFunc, pReso->irq);
 
    if (pReso->eeprom[3] & 0x03)
        printf ("Receiver lock-up bug exists -- enabling work-around.\n");
 
    printf ("Board assembly %4.4x%2.2x-%3.3d, Physical connectors present:",
        pReso->eeprom[8], pReso->eeprom[9]>>8, pReso->eeprom[9] & 0xff);

    for (ix = 0; ix < 4; ix++)
        if (pReso->eeprom[5] & (1 << ix))
            printf (connectors [ix]);
 
    printf ("\nPrimary interface chip %s PHY #%d.\n",
        phys[(pReso->eeprom[6]>>8)&7], pReso->eeprom[6] & 0x1f);
    if (pReso->eeprom[7] & 0x0700)
        printf ("Secondary interface chip %s.\n",
                phys[(pReso->eeprom[7]>>8)&7]);
 

    /* ToDo: Read and set PHY registers through MDIO port. */
    for (ix = 0; ix < 2; ix++)
        printf ("MDIO register %d is %4.4x.\n",
                ix, sys557mdioRead (unit, pReso->eeprom[6] & 0x1f, ix));
   for (ix = 5; ix < 7; ix++)
        printf ("MDIO register %d is %4.4x.\n",
                ix, sys557mdioRead (unit, pReso->eeprom[6] & 0x1f, ix));
   printf ("MDIO register %d is %4.4x.\n",
            25, sys557mdioRead (unit, pReso->eeprom[6] & 0x1f, 25));
 
    if (pReso->timeout < 0)
        {               /* Test optimized out. */
        printf ("Self test failed, status %8.8x:\n"
                " Failure to initialize the 82557.\n"
                " Verify that the card is a bus-master capable slot.\n",
               pReso->pResults[1]);
        }
    else
        {
        printf ("General self-test: %s.\n"
                " Serial sub-system self-test: %s.\n"
                " Internal registers self-test: %s.\n"
                " ROM checksum self-test: %s (%#8.8x).\n",
                pReso->pResults[1] & 0x1000 ? "failed" : "passed",
                pReso->pResults[1] & 0x0020 ? "failed" : "passed",
                pReso->pResults[1] & 0x0008 ? "failed" : "passed",
                pReso->pResults[1] & 0x0004 ? "failed" : "passed",
                pReso->pResults[0]);
        }
    }
 
/******************************************************************************
*
* sys557LocalToPciBusAdrs - convert a local address to a bus address
*
* This routine returns a PCIbus address for the LOCAL bus address.
*
*/
 
LOCAL UINT32 sys557LocalToPciBusAdrs
    (
    int unit,
    UINT32      adrs    /* Local Address */
    )
{
#ifdef PCI_AUTO_DEBUG
  char txt[100];
#endif

  if(adrs>PCI2DRAM_MAP_SIZE)
  {
#ifdef PCI_AUTO_DEBUG
    sprintf(txt,"Local To Pci Address is 0x%x\r\n",(int)adrs);    
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
  }

  return (LOCAL2PCI_MEMIO(adrs));
}
 
 
/******************************************************************************
*
* sys557PciBusToLocalAdrs - convert a bus address to a local address
*
* This routine returns a local address that is used to access the PCIbus.
* The bus address that is passed into this routine is the PCIbus address
* as it would be seen on the local bus.
*
*/
 
LOCAL UINT32 sys557PciBusToLocalAdrs
    (
    int unit,
    UINT32      adrs    /* PCI Address */
    )
{
  volatile int localAdrs;
#ifdef PCI_AUTO_DEBUG
  char txt[100];
#endif

  localAdrs = PCI_MEMIO2LOCAL(adrs);
  if(localAdrs>PCI2DRAM_MAP_SIZE)
  {
#ifdef PCI_AUTO_DEBUG
    sprintf(txt,"Pci Bus To Local Address is 0x%x\r\n",(int)localAdrs);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
  }

  return(localAdrs);
}


/*******************************************************************************
*
* sys557IntEnable - enable 82557 interrupts
*
* This routine enables 82557 interrupts.  This may involve operations on
* interrupt control hardware.
*
* The 82557 driver calls this routine throughout normal operation to terminate
* critical sections of code.
*
* This routine assumes that the PCI configuration information has already
* been setup.
*
* RETURNS: OK, or ERROR if interrupts could not be enabled.
*/

LOCAL STATUS sys557IntEnable
(
  int unit            /* unit number */
)
{
  FEI_RESOURCE *pReso = &feiResources [unit];
#ifdef MVME5500_DEBUG
  logMsg("sys557IntEnable reached for unit # %d\n",unit,0,0,0,0,0);
#endif

  return (intEnable (pReso->irq));
}

/*******************************************************************************
*
* sys557IntDisable - disable 82557 interrupts
*
* This routine disables 82557 interrupts.  This may involve operations on
* interrupt control hardware.
*
* The 82557 driver calls this routine throughout normal operation to enter
* critical sections of code.
*
* This routine assumes that the PCI configuration information has already
* been setup.
*
* RETURNS: OK, or ERROR if interrupts could not be disabled.
*/

LOCAL STATUS sys557IntDisable
(
  int unit            /* unit number */
)
{
  FEI_RESOURCE *pReso = &feiResources [unit];
#ifdef MVME5500_DEBUG
  logMsg("sys557IntDisable reached for unit # %d\n",unit,0,0,0,0,0);
#endif
  return (intDisable (pReso->irq));
}

#endif /* defined (INCLUDE_FEI_END) && defined (INCLUDE_NETWORK) */
