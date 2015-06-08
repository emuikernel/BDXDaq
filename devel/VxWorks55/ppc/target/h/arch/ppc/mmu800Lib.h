/* mmuPpc800Lib.h - mmu library for PowerPc 800 serie */

/* Copyright 1984-1997  Wind River Systems, Inc. */

/*
modification history
--------------------
01b,05feb97,tpr  removed reserved macro (SPR 7783).
01a,27apr96,tpr	 written.
*/

#ifndef __INCmmuPpc800Libh
#define __INCmmuPpc800Libh

#ifdef __cplusplus
extern "C" {
#endif

#define MMU_I_ADDR_TRANS	0
#define MMU_D_ADDR_TRANS	1

#define MMU_INST		0x01
#define MMU_DATA		0x02

#define MMU_STATE_VALID				0x00000001
#define MMU_STATE_VALID_NOT			0x00000000
#define MMU_STATE_WRITABLE			0x00000002
#define MMU_STATE_WRITABLE_NOT			0x00000000
#define MMU_STATE_CACHEABLE			0x00000004
#define MMU_STATE_CACHEABLE_NOT			0x00000000
#define MMU_STATE_CACHEABLE_WRITETHROUGH	0x00000008
#define MMU_STATE_CACHEABLE_COPYBACK		0x00000000
#define MMU_STATE_GUARDED			0x00000010
#define MMU_STATE_GUARDED_NOT			0x00000000

#define MMU_STATE_MASK_VALID			0x00000001
#define MMU_STATE_MASK_WRITABLE			0x00000002
#define MMU_STATE_MASK_CACHEABLE		0x0000000c
#define MMU_STATE_MASK_GUARDED			0x00000010

#define MMU_LVL_2_DESC_NB			0x00000400

#define MMU_LVL_1_L2BA_MSK			0xfffff000
#define MMU_RPN_SHIFT				0x0000000c

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
	}field;

    void * realAddr;		/* Real Address */

    } REAL_ADDRESS;	

/* Level 1 Descriptor Definition */

typedef union level_1_desc	/* Level 1 descriptor format */
    {
    struct			/* Bit field desciption */
	{
	u_int l2ba:20;		/* Level 2 table Base Address */
	u_int reserved:3;	/* Reserved */
	u_int apg:4;		/* Access Protection Group */
	u_int g:1;		/* Guarded storage attribute for entry */
	u_int ps:2;		/* Page Size level one */
	u_int wt:1;		/* Write Through attribute for entry */
	u_int v:1;		/* Segment Valid bit */
	} field;

    u_int l1desc;		/* Level 1 descriptor */

    } LEVEL_1_DESC;

/* Level 1 Table pointer definition */

typedef union	level_1_tbl_ptr	/* Level 1 Table pointer structure */
    {
    struct			/* Bit field description */
	{
	u_int l1tb:20;		/* Level 1 table Base */
	u_int l1index:10;	/* Level 1 table Index */
	u_int reserved:2;	/* Reserved */
	} field;		

    LEVEL_1_DESC * pL1Desc;	/* Level 1 descriptor table pointer */

    } LEVEL_1_TBL_PTR;

/* Level 2 descriptor definition */

typedef union level_2_desc	/* Level 2 descriptor format */
    {
    struct			/* Bit field desciption */
	{
	u_int rpn:20;		/* Real Page Number */
	u_int pp:2;		/* Page Protection */
	u_int ppe:1;		/* Page Protection encoding */
	u_int c:1;		/* Change bit */
	u_int spv:4;		/* Sub Page Valid  */
	u_int sps:1;		/* Small Page Size */
	u_int sh:1;		/* Shared Page */
	u_int ci:1;		/* Cache Inhibit */
	u_int v:1;		/* Page Valid bit */
	} field;

    u_int l2desc;		/* Level 2 descriptor */

    } LEVEL_2_DESC;

/* Level 2 Table pointer definition */

typedef union	level_2_tbl_ptr	/* Level 2 Table pointer structure */
    {
    struct			/* Bit field description */
	{
	u_int l2tb:20;		/* Level 2 Table Base */
	u_int l2index:10;	/* Level 2 table Index */
	u_int reserved:2;	/* Reserved */
	} field;		

    LEVEL_2_DESC * pL2Desc;	/* Level 2 descriptor table pointer */ 

    } LEVEL_2_TBL_PTR;

/* Translation Table Definition */

typedef struct mmuTransTblStruct
    {
    LEVEL_1_TBL_PTR	l1TblPtr;
    } MMU_TRANS_TBL;

/* defines */

#define rpn		field.rpn		/* Real Page Number field */
#define po		field.po		/* Page offset field */

#define l1tb		field.l1tb		/* Level 1 table Base field */
#define l1index 	field.l1index		/* Level 1 table Index field */

#define l2tb		field.l2tb		/* Level 2 table Base field */
#define l2index		field.l2index		/* Level 2 table Index field */

#define l2ba		field.l2ba		/* Level 2 table Base Address */
#define	apg		field.apg		/* Access Protection Group */
#define g		field.g			/* Guarded storage attribute */
#define ps		field.ps		/* Page Size level one field */
#define wt		field.wt		/* Write Through attribute */
#define	v		field.v			/* Segment Valid field */

#define pp		field.pp		/* Page Protection field */
#define ppe		field.ppe		/* Page Protection encoding */
#define c		field.c			/* Change field */
#define spv		field.spv		/* Sub Page Valid field */
#define sps		field.sps		/* Small Page Size field */
#define sh		field.sh		/* Shared Page field */
#define ci		field.ci		/* Cache Inhibit field */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS mmu800LibInit (int mmuType);

#else   /* __STDC__ */

extern STATUS mmu800LibInit ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCmmuPpc800Libh */
