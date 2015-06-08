/* mmuPpcLib.h - mmu libarby for PowerPc */

/* Copyright 1984-1995  Wind River Systems, Inc. */

/*
modification history
--------------------
01a,11sep95,tpr	 written.
*/

#ifndef __INCmmuPpcLibh
#define __INCmmuPpcLibh

#ifdef __cplusplus
extern "C" {
#endif

#define MMU_SDR1_HTABORG_MASK	0xffff0000	/* HTABORG mask */
#define MMU_SDR1_HTABORG_8M	0xffff0000	/* HTABORG value for 8M */
#define MMU_SDR1_HTABORG_16M	0xfffe0000	/* HTABORG value for 16M */
#define MMU_SDR1_HTABORG_32M	0xfffc0000	/* HTABORG value for 32M */
#define MMU_SDR1_HTABORG_64M	0xfff80000	/* HTABORG value for 64M */
#define MMU_SDR1_HTABORG_128M	0xfff00000	/* HTABORG value for 128M */
#define MMU_SDR1_HTABORG_256M	0xffe00000	/* HTABORG value for 256M */
#define MMU_SDR1_HTABORG_512M	0xffc00000	/* HTABORG value for 512M */
#define MMU_SDR1_HTABORG_1G	0xff800000	/* HTABORG value for 1G */
#define MMU_SDR1_HTABORG_2G	0xff000000	/* HTABORG value for 2G */
#define MMU_SDR1_HTABORG_4G	0xfe000000	/* HTABORG value for 4G */

#define MMU_SDR1_HTABMASK_MASK	0x000001ff	/* HTABMASK mask */
#define MMU_SDR1_HTABMASK_SHIFT	0x00000010	/* HTABMASK shift */
#define MMU_SDR1_HTABMASK_8M	0x00000000	/* HTABMASK value for 8M */
#define MMU_SDR1_HTABMASK_16M	0x00000001	/* HTABMASK value for 16M */
#define MMU_SDR1_HTABMASK_32M	0x00000003	/* HTABMASK value for 32M */
#define MMU_SDR1_HTABMASK_64M	0x00000007	/* HTABMASK value for 64M */
#define MMU_SDR1_HTABMASK_128M	0x0000000f	/* HTABMASK value for 128M */
#define MMU_SDR1_HTABMASK_256M	0x0000001f	/* HTABMASK value for 256M */
#define MMU_SDR1_HTABMASK_512M	0x0000003f	/* HTABMASK value for 512M */
#define MMU_SDR1_HTABMASK_1G	0x0000007f	/* HTABMASK value for 1G */
#define MMU_SDR1_HTABMASK_2G	0x000000ff	/* HTABMASK value for 2G */
#define MMU_SDR1_HTABMASK_4G	0x000001ff	/* HTABMASK value for 4G */

#define MMU_PTE_MIN_SIZE_8M	0x00010000	/*  64k min size */ 
#define MMU_PTE_MIN_SIZE_16M	0x00020000	/* 128k min size */ 
#define MMU_PTE_MIN_SIZE_32M	0x00040000	/* 256k min size */ 
#define MMU_PTE_MIN_SIZE_64M	0x00080000	/* 512k min size */ 
#define MMU_PTE_MIN_SIZE_128M	0x00100000	/*   1M min size */ 
#define MMU_PTE_MIN_SIZE_256M	0x00200000	/*   2M min size */ 
#define MMU_PTE_MIN_SIZE_512M	0x00400000	/*   4M min size */ 
#define MMU_PTE_MIN_SIZE_1G	0x00800000	/*   8M min size */ 
#define MMU_PTE_MIN_SIZE_2G	0x01000000	/*  16M min size */ 
#define MMU_PTE_MIN_SIZE_4G	0x02000000	/*  32M min size */ 

#define MMU_EA_SR		0xf0000000	/* SR field in E.A. */
#define MMU_EA_SR_SHIFT		0x0000001c	/* */
#define MMU_EA_PAGE_INDEX	0x0ffff000	/* Page Index in E.A. */
#define MMU_EA_PAGE_INDEX_SHIFT	0x0000000c	/* Page Index shift */
#define MMU_EA_BYTE_OFFSET	0x00000fff	/* Byte Offset in E.A */
#define MMU_EA_API		0x0000fc00	/* abbreviated page index */
#define MMU_EA_API_SHIFT	0x0000000a	/* api shift in page index */

#define MMU_SR_VSID_MASK	0x00ffffff	/* virtual segment ID */
#define MMU_SR_NB_SHIFT		0x0000001c

#define MMU_VSID_PRIM_HASH	0x000fffff	/* primary hash value in VSID */

#define MMU_HASH_VALUE_LOW	0x000003ff

#define MMU_HASH_VALUE_HIGH	0x0007fc00

#define MMU_HASH_VALUE_HIGH_SHIFT 0x0a

#define MMU_PTE_HASH_VALUE_HIGH_SHIFT	0x10
#define MMU_PTE_HASH_VALUE_LOW_SHIFT	0x06

#define MMU_PTE_VSID_SHIFT	0x07
#define MMU_PTE_V		0x80000000
#define MMU_PTE_H		0x00000040
#define MMU_PTE_RPN		0xfffff000
#define MMU_PTE_RPN_SHIFT	0x0000000c

#define MMU_PTE_BY_PTEG			0x08

#define MMU_I_ADDR_TRANS	0
#define MMU_D_ADDR_TRANS	1

#define MMU_INST		0x01
#define MMU_DATA		0x02

#define MMU_STATE_VALID				0x80000000
#define MMU_STATE_VALID_NOT			0x00000000
#define MMU_STATE_WRITABLE			0x00000002
#define MMU_STATE_WRITABLE_NOT			0x00000003
#define MMU_STATE_CACHEABLE			0x00000000
#define MMU_STATE_CACHEABLE_COPYBACK		0x00000000
#define MMU_STATE_CACHEABLE_WRITETHROUGH	0x00000040
#define MMU_STATE_CACHEABLE_NOT			0x00000020
#define MMU_STATE_MEM_COHERENCY			0x00000010
#define MMU_STATE_MEM_COHERENCY_NOT		0x00000000
#define MMU_STATE_GUARDED			0x00000008
#define MMU_STATE_GUARDED_NOT			0x00000000

/* 
 * the 3 following mask are not supported by the MMU for now
 */
#define MMU_STATE_ACCESS_NOT			0x00000000
#define MMU_STATE_EXECUTE			0x00000000
#define MMU_STATE_EXECUTE_NOT			0x10000000

#define MMU_STATE_MASK_VALID			0x80000000
#define MMU_STATE_MASK_WRITABLE			0x00000003
#define MMU_STATE_MASK_CACHEABLE		0x00000060
#define MMU_STATE_MASK_MEM_COHERENCY		0x00000010
#define MMU_STATE_MASK_GUARDED			0x00000008
#define MMU_STATE_MASK_EXECUTE			0x10000000
#define MMU_STATE_MASK_WIMG_AND_WRITABLE	0x0000007B

#define MMU_PPC_PAGE_SIZE			0x00001000

typedef union pte		/* PTE structure */
    {
    struct			/* bit field desciption */
	{
	u_int v:1;		/* entry valid (v=1) or invalid (v=0) */
	u_int vsid:24;		/* Virtual Segment ID */
	u_int h:1;		/* Hash function identifier */
	u_int api:6;		/* Abbreviated page index */
	u_int rpn:20;		/* Physical page number */
	u_int reserved1:3;	/* reserved */
	u_int r:1;		/* Referenced bit */
	u_int c:1;		/* Changed bit */
	u_int wimg:4;		/* Memory/cache control bit */
	u_int reserved2:1;	/* reserved */
	u_int pp:2;		/* Page protection bits */
	} field;
    struct			/* word description */
	{
	u_int word0;		/* word 0 */
	u_int word1;		/* word 1 */
	} bytes;
    } PTE;

typedef struct pteg		/* PTEG structure */
    {
    PTE pte[MMU_PTE_BY_PTEG];
    } PTEG;

typedef struct mmuTransTblStruct
    {
    u_int	hTabOrg;
    u_int	hTabMask;
    u_int	pteTableSize;
    } MMU_TRANS_TBL;

typedef union sr
    {
    struct
	{
	u_int t:1;			/* SR format flag */
	u_int ks:1;			/* Supervisor-state protection key */
	u_int kp:1;			/* User-state protection key */
	u_int n:1;			/* No-execute protection */
	u_int reserved:4;		/* reserved */
	u_int vsid:24;			/* Virtual segment Id */
	} bit;
    u_int value;			/* SR value */
    } SR;

typedef struct ea
    {
    u_int srSelect:4;		/* SR select */
    u_int pageIndex:16;		/* Page index */
    u_int byteOffset:12;	/* Byte offset */
    } EA;

void mmuPpcInstMissHandler (void);
void mmuPpcDataLoadMissHandler (void);
void mmuPpcDataStoreMissHandler (void);
#ifdef __cplusplus
}
#endif

#endif /* __INCmmuPpcLibh */
