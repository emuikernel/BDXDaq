/* if_cs.c - Crystal Semiconductor CS8900 network interface driver */

/*
 * Copyright 1996 Crystal Semiconductor Corp.
 * Copyright 1984-1997 Wind River Systems, Inc.
 */

#include "copyright_wrs.h"

/*
modification history
--------------------
01c,23feb98,vin  converted to BSD44 driver, merge from SENS
01b,13jun97,map  doc updates [SPR# 8138]
01a,16dec96,hdn  adapted.
*/

/*
DESCRIPTION

This module implements a driver for a Crystal Semiconductor CS8900 Ethernet
controller chip.

The CS8900 is a single chip Ethernet controller with a direct ISA bus
interface which can operate in either memory space or I/O space. It also
supports a direct interface to a host DMA controller to transfer receive
frames to host memory.  The device has a 4K RAM which is used for transmit,
and receive buffers; a serial EEPROM interface; and both 10BASE-T/AUI port
support.

This driver is capable of supporting both memory mode and I/O mode operations
of the chip. When configured for memory mode, the intenal RAM of the chip is
mapped to a contiguous 4K address block, providing the CPU direct access to
the internal registers and frame buffers. When configured for I/O mode, the
internal registers are accessible through eight contiguous, 16-bit I/O ports.
The driver also supports an interface to an EEPROM containing device
configuration.

While the DMA slave mode is supported by the device for receive frame
transfers, this driver does not enable DMA.

This network interface driver does not support output hook routines,
because to do so requires that an image of the transmit packet be built in
memory before the image is copied to the CS8900 chip.  It is much more
efficient to copy the image directly from the mbuf chain to the CS8900 chip.
However, this network interface driver does support input hook routines.

CONFIGURATION
The defined I/O address and IRQ in config.h must match the one stored
in EEPROM by the vendor's DOS utility program.

The I/O Address parameter is the only required csAttach() parameter.  If the
CS8900 chip has a EEPROM attached, then the I/O Address parameter, passed to
the csAttach() routine, must match the I/O address programmed into the EEPROM.
If the CS8900 chip does not have a EEPROM attached, then the I/O Address
parameter must be 0x300.

The Interrupt Level parameter must have one of the following values:
   0  - Get interrupt level from EEPROM
   5  - IRQ 5
   10 - IRQ 10
   11 - IRQ 11
   12 - IRQ 12

If the Interrupt Vector parameter is zero, then the network interface driver
derives the interrupt vector from the interrupt level if possible.  It is
possible to derive the interrupt vector in an IBM PC compatible system.  This
parameter is present for systems which are not IBM PC compatible.

The Memory Address parameter specifies the base address of the CS8900 chip's
memory buffer (PacketPage).  If the Memory Address parameter is not zero, then
the CS8900 chip operates in memory mode at the specified address.  If the
Memory Address parameter is zero, then the CS8900 chip operates in the mode
specified by the EEPROM or the Configuration Flags parameter.

The Media Type parameter must have one of the following values:
.CS
 0 - Get media type from EEPROM
 1 - AUI  (Thick Cable)
 2 - BNC  10Base2 (Thin Cable)
 3 - RJ45 10BaseT (Twisted Pair)
.CE

The Configuration Flags parameter is usually passed to the csAttach() routine
as zero and the Configuration Flags information is retrieved from the EEPROM.
The bits in the Configuration Flags parameter are usually specified by a
hardware engineer and not by the end user.  However, if the CS8900 chip does
not have a EEPROM attached, then this information must be passed as a parameter
to the csAttach() routine.  The Configuration Flags are:
.CS
 0x8000 - CS_CFGFLG_NOT_EEPROM    Don't get Config. Flags from the EEPROM
 0x0001 - CS_CFGFLG_MEM_MODE      Use memory mode to access the chip
 0x0002 - CS_CFGFLG_USE_SA        Use system addr to qualify MEMCS16 signal
 0x0004 - CS_CFGFLG_IOCHRDY       Use IO Channel Ready signal to slow access
 0x0008 - CS_CFGFLG_DCDC_POL      The DC/DC conv. enable pin is active high
 0x0010 - CS_CFGFLG_FDX           10BaseT is full duplex
.CE

If configuration flag information is passed to the csAttach() routine, then the
CS_CFGFLG_NOT_EEPROM flag should be set.  This ensures that the Configuration
Flags parameter is not zero, even if all specified flags are zero.

If the Memory Address parameter is not zero and the Configuration Flags
parameter is zero, then the CS8900 network interface driver implicitly sets
the CS_CFGFLG_MEM_MODE flag and the CS8900 chip operates in memory mode.
However, if the Configuration Flags parameter is not zero, then the CS8900
chip operates in memory mode only if the CS_CFGFLG_MEM_MODE flag is explicitly
set.  If the Configuration Flags parameter in not zero and the
CS_CFGFLG_MEM_MODE flag is not set, then the CS8900 chip operates in I/O mode.

The Ethernet Address parameter is usually passed to the csAttach() routine as
zero and the Ethernet address is retrieved from the EEPROM.  The Ethernet
address (also called hardware address and individual address) is usually
supplied by the adapter manufacturer and is stored in the EEPROM.  However, if
the CS8900 chip does not have a EEPROM attached, then the Ethernet address must
be passed as a parameter to the csAttach() routine.
The Ethernet Address parameter, passed to the csAttach() routine, contains the
address of a NULL terminated string.  The string consists of 6 hexadecimal
numbers separated by colon characters.  Each hexadecimal number is in the range
00 - FF.  An example of this string is:
.CS
   "00:24:20:10:FF:2A"
.CE

BOARD LAYOUT
This device is soft-configured.  No jumpering diagram is required.

EXTERNAL INTERFACE
The only user-callable routines are csAttach():
.iP csAttach() 14
publishes the `cs' interface and initializes the driver and device.
.LP
The network interface driver includes a show routine, called csShow(), which
displays driver configuration and statistics information.  To invoke the show
routine, type at the shell prompt:
.CS
        -> csShow
.CE

To reset the statistics to zero, type at the shell prompt:
.CS
        -> csShow 0, 1
.CE

Another routine that you may find useful is:
.CS
        -> ifShow "cs0"
.CE

EXTERNAL ROUTINES

For debugging purposes, this driver calls logMsg() to print error and debugging
information.  This will cause the logLib library to be linked with any image
containing this driver.

This driver needs the following macros defined for proper execution.  Each
has a default definition that assumes a PC386/PC486 system and BSP.

The macro CS_IN_BYTE(reg,pAddr) reads one byte from the I/O address 'reg',
placing the result at address 'pAddr'.  There is no status result from this
operation, we assume the operation completes normally, or a bus exception
will occur.  By default, this macro assumes there is a BSP routine sysInByte()
to perform the I/O operation.

The macro CS_IN_WORD(reg,pAddr) read a short word (2 bytes) from the I/O
address 'reg', storing the result at address 'pAddr'.  We assume this
completes normally, or causes a bus exception.  The default declaration
assumes a BSP routine sysInWord() to perform the operation.

The macro CS_OUT_WORD(reg,data) writes a short word value 'data' at the
I/O address 'reg'.  The default declaration assumes a BSP routine sysOutWord().

The macro CS_INT_ENABLE(level, pResult) is used to enable the interrupt
level passed as an argument to csAttach.  The default definition call the
BSP routine sysIntEnablePIC(level).  The STATUS return value from the 
actual routine is stored at 'pResult' for the driver to examine.

The macro CS_INT_CONNECT(ivec,rtn,arg,pResult) macro is used to connect
the driver interrupt routine to the vector provided as an argument to 
csAttach (after translaction by INUM_TO_IVEC).  The default definition
calls the cpu architecture routine intConnect().

The macro CS_IRQ0_VECTOR(pAddr) is used to fetch the base vector for the
interrupt level mechanism.  If the int vector argument to csAttach is zero,
then the driver will compute a vector number by adding the interrupt level
to the value returned by this macro.  If the user supplies a non-zero
interrupt vector number, then this macro is not used.  The default definition
of this macro fetches the base vector number from a global value
called 'sysVectorIRQ0'.

The macro CS_MSEC_DELAY(msec) is used to delay execution for a specified
number of milliseconds.  The default definition uses taskDelay to suspend
task for some number of clock ticks.  The resolution of the system clock
is usually around 16 milliseconds (msecs), which is fairly coarse.
*/

#include "vxWorks.h"
#include "sysLib.h"
#include "intLib.h"
#include "logLib.h"
#include "netLib.h"
#include "etherLib.h"
#include "stdio.h"
#include "iv.h"
#include "ioctl.h"
#include "taskLib.h"
#include "net/if_subr.h"
#include "drv/netif/if_cs.h"


/* defines */

#define MAXUNITS    2			/* Maximum number of CS8900 chips */

#ifndef CS_IN_BYTE			/* read one byte from I/O space */
#define CS_IN_BYTE(reg,pAddr) \
	(*(pAddr) = sysInByte(reg))
#endif /*CS_IN_BYTE*/

#ifndef CS_IN_WORD			/* read a short (16bits) from I/O */
#define CS_IN_WORD(reg,pAddr) \
	(*(pAddr) = sysInWord(reg))
#endif /*CS_IN_WORD*/

#ifndef CS_OUT_WORD			/* write a short to I/O space */
#define CS_OUT_WORD(reg,data) \
	(sysOutWord((reg),(data)))
#endif /*CS_OUT_WORD*/

#ifndef CS_INT_ENABLE			/* enable interrupt level */
#define CS_INT_ENABLE(level,pResult) \
	(*pResult = sysIntEnablePIC(level))
#endif /*CS_INT_ENABLE*/

#ifndef CS_INT_CONNECT			/* connect routine to intr. vector */
#define CS_INT_CONNECT(ivec,rtn,arg,pResult) \
	(*pResult = intConnect(ivec,rtn,arg))
#endif /*CS_INT_CONNECT*/

#ifndef CS_IRQ0_VECTOR			/* get vector for intr. level 0 */
#define CS_IRQ0_VECTOR(pAddr) \
	{ \
	IMPORT UINT sysVectorIRQ0; \
	*pAddr = sysVectorIRQ0; \
	}
#endif /*CS_IRQ0_VECTOR*/

#ifndef CS_MSEC_DELAY			/* delay for (x) milliseconds */
#define CS_MSEC_DELAY(msec) \
	taskDelay ((((msec) * sysClkRateGet()) + sysClkRateGet() - 1)/ 1000);
#endif /*CS_MSEC_DELAY*/

/* typedefs */
typedef struct ether_header ETH_HDR; 		

/* globals */


/* locals */

LOCAL CS_SOFTC cs_softc[MAXUNITS];	/* Instance global variables */


/* prototypes */

#if defined(__STDC__) || defined(__cplusplus)

      STATUS	csAttach	(int unit, int ioAddr, int intVector,
				 int intLevel, int memAddr, int mediaType,
				 int configFlags, char *pEnetAddr);
      void	csShow		(int unit, BOOL zap);
LOCAL STATUS	csInit		(int unit);
LOCAL void	csReset		(int unit);
LOCAL void	csIntr		(int unit);
LOCAL int	csIoctl		(struct ifnet *pIf, int command, char *pData);
#ifdef BSD43_DRIVER
LOCAL void	csStartOutput	(int unit);
LOCAL int	csOutput	(struct ifnet *pIf, struct mbuf *pMbufChain,
				 SOCKADDR *pDst);
#else
LOCAL void	csStartOutput	(CS_SOFTC *pCs);
#endif
LOCAL STATUS	csChipVerify	(CS_SOFTC *pCs);
LOCAL STATUS	csChipReset	(CS_SOFTC *pCs);
LOCAL void	csChipInit	(CS_SOFTC *pCs);
LOCAL void	csIntrBuffer	(CS_SOFTC *pCs, USHORT buffEvent);
LOCAL void	csIntrTx	(CS_SOFTC *pCs, USHORT txEvent);
LOCAL void	csIntrRx	(CS_SOFTC *pCs, USHORT rxEvent);
LOCAL void	csTxFrameCopy	(CS_SOFTC *pCs, struct mbuf *pMbufChain);
LOCAL void	csRxFrameCopy	(CS_SOFTC *pCs, CS_RXBUF *pRxBuff);
LOCAL void	csRxProcess	(CS_SOFTC *pCs, CS_RXBUF *pRxBuff);
LOCAL STATUS	csRxBuffInit	(CS_SOFTC *pCs);
LOCAL CS_RXBUF	*csRxBuffAlloc	(CS_SOFTC *pCs);
LOCAL void	csRxBuffFree	(CS_SOFTC *pCs, CS_RXBUF *pRxBuff);
LOCAL USHORT	csPacketPageR	(CS_SOFTC *pCs, USHORT offset);
LOCAL void	csPacketPageW	(CS_SOFTC *pCs, USHORT offset, USHORT value);
LOCAL void	csError		(CS_SOFTC *pCs, char *pString);
LOCAL STATUS	csParmsGet	(CS_SOFTC *pCs);
LOCAL STATUS	csParmsValidate	(CS_SOFTC *pCs);
LOCAL STATUS	csEnetAddrGet	(CS_SOFTC *pCs, char *pEnetAddr);
LOCAL STATUS	csEepromRead	(CS_SOFTC *pCs, USHORT offset, USHORT *pValue);
LOCAL char	*csHexWord	(char *pChar, USHORT *pWord);
LOCAL char	*csHexByte	(char *pChar, UCHAR *pByte);

#else  /* Not ANSI C or C++ */

      STATUS	csAttach	();
      void	csShow		();
LOCAL STATUS	csInit		();
LOCAL void	csReset		();
LOCAL void	csStartOutput	();
LOCAL void	csIntr		();
LOCAL int	csIoctl		();
LOCAL int	csOutput	();
LOCAL STATUS	csChipVerify	();
LOCAL STATUS	csChipReset	();
LOCAL void	csChipInit	();
LOCAL void	csIntrBuffer	();
LOCAL void	csIntrTx	();
LOCAL void	csIntrRx	();
LOCAL void	csTxFrameCopy	();
LOCAL void	csRxFrameCopy	();
LOCAL void	csRxProcess	();
LOCAL STATUS	csRxBuffInit	();
LOCAL CS_RXBUF	*csRxBuffAlloc	();
LOCAL void	csRxBuffFree	();
LOCAL USHORT	csPacketPageR	();
LOCAL void	csPacketPageW	();
LOCAL void	csError		();
LOCAL STATUS	csParmsGet	();
LOCAL STATUS	csParmsValidate	();
LOCAL STATUS	csEnetAddrGet	();
LOCAL STATUS	csEepromRead	();
LOCAL char	*csHexWord	();
LOCAL char	*csHexByte	();

#endif  /* defined(__STDC__) || defined(__cplusplus) */


/*******************************************************************************
*
* csAttach - publish the `cs' network interface and initialize the driver.
*
* This routine is a major entry point to this network interface driver and is
* called only once per operating system reboot by the operating system startup
* code.  This routine is called before the csInit() routine.
*
* This routine takes passed-in configuration parameters and parameters from
* the EEPROM and fills in the instance global variables in the cs_softc
* structure.  These variables are later used by the csChipInit() routine.
*
* This routine connects the interrupt handler, csIntr(), to the specified
* interrupt vector, initializes the 8259 PIC and resets the CS8900 chip.
*
* Finally, this routine calls the ether_attach() routine, to fill in the ifnet
* structure and attach this network interface driver to the system.  The
* driver's main entry points (csInit(), csIoctl(), csOutput(), csReset()) are
* made visable to the protocol stack.
*
* Refer to "man if_cs" for detailed description of the configuration flags.
*
* RETURNS: OK or ERROR.
*/

STATUS csAttach
    (
    int unit,		/* unit number */
    int ioAddr,		/* base IO address */
    int intVector,	/* interrupt vector, or zero */
    int intLevel,	/* interrupt level */
    int memAddr,	/* base memory address */
    int mediaType,	/* 0: Autodetect  1: AUI  2: BNC  3: RJ45 */
    int configFlags,	/* configuration flag */
    char *pEnetAddr	/* ethernet address */
    )
    {
    FAST CS_SOFTC *pCs = &cs_softc[unit];
    STATUS result;

    if (unit >= MAXUNITS)
	{
	printf ("\ncs%d - Invalid unit number (%d)\n", unit, unit);
	return (ERROR);
	}

    /* Save the passed-in parameters */

    pCs->ioAddr      = ioAddr;
    pCs->intLevel    = intLevel;
    pCs->intVector   = intVector;
    pCs->pPacketPage = (USHORT *)memAddr;
    pCs->mediaType   = mediaType;
    pCs->configFlags = configFlags;

    /* Start out in IO mode */

    pCs->inMemoryMode = FALSE;

    /* Set counters to zero */

    pCs->rxDepth    = 0;
    pCs->maxRxDepth = 0;
    pCs->maxTxDepth = 0;
    pCs->loanCount  = 0;

    /* Allocate the receive frame buffers */

    if (csRxBuffInit (pCs) == ERROR)
	{
	printf ("\ncs%d - Can not allocate receive buffers\n", unit);
	return (ERROR);
	}

    /* Verify that it is the correct chip */

    if (csChipVerify (pCs) == ERROR)
	return (ERROR);

    /* Get parameters, which were not specified, from the EEPROM */

    if (csParmsGet (pCs) == ERROR)
	return (ERROR);

    /* Verify that parameters are valid */

    if (csParmsValidate (pCs) == ERROR)
	return (ERROR);

    /* Get and store the Ethernet address */

    if (csEnetAddrGet (pCs,pEnetAddr) == ERROR)
	return (ERROR);

    /* If memory address was specified but configuration flags were not */

    if (memAddr!=0 && configFlags==0)
	pCs->configFlags |= CS_CFGFLG_MEM_MODE;  /* Implictly set memory mode */

    /* If the interrupt vector was not specified then derive it */

    if (intVector == 0)
	{
	CS_IRQ0_VECTOR(&intVector);
	pCs->intVector = intVector + pCs->intLevel;
	}

    /* Setup the interrupt handler and the IDT table entry */

    CS_INT_CONNECT (INUM_TO_IVEC (pCs->intVector), csIntr, unit, &result);
    if (result == ERROR)
	{
	printf ("\ncs%d - Can not connect interrupt\n", unit);
	return (ERROR);
	}

    /* Enable interrupt at the 8259 PIC */

    CS_INT_ENABLE(pCs->intLevel, &result);
    if (result == ERROR)
	{
	printf ("\ncs%d - Can not enable interrupt\n", unit);
	return (ERROR);
	}

    /* Reset the chip */

    if (csChipReset (pCs) == ERROR)
	{
	printf ("\ncs%d - Can not reset the chip\n", unit);
	return (ERROR);
	}

    /* Attach this network interface driver to the system */

#ifdef BSD43_DRIVER
    ether_attach (&pCs->arpCom.ac_if, unit, "cs", csInit, csIoctl, csOutput,
		  (FUNCPTR)csReset);
#else
    ether_attach    (
                    &pCs->arpCom.ac_if,
                    unit,
                    "cs",
                    (FUNCPTR) csInit,
                    (FUNCPTR) csIoctl,
                    (FUNCPTR) ether_output, /* generic ether_output */
                    (FUNCPTR) csReset
                    );

    pCs->arpCom.ac_if.if_start = (FUNCPTR) csStartOutput; 
#endif
    return (OK);
    }


/*******************************************************************************
*
* csChipVerify - verifies that the chip is present and correct
*
* This routine verifies that the Ethernet chip is present and correct.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS csChipVerify 
    (
    CS_SOFTC *pCs
    )
    {
    USHORT eisaNumber = csPacketPageR (pCs, CS_PKTPG_EISA_NUM);

    /* Verify that we can read from the chip */

    if (eisaNumber == 0xFFFF)
	{
	printf ("\ncs0 - Can't read from chip (I/O address correct?)\n");
	return (ERROR);
	}
    
    /* Verify that the chip is a Crystal Semiconductor chip */

    if (eisaNumber != CS_EISA_NUM_CRYSTAL)
	{
	printf ("\ncs0 - Chip is not a Crystal Semiconductor chip\n");
	return (ERROR);
	}

    /* Verify that the chip is a CS8900 */

    if ((csPacketPageR (pCs, CS_PKTPG_PRODUCT_ID) & CS_PROD_ID_MASK) !=
	CS_PROD_ID_CS8900)
	{
	printf ("\ncs0 - Chip is not a CS8900\n");
	return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* csParmsGet - gets parameters that were not specifed to csAttach()
*
* This routine gets parameters, that were not specifed to csAttach(),
* from the EEPROM and puts them in the cs_softc structure.  If all the
* parameters were specified to csAttach(), then this routine does
* not attempt to read the EEPROM.
*
* RETURNS: N/A
*/

LOCAL STATUS csParmsGet
    (
    CS_SOFTC *pCs
    )
    {
    USHORT selfStatus;
    USHORT isaConfig;
    USHORT memBase;
    USHORT adapterConfig;
    USHORT xmitCtl;

    /* If all of these parameters were specified */

    if ((pCs->configFlags != 0) &&
	(pCs->pPacketPage != NULL) &&
	(pCs->intLevel != 0) &&
	(pCs->mediaType != 0))
	return (OK);  /* Don't need to get anything from the EEPROM */

    /* Verify that the EEPROM is present and OK */

    selfStatus = csPacketPageR (pCs, CS_PKTPG_SELF_ST);
    if (!((selfStatus & CS_SELF_ST_EEP_PRES) &&
	(selfStatus & CS_SELF_ST_EEP_OK)))
	{
	printf ("\ncs0 - EEPROM is missing or bad\n");
	return (ERROR);
	}

    /* Get ISA configuration from the EEPROM */
    if (csEepromRead (pCs, CS_EEPROM_ISA_CFG, &isaConfig) == ERROR)
	return (ERROR);

    /* Get adapter configuration from the EEPROM */
    if (csEepromRead (pCs, CS_EEPROM_ADPTR_CFG, &adapterConfig) == ERROR)
	return (ERROR);

    /* Get transmission control from the EEPROM */
    if (csEepromRead (pCs, CS_EEPROM_XMIT_CTL, &xmitCtl) == ERROR)
	return (ERROR);

    /* If the configuration flags were not specified */
    if (pCs->configFlags == 0)
	{
	/* Copy the memory mode flag */
	if (isaConfig & CS_ISA_CFG_MEM_MODE)
	    pCs->configFlags |= CS_CFGFLG_MEM_MODE;

	/* Copy the USE_SA flag */
	if (isaConfig & CS_ISA_CFG_USE_SA)
	    pCs->configFlags |= CS_CFGFLG_USE_SA;

	/* Copy the IO Channel Ready flag */
	if (isaConfig & CS_ISA_CFG_IOCHRDY)
	    pCs->configFlags |= CS_CFGFLG_IOCHRDY;

	/* Copy the DC/DC Polarity flag */
	if (adapterConfig & CS_ADPTR_CFG_DCDC_POL)
	    pCs->configFlags |= CS_CFGFLG_DCDC_POL;

	/* Copy the Full Duplex flag */
	if (xmitCtl & CS_XMIT_CTL_FDX)
	    pCs->configFlags |= CS_CFGFLG_FDX;
	}

     /* If the PacketPage pointer was not specified */
     if (pCs->pPacketPage == NULL)
	 {
	 /* If memory mode is enabled
	  *   Get the memory base address from EEPROM
	  *   Setup the PacketPage pointer
	  */
	 if (pCs->configFlags & CS_CFGFLG_MEM_MODE)
	     {
	     if (csEepromRead (pCs, CS_EEPROM_MEM_BASE, &memBase) == ERROR)
		 return (ERROR);

	      memBase &= CS_MEM_BASE_MASK;  /* Clear unused bits */

	      pCs->pPacketPage = (USHORT *)(((ULONG)memBase) << 8);
	      }
	 }

    /* If the interrupt level was not specified
     *   Get the interrupt level from the IsaConfig
     */
    if (pCs->intLevel == 0)
	{
	pCs->intLevel = isaConfig & CS_ISA_CFG_IRQ_MASK;
	if (pCs->intLevel == 3)
	    pCs->intLevel = 5;
	else
	    pCs->intLevel += 10;
	}

    /* If the media type was not specified
     *   Get the media type from the adapterConfig
     */
    if (pCs->mediaType == 0)
	{
	switch (adapterConfig & CS_ADPTR_CFG_MEDIA)
	    {
	    case CS_ADPTR_CFG_AUI:
		pCs->mediaType = CS_MEDIA_AUI;
		break;
	    case CS_ADPTR_CFG_10BASE2:
		pCs->mediaType = CS_MEDIA_10BASE2;
		break;
	    case CS_ADPTR_CFG_10BASET:
	    default:
		pCs->mediaType = CS_MEDIA_10BASET;
		break;
	    }
	}

    return (OK);
    }

/*******************************************************************************
*
* csParmsValidate - validate the memory address, interrupt level and media type
*
* This routine verifies that the memory address, interrupt level and media
* type are valid.  If any of these parameters are invalid, then and error
* message is printed and ERROR is returned.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS csParmsValidate
    (
    CS_SOFTC *pCs
    )
    {
    int memAddr = (int)(pCs->pPacketPage);

    if ((memAddr & 0x000FFF) != 0)
	{
	printf ("\ncs0 - Memory address (0x%X) must start on a 4K boundry\n",
	        memAddr);
	return (ERROR);
	}

    if (memAddr > 0xFFF000)
	{
	printf ("\ncs0 - Memory address (0x%X) is too large\n", memAddr);
	return (ERROR);
	}

    if (!(pCs->intLevel==5 || pCs->intLevel==10 || pCs->intLevel==11 ||
	  pCs->intLevel==12))
	{
	printf ("\ncs0 - Interrupt level (%d) is invalid\n", pCs->intLevel);
	return (ERROR);
	}

    if (!(pCs->mediaType==CS_MEDIA_AUI || pCs->mediaType==CS_MEDIA_10BASE2 ||
	  pCs->mediaType==CS_MEDIA_10BASET))
	{
	printf ("\ncs0 - Media type (%d) is invalid\n", pCs->mediaType);
	return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* csEnetAddrGet - get the ethernet address from arguments or EEPROM
*
* If a pointer to an Ethernet address string was passed-in to the csAttach()
* routine, then this routine coverts the address string to an Ethernet address
* and saves it in the arpcom structure.  If an Ethernet address string was
* not passed-in, then the Ethernet address is read from the EEPROM and saved
* in the arpcom structure.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS csEnetAddrGet
    (
    CS_SOFTC *pCs,
    char *pEthernetAddr
    )
    {
    USHORT selfStatus;
    CS_IA *pIA = (CS_IA *)pCs->arpCom.ac_enaddr;

    /* If the Ethernet address was not specified */
    if (pEthernetAddr == NULL)
	{
	/* Verify that the EEPROM is present and OK */
	selfStatus = csPacketPageR (pCs, CS_PKTPG_SELF_ST);
	if (!((selfStatus & CS_SELF_ST_EEP_PRES) && 
	    (selfStatus & CS_SELF_ST_EEP_OK)))
	    {
	    printf ("\ncs0 - EEPROM is missing or bad\n");
	    return (ERROR);
	    }

       /* Get Ethernet address from the EEPROM */
       if (csEepromRead (pCs, CS_EEPROM_IND_ADDR_H, &pIA->word[0]) == ERROR)
	   return (ERROR);
       if (csEepromRead (pCs, CS_EEPROM_IND_ADDR_M, &pIA->word[1]) == ERROR)
	   return (ERROR);
       if (csEepromRead (pCs, CS_EEPROM_IND_ADDR_L, &pIA->word[2]) == ERROR)
	   return (ERROR);
       }
    else  /* The Ethernet address was specified */
       {
       /* Convert and save the Ethernet address string */
       pEthernetAddr = csHexWord (pEthernetAddr, &pIA->word[0]);
       if (pEthernetAddr == NULL)
	   return (ERROR);
       pEthernetAddr = csHexWord (pEthernetAddr, &pIA->word[1]);
       if (pEthernetAddr == NULL)
	   return (ERROR);
       pEthernetAddr = csHexWord (pEthernetAddr, &pIA->word[2]);
       if (pEthernetAddr == NULL)
	   return (ERROR);
       }

    return (OK);
    }

/*******************************************************************************
*
* csHexWord - converts a sequence of hex characters to the 16-bit value
*
* This routine converts a sequence of hex characters to the 16-bit value
* that they represent.  The address of the first hex character is passed
* in the pChar parameter and the address just beyond the last hex
* character is returned.  The 16-bit variable pointed to by the pWord
* parameter is updated with the converted 16-bit value.  If an error
* occurred then NULL is returned.
*
* RETURNS: address of next character
*/

LOCAL char *csHexWord
    (
    char *pChar,
    USHORT *pWord
    )
    {
    union
	{
	USHORT word;
	UCHAR  byte[2];
	} value;

    /* Get the value of the first hex byte */
    pChar = csHexByte (pChar, &value.byte[0]);
    if ((pChar != NULL) && (*pChar == 0))
	printf ("\ncs0 - Ethernet address string too short\n");
    if ((pChar == NULL) || (*pChar == 0))
	return (NULL);

    /* Get the value of the second hex byte */
    pChar = csHexByte (pChar, &value.byte[1]);
    if (pChar == NULL)
	return (NULL);

    /* Save value of the hex word */
    *pWord = value.word;
 
    return (pChar);
    }

/*******************************************************************************
*
* csHexByte - converts a sequence of hex characters to the 8-bit value
*
* This routine converts a sequence of hex characters to the 8-bit value
* that they represent.  There may be zero, one or two hex characters and
* they may be optionally terminated with a colon or a zero byte.
* The address of the first hex character is passed in the pChar parameter
* and the address just beyond the last hex character is returned.
* The 8-bit variable pointed to by the pByte parameter is updated with
* the converted 8-bit value.  If an error occurred then NULL is returned.
*
* RETURNS: address of next character
*/

LOCAL char *csHexByte
    (
    char *pChar,
    UCHAR *pByte
    )
    {
    int ix;

    /* Inititalize the byte value to zero */
    *pByte = 0;

    /* Process two hex characters */
    for (ix=0; ix<2; ix++, pChar++)
	{
	/* Stop early if find a colon or end of string */
	if ((*pChar == ':') || (*pChar == 0))
	    break;

	/* Convert the hex character to a value */
	if ((*pChar >= '0') && (*pChar <= '9'))
	    *pByte = (*pByte * 16) + *pChar - '0';
	else if ((*pChar >= 'a') && (*pChar <= 'f'))
	    *pByte = (*pByte * 16) + *pChar - 'a' + 10;
	else if ((*pChar >= 'A') && (*pChar <= 'F'))
	    *pByte = (*pByte * 16) + *pChar - 'A' + 10;
	else
	    {
	    printf ("\ncs0 - Illegal character '%c' in Enet address string\n",
	            *pChar);
	    return (NULL);  /* Illegal character */
	    }
	}

    /* Skip past terminating colon */
    if (*pChar == ':')
	pChar++;

    return (pChar);
    }

/*******************************************************************************
*
* csEepromRead - reads a word from the EEPROM
*
* This routine reads a word from the EEPROM at the specified offset.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS csEepromRead
    (
    CS_SOFTC *pCs,
    USHORT offset,
    USHORT *pValue
    )
    {
    int ix;

    /* Ensure that the EEPROM is not busy */
    for (ix=0; ix<CS_MAXLOOP; ix++)
	if (!(csPacketPageR (pCs, CS_PKTPG_SELF_ST) & CS_SELF_ST_SI_BUSY))
	    break;
    if (ix == CS_MAXLOOP)
       {
       printf ("\ncs0 - Can not read from EEPROM\n");
       return (ERROR);
       }

    /* Issue the command to read the offset within the EEPROM */
    csPacketPageW (pCs, CS_PKTPG_EEPROM_CMD, offset | CS_EEPROM_CMD_READ);

    /* Wait until the command is completed */
    for (ix=0; ix<CS_MAXLOOP; ix++)
	if (!(csPacketPageR (pCs, CS_PKTPG_SELF_ST) & CS_SELF_ST_SI_BUSY))
	    break;
    if (ix == CS_MAXLOOP)
	{
	printf ("\ncs0 - Can not read from EEPROM\n");
	return (ERROR);
	}

    /* Get the EEPROM data from the EEPROM data register */
    *pValue = csPacketPageR (pCs, CS_PKTPG_EEPROM_DATA);
 
    return (OK);
    }

/*******************************************************************************
*
* csInit - resets and then initializes the CS8900 chip
*
* This routine is a major entry point and is called to initialize this network
* interface driver.  This routine may be called several times for each
* operating system reboot to dynamically bring the network interface driver
* to an up and running state.  This routine is called by the protocol stack,
* the set_if_addr() routine, and the csIoctl() routine.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS csInit
    (
    int unit
    )
    {
    FAST CS_SOFTC *pCs = &cs_softc[unit];

    if (unit >= MAXUNITS)
	return (ERROR);

    /* Mark the interface as down */

    pCs->arpCom.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);

    /* Enable debugging */
    /* pCs->arpCom.ac_if.if_flags |= IFF_DEBUG; */

    csError (pCs, "Initializing interface");

    /* Reset the chip */

    if (csChipReset (pCs) == ERROR)
	{
	csError (pCs, "Can not reset the chip");
	return (ERROR);
	}

    /* Initialize the chip */

    csChipInit (pCs);

    /* Mark the interface as up and running */

    pCs->arpCom.ac_if.if_flags |= (IFF_UP | IFF_RUNNING);

    return (OK);
    }

/*******************************************************************************
*
* csReset - resets the CS8900 chip.
*
* This routine is a major entry point and is called by the protocol stack to
* shut down this network interface driver.  This routine may be called several
* times for each operating system reboot to dynamically bring the network
* interface driver to a non-running state.
*
* RETURNS: N/A
*/

LOCAL void csReset
    (
    int unit
    )
    {
    FAST CS_SOFTC *pCs = &cs_softc[unit];

    if (unit >= MAXUNITS)
	return;

    csError (pCs, "Resetting interface");

    /* Mark the interface as down */

    pCs->arpCom.ac_if.if_flags &= ~IFF_RUNNING;

    /* Reset the chip */

    csChipReset (pCs);
    }

/*******************************************************************************
*
* csIoctl - ioctl for interface
*
* This routine is a major entry point and is called by the protocol stack to
* modify characteristics of this network interface driver.  There are many
* network interface ioctl commands, but this driver only supports two of them:
* Set Interface Address and Set Interface Flags.
*
* RETURNS: OK if successful, otherwise errno.
*/

LOCAL int csIoctl
    (
    struct ifnet *pIf,
    int command,
    char *pData
    )
    {
    FAST CS_SOFTC *pCs = &cs_softc[pIf->if_unit];
    int result         = OK;  /* Assume everything will go OK */
    int state          = splnet();

    switch (command)
	{
	case SIOCSIFADDR:  /* Set interface address (IP address) */
	    csError (pCs, "Ioctl: Set interface address");
	    result = set_if_addr (pIf, pData, (UCHAR *)pCs->arpCom.ac_enaddr);
	    /* Note: set_if_addr () calls csInit() */
	    break;
	case SIOCSIFFLAGS:  /* Set interface flags */
	    csError (pCs, "Ioctl: Set interface flags");
	    if (pIf->if_flags & IFF_UP)
		{
		/* Mark the interface as up and running */
		pCs->arpCom.ac_if.if_flags |= (IFF_UP | IFF_RUNNING);

		/* Initialize the interface */
		csInit (pIf->if_unit);
		}
	    else  /* The interface is down */
		{
		/* Mark the interface as down */
		pCs->arpCom.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);

		/* Reset the chip */
		csChipReset (pCs);
		}
	    break;
	default:
	    result = EINVAL;  /* Invalid argument */
	    break;
	}

    splx (state);

    return (result);
    }

/*******************************************************************************
*
* csChipReset - resets the CS8900 chip
*
* This routine resets the CS8900 chip.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS csChipReset 
    (
    CS_SOFTC *pCs
    )
    {
    int intState;
    int ix;
    UCHAR temp;

    /* Disable interrupts at the CPU so reset command is atomic */

    intState = intLock ();	/* INT LOCK */

    /* We are now resetting the chip
     * A spurious interrupt is generated by the chip when it is reset.
     * This variable informs the interrupt handler to ignore this interrupt.
     */
    pCs->resetting = TRUE;

    /* Issue a reset command to the chip */
    csPacketPageW (pCs, CS_PKTPG_SELF_CTL, CS_SELF_CTL_RESET);

    /* Re-enable interrupts at the CPU */
    intUnlock (intState);	/* INT UNLOCK */

    /* The chip is always in IO mode after a reset */
    pCs->inMemoryMode = FALSE;

    /* If transmission was in progress, it is not now */
    pCs->txInProgress = FALSE;

    /* Delay for 125 micro-seconds (one eighth of a second) */

    CS_MSEC_DELAY(125);

    /* Transition SBHE to switch chip from 8-bit to 16-bit */

    CS_IN_BYTE (pCs->ioAddr + CS_PORT_PKTPG_PTR, &temp);
    CS_IN_BYTE (pCs->ioAddr + CS_PORT_PKTPG_PTR + 1, &temp);
    CS_IN_BYTE (pCs->ioAddr + CS_PORT_PKTPG_PTR, &temp);
    CS_IN_BYTE (pCs->ioAddr + CS_PORT_PKTPG_PTR + 1, &temp);

    /* Wait until the EEPROM is not busy */
    for (ix=0; ix<CS_MAXLOOP; ix++)
	if (!(csPacketPageR (pCs, CS_PKTPG_SELF_ST) & CS_SELF_ST_SI_BUSY))
	    break;
    if (ix == CS_MAXLOOP)
	return (ERROR);

    /* Wait until initialization is done */
    for (ix=0; ix<CS_MAXLOOP; ix++)
	if (csPacketPageR (pCs, CS_PKTPG_SELF_ST) & CS_SELF_ST_INIT_DONE)
	    break;
    if (ix == CS_MAXLOOP)
	return (ERROR);

    /* Reset is no longer in progress */

    pCs->resetting = FALSE;

    return (OK);
    }

/*******************************************************************************
*
* csChipInit - initializes the CS8900 chip
*
* This routine uses the instance global variables in the cs_softc structure to
* initialize the CS8900 chip.
*
* RETURNS: N/A
*/

LOCAL void csChipInit
    (
    CS_SOFTC *pCs
    )
    {
    USHORT busCtl;
    USHORT selfCtl;
    CS_IA  *pIA;

    /* If memory mode is enabled */
    if (pCs->configFlags & CS_CFGFLG_MEM_MODE)
	{
	/* If external logic is present for address decoding */
	if (csPacketPageR (pCs, CS_PKTPG_SELF_ST) & CS_SELF_ST_EL_PRES)
	    {
	    /* Program the external logic to decode address bits SA20-SA23 */
	    csPacketPageW (pCs, CS_PKTPG_EEPROM_CMD,
		       ((UINT)(pCs->pPacketPage)>>20) | CS_EEPROM_CMD_ELSEL);
	    }

	/* Setup chip for memory mode */
	csPacketPageW (pCs, CS_PKTPG_MEM_BASE, (UINT)(pCs->pPacketPage)&0xFFFF);
	csPacketPageW (pCs, CS_PKTPG_MEM_BASE+2, (UINT)(pCs->pPacketPage)>>16);
	busCtl = CS_BUS_CTL_MEM_MODE;
	if (pCs->configFlags & CS_CFGFLG_USE_SA)
	    busCtl |= CS_BUS_CTL_USE_SA;
	csPacketPageW (pCs, CS_PKTPG_BUS_CTL, busCtl);

	/* We are in memory mode now! */
	pCs->inMemoryMode = TRUE;
	}

    /* If IOCHRDY is enabled then clear the bit in the busCtl register */
    busCtl = csPacketPageR (pCs, CS_PKTPG_BUS_CTL);
    if (pCs->configFlags & CS_CFGFLG_IOCHRDY)
	csPacketPageW (pCs, CS_PKTPG_BUS_CTL, busCtl & ~CS_BUS_CTL_IOCHRDY);
    else
	csPacketPageW (pCs, CS_PKTPG_BUS_CTL, busCtl | CS_BUS_CTL_IOCHRDY);

    /* Set the Line Control register to match the media type */
    if (pCs->mediaType == CS_MEDIA_10BASET)
	csPacketPageW (pCs, CS_PKTPG_LINE_CTL, CS_LINE_CTL_10BASET);
    else
	csPacketPageW (pCs, CS_PKTPG_LINE_CTL, CS_LINE_CTL_AUI_ONLY);

    /* Set the BSTATUS/HC1 pin to be used as HC1 */
    /* HC1 is used to enable the DC/DC converter */
    selfCtl = CS_SELF_CTL_HC1E;

    /* If the media type is 10Base2 */
    if (pCs->mediaType == CS_MEDIA_10BASE2)
	{
	/* Enable the DC/DC converter
	 * If the DC/DC converter has a low enable
	 *   Set the HCB1 bit, which causes the HC1 pin to go low
	 */
	if ((pCs->configFlags & CS_CFGFLG_DCDC_POL) == 0)
	    selfCtl |= CS_SELF_CTL_HCB1;
	}
    else  /* Media type is 10BaseT or AUI */
	{
	/* Disable the DC/DC converter
	 * If the DC/DC converter has a high enable
	 *   Set the HCB1 bit, which causes the HC1 pin to go low
	 */
	if ((pCs->configFlags & CS_CFGFLG_DCDC_POL) != 0)
	    selfCtl |= CS_SELF_CTL_HCB1;
	}
    csPacketPageW (pCs, CS_PKTPG_SELF_CTL, selfCtl);

    /* If media type is 10BaseT */
    if (pCs->mediaType == CS_MEDIA_10BASET)
	{
	/* If full duplex mode then set the FDX bit in TestCtl register */
	if (pCs->configFlags & CS_CFGFLG_FDX)
	    csPacketPageW (pCs, CS_PKTPG_TEST_CTL, CS_TEST_CTL_FDX);
	}

    /* Initialize the config and control registers */
    csPacketPageW (pCs, CS_PKTPG_RX_CFG, CS_RX_CFG_ALL_IE);
    csPacketPageW (pCs, CS_PKTPG_RX_CTL,
		   CS_RX_CTL_RX_OK_A | CS_RX_CTL_IND_A | CS_RX_CTL_BCAST_A);
    csPacketPageW (pCs, CS_PKTPG_TX_CFG, CS_TX_CFG_ALL_IE);
    csPacketPageW (pCs, CS_PKTPG_BUF_CFG, CS_BUF_CFG_RX_MISS_IE);

    /* Put Ethernet address into the Individual Address register */
    pIA = (CS_IA *)pCs->arpCom.ac_enaddr;
    csPacketPageW (pCs, CS_PKTPG_IND_ADDR,   pIA->word[0]);
    csPacketPageW (pCs, CS_PKTPG_IND_ADDR+2, pIA->word[1]);
    csPacketPageW (pCs, CS_PKTPG_IND_ADDR+4, pIA->word[2]);

    /* Set the interrupt level in the chip */
    if (pCs->intLevel == 5)
	csPacketPageW (pCs, CS_PKTPG_INT_NUM, 3);
    else
	csPacketPageW (pCs, CS_PKTPG_INT_NUM, (pCs->intLevel) - 10);

    /* Enable reception and transmission of frames */
    csPacketPageW (pCs, CS_PKTPG_LINE_CTL,
		    csPacketPageR (pCs, CS_PKTPG_LINE_CTL) |
				   CS_LINE_CTL_RX_ON | CS_LINE_CTL_TX_ON);

    /* Enable interrupt at the chip */
    csPacketPageW (pCs, CS_PKTPG_BUS_CTL,
		    csPacketPageR (pCs, CS_PKTPG_BUS_CTL) |
				   CS_BUS_CTL_INT_ENBL);
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* csOutput - ethernet output routine
*
* This routine is a major entry point and is called by the protocol stack to
* transmit a packet across the LAN.  The packet data is passed to this routine
* in a chain of mbuf structures.
*
* This routine calls the ether_output() routine, which in turn calls the
* csStartOutput() routine.  The ether_output() routine resolves the destination
* IP address to it's Ethernet address, builds an Ethernet header and prepends
* the Ethernet header to the mbuf chain.  The mbuf chain is added to the end
* of the network interface driver's transmit queue.  If the network interface
* NOTRAILERS flag is clear (trailers enabled), then the ether_output() routine
* converts the packet data to the trailers format.  Finally, the ether_output()
* routine calls the csStartOutput() routine begin transmission of the packet.
*
* RETURNS: OK if successful, otherwise errno.
*/

LOCAL int csOutput
    (
    struct ifnet *pIf,
    struct mbuf *pMbufChain,
    SOCKADDR *pDst
    )
    {
    FAST CS_SOFTC *pCs = &cs_softc[pIf->if_unit];

    return (ether_output (pIf, pMbufChain, pDst, (FUNCPTR)csStartOutput,
			  &pCs->arpCom));
    }

/*******************************************************************************
*
* csStartOutput - copy a packet to the interface
*
* This routine is called to start the transmission of the packet at the head
* of the transmit queue.  This routine is called by the ether_output() routine
* when a new packet is added to the transmit queue and this routine is called
* by csIntrTx() via netTask() when a previous packet transmission is
* completed.  This routine is always executed at task time (never at interrupt
* time).
*
* Note: This network interface driver does not support output hook routines,
* because to do so requires that an image of the transmit packet be built in
* memory before the image is copied to the CS8900 chip.  It is much more
* efficient to copy the image directly from the mbuf chain to the CS8900 chip.
* However, this network interface driver does support input hook routines.
*
* RETURNS: N/A
*/

LOCAL void csStartOutput
    (
    int unit
    )
    {
    FAST CS_SOFTC *pCs = &cs_softc[unit];
#else /* BSD43_DRIVER */
LOCAL void csStartOutput
    (
    CS_SOFTC * pCs		/* pointer to control structure */
    )
    {
#endif /* BSD43_DRIVER */
    FAST struct mbuf *pMbuf;
    struct mbuf *pMbufChain;
    struct ifqueue *pTxQueue;
    USHORT busStatus;
    USHORT length;
    int intState;

#ifdef BSD43_DRIVER
    if (unit >= MAXUNITS)
	return;
#endif /* BSD43_DRIVER */

    pTxQueue = &pCs->arpCom.ac_if.if_snd;

    /* Note the maximum depth of the transmit queue */
    if (pTxQueue->ifq_len > pCs->maxTxDepth)
	pCs->maxTxDepth = pTxQueue->ifq_len;

    /* Don't interrupt a transmission in progress */
    if (pCs->txInProgress)
	return;

    /* Disable interrupts at the CPU so disabling chip interrupt is atomic */
    intState = intLock ();	/* INT LOCK */

    /* Disable interrupt at the chip so transmit sequence is not disturbed */
    csPacketPageW (pCs, CS_PKTPG_BUS_CTL,
		    csPacketPageR (pCs, CS_PKTPG_BUS_CTL) &
				   ~CS_BUS_CTL_INT_ENBL);

    /* Re-enable interrupts at the CPU */
    intUnlock (intState);	/* INT UNLOCK */

    /* While there is still more to transmit */
    while (pTxQueue->ifq_head != NULL)
	{
	/* Dequeue an mbuf chain from the transmit queue */
	IF_DEQUEUE (pTxQueue, pMbufChain);

	/* Find the total length of the data to transmit */
	length = 0;
	for (pMbuf=pMbufChain; pMbuf!=NULL; pMbuf=pMbuf->m_next)
	    length += pMbuf->m_len;

	/* etherOutputHookRtn is not supported */
 
	/* Request that the transmit be started after
	 * all data has been copied
	 */
	if (pCs->inMemoryMode)
	    {
	    csPacketPageW (pCs, CS_PKTPG_TX_CMD, CS_TX_CMD_START_ALL);
	    csPacketPageW (pCs, CS_PKTPG_TX_LENGTH, length);
	    }
	else  /* In IO mode */
	    {
	    CS_OUT_WORD (pCs->ioAddr + CS_PORT_TX_CMD, CS_TX_CMD_START_ALL);
	    CS_OUT_WORD (pCs->ioAddr + CS_PORT_TX_LENGTH, length);
	    }

	/* Read the busStatus register which indicates success of the request */
	busStatus = csPacketPageR (pCs, CS_PKTPG_BUS_ST);

	/* If there was an error in the transmit bid */
	if (busStatus & CS_BUS_ST_TX_BID_ERR)
	    {
	    csError (pCs, "Transmit bid error (too big)");
	    /* Discard the bad mbuf chain */
	    m_freem (pMbufChain);
	    pCs->arpCom.ac_if.if_oerrors++;
	    /* Loop up to transmit the next chain */
	    }
	else if (busStatus & CS_BUS_ST_RDY4TXNOW)
	    {
	    /* The chip is ready for transmission now */
	    /* Copy the frame to the chip to start transmission */
	    csTxFrameCopy (pCs, pMbufChain);
	    /* Free the mbuf chain */
	    m_freem (pMbufChain);
	    /* Transmission is now in progress */
	    pCs->txInProgress = TRUE;
	    break;  /* Exit this routine */
	    }
	else
  	    csError (pCs, "Not ready for transmission now");
	}

    /* Re-enable interrupt at the chip */
    csPacketPageW (pCs, CS_PKTPG_BUS_CTL, 
		   csPacketPageR (pCs, CS_PKTPG_BUS_CTL) | CS_BUS_CTL_INT_ENBL);
    }

/*******************************************************************************
*
* csTxFrameCopy - copies the packet from a mbuf chain to the chip
*
* This routine copies the packet from a chain of mbufs to the chip.  When all
* the data has been copied, then the chip automatically begins transmitting
* the data.
*
* The reason why this "simple" copy routine is so long and complicated is
* because all reads and writes to the chip must be done as 16-bit words.
* If an mbuf has an odd number of bytes, then the last byte must be saved
* and combined with the first byte of the next mbuf.
*
* RETURNS: N/A
*/

LOCAL void csTxFrameCopy
    (
    CS_SOFTC *pCs,
    struct mbuf *pMbufChain
    )
    {
    struct mbuf *pMbuf;
    FAST USHORT *pFrame;
    FAST USHORT *pBuff;
    FAST USHORT *pBuffLimit;
    FAST int txDataPort;
    UCHAR  *pStart;
    USHORT  length;
    BOOL haveExtraByte;
    union
	{
	UCHAR  byte[2];
	USHORT word;
	} straddle;

    /* Initialize frame pointer and data port address */
    pFrame = pCs->pPacketPage + (CS_PKTPG_TX_FRAME/2);
    txDataPort = pCs->ioAddr + CS_PORT_RXTX_DATA;

    haveExtraByte = FALSE;  /* Start out with no extra byte */

    /* Process the chain of mbufs */
    for (pMbuf=pMbufChain; pMbuf!=NULL; pMbuf=pMbuf->m_next)
	{
	/* Setup starting pointer and length */
	pStart = mtod (pMbuf, UCHAR *);
	length = pMbuf->m_len;

	/* If there is an extra byte left over from the previous mbuf */
	if (haveExtraByte)
	    {
	    /* Add the first byte from this mbuf to make a word */
	    straddle.byte[1] = *pStart;

	    /* Write the word which straddles the mbufs to the chip */
	    if (pCs->inMemoryMode)
		*pFrame++ = straddle.word;
	    else
		{
		CS_OUT_WORD (txDataPort, straddle.word);
		}

	    /* Adjust starting pointer and length */
	    pStart++;
	    length--;
	    }

	/* Point pBuff to the correct starting point */
	pBuff = (USHORT *)pStart;
 
	/* If there are odd bytes remaining in the mbuf */
	if (length & 1)
	    {
	    haveExtraByte = TRUE;

	    /* Point pBuffLimit to the extra byte */
	    pBuffLimit = (USHORT *)(pStart+length-1);
	    }
	else  /* There is an even number of bytes remaining */
	    {
	    haveExtraByte = FALSE;

	    /* Point pBuffLimit to just beyond the last word */
	    pBuffLimit = (USHORT *)(pStart+length);
	    }

	/* Copy the words in the mbuf to the chip */
	if (pCs->inMemoryMode)
	    while (pBuff < pBuffLimit)
		*pFrame++ = *pBuff++;
	else
	    while (pBuff < pBuffLimit)
		{
		CS_OUT_WORD (txDataPort, *pBuff++);
		}

        /* If there is an extra byte left over in this mbuf
	 *   Save the extra byte for later
	 */
        if (haveExtraByte)
	    straddle.byte[0] = *(UCHAR *)pBuff;
	}

    /* If there is an extra byte left over from the last mbuf */
    if (haveExtraByte)
	{
	/* Add a zero byte to make a word */
	straddle.byte[1] = 0;

	/* Write the last word to the chip */
	if (pCs->inMemoryMode)
	    *pFrame = straddle.word;
	else
	    {
	    CS_OUT_WORD (txDataPort, straddle.word);
	    }
	}
    }
 
/*******************************************************************************
*
* csIntr - Ethernet interface interrupt
*
* This routine in the interrupt service routine.  This routine is called by
* assembly language wrapper code whenever the CS8900 chip generates and
* interrupt.  The wrapper code issues an EOI command intr. controller.
*
* This routine processes the events on the Interrupt Status Queue.  The events
* are read one at a time from the ISQ and the appropriate event handlers are
* called.  The ISQ is read until it is empty.  If the chip's interrupt request
* line is active, then reading a zero from the ISQ will deactivate the
* interrupt request line.
*
* RETURNS: N/A
*/

LOCAL void csIntr
    (
    int unit
    )
    {
    FAST CS_SOFTC *pCs = &cs_softc[unit];
    USHORT event;

    if (unit >= MAXUNITS)
	return;

    /* Ignore any interrupts that happen while the chip is being reset */
    if (pCs->resetting)
	return;

    /* Read an event from the Interrupt Status Queue */
    if (pCs->inMemoryMode)
	event = csPacketPageR (pCs, CS_PKTPG_ISQ);
    else
	{
	CS_IN_WORD (pCs->ioAddr + CS_PORT_ISQ, &event);
	}

    /* Process all the events in the Interrupt Status Queue */
    while (event != 0)
	{
	/* Dispatch to an event handler based on the register number */
	switch (event & CS_REG_NUM_MASK)
	    {
	    case CS_REG_NUM_RX_EVENT:
		csIntrRx (pCs, event);
		break;
	    case CS_REG_NUM_TX_EVENT:
		csIntrTx (pCs, event);
		break;
	    case CS_REG_NUM_BUF_EVENT:
		csIntrBuffer (pCs, event);
		break;
	    default:
		csError (pCs, "Unknown interrupt event");
		break;
	    }

	/* Read another event from the Interrupt Status Queue */
	if (pCs->inMemoryMode)
	    event = csPacketPageR (pCs, CS_PKTPG_ISQ);
	else
	    {
	    CS_IN_WORD (pCs->ioAddr + CS_PORT_ISQ, &event);
	    }
	}
    }

/*******************************************************************************
*
* csIntrBuffer - interrupt handler for the receive miss event
*
* The routine is called whenever an event occurs regarding the transmit and
* receive buffers within the CS8900 chip.  The only buffer event that could
* happen with this network interface driver is the receive miss event.  When
* there are no receive buffers available within the chip and a packet arrives
* from the LAN, then this interrupt is generated.  This routine simply
* increments the input error counter.
*
* RETURNS: N/A
*/

LOCAL void csIntrBuffer 
    (
    CS_SOFTC *pCs,
    USHORT bufEvent
    )
    {
    struct ifnet *pIf = &pCs->arpCom.ac_if;

    if (bufEvent & CS_BUF_EVENT_RX_MISS)
	{
	/* Increment the input error count */
	pIf->if_ierrors++;
	csError (pCs, "Receive miss");
	}

    if (bufEvent & CS_BUF_EVENT_SW_INT)
	{
	csError (pCs, "Software initiated interrupt");
	}
    }

/*******************************************************************************
*
* csIntrTx - interrupt handler for the transmission
*
* This routine is called whenever the transmission of a packet has completed
* successfully or unsuccessfully.  If the transmission was not successful,
* then the output error count is incremented.  If collisions occured while
* sending the packet, then the number of collisions is added to the collision
* counter.  If there are more packets on the transmit queue, then the next
* packet is started at task time by calling csStartOutput() via netTask().
*
* RETURNS: N/A
*/

LOCAL void csIntrTx
    (
    CS_SOFTC *pCs,
    USHORT txEvent
    )
    {
    struct ifnet *pIf = &pCs->arpCom.ac_if;

    /* If there were any errors transmitting this frame */
    if (txEvent & (CS_TX_EVENT_LOSS_CRS | CS_TX_EVENT_SQE_ERR 
		| CS_TX_EVENT_OUT_WIN | CS_TX_EVENT_JABBER
		| CS_TX_EVENT_16_COLL))
	{
	/* Increment the output error count */
	pIf->if_oerrors++;

	/* If debugging is enabled then log error messages */
	if (pIf->if_flags & IFF_DEBUG)
	    {
	    if (txEvent & CS_TX_EVENT_LOSS_CRS)
		csError (pCs, "Loss of carrier");
	    if (txEvent & CS_TX_EVENT_SQE_ERR)
		csError (pCs, "SQE error");
	    if (txEvent & CS_TX_EVENT_OUT_WIN)
		csError (pCs, "Out-of-window collision");
	    if (txEvent & CS_TX_EVENT_JABBER)
		csError (pCs, "Jabber");
	    if (txEvent & CS_TX_EVENT_16_COLL)
		csError (pCs, "16 collisions");
	    }
	}

    /* Add the number of collisions for this frame */

    if (txEvent & CS_TX_EVENT_16_COLL)
	pIf->if_collisions += 16;
    else
	pIf->if_collisions += ((txEvent & CS_TX_EVENT_COLL_MASK) >> 11);

    /* Transmission is no longer in progress */

    pCs->txInProgress = FALSE;

    /* If there is more to transmit
     *   Start the next transmission at task time
     */
    if (pIf->if_snd.ifq_head != NULL)
	{
#ifdef BSD43_DRIVER
	netJobAdd ((FUNCPTR)csStartOutput, pIf->if_unit, 0, 0, 0, 0);
#else /* BSD43_DRIVER */
	netJobAdd ((FUNCPTR)csStartOutput, (int)pCs, 0, 0, 0, 0);
#endif /* BSD43_DRIVER */
	}
    }

/*******************************************************************************
*
* csIntrRx - interrupt handler for the reception
*
* This routine is called whenever a packet is received at the chip.  If the
* packet is received with errors, then the input error count is incremented.
* If the packet is received OK, then the data is copied to an internal receive
* buffer and the csRxProcess() routine is called via netTask() to
* process the received packet at task time.
*
* RETURNS: N/A
*/

LOCAL void csIntrRx
    (
    CS_SOFTC *pCs,
    USHORT rxEvent
    )
    {
    struct ifnet *pIf = &pCs->arpCom.ac_if;
    CS_RXBUF *pRxBuff;

    /* If the frame was not received OK */
    if (!(rxEvent & CS_RX_EVENT_RX_OK))
        {
        /* Increment the input error count */
        pIf->if_ierrors++;

        /* If debugging is enabled then log error messages */
        if (pIf->if_flags & IFF_DEBUG)
	    {
	    /* If an error bit is set */
	    if (rxEvent != CS_REG_NUM_RX_EVENT)
	        {
	        if (rxEvent & CS_RX_EVENT_RUNT)
		    csError (pCs, "Runt");
	        if (rxEvent & CS_RX_EVENT_X_DATA)
		    csError (pCs, "Extra data");
	        if (rxEvent & CS_RX_EVENT_CRC_ERR)
		    {
		    if (rxEvent & CS_RX_EVENT_DRIBBLE)
		        csError (pCs, "Alignment error");
		    else
		        csError (pCs, "CRC Error");
		    }
	        else if (rxEvent & CS_RX_EVENT_DRIBBLE)
		    csError (pCs, "Dribble bits");

	        /* Must read the length of all received frames */
	        csPacketPageR (pCs, CS_PKTPG_RX_LENGTH);

	        /* Skip the received frame */
	        csPacketPageW (pCs, CS_PKTPG_RX_CFG,
			       csPacketPageR (pCs,CS_PKTPG_RX_CFG)
					    | CS_RX_CFG_SKIP);
	        }
	    else 
	        csError (pCs, "Implied skip");
	    }
	return;
	}

    /* Get a receive frame buffer */
    pRxBuff = csRxBuffAlloc (pCs);
    if (pRxBuff == NULL)  /* If no buffer available */
	{
	/* Increment the input error count */
	pIf->if_ierrors++;
	csError (pCs, "No receive buffer available");
	/* Must read the length of all received frames */
	csPacketPageR (pCs, CS_PKTPG_RX_LENGTH);
	/* Skip the received frame */
	csPacketPageW (pCs, CS_PKTPG_RX_CFG,
		       csPacketPageR (pCs, CS_PKTPG_RX_CFG) | CS_RX_CFG_SKIP);
	return;
	}

    /* Copy the received frame from the chip to the buffer */
    csRxFrameCopy (pCs, pRxBuff);

    /* Process the received frame at task time */
    netJobAdd ((FUNCPTR)csRxProcess, (int)pCs, (int)pRxBuff, 0, 0, 0);
    }

/*******************************************************************************
*
* csRxFrameCopy - copies a received frame from the chip to a receive buffer
*
* This routine copies a received frame from the chip to a receive buffer.
*
* RETURNS: N/A
*/

LOCAL void csRxFrameCopy
    (
    CS_SOFTC *pCs,
    CS_RXBUF *pRxBuff
    )
    {
    FAST USHORT *pFrame = pCs->pPacketPage + (CS_PKTPG_RX_LENGTH/2);
    FAST int rxDataPort = pCs->ioAddr + CS_PORT_RXTX_DATA;
    FAST USHORT *pBuff;
    FAST USHORT *pBuffLimit;
    USHORT temp;

    /* Get the length of the received frame */
    if (pCs->inMemoryMode)
	{
	pRxBuff->length = *pFrame++;
	}
    else  /* In IO mode */
	{
	CS_IN_WORD (rxDataPort, &temp);  /* Discard RxStatus */
	CS_IN_WORD (rxDataPort, &pRxBuff->length);
	}

    /* Setup pointers to the buffer for copying */
    pBuff = (USHORT *)pRxBuff->data;
    pBuffLimit = pBuff + ((pRxBuff->length + 1)/2);

    /* Copy the frame from the chip to the buffer */
    if (pCs->inMemoryMode)
	while (pBuff < pBuffLimit)
	    *pBuff++ = *pFrame++;
    else
	while (pBuff < pBuffLimit)
	    {
	    CS_IN_WORD (rxDataPort, pBuff);
	    pBuff++;
	    }
    }

/*******************************************************************************
*
* csRxProcess - processes a received packet in task level
*
* This routine processes a received packet.  The received packet was copied to
* a receive buffer at interrupt time and this routine processses the receive
* buffer at task time via netTask().
*  
* If a recieve hook routine is specified, then the packet is given to the
* hook routine for processing.  If the received packet does not use trailers
* and the packet is large, then a cluster mbuf is built that points to the
* receive buffer directly.  If a cluster mbuf is not used, then the packet
* is copied to an mbuf chain.  The cluster mbuf or mbuf chain is then passed
* up to the protocol stack.
*
* RETURNS: N/A
*/

LOCAL void csRxProcess
    (
    CS_SOFTC *pCs,
    CS_RXBUF *pRxBuff
    )
    {
    struct ifnet *pIf = &pCs->arpCom.ac_if;
    struct mbuf *pMbufChain;
    ETH_HDR *pEtherHeader;
    UCHAR *pData;
    int dataLength;
#ifdef BSD43_DRIVER
    int trailerOffset;
    USHORT type;
#endif /* BSD43_DRIVER */

    /* If a hook routine is specified */
    if (etherInputHookRtn != NULL)
	{
	/* Call the hook routine */
	if ((*etherInputHookRtn) (pIf, pRxBuff->data, pRxBuff->length) != 0)
	    {
	    /* The hook routine has handled the received frame */
	    csRxBuffFree (pCs, pRxBuff);
	    return;
	    }
	}

    /* Setup working variables */
    pEtherHeader = (ETH_HDR *)pRxBuff->data;
    pData = &pRxBuff->data[SIZEOF_ETHERHEADER];
    dataLength = pRxBuff->length - SIZEOF_ETHERHEADER;

#ifdef BSD43_DRIVER
    /* Check if the received frame uses trailers */
    check_trailer (pEtherHeader, pData, &dataLength, &trailerOffset, pIf);

    /* Save the type because build_cluster() will overwrite it */
    type = pEtherHeader->ether_type;

    /* If trailers are not used and there is enough data for clustering */
    if ((trailerOffset == 0) && USE_CLUSTER (dataLength))
#else /* BSD43_DRIVER */
    /* If trailers are not used and there is enough data for clustering */
    if (USE_CLUSTER (dataLength))
#endif /* BSD43_DRIVER */
	{
	/* Build a cluster mbuf that points to my receive buffer */
	pMbufChain = build_cluster (pData, dataLength, pIf, CS_MC_LOANED,
	 &pRxBuff->refCount, (FUNCPTR)csRxBuffFree , (int)pCs, (int)pRxBuff, 0);
	if (pMbufChain != NULL)
	    {
	    pRxBuff->status = CS_RXBUF_LOANED;
	    pCs->loanCount++;
	    }
	else
	    csRxBuffFree (pCs, pRxBuff);
	}
    else  /* Can not do clustering */
	{
	/* Copy the received data to a chain of mbufs */
#ifdef BSD43_DRIVER
	pMbufChain = copy_to_mbufs (pData, dataLength, trailerOffset, pIf);
#else /* BSD43_DRIVER */
	pMbufChain = copy_to_mbufs (pData, dataLength, 0, pIf);
#endif /* BSD43_DRIVER */
	csRxBuffFree (pCs, pRxBuff);
	}

    /* If could not get an mbuf */
    if (pMbufChain == NULL)
	{
	pIf->if_ierrors++;
	csError (pCs, "No receive mbuf available");
	return;
	}

    /* Pass the mbuf chain up to the protocol stack */
#ifdef BSD43_DRIVER
    do_protocol_with_type (type, pMbufChain, &pCs->arpCom, dataLength);
#else /* BSD43_DRIVER */
    do_protocol (pEtherHeader, pMbufChain, &pCs->arpCom, dataLength);
#endif /* BSD43_DRIVER */

    /* Another packet successfully received! */
    pIf->if_ipackets++;
    }

/*******************************************************************************
*
* csRxBuffInit - initializes the receive buffers
*
* This routine initializes the network interface driver's collection of
* receive buffers.  The receive buffers are allocated from system memory
* and linked together in a linked list of free receive buffers.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS csRxBuffInit
    (
    CS_SOFTC *pCs
    )
    {
    CS_RXBUF *pRxBuff;
    CS_RXBUF *pRxLast;

    /* Allocate the receive frame buffers */
    pCs->pFreeRxBuff = (CS_RXBUF *)malloc (sizeof(CS_RXBUF) * CS_RXBUFCOUNT);
    if (pCs->pFreeRxBuff == NULL)
	return (ERROR);

    /* Link all the receive frame buffers together on the free list */
    pRxLast = pCs->pFreeRxBuff + CS_RXBUFCOUNT - 1;
    for (pRxBuff=pCs->pFreeRxBuff; pRxBuff<pRxLast; pRxBuff++)
	{
	pRxBuff->pNext  = pRxBuff + 1;
	pRxBuff->status = CS_RXBUF_FREE;
	}
    pRxLast->pNext  = NULL;
    pRxLast->status = CS_RXBUF_FREE;

    return (OK);
    }

/*******************************************************************************
*
* csRxBuffAlloc - removes a receive buffer from the free receive buffer list
*
* This routine removes a receive buffer from the free receive buffer list.
*
* RETURNS: pointer of the buffer
*/

LOCAL CS_RXBUF *csRxBuffAlloc
    (
    CS_SOFTC *pCs
    )
    {
    CS_RXBUF *pRxBuff;

    if (pCs->pFreeRxBuff == NULL)
	return (NULL);

    /* Remove a buffer from the free list */
    pRxBuff = pCs->pFreeRxBuff;
    pCs->pFreeRxBuff = pRxBuff->pNext;
    pRxBuff->pNext = NULL;

    /* Init the reference count to zero */
    pRxBuff->refCount = 0;

    /* The buffer is now allocated */
    pRxBuff->status = CS_RXBUF_ALLOCATED;

    /* Increment number of receive buffers currently in use */
    pCs->rxDepth++;
    if (pCs->rxDepth > pCs->maxRxDepth)
	pCs->maxRxDepth = pCs->rxDepth;

    return (pRxBuff);
    }

/*******************************************************************************
*
* csRxBuffFree - returns an allocated receive buffer back to the free list
*
* This routine returns an allocated receive buffer back to the free list.
*
* RETURNS: N/A
*/

LOCAL void csRxBuffFree
    (
    CS_SOFTC *pCs,
    CS_RXBUF *pRxBuff
    )
    {
    int intState = intLock();	/* INT LOCK (csRxBuffAlloc() won't interrupt) */

    /* Put the buffer at the head of the free list */
    pRxBuff->pNext = pCs->pFreeRxBuff;
    pCs->pFreeRxBuff = pRxBuff;

    /* The buffer is now free */
    pRxBuff->status = CS_RXBUF_FREE;

    /* Decrement the outstanding receive depth */
    pCs->rxDepth--;

    /* Re-enable interrupts at the CPU */
    intUnlock (intState);	/* INT UNLOCK */
    }

/*******************************************************************************
*
* csPacketPageR - reads a word from the PacketPage
*
* This routine reads a word from the PacketPage at the specified offset.
*
* RETURNS: a word in the offset
*/

LOCAL USHORT csPacketPageR
    (
    CS_SOFTC *pCs,
    USHORT offset
    )
    {
    if (pCs->inMemoryMode)
	{
	return (*((pCs->pPacketPage) + (offset/2)));
	}
    else  /* In IO mode */
	{
	USHORT temp;
	CS_OUT_WORD (pCs->ioAddr + CS_PORT_PKTPG_PTR, offset);
	CS_IN_WORD (pCs->ioAddr + CS_PORT_PKTPG_DATA, &temp);
	return (temp);
	}
    }

/*******************************************************************************
*
* csPacketPageW - writes a value to the PacketPage
*
* This routine writes a value to the PacketPage at the specified offset.
*
* RETURNS: N/A
*/

LOCAL void csPacketPageW
    (
    CS_SOFTC *pCs,
    USHORT offset,
    USHORT value
    )
    {
    if (pCs->inMemoryMode)
	{
	*((pCs->pPacketPage) + (offset/2)) = value;
	}
    else  /* In IO mode */
	{
	CS_OUT_WORD (pCs->ioAddr + CS_PORT_PKTPG_PTR, offset);
	CS_OUT_WORD (pCs->ioAddr + CS_PORT_PKTPG_DATA, value);
	}
    }

/*******************************************************************************
*
* csError - logs a message to the logging task
*
* This routine logs a message to the logging task if debugging is enabled.
*
* RETURNS: N/A
*/

LOCAL void csError
    (
    CS_SOFTC *pCs,
    char *pString
    )
    {
    /* If debugging is enabled */
    if (pCs->arpCom.ac_if.if_flags & IFF_DEBUG)
	{
	logMsg ("cs%d - %s\n", pCs->arpCom.ac_if.if_unit, (int)pString, 
		0, 0, 0, 0);
	}
    }

/*******************************************************************************
*
* csShow - shows statistics for the `cs' network interface
*
* This routine displays statistics about the `cs' Ethernet network interface.
* It has two parameters:
* .iP <unit>
* interface unit; should be 0.
* .iP <zap>
* if 1, all collected statistics are cleared to zero.
* .LP
*
* RETURNS: N/A
*/

void csShow
    (
    int unit,		/* interface unit */
    BOOL zap		/* zero totals */
    )
    {
    FAST CS_SOFTC *pCs = &cs_softc[unit];
    struct ifnet *pIf  = &pCs->arpCom.ac_if;

    if (unit >= MAXUNITS)
	{
	printf ("cs%d - Invalid unit number\n", unit);
	return;
	}

    if (zap)
	{
	/* Reset all the statistics to zero */
	pIf->if_ipackets   = 0;
	pIf->if_opackets   = 0;
	pIf->if_ierrors    = 0;
	pIf->if_oerrors    = 0;
	pIf->if_collisions = 0;

	/* Reset counters to zero */
	pCs->rxDepth    = 0;
	pCs->maxRxDepth = 0;
	pCs->maxTxDepth = 0;
	pCs->loanCount  = 0;
	}

    printf ("\n");
    printf ("Show cs%d: ", unit);
    printf ("Crystal Semiconductor CS8900 Network Interface Driver\n");
    printf ("I/O Address      : 0x%X\n", pCs->ioAddr);
    printf ("Interrupt Level  : %d\n",   pCs->intLevel);
    printf ("Interrupt Vector : %d\n",   pCs->intVector);

    printf ("Access Mode      : ");
    if (pCs->inMemoryMode)
	{
	printf ("Memory Mode\n");
	printf ("Memory Address   : 0x%X\n", (int)pCs->pPacketPage);
	}
    else 
	printf ("I/O Mode\n");

    printf ("Media Type       : ");
    switch (pCs->mediaType)
	{
	case CS_MEDIA_AUI:
	    printf ("AUI\n");
	    break;
	case CS_MEDIA_10BASE2:
	    printf ("10Base2\n");
	    break;
	case CS_MEDIA_10BASET:
	    if (pCs->configFlags & CS_CFGFLG_FDX)
		printf ("10BaseT, FDX\n");
	    else
		printf ("10BaseT\n");
	    break;
	default:
	    printf ("Unknown\n");
	    break;
	}

    printf ("Interface Flags  : ");
    if (pIf->if_flags & IFF_UP)
	printf ("UP");
    else 
	printf ("DOWN");
    if (pIf->if_flags & IFF_RUNNING)
	printf (", RUNNING");
    if (pIf->if_flags & IFF_BROADCAST)
	printf (", BROADCAST");
    if (pIf->if_flags & IFF_DEBUG)
	printf (", DEBUG");
    if (pIf->if_flags & IFF_LOOPBACK)
	printf (", LOOPBACK");
    if (pIf->if_flags & IFF_POINTOPOINT)
	printf (", POINTOPOINT");
    if (pIf->if_flags & IFF_NOTRAILERS)
	printf (", NOTRAILERS");
    if (pIf->if_flags & IFF_NOARP)
	printf (", NOARP");
    if (pIf->if_flags & IFF_PROMISC)
	printf (", PROMISC");
    if (pIf->if_flags & IFF_ALLMULTI)
	printf (", ALLMULTI");
    printf ("\n");

    printf ("Input Packets    : %d\n", pIf->if_ipackets);
    printf ("Output Packets   : %d\n", pIf->if_opackets);
    printf ("Input Errors     : %d\n", pIf->if_ierrors );
    printf ("Output Errors    : %d\n", pIf->if_oerrors );
    printf ("Collisions       : %d\n", pIf->if_collisions);
    printf ("Current Tx Depth : %d\n", pIf->if_snd.ifq_len);
    printf ("Current Rx Depth : %d\n", pCs->rxDepth);
    printf ("Maximum Tx Depth : %d\n", pCs->maxTxDepth);
    printf ("Maximum Rx Depth : %d\n", pCs->maxRxDepth);
    printf ("Loan Count       : %d\n", pCs->loanCount);

    printf ("\n");
    }
