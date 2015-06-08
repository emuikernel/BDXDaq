/* sysTempe.c - Tempe PCI-X to VME bridge chip top level interface file */

/* Copyright 2004 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01g,10aug04,scb Add error handling support.
01f,05may04,scb Tweek registers to speed up Tempe DMA.
01e,19mar04,scb Suppot routines for sysBusProbe().
01d,17mar04,scb Things encountered during Tempe DMA port development.
01c,09feb04,scb Add some window mapping support.
01b,17sep03,scb Add sysTempeReset() function.
01a,03sep03,scb Initial writing.
*/

/*
DESCRIPTION

This "top level" Tempe PCI-X to VME bridge chip support file contains
support functions for initializing the Tempe chip.  It #include's the
subordinate support files.
*/

/* includes */

#include "vxWorks.h"
#include "pci.h"
#include "drv/pci/pciConfigLib.h"
#include "sysLib.h"
#include "tempe.h"
#ifdef INCLUDE_ERROR_HANDLING
#   include "errorHandler.h"
#endif
#include "config.h"

/* force include of Show routines DJA 8/05 */
#define INCLUDE_TEMPE_SHOW_ROUTINES
#ifdef INCLUDE_TEMPE_SHOW_ROUTINES
#   include "sysTempeShow.c"
#endif

/* defines */

/* typedefs */

/* globals */

/* locals */

/*
 * This table contains static reset values for the Tempe chip.
 * These values will be programmed into the chip first, right
 * after a powerup or reboot.  The values programmed into the
 * chip from this table will put the chip into the same state
 * it would be in after a powerup.  Programming of operational
 * values into the chip is done elsewhere at a later time.
 */

LOCAL UINT32 tempeResetTbl [] =
    {

    /* PCI Configuration:  Header */

    NOT_INIT_TEMPE_REG (TEMPE_DEVI_VENI, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_STAT_CMMD, dynamic)
    NOT_INIT_TEMPE_REG (TEMPE_CLAS_REVI, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_HEAD_MLAT_CLSZ, dynamic)
    NOT_INIT_TEMPE_REG (TEMPE_MBARL, autoConfiged)
    NOT_INIT_TEMPE_REG (TEMPE_MBARU, autoConfiged)
    NOT_INIT_TEMPE_REG (TEMPE_SUBI_SUBV, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_CAPP, readOnly)
    YES_INIT_TEMPE_REG (TEMPE_MXLA_MNGN_INTP_INTL, 0x54010000)

    /* PCI Configuration: PCI-X Capabilities */

    YES_INIT_TEMPE_REG (TEMPE_PCIXCAP, 0x07007c00)
    NOT_INIT_TEMPE_REG (TEMPE_PCIXSTAT, readOnly)

    /* Outbound Functions: Outbound Translation 0 */

    YES_INIT_TEMPE_REG (TEMPE_OTSAU0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTSAL0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTEAU0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTEAL0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTOFU0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTOFL0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTBS0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTAT0, 0x00000000)

    /* Outbound Functions: Outbound Translation 1 */

    YES_INIT_TEMPE_REG (TEMPE_OTSAU1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTSAL1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTEAU1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTEAL1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTOFU1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTOFL1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTBS1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTAT1, 0x00000000)

    /* Outbound Functions: Outbound Translation 2 */


    YES_INIT_TEMPE_REG (TEMPE_OTSAU2, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTSAL2, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTEAU2, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTEAL2, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTOFU2, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTOFL2, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTBS2, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTAT2, 0x00000000)

    /* Outbound Functions: Outbound Translation 3 */

    YES_INIT_TEMPE_REG (TEMPE_OTSAU3, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTSAL3, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTEAU3, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTEAL3, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTOFU3, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTOFL3, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTBS3, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTAT3, 0x00000000)

    /* Outbound Functions: Outbound Translation 4 */

    YES_INIT_TEMPE_REG (TEMPE_OTSAU4, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTSAL4, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTEAU4, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTEAL4, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTOFU4, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTOFL4, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTBS4, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTAT4, 0x00000000)

    /* Outbound Functions: Outbound Translation 5 */

    YES_INIT_TEMPE_REG (TEMPE_OTSAU5, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTSAL5, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTEAU5, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTEAL5, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTOFU5, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTOFL5, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTBS5, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTAT5, 0x00000000)

    /* Outbound Functions: Outbound Translation 6 */

    YES_INIT_TEMPE_REG (TEMPE_OTSAU6, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTSAL6, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTEAU6, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTEAL6, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTOFU6, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTOFL6, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTBS6, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTAT6, 0x00000000)

    /* Outbound Functions: Outbound Translation 7 */

    YES_INIT_TEMPE_REG (TEMPE_OTSAU7, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTSAL7, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTEAU7, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTEAL7, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTOFU7, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTOFL7, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTBS7, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_OTAT7, 0x00000000)

    /* Outbound Functions:   VMEbus Interrupt Acknowledge */

    NOT_INIT_TEMPE_REG (TEMPE_VIACK1, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_VIACK2, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_VIACK3, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_VIACK4, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_VIACK5, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_VIACK6, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_VIACK7, readOnly)

    /* Outbound Functions: RMW */

    YES_INIT_TEMPE_REG (TEMPE_RMWAU, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_RMWAL, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_RMWEN, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_RMWC, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_RMWS, 0x00000000)

    /* Outbound Functions:  VMEbus Control */

    YES_INIT_TEMPE_REG (TEMPE_VMCTRL, 0x00000707)
    YES_INIT_TEMPE_REG (TEMPE_VCTRL, 0x00000008)
    YES_INIT_TEMPE_REG (TEMPE_VSTAT, 0x00008000)

    /* Outbound Functions: PCI configuration/status */

    NOT_INIT_TEMPE_REG (TEMPE_PSTAT, readOnly)

    /* VME Filters: VME Filter */

    YES_INIT_TEMPE_REG (TEMPE_VMEFL, 0x02000f00)

    /* VME Exception: VME Exception Status */

    NOT_INIT_TEMPE_REG (TEMPE_VEAU, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_VEAL, readOnly)
    YES_INIT_TEMPE_REG (TEMPE_VEAT, 0x20000000)

    /*  PCI Error: PCI Error Status */

    NOT_INIT_TEMPE_REG (TEMPE_EDPAU, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_EDPAL, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_EDPXA, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_EDPXS, readOnly)
    YES_INIT_TEMPE_REG (TEMPE_EDPAT, 0x20000000)

    /* Inbound Functions: Inbound Translation 0 */

    YES_INIT_TEMPE_REG (TEMPE_ITSAU0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITSAL0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITEAU0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITEAL0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITOFU0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITOFL0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITAT0, 0x00000000)

    /* Inbound Functions: Inbound Translation 1 */

    YES_INIT_TEMPE_REG (TEMPE_ITSAU1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITSAL1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITEAU1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITEAL1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITOFU1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITOFL1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITAT1, 0x00000000)

    /* Inbound Functions: Inbound Translation 2 */

    YES_INIT_TEMPE_REG (TEMPE_ITSAU2, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITSAL2, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITEAU2, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITEAL2, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITOFU2, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITOFL2, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITAT2, 0x00000000)

    /* Inbound Functions: Inbound Translation 3 */

    YES_INIT_TEMPE_REG (TEMPE_ITSAU3, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITSAL3, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITEAU3, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITEAL3, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITOFU3, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITOFL3, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITAT3, 0x00000000)

    /* Inbound Functions: Inbound Translation 4 */

    YES_INIT_TEMPE_REG (TEMPE_ITSAU4, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITSAL4, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITEAU4, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITEAL4, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITOFU4, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITOFL4, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITAT4, 0x00000000)

    /* Inbound Functions: Inbound Translation 5 */

    YES_INIT_TEMPE_REG (TEMPE_ITSAU5, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITSAL5, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITEAU5, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITEAL5, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITOFU5, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITOFL5, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITAT5, 0x00000000)

    /* Inbound Functions: Inbound Translation 6 */

    YES_INIT_TEMPE_REG (TEMPE_ITSAU6, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITSAL6, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITEAU6, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITEAL6, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITOFU6, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITOFL6, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITAT6, 0x00000000)

    /* Inbound Functions: Inbound Translation 7 */

    YES_INIT_TEMPE_REG (TEMPE_ITSAU7, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITSAL7, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITEAU7, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITEAL7, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITOFU7, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITOFL7, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_ITAT7, 0x00000000)

    /* Inbound Functions: Inbound Translation GCSR */

    YES_INIT_TEMPE_REG (TEMPE_GBAU, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_GBAL, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_GCSRAT, 0x00000000)

    /* Inbound Functions: Inbound Translation CRG */

    YES_INIT_TEMPE_REG (TEMPE_CBAU, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_CBAL, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_CSRAT, 0x00000000)

    /* Inbound Functions: Inbound Translation CR/CSR */

    YES_INIT_TEMPE_REG (TEMPE_CROU, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_CROL, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_CRAT, 0x00000000)

    /* Inbound Functions: Inbound Translation Location Monitor */

    YES_INIT_TEMPE_REG (TEMPE_LMBAU, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_LMBAL, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_LMAT, 0x00000000)

    /* Interrupt Functions: VMEbus Interrupt Control */

    YES_INIT_TEMPE_REG (TEMPE_64BCU, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_64BCL, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_BPGTR, 0x00000022)
    YES_INIT_TEMPE_REG (TEMPE_BPCTR, 0x000003e8)
    YES_INIT_TEMPE_REG (TEMPE_VICR, 0x0000000f)

    /* Interrupt Functions: Local Bus Interrupt Control */

    YES_INIT_TEMPE_REG (TEMPE_INTEN, TEMPE_INTEN_INIT_VAL)
    YES_INIT_TEMPE_REG (TEMPE_INTEO, 0x00000000)
    NOT_INIT_TEMPE_REG (TEMPE_INTS, readOnly)
    YES_INIT_TEMPE_REG (TEMPE_INTC, 0x03ff3f00)
    YES_INIT_TEMPE_REG (TEMPE_INTM1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_INTM2, 0x00000000)

    /* DMA Controller: DMA Controller 0 */

    YES_INIT_TEMPE_REG (TEMPE_DCTL0, 0x00000000)
    NOT_INIT_TEMPE_REG (TEMPE_DSTA0, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_DCSAU0, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_DCSAL0, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_DCDAU0, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_DCDAL0, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_DCLAU0, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_DCLAL0, readOnly)
    YES_INIT_TEMPE_REG (TEMPE_DSAU0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DSAL0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DDAU0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DDAL0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DSAT0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DDAT0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DNLAU0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DNLAL0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DCNT0, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DDBS0, 0x00000000)

    /* DMA Controller:  DMA Controller 1 */

    YES_INIT_TEMPE_REG (TEMPE_DCTL1, 0x00000000)
    NOT_INIT_TEMPE_REG (TEMPE_DSTA1, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_DCSAU1, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_DCSAL1, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_DCDAU1, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_DCDAL1, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_DCLAU1, readOnly)
    NOT_INIT_TEMPE_REG (TEMPE_DCLAL1, readOnly)
    YES_INIT_TEMPE_REG (TEMPE_DSAU1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DSAL1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DDAU1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DDAL1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DSAT1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DDAT1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DNLAU1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DNLAL1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DCNT1, 0x00000000)
    YES_INIT_TEMPE_REG (TEMPE_DDBS1, 0x00000000)

    /* Finally, we're at the end of the table */

    END_INIT_TEMPE_REG

    };

ERROR_UTILS tempeErrorUtils = { NULL, NULL };

/* forward declarations */

void sysTempeReset (void);
void sysTempeInit (void);
void sysTempeInit2 (void);
void sysTempeRegInit (UINT32 *p);
UINT32 sysTempeBaseAdrsGet (void);
BOOL sysVmeBusLock (void);
void sysVmeBusUnlock (void);
void sysTempeVeatClear (BOOL);

/* externals */

BOOL sysIsPciX (int);		/* Is this a PCI-X bus or not ? */
char * sysPhysMemTop (void);

/* subordinate support */

#include "sysTempeMap.c"
#include "sysTempeInt.c"
#include "sysTempeDma.c"
#ifdef INCLUDE_ERROR_HANDLING
#   include "sysTempeError.c"
#endif

/******************************************************************************
*
* sysTempeReset - Reprogram Tempe registers to powerup values
*
* This routine will program all of the Tempe registers to powerup
* reset values.  
*
* RETURNS: NA
*/

void sysTempeReset (void)
    {
    UINT32 reg;

    /* Program those registers which can be statically dealt with */

    sysTempeRegInit ((UINT32 *)tempeResetTbl);

    /* Clear error bits, set SERR PERR enables */

    reg = LONGSWAP(TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_STAT_CMMD));
    reg |= 0xfa300000;	/* Clear error bits */
    reg &= ~0x00000140; /* Clear SERR PERR enables */
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_STAT_CMMD, LONGSWAP(reg));
     
    /*
     * Program those which require dynamic adjustment: 
     *
     * Cache Line Size / Master Latency Timer / Header Type Registers
     * require that the MLAT field (15:8) be programmed as 0x80 for
     * PCI-X or 0x00 for PCI-C, the rest of the register is zero.
     */

    if (sysIsPciX (TEMPE_PCI_BUS))
	{
        TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_HEAD_MLAT_CLSZ, 
			    LONGSWAP(0x00008000));
	}
    else
	{
        TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_HEAD_MLAT_CLSZ, 
			    LONGSWAP(0x00000000));
	}
    }

/******************************************************************************
*
* sysTempeInit - Program the Tempe registers to their operational values.
*
* This routine will program the Tempe chip registers with application 
* specific values based on static #define's which appear elsewhere in
* this BSP.  Not all registers are necessarily programmed by this
* function.  It should be assumed that those registers which are not 
* programmed contain either powerup reset values or contain values previously
* programmed via call to sysTempeReset().
*
* RETURNS: NA
*/

void sysTempeInit (void)
    {
    int    i;

    LOCAL VME_OUT_WINDOW_CFG vmeOutWinCfg [] = 
				{ 
				    { VME_OUT0_CFG_PARAMS }, 
				    { VME_OUT1_CFG_PARAMS }, 
				    { VME_OUT2_CFG_PARAMS }, 
				    { VME_OUT3_CFG_PARAMS }, 
				    { VME_OUT4_CFG_PARAMS }, 
				    { VME_OUT5_CFG_PARAMS }, 
				    { VME_OUT6_CFG_PARAMS }, 
				    { VME_OUT7_CFG_PARAMS }
				};

    int vOutWinCount = sizeof (vmeOutWinCfg) / sizeof (VME_OUT_WINDOW_CFG);

    LOCAL VME_IN_WINDOW_CFG vmeInWinCfg [] = 
				{ 
				    { VME_IN0_CFG_PARAMS }, 
				    { VME_IN1_CFG_PARAMS }, 
				    { VME_IN2_CFG_PARAMS }, 
				    { VME_IN3_CFG_PARAMS }, 
				    { VME_IN4_CFG_PARAMS }, 
				    { VME_IN5_CFG_PARAMS }, 
				    { VME_IN6_CFG_PARAMS }, 
				    { VME_IN7_CFG_PARAMS }
				};

    int vInWinCount = sizeof (vmeInWinCfg) / sizeof (VME_IN_WINDOW_CFG);

    /* Configure outbound windows */

    for (i = 0; i < vOutWinCount; i++)
        sysTempeOutMapInit (i, &vmeOutWinCfg [i]); 

    /* Configure inbound windows */

    for (i = 0; i < vInWinCount; i++)
        sysTempeInMapInit (i, &vmeInWinCfg [i]); 

    sysTempeIntInit ();
    }

/******************************************************************************
*
* sysTempeRegInit - Initialize Tempe registers
*
* This function statically initializes specific Tempe registers specified
* by a table pointed at by the input parameter.
*
* RETURNS: NA
*/

void sysTempeRegInit
    (
    UINT32 *p           /* Pointer to initialization table */
    )
    {
    while (*p != END_INIT_TEMPE_REG)
        {
        switch (*p)
            {
            case TEMPE_INTRNL:
                TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_REG_OFFSET(p),
                                    TEMPE_REG_VALUE(p));
                p += TEMPE_INTRNL_INIT_SZ;
                break;

            default:
                break;  /* Should never get here, if so we're pooched!  */
            }
        }
    }

/*******************************************************************************
*
* sysTempeBaseAdrsGet() - return base address of Tempe CRG registers.
*
* This routine returns the address of the CRG register set of the Tempe
* chip.  The function returns the correct address only after the Tempe
* chip has been configured by pciAutoConfig().  If the Tempe chip is not
* present, zero is returned for the address.
*
* RETURNS: Tempe base address or zero if Tempe chip cannot be found.
*/

UINT32 sysTempeBaseAdrsGet (void)
    {
    UINT32 reg1;
    LOCAL UINT32 baseAddr = 0;

    if ((UINT32)baseAddr == 0)
	{
	pciConfigInLong (TEMPE_PCI_BUS, TEMPE_PCI_DEV, TEMPE_PCI_FCN, 
	PCI_CFG_BASE_ADDRESS_0, &reg1);

	if (reg1 != 0xffffffff)		/* If "Tempe" chip present */
	    {

	    reg1 &= PCI_MEMBASE_MASK;	/* Mask off "space" bits */

            /*
             * We have the "bus address" of the Tempe CRG space,
             * now convert it to a local address.
             */

            (void)sysBusToLocalAdrs (PCI_SPACE_MEM_PRI, (char *)reg1,
                                     (char **)&baseAddr);
            }
        }
    return((UINT32)baseAddr);
    }

/******************************************************************************
*
* sysTempeInit2 - Perform post autoConfig initialization of Tempe chip.
*
* This routine will perform the post initialization function, necessary
* after bootrom has done basic configuration and the kernel has taken
* control.
*
* RETURNS: NA
*/

void sysTempeInit2 (void)
    {

    /* Configure CRG group */

    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_CBAU, TEMPE_CBAU_VAL);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_CBAL, TEMPE_CBAL_VAL);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_CSRAT, TEMPE_CSRAT_VAL);

    /*
     * Capture the Tempe window mapping registers for future address 
     * translation support.
     */

    sysTempeCapt();

#ifdef INCLUDE_ERROR_HANDLING
    sysTempeErrorInit ();
#endif
    }

/******************************************************************************
*
* sysVmeBusLock - Lock the VME bus
*
* This routine attempts to lock the VME bus via the Tempe chip DWB (Device
* Wants Bus) bit in the VMEbus Master Control Register.
*
* RETURNS: TRUE if bus lock obtained, FALSE otherwise
*/

BOOL sysVmeBusLock (void)
    {

    UINT32 ticksToWait;	/* number of time base ticks to wait */
    UINT32 startTick;
    int	   lockKey;
    UINT32 reg32;
    BOOL   retVal;

    /*
     * convert LOCK_TIMEOUT to timebase ticks as follows:
     *
     * ticks/Usec = (bus frequency / decrementer pre-scaler) / 1000000
     *
     * ticks to wait = requested delay (in Usecs) * ticks/USec.
     */

    ticksToWait = LOCK_TIMEOUT *
                  ((cpuBusSpeed/DECREMENTER_PRESCALE)/USECS_PER_SECOND);

    /* lock interrupts so there will be no TAS interference */

    lockKey = intLock ();

    /* Request ownership of the VMEbus */

    reg32 = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_VMCTRL);
    reg32 |= TEMPE_VMCTRL_DWB_MASK;
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_VMCTRL, reg32);
    EIEIO_SYNC;

    /* snapshot starting time base low value */

    startTick = sysTimeBaseLGet ();

    /* Wait for the VME controller to give us the BUS or timeout */

    while ( ((TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_VMCTRL)) & 
                            TEMPE_VMCTRL_DHB_MASK) == 0 )
        {
        if ( (sysTimeBaseLGet () - startTick) >= ticksToWait)
	    break; 
	}

    intUnlock (lockKey);
    
    if ( ((TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_VMCTRL)) & 
                         TEMPE_VMCTRL_DHB_MASK) == 0 )
        retVal = FALSE;
    else
        retVal = TRUE;

    return (retVal);
   
    }

/******************************************************************************
*
* sysVmeBusUnlock - Unlock the VME bus
*
* This routine unlocks the VME bus via the Tempe chip DWB (Device
* Wants Bus) bit in the VMEbus Master Control Register.
*
* RETURNS: NA
*/

void sysVmeBusUnlock (void)
    {
    UINT32 reg32;
    int    lockKey;

    lockKey = intLock ();

    reg32 = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_VMCTRL);
    reg32 &= ~TEMPE_VMCTRL_DWB_MASK;
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_VMCTRL, reg32);

    intUnlock (lockKey);

    }

/******************************************************************************
* 
* sysTempeVeatClear - Clear the errors in the VME exceptions attribute reg 
*
* This routine will clear the VMEbus exception attributes register.  This
* register can hold only one error at a time, if a second error occurs before
* the first is cleared, that error record is lost and an overflow indicator
* is set in this register.  In addition to clearing the register, a
* conditional compile can be set which will log the register image if it
* indeed did reflect an error.
*/

void sysTempeVeatClear
    (
    BOOL logOrNot	/* TRUE if errors are logged, FALSE otherwise */
    )
    {
    UINT32 veatReg;
    veatReg = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_VEAT);

    /* Is there an error indicator captured ? */

    if ((veatReg & TEMPE_VEAT_VES_MASK) != 0) /* Did any kind of error occur? */
        {

	/* Yes, conditionally log the error and then clear it */

	if (logOrNot == TRUE)
	    {
            logMsg 
	    ("\nClearing VMEbus exception, attrib reg TEMPE_VEAT = 0x%08x\n", 
	             veatReg, 0,0,0,0,0);
	    }
        }

    /* Clear the register, whether it needs it or not */

    veatReg |= TEMPE_VEAT_VESCL_MASK;	
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_VEAT, veatReg);
    }
