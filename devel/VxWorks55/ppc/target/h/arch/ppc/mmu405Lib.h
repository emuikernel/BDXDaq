/* mmu405Lib.h - mmu library for PowerPc 405 series */

/* Copyright 1984-1997  Wind River Systems, Inc. */

/*
modification history
--------------------
01a,18jul00,sm	 written.
*/

#ifndef __INCmmuPpc405Libh
#define __INCmmuPpc405Libh

#ifdef __cplusplus
extern "C" {
#endif

#define MMU_I_ADDR_TRANS	0
#define MMU_D_ADDR_TRANS	1

#define MMU_INST		0x01
#define MMU_DATA		0x02

#define MMU_STATE_VALID				0x00000040
#define MMU_STATE_VALID_NOT			0x00000000
#define MMU_STATE_WRITABLE			0x00000100
#define MMU_STATE_WRITABLE_NOT			0x00000000
#define MMU_STATE_CACHEABLE			0x00000000
#define MMU_STATE_CACHEABLE_NOT			0x00000004
#define MMU_STATE_CACHEABLE_WRITETHROUGH	0x00000008
#define MMU_STATE_CACHEABLE_COPYBACK		0x00000000
#define MMU_STATE_GUARDED			0x00000001
#define MMU_STATE_GUARDED_NOT			0x00000000
#define MMU_STATE_EXECUTE			0x00000200
#define MMU_STATE_EXECUTE_NOT			0x00000000

#define MMU_STATE_MASK_VALID			0x00000040
#define MMU_STATE_MASK_WRITABLE			0x00000100
#define MMU_STATE_MASK_CACHEABLE    		0x0000000c
#define MMU_STATE_MASK_GUARDED			0x00000001
#define MMU_STATE_MASK_EXECUTE			0x00000200

#define MMU_STATE_MASK_WIMG_WRITABLE_EXECUTE	0x0000030d

#define MMU_LVL_2_DESC_NB			0x00000400

#define MMU_LVL_1_L2BA_MSK			0xfffffffc
#define MMU_RPN_SHIFT				0x0000000c

/* initial value of the ZPR. All zones are setup so that access is
 * controlled by EX and WR bits for both user & supervisor states.
 */
#define MMU_ZPR_INIT_VAL			0x55555555

/* sizes of Level 1 & Level 2 PTEs in powers of 2 */
#define MMU_LVL1_PTE_SHIFT			2
#define MMU_LVL2_PTE_SHIFT			3

/* Effective Address Definition */

typedef union effectiveAddr		/* effective Address structure */
    {
    struct
	{
	u_int l1index:10;		/* Level 1 Index */
	u_int l2index:10;		/* Level 2 Index */
	u_int po:12;			/* Page Offset */
	} field;
   
    void * effAddr;

    } EFFECTIVE_ADDR;

/* Real Address Definition */

typedef union	realAddress	/* Real Address Structure */
    {
    struct			/* Bit field description */
	{
	u_int rpn:20;		/* Real Page Number */
	u_int po:12;		/* Page Offset */
	} field;

    void * realAddr;		/* Real Address */

    } REAL_ADDRESS;	

/* Level 1 Descriptor Definition */

typedef union level_1_desc	/* Level 1 descriptor format */
    {
    struct			/* Bit field desciption */
	{
	u_int l2ba:30;		/* Level 2 table Base Address */
	u_int reserved:1;	/* Reserved */
	u_int v:1;		/* Segment Valid bit */
	} field;

    u_int l1desc;		/* Level 1 descriptor */

    } LEVEL_1_DESC;

/* Level 1 Table pointer definition */

typedef union	level_1_tbl_ptr	/* Level 1 Table pointer structure */
    {

    LEVEL_1_DESC * pL1Desc;	/* Level 1 descriptor table pointer */

    } LEVEL_1_TBL_PTR;

/******************************************************************************
*
* Level 2 descriptor definition 
* This stores most of the fields nneded to update a TLB entry. Note that
* we store only 20 bits of the epn, since we support a minimum page size
* of 4K only. Also zone protection is currently unimplemented as are
* variable page sizes.
*
*/

typedef union level_2_desc	/* Level 2 descriptor format */
    {
    struct			/* Bit field desciption */
	{
	/* hi word */
	u_int epn:20;		/* effective page number */
	u_int rsvd1:2;		/* reserved */
	u_int size:3;		/* page size (min 4k) */
	u_int v:1;		/* valid bit */
	u_int e:1;		/* little endian bit */
	u_int u0:1;		/* user attribute */
	u_int rsvd2:4;		/* reserved */

	/* lo word */
	u_int rpn:20;		/* real page number */
	u_int rsvd3:2;		/* reserved */
	u_int ex:1;		/* execute protection */
	u_int wr:1;		/* write protection */
	u_int zsel:4;		/* zone protection */
	u_int w:1;		/* write thru/back */
	u_int i:1;		/* cache inhibited */
	u_int m:1;		/* memory coherent */
	u_int g:1;		/* memory guarded  */

	} field;

    struct 
	{
	u_int word0;		/* hi word */
	u_int word1;		/* lo word */
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
    u_char		pid;	/* pid value associated with this map */
    } MMU_TRANS_TBL;

/* defines */

#define MMU_ADDR_MAP_ARRAY_SIZE   256
#define MMU_ADDR_MAP_ARRAY_START    1 		/* start from 1 */
#define MMU_ADDR_MAP_ARRAY_INV	   (MMU_TRANS_TBL *)-1	/* invalid entry */


#if defined(__STDC__) || defined(__cplusplus)

extern STATUS mmu405LibInit (int mmuType);

#else   /* __STDC__ */

extern STATUS mmu405LibInit ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCmmuPpc405Libh */
