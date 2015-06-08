/* mmu440Lib.h - mmu library for PowerPc 440 series */

/* Copyright 2001-2002  Wind River Systems, Inc. */

/*
modification history
--------------------
01a,17apr02,jtp	 written, based on mmu405Lib.h
*/

#ifndef __INCmmuPpc440Libh
#define __INCmmuPpc440Libh

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define N_TLB_ENTRIES		64		/* number of UTLB Entries */

#define MMU_PAGE_SIZE		0x1000		/* pages are 4K bytes */

#define MMU_I_ADDR_TRANS	0
#define MMU_D_ADDR_TRANS	1

#define MMU_INST		0x01
#define MMU_DATA		0x02

/*
 * MMU_STATE attributes
 *
 * Note that all of these map 1-1 with actual bit offsets in TLB Word 1, with
 * the exception of MMU_STATE_VALID and MMU_STATE_MASK_VALID -- which in the
 * actual hardware is stored in TLB Word 0 at a different bit position (see
 * MMU_STATE_VALID_SHIFT below).
 */

#define MMU_STATE_VALID				0x00000040
#define MMU_STATE_VALID_NOT			0x00000000
#define MMU_STATE_CACHEABLE			0x00000000
#define MMU_STATE_CACHEABLE_NOT			0x00000400
#define MMU_STATE_CACHEABLE_WRITETHROUGH	0x00000800
#define MMU_STATE_CACHEABLE_COPYBACK		0x00000000
#define MMU_STATE_ENDIAN_LITTLE			0x00000080
#define MMU_STATE_GUARDED			0x00000100
#define MMU_STATE_GUARDED_NOT			0x00000000
#define MMU_STATE_EXECUTE			0x00000004
#define MMU_STATE_EXECUTE_NOT			0x00000000
#define MMU_STATE_WRITABLE			0x00000002
#define MMU_STATE_WRITABLE_NOT			0x00000000

#define MMU_STATE_MASK_VALID			0x00000040
#define MMU_STATE_MASK_CACHEABLE    		0x00000c00
#define MMU_STATE_MASK_GUARDED			0x00000100
#define MMU_STATE_MASK_EXECUTE			0x00000004
#define MMU_STATE_MASK_WRITABLE			0x00000002

#ifdef _WRS_PPC440_MEM_COHERENCY_SUPPORT
#  define MMU_STATE_MEM_COHERENCY		0x00000200
#  define MMU_STATE_MEM_COHERENCY_NOT		0x00000000
#  define MMU_STATE_MASK_MEM_COHERENCY		0x00000200

#  define MMU_STATE_MASK_WIMG_WRITABLE_EXECUTE	0x00000f06
#else
#  define MMU_STATE_MASK_WIMG_WRITABLE_EXECUTE	0x00000d06
#endif /* _WRS_PPC440_MEM_COHERENCY_SUPPORT */

/* Valid bit in mmu_state is in different bit position and in TLB Word 0 */

#define MMU_STATE_VALID_SHIFT			3

/* Number of Level 2 descriptors in the lookup table = 1024 */

#define MMU_LVL_2_DESC_NB			0x00000400

/* Mask for base address in L2 descriptor */

#define MMU_LVL_1_L2BA_MSK			0xfffffffc

/* RPN field offset in TLB Word 0 (left shift this number of bits) */

#define MMU_RPN_SHIFT				12

/* sizes of Level 1 & Level 2 PTEs in powers of 2 */

#define MMU_LVL1_PTE_SHIFT			2	/* see LEVEL_1_DESC */
#define MMU_LVL2_PTE_SHIFT			4	/* see LEVEL_2_DESC */

/* address space array */

#define MMU_ADDR_MAP_ARRAY_SIZE		256
#define MMU_ADDR_MAP_ARRAY_START	1 		/* start from 1 */
#define MMU_ADDR_MAP_ARRAY_INV	   (MMU_TRANS_TBL *)-1	/* invalid entry */

/* TLB_ENTRY_DESC page attributes */

#define _MMU_TLB_TS_0		0x00000000	/* Translation Space 0 */
#define _MMU_TLB_TS_1		0x00010000	/* Translation Space 1 */

#define _MMU_TLB_SZ_1K		0x00000000	/* 1KB page size */
#define _MMU_TLB_SZ_4K		0x00000010	/* 4KB page size */
#define _MMU_TLB_SZ_16K		0x00000020	/* 16KB page size */
#define _MMU_TLB_SZ_64K		0x00000030	/* 64KB page size */
#define _MMU_TLB_SZ_256K	0x00000040	/* 256KB page size */
#define _MMU_TLB_SZ_1M		0x00000050	/* 1MB page size */
#define _MMU_TLB_SZ_16M		0x00000070	/* 16MB page size */
#define _MMU_TLB_SZ_256M	0x00000090	/* 256MB page size */
#define _MMU_TLB_SZ_MASK	0x000000f0	/* mask for page size field */
#define _MMU_TLB_SZ_SHIFT	4		/* offset of page size field */

#define _MMU_TLB_ATTR_W		0x00000800	/* writethrough */
#define _MMU_TLB_ATTR_I		0x00000400	/* inhibit caching */
#define _MMU_TLB_ATTR_G		0x00000100	/* guarded */

#define _MMU_TLB_PERM_X		0x00000004	/* executable */
#define _MMU_TLB_PERM_W		0x00000002	/* writable */


#ifndef _ASMLANGUAGE

/* Effective Address Definition */

typedef union effectiveAddr	/* Effective Address structure */
    {
    struct
	{
	UINT32 l1index:10;	/* Level 1 Index */
	UINT32 l2index:10;	/* Level 2 Index */
	UINT32 po:12;		/* Page Offset */
	} field;
   
    void * effAddr;

    } EFFECTIVE_ADDR;

/* Real Address Definition */

typedef union	realAddress	/* Real Address Structure */
    {
    struct			/* Bit field description */
	{
	UINT32 rpn:20;		/* Real Page Number */
	UINT32 po:12;		/* Page Offset */
	} field;

    void * realAddr;		/* Real Address */

    } REAL_ADDRESS;	

/* Level 1 Descriptor Definition */

typedef union level_1_desc	/* Level 1 descriptor format */
    {
    struct			/* Bit field desciption */
	{
	UINT32 l2ba:30;		/* Level 2 table Base Address */
	UINT32 reserved:1;	/* Reserved */
	UINT32 v:1;		/* Segment Valid bit */
	} field;

    UINT32 l1desc;		/* Level 1 descriptor */

    } LEVEL_1_DESC;

/* Level 1 Table pointer definition */

typedef union	level_1_tbl_ptr	/* Level 1 Table pointer structure */
    {

    struct			/* Bit field description */
	{
	UINT32 l1tb:20;		/* Level 1 table Base */
	UINT32 l1index:10;	/* Level 1 table Index */
	UINT32 reserved:2;	/* Reserved */
	} field;

    LEVEL_1_DESC * pL1Desc;	/* Level 1 descriptor table pointer */

    } LEVEL_1_TBL_PTR;

/******************************************************************************
*
* Level 2 descriptor definition 
* This stores most of the fields needed to update a TLB entry. Note that
* we store only 20 bits of the epn, since we support a minimum page size
* of 4K only. ERPN and variable page sizes are currently unimplemented.
*
* TLB fields are from ch. 6. PPC440GP User's Manual, version 7, March
* 2002
*
* TLB Word 0:
*
*  0               8              16        21    24            31
* +---------------.---------------.-----------+-+-+-------+-------+
* |                    EPN                    |V|T| SIZE  |-------|
* |                                           | |S|       |-------|
* +---------------'---------------'-----------+-+-+-------+-------+
*
* EPN  - Effective (Virtual) address Page Number
* V    - Valid page bit
* TS   - Translation address Space - mmu440Lib only manages
* 	 values of '1' -- that is, dynamic mmu page mapping only
*	 applies for MSR[IS,DS] values of 1. TS values of zero are
*	 set up in a static TLB mapping in the BSP.
* SIZE - Page Size - mmu440Lib maps pages of size MMU_PAGE_SIZE. Other
*	 sized pages are set up in static TLB mappings in the BSP.
*
* TLB Word 1:
*
*  0               8              16        21    24            31
* +---------------.---------------.-----------+---.-------+-------+
* |                    RPN                    |-----------| ERPN  |
* |                                           |-----------|       |
* +---------------'---------------'-----------+---'-------+-------+
*
* RPN  - Real (Physical) address Page Number
* ERPN - Extended Real Page Number - always zero, due to vxWorks'
*	 design limitation that physical addresses greater than 32
*	 bits cannot be managed by vmBaseLib and vmLib.  In PPC440,
*	 devices at higher physical addresses are reached by setting
*	 up a static TLB mapping in the BSP.
*
* TLB Word 2:
*
*  0               8              16              24            31
* +---------------.---------------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* |               0               |U|U|U|U|W|I|M|G|E|-|U|U|U|S|S|S|
* |                               |0|1|2|3| | | | | |-|X|W|R|X|W|R|
* +---------------'---------------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*
* 0     - Field of zeroes
* U0-U3	- Unused by mmu440Lib and set to zero
* W    	- Write-Through
* I    	- Caching Inhibited
* M    	- Unused by mmu440Lib and set to zero
* G    	- Guarded
* E,UX,UW,UR
*	- Unused by mmu440Lib and set to zero
* SX    - Supervisor eXecute
* SW    - Supervisor Write
* SR    - Supervisor Read - always 1.
*/

typedef union level_2_desc	/* Level 2 descriptor format */
    {
    struct			/* Bit field desciption */
	{
	/* word 0 */
	UINT32 epn:20;		/* effective page number */
	UINT32 rsvd1:2;		/* reserved */
	UINT32 v:1;		/* valid bit */
	UINT32 ts:1;		/* translation space bit */
	UINT32 size:4;		/* page size */
	UINT32 rsvd2:4;		/* reserved */

	/* word 1 */
	UINT32 rpn:20;		/* real page number */
	UINT32 rsvd3:8;		/* reserved */
	UINT32 erpn:4;		/* extended real page number */

	/* word 2 */
	UINT32 rsvd4:16;	/* reserved */
	UINT32 u0:1;		/* user attribute 0 */
	UINT32 u1:1;		/* user attribute 1 */
	UINT32 u2:1;		/* user attribute 2 */
	UINT32 u3:1;		/* user attribute 3 */
	UINT32 w:1;		/* write thru/back */
	UINT32 i:1;		/* cache inhibited */
	UINT32 m:1;		/* memory coherent */
	UINT32 g:1;		/* memory guarded  */
	UINT32 e:1;		/* little endian bit */
	UINT32 rsvd5:1;		/* reserved */
	UINT32 ux:1;		/* user execute protection */
	UINT32 uw:1;		/* user write protection */
	UINT32 ur:1;		/* user read protection */
	UINT32 sx:1;		/* supervisor execute protection */
	UINT32 sw:1;		/* supervisor write protection */
	UINT32 sr:1;		/* supervisor read protection */

	UINT32 rsvd6:32;	/* pad descriptor table to power of 2 */

	} field;

    struct 
	{
	UINT32 word0;		/* word 0 */
	UINT32 word1;		/* word 1 */
	UINT32 word2;		/* word 2 */
	UINT32 pad1;		/* pad word */
	} words;

    } LEVEL_2_DESC;

/* Level 2 Table pointer definition */

typedef union	level_2_tbl_ptr	/* Level 2 Table pointer structure */
    {

    LEVEL_2_DESC * pL2Desc;	/* Level 2 descriptor table pointer */ 

    } LEVEL_2_TBL_PTR;

/* Translation Table Definition */

typedef struct mmuTransTblStruct
    {
    LEVEL_1_TBL_PTR	l1TblPtr;
    UINT8		pid;	/* pid value associated with this map */
    } MMU_TRANS_TBL;

/* TLB Entry description, used for mmu initialization */

typedef struct
    {
    UINT32	effAddr;	/* Effective address of base of page */
    UINT32	realAddrExt;	/* top 4 bits of real address */
    UINT32	realAddr;	/* rest of real address */
    UINT32	attr;		/* page attributes */
    } TLB_ENTRY_DESC;


#if defined(__STDC__) || defined(__cplusplus)

extern STATUS mmu440LibInit (int mmuType, int staticTlbNum,
			     TLB_ENTRY_DESC * pStaticTlbDesc);

#else   /* __STDC__ */

extern STATUS mmu440LibInit ();

#endif  /* __STDC__ */

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCmmuPpc440Libh */
