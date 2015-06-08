/* sysFei82557End.c - system configuration module for fei82557End */
 
/* Copyright 1984 - 2002 Wind River Systems, Inc. */
 
/*
modification history
--------------------
01f,16jul02,pmr  removed debugging code.
01e,07mar02,kab  SPR 70817: *EndLoad returns NULL on failure
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

#if (defined(INCLUDE_FEI_END) && defined (INCLUDE_NETWORK)	\
     && defined (INCLUDE_END))
 
/* includes */

#include "vxWorks.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "end.h"
#include "config.h"
#include "mv5100.h" /* PCI_ID_I82559ER */

#include "drv/end/fei82557End.h"	

/* imports */

IMPORT FUNCPTR feiEndIntConnect;
IMPORT FUNCPTR feiEndIntDisconnect;
IMPORT END_OBJ * fei82557EndLoad (char *);
IMPORT void   sysUsDelay (UINT32);

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
 
 
/* forward declarations */

UINT16    sys557eepromRead (int unit, int location);
LOCAL UINT16    sys557mdioRead   (int unit, int phyId, int location);
LOCAL UINT16    sys557mdioWrite  (int unit, int phyId, int location, int value);
LOCAL int       sys557IntAck     (int unit);
LOCAL STATUS sys557IntDisable (int unit);
LOCAL STATUS sys557IntEnable (int unit);




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
    /*
     * The fei82557End driver END_LOAD_STRING should be:
     * "<memBase>:<memSize>:<nTfds>:<nRfds>:<flags>"
     *
     */

    char * cp; 			
    char paramStr [END_INIT_STR_MAX];   /* from end.h */
    END_OBJ * pEnd;

    if (strlen (pParamStr) == 0)
        {
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

        /* Now, we advance cp, by finding the end the string */

        cp += strlen (paramStr);
        
        /* finish off the initialization parameter string */

	sprintf (cp, "-1:-1:0x40:0x40:0x00");

        if ((pEnd = fei82557EndLoad (paramStr)) == (END_OBJ *)NULL)
	    {
            printf ("Error: device failed fei82557EndLoad routine.\n");
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
 
void sys557PciInit (void)
    {
    FEI_RESOURCE *pReso;
    int pciBus;
    int pciDevice;
    int pciFunc;
    int unit;
    BOOL duplicate;
    UINT32 membaseCsr;
    UINT32 iobaseCsr;
    UINT32 membaseFlash;
    char irq;
    int ix;
 
    for (unit = 0; unit < FEI_MAX_UNITS; unit++)
        {
        if (pciFindDevice (PRO100B_PCI_VENDOR_ID, (PCI_ID_I82559ER >> 16),
                           unit, &pciBus, &pciDevice, &pciFunc) != OK)
            break;
 
        /* check the duplicate */
 
        pReso     = &feiResources [0];
        duplicate = FALSE;
 
        for (ix = 0; ix < FEI_MAX_UNITS; ix++, pReso++)
            {
            if ((ix != unit) && (pReso->pciBus == pciBus) &&
                (pReso->pciDevice == pciDevice) && (pReso->pciFunc == pciFunc))
                duplicate = TRUE;
            }
 
        if (duplicate)
            continue;
 
        /* we found the right one */
 
        pReso = &feiResources [unit];
        pReso->pciBus    = pciBus;
        pReso->pciDevice = pciDevice;
        pReso->pciFunc   = pciFunc;

        /* get memory base address and IO base address */
 
        pciConfigInLong (pReso->pciBus, pReso->pciDevice, pReso->pciFunc,
                         PCI_CFG_BASE_ADDRESS_0, &membaseCsr);
        pciConfigInLong (pReso->pciBus, pReso->pciDevice, pReso->pciFunc,
                         PCI_CFG_BASE_ADDRESS_1, &iobaseCsr);
        pciConfigInLong (pReso->pciBus, pReso->pciDevice, pReso->pciFunc,
                         PCI_CFG_BASE_ADDRESS_2, &membaseFlash);
        pciConfigInByte (pReso->pciBus, pReso->pciDevice, pReso->pciFunc,
                         PCI_CFG_DEV_INT_LINE, &irq);
 
        membaseCsr   &= PCI_MEMBASE_MASK;
        iobaseCsr    &= PCI_IOBASE_MASK;
        membaseFlash &= PCI_MEMBASE_MASK;
 
        /* over write the resource table with read value */
 
        pReso->membaseCsr   = membaseCsr;
        pReso->iobaseCsr    = iobaseCsr; 
        pReso->membaseFlash = membaseFlash;
        pReso->irq          = irq;
 
        /* enable mapped memory and IO addresses */
 
        pciConfigOutWord (pReso->pciBus, pReso->pciDevice, pReso->pciFunc,
                          PCI_CFG_COMMAND, PCI_CMD_IO_ENABLE |
                          PCI_CMD_MEM_ENABLE | PCI_CMD_MASTER_ENABLE);
        feiUnits++;
        }
 
    /* specify the interrupt connect/disconnect routines to be used */
 
    feiEndIntConnect = (FUNCPTR) pciIntConnect;
    feiEndIntDisconnect = (FUNCPTR) pciIntDisconnect;
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
* RETURNS: OK or ERROR if the adapter could not be prepared for initialization.
*/
 
STATUS
sys557Init (
    int unit,                   /* unit number */
    FEI_BOARD_INFO *pBoard      /* board information for the end driver */
    )
   {
    FEI_RESOURCE *pReso = &feiResources [unit];
    UINT16 sum          = 0;
    int ix;
    int iy;
    UINT16 value;
char txt[100];
 
    if (unit >= feiUnits)
	{
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
		/*
sprintf(txt,"sys557eepromRead: [%3d] = 0x%04x\r\n",ix,value);
sysDebugMsg(txt,CONTINUE_EXECUTION);
		*/
        pReso->eeprom[ix] = value;
        sum += value;
      }
 
      if(sum != EE_CHECKSUM)
            printf ("i82557(%d): Invalid EEPROM checksum %#4.4x\n", unit, sum);
 
        /* DP83840 specific setup */
 
        if (((pReso->eeprom[6]>>8) & 0x3f) == DP83840)
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
                    (int)pReso->pResults | 1);
        do  {
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

#ifdef FEI_10MB
    pBoard->phySpeed      = NULL;
    pBoard->phyDpx        = NULL;
#endif


    /* User force full duplex and 100Mbit speed */
	/*
    pBoard->phySpeed    = PHY_100MBS;
    pBoard->phyDpx      = PHY_FULL_DPX;
	*/
 
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
 
    switch (pReso->boardType)
        {
        case TYPE_PRO100B_PCI:          /* handle PRO100B LAN Adapter */
           /* no addition work necessary for the PRO100B */
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
 
UINT16
sys557eepromRead (
    int unit,           /* unit number */
    int location        /* address of word to be read */
    )
{
  UINT32 membase = PCI_MEMIO2LOCAL(feiResources[unit].membaseCsr);
  UINT32 retval = 0;
  UINT32 dataval;
  volatile UINT32 dummy;
  int ix;
  /*
  char txt[100];
sprintf(txt,"sys557eepromRead: feiResources[%1d].membaseCsr = 0x%08x\r\n",
                 unit,feiResources[unit].membaseCsr);
sysDebugMsg(txt,CONTINUE_EXECUTION);
sprintf(txt,"sys557eepromRead: membase = 0x%08x\r\n",membase);
sysDebugMsg(txt,CONTINUE_EXECUTION);
  */
  sysOutWord(membase + SCB_EEPROM, EE_CS);    /* enable EEPROM */
 
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
 
    for (ix = EE_ADDR_BITS - 1; ix >= 0; ix--)
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
        ;
 
    /* read the data */
 
    for (ix = EE_DATA_BITS - 1; ix >= 0; ix--)
        {
        sysOutWord (membase + SCB_EEPROM, EE_CS | EE_SK);
 
        sysUsDelay (1000);    /* delay for one IO READ cycle */
 
        retval = (retval << 1) |
                 ((sysInWord (membase + SCB_EEPROM) & EE_DO) ? 1 : 0);
 
        sysOutWord (membase + SCB_EEPROM, EE_CS);
 
        sysUsDelay (1000);    /* delay for one IO READ cycle */
        }
 
    sysOutWord (membase + SCB_EEPROM, 0x00);     /* disable EEPROM */
 
    return (retval);
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
    UINT32 membase = PCI_MEMIO2LOCAL((feiResources[unit].membaseCsr));
    int timeout   = 64*4;       /* <64 usec. to complete, typ 27 ticks */
    int val;
 
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
    UINT32 membase       = PCI_MEMIO2LOCAL (pReso->membaseCsr);
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
 
#if     FALSE  /* we don't show PHY specific info at this time */
    /* ToDo: Read and set PHY registers through MDIO port. */
    for (ix = 0; ix < 2; ix++)
        printf ("MDIO register %d is %4.4x.\n",
                ix, sys557mdioRead (unit, pReso->eeprom[6] & 0x1f, ix));
   for (ix = 5; ix < 7; ix++)
        printf ("MDIO register %d is %4.4x.\n",
                ix, sys557mdioRead (unit, pReso->eeprom[6] & 0x1f, ix));
   printf ("MDIO register %d is %4.4x.\n",
            25, sys557mdioRead (unit, pReso->eeprom[6] & 0x1f, 25));
#endif  /* FALSE */
 
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
    return (LOCAL2PCI_MEMIO (adrs)); /* yk.h */
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
    return (PCI_MEMIO2LOCAL(adrs)); /* yk.h */
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

    return (intDisable (pReso->irq));
    }

#endif /* defined (INCLUDE_FEI_END) && defined (INCLUDE_NETWORK) */
