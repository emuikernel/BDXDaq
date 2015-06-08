/* mmu601Lib.h - mmuLib header for PowerPC 601 */

/* Copyright 1984-1994 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,07oct95,tpr  changed _MMI_NUM_BAT by _MMU_NUM_BAT.
01a,03aug94,caf  written.
*/

#ifndef _INCmmu601Libh
#define _INCmmu601Libh

#ifdef __cplusplus
extern "C" {
#endif

#define PAGE_SIZE 4096

#define _MMU_NUM_BAT    4                       /* 4 instr+data BAT regs     */

/*****************************************************************************
* _MMU_UBAT                     Upper BAT Registers                          *
******************************************************************************/

#define _MMU_UBAT_BLPI_MASK     0xfffe0000      /*                           */
#define _MMU_UBAT_BLPI_SHIFT    17

#define _MMU_UBAT_WIM_MASK      0x00000070      /* memory/cache access mode  */
#define _MMU_UBAT_WIM_SHIFT     4
#define _MMU_UBAT_WRITE_THROUGH 0x00000040      /* W - write-through         */
#define _MMU_UBAT_CACHE_INHIBIT 0x00000020      /* I - caching-inhibited     */
#define _MMU_UBAT_MEM_COHERENT  0x00000010      /* M - memory coherence      */

#define _MMU_UBAT_KS            0x00000008      /* supervisor key            */

#define _MMU_UBAT_KU            0x00000004      /* user key                  */

#define _MMU_UBAT_PP_MASK       0x00000003      /* define access options     */
#define _MMU_UBAT_PP_SHIFT      0
#define _MMU_UBAT_PP_00         0x00000000      /* PP bits = 00              */
#define _MMU_UBAT_PP_01         0x00000001      /* PP bits = 01              */
#define _MMU_UBAT_PP_10         0x00000002      /* PP bits = 10              */
#define _MMU_UBAT_PP_11         0x00000003      /* PP bits = 11              */

/*****************************************************************************
* _MMU_LBAT                     Lower BAT Registers                          *
******************************************************************************/

#define _MMU_LBAT_PBN_MASK      0xfffe0000      /* physical block number     */
#define _MMU_LBAT_PBN_SHIFT     17

#define _MMU_LBAT_V             0x00000040      /* valid                     */

#define _MMU_LBAT_BSM_MASK      0x0000003f      /* block size mask           */
#define _MMU_LBAT_BSM_SHIFT     0
#define _MMU_LBAT_BSM_128K      0x00000000      /* block size 128K           */
#define _MMU_LBAT_BSM_256K      0x00000001      /* block size 256K           */
#define _MMU_LBAT_BSM_512K      0x00000003      /* block size 512K           */
#define _MMU_LBAT_BSM_1M        0x00000007      /* block size 1M             */
#define _MMU_LBAT_BSM_2M        0x0000000f      /* block size 2M             */
#define _MMU_LBAT_BSM_4M        0x0000001f      /* block size 4M             */
#define _MMU_LBAT_BSM_8M        0x0000003f      /* block size 8M             */

/*****************************************************************************
* _MMU_SEG                      Segment Descriptors                          *
******************************************************************************/

#define _MMU_SEG_T              0x80000000      /* T=0 for page, T=1 for I/O */

#define _MMU_SEG_KS             0x40000000      /* supervisor key      (T=0) */

#define _MMU_SEG_KU             0x20000000      /* user key            (T=0) */

#define _MMU_SEG_VSID_MASK      0x00ffffff      /* virtual segment ID  (T=0) */
#define _MMU_SEG_VSID_SHIFT     0

/*****************************************************************************
* _MMU_PTE                      Page Table Entries                           *
******************************************************************************/

#define _MMU_PTE0_V             0x80000000      /* valid                     */

#define _MMU_PTE0_VSID_MASK     0x7fffff80      /* virtual segment ID        */
#define _MMU_PTE0_VSID_SHIFT    7

#define _MMU_PTE0_H             0x00000040      /* hash function identifier  */

#define _MMU_PTE0_API_MASK      0x0000003f      /* abbreviated page index    */
#define _MMU_PTE0_API_SHIFT     0

#define _MMU_PTE1_PPN_MASK      0xfffff000      /* physical page number      */
#define _MMU_PTE1_PPN_SHIFT     12

#define _MMU_PTE1_R             0x00000100      /* referenced                */

#define _MMU_PTE1_C             0x00000080      /* changed                   */

#define _MMU_PTE1_WIM_MASK      0x00000070      /* memory/cache control      */
#define _MMU_PTE1_WIM_SHIFT     4
#define _MMU_PTE1_WRITE_THROUGH 0x00000040      /* W - write-through         */
#define _MMU_PTE1_CACHE_INHIBIT 0x00000020      /* I - caching-inhibited     */
#define _MMU_PTE1_MEM_COHERENT  0x00000010      /* M - memory coherence      */

#define _MMU_PTE1_PP_MASK       0x00000003      /* page protection           */
#define _MMU_PTE1_PP_SHIFT      0
#define _MMU_PTE1_PP_00         0x00000000      /* PP bits = 00              */
#define _MMU_PTE1_PP_01         0x00000001      /* PP bits = 01              */
#define _MMU_PTE1_PP_10         0x00000002      /* PP bits = 10              */
#define _MMU_PTE1_PP_11         0x00000003      /* PP bits = 11              */

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS mmu601LibInit (int pageSize);

#else   /* __STDC__ */

extern STATUS mmu601LibInit ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* _INCmmu601Libh */
