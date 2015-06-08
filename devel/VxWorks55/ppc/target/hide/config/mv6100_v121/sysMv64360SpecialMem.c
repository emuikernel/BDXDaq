
/* Copyright 2004 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01a,05may04,scb Initial writing, used MVME5500 as example.
*/

/*
This file contains the sysMv64360SpecialMem() function which will
reconfigure a piece of memory with characteristics which the user can
select.  Discovery II PCI access control registers can be affected as
well as cacheing state of the memory.  Specifically, the modifiable
attributes are:

.CS
  1) PCI bus designator (0 or 1) depending on which of the primary
       GT64360 PCI busses apply.
  2) Register set to modify (0 thru 5).
  3) Enabled/disabled state.
  4) Size of memory to modify, must be power of 2.
  5) Base address of memory to modify, must be on 'size' boundary.
  6) Cacheing attribute: (Copy (Write) Back, Write Through, No Cache).
  7) PCI snoopable (snoopable to Write thru, Write Back or not snoopable).
  8) PCI mburst attribute (32-byte, 64-byte, or 128-byte).
  9) PCI read transaction size (32-byte, 64-byte, 128-byte, 256-byte).
.CE

On an MVME6100, the following call will configure DRAM between
addresses 0x2000000 and 0x3000000 to be as fast as possible with VME
DMA transfers (using the Tsi148 Tempe bridge DMA engine).  The sending
and receiving DRAM should be configured in a similar manner to achieve
the fastest possible VME DMA transfer.

.CS

#include "mv64360.h"
void fastVmeDMA()
    {
    SPECIAL_ATTR specialAttr;
    UINT32 pciBusNo;
    UINT32 regSet;
    UINT32 baseAddr;
    UINT32 size;

    pciBusNo = 0;
    regSet   = 0;
    baseAddr = 0x20000000;
    size     = 0x10000000;
    specialAttr.enabled = TRUE;
    specialAttr.snoopAttr = snoopNo;
    specialAttr.mburstVal = mburst_128;
    specialAttr.rdSize = rdSize_256;
    specialAttr.cacheable = cacheIgnore;
    (void)sysMv64360SpecialMem(pciBusNo,regSet,baseAddr,size,&specialAttr);
    }

.CE

*/

/* includes */

#include "vmLib.h"
#include "cacheLib.h"
#include "mv64360.h"
#include "config.h"

/* defines */

/* structures */

/* globals */

/* locals */

/* forward declarations */

/* externals */

char *  sysPhysMemTop (void);
void    sysOutLong (ULONG, ULONG);
ULONG   sysInLong (ULONG);

struct _pciAccessRegSet
    {
    UINT32 pciAccessCtrlBaseXLo;
    UINT32 pciAccessCtrlBaseXHi;
    UINT32 pciAccessCtrlSize;
    } pciAccessRegSet [2] [6] =
        {
            {
                {
                PCIIF_ACCESS_CTRL_BASE0_LOW_PCI0,
                PCIIF_ACCESS_CTRL_BASE0_HI_PCI0,
                PCIIF_ACCESS_CTRL_SIZE0_PCI0
                },

                {
                PCIIF_ACCESS_CTRL_BASE1_LOW_PCI0,
                PCIIF_ACCESS_CTRL_BASE1_HI_PCI0,
                PCIIF_ACCESS_CTRL_SIZE1_PCI0
                },
		 
		{
                PCIIF_ACCESS_CTRL_BASE2_LOW_PCI0,
                PCIIF_ACCESS_CTRL_BASE2_HI_PCI0,
                PCIIF_ACCESS_CTRL_SIZE2_PCI0
                },

		{
                PCIIF_ACCESS_CTRL_BASE3_LOW_PCI0,
                PCIIF_ACCESS_CTRL_BASE3_HI_PCI0,
                PCIIF_ACCESS_CTRL_SIZE3_PCI0
                },

		{
                PCIIF_ACCESS_CTRL_BASE4_LOW_PCI0,
                PCIIF_ACCESS_CTRL_BASE4_HI_PCI0,
                PCIIF_ACCESS_CTRL_SIZE4_PCI0
                },

		{
                PCIIF_ACCESS_CTRL_BASE5_LOW_PCI0,
                PCIIF_ACCESS_CTRL_BASE5_HI_PCI0,
                PCIIF_ACCESS_CTRL_SIZE5_PCI0
                },
            },

            {
                {
                PCIIF_ACCESS_CTRL_BASE0_LOW_PCI1,
                PCIIF_ACCESS_CTRL_BASE0_HI_PCI1,
                PCIIF_ACCESS_CTRL_SIZE0_PCI1
                },

                {
                PCIIF_ACCESS_CTRL_BASE1_LOW_PCI1,
                PCIIF_ACCESS_CTRL_BASE1_HI_PCI1,
                PCIIF_ACCESS_CTRL_SIZE1_PCI1
                },
		 
		{
                PCIIF_ACCESS_CTRL_BASE2_LOW_PCI1,
                PCIIF_ACCESS_CTRL_BASE2_HI_PCI1,
                PCIIF_ACCESS_CTRL_SIZE2_PCI1
                },

		{
                PCIIF_ACCESS_CTRL_BASE3_LOW_PCI1,
                PCIIF_ACCESS_CTRL_BASE3_HI_PCI1,
                PCIIF_ACCESS_CTRL_SIZE3_PCI1
                },

		{
                PCIIF_ACCESS_CTRL_BASE4_LOW_PCI1,
                PCIIF_ACCESS_CTRL_BASE4_HI_PCI1,
                PCIIF_ACCESS_CTRL_SIZE4_PCI1
                },

		{
                PCIIF_ACCESS_CTRL_BASE5_LOW_PCI1,
                PCIIF_ACCESS_CTRL_BASE5_HI_PCI1,
                PCIIF_ACCESS_CTRL_SIZE5_PCI1
                } 
            } 
        };

/*****************************************************************************
*
* sysMv64360SpecialMem - Configure a block of memory in a special way
*
* This function takes a memory  address range and attribute parameters and
* configures the memory according to the attributes.  The MVME64360 PCI
* access control registers are modified with this call as well as the
* cacheing attribute of the memory.
*
* RETURNS: OK if parameters are sane, ERROR otherwise.
*/

STATUS sysMv64360SpecialMem
    (
    UINT32 	  pciBusNo,	/* PCI bus number 0 or 1 */
    UINT32	  regSet,	/* Access control register set (0-5) */
    UINT32	  baseAddr,	/* Base address, must be 4K (0x1000) aligned */
    UINT32	  size,		/* Size in bytes, must be power of 2 */
    SPECIAL_ATTR *specialAttr	/* "special" attribute */
    )
    {
    UINT32 baseXLo;
    UINT32 baseXHi;
    UINT32 sizeX;
    UINT32 baseXLoVal;
    UINT32 baseXHiVal;
    UINT32 sizeXVal;
    UINT32 tryBaseXLo;
    UINT32 tryBaseXHi;
    UINT32 trySizeX;
    UINT32 tryBaseXLoVal;
    UINT32 tryBaseXHiVal;
    UINT32 trySizeXVal;
    UINT32 existingLo = 0;
    UINT32 existingHi = 0;
    BOOL   overlap;
    int    key;
    int	   i;

    /* Check for parameter validity and consistency */

    if (pciBusNo > 1)
	{
	printf("sysMv64360SpecialMem: "
	       "pciBusNo (param 1) must be 0 or 1\n");
	return (ERROR);
	}

    if (regSet > 5)
	{
	printf("sysMv64360SpecialMem: regSet (param 2) "
	       "must be 0,1,2,3,4, or 5\n");
	return (ERROR);
	}

    /*
     * We cannot check the "brain dead" case of just disabling the access
     * control register.
     */

    if (specialAttr->enabled == FALSE)
	{
        baseXLo = pciAccessRegSet[pciBusNo][regSet].pciAccessCtrlBaseXLo; 
	baseXLoVal = MV64360_READ32 (MV64360_REG_BASE, baseXLo);
	baseXLoVal &= ~1;
	MV64360_WRITE32_PUSH (MV64360_REG_BASE, baseXLo, baseXLoVal);
	return (OK);		/* Nothing more to do */
	}

    if ((baseAddr & 0xfff) != 0)
	{
	printf("sysMv64360SpecialMem: baseAddr (param 3) "
	       "must be multiple of 0x1000\n");
	return (ERROR);
	}

    if (size < 0x1000)
	{
	printf("sysMv64360SpecialMem: "
	       "size (param 4) must be >= 0x1000\n");
	return(ERROR);
	}

    if ((size & (size - 1)) != 0)
	{
	printf("sysMv64360SpecialMem: "
	       "size (param 4) must be power of 2\n");
	return (ERROR);
	}

    if ((baseAddr % size) != 0)
	{
	printf("sysMv64360SpecialMem: baseAddr (param 3) must be "
	       "multiple of size (param 4)\n");
        return (ERROR);
	}

    /* Load up register offsets which we will affect */

    baseXLo = pciAccessRegSet[pciBusNo][regSet].pciAccessCtrlBaseXLo; 
    baseXHi = pciAccessRegSet[pciBusNo][regSet].pciAccessCtrlBaseXHi; 
    sizeX = pciAccessRegSet[pciBusNo][regSet].pciAccessCtrlSize;

    /* Check for window overlap */

    overlap = FALSE;

    for (i=0; i<=5; i++)
	{
	if (i == regSet)
	    continue;		/* Ignore "my" register set */

        tryBaseXLo = pciAccessRegSet[pciBusNo][i].pciAccessCtrlBaseXLo; 
        tryBaseXHi = pciAccessRegSet[pciBusNo][i].pciAccessCtrlBaseXHi; 
        trySizeX = pciAccessRegSet[pciBusNo][i].pciAccessCtrlSize;

	tryBaseXLoVal = MV64360_READ32 (MV64360_REG_BASE, tryBaseXLo);
	tryBaseXHiVal = MV64360_READ32 (MV64360_REG_BASE, tryBaseXHi);
	trySizeXVal = MV64360_READ32 (MV64360_REG_BASE, trySizeX);

	if ((tryBaseXLoVal & 1) == 0)
	    continue;		/* Ignore disabled access control */

	existingLo = MV64360_PCI_LO_ADDR(tryBaseXLoVal);
	existingHi = MV64360_PCI_HI_ADDR(tryBaseXLoVal, trySizeXVal);

	if (baseAddr == existingLo)
	    {
	    overlap = TRUE;
	    break;
	    }
	if (baseAddr < existingLo)
	    if ((baseAddr + size) > existingLo)
		{
		overlap = TRUE;
		break;
		}

	if (existingHi == (baseAddr + size))
	    {
	    overlap = TRUE;
	    break;
	    }
	if ((baseAddr + size) > existingHi)
	    if (baseAddr < existingHi)
		{
		overlap = TRUE;
		break;
		}
	}

    if (overlap == TRUE)
	{
	printf("sysMv64360SpecialMemspecialMem: "
	       "Overlap with window reg set %d, 0x%08x - 0x%08x\n",
	       i, existingLo, existingHi);
	return (ERROR);
	}

    /* 
     * Finally we have verified that the parameters passed in are "sane"
     * Now deal with the "cacheable" attribute
     */

    key = intLock();
    switch (specialAttr->cacheable)
        {

	case cacheCopyBack:
            cacheFlush (DATA_CACHE, (void *)LOCAL_MEM_LOCAL_ADRS, 
			(UINT32)sysPhysMemTop());
            vmBaseStateSet (0, (void *)baseAddr, size, 
                            VM_STATE_MASK_CACHEABLE, VM_STATE_CACHEABLE);
            cacheInvalidate (DATA_CACHE, (void *)baseAddr, size);
	    break;

	case cacheWriteThru:
            cacheFlush (DATA_CACHE, (void *)LOCAL_MEM_LOCAL_ADRS, 
			(UINT32)sysPhysMemTop());
            vmBaseStateSet (0, (void *)baseAddr, size, 
                            VM_STATE_MASK_CACHEABLE, 
			    VM_STATE_CACHEABLE_WRITETHROUGH);
            cacheInvalidate (DATA_CACHE, (void *)baseAddr, size);
	    break;

        case cacheNot:
            cacheFlush (DATA_CACHE, (void *)LOCAL_MEM_LOCAL_ADRS, 
			(UINT32)sysPhysMemTop());
            vmBaseStateSet (0, (void *)baseAddr, size, 
                            VM_STATE_MASK_CACHEABLE, VM_STATE_CACHEABLE_NOT);
	    cacheInvalidate (DATA_CACHE, (void *)baseAddr, size);
	    break;
        case cacheIgnore:
	    break;
        }
    intUnlock(key);
   
    /*
     * Carve out the region of memory with regard to the PCI access
     * control registers - We will assume that the pci Access control
     * BaseX HI register is always zero.
     */

    baseXHiVal = 0;
    sizeXVal = MV64360_PCI_SIZE_ENCODE(size);

    /*
     * The PCI access control register low requires some building.
     * Let's start with the base address.  Note that we have long ago
     * taken care of the "enabled" check - so we can assume that the
     * window is to be enabled here.  We will start with the base
     * address (bits 31:12) and "enabled bit" (bit 0) and proceed in
     * numerical order to the bits in the range 1 -> 11.  Note that
     * many are forced (we don't allow changes) because the "forced"
     * value is the only one that makes sense on the MVME6100.
     */

    baseXLoVal = (baseAddr | 1);	/* Or in "enabled" bit, zero others */

    /* Note that Force REQ64 (bit 1) is left as zero - Disabled */
    
    /* Handle "Snoop" (Cache Coherency support) */

    baseXLoVal |= (specialAttr->snoopAttr << 2);

    /*
     * The following bits are not changeable by this routine:
     *   Bit 4 = 0: AccProt:     PCI access to this region is allowed.
     *   Bit 5 = 0: WrProt:      PCI write access to this region is allowed.
     *   Bits 7:6 = 01: PCISwap: No PCI swapping
     */

    baseXLoVal |= (1 << 6);	/* PCI Swap value */

    /* Wrap up with MBurst (bits 9:8) and RdSize (bits 11:10) */

    baseXLoVal |= (specialAttr->mburstVal << 8);
    baseXLoVal |= (specialAttr->rdSize << 10);

    /* We're ready, let's Program the registers */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, baseXHi, baseXHiVal);
    MV64360_WRITE32_PUSH (MV64360_REG_BASE, baseXLo, baseXLoVal);
    MV64360_WRITE32_PUSH (MV64360_REG_BASE, sizeX, sizeXVal);

    return (OK);
    }

#ifdef INCLUDE_SHOW_ROUTINES
/*****************************************************************************
* 
* pciAccessCtlShow - Show MVME64360 PCI Access Control Register settings
*
* Show the PCI access control register settings (High, Low, Size) for
* both PCI busses, 6 sets per bus.  Break out attributes and display
* these in formatted output.
*
* RETURNS: NA
*/

void pciAccessCtlShow (void)
    {
    UINT32 baseXLo;
    UINT32 baseXHi;
    UINT32 sizeX;
    UINT32 baseXLoVal;
    UINT32 baseXHiVal;
    UINT32 sizeXVal;
    int pciBusNo;
    int regSet;

    for (pciBusNo = 0; pciBusNo <= 1; pciBusNo++)
	{
	for (regSet = 0; regSet <= 5 ; regSet++)
	    {

	    printf("\nPCI bus %d Base %d set: ",pciBusNo,regSet);

    	    baseXLo = pciAccessRegSet[pciBusNo][regSet].pciAccessCtrlBaseXLo; 
            baseXHi = pciAccessRegSet[pciBusNo][regSet].pciAccessCtrlBaseXHi; 
            sizeX = pciAccessRegSet[pciBusNo][regSet].pciAccessCtrlSize;

	    baseXLoVal = MV64360_READ32 (MV64360_REG_BASE, baseXLo);
	    baseXHiVal = MV64360_READ32 (MV64360_REG_BASE, baseXHi);
	    sizeXVal = MV64360_READ32 (MV64360_REG_BASE, sizeX);

	    if ((baseXLoVal & 1) == 0)
		{
		printf("DISABLED\n");
		continue;
		}
            else
		{
		printf("ENABLED ");
		}

	    printf("PCI Address range: 0x%08x thru 0x%08x\n",
		   MV64360_PCI_LO_ADDR(baseXLoVal),
		   MV64360_PCI_HI_ADDR(MV64360_PCI_LO_ADDR(baseXLoVal),
				       sizeXVal));
	    printf("Size @0x%08x = 0x%08x\n", 
		   (sizeX + MV64360_REG_BASE), sizeXVal);
	    printf("High @0x%08x = 0x%08x\n", (baseXHi + MV64360_REG_BASE),
		    baseXHiVal);
	    printf("Low  @0x%08x = 0x%08x\n", (baseXLo + MV64360_REG_BASE),
		    baseXLoVal);

            if ((baseXLoVal & (1 << 1)) != 0)
		printf("  Req64:   (bit 1 = 1)      Enabled\n");
            else
		printf("  Req64:   (bit 1 = 0)      Disabled\n");

	    switch ((baseXLoVal >> 2) & (0x3))
		{
		case 0: 
		    printf("  Snoop:   (bits 3:2 = 0)   Non cache coherent\n");
		    break;
		case 1: 
		    printf("  Snoop:   (bits 3:2 = 1)   Cache coherent WT\n");
		    break;
		case 2: 
		    printf("  Snoop:   (bits 3:2 = 2)   Cache coherent WB\n");
		    break;
		}

            if ((baseXLoVal & (1 << 4)) != 0)
		printf("  AccProt: (bit 4 = 1)      PCI access NOT allowed\n");
            else
		printf("  AccProt: (bit 4 = 0)      PCI access allowed\n");

            if ((baseXLoVal & (1 << 5)) != 0)
		printf("  WrProt:  (bit 5 = 1)  "
		       "    PCI write access NOT allowed\n");
            else
		printf("  WrProt:  (bit 5 = 0)  "
		       "    PCI write access allowed\n");

	    switch ((baseXLoVal >> 6) & (0x3))
		{
		case 0: 
		    printf("  PCISwap: (bits 7:6 = 0)   Byte Swap\n");
		    break;
		case 1: 
		    printf("  PCISwap: (bits 7:6 = 1)   No Swap\n");
		    break;
		case 2: 
		    printf("  PCISwap: (bits 7:6 = 2)   Byte & Word Swap\n");
		    break;
		}

	    switch ((baseXLoVal >> 8) & (0x3))
		{
		case 0: 
		    printf("  MBurst:  (bits 9:8 = 0)   32 Bytes\n");
		    break;
		case 1: 
		    printf("  MBurst:  (bits 9:8 = 1)   64 Bytes\n");
		    break;
		case 2: 
		    printf("  MBurst:  (bits 9:8 = 2)   128 Bytes\n");
		    break;
		}

	    switch ((baseXLoVal >> 10) & (0x3))
		{
		case 0: 
		    printf("  RdSize:  (bits 11:10 = 0) 32 Bytes\n");
		    break;
		case 1: 
		    printf("  RdSize:  (bits 11:10 = 1) 64 Bytes\n");
		    break;
		case 2: 
		    printf("  RdSize:  (bits 11:10 = 2) 128 Bytes\n");
		    break;
		case 3: 
		    printf("  RdSize:  (bits 11:10 = 3) 256 Bytes\n");
		    break;
		}
	    }
	}
    }
#endif
