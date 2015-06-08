/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                 *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
******************************************************************************/

#ifdef PMC280_DUAL_CPU
#include "vxWorks.h"
#include "sysLib.h"
#include "config.h"
#include "errno.h"
#include "sioLib.h"
#include "arch/ppc/mmu603Lib.h"

#ifdef PMC280_DUAL_CPU_PTE_SETUP
#include "arch/ppc/mmuPpcLib.h"
#include "vmLib.h"
#include "private/vmLibP.h"
#include "gtCore.h"
#endif /* PMC280_DUAL_CPU_PTE_SETUP */

#include "pmc280.h"
#include "dbgprints.h"
#include "stdio.h"
#include "string.h "


/*#include "dbg_vsprintf.c"*/
#include "dbgbats.h"
#include "frcWrsRoutines.h"

#include "cacheLib.h"

extern unsigned int frcWhoAmI(void);
extern void frcPermitCPU1ToBoot(void);
unsigned int data = 0;
void dbg_printregval(unsigned int, unsigned int);

#ifdef PMC280_DUAL_CPU_PTE_SETUP
/*
 * These defines are given only to identify from which files in the Windriver 
 * source these code sections moved here have come from WRS code.
 */
#define CODE_FROM_MMUPPCLIB_C
#define CODE_FROM_MMUPPCALIB_S
#define CODE_FROM_SYSLIB_C
#define CODE_FROM_VMBASELIB_C
#define CODE_FOR_CPU_0
#define CODE_FOR_CPU_1

#define FRCNELEMENTS(array) /* number of elements in an array */ \
                (sizeof (array) / sizeof ((array) [0]))

#ifdef CODE_FROM_MMUPPCLIB_C
/* typedefs */
typedef struct frchashTabInfo
    {
    unsigned int        hashTabOrgMask;
    unsigned int        hastTabMask;
    unsigned int        pteTabMinSize;
    } HASH_TAB_INFO ;

MMU_TRANS_TBL     frcmmuGlobalTransTbl0;      /* global translation table */
MMU_TRANS_TBL     frcmmuGlobalTransTbl1;      /* global translation table */
HASH_TAB_INFO frcmmuPpcHashTabInfo[] =
    {
    {MMU_SDR1_HTABORG_8M,       MMU_SDR1_HTABMASK_8M,   MMU_PTE_MIN_SIZE_8M},
    {MMU_SDR1_HTABORG_16M,      MMU_SDR1_HTABMASK_16M,  MMU_PTE_MIN_SIZE_16M},
    {MMU_SDR1_HTABORG_32M,      MMU_SDR1_HTABMASK_32M,  MMU_PTE_MIN_SIZE_32M},
    {MMU_SDR1_HTABORG_64M,      MMU_SDR1_HTABMASK_64M,  MMU_PTE_MIN_SIZE_64M},
    {MMU_SDR1_HTABORG_128M,     MMU_SDR1_HTABMASK_128M, MMU_PTE_MIN_SIZE_128M},
    {MMU_SDR1_HTABORG_256M,     MMU_SDR1_HTABMASK_256M, MMU_PTE_MIN_SIZE_256M},
    {MMU_SDR1_HTABORG_512M,     MMU_SDR1_HTABMASK_512M, MMU_PTE_MIN_SIZE_512M},
    {MMU_SDR1_HTABORG_1G,       MMU_SDR1_HTABMASK_1G,   MMU_PTE_MIN_SIZE_1G},
    {MMU_SDR1_HTABORG_2G,       MMU_SDR1_HTABMASK_2G,   MMU_PTE_MIN_SIZE_2G},
    {MMU_SDR1_HTABORG_4G,       MMU_SDR1_HTABMASK_4G,   MMU_PTE_MIN_SIZE_4G},
    };

int frcmmuPpcHashTabInfoNumEnt = FRCNELEMENTS (frcmmuPpcHashTabInfo);

STATE_TRANS_TUPLE frcmmuStateTransArrayLocal [] =
{
    {VM_STATE_MASK_VALID, MMU_STATE_MASK_VALID,
     VM_STATE_VALID, MMU_STATE_VALID},

    {VM_STATE_MASK_VALID, MMU_STATE_MASK_VALID,
     VM_STATE_VALID_NOT, MMU_STATE_VALID_NOT},

    {VM_STATE_MASK_WRITABLE, MMU_STATE_MASK_WRITABLE,
     VM_STATE_WRITABLE, MMU_STATE_WRITABLE},

    {VM_STATE_MASK_WRITABLE, MMU_STATE_MASK_WRITABLE,
     VM_STATE_WRITABLE_NOT, MMU_STATE_WRITABLE_NOT},

    {VM_STATE_MASK_CACHEABLE, MMU_STATE_MASK_CACHEABLE,
     VM_STATE_CACHEABLE, MMU_STATE_CACHEABLE},

    {VM_STATE_MASK_CACHEABLE, MMU_STATE_MASK_CACHEABLE,
     VM_STATE_CACHEABLE_NOT, MMU_STATE_CACHEABLE_NOT},

    {VM_STATE_MASK_CACHEABLE, MMU_STATE_MASK_CACHEABLE,
     VM_STATE_CACHEABLE_WRITETHROUGH, MMU_STATE_CACHEABLE_WRITETHROUGH},

    {VM_STATE_MASK_MEM_COHERENCY, MMU_STATE_MASK_MEM_COHERENCY,
     VM_STATE_MEM_COHERENCY, MMU_STATE_MEM_COHERENCY},

    {VM_STATE_MASK_MEM_COHERENCY, MMU_STATE_MASK_MEM_COHERENCY,
     VM_STATE_MEM_COHERENCY_NOT, MMU_STATE_MEM_COHERENCY_NOT},

    {VM_STATE_MASK_GUARDED, MMU_STATE_MASK_GUARDED,
     VM_STATE_GUARDED, MMU_STATE_GUARDED},

    {VM_STATE_MASK_GUARDED, MMU_STATE_MASK_GUARDED,
     VM_STATE_GUARDED_NOT, MMU_STATE_GUARDED_NOT},
};

/*
 * These are there because frcmmuPpcPteUpdate needs it. It is set after enabling
 * MMU in romStart().
 */
int frcmmuPpcIEnabled = 0, frcmmuPpcDEnabled = 0;

/* All forward declarations of functions taken from mmuPpcLib.c */
#endif /* CODE_FROM_MMUPPCLIB_C */

#ifdef CODE_FROM_VMBASELIB_C
#define NUM_PAGE_STATES 256
unsigned int frcvmStateTransTbl[NUM_PAGE_STATES];
unsigned int frcvmMaskTransTbl[NUM_PAGE_STATES];
unsigned int frcvmPageSize = VM_PAGE_SIZE;
int    frcmmuPhysAddrShift = 0;
STATE_TRANS_TUPLE *frcmmuStateTransArray;
int frcmmuStateTransArraySize;

/* All forward declarations of functions taken from mmuPpcLib.c */

#endif /* CODE_FROM_VMBASELIB_C */

unsigned int frcPgtnextFreeBlkPtr = 0, frcSysPgtTopAddr = 0;

/* Forward declarations */
void frcSysPgtMemoryInit();
void *frcSysPgtMalloc(unsigned int);
void *frcmemalign(unsigned int, unsigned int);
unsigned int frcSetupPTEs();
#endif /* PMC280_DUAL_CPU_PTE_SETUP */

UINT32 earlysysBatDescCPU0 [2 * (_MMU_NUM_IBAT + _MMU_NUM_EXTRA_IBAT + 
                                 _MMU_NUM_DBAT + _MMU_NUM_EXTRA_DBAT)] =
{
    /* IBAT */
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,

#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
#endif /* PMC280_MMU_SUPPORT_FOR_MPC7447 */

    /* DBAT */

    ((PCI0_MEM0_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_128M | _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((PCI0_MEM0_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW | _MMU_LBAT_CACHE_INHIBIT),

    ((INTERNAL_REG_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_1M | _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((INTERNAL_REG_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW | _MMU_LBAT_CACHE_INHIBIT),

     ((PCI0_IO_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_16M | _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((PCI0_IO_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW  |
	  _MMU_LBAT_CACHE_INHIBIT),
    0,
    0,    

/*    ((CS0_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_32M | _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((CS0_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW | _MMU_LBAT_CACHE_INHIBIT),

    ((CS1_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_32M | _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((CS1_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW | _MMU_LBAT_CACHE_INHIBIT),*//* Aravind*/

#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
#endif /* PMC280_MMU_SUPPORT_FOR_MPC7447 */
};

#ifdef PMC280_DUAL_CPU_PTE_SETUP
PHYS_MEM_DESC earlysysPhysMemDescCPU0 [] =
{
    {
    (void *) VIR_CPU0_MEM_BOT,
    (void *) PHY_CPU0_MEM_BOT,
    RAM_LOW_ADRS,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    {
    (void *) (VIR_CPU0_MEM_BOT + RAM_LOW_ADRS),
    (void *) (PHY_CPU0_MEM_BOT + RAM_LOW_ADRS),
    (UINT) NULL, /* Will be filled later */
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE
    },

    {
    (void *) NULL, /* Will be filled later */
    (void *) NULL, /* Will be filled later */
    USER_RESERVED_MEM,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    {
    (void *) BOOTCS_BASE_ADRS,
    (void *) BOOTCS_BASE_ADRS,
    BOOTCS_ADRS_SPACE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    {
    (void *) INTERNAL_SRAM_ADRS,
    (void *) INTERNAL_SRAM_ADRS,
    0x40000,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT | VM_STATE_MEM_COHERENCY
    },
{
    (void *) CS0_BASE_ADRS,
    (void *) CS0_BASE_ADRS,
    CS0_ADRS_SPACE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT | VM_STATE_MEM_COHERENCY
    },

	/*Aravind*/
   
#ifndef ROHS
    {
    (void *) CS1_BASE_ADRS,
    (void *) CS1_BASE_ADRS,
    CS1_ADRS_SPACE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT | VM_STATE_MEM_COHERENCY | VM_STATE_GUARDED
    
    },
#endif    


    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY
};
unsigned int earlysysPhysMemDescNumEntCPU0 = 0;
#endif /* PMC280_DUAL_CPU_PTE_SETUP */

UINT32 earlysysBatDescCPU1 [2 * (_MMU_NUM_IBAT + _MMU_NUM_EXTRA_IBAT + 
                                 _MMU_NUM_DBAT + _MMU_NUM_EXTRA_DBAT)] =
{
        /* IBAT */
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,

#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
#endif /* PMC280_MMU_SUPPORT_FOR_MPC7447 */

        /* DBAT */

    ((PCI0_MEM0_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_128M | _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((PCI0_MEM0_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW | _MMU_LBAT_CACHE_INHIBIT),

    ((INTERNAL_REG_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_1M | _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((INTERNAL_REG_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW | _MMU_LBAT_CACHE_INHIBIT),

    ((PCI0_IO_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_16M | _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((PCI0_IO_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW  |
	  _MMU_LBAT_CACHE_INHIBIT), 

	0,
	0,   
 /*   ((CS0_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_32M | _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((CS0_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW | _MMU_LBAT_CACHE_INHIBIT),

    ((CS1_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_32M | _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((CS1_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW | _MMU_LBAT_CACHE_INHIBIT),
*//* Aravind*/

#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
#endif /* PMC280_MMU_SUPPORT_FOR_MPC7447 */
};

#ifdef PMC280_DUAL_CPU_PTE_SETUP
PHYS_MEM_DESC earlysysPhysMemDescCPU1 [] =
{
    {
    (void *) VIR_CPU1_MEM_BOT,
    (void *) NULL, /* Will be filled later */
    RAM_LOW_ADRS,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    {
    (void *) (VIR_CPU1_MEM_BOT + RAM_LOW_ADRS),
    (void *) (NULL), /* Will be filled later */
    (UINT) NULL, /* Will be filled later */
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE
    },

    {
    (void *) NULL, /* Will be filled later */
    (void *) NULL, /* Will be filled later */
    USER_RESERVED_MEM,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    {
    (void *) BOOTCS_BASE_ADRS,
    (void *) BOOTCS_BASE_ADRS,
    BOOTCS_ADRS_SPACE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    {
    (void *) INTERNAL_SRAM_ADRS,
    (void *) INTERNAL_SRAM_ADRS,
    0x40000, 
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT | VM_STATE_MEM_COHERENCY
    },
    {
    (void *) CS0_BASE_ADRS,
    (void *) CS0_BASE_ADRS,
    CS0_ADRS_SPACE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT | VM_STATE_MEM_COHERENCY
    },

	/*Aravind*/
   
#ifndef ROHS
    {
    (void *) CS1_BASE_ADRS,
    (void *) CS1_BASE_ADRS,
    CS1_ADRS_SPACE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT | VM_STATE_MEM_COHERENCY | VM_STATE_GUARDED
    
    },
#endif    
    /*
     * Here are a few dummy entries for any dynamic mappings developer
     * may need in the future.  They are used via a call to sysMmuMapAdd().
     * Do ensure that the BSP provides sufficient entries!
     */

    /*  PCI0  */
    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY,

    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY
};
unsigned int earlysysPhysMemDescNumEntCPU1 = 0;
#endif /* PMC280_DUAL_CPU_PTE_SETUP */

#ifdef PMC280_DUAL_CPU_PTE_SETUP

#ifdef CODE_FROM_MMUPPCALIB_S
/*
 * Since this code is assembly, it is put in romInit.s
 */
extern void frcmmuPpcSrSet(unsigned int, unsigned int);
extern unsigned int frcmmuPpcSrGet(unsigned int);
extern void frcmmuTlbie(unsigned int);
#endif /* CODE_FROM_MMUPPCALIB_S */

#ifdef CODE_FROM_SYSLIB_C

char *frcsysPhysMemTop (void)
{
    return(char *)(LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE);
}

char * frcsysMemTop (void)
{
    static char * memTop = NULL;

    if(memTop == NULL)
    {
        memTop = frcsysPhysMemTop () - USER_RESERVED_MEM;
    }

    return memTop;
}

#endif /* CODE_FROM_SYSLIB_C */

void frcSysPgtMemoryInit()
{
    frcPgtnextFreeBlkPtr = LOCAL_MEM_SIZE - USER_RESERVED_MEM + SYS_DRV_SIZE;
    frcSysPgtTopAddr = frcPgtnextFreeBlkPtr + SYS_PGT_SIZE;
}

void *frcSysPgtMalloc(unsigned int size)
{
    unsigned int returnPtr;

    if(frcPgtnextFreeBlkPtr + size > frcSysPgtTopAddr)
            return 0;
    returnPtr = frcPgtnextFreeBlkPtr;
    frcPgtnextFreeBlkPtr += size;
    return ((void *)returnPtr);
}

/*
 * This routine does the same function as memalign. Since this routine
 * is going to be called when memLib is not yet initialised.
 */
void *frcmemalign(unsigned int size, unsigned int align)
{
    unsigned int blkBase, alignmentMask = 0;

    /* Add aligment bytes to the block size for aligment space */
    blkBase = (unsigned int) frcSysPgtMalloc(size + align);

    /* If malloc is already aligned */
    if(blkBase % align != 0)
    {
        /* Mask for getting the address low bits for checking the aligment */
        alignmentMask = align - 1;

        /* Add offset to the block base to complete the aligment */
        blkBase += align - (alignmentMask & blkBase);
    }

    return ((void *)blkBase);
}


#ifdef CODE_FROM_MMUPPCLIB_C
/******************************************************************************
*
* frcmmuPpcStateSet - set state of virtual memory page
*
*
* MMU_STATE_VALID       MMU_STATE_VALID_NOT     valid/invalid
* MMU_STATE_WRITABLE    MMU_STATE_WRITABLE_NOT  writable/writeprotected
* MMU_STATE_CACHEABLE   MMU_STATE_CACHEABLE_NOT cachable/notcachable
* MMU_STATE_CACHEABLE_WRITETHROUGH
* MMU_STATE_CACHEABLE_COPYBACK
* MMU_STATE_MEM_COHERENCY       MMU_STATE_MEM_COHERENCY_NOT
* MMU_STATE_GUARDED             MMU_STATE_GUARDED_NOT
* MMU_STATE_NO_ACCESS
* MMU_STATE_NO_EXECUTE
*/

STATUS frcmmuPpcStateSet
    (
    MMU_TRANS_TBL *     pTransTbl,      /* translation table */
    void *              effectiveAddr,  /* page whose state to modify */
    UINT                stateMask,      /* mask of which state bits to modify */    UINT                state           /* new state bit values */
    )
{
    PTE *       pPte;           /* PTE address */
    PTE         pte;            /* PTE value */

    /*
     * Try to find in the PTEG table pointed to by the pTransTbl structure,
     * the PTE corresponding to the <effectiveAddr>.
     * If this PTE can not be found then return ERROR.
     */

    if (frcmmuPpcPteGet (pTransTbl, effectiveAddr, &pPte) != OK)
    {
        return -1;
    }

    /*
     * Check if the state to set page corresponding to <effectiveAddr> will
     * not set the cache in inhibited and writethrough mode. This mode is not
     * supported by the cache.
     */

    if ((stateMask & MMU_STATE_MASK_CACHEABLE) &&
        (state & MMU_STATE_CACHEABLE_NOT) &&
        (state & MMU_STATE_CACHEABLE_WRITETHROUGH))
    {
        return -1;
    }

    /* load the value of the PTE pointed to by pPte to pte */

    pte = *pPte;

    /* set or reset the VALID bit if requested */

    pte.bytes.word0 = (pte.bytes.word0 & ~(stateMask & MMU_STATE_MASK_VALID)) |
                      (state & stateMask & MMU_STATE_MASK_VALID);

    /* set or reset the WIMG bit if requested */

    pte.bytes.word1 = (pte.bytes.word1 &
                        ~(stateMask & MMU_STATE_MASK_WIMG_AND_WRITABLE)) |
                         (state & stateMask & MMU_STATE_MASK_WIMG_AND_WRITABLE);


    /* update the PTE in the table */

    frcmmuPpcPteUpdate (pPte, &pte);

    /* update the Translation Lookside Buffer */

    frcmmuPpcTlbie (effectiveAddr);

    return (OK);
}

/******************************************************************************
*
* frcmmuPpcMemPagesWriteDisable - write disable memory holding PTEs
*
* Memory containing translation table descriptors is marked as read only
* to protect the descriptors from being corrupted.  This routine write protects
* all the memory used to contain a given translation table's descriptors.
*
*/
void frcmmuPpcMemPagesWriteDisable
    (
    MMU_TRANS_TBL * pTransTbl
    )
{
    unsigned int thisPage;

    thisPage = pTransTbl->hTabOrg;

    while (thisPage < (pTransTbl->hTabOrg + pTransTbl->pteTableSize))
    {
        if (frcmmuPpcStateSet (pTransTbl,(void *)  thisPage,
                        MMU_STATE_MASK_WIMG_AND_WRITABLE,
                        MMU_STATE_WRITABLE_NOT  | MMU_STATE_GUARDED_NOT |
                        MMU_STATE_MEM_COHERENCY | MMU_STATE_CACHEABLE_COPYBACK)
                        == ERROR)
            return ;

        thisPage += PAGE_SIZE;
    }
}

/******************************************************************************
*
* frcmmuPpcCurrentSet - change active translation table
*
*/

void frcmmuPpcCurrentSet
    (
    MMU_TRANS_TBL * pTransTbl   /* new active tranlation table */
    )
{
    static BOOL firstTime = TRUE;       /* first time call flag */

    if (firstTime)
    {
        /*
         * write protect all the pages containing the PTEs allocated for
         * the global translation table.  Need to do this because when this
         * memory is allocated, the global translation table doesn't exist yet.
         */
#if FALSE
         frcmmuPpcMemPagesWriteDisable (&frcmmuGlobalTransTbl);
#endif
         /* frcmmuPpcMemPagesWriteDisable (pTransTbl); */

         firstTime = FALSE;
     }

    /*
     * the SDR1 register MUST NOT be altered when the MMU is enabled
     * (see "PowerPc Mircoprocessor Family: The Programming Environments
     * page 2-40 note Nb 5."
     * If the MMU is enabled then turn it off, change SDR1 and
     * enabled it again. Otherwise change SDR1 value.
     */
    if ((frcmmuPpcIEnabled)  || (frcmmuPpcDEnabled))
    {
#if FALSE
        mmuPpcEnable (FALSE);                   /* disable the MMU */
        mmuPpcSdr1Set (pTransTbl->hTabOrg | pTransTbl->hTabMask);
        mmuPpcEnable (TRUE);                    /* re-enable  the MMU */
#endif 
    }
    else
        mmuPpcSdr1Set (pTransTbl->hTabOrg | pTransTbl->hTabMask);
 }

/********************************************************************************
* frcmmuPpcPteUpdate - update a PTE value
*
*/
void  frcmmuPpcPteUpdate
    (
    PTE *       pteAddr,        /* address of the PTE to update */
    PTE *       pteVal          /* PTE value */
    )
{
    /*
     * We don't intend to call this routine with MMU enabled. This is left
     * as was in the original mmuPpcPteUpdate().
     */
    if ((frcmmuPpcIEnabled) | (frcmmuPpcDEnabled))
    {
#if FALSE
        mmuPpcEnable (FALSE);                   /* disable the mmu */
        memcpy ((void *) pteAddr, pteVal, sizeof (PTE));
        mmuPpcEnable (TRUE);
#endif
    }
    else
        memcpy ((void *) pteAddr, pteVal, sizeof (PTE));
}

/********************************************************************************
* frcmmuPpcPtegAddrGet- get the address of the two Page Table Entry Groups(PTEG)
*
*/
void frcmmuPpcPtegAddrGet
    (
    MMU_TRANS_TBL *     pTransTbl,              /* translation table */
    void *              effectiveAddr,          /* effective address */
    PTEG **             ppPteg1,                /* page table entry group 1 */
    PTEG **             ppPteg2,                /* page table entry group 2 */
    u_int *             pVirtualSegId,          /* virtual segment Id value */
    u_int *             pAbbrevPageIndex        /* abbreviated page index */
    )
{
    SR          srVal;                  /* segment register value */
    UINT        pageIndex;              /* page index value */
    UINT        primHash;               /* primary hash value */
    UINT        hashValue1;             /* hash value 1 */
    UINT        hashValue2;             /* hash value 2 */
    UINT        hashValue1L;
    UINT        hashValue1H;
    UINT        hashValue2L;
    UINT        hashValue2H;
    EA          effAddr;

#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
    /* dbg_puts0("Entering frcmmuPpcPtegAddrGet...\n"); */
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */

    effAddr = * ((EA *)  &effectiveAddr);

    srVal.value =  frcmmuPpcSrGet (effAddr.srSelect);
    * pVirtualSegId = srVal.bit.vsid ;

    pageIndex = effAddr.pageIndex;

    * pAbbrevPageIndex = (pageIndex & MMU_EA_API) >> MMU_EA_API_SHIFT;

    primHash = srVal.bit.vsid & MMU_VSID_PRIM_HASH;

    hashValue1 = primHash ^ pageIndex;

    hashValue2 = ~ hashValue1;

    hashValue1L = (hashValue1 & MMU_HASH_VALUE_LOW) << MMU_PTE_HASH_VALUE_LOW_SHIFT;
    hashValue1H = (hashValue1 & MMU_HASH_VALUE_HIGH) >> MMU_HASH_VALUE_HIGH_SHIFT;

    hashValue2L = (hashValue2 & MMU_HASH_VALUE_LOW) << MMU_PTE_HASH_VALUE_LOW_SHIFT;
    hashValue2H = (hashValue2 & MMU_HASH_VALUE_HIGH) >> MMU_HASH_VALUE_HIGH_SHIFT;

    hashValue1H = (hashValue1H & pTransTbl->hTabMask) << MMU_PTE_HASH_VALUE_HIGH_SHIFT ;
    hashValue2H = (hashValue2H & pTransTbl->hTabMask) << MMU_PTE_HASH_VALUE_HIGH_SHIFT;

    * ppPteg1 = (PTEG *) (pTransTbl->hTabOrg | hashValue1H | hashValue1L);

    * ppPteg2 = (PTEG *) (pTransTbl->hTabOrg | hashValue2H | hashValue2L);

#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
    /* dbg_puts0("Exiting frcmmuPpcPtegAddrGet...\n"); */
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */
}

/********************************************************************************
* frcmmuPpcPteGet - get the address of a PTE of a given effective address
*
* This routine finds the PTE address corresponding to the <effectiveAddr> in
* the PTEG table pointed to by <pTransTbl> structure. If a matching PTE
* existe, the routine save the PTE address at the address pointed to by <ppPte>.* If any PTE matching the <effectiveAddr> is not found then the function
* return ERROR.
*
* RETURNS: OK or ERROR.
*/
STATUS frcmmuPpcPteGet
    (
    MMU_TRANS_TBL *     pTransTbl,      /* translation table */
    void *              effectiveAddr,  /* effective address */
    PTE **              ppPte           /* result */
    )
{
    PTEG *      pPteg1;                 /* PTEG 1 address */
    PTEG *      pPteg2;                 /* PTEG 2 address */
    UINT        vsid;                   /* virtual segment ID */
    UINT        api;                    /* abbreviated page index */
    UINT        pteIndex;               /* PTE index in a PTEG */

#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
    /* dbg_puts0("Entering frcmmuPpcPteGet...\n"); */
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */

    /* get the address of both PTEGs, the VSID and API values */

    /* Using frcmmuPpcPtegAddrGet() instead of mmuPpcPtegAddrGet */
    frcmmuPpcPtegAddrGet (pTransTbl, effectiveAddr, &pPteg1,
                                                    &pPteg2, &vsid, &api);

    pteIndex = 0;
    /*
     * read the PTEs of the first group. If one of the PTE matchs
     * the expected PTE then extrats the physical address from the PTE word 1
     * and exits the function with OK. If not, checks the next PTE. If no PTE
     * matchs the expected PTE in the first PTEG then read the second PTEG.
     */

    do
    {
        if ((pPteg1->pte[pteIndex].field.v    == TRUE) &&
            (pPteg1->pte[pteIndex].field.vsid == vsid) &&
            (pPteg1->pte[pteIndex].field.api  == api) &&
            (pPteg1->pte[pteIndex].field.h    == 0 ))
        {
            * ppPte = &pPteg1->pte[pteIndex];
#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
            /* dbg_puts0("Exiting frcmmuPpcPteGet...\n"); */
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */
            return (OK);
        }
    }
    while (++pteIndex < MMU_PTE_BY_PTEG);

    pteIndex = 0;
    /*
     * read the PTEs of the second PTEG. If one of the PTE matchs
     * the expected PTE then extrats the physical address from the PTE word 1
     * and exit the function with OK. If not check the next PTE. If no PTE
     * match the expected PTE then exit the function with ERROR.
     */

    do
    {
        if ((pPteg2->pte[pteIndex].field.v    == TRUE) &&
            (pPteg2->pte[pteIndex].field.vsid == vsid) &&
            (pPteg2->pte[pteIndex].field.api  ==  api) &&
            (pPteg2->pte[pteIndex].field.h    == 1 ))
        {
            * ppPte = &pPteg2->pte[pteIndex];
#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
            /* dbg_puts0("Exiting frcmmuPpcPteGet...\n"); */
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */
            return (OK);
        }
    }
    while (++pteIndex < MMU_PTE_BY_PTEG);

    return -1;
}

/******************************************************************************
*
* frcmmuPpcPageMap - map physical memory page to virtual memory page
*
* The physical page address is entered into the pte corresponding to the
* given virtual page.  The state of a newly mapped page is undefined.
*
* RETURNS: OK or ERROR if translation table creation failed.
*/
STATUS frcmmuPpcPageMap
    (
    MMU_TRANS_TBL *     pTransTbl,      /* translation table */
    void *              effectiveAddr,  /* effective address */
    void *              physicalAddr    /* physical address */
    )
{
    PTE *       pPte;                   /* PTE address */
    PTE         pte;                    /* PTE value */
    PTEG *      pPteg1;                 /* PTEG 1 address */
    PTEG *      pPteg2;                 /* PTEG 2 address */
    UINT        vsid;                   /* virtual segment ID */
    UINT        api;                    /* abbreviated page index */
    UINT        pteIndex;               /* PTE index in a PTEG */
    UINT        hashLevel;              /* hash table level */

#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
    /* dbg_puts0("Entering frcmmuPpcPageMap...\n"); */
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */

    if (frcmmuPpcPteGet (pTransTbl, effectiveAddr, &pPte) == OK)
    {
        pte = * pPte;
    }
    else
    {
        /* get the address of both PTEGs, VSID and API value */

        /* Using frcmmuPpcPtegAddrGet() instead of mmuPpcPtegAddrGet */
        frcmmuPpcPtegAddrGet (pTransTbl, effectiveAddr, &pPteg1,
                                                        &pPteg2, &vsid, &api);

        pteIndex = 0;
        pPte = NULL;
        hashLevel = 0;

        /*
         * read the PTEs of the first group. If one of the PTE matchs
         * the expected PTE then extrats the physical address from the PTE
         * word 1 and exits the function with OK. If not, chechs the next PTE.
         * If no PTE matchs the expected PTE then read the second group.
         */
        do
        {
            if (pPteg1->pte[pteIndex].field.v == FALSE)
            {
                pPte = &pPteg1->pte[pteIndex];
                hashLevel = 0;
                break;
            }

            if (pPteg2->pte[pteIndex].field.v == FALSE)
            {
                pPte = &pPteg2->pte[pteIndex];
                hashLevel = 1;
            }
        }
        while (++pteIndex < MMU_PTE_BY_PTEG);
        if (pPte == NULL)
        {
            return -1;
        }

        pte.field.v = TRUE;             /* entry valid */
        pte.field.vsid = vsid;
        pte.field.h = hashLevel;
        pte.field.api = api;
        pte.field.r = 0;
        pte.field.c = 0;
        pte.field.wimg = 0;             /* cache writethrough mode */
        pte.field.pp = 2;               /* read/write */
    }

    pte.field.rpn = (u_int) physicalAddr >> MMU_PTE_RPN_SHIFT;

    frcmmuPpcPteUpdate (pPte, &pte);

    frcmmuPpcTlbie (effectiveAddr);

#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
    /* dbg_puts0("Exiting frcmmuPpcPageMap...\n"); */
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */

    return 0;
}

/******************************************************************************
*
* frcmmuPpcGlobalPageMap- map physical memory page to global virtual memory page
*
* mmuPpcGlobalPageMap is used to map physical pages into global virtual memory
* that is shared by all virtual memory contexts.  The translation tables
* for this section of the virtual space are shared by all virtual memory
* contexts.
*
* RETURNS: OK or ERROR if no pte for given virtual page.
*/
STATUS frcmmuPpcGlobalPageMap
    (
    void *  effectiveAddr,      /* effective address */
    void *  physicalAddr,       /* physical address */
    unsigned int cpu            /* CPU number */
    )
{
    unsigned int status;
    if(cpu)
    {
        status = frcmmuPpcPageMap (&frcmmuGlobalTransTbl1, 
                                   effectiveAddr, physicalAddr);
    }
    else
    {
        status = frcmmuPpcPageMap (&frcmmuGlobalTransTbl0, 
                                   effectiveAddr, physicalAddr);
    }
    return status;
}
#endif /* CODE_FROM_MMUPPCLIB_C */

#ifdef CODE_FROM_VMBASELIB_C
/****************************************************************************
*
* frcvmBaseGlobalMap - map physical to virtual in shared global virtual memory
*
* frcvmBaseGlobalMap maps physical pages to virtual space that is shared by all
* virtual memory contexts.  Calls to vmBaseGlobalMap should be made before any
* virtual memory contexts are created to insure that the shared global mappings
* will be included in all virtual memory contexts.  Mappings created with
* vmBaseGlobalMap after virtual memory contexts are created are not guaranteed
* to appear in all virtual memory contexts.
*
* RETURNS: OK, or ERROR if virtualAddr or physcial page addresses are not on
* page boundries, len is not a multiple of page size, or mapping failed.
*/
STATUS frcvmBaseGlobalMap
    (
    void *virtualAddr,
    void *physicalAddr,
    UINT len,
    unsigned int cpu
    )
{
    int pageSize = frcvmPageSize;
    char *thisVirtPage = (char *) virtualAddr;
    char *thisPhysPage = (char *) physicalAddr;
    FAST UINT numBytesProcessed = 0;

#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
    if(frcWhoAmI())
    {
#ifdef DEBUGGING
        dbg_puts1("Entering frcvmBaseGlobalMap...\n");
#endif
    }
    else
    {
#ifdef DEBUGGING
        dbg_puts0("Entering frcvmBaseGlobalMap...\n");
#endif
    }
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */

    if (((UINT) thisVirtPage % pageSize) != 0)
    {
#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
        if(frcWhoAmI())
        {
            dbg_printf1("frcvmBaseGlobalMap: S_vmLib_NOT_PAGE_ALIGNED(1)\n");
            dbg_printf1("thisVirtPage = 0x%08x pageSize = 0x%08x\n", 
                         thisVirtPage, pageSize);
        }
        else
        {
            dbg_printf0("frcvmBaseGlobalMap: S_vmLib_NOT_PAGE_ALIGNED(1)\n");
            dbg_printf0("thisVirtPage = 0x%08x pageSize = 0x%08x\n", 
                         thisVirtPage, pageSize);
        }
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */
        return -1;
    }

    if ((!frcmmuPhysAddrShift) && (((UINT) thisPhysPage % pageSize) != 0))
    {
#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
        dbg_puts0("frcvmBaseGlobalMap: S_vmLib_NOT_PAGE_ALIGNED(2)\n");
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */
        return -1;
    }

    if ((len % pageSize) != 0)
    {
#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
        dbg_puts0("frcvmBaseGlobalMap: S_vmLib_NOT_PAGE_ALIGNED(3)\n");
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */
        return -1;
    }

    while (numBytesProcessed < len)
    {
        /*
         * Function call to mmuPpcGlobalMapPage has been changed to
         * frcmmuPpcGlobalMapPage().
         */
        if (frcmmuPpcGlobalPageMap(thisVirtPage, thisPhysPage, cpu) == ERROR)
#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
        {
            dbg_puts0("frcvmBaseGlobalMap: frcmmuPpcGlobalPageMap in ERROR\n");
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */
            return -1;
#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
        }
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */

        thisVirtPage += pageSize;
        thisPhysPage += (frcmmuPhysAddrShift ? 1 : pageSize);
        numBytesProcessed += pageSize;
    }

#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
    if(frcWhoAmI())
    {
#ifdef DEBUGGING
        dbg_puts1("Exiting frcvmBaseGlobalMap...\n");
#endif
    }
    else
    {
#ifdef DEBUGGING
        dbg_puts0("Exiting frcvmBaseGlobalMap...\n");
#endif
    }
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */
    return 0;
}
#endif /* CODE_FROM_VMBASELIB_C */

unsigned int frcSetupPTEs()
{
#ifdef CODE_FROM_MMUPPCLIB_C
    unsigned int entryNb;                /* entry number */
    unsigned int memSize = 0;            /* total memorie size */
    unsigned int position;
    SR  srVal;
    PHYS_MEM_DESC *pMemDesc;
#endif /* CODE_FROM_MMUPPCLIB_C */

#ifdef CODE_FROM_SYSLIB_C
    PHYS_MEM_DESC *pMmu;
    int ix;
#endif /* CODE_FROM_SYSLIB_C */

#ifdef CODE_FROM_VMBASELIB_C
    unsigned int i;
    PHYS_MEM_DESC *thisDesc;
    char *thisPage;
    int         j;
    unsigned int newState;
    unsigned int newMask;
    int pageSize = frcvmPageSize;
    unsigned int archIndepStateMask;
    unsigned int archDepStateMask;
    unsigned int archDepState;
    unsigned int archIndepState;
    unsigned int numBytesProcessed;
#endif /* CODE_FROM_VMBASELIB_C */

#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
    if(frcWhoAmI())
    {
#ifdef DEBUGGING
        dbg_puts1("Entering frcSetupPTEs...\n");
#endif
    }
    else
    {
#ifdef DEBUGGING
        dbg_puts0("Entering frcSetupPTEs...\n");
#endif
    }
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */
    
    if(!frcWhoAmI())
    {
#ifdef CODE_FOR_CPU_0
    earlysysPhysMemDescCPU0[1].len = 
               (LOCAL_MEM_SIZE - USER_RESERVED_MEM - RAM_LOW_ADRS);
    earlysysPhysMemDescCPU0[2].virtualAddr =
               (void *) VIR_CPU0_USER_RESERVED_MEM_BOT; 
    earlysysPhysMemDescCPU0[2].physicalAddr =
               (void *) PHY_CPU0_USER_RESERVED_MEM_BOT; 

#ifdef CODE_FROM_SYSLIB_C
    /* Calculate the number of entries in the sysPhysMemDesc */
    pMmu = &earlysysPhysMemDescCPU0[0];

    for(ix = 0; ix < FRCNELEMENTS (earlysysPhysMemDescCPU0); ix++)
    {
        if(pMmu->virtualAddr != (void *)DUMMY_VIRT_ADDR)
            pMmu++;     /* used entry */
        else
            break;  /* dummy entry, assume no more */
     }

    /* This is the REAL number of sysPhysMemDesc entries in use. */
    earlysysPhysMemDescNumEntCPU0 = ix;
#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
#ifdef DEBUGGING
    dbg_printf0("earlysysPhysMemDescNumEntCPU0 = %d\n", 
                 earlysysPhysMemDescNumEntCPU0);
#endif
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */
#endif /* CODE_FROM_SYSLIB_C */

#ifdef CODE_FROM_MMUPPCLIB_C

    /* initialize the Segment Registers */

    srVal.bit.t = 0;
    srVal.bit.ks = 1;
    srVal.bit.kp = 1;
    srVal.bit.n = 0;
    srVal.bit.vsid = 0;

    for (entryNb = 0; entryNb < 16; entryNb++)          /* XXX TPR to fix */
        {
        frcmmuPpcSrSet (entryNb, srVal.value);
        srVal.bit.vsid += 0x100;
        }

    pMemDesc =  (PHYS_MEM_DESC *) &earlysysPhysMemDescCPU0[0];

    /*
     * compute the total size of memory to map by reading the
     * sysPhysMemDesc[] table.
     */

    for (entryNb = 0; entryNb < earlysysPhysMemDescNumEntCPU0; entryNb++)
        {
        memSize += earlysysPhysMemDescCPU0[entryNb].len;
        }

#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
#ifdef DEBUGGING
    dbg_printf0("memSize = 0x%08x\n", memSize);
#endif
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */

    memSize = (memSize - 1) / 0x800000 ;

    position = 0;

    do
        memSize = memSize >> 1;
    while ((++position < frcmmuPpcHashTabInfoNumEnt) & (memSize != 0));

#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
#ifdef DEBUGGING
    dbg_printf0("position = 0x%08x\n", position);
#endif
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */

    frcmmuGlobalTransTbl0.hTabOrg=frcmmuPpcHashTabInfo[position].hashTabOrgMask;
    frcmmuGlobalTransTbl0.hTabMask = frcmmuPpcHashTabInfo[position].hastTabMask;
    frcmmuGlobalTransTbl0.pteTableSize = frcmmuPpcHashTabInfo[position].pteTabMinSize;

    /*
     * allocate memory to save PTEs
     * The mimimum size depends of the total memory to map.
     */

    /* 
     * Note: frcmemalign is a routine used to allocate aligned memory.
     *       Changed from memalign.
     */
    frcmmuGlobalTransTbl0.hTabOrg &=
                (u_int) frcmemalign (frcmmuGlobalTransTbl0.pteTableSize,
                                        frcmmuGlobalTransTbl0.pteTableSize);

    WRITE_DWORD((int *)HTABORG_0, frcmmuGlobalTransTbl0.hTabOrg);
    WRITE_DWORD((int *)HTABMASK_0, frcmmuGlobalTransTbl0.hTabMask);
    WRITE_DWORD((int *)PTETABLESIZE_0, frcmmuGlobalTransTbl0.pteTableSize);

#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
#ifdef DEBUGGING
    dbg_printf0("hTabOrg = 0x%08x\n", frcmmuGlobalTransTbl0.hTabOrg);
#endif
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */

    /* invalided all PTE in the table */
    memset ((void *) frcmmuGlobalTransTbl0.hTabOrg, 0x00,
            frcmmuGlobalTransTbl0.pteTableSize);

    frcmmuStateTransArray = &frcmmuStateTransArrayLocal [0];

    frcmmuStateTransArraySize =
                sizeof (frcmmuStateTransArrayLocal)/sizeof (STATE_TRANS_TUPLE);

#endif /* CODE_FROM_MMUPPCLIB_C */

#ifdef CODE_FROM_VMBASELIB_C


    /* initialize the page state translation table.  This table is used to
     * translate betseen architecture independent state values and architecture
     * dependent state values */

    for (i = 0; i < NUM_PAGE_STATES; i++)
    {
        newState = 0;

        for (j = 0; j < frcmmuStateTransArraySize; j++)
        {
            STATE_TRANS_TUPLE *thisTuple = &frcmmuStateTransArray[j];
            UINT frcarchIndepState = thisTuple->archIndepState;
            UINT frcarchDepState = thisTuple->archDepState;
            UINT frcarchIndepMask = thisTuple->archIndepMask;

            if ((i & frcarchIndepMask) == frcarchIndepState)
                newState |= frcarchDepState;
        }

        frcvmStateTransTbl [i] = newState;
    }

    /* initialize the page state mask translation table.  This table is used to
     * translate betseen architecture independent state masks and architecture
     * dependent state masks */

    for (i = 0; i < NUM_PAGE_STATES; i++)
    {
        newMask = 0;
        for (j = 0; j < frcmmuStateTransArraySize; j++)
        {
            STATE_TRANS_TUPLE *thisTuple = &frcmmuStateTransArray[j];
            UINT frcarchIndepMask = thisTuple->archIndepMask;
            UINT frcarchDepMask = thisTuple->archDepMask;

            if ((i & frcarchIndepMask) == frcarchIndepMask)
                newMask |= frcarchDepMask;
        }

        frcvmMaskTransTbl [i] = newMask;
    }

    /* set up global transparent mapping of physical to virtual memory */

    for (i = 0; i < earlysysPhysMemDescNumEntCPU0; i++)
    {
        thisDesc = &earlysysPhysMemDescCPU0[i];

#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
#ifdef DEBUGGING
        dbg_printf0("thisDesc->virtualAddr = 0x%08x\n",thisDesc->virtualAddr);
        dbg_printf0("thisDesc->physicalAddr = 0x%08x\n",thisDesc->physicalAddr);
        dbg_printf0("thisDesc->len = 0x%08x\n",thisDesc->len);
#endif
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */

        /* map physical directly to virtual and set initial state */

        /*
         * The call was to vmBaseGlobalMap. This has been changed to 
         * frcvmBaseGlobalMap().
         */
        if (frcvmBaseGlobalMap ((void *) thisDesc->virtualAddr,
                         (void *) thisDesc->physicalAddr,
                         thisDesc->len, 0) == ERROR)
        {
            return -1;
        }
    }

    /* set the state of all the global memory we just created */
    for (i = 0; i < earlysysPhysMemDescNumEntCPU0; i++)
    {
        thisDesc = &earlysysPhysMemDescCPU0[i];

        thisPage = thisDesc->virtualAddr;
        numBytesProcessed = 0;

        while (numBytesProcessed < thisDesc->len)
        {
            /* we let the use specifiy cacheablility, but not writeablility */

            archIndepState = thisDesc->initialState;

            archDepState = frcvmStateTransTbl [archIndepState];

            archIndepStateMask =
                VM_STATE_MASK_CACHEABLE |
                VM_STATE_MASK_VALID |
                VM_STATE_MASK_WRITABLE |
                VM_STATE_MASK_MEM_COHERENCY |
                VM_STATE_MASK_GUARDED;

            archDepStateMask = frcvmMaskTransTbl [archIndepStateMask];

            if (frcmmuPpcStateSet(&frcmmuGlobalTransTbl0, thisPage,
                            archDepStateMask, archDepState) == ERROR)
                return -1;

            thisPage += pageSize;
            numBytesProcessed += pageSize;
        }
    }
    frcmmuPpcCurrentSet(&frcmmuGlobalTransTbl0);
#endif /* CODE_FROM_VMBASELIB_C */

#endif /* CODE_FOR_CPU_0 */
    } 
    else
    {
#ifdef CODE_FOR_CPU_1
    earlysysPhysMemDescCPU1[0].physicalAddr = (void *) PHY_CPU1_MEM_BOT;
    earlysysPhysMemDescCPU1[1].physicalAddr = 
               (void *) PHY_CPU1_MEM_BOT + RAM_LOW_ADRS;
    earlysysPhysMemDescCPU1[1].len = 
               LOCAL_MEM_SIZE - USER_RESERVED_MEM - RAM_LOW_ADRS; 
    earlysysPhysMemDescCPU1[2].virtualAddr = 
               (void *) VIR_CPU1_USER_RESERVED_MEM_BOT;
    earlysysPhysMemDescCPU1[2].physicalAddr = 
               (void *) PHY_CPU1_USER_RESERVED_MEM_BOT;
    
#ifdef CODE_FROM_SYSLIB_C
    /* Calculate the number of entries in the sysPhysMemDesc */

    pMmu = &earlysysPhysMemDescCPU1[0];

    for(ix = 0; ix < FRCNELEMENTS (earlysysPhysMemDescCPU1); ix++)
    {
        if(pMmu->virtualAddr != (void *)DUMMY_VIRT_ADDR)
            pMmu++;     /* used entry */
        else
            break;  /* dummy entry, assume no more */
    }

    /* This is the REAL number of sysPhysMemDesc entries in use. */
    earlysysPhysMemDescNumEntCPU1 = ix;
#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
#ifdef DEBUGGING
    dbg_printf1("earlysysPhysMemDescNumEntCPU1 = %d\n", 
                 earlysysPhysMemDescNumEntCPU1);
#endif
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */

#endif /* CODE_FROM_SYSLIB_C */

#ifdef CODE_FROM_MMUPPCLIB_C

    /* initialize the Segment Registers */
    srVal.bit.t = 0;
    srVal.bit.ks = 1;
    srVal.bit.kp = 1;
    srVal.bit.n = 0;
    srVal.bit.vsid = 0;

    for (entryNb = 0; entryNb < 16; entryNb++)          /* XXX TPR to fix */
    {
        frcmmuPpcSrSet (entryNb, srVal.value);
        srVal.bit.vsid += 0x100;
    }

    pMemDesc =  (PHYS_MEM_DESC *) &earlysysPhysMemDescCPU1[0];

    /*
     * compute the total size of memory to map by reading the
     * sysPhysMemDesc[] table.
     */

    for (entryNb = 0; entryNb < earlysysPhysMemDescNumEntCPU1; entryNb++)
        {
        memSize += earlysysPhysMemDescCPU1[entryNb].len;
        }

#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
#ifdef DEBUGGING
    dbg_printf1("memSize = 0x%08x\n", memSize);
#endif
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */

    memSize = (memSize - 1) / 0x800000 ;

    position = 0;

    do
        memSize = memSize >> 1;
    while ((++position < frcmmuPpcHashTabInfoNumEnt) & (memSize != 0));

#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
#ifdef DEBUGGING
    dbg_printf1("position = 0x%08x\n", position);
#endif
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */

    frcmmuGlobalTransTbl1.hTabOrg=frcmmuPpcHashTabInfo[position].hashTabOrgMask;
    frcmmuGlobalTransTbl1.hTabMask = frcmmuPpcHashTabInfo[position].hastTabMask;
    frcmmuGlobalTransTbl1.pteTableSize = frcmmuPpcHashTabInfo[position].pteTabMinSize;

    /*
     * allocate memory to save PTEs
     * The mimimum size depends of the total memory to map.
     */

    /* 
     * Note: frcmemalign is a routine used to allocate aligned memory.
     *       Changed from memalign.
     */
    frcmmuGlobalTransTbl1.hTabOrg &=
                (u_int) frcmemalign (frcmmuGlobalTransTbl1.pteTableSize,
                                        frcmmuGlobalTransTbl1.pteTableSize);

    WRITE_DWORD((int *)HTABORG_1, frcmmuGlobalTransTbl1.hTabOrg);
    WRITE_DWORD((int *)HTABMASK_1, frcmmuGlobalTransTbl1.hTabMask);
    WRITE_DWORD((int *)PTETABLESIZE_1, frcmmuGlobalTransTbl1.pteTableSize);

#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
#ifdef DEBUGGING
    dbg_printf1("hTabOrg = 0x%08x\n", frcmmuGlobalTransTbl1.hTabOrg);
#endif
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */

    /* invalided all PTE in the table */
    memset ((void *) frcmmuGlobalTransTbl1.hTabOrg, 0x00,
            frcmmuGlobalTransTbl1.pteTableSize);

    frcmmuStateTransArray = &frcmmuStateTransArrayLocal [0];

    frcmmuStateTransArraySize =
                sizeof (frcmmuStateTransArrayLocal)/sizeof (STATE_TRANS_TUPLE);

#endif /* CODE_FROM_MMUPPCLIB_C */

#ifdef CODE_FROM_VMBASELIB_C
    /* set up global transparent mapping of physical to virtual memory */
    for (i = 0; i < earlysysPhysMemDescNumEntCPU1; i++)
    {
        thisDesc = &earlysysPhysMemDescCPU1[i];

#ifdef PMC280_DUAL_CPU_PTE_SETUP_DBG
#ifdef DEBUGING
        dbg_printf1("thisDesc->virtualAddr = 0x%08x\n",thisDesc->virtualAddr);
        dbg_printf1("thisDesc->physicalAddr = 0x%08x\n",thisDesc->physicalAddr);
        dbg_printf1("thisDesc->len = 0x%08x\n",thisDesc->len);
#endif
#endif /* PMC280_DUAL_CPU_PTE_SETUP_DBG */

        /* map physical directly to virtual and set initial state */

        /*
         * The call was to vmBaseGlobalMap. This has been changed to 
         * frcvmBaseGlobalMap().
         */
        if (frcvmBaseGlobalMap ((void *) thisDesc->virtualAddr,
                         (void *) thisDesc->physicalAddr,
                         thisDesc->len, 1) == ERROR)
        {
            return -1;
        }
    }

    /* set the state of all the global memory we just created */
    for (i = 0; i < earlysysPhysMemDescNumEntCPU1; i++)
    {
        thisDesc = &earlysysPhysMemDescCPU1[i];
        thisPage = thisDesc->virtualAddr;
        numBytesProcessed = 0;

        while (numBytesProcessed < thisDesc->len)
        {
            /* we let the use specifiy cacheablility, but not writeablility */

            archIndepState = thisDesc->initialState;

            archDepState = frcvmStateTransTbl [archIndepState];

            archIndepStateMask =
                VM_STATE_MASK_CACHEABLE |
                VM_STATE_MASK_VALID |
                VM_STATE_MASK_WRITABLE |
                VM_STATE_MASK_MEM_COHERENCY |
                VM_STATE_MASK_GUARDED;

            archDepStateMask = frcvmMaskTransTbl [archIndepStateMask];

            if (frcmmuPpcStateSet(&frcmmuGlobalTransTbl1, thisPage,
                            archDepStateMask, archDepState) == ERROR)
                return -1;

            thisPage += pageSize;
            numBytesProcessed += pageSize;
        }
    }
    frcmmuPpcCurrentSet(&frcmmuGlobalTransTbl1);
#endif /* CODE_FROM_VMBASELIB_C */
#endif /* CODE_FOR_CPU_1 */
    }

    return 0;
}
#endif /* PMC280_DUAL_CPU_PTE_SETUP */

#endif /* PMC280_DUAL_CPU */
