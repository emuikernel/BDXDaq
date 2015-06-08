/* dshppa.h - simhppa disassembler library header */

/* Copyright 1984-1993 Wind River Systems, Inc. */
/*
modification history
--------------------
01a,05jan96,kab  changed INSN_TAB and deleted DSM_TAB
*/

#ifndef __dsmhppah
#define __dsmhppah

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PARISC_HPUX9
#include "wtxtclp.h"
#else
#include "vwModNum.h"

/* XXX p_m should find a better error code here */

#define S_dsmLib_UNKNOWN_INSTRUCTION    (0x20000 | 1)

#endif /*PARISC_HPUX9*/

/* macros for bit mask and shift for operation field */

/* ?!?BUG -- len is never used, presumably it's a width?!? */
#define BIT_MASK(p,len)  ((1<<(32-(p)))-1)
#define BIT_SHIFT(p,len) (32 -(p) - (len))

#define IMM5_MASK	0x0000001f
#define	IMM5_13_MASK	BIT_MASK(5, 13)
#define	BIT27_5_MASK	BIT_MASK(27, 5)
#define	BIT19_8_MASK	BIT_MASK(19, 8)
#define	BIT_M_N_MASK	0x00000020
#define	BIT6_5_MASK	BIT_MASK(6, 5)
#define	BIT11_5_MASK	BIT_MASK(11, 5)
#define	BIT12_1_MASK	BIT_MASK(12, 1)
#define	BIT13_3_MASK	BIT_MASK(13, 3)
#define	BIT16_2_MASK	BIT_MASK(16, 2)
#define	BIT16_3_MASK	BIT_MASK(16, 3)
#define BIT16_5_MASK	BIT_MASK(16, 5)
#define BIT21_2_MASK	BIT_MASK(21, 2)
#define BIT23_3_MASK	BIT_MASK(23, 3)

/* operation code extention definitions */

/* load indexed instructions */

#define EXT_LDWX_CODE	0x02
#define EXT_LDHX_CODE	0x01
#define EXT_LDBX_CODE	0x00
#define EXT_LDWAX_CODE	0x06
#define EXT_LDWAS_CODE	0x06
#define EXT_LDCWX_CODE	0x07
#define EXT_STWS_CODE 	0x0a
#define EXT_STHS_CODE	0x09
#define EXT_STBS_CODE	0x08
#define EXT_STWAS_CODE	0x0e
#define EXT_STBYS_CODE	0x0c

/* system control instructions */

#define EXT_SYSCTL_MASK		0x00001fe0
#define EXT_BREAK_CODE		0
#define EXT_RFI_CODE		0x60
#define EXT_RFIR_CODE		0x65
#define EXT_SSM_CODE		0x6b
#define EXT_RSM_CODE		0x73
#define EXT_MTSM_CODE		0xc3
#define EXT_LDSID_CODE		0x85
#define EXT_MTSP_CODE		0xc1
#define EXT_MTCTL_CODE		0xc2
#define EXT_MFSP_CODE		0x25
#define EXT_MFCTL_CODE		0x45
#define EXT_SYNC_CODE		0x20
#define EXT_PROBER_CODE		0x46
#define EXT_PROBERI_CODE	0xc6
#define EXT_PROBEW_CODE		0x47
#define EXT_PROBEWI_CODE	0xc7
#define EXT_LPA_CODE		0x4d

/* memory managemnet instructions */

#define EXT_PDTLB_CODE		0x48
#define EXT_PDTLBE_CODE		0x49
#define EXT_PDC_CODE		0x4e
#define EXT_FDC_CODE		0x4a
#define EXT_FDCE_CODE		0x4b
#define EXT_PITLB_CODE		0x08
#define EXT_PITLBE_CODE		0x09
#define EXT_FIC_CODE		0x0a
#define EXT_FICE_CODE		0x0b
#define EXT_IDTLBA_CODE		0x41
#define EXT_IDTLBP_CODE		0x40
#define EXT_IITLBA_CODE		0x01
#define EXT_IITLBP_CODE		0x00

/* memory managemnet instructions */

#define EXT_PDTLB_CODE		0x48
#define EXT_PDTLBE_CODE		0x49
#define EXT_PDC_CODE		0x4e
#define EXT_FDC_CODE		0x4a
#define EXT_FDCE_CODE		0x4b
#define EXT_PITLB_CODE		0x08
#define EXT_PITLBE_CODE		0x09
#define EXT_FIC_CODE		0x0a
#define EXT_FICE_CODE		0x0b
#define EXT_IDTLBA_CODE		0x41
#define EXT_IDTLBP_CODE		0x40
#define EXT_IITLBA_CODE		0x01
#define EXT_IITLBP_CODE		0x00

/* deposit instructions */

#define EXT_VDEP_CODE 		1
#define EXT_ZVDEP_CODE 		0
#define EXT_DEP_CODE 		3
#define EXT_ZDEP_CODE 		2
#define EXT_VDEPI_CODE 		5
#define EXT_ZVDEPI_CODE 	4
#define EXT_DEPI_CODE		7
#define EXT_ZDEPI_CODE		6

/* floating point load/store indexed instructions */

#define EXT_FLDWX_CODE	0x00
#define EXT_FLDDX_CODE	0x00
#define EXT_FLDWS_CODE	0x08
#define EXT_FLDDS_CODE	0x08
#define EXT_FSTWX_CODE 	0x01
#define EXT_FSTDX_CODE	0x01
#define EXT_FSTWS_CODE	0x09
#define EXT_FSTDS_CODE	0x09

/* load and store coprocessor instructions */

#define EXT_CLDX_CODE 0
#define EXT_CSTX_CODE 1
#define EXT_CLDS_CODE 8
#define EXT_CSTS_CODE 9

/* extract and deposit instructions */

#define EXT_VSHD_CODE 	0
#define EXT_SHD_CODE	2 
#define EXT_VEXTRU_CODE 4
#define EXT_VEXTRS_CODE 5
#define EXT_EXTRU_CODE  6
#define EXT_EXTRS_CODE  7

typedef struct 
    {
    UINT32 mask;	/* bit mask for operation code */
    UINT32 opCode;	/* operation code */
    char *name;		/* instruction name */
    int flags;		/* nullification? check cond flag? */
    VOIDFUNCPTR pFunc;      /* disassembler function */
    } INST_TAB;

#if defined(__STDC__) || defined(__cplusplus)

#ifdef PARISC_HPUX9
IMPORT	  int	dsmHppaInst (UINT32 *binInst, int address,
			     void (*prtAddress)(), Tcl_DString *pDString);
IMPORT	  int	dsmHppaNbytes (UINT32 *binInst);
#else
IMPORT	  int	dsmInst (UINT32 *binInst, int address,
			     void (*prtAddress)());
IMPORT	  int	dsmNbytes (UINT32 *binInst);
#endif /*PARISC_HPUX9 */
#else

#ifdef PARISC_HPUX9
IMPORT	  int	dsmHppaInst ();
IMPORT	  int	dsmHppaNbytes ();
#else
IMPORT	  int	dsmHppaInst ();
IMPORT	  int	dsmHppaNbytes ();
#endif /*PARISC_HPUX9*/

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCdsmhppah */
