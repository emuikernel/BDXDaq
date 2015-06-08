/* loadElfLibP.h - private ELF loader library header */

/* Copyright 2001-2002 Wind River Systems, Inc. */
/*
modification history
--------------------
01g,16may02,fmk  move the definition of SYM_ADRS to loadLibP.h
01f,08feb02,jn   Add comments about CHECK_FITS and SIGN_EXTEND for ARM. 
01e,09jan02,jn   Reinstate previous definition of CHECK_FITS for ARM.  Keep new
                 definition for MIPS.
01d,03jan02,jn   put back LO_VALUE macro for PPC 
01c,27nov01,pad  Added macro definitions (SIGN_EXTEND, etc.)
01b,05sep01,jn   created from loadElfLib.h@@/main/3 - moved utility macros 
                 and private type and structure definitions here
*/

#ifndef __INCloadElfLibPh
#define __INCloadElfLibPh

#ifdef __cplusplus
extern "C" {
#endif

#include "symLib.h"
#include "loadLibP.h"

/* Defines */

#define CHECK_LOW14(val)        (((val & 0xffff8000) == 0) ||           \
                                 ((val & 0xffff8000) == 0xffff8000))

#define CHECK_LOW16(val)        (((val & 0xffff8000) == 0) ||           \
                                 ((val & 0xffff8000) == 0xffff8000))

#define CHECK_LOW16_STRICT(val) (((val & 0xffff0000) == 0) ||           \
                                 ((val & 0xffff0000) == 0xffff0000))

#define CHECK_LOW24(val)        (((val & 0xfe000000) == 0) ||           \
                                 ((val & 0xfe000000) == 0xfe000000))

#define INSERT(buf,value,mask) \
			((buf) = (((buf) & ~(mask)) | ((value) & (mask))))

#if   (CPU_FAMILY == PPC)
#define LOW24_INSERT(buf,value) INSERT ((buf), (value), 0x03fffffc)
#endif

#if   (CPU_FAMILY == ARM) 
#define LOW24_INSERT(buf,value)	INSERT ((buf), (value), 0x00ffffff)
#endif

#define LOW14_INSERT(buf,value) INSERT ((buf), (value), 0x0000fffc)

#define LOW16_VALUE(val)	((val) & 0x0000ffff)
#define LOW16_INSERT(buf,value)	INSERT ((buf), (value), 0x0000ffff)

#define LOW26_VALUE(val)	((val) & 0x03ffffff)
#define LOW26_INSERT(buf,value) INSERT ((buf), (value), 0x03ffffff)

#define LO_VALUE(val)           ((val) & 0x0000ffff)

#define HI_VALUE(val)           (((val) >> 16) & 0x0000ffff)

#define HA_VALUE(val)           ((((val) >> 16)+(((val) >> 15) & 0x1)) & 0xffff)

#define STORE_MASKED_VALUE(address, mask, value) \
    *(address) = ((*(address)) & ~(mask)) | ((value) & (mask))

/* macro to check if the alignment is a power of 2 */

#define CHECK_2_ALIGN(x)        (((x) & ((x) - 1)) == 0)

/* the binary number that is "n" 1's */

#define MASK(n)		((1 << (n)) - 1)

/* mask to get the most significant bit of a value on n bit */

#define SIGN_BIT_MASK(n) (1 << ((n) - 1))

/* XXX - jn - use of these macro names should be standardized across arches. */

/* 
 * XXX - Before they were added here, both CHECK_FITS and SIGN_EXTEND were 
 * used by ARM and defined in target/h/arch/arm/elfArm.h
 */

#if (CPU_FAMILY == MIPS)

/* Check whether the value fits in n bits */

#define CHECK_FITS(val,nBits) (((val) & ~(MASK(nBits))) == 0)

#elif (CPU_FAMILY == ARM)

/*
 * macro to check that a signed <val> fits in <nBits> without overflow
 * i.e. that the "sign-bit" is correctly propagated into the excess high bits
 */
   
#define CHECK_FITS(val,nBits) \
        ((((val) & ~((1 << ((nBits)-1))-1)) == 0) || \
         (((val) & ~((1 << ((nBits)-1))-1)) == ~(UINT32)((1 << ((nBits)-1))-1)))
#endif

/*
 * Sign extension macro. Sign extension replaces all higher bits in <val>
 * with copies of the most significant bit on <nBits>.
 *    <val> is the value we want to sign extend.
 *    <nBits> is the number of significant bits in <val>.
 *
 * Note: the assumption is that the storage is greater than <nBits>.
 */

#define SIGN_EXTEND(val, nBits) \
	(((MASK(nBits) & (val)) ^ SIGN_BIT_MASK(nBits)) - SIGN_BIT_MASK(nBits))

/*
 * Overflow verification macro. It checks that the signed value <val> does
 * not overflow the number of bits specified by <nBits>. The overflow for
 * signed binary numbers is defined as occuring if and only if at least one
 * of the bits beyond <nBits> is not equal to the sign bit of the value
 * truncated on <nBits>.
 *
 * The expression evaluation gives 1 when the value overflows, 0 otherwise.
 *
 * Note: the assumption is that the maximum storage is on 32 bits.
 */

#define CHECK_SIGNED_OVERFLOW(val,nBits) \
	(!(SIGN_EXTEND((val),(nBits)) == (val)))

/* Some masks */

#define RA_MSK          0x001f0000
#define GPR13_MSK       0x000d0000
#define GPR2_MSK        0x00020000

/* type definitions */

typedef void **         SYM_ADRS_TBL;           /* table of symbol's addr */
typedef void *          SCN_ADRS;               /* section's address */
typedef void **         SCN_ADRS_TBL;           /* table of section's addr */

/* data structures */

typedef struct
    {
    UINT32 *    pLoadScnHdrIdxs;        /* loadable sections header index tbl */
    UINT32 *    pSymTabScnHdrIdxs;      /* sym table sections header idx tbl */
    UINT32 *    pRelScnHdrIdxs;         /* reloc info sections header idx tbl */
    UINT32 *    pStrTabScnHdrIdxs;      /* str table sections header idx tbl */
    } IDX_TBLS;

typedef struct
    {
    void *      pAddrSdata;     /* address of SDA data segment */
    void *      pAddrSbss;      /* address of SDA bss segment */
    int         sizeSdata;      /* size of SDA data segment */
    int         sizeSbss;       /* size of SDA bss segment */
    int         flagsSdata;     /* SDA data flags for module */
    int         flagsSbss;      /* SDA bss flags for module */
    PART_ID     sdaMemPartId;   /* ID of memory partition dedicated for SDA */
    void *      sdaBaseAddr;    /* Base address of the SDA area */
    int         sdaAreaSize;    /* Size of the SDA area */
    void *      pAddrSdata2;    /* address of SDA2 data segment */
    void *      pAddrSbss2;     /* address of SDA2 bss segment */
    int         sizeSdata2;     /* size of SDA2 data segment */
    int         sizeSbss2;      /* size of SDA2 bss segment */
    int         flagsSdata2;    /* SDA2 data flags for module */
    int         flagsSbss2;     /* SDA2 bss flags for module */
    PART_ID     sda2MemPartId;  /* ID of memory partition dedicated for SDA2 */
    void *      sda2BaseAddr;   /* Base address of the SDA2 area */
    int         sda2AreaSize;   /* Size of the SDA2 area */
    } SDA_INFO;	

typedef struct
    {
    SYM_ADRS    pAddr;  /* symbol's address */
    SYM_TYPE    type;   /* symbol's type */
    } SYM_INFO;

typedef Elf32_Sym **    SYMTBL_REFS;    /* table of pointers to symbol tables */
typedef void ***        SYMADDR_REFS;   /* table of ptrs to sym adrs tables */
typedef SYM_INFO *      SYM_INFO_TBL;   /* table of symbol address/type info */
typedef SYM_INFO_TBL *  SYMINFO_REFS;   /* table of ptr to symbol info tables */

/* function declarations */

int    elfRelocRelEntryRd (int fd, int posRelocEntry, Elf32_Rel * pReloc);
int    elfRelocRelaEntryRd (int fd, int posRelocEntry, Elf32_Rela * pReloc);


/*
 * The following routines are actually located in the relocation units, but
 * we need the prototypes here to allow for a clean build.
 */

#if (CPU_FAMILY == I80X86)

STATUS elfI86Init 
    (
    FUNCPTR * pElfModuleVerifyRtn, 
    FUNCPTR * pElfRelSegRtn
    );

#elif (CPU_FAMILY == ARM)

STATUS elfArmInit 
    (
    FUNCPTR * pElfModuleVerifyRtn, 
    FUNCPTR * pElfRelSegRtn
    );

#elif (CPU_FAMILY == MIPS)

STATUS elfMipsInit 
    (
    FUNCPTR * pElfModuleVerifyRtn, 
    FUNCPTR * pElfRelSegRtn
    );

#endif

#ifdef __cplusplus
}
#endif

#endif /* __INCloadElfLibPh */
