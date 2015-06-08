/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

 * if_i82557.c - Intel 82557 Ethernet network interface support library */

/*
 * Copyright 1989-1996 Wind River Systems, Inc.
 */

#include "copyright_wrs.h"

/*
modification history
--------------------
01b,07nov96,hdn  re-written.
01a,31aug96,dzb  written, based on v01a of src/drv/netif/if_iep.c.
Revision
July31 2002
Modified the Load string for FEI0 and FEI1 ,passing preallocated buffers
*/
              
/*
This module implements the Intel 82557 Ethernet network interface support
library.

Currently, this library only supports the Intel EtherExpress PRO100B LAN
Adapter.  If support for more 82557-based adapters is desired, this library
must be modified.

SEE ALSO: ifLib,
.I "Intel 82557 User's Manual,"
.I "Intel PRO100B PCI Adapter Driver Technical Reference,"
*/

#include "vxWorks.h"
#include "stdio.h"
#include "cacheLib.h"
#include "intLib.h"
#include "taskLib.h"
#include "sysLib.h"
#include "config.h"
#include "fei82557End.h"
#include "pciConfigLib.h"
#include "pmc280_pci.h" 
#include "end.h"

LOCAL void    mySysDelay ();   
extern  int DELAY (int someLoopCount);
extern	void	*gtPciIntrToVecNum (int intrLine, int bus, int device);

UINT32 addCacheControlBits(int unit, UINT32 sysAdr);
UINT32 remCacheControlBits(int unit,UINT32 localAdr);

IMPORT struct _ethernetdevice	feidevices[FEI_MAX_UNITS];
IMPORT int nooffeidevices;
LOCAL STATUS sys557LoadStrCompose (int unit);

#ifdef   INCLUDE_END_FEI
#define  FEI_LOAD_FUNC	 frcFei82557EndLoad    
#define  FEI_BUFF_LOAN   1           		

#define  FEI_LOAD_STRING0 "-1:0:0x20:0x20:0x00"
#define  FEI_LOAD_STRING1 "-1:0:0x20:0x20:0x00"
IMPORT END_OBJ* FEI_LOAD_FUNC (char*, void*);  
#endif

IMPORT END_TBL_ENTRY endDevTbl [];
/* defines */

#define I82557_DEBUG  /* TRUE for debug */
#ifdef   I82557_DEBUG
#undef   LOCAL
#define  LOCAL
#endif   /* I82557_DEBUG */

#define	TYPE_PRO100B_PCI	1

/* EEPROM control bits */

#define EE_SK		0x01		/* shift clock */
#define EE_CS		0x02		/* chip select */
#define EE_DI		0x04		/* chip data in */
#define EE_DO		0x08		/* chip data out */

/* EEPROM opcode */

#define EE_CMD_WRITE	0x05		/* WRITE opcode, 101 */
#define EE_CMD_READ	0x06		/* READ  opcode, 110 */
#define EE_CMD_ERASE	0x07		/* ERASE opcode, 111 */

/* EEPROM misc. defines */

#define EE_CMD_BITS	3		/* number of opcode bits */
#define EE_ADDR_BITS	6		/* number of address bits */
#define EE_DATA_BITS	16		/* number of data bits */
#define EE_SIZE		0x40		/* 0x40 words */
#define EE_CHECKSUM	0xbaba		/* checksum */



typedef struct feiResource		/* FEI_RESOURCE */
    {
    UINT32	membaseCsr;		/* Base Address Register 0 */
    UINT32	iobaseCsr;		/* Base Address Register 1 */
    UINT32 	membaseFlash;		/* Base Address Register 2 */
    char	irq;			/* Interrupt Request Level */
    UINT32	configType;		/* type of configuration - unused */
    UINT32	boardType;		/* type of LAN board this unit is */
    UINT32	pciBus;			/* PCI Bus number */
    UINT32	pciDevice;		/* PCI Device number */
    UINT32	pciFunc;		/* PCI Function number */
    UINT16	eeprom[0x40];		/* Ethernet Address of this unit */
    INT32	timeout;		/* timeout for the self-test */
    INT32	str[6];			/* storage for the self-test result */
    INT32	*pResults;		/* pointer to the self-test result */
    UINT        memLength;		/* required memory size */
    UINT        initialStateMask; 	/* mask parameter to vmStateSet */
    UINT        initialState;		/* state parameter to vmStateSet */

    } FEI_RESOURCE;


#define UNKNOWN 	-1

/* locals */



LOCAL FEI_RESOURCE feiResources [FEI_MAX_UNITS] =
    {
     /* membase, iobase, membaseF, irq */
    {UNKNOWN, UNKNOWN, UNKNOWN, /*ETHER_ON_BOARD_LVL*/(char)TARGET_INTR_A_VEC, UNKNOWN,
     UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, {UNKNOWN}, UNKNOWN, {UNKNOWN}, NULL,
     UNKNOWN, UNKNOWN, UNKNOWN},
    {UNKNOWN, UNKNOWN, UNKNOWN, /*ETHER_ON_BOARD_LVL*/(char)TARGET_INTR_A_VEC, UNKNOWN,
     UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, {UNKNOWN}, UNKNOWN, {UNKNOWN}, NULL,
     UNKNOWN, UNKNOWN, UNKNOWN},
#if (FEI_MAX_UNITS > 2)
    {UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
     UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, {UNKNOWN}, UNKNOWN, {UNKNOWN}, NULL,
     UNKNOWN, UNKNOWN, UNKNOWN},
    {UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
     UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, {UNKNOWN}, UNKNOWN, {UNKNOWN}, NULL,
     UNKNOWN, UNKNOWN, UNKNOWN},
#endif
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

int feiDevices[2] = {0,0};

/* forward declarations */

LOCAL UINT16   sys557eepromRead  (int unit, int location);
LOCAL UINT32   sys557mdioRead    (int unit, int phyId, int location);
LOCAL UINT32   sys557mdioWrite   (int unit, int phyId, int location, int value);
LOCAL int       sys557IntEnable   (int unit);
LOCAL int       sys557IntDisable  (int unit);
LOCAL int       sys557IntAck      (int unit);

/* The input to these functions are always in HOST order. 
* These make getting at bytes/words easier */
long	gtPciGetByte (long data, int byte);
long	gtPciSetByte (long data, long pciData, int byte);
long	gtPciGetWord (long data, int word);
long	gtPciSetWord (long data, long pciData, int word);





/*******************************************************************************
*
* frcSys557Init - prepare LAN adapter for 82557 initialization
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
*********************************************************************************/



STATUS frcSys557Init
    (
    int  unit,       /* unit number */
    BOARD_INFO *pBoard     /* board information */
    )
{
	FEI_RESOURCE *pResource = &feiResources [unit];
	UINT16 sum              = 0;
	int   ix;
	int   iy;
	UINT16 value;

	if(unit >= nooffeidevices)	/*Sanity check*/
		return(ERROR);

	/* read the configuration in EEPROM */
        for (ix = 0; ix < EE_SIZE; ix++) 
	{
		value = sys557eepromRead (unit, ix);
		pResource->eeprom[ix] = value;
		sum += value;
	}   

	if (sum != EE_CHECKSUM) 
	{
		printf ("i82557(%d): Invalid EEPROM checksum %#4.4x\n", unit, sum);
		pResource->eeprom[0] = 0x0100;
		pResource->eeprom[1] = 0x0302;
		pResource->eeprom[2] = 0x0004;
	}
			/* DP83840 specific setup */
	if (((pResource->eeprom[6]>>8) & 0x3f) == DP83840)
	{
		int reg23;
		reg23 = sys557mdioRead (unit, pResource->eeprom[6] & 0x1f, 23);
		sys557mdioWrite (unit, pResource->eeprom[6] & 0x1f, 23, reg23 | 0x0420);
	}
   #if 0       
          				
			/* perform a system self-test. */
			pResource->timeout     = 160000;      /* Timeout for set-test. */
	                pResource->pResults    = (UINT *) cacheDmaMalloc(8);
			pResource->pResults[0] = 0;
			pResource->pResults[1] = -1;
			printf("\nBefore sysOutLong !!!!");
			sysOutLong (pResource->iobaseCsr + SCB_PORT,((UINT32)pResource->pResults | 1));
			printf("\nAfter sysOutLong!!!!");
			mySysDelay ();  /* delay for some time */
			do 
			{    
				mySysDelay ();  /* delay for one IO READ cycle */
			} while ((pResource->pResults[1] == -1)  &&  (--pResource->timeout != 0)); 
   #endif
			
			
	pResource->boardType = TYPE_PRO100B_PCI;

	/* initializes the board information structure */
	pBoard->vector = pResource->irq;
	/*pBoard->intVector = *gtPciIntrToVecNum(pResource->irq,pResource->pciBus,pResource->pciDevice);*/
	pBoard->baseAddr   = pResource->membaseCsr;


    	for (ix = 0, iy = 0; ix < 3; ix++)
	{
		pBoard->enetAddr[iy++] = pResource->eeprom[ix] & 0xff;
		pBoard->enetAddr[iy++] = (pResource->eeprom[ix] >> 8) & 0xff;
	}

#ifdef  FEI_DEBUG
	/* Print out the MAC Address */
	printf("\n\rEnet HW address is ");
	for (ix = 0; ix < 6; ix++) 
	{
		if (ix!=0)
			printf(":");
		printf("%02X",pBoard->enetAddr[ix]);
	}
	printf ("\n");
#endif      
	pBoard->intEnable     = sys557IntEnable;
	pBoard->intDisable    = sys557IntDisable;
	pBoard->intAck        = sys557IntAck;

#ifdef FEI_10MB 
	pBoard->phySpeed   = NULL;
	pBoard->phyDpx     = NULL;
#endif
	
    return (OK);
}





STATUS sys557PciInit(void)
{

	int unit=0 , pciData , pciOrgData;
	FEI_RESOURCE *pResource;


        for(unit=0;unit < nooffeidevices;unit++)
	{
		
		pResource = &feiResources [unit];
		pResource->pciBus    = feidevices[unit].busno;
		pResource->pciDevice = feidevices[unit].deviceno;

		/* set memory base address and IO base address */
		frcPciConfigRead (pResource->pciBus,
				pResource->pciDevice,
				0,
				PCI_REG_BaseAddress_0,
				(UINT32 *)&pciData);
	
       		if (pciData == ~0)
			return (ERROR);
		pResource->membaseCsr = pciData & 0xfffffff0;
			
		frcPciConfigRead  (pResource->pciBus,
				pResource->pciDevice,
				0,
				PCI_REG_BaseAddress_1,
				(UINT32 *)&pciData);
				
		if (pciData == ~0)
				return (ERROR);
		pResource->iobaseCsr = pciData & 0xfffffffe;
            /*  This we don't know why on...
			*/
			/*pResource->iobaseCsr = pResource->membaseCsr;*/
					
		frcPciConfigRead  (pResource->pciBus,
				pResource->pciDevice,
				0,
				PCI_REG_BaseAddress_1 + 4,
				(UINT32 *)&pciData);
		if (pciData == ~0)
				return (ERROR);
		pResource->membaseFlash = pciData & 0xfffffff0;
			
		/* enable mapped memory and IO addresses */
			
		frcPciConfigRead  (pResource->pciBus,
				pResource->pciDevice,
				0,
				PCI_REG_Command,
				(UINT32 *)&pciOrgData);

		pciData = gtPciGetWord(pciOrgData, PCI_REG_Command);
		pciData |= PCI_REG_Command_IOSpace |
			PCI_REG_Command_MemSpace |
			PCI_REG_Command_BusMaster ;

		pciData = gtPciSetWord(pciData, pciOrgData, PCI_REG_Command);
		frcPciConfigWrite  (pResource->pciBus,
				pResource->pciDevice,
				0,
				PCI_REG_Command,
				pciData);

#if 0
			PRINTF (("frcSys557Init: unit %d: membaseCsr %x, iobaseCsr %x, membaseFlash %x, intr %d\n",
				unit, pResource->membaseCsr, pResource->iobaseCsr, pResource->membaseFlash, pResource->irq));
#endif

if (sys557LoadStrCompose(unit) == ERROR)
			return ERROR;	


	}    
	return(OK);
	
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

LOCAL STATUS sys557IntAck (int unit)
{
FEI_RESOURCE *pResource = &feiResources [unit];

   switch (pResource->boardType)
   {
      case TYPE_PRO100B_PCI:     /* handle PRO100B LAN Adapter */
      /* no addition work necessary for the PRO100B */
      break;

      default:
      return (ERROR);
   }
   return (OK);
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

LOCAL STATUS sys557IntEnable(int unit)
{
FEI_RESOURCE *pResource = &feiResources [unit];

   switch (pResource->boardType)
   {
      case TYPE_PRO100B_PCI:     /* handle PRO100B LAN Adapter */
       /* no addition work necessary for the PRO100B */
      intEnable(pResource->irq);
      break;

      default:
       return (ERROR);
   }

   return (OK);
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

LOCAL STATUS sys557IntDisable(int unit)
{
FEI_RESOURCE *pResource = &feiResources [unit];

   switch (pResource->boardType)
   {
   case TYPE_PRO100B_PCI:     /* handle PRO100B LAN Adapter */
       /* no addition work necessary for the PRO100B */
       intDisable(pResource->irq);   	
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

LOCAL UINT16 sys557eepromRead
    (
    int  unit,    /* unit number */
    int  location /* address of word to be read */
    )
    {
    UINT32 iobase = feiResources[unit].iobaseCsr;
    UINT16 retval = 0;
    UINT16 dataval;
    UINT16 dummy;
    int ix;

    sysOutWord (iobase + SCB_EEPROM, 0); /* disable EEPROM */    /* uri 27/12 - exception is here */
    mySysDelay (); /* delay for one IO READ cycle */
    mySysDelay (); /* delay for one IO READ cycle */

    sysOutWord (iobase + SCB_EEPROM, EE_CS); /* enable EEPROM */
    mySysDelay (); /* delay for one IO READ cycle */
 
    /* write the READ opcode */

    for (ix = EE_CMD_BITS - 1; ix >= 0; ix--)
    {
        dataval = (EE_CMD_READ & (1 << ix)) ? EE_DI : 0;
        sysOutWord (iobase + SCB_EEPROM, (short) (EE_CS | dataval));
        mySysDelay (); /* delay for one IO READ cycle */
        sysOutWord (iobase + SCB_EEPROM, (short) (EE_CS | dataval | EE_SK));
        mySysDelay (); /* delay for one IO READ cycle */
        sysOutWord (iobase + SCB_EEPROM, (short) (EE_CS | dataval)); 
        mySysDelay (); /* delay for one IO READ cycle */
    }

    /* write the location */

    for (ix = EE_ADDR_BITS - 1; ix >= 0; ix--)
    {
        dataval = (location & (1 << ix)) ? EE_DI : 0;
        sysOutWord (iobase + SCB_EEPROM, (short) (EE_CS | dataval));
        mySysDelay (); /* delay for one IO READ cycle */
        sysOutWord (iobase + SCB_EEPROM, (short) (EE_CS | dataval | EE_SK));
        mySysDelay (); /* delay for one IO READ cycle */
        sysOutWord (iobase + SCB_EEPROM, (short) (EE_CS | dataval)); 
        mySysDelay (); /* delay for one IO READ cycle */
        dummy = sysInWord (iobase + SCB_EEPROM);
        mySysDelay (); /* delay for one IO READ cycle */
    }

    if ((dummy & EE_DO) == 0)    /* dummy read */
    ;
 
    /* read the data */

    for (ix = EE_DATA_BITS - 1; ix >= 0; ix--)
    {
        sysOutWord (iobase + SCB_EEPROM, EE_CS | EE_SK);
        mySysDelay (); /* delay for one IO READ cycle */
        retval = (retval << 1) | 
            ((sysInWord (iobase + SCB_EEPROM) & EE_DO) ? 1 : 0);
        mySysDelay (); /* delay for one IO READ cycle */
        sysOutWord (iobase + SCB_EEPROM, EE_CS);
        mySysDelay (); /* delay for one IO READ cycle */
    }
 
    sysOutWord (iobase + SCB_EEPROM, 0x00);  /* disable EEPROM */

    return (retval);
}

/*******************************************************************************
*
* sys557mdioRead - read MDIO
*
* RETURNS: read value
*/

LOCAL UINT32 sys557mdioRead
    (
    int  unit,    /* unit number */
    int phyId,    /* PHY ID */
    int location  /* location to read */
    )
{
UINT32 iobase = feiResources[unit].iobaseCsr;
int timeout   = 64*4;  /* <64 usec. to complete, typ 27 ticks */
int val;

    sysOutLong (iobase + SCB_MDI, 0x08000000 | (location<<16) | (phyId<<21));
    do 
    {
      mySysDelay ();   /* delay for one IO READ cycle */
      val = sysInLong (iobase + SCB_MDI);
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

LOCAL UINT32 sys557mdioWrite (
    int unit,     /* unit number */
    int phyId,    /* PHY ID */
    int location, /* location to write */
    int value)    /* value to write */
{
UINT32 iobase = feiResources[unit].iobaseCsr;
int timeout   = 64*40;  /* <64 usec. to complete, typ 27 ticks */
int val;

    sysOutLong (iobase + SCB_MDI,
      0x04000000 | (location<<16) | (phyId<<21) | value);
    do 
    {
      mySysDelay ();   /* delay for one IO READ cycle */
      val = sysInLong (iobase + SCB_MDI);
      if (--timeout < 0)
          printf ("sys557mdioWrite() timed out with val = %8.8x.\n", val);
    } while (! (val & 0x10000000));

    return (val & 0xffff);
}

/*******************************************************************************
*
* sys557Show - shows 82557 configuration 
*
* this routine shows 82557 configuration 
*
* RETURNS: N/A
*/

void sys557Show (int  unit)
{
FEI_RESOURCE *pResource = (FEI_RESOURCE *)addCacheControlBits(0, (UINT32)&feiResources [unit]);
UCHAR etheraddr[6];
int ix;
int iy;

   for (ix = 0, iy = 0; ix < 3; ix++) 
   {
        etheraddr[iy++] = (uint8_t) pResource->eeprom[ix];
        etheraddr[iy++] = (uint8_t) (pResource->eeprom[ix] >> 8); 
   }
   printf ("82557(%d): Intel EtherExpress Pro 10/100 at %#3x and %#3x ", 
                       unit, feiResources[unit].iobaseCsr, feiResources[unit].membaseCsr);
   for (ix = 0; ix < 5; ix++)
     printf ("%2.2X:", etheraddr[ix]);
   printf ("%2.2X\n", etheraddr[ix]);

   if (pResource->eeprom[3] & 0x03)
        printf ("Receiver lock-up bug exists -- enabling work-around.\n");

   printf ("Board assembly %4.4x%2.2x-%3.3d, Physical connectors present:",
   pResource->eeprom[8], pResource->eeprom[9]>>8, pResource->eeprom[9] & 0xff);
   for (ix = 0; ix < 4; ix++)
      if (pResource->eeprom[5] & (1 << ix))
         printf (connectors [ix]);

   printf ("\nPrimary interface chip %s PHY #%d.\n",
   phys[(pResource->eeprom[6]>>8)&7], pResource->eeprom[6] & 0x1f);
   if (pResource->eeprom[7] & 0x0700)
      printf ("Secondary interface chip %s.\n", phys[(pResource->eeprom[7]>>8)&7]);

#if   FALSE
    /* ToDo: Read and set PHY registers through MDIO port. */
    for (ix = 0; ix < 2; ix++)
   printf ("MDIO register %d is %4.4x.\n",
      ix, sys557mdioRead (unit, pResource->eeprom[6] & 0x1f, ix));
    for (ix = 5; ix < 7; ix++)
   printf ("MDIO register %d is %4.4x.\n",
      ix, sys557mdioRead (unit, pResource->eeprom[6] & 0x1f, ix));
    printf ("MDIO register %d is %4.4x.\n",
       25, sys557mdioRead (unit, pResource->eeprom[6] & 0x1f, 25));
#endif   /* FALSE */

    if (pResource->timeout < 0) 
    {      /* Test optimized out. */
       printf ("Self test failed, status %8.8x:\n"
           " Failure to initialize the 82557.\n"
           " Verify that the card is a bus-master capable slot.\n",
           pResource->pResults[1]);
    }
    else 
    {
       printf ("General self-test: %s.\n"
           " Serial sub-system self-test: %s.\n"
           " Internal registers self-test: %s.\n"
           " ROM checksum self-test: %s (%#8.8x).\n",
           pResource->pResults[1] & 0x1000 ? "failed" : "passed",
           pResource->pResults[1] & 0x0020 ? "failed" : "passed",
           pResource->pResults[1] & 0x0008 ? "failed" : "passed",
           pResource->pResults[1] & 0x0004 ? "failed" : "passed",
           pResource->pResults[0]);
    }
}

/*
void sysOutWord (ULONG address, UINT16 data)
{
   *(UINT16 *)address = BYTE_SWAP_16(data);
}
void sysOutLong (ULONG address, ULONG data)
{
   *(UINT32 *)address = BYTE_SWAP_32(data);
}
UINT16 sysInWord (ULONG address)
{
   return (BYTE_SWAP_16(*(UINT16 *)address));
}
ULONG sysInLong (ULONG address)
{
   return (BYTE_SWAP_32(*(UINT32 *)address));
}
*/

int DELAY (int DelayCount)
{
int delay2 = 0;

   if (DelayCount) 
   {
      DelayCount--;
      delay2++;
   }
   return (delay2);
}
LOCAL void mySysDelay ()
{
 DELAY (1000);
}
UINT32 addCacheControlBits(int unit, UINT32 sysAdr)
{
    return(sysAdr); 
}

UINT32 remCacheControlBits(int unit, UINT32 localAdr)
{
   return (localAdr);
}


/* These make getting at bytes/words easier */
long
gtPciGetByte (
	long	data,			/* PCI Config Register value */
	int		byte			/* 0 to 3 */
)
{
	byte &= 0x03;
	data >>= 8 * byte;
	data &= 0xFF;
	return	data;
}
long
gtPciSetByte (
	long	data,			/* Data value */
	long	pciData,		/* PCI Config Register value */
	int		byte			/* 0 to 3 */
)
{
	byte &= 0x03;
	data &= 0xFF;
	data <<= 8 * byte;
	pciData &= ~(0xFF << (8 * byte));
	pciData |= data;
	return	pciData;
}
long
gtPciGetWord (
	long	data,			/* PCI Config Register value */
	int		word			/* 0 to 1 */
)
{
	word /= 2;
	word &= 0x01;
	data >>= 16 * word;
	data &= 0xFFFF;
	return	data;
}
long
gtPciSetWord (
	long	data,			/* Data value */
	long	pciData,		/* PCI Config Register value */
	int		word			/* 0 to 1 */
)
{
	word /= 2;
	word &= 0x01;
	data &= 0xFFFF;
	data <<= 16 * word;
	pciData &= ~(0xFFFF << (16 * word));
	pciData |= data;
	return	pciData;
}


/***********************************************************************
*
* sysPci32Swap - Swap 32 bits for a PCI access
*
* RETURNS: 32 bit data swapped.
*/

UINT32 sysPci32Swap 
    (
	UINT32      data 
    )
  {
	UINT32      dataswap;


	dataswap = LONGSWAP (data);

	return (dataswap);
  }

 
/***********************************************************************
*
* sysPci16Swap - Swap 16 bits for a PCI access
*
* RETURNS: 16 bit data swapped.
*/

UINT16 sysPci16Swap 
    (
	UINT16      data 
    )
  {
	UINT16      dataswap;

	dataswap =    MSB(data) | (LSB(data) << 8) ;

	return (dataswap);
  }

/*****************************************************************************/


/****************************************************************************
*
* sys557LoadStrCompose - Compose the END load string for the device
*
* The END device load string formed by this routine is in the following
* following format.
* <shMemBase>:<shMemSize>:<rxDesNum>:<txDesNum>:<usrFlags>:<offset>
*
* RETURN: N/A
*/

LOCAL STATUS sys557LoadStrCompose
    (
    int unit
    )
    {
    END_TBL_ENTRY      *pDevTbl;
    int i;

    if(unit >= FEI_MAX_UNITS)
	return(ERROR); 
    
    for (i=0,pDevTbl = endDevTbl; pDevTbl->endLoadFunc != END_TBL_END;
         pDevTbl++,i++);
  
    	if(i < 6)
         {
		pDevTbl->unit = unit;
		pDevTbl->endLoadFunc = FEI_LOAD_FUNC;
		if(unit == 0)
			pDevTbl->endLoadString = FEI_LOAD_STRING0;
		else
			pDevTbl->endLoadString = FEI_LOAD_STRING1;

		pDevTbl->endLoan = FEI_BUFF_LOAN;
		pDevTbl->pBSP	= NULL;
		pDevTbl->processed = FALSE;	
  
         }
     return OK;
     }

