/* vme2Pci.c - VME2PCI device configuration library */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01f,03jan97,wlf  doc: cleanup.
01e,13nov96,mas  int levels read in IO space instead of config space (SPR 7475).
01d,04mar96,tpr  reworked interrupt handling.
01c,23feb96,tpr  added intVecNum initialization in vme2PciIntr().
01b,20jan95,vin	 enhancements and clean up.
01a,09jan95,kvk  created.
*/

/*
DESCRIPTION
This library provides routines which configure the VME2PCI chip for a
device which is located on the Peripheral Component Interconnect
(PCI) bus. This library has been designed with respect to the Genesis
1.5 Engineering Specification Version 1.0A of Motorola
MVME1600/PM603-604 preliminary engineering specification guide.  The
macro VME2PCI_BASE_ADRS has to defined before using this library.

INCLUDE FILES: vme2Pci.h
*/

/* include files */

#include "drv/intrCtl/i82378Ibc.h"
#include "drv/vme/vme2Pci.h"

/* extern declarations */

IMPORT INT_HANDLER_DESC * sysIntTbl [256];

/* locals */

/* ptr to regs in PCI IO space */

LOCAL ULONG 	vme2PciRegBase = VME2PCI_BASE_ADRS + VME2PCI_IO_OFFSET;


/*******************************************************************************
*
* sysVme2PciInit - initialize a VME2PCI chip 
*
* This routine initializes additional chip-specific features of a VME2PCI
* chip, such as the PCI slave addresses.  It should be called only at
* initialization time.
*
* RETURNS: OK, always.
*/

STATUS sysVme2PciInit 
    ( 
    VME2PCI_CNFG_HDR *	pVme2PciCnfgHdr,	/* ptr to VME2PCI config hdr */
    ULONG 		vme2PciCnfgBase		/* ptr to VME2PCI reg base */
    )
    {
    int 	ix;
    ULONG *	cnfgPtr;
    ULONG *	pVme2PciRegBase;

    pVme2PciRegBase = (ULONG *)(vme2PciCnfgBase + VME2PCI_CNFG_OFFSET);

    /* Program the PCI command register */
    
    ix = sizeof (VME2PCI_CNFG_HDR) / sizeof (long);

    for (cnfgPtr = (ULONG *)(pVme2PciCnfgHdr); ix > 0; cnfgPtr++, ix--)
#if (_BYTE_ORDER == _BIG_ENDIAN)
	*pVme2PciRegBase++ = LONGSWAP(*cnfgPtr);
#else
	*pVme2PciRegBase++ = *cnfgPtr;
#endif /* _BYTE_ORDER == _BIG_ENDIAN */    
    
    return (OK);
    }

/*******************************************************************************
*
* vme2PciIntr - handle a VMEbus interrupt
*
* This routine gets called to handle VMEbus interrupts.
*
* NOMANUAL
*/

void vme2PciIntr (void)
    {
    UCHAR 		intVecNum = 0;
    int 		vmeIntLvl;
    INT_HANDLER_DESC *	currHandler;


#if (_BYTE_ORDER == _BIG_ENDIAN)
    vmeIntLvl = VME2PCI_INT_LVL_GET(LONGSWAP(((VME2PCI_CNFG_HDR *)
				     (vme2PciRegBase))->pciIntEnable));
#else
    vmeIntLvl = VME2PCI_INT_LVL_GET(((VME2PCI_CNFG_HDR *)
				     (vme2PciRegBase))->pciIntEnable);
#endif /* _BYTE_ORDER == _BIG_ENDIAN */

    switch (vmeIntLvl)
	{
	case 1:

	/* get the vector number by reading IACK registers in VME2PCI chip */

	    intVecNum = *(VME2PCI_IACK_LEVEL1);
	    break;
	case 2:
	    intVecNum = *(VME2PCI_IACK_LEVEL2);
	    break;
	case 3:
	    intVecNum = *(VME2PCI_IACK_LEVEL3);
	    break;
	case 4:
	    intVecNum = *(VME2PCI_IACK_LEVEL4);
	    break;
	case 5:
	    intVecNum = *(VME2PCI_IACK_LEVEL5);
	    break;
	case 6:
	    intVecNum = *(VME2PCI_IACK_LEVEL6);
	    break;
	case 7:
	    intVecNum = *(VME2PCI_IACK_LEVEL7);
	    break;
	}

    /* call the respective interrupt handler(s) */
    
    if ((currHandler = sysIntTbl [intVecNum]) == NULL)
	logMsg ("uninitialized vme interrupt %d\n", intVecNum, 0,0,0,0,0);
    else
	{
	while (currHandler != NULL)
	    {
	    currHandler->vec (currHandler->arg);
	    currHandler = currHandler->next;
	    }
	}
    }

/*******************************************************************************
*
* vme2PciSlaveAddrSet - set address-related PCI features
*
* This routine sets the Peripheral Component Interconnect (PCI) slave start
* address, the PCI slave end address, and the address offset.
*
* RETURNS: OK, always.
*/

STATUS vme2PciSlaveAddrSet
    (
    VME2PCI_CNFG_HDR *	pVme2PciCnfgHdr, 	/* ptr to VME2PCI config hdr */
    ULONG *		pVme2PciRegBase,	/* ptr to VME2PCI reg base */
    ULONG		startAddress,		/* slave starting address */
    ULONG		endAddress,		/* ending address */
    ULONG 		addrOffset,		/* slave offset */
    int 		slaveNo	 		/* slave number */
    )
    {
    int 	ix;
    ULONG *	iy;
    
    switch(slaveNo)
	{
	case 0:
	
	    pVme2PciCnfgHdr->pciSlvStrtAddr1 = MSW(startAddress);
	    pVme2PciCnfgHdr->pciSlvEndAddr1 = MSW(endAddress);
	    pVme2PciCnfgHdr->pciSlvAddrOffset1 = MSW(addrOffset);
	    pVme2PciCnfgHdr->pciSlvEnable1 = VME2PCI_SLAVE_ENABLE;
	    break;

	case 1:

	    pVme2PciCnfgHdr->pciSlvStrtAddr2 = MSW(startAddress);
	    pVme2PciCnfgHdr->pciSlvEndAddr2 = MSW(endAddress);
	    pVme2PciCnfgHdr->pciSlvAddrOffset2 = MSW(addrOffset);
	    pVme2PciCnfgHdr->pciSlvEnable2 = VME2PCI_SLAVE_ENABLE;
	    break;
	
	default:				
	    break;				
	}				

    ix = sizeof (VME2PCI_CNFG_HDR) / sizeof (long);

    for (iy = (ULONG *)(pVme2PciCnfgHdr); ix > 0; iy++, ix--)
#if (_BYTE_ORDER == _BIG_ENDIAN)
	*pVme2PciRegBase++ = LONGSWAP(*iy);
#else
	*pVme2PciRegBase++ = *iy;
#endif /* _BYTE_ORDER == _BIG_ENDIAN */

    return (OK);
    }


