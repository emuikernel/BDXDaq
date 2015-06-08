/* sysTempeMap.c - Tempe PCI-X to VME bridge chip address mapping support */

/* Copyright 2004 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01g,06apr04,scb Tempe RMW generation support.
01f,05apr04,scb Fix bug in use of vmeSpaceMask.
01e,18mar04,scb Add isMappedToVME() and sysVMEProbe() functions.
01d,17mar04,scb Move FSET32 macro out of this file.
01c,27feb04,scb Fix diab compiler warnings.
01b,09feb04,scb Add meat to the bones of the stubs.
01a,03sep03,scb Initial writing
*/

/*
DESCRIPTION

This file contains Tempe PCI-X to VME bridge chip address mapping support.
*/

/* includes */

#include "tempe.h"

/* defines */

/* typedefs */

/*
 * Windows for Tempe inbound/outbound transactions used in bus to local
 * and local to bus translation algorithms.
 */

typedef struct
    {
    UINT32 baseU;
    UINT32 baseL;
    UINT32 limitU;
    UINT32 limitL;
    UINT32 transU;
    UINT32 transL;
    UINT32 att;
    } VME_OUTBOUND_WIN;

typedef struct
    {
    UINT32 baseU;
    UINT32 baseL;
    UINT32 limitU;
    UINT32 limitL;
    UINT32 transU;
    UINT32 transL;
    UINT32 att;
    } VME_INBOUND_WIN;

/* globals */

/*
 * Offsets to the outbound window mapping related registers, indexed
 * by window number (0 - 7)
 */

UINT32 otsauX [8] = { TEMPE_OTSAU0, TEMPE_OTSAU1, TEMPE_OTSAU2, TEMPE_OTSAU3,
		      TEMPE_OTSAU4, TEMPE_OTSAU5, TEMPE_OTSAU6, TEMPE_OTSAU7 };	
UINT32 otsalX [8] = { TEMPE_OTSAL0, TEMPE_OTSAL1, TEMPE_OTSAL2, TEMPE_OTSAL3,
		      TEMPE_OTSAL4, TEMPE_OTSAL5, TEMPE_OTSAL6, TEMPE_OTSAL7 };	
UINT32 oteauX [8] = { TEMPE_OTEAU0, TEMPE_OTEAU1, TEMPE_OTEAU2, TEMPE_OTEAU3, 
		      TEMPE_OTEAU4, TEMPE_OTEAU5, TEMPE_OTEAU6, TEMPE_OTEAU7 };	
UINT32 otealX [8] = { TEMPE_OTEAL0, TEMPE_OTEAL1, TEMPE_OTEAL2, TEMPE_OTEAL3,
		      TEMPE_OTEAL4, TEMPE_OTEAL5, TEMPE_OTEAL6, TEMPE_OTEAL7 };	
UINT32 otofuX [8] = { TEMPE_OTOFU0, TEMPE_OTOFU1, TEMPE_OTOFU2, TEMPE_OTOFU3,
		      TEMPE_OTOFU4, TEMPE_OTOFU5, TEMPE_OTOFU6, TEMPE_OTOFU7 };	
UINT32 otoflX [8] = { TEMPE_OTOFL0, TEMPE_OTOFL1, TEMPE_OTOFL2, TEMPE_OTOFL3,
		      TEMPE_OTOFL4, TEMPE_OTOFL5, TEMPE_OTOFL6, TEMPE_OTOFL7 };	
UINT32 otbsX  [8] = { TEMPE_OTBS0,  TEMPE_OTBS1,  TEMPE_OTBS2,  TEMPE_OTBS3,
		      TEMPE_OTBS4,  TEMPE_OTBS5,  TEMPE_OTBS6,  TEMPE_OTBS7 };	
UINT32 otatX  [8] = { TEMPE_OTAT0,  TEMPE_OTAT1,  TEMPE_OTAT2,  TEMPE_OTAT3,
                      TEMPE_OTAT4,  TEMPE_OTAT5,  TEMPE_OTAT6,  TEMPE_OTAT7 };

/*
 * Offsets to the inbound window mapping related registers, indexed
 * by window number (0 - 7)
 */

UINT32 itsauX [8] = { TEMPE_ITSAU0, TEMPE_ITSAU1, TEMPE_ITSAU2, TEMPE_ITSAU3,
		      TEMPE_ITSAU4, TEMPE_ITSAU5, TEMPE_ITSAU6, TEMPE_ITSAU7 };	
UINT32 itsalX [8] = { TEMPE_ITSAL0, TEMPE_ITSAL1, TEMPE_ITSAL2, TEMPE_ITSAL3,
		      TEMPE_ITSAL4, TEMPE_ITSAL5, TEMPE_ITSAL6, TEMPE_ITSAL7 };	
UINT32 iteauX [8] = { TEMPE_ITEAU0, TEMPE_ITEAU1, TEMPE_ITEAU2, TEMPE_ITEAU3, 
		      TEMPE_ITEAU4, TEMPE_ITEAU5, TEMPE_ITEAU6, TEMPE_ITEAU7 };	
UINT32 itealX [8] = { TEMPE_ITEAL0, TEMPE_ITEAL1, TEMPE_ITEAL2, TEMPE_ITEAL3,
		      TEMPE_ITEAL4, TEMPE_ITEAL5, TEMPE_ITEAL6, TEMPE_ITEAL7 };	
UINT32 itofuX [8] = { TEMPE_ITOFU0, TEMPE_ITOFU1, TEMPE_ITOFU2, TEMPE_ITOFU3,
		      TEMPE_ITOFU4, TEMPE_ITOFU5, TEMPE_ITOFU6, TEMPE_ITOFU7 };	
UINT32 itoflX [8] = { TEMPE_ITOFL0, TEMPE_ITOFL1, TEMPE_ITOFL2, TEMPE_ITOFL3,
		      TEMPE_ITOFL4, TEMPE_ITOFL5, TEMPE_ITOFL6, TEMPE_ITOFL7 };	
UINT32 itatX  [8] = { TEMPE_ITAT0,  TEMPE_ITAT1,  TEMPE_ITAT2,  TEMPE_ITAT3,
                      TEMPE_ITAT4,  TEMPE_ITAT5,  TEMPE_ITAT6,  TEMPE_ITAT7 };

/* Offsets to Outbound window mapping regsiter set base */

UINT32 outWinOffset [] = { TEMPE_OTSAU0, TEMPE_OTSAU1, TEMPE_OTSAU2,
                           TEMPE_OTSAU3, TEMPE_OTSAU4, TEMPE_OTSAU5,
                           TEMPE_OTSAU6, TEMPE_OTSAU7
                         };
/* Offsets to Inbound window mapping register set base */

UINT32 inWinOffset [] = { TEMPE_ITSAU0, TEMPE_ITSAU1, TEMPE_ITSAU2,
                          TEMPE_ITSAU3, TEMPE_ITSAU4, TEMPE_ITSAU5,
                          TEMPE_ITSAU6, TEMPE_ITSAU7
                        };
/* locals */

LOCAL VME_OUTBOUND_WIN vmeOutWin [TEMPE_OUTBOUND_WINDOW_COUNT] = 
				                     { { 0,0,0,0,0,0,0 } };
LOCAL VME_INBOUND_WIN  vmeInWin  [TEMPE_INBOUND_WINDOW_COUNT] = 
						     { { 0,0,0,0,0,0,0 } };
LOCAL VME_OUT_MAP vmeOutMap;
LOCAL VME_IN_MAP  vmeInMap;

/* forward declarations */

LOCAL void sysTempeCapt (void);
LOCAL void sysTempeOutRegWrite (int, VME_OUT_MAP *);
LOCAL void sysTempeInRegWrite (int, VME_IN_MAP *);
LOCAL VME_OUT_MAP * sysTempeOutRegImg (VME_OUT_WINDOW_CFG *);
LOCAL VME_IN_MAP * sysTempeInRegImg (VME_IN_WINDOW_CFG *);

void getOutWinVals (UINT32, VME_OUT_MAP *);
void getInWinVals (UINT32, VME_IN_MAP *);
void sysTempeOutMapInit (int, VME_OUT_WINDOW_CFG *);
void sysTempeInMapInit (int, VME_IN_WINDOW_CFG *);
STATUS sysVmeToPciAdrs (int, char *, char **);
STATUS sysPciToVmeAdrs (int, char *, char **);
BOOL   isMappedToVME (char * pciAdrs, char ** vmeTrans, UINT32 * outRegSet);
STATUS sysVMEProbe (char *, int, int, char *);
void sysBusRmwEnable (UINT32, UINT32, UINT32, char *);
void sysBusRmwDisable (void);

/* externals */

IMPORT int    intLock ();
IMPORT int    intUnlock (int);
IMPORT STATUS sysPciProbe (char *, int, int, char *);

/******************************************************************************
*
* sysTempeCapt - Capture Tempe address translation information
*
* This routine captures the outbound and inbound address base, limit, 
* translation and control information from the Tempe chip.  This information
* is useful in performing address tranlations and ultimately supports
* the sysBusToLocalAdrs and sysLocalToBusAdrs functions.  This routine
* should be called every time a mapping change is made to any Tempe
* window mapping set since address translation functions are affected
* by these changes.
*
* RETURNS: N/A
*
* SEE ALSO: sysBusToLocalAdrs(), sysLocalToBusAdrs(), 
*/

LOCAL void sysTempeCapt (void)
    {
    int winSet;
    VME_OUT_MAP vOut;
    VME_IN_MAP vIn;

    for (winSet = 0; winSet < TEMPE_OUTBOUND_WINDOW_COUNT; winSet++)
	{
	getOutWinVals (winSet, &vOut);
	vmeOutWin[winSet].baseU  = vOut.otsauImg;
	vmeOutWin[winSet].baseL  = vOut.otsalImg & TEMPE_OTATLx_STAL_MASK;
	vmeOutWin[winSet].limitU = vOut.oteauImg;
	vmeOutWin[winSet].limitL = vOut.otealImg | 0x0000ffff;
	vmeOutWin[winSet].transU = vOut.otofuImg;
	vmeOutWin[winSet].transL = vOut.otoflImg & TEMPE_OTOFLx_OFFL_MASK;
	vmeOutWin[winSet].att    = vOut.otatImg;
	}

    for (winSet = 0; winSet < TEMPE_INBOUND_WINDOW_COUNT; winSet++)
	{
	getInWinVals (winSet, &vIn);
	vmeInWin[winSet].baseU  = vIn.itsauImg;
	vmeInWin[winSet].baseL  = vIn.itsalImg & TEMPE_ITATLx_STAL_MASK;
	vmeInWin[winSet].limitU = vIn.iteauImg;
	vmeInWin[winSet].limitL = vIn.itealImg | 0x0000000f;
	vmeInWin[winSet].transU = vIn.itofuImg;
	vmeInWin[winSet].transL = vIn.itoflImg & TEMPE_ITOFLx_OFFL_MASK;
	vmeInWin[winSet].att    = vIn.itatImg;
	}
    }

/******************************************************************************
* 
* sysTempeOutRegWrite - Write one outbound window mapping set into Tempe chip
*
* This routine takes an image of one of the outbound window Tempe register 
* set and actually writes it to the chip.
*
* RETURNS: NA
*/

LOCAL void sysTempeOutRegWrite
    (
    int winSet,			    /* VME outbound window number (0 - 7) */
    VME_OUT_MAP * vmeOutMap         /* VME outbound register images values */
    )
    {
    UINT32 reg;

    /* 
     * Write with "push" each of the register images but first disable
     * the window.
     */

    reg = TEMPE_READ32 (TEMPE_REG_BASE, otatX [winSet]);
    reg &= ~TEMPE_OTATx_EN_MASK;

    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, otatX [winSet], reg);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, otsauX [winSet], vmeOutMap->otsauImg);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, otsalX [winSet], vmeOutMap->otsalImg);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, oteauX [winSet], vmeOutMap->oteauImg);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, otealX [winSet], vmeOutMap->otealImg);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, otofuX [winSet], vmeOutMap->otofuImg);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, otoflX [winSet], vmeOutMap->otoflImg);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, otbsX [winSet], vmeOutMap->otbsImg);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, otatX [winSet], vmeOutMap->otatImg);

    sysTempeCapt();
    }

/******************************************************************************
* 
* sysTempeInRegWrite - Write one inbound window mapping set into Tempe chip
*
* This routine takes an image of one of the inbound window Tempe register 
* set and actually writes it to the chip.
*
* RETURNS: NA
*/

LOCAL void sysTempeInRegWrite
    (
    int winSet,			  /* VME outbound window number (0 - 7) */
    VME_IN_MAP * vmeInMap         /* VME outbound register images values */
    )
    {
    UINT32 reg;

    /*
     * Write with "push" each of the register images but first disable
     * the window.
     */

    reg = TEMPE_READ32 (TEMPE_REG_BASE, itatX [winSet]);
    reg &= ~TEMPE_ITATx_EN_MASK;
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, itatX [winSet], reg);

    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, itsauX [winSet], vmeInMap->itsauImg);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, itsalX [winSet], vmeInMap->itsalImg);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, iteauX [winSet], vmeInMap->iteauImg);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, itealX [winSet], vmeInMap->itealImg);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, itofuX [winSet], vmeInMap->itofuImg);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, itoflX [winSet], vmeInMap->itoflImg);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, itatX [winSet], vmeInMap->itatImg);

    sysTempeCapt();

    }

/******************************************************************************
* 
* sysTempeOutRegImg - Compute one Tempe outbound register image set
*
* This routine takes a structure describing window parameters and computes
* the actual Tempe outbound register set image.
*
* RETURNS: Outbound register set image.
*/

LOCAL VME_OUT_MAP * sysTempeOutRegImg
    (
    VME_OUT_WINDOW_CFG * pCfg   /* Desired window configuration */
    )
    {

    UINT32 upperHalf;
    UINT32 lowerHalf;
    UINT64 val64;

    /* Set up variables in which we will stage the programming values */

    vmeOutMap.otsauImg = 0;
    vmeOutMap.otsalImg = 0;
    vmeOutMap.oteauImg = 0;
    vmeOutMap.otealImg = 0;
    vmeOutMap.otofuImg = 0;
    vmeOutMap.otoflImg = 0;
    vmeOutMap.otbsImg = 0;
    vmeOutMap.otatImg = 0;

    /* 
     * Build register images based on parameters passed in pCfg
     * First let's build the otsau (outbound translation starting 
     * address upper) image and the otsal (outbound translation starting 
     * address lower).
     */

    FSET32 (vmeOutMap.otsauImg, pCfg->pciBusAddrU, 0);
    FSET32 (vmeOutMap.otsalImg, 
	    (pCfg->pciBusAddrL & TEMPE_OTATLx_STAL_MASK), 0);

    /*
     * Now that the starting address registers images are built, let's build 
     * the oteal (outbound translation ending address lower) and oteau 
     * (outbound translation ending address upper) image.  For an address to 
     * hit inside the window it must be >= the otsau set and <= the oteal set.
     * Thus the upper address register = start + size - 1.
     *
     * compute end = start + size - 1
     */

    val64 = (MAKE64(pCfg->pciBusAddrU, pCfg->pciBusAddrL) +
             MAKE64(pCfg->windowSizeU,  pCfg->windowSizeL)) - (UINT64)(1);
    UNMAKE64(val64, upperHalf, lowerHalf);

    FSET32 (vmeOutMap.oteauImg, upperHalf, 0);
    FSET32 (vmeOutMap.otealImg, (lowerHalf & TEMPE_OTEALx_ENDL_MASK), 0);

    /* 
     * Now we need to to deal with the translated address on the VME bus.  
     * We are given a PCI bus address and the desired VME bus address.
     * Both of these addresses are 64-bits long, expressed as 32-bit lower
     * and upper quantities.  We must come up with a pair of 32-bit 
     * translation (lower and upper) values which, when added tothe PCI
     * components yield the VME components.  The basic formula is:
     *
     * PCI_address + translate_value = VME_address
     *
     * Solving for translate_value we get:
     *
     * translate_value = VME_address - PCI_address
     */

    val64 = MAKE64(pCfg->vmeBusAddrU, pCfg->vmeBusAddrL) -
	    MAKE64(pCfg->pciBusAddrU, pCfg->pciBusAddrL);
    UNMAKE64(val64, upperHalf, lowerHalf);

    FSET32 (vmeOutMap.otoflImg, (lowerHalf & TEMPE_OTATLx_STAL_MASK), 0);
    FSET32 (vmeOutMap.otofuImg, upperHalf, 0);

    /*
     * Now we will deal with the "Outbound translation 2eSST broadcast select" 
     * image.
     */

    FSET32 (vmeOutMap.otbsImg, pCfg->bcastSelect2esst, 0);

    /*
     * Finally, the "Outbound translation attribute" image" , there are a 
     * lot of little fields in this image which we set one at a time.
     */

    if (pCfg->rdPrefetchEnable == FALSE)
        FSET32 (vmeOutMap.otatImg, 1, TEMPE_OTATx_MRPFD_BIT); 

    FSET32 (vmeOutMap.otatImg, pCfg->rdPrefetchSize, TEMPE_OTATx_PFSx_BIT);
    FSET32 (vmeOutMap.otatImg, pCfg->vme2esstRate, TEMPE_OTATx_2eSSTMx_BIT);
    FSET32 (vmeOutMap.otatImg, pCfg->xferProtocolOut, TEMPE_OTATx_TMx_BIT);
    FSET32 (vmeOutMap.otatImg, pCfg->dataWidth, TEMPE_OTATx_DBWx_BIT);
    if (pCfg->superAccessType == TRUE)
	FSET32 (vmeOutMap.otatImg, 1, TEMPE_OTATx_SUP_BIT);
    if (pCfg->pgmAccessType == TRUE)
	FSET32 (vmeOutMap.otatImg, 1, TEMPE_OTATx_PGM_BIT);
    FSET32 (vmeOutMap.otatImg, pCfg->addressMode, 0);

    if (pCfg->windowEnable == TRUE)
	FSET32 (vmeOutMap.otatImg, 1, TEMPE_OTATx_EN_BIT);

    return (&vmeOutMap);     
    }

/******************************************************************************
* 
* sysTempeInRegImg - Compute one Tempe inbound register image set
*
* This routine takes a structure describing window parameters and computes
* the actual Tempe inbound register set image.
*
* RETURNS: Inbound register set image.
*/

LOCAL VME_IN_MAP * sysTempeInRegImg
    (
    VME_IN_WINDOW_CFG * pCfg    /* Desired window configuration */
    )
    {
    UINT32 upperHalf;
    UINT32 lowerHalf;
    UINT64 val64;

    /* Set up variables in which we will stage the programming values */

    vmeInMap.itsauImg = 0;
    vmeInMap.itsalImg = 0;
    vmeInMap.iteauImg = 0;
    vmeInMap.itealImg = 0;
    vmeInMap.itofuImg = 0;
    vmeInMap.itoflImg = 0;
    vmeInMap.itatImg = 0;

    /* 
     * Build register images based on parameters passed in pCfg
     * First let's build the itsau (inbound translation starting 
     * address upper) image and the itsal (inbound translation starting 
     * address lower).
     */

    FSET32 (vmeInMap.itsauImg, pCfg->vmeBusAddrU, 0);
    FSET32 (vmeInMap.itsalImg, 
	    (pCfg->vmeBusAddrL & TEMPE_ITATLx_STAL_MASK), 0);

    /*
     * Now that the starting address registers images are built, let's build 
     * the iteal (inbound translation ending address lower) and iteau 
     * (inbound translation ending address upper) image.  For an address to 
     * hit inside the window it must be >= the itsau set and <= the iteal set.
     * Thus the upper address register = start + size - 1.
     */

    
    val64 = MAKE64(pCfg->vmeBusAddrU, pCfg->vmeBusAddrL) +
            MAKE64(pCfg->windowSizeU, pCfg->windowSizeL) - (UINT64)(1);
    UNMAKE64(val64, upperHalf, lowerHalf);

    FSET32 (vmeInMap.iteauImg, upperHalf, 0);
    FSET32 (vmeInMap.itealImg, (lowerHalf & TEMPE_ITEALx_ENDL_MASK), 0);

    /* 
     * Now we need to to deal with the translated address on the PCI bus.  
     * We are given a VME bus address and the desired PCI bus address.
     * Both of these addresses are 64-bits long, expressed as 32-bit lower
     * and upper quantities.  We must come up with a pair of 32-bit 
     * translation (lower and upper) values which, when added tothe VME
     * components yield the PCI components.  The basic formula is:
     *
     * VME_address + translate_value = PCI_address
     *
     * Solving for translate_value we get:
     *
     * translate_value = PCI_address - VME_address
     */

    
    val64 = MAKE64(pCfg->pciBusAddrU, pCfg->pciBusAddrL) -
	    MAKE64(pCfg->vmeBusAddrU, pCfg->vmeBusAddrL);
    UNMAKE64(val64, upperHalf, lowerHalf);

    FSET32 (vmeInMap.itoflImg, (lowerHalf & TEMPE_ITOFLx_OFFL_MASK), 0);
    FSET32 (vmeInMap.itofuImg, upperHalf, 0);

    /*
     * Finally, the "Inbound translation attribute" image" , there are a 
     * lot of little fields in this image which we set one at a time.
     */

    FSET32 (vmeInMap.itatImg, pCfg->rdThreshold, TEMPE_ITATx_TH_BIT);
    FSET32 (vmeInMap.itatImg, pCfg->vfsFifoSize, TEMPE_ITATx_VFSx_BIT);
    FSET32 (vmeInMap.itatImg, pCfg->vme2esstRate, TEMPE_ITATx_2eSSTMx_BIT);
    FSET32 (vmeInMap.itatImg, pCfg->xferProtocolIn, TEMPE_ITATx_PRTCL_BIT);
    FSET32 (vmeInMap.itatImg, pCfg->addressSpace, TEMPE_ITATx_ASx_BIT);

    if (pCfg->superAccessSlave == TRUE)
	FSET32 (vmeInMap.itatImg, 1, TEMPE_ITATx_SUPR_BIT);
    if (pCfg->nonSuperAccessSlave == TRUE)
	FSET32 (vmeInMap.itatImg, 1, TEMPE_ITATx_NPRIV_BIT);
    if (pCfg->pgmAccessSlave == TRUE)
	FSET32 (vmeInMap.itatImg, 1, TEMPE_ITATx_PGM_BIT);
    if (pCfg->dataAccessSlave == TRUE)
	FSET32 (vmeInMap.itatImg, 1, TEMPE_ITATx_DATA_BIT);

    if (pCfg->windowEnable == TRUE)
	FSET32 (vmeInMap.itatImg, 1, TEMPE_ITATx_EN_BIT);

    return (&vmeInMap);     
    }

/******************************************************************************
* 
* getOutWinVals - Get Tempe Outbound window mapping set
*
* This routine obtains one set of eight Tempe outbound window mapping register
* values for one of the eight outbound Tempe mapping register sets.
*
* RETURNS: NA
*/

void getOutWinVals
    (
    UINT32 winSet,	/* Which set (0 -7) to return */
    VME_OUT_MAP *v	/* Return the 8-register set here */
    )
    {
    UINT32 off = outWinOffset[winSet];

    v->otsauImg = TEMPE_READ32(TEMPE_REG_BASE,off+(TEMPE_OTSAU0-TEMPE_OTSAU0));
    v->otsalImg = TEMPE_READ32(TEMPE_REG_BASE,off+(TEMPE_OTSAL0-TEMPE_OTSAU0));
    v->oteauImg = TEMPE_READ32(TEMPE_REG_BASE,off+(TEMPE_OTEAU0-TEMPE_OTSAU0));
    v->otealImg = TEMPE_READ32(TEMPE_REG_BASE,off+(TEMPE_OTEAL0-TEMPE_OTSAU0));
    v->otofuImg = TEMPE_READ32(TEMPE_REG_BASE,off+(TEMPE_OTOFU0-TEMPE_OTSAU0));
    v->otoflImg = TEMPE_READ32(TEMPE_REG_BASE,off+(TEMPE_OTOFL0-TEMPE_OTSAU0));
    v->otbsImg  = TEMPE_READ32(TEMPE_REG_BASE,off+(TEMPE_OTBS0 -TEMPE_OTSAU0));
    v->otatImg  = TEMPE_READ32(TEMPE_REG_BASE,off+(TEMPE_OTAT0 -TEMPE_OTSAU0));
    }

/******************************************************************************
* 
* getInWinVals - Get Tempe Inbound window mapping set
*
* This routine obtains one set of seven Tempe outbound window mapping register
* values for one of the eight outbound Tempe mapping register sets.
*
* RETURNS: NA
*/

void getInWinVals
    (
    UINT32 winSet,	/* Which set (0 - 7) to return */
    VME_IN_MAP 	*v	/* Return the 7-register set here */
    )
    {
    UINT32 off = inWinOffset[winSet];
    v->itsauImg = TEMPE_READ32(TEMPE_REG_BASE,off+(TEMPE_ITSAU0-TEMPE_ITSAU0));
    v->itsalImg = TEMPE_READ32(TEMPE_REG_BASE,off+(TEMPE_ITSAL0-TEMPE_ITSAU0));
    v->iteauImg = TEMPE_READ32(TEMPE_REG_BASE,off+(TEMPE_ITEAU0-TEMPE_ITSAU0));
    v->itealImg = TEMPE_READ32(TEMPE_REG_BASE,off+(TEMPE_ITEAL0-TEMPE_ITSAU0));
    v->itofuImg = TEMPE_READ32(TEMPE_REG_BASE,off+(TEMPE_ITOFU0-TEMPE_ITSAU0));
    v->itoflImg = TEMPE_READ32(TEMPE_REG_BASE,off+(TEMPE_ITOFL0-TEMPE_ITSAU0));
    v->itatImg  = TEMPE_READ32(TEMPE_REG_BASE,off+(TEMPE_ITAT0 -TEMPE_ITSAU0));
    }

/******************************************************************************
* 
* sysTempeOutMapInit - Initialize one set of Tempe outbound windows.
*
* This takes an input structure describing the characterists of one
* Tempe outbound window set, tranlates it into actual Tempe register
* values and programs the Tempe window mapping set with the computed
* values.
*
* RETURNS: NA
*/

void sysTempeOutMapInit 
    (
    int		  winSet,	   /* Output window group number */
    VME_OUT_WINDOW_CFG * vOutCfg   /* Mapping parameters */ 
    )
    {
    VME_OUT_MAP * vmeOut;

    /* Compute the values for the outbound register image set */

    vmeOut = sysTempeOutRegImg (vOutCfg);

    /* Now write the outbound register set to the chip */

    sysTempeOutRegWrite (winSet, vmeOut);
    }

/******************************************************************************
* 
* sysTempeInMapInit - Initialize one set of Tempe inbound windows.
*
* This takes an input structure describing the characterists of one
* Tempe inbound window set, tranlates it into actual Tempe register
* values and programs the Tempe window mapping set with the computed
* values.
*
* RETURNS: NA
*/

void sysTempeInMapInit
    (
    int		  winSet,   	   /* Input window group number */
    VME_IN_WINDOW_CFG * vInCfg     /* Mapping parameters */
    )
    {
    VME_IN_MAP * vmeIn;

    /* Compute the values for the inbound register image set */

    vmeIn = sysTempeInRegImg (vInCfg);

    /* Now write the inbound register set to the chip */

    sysTempeInRegWrite (winSet, vmeIn);
    }

/******************************************************************************
*
* sysVmeToPciAdrs - convert a VME bus address to a PCI address
*
* This routine converts a given a VME address and VME address modifier, 
* to a  corresponding PCI address provided such an address exists.  This
* routine supports the more general sysBusToLocalAdrs() function.  This
* conversion concerns itself with the outbound windows of the Tempe chip.
* That is, the given address (address to convert) is assumed to be the
* target of a translation and this function returns the PCI address which
* would access this target VME address.
*
* RETURNS: OK, or ERROR if the address space is unknown or the mapping is not
* possible.
*
* SEE ALSO: vmeLocalToBusAddrs()
*/

STATUS sysVmeToPciAdrs
    (
    int     vmeAdrsSpace, /* VME bus adress space where busAdrs resides */
    char *  vmeBusAdrs,   /* VME bus adress to convert */
    char ** pPciAdrs      /* where to return converted local (PCI) adress */
    )
    {
    int i;
    char * pciBusAdrs = 0;
    UINT32 busH;
    UINT32 busL;
    UINT64 base;
    UINT64 limit;
    UINT64 trans;
    UINT64 busAdrs;
    UINT64 vmeAdrToConvert;
    BOOL   adrConverted;
    UINT32 vmeSpaceMask;

    /* 
     * We are going to look through each of the outbound windows to find
     * one which covers the VME bus adress and also passes the following
     * tests:
     *	 - Window is enabled.
     *	 - Window's adress mode is compatable with the adress space
     *	     parameter.
     */

    adrConverted = FALSE;

    for (i = 0; i < TEMPE_OUTBOUND_WINDOW_COUNT; i++)
        {

	/* If window is enabled ... */

        if ((vmeOutWin[i].att & TEMPE_OTATx_EN_VAL_ENABLED) != 0)
            {

	    /* It is enabled */

            switch (vmeAdrsSpace)
                {
                case VME_AM_EXT_SUP_PGM:
                case VME_AM_EXT_SUP_DATA:
                case VME_AM_EXT_USR_PGM:
                case VME_AM_EXT_USR_DATA:

		    /* See if the window is A32 enabled */

		    if ( ((vmeOutWin[i].att & TEMPE_OTATx_AMODEx_MASK) ==
			 (TEMPE_OTATx_AMODE_VAL_A32)) ||
			 ((vmeOutWin[i].att & TEMPE_OTATx_AMODEx_MASK) ==
			 (TEMPE_OTATx_AMODE_VAL_CSR)) )
			{
			vmeSpaceMask = 0xffffffff;
			vmeAdrToConvert = (UINT32)vmeBusAdrs;
			break;
			}
                    else
                        continue;

                case VME_AM_STD_SUP_PGM:
                case VME_AM_STD_SUP_DATA:
                case VME_AM_STD_USR_PGM:
                case VME_AM_STD_USR_DATA:

		    /* See if the window is A24 enabled */

		    if ((vmeOutWin[i].att & TEMPE_OTATx_AMODEx_MASK) ==
			(TEMPE_OTATx_AMODE_VAL_A24))
			{
			vmeSpaceMask = 0x00ffffff;
			vmeAdrToConvert = (UINT32)vmeBusAdrs & vmeSpaceMask;
			break;
			}
                    else
                        continue;

                case VME_AM_SUP_SHORT_IO:
                case VME_AM_USR_SHORT_IO:

		    /* See if the window is A24 enabled */

		    if ((vmeOutWin[i].att & TEMPE_OTATx_AMODEx_MASK) ==
			(TEMPE_OTATx_AMODE_VAL_A16))
			{
			vmeSpaceMask = 0x0000ffff;
			vmeAdrToConvert = (UINT32)vmeBusAdrs & vmeSpaceMask;
			break;
			}
                    else
                        continue;

		default:
			return (ERROR);		/* invalid address space */
                }

	    base = MAKE64(vmeOutWin[i].baseU, vmeOutWin[i].baseL);
	    limit = MAKE64(vmeOutWin[i].limitU, vmeOutWin[i].limitL);
	    trans = MAKE64(vmeOutWin[i].transU, vmeOutWin[i].transL);

	    /* If VME address is numerically targeted by this window ... */

	    if ( ((UINT64)(base + trans) <= vmeAdrToConvert) &&
	         ((UINT64)(limit + trans) >= vmeAdrToConvert) ) 
	        {
	        busAdrs = vmeAdrToConvert - trans;
	        UNMAKE64(busAdrs, busH, busL);
	        pciBusAdrs = (char *)busL;
	        adrConverted = TRUE;
	        break;
	        }
	    else
	        continue;
            }
        }

    if (adrConverted == TRUE)
	{
        *pPciAdrs = pciBusAdrs;
	return (OK);
	}
    return (ERROR);
    }

/******************************************************************************
*
* sysPciBusToVmeAdrs - convert a PCI bus address to a VME address
*
* This routine converts a given a PCI address and VME address modifier, 
* to a  corresponding VME address provided such an address exists.  This
* routine supports the more general sysLocalToBusAdrs() function.  This
* conversion concerns itself with the inbound windows of the Tempe chip.
* That is, the given address (address to convert) is assumed to be the
* target of a translation and this function returns the VME address which
* would access this target PCI address.
*
* RETURNS: OK, or ERROR if the address space is unknown or the mapping is not
* possible.
*
* SEE ALSO: sysVmeToPciAdrs()
*/

STATUS sysPciToVmeAdrs
    (
    int     vmeAdrsSpace, /* VME bus adress space where busAdrs resides */
    char *  pciBusAdrs,   /* PCI bus adress to convert */
    char ** pVmeAdrs      /* where to return converted VME adress */
    )
    {
    int i;
    UINT32 itatxAsx;
    UINT32 busH;
    UINT32 busL;
    UINT64 base;
    UINT64 limit;
    UINT64 trans;
    UINT64 busAdrs;
    UINT64 pciAdrToConvert;
    BOOL   adrConverted;
    UINT32 vmeSpaceMask;

    adrConverted = FALSE;

    switch (vmeAdrsSpace)
        {
        case VME_AM_EXT_SUP_PGM:
        case VME_AM_EXT_SUP_DATA:
        case VME_AM_EXT_USR_PGM:
        case VME_AM_EXT_USR_DATA:

            vmeSpaceMask = 0xffffffff;
	    itatxAsx = TEMPE_ITATx_ASx_VAL_A32;
            break;

        case VME_AM_STD_SUP_PGM:
        case VME_AM_STD_SUP_DATA:
        case VME_AM_STD_USR_PGM:
        case VME_AM_STD_USR_DATA:

            vmeSpaceMask = 0x00ffffff;
	    itatxAsx = TEMPE_ITATx_ASx_VAL_A24;
            break;

        case VME_AM_SUP_SHORT_IO:
        case VME_AM_USR_SHORT_IO:

            vmeSpaceMask = 0x0000ffff;
            itatxAsx = TEMPE_ITATx_ASx_VAL_A16;
            break;

	default:
	    return (ERROR);		/* invalid address space */
        }

    for (i = 0; i < TEMPE_INBOUND_WINDOW_COUNT; i++)
        {

	/* If window is enabled ... */

        if ( (vmeInWin[i].att & TEMPE_ITATx_EN_VAL_ENABLED) != 0)
	    {

	    /* If window address space jibes */

	    if ((vmeInWin[i].att & TEMPE_ITATx_ASx_MASK) == itatxAsx)
                {

	        base = MAKE64(vmeInWin[i].baseU, vmeInWin[i].baseL);
	        limit = MAKE64(vmeInWin[i].limitU, vmeInWin[i].limitL);
	        trans = MAKE64(vmeInWin[i].transU, vmeInWin[i].transL);
	        pciAdrToConvert = (UINT32)pciBusAdrs;

		/* If PCI address is numerically targeted by this window ... */

	        if ( ((base + trans) <= pciAdrToConvert) &&
	             ((limit + trans) >= pciAdrToConvert) ) 
		    {
		    busAdrs = pciAdrToConvert - trans;
		    UNMAKE64(busAdrs, busH, busL);
		    busL &= vmeSpaceMask;
		    *pVmeAdrs = (char *)busL;
		    adrConverted = TRUE;
		    break;
		    }
                }
	    }
	}

    if (adrConverted == TRUE)
	return (OK);
    else
	return (ERROR);
    }

/******************************************************************************
*
* isMappedToVME - determine if local PCI address maps to VME bus
*
* Given a PCI address, determine if there is any enabled Tempe outbound
* window which maps this address onto the VME bus.  If a mapping exists,
* return a status indicating that fact as well as the VME address to which
* the PCI address translates and the number (0 thru 7) of the outbound
* Tempe mapping register set which actually performs the mapping.  This
* function can be used in support of bus probing.  Note that if a mapping
* exists, it does not guarantee that there is a responder on the VME
* bus at the mapped address.
*
* RETURNS: TRUE if mapping exists, FALSE otherwise.
*/

BOOL isMappedToVME
    (
    char    * pciAdrs,   /* Local (PCI) address to see if it maps */
    char   ** vmeTrans,  /* If it's VME, this will be the VME bus address */
    UINT32  * outRegSet  /* Outbound window mapping set which maps this addr */
    )
    {
    BOOL   stat = FALSE;
    int    i;

    /*
     * See if pciAdrs hits any outbound window, if so translate it
     * to VME address and return the outbound register set number.
     */

    for (i = 0; i < TEMPE_OUTBOUND_WINDOW_COUNT; i++)
        {

	/* If window is enabled ... */

        if ((vmeOutWin[i].att & TEMPE_OTATx_EN_VAL_ENABLED) != 0)
            {

            /* If address hits inside of this window ... */

	    /* If VME address is numerically targeted by this window ... */

	    if ( ((UINT64)(vmeOutWin[i].baseL) <= (UINT32)pciAdrs) &&
	         ((UINT64)(vmeOutWin[i].limitL) >= (UINT32)pciAdrs) ) 
	        {

		/* Assume 32-bit (only "low" portion of translation needed */

		*vmeTrans = pciAdrs + vmeOutWin[i].transL;
		*outRegSet = i;
		stat = TRUE;
		break;
		}
            }
	}
    return (stat);
    }

/******************************************************************************
*
* sysVMEProbe - probe a VME bus address
*
* This routine probes an address on the VME bus and returns an indication 
* of whether the probe resulted in and VMEbus esception or not.
*
* RETURNS: OK if probing does not result in VME exception, ERROR otherwis.
*/

STATUS sysVMEProbe
    (
    char   * adrs,     /* address to be probed */
    int      mode,     /* VX_READ or VX_WRITE */
    int      length,   /* 1, 2 or 4 byte probe */
    char   * pVal      /* address of value to write OR */
                       /* address of location to place value read */
    )
    {
    STATUS stat = ERROR;
    UINT32 veatReg;

    int    oldLevel;

    /* probe performed with interrupts disabled */

    oldLevel = intLock ();

    sysTempeVeatClear (FALSE);

    /* Probe as a PCI probe since all VME is PCI on this board */

    stat = sysPciProbe (adrs, mode, length, pVal);

    /* Capture possible error bit from Tempe VEAT register */

    veatReg = TEMPE_READ32(TEMPE_REG_BASE, TEMPE_VEAT);

    /* safe to restore original interrupt state */

    intUnlock (oldLevel);

    if ((veatReg & TEMPE_VEAT_VES_MASK) != 0)
	stat = ERROR;

    return (stat);
    }

/******************************************************************************
*
* sysBusRmwEnable - enable Read-Modify-Write (RMW) cycle on VMEbus
*
* RETURNS: N/A
*/

void sysBusRmwEnable
    (
    UINT32 swapCompareEnable,	/* Enabled bits involved in compare and swap */
    UINT32 compareData,		/* Data to compare with read portion of RMW */
    UINT32 swapData,		/* Data to write during write portion of RMW */
    char * rmwAddress		/* RMW PCI address */
    )
    {
    UINT32 ctlReg;

    /* Set up the compare and swap registers */

    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_RMWEN, swapCompareEnable);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_RMWC, compareData);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_RMWS, swapData);

    /* Program the address */

    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_RMWAU, 0)
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_RMWAL, (UINT32)rmwAddress)

    /* Set the control register bit to enable RMW cycle */

    ctlReg = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_VMCTRL);
    ctlReg |= TEMPE_VMCTRL_RMWEN_MASK;
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_VMCTRL, ctlReg);

    }

/******************************************************************************
*
* sysBusRmwDisable - Disable Read-Modify-Write (RMW) cycle on the VMEbus.
*
* Disable Read-Modify-Write (RMW) cycle on the VMEbus.
*
* RETURNS: N/A
*/

void sysBusRmwDisable (void)
    {
    UINT32 ctlReg;

    /* Set the control register bit to disable RMW cycle */

    ctlReg = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_VMCTRL);
    ctlReg &= ~(TEMPE_VMCTRL_RMWEN_MASK);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_VMCTRL, ctlReg);

    }
