/* mpc107Pci.c - MPC107 PCI bus support */

/* Copyright 1996-2000 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,11sep00,rcs fix includes
01a,06jun00,bri written
*/

/*
DESCRIPTION
This module contains routines to detect whether the processor uses
Address Map A or Address Map B and use the corresponding PCI address
map depending on the the address map detected for PCI configuations.

.SH INITIALIZATION
mpc107MemMapDetect() should be called before any other PCI configuration
functions are called.
*/


/* includes */

#include "vxWorks.h"
#include "sysLib.h"
#include "drv/pci/pciConfigLib.h"
#include "drv/pci/pciIntLib.h"
#include "drv/pci/pciAutoConfigLib.h"
#include "drv/multi/mpc107.h"
#include "drv/pci/mpc107Pci.h"

/* static file scope locals */


/* static locals which define the PREP or CHRP mapping */

LOCAL ULONG 	mpc107PciConfAddr;	 /* PCI Configuration Address */
LOCAL ULONG 	mpc107PciConfData;	 /* PCI Configuration Data */
LOCAL ULONG 	mpc107PciMstrCfgAdrs;	 /* CPU's PCI Config Address */
LOCAL ULONG 	mpc107PciMstrCfgSize; 	 /* CPU's PCI Config Size */
LOCAL ULONG 	mpc107PciMstrIoLocal;	 /* CPU's PCI IO Address */
LOCAL ULONG 	mpc107PciMstrIoSize;	 /* CPU's PCI I/O Size */
LOCAL ULONG 	mpc107PciMstrIoBus;	 /* PCI view of I/O */
LOCAL ULONG 	mpc107PciMstrIackLocal;	 /* CPU's PCI IACK Address */
LOCAL ULONG 	mpc107PciMstrIackSize;	 /* CPU's PCI IACK Address */
LOCAL ULONG 	mpc107PciMstrMemIoLocal; /* CPU's PCI Memory Address */
LOCAL ULONG 	mpc107PciSlvMemBus;	 /* PCI view of system memory */
LOCAL ULONG 	mpc107PciMstrMemIoBus;	 /* PCI view of PCI */
LOCAL ULONG 	mpc107PciMemorySize;     /* Memory Size */

/* forward Declarations */

LOCAL ULONG mpc107IndirectRegRead (ULONG regNum);

/***************************************************************************
*
* mpc107MemMapDetect - detect PCI address map A or B
*
* This procedure detects the address map (A or B)  and
* assigns proper values to some global variables.  First it assumes
* that the memory map is of Map B, then reads the Vendor and Device
* ids.  If the value does not match with what is expected, all global
* variables are then set for Map A.
*
* The following global variables are set to reflect the PCI address map:
* .CS
*    mpc107PciConfAddr
*    mpc107PciConfData
*    mpc107PciMstrCfgAdrs
*    mpc107PciMstrCfgSize
*    mpc107PciMstrIoLocal
*    mpc107PciMstrIoSize
*    mpc107PciMstrIoBus
*    mpc107PciMstrIackLocal
*    mpc107PciMstrIackSize
*    mpc107PciMstrMemIoLocal
*    mpc107PciSlvMemBus
*    mpc107PciMstrMemIoBus
*    mpc107PciMemorySize
* .CE
*
* RETURNS: N/A
*/

void mpc107MemMapDetect(void)
    {
    ULONG retVal;

    /*
     * Read the Device & Vendor Id Register of PCI to  determine whether
     * whether MAP A(PREP) or MAP B(CHRP) is to be used .
     * We assume that the memory map is Map B  and attempt to read the MPC107's
     * PCI Vendor and Device IDs. If we read it ok, then use map B, else assume
     * map A.
     */

    mpc107PciConfAddr           = MPC107_CFG_ADDR_CHRP;
    mpc107PciConfData           = MPC107_CFG_DATA_CHRP;

    retVal = mpc107IndirectRegRead(MPC107_CFG_BASE);

    /* Read Device & Vendor Ids */

    if ((retVal == MPC107_DEV_VEN_ID ))	/* True if it is address Map B */
        {

        /* If it is address Map B*/

        mpc107PciConfAddr        = MPC107_CFG_ADDR_CHRP;
        mpc107PciConfData        = MPC107_CFG_DATA_CHRP;
        mpc107PciMstrCfgAdrs	 = MPC107_PCI_MSTR_CFG_ADRS_B;
        mpc107PciMstrCfgSize	 = MPC107_PCI_MSTR_CFG_SIZE_B;
        mpc107PciMstrIoLocal	 = MPC107_PCI_MSTR_IO_LOCAL_B;
        mpc107PciMstrIoSize    	 = MPC107_PCI_MSTR_IO_SIZE_B;
        mpc107PciMstrIoBus	 = MPC107_PCI_MSTR_IO_BUS_B;
        mpc107PciMstrIackLocal	 = MPC107_PCI_MSTR_IACK_LOCAL_B;
        mpc107PciMstrIackSize	 = MPC107_PCI_MSTR_IACK_SIZE_B;
        mpc107PciMstrMemIoLocal	 = MPC107_PCI_MSTR_MEMIO_LOCAL_B;
        mpc107PciSlvMemBus	 = MPC107_PCI_SLV_MEM_BUS_B;
        mpc107PciMstrMemIoBus	 = MPC107_PCI_MSTR_MEMIO_BUS_B;
        mpc107PciMemorySize	 = MPC107_PCI_MSTR_MEMIO_SIZE_B;

        return ;
        }

    /* If it is Map -A */

    mpc107PciConfAddr        = MPC107_CFG_ADDR_PREP;
    mpc107PciConfData 	     = MPC107_CFG_DATA_PREP;
    mpc107PciMstrCfgAdrs     = MPC107_PCI_MSTR_CFG_ADRS_A;
    mpc107PciMstrCfgSize     = MPC107_PCI_MSTR_CFG_SIZE_A;
    mpc107PciMstrIoLocal     = MPC107_PCI_MSTR_IO_LOCAL_A;
    mpc107PciMstrIoSize      = MPC107_PCI_MSTR_IO_SIZE_A;
    mpc107PciMstrIoBus	     = MPC107_PCI_MSTR_IO_BUS_A;
    mpc107PciMstrIackLocal   = MPC107_PCI_MSTR_IACK_LOCAL_A;
    mpc107PciMstrIackSize    = MPC107_PCI_MSTR_IACK_SIZE_A;
    mpc107PciMstrMemIoLocal  = MPC107_PCI_MSTR_MEMIO_LOCAL_A;
    mpc107PciSlvMemBus       = MPC107_PCI_SLV_MEM_BUS_A;
    mpc107PciMstrMemIoBus    = MPC107_PCI_MSTR_MEMIO_BUS_A;
    mpc107PciMemorySize	     = MPC107_PCI_MSTR_MEMIO_SIZE_A;


    }


/***************************************************************************
*
* mpc107IndirectRegRead - read a 32-bit address MPC107 register
*
* Read a 32-bit address MPC107 register and returns
* a 32 bit value.  Swap the address to little endian before
* writing it to config address since it is PCI, and swap the
* value to big endian before returning to the caller.
*
* RETURNS: The contents of the specified MPC107 register.
*/

LOCAL ULONG mpc107IndirectRegRead
    (
    ULONG regNum  /* register number */
    )
    {
    ULONG temp;

    /* swap the value to little endian */

    *(ULONG *) mpc107PciConfAddr =  MPC107LONGSWAP(regNum);

    SYNC;

    temp = *(ULONG *) mpc107PciConfData;

    SYNC;

    /* swap the data & return */

    return ( MPC107LONGSWAP (temp));
    }

