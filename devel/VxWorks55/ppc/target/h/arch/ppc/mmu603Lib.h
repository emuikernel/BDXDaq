/* mmu603Lib.h - mmuLib header for PowerPC 603 */

/* Copyright 1984-1994 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,29apr02,pcs  Add defines for the EXTRA BAT support on MPC755/MPC7455.
01d,29sep98,fle  doc : made it refgen parsable
01c,12may95,caf  removed bogus typedef.
01b,15aug94,caf  revised, changed name to mmu603Lib.h.
01a,21apr94,yao  written.
*/

#ifndef _INCmmu603Libh
#define _INCmmu603Libh

#ifdef __cplusplus
extern "C" {
#endif


#define PAGE_SIZE       4096                    /* pages are 4K bytes        */

#define _MMU_NUM_IBAT   4                       /* 4 instr BAT registers     */
#define _MMU_NUM_DBAT   4                       /* 4 data BAT registers      */

#define _MMU_NUM_EXTRA_IBAT   4                       /* 4 instr BAT registers     */
#define _MMU_NUM_EXTRA_DBAT   4                       /* 4 data BAT registers      */


/* _MMU_UBAT                     Upper BAT Registers                         */

#define _MMU_UBAT_BEPI_MASK     0xfffe0000      /* effective pg index        */
#define _MMU_UBAT_BEPI_SHIFT    17

#define _MMU_UBAT_BL_MASK       0x00001ffc      /* block size                */
#define _MMU_UBAT_BL_SHIFT      2
#define _MMU_UBAT_BL_128K       0x00000000      /* block size 128K           */
#define _MMU_UBAT_BL_256K       0x00000004      /* block size 256K           */
#define _MMU_UBAT_BL_512K       0x0000000c      /* block size 512K           */
#define _MMU_UBAT_BL_1M         0x0000001c      /* block size 1M             */
#define _MMU_UBAT_BL_2M         0x0000003c      /* block size 2M             */
#define _MMU_UBAT_BL_4M         0x0000007c      /* block size 4M             */
#define _MMU_UBAT_BL_8M         0x000000fc      /* block size 8M             */
#define _MMU_UBAT_BL_16M        0x000001fc      /* block size 16M            */
#define _MMU_UBAT_BL_32M        0x000003fc      /* block size 32M            */
#define _MMU_UBAT_BL_64M        0x000007fc      /* block size 64M            */
#define _MMU_UBAT_BL_128M       0x00000ffc      /* block size 128M           */
#define _MMU_UBAT_BL_256M       0x00001ffc      /* block size 256M           */

#define _MMU_UBAT_BL_512M       0x00003ffc      /* block size 512M           */
#define _MMU_UBAT_BL_1G         0x00007ffc      /* block size 1G             */
#define _MMU_UBAT_BL_2G         0x0000fffc      /* block size 2G             */
#define _MMU_UBAT_BL_4G         0x0001fffc      /* block size 4G             */

#define _MMU_UBAT_VS            0x00000002      /* supervisor mode valid bit */

#define _MMU_UBAT_VP            0x00000001      /* user mode valid bit       */

/* _MMU_LBAT                     Lower BAT Registers                         */

#define _MMU_LBAT_BRPN_MASK     0xfffe0000      /* physical block number     */
#define _MMU_LBAT_BRPN_SHIFT    17

#define _MMU_LBAT_WIMG_MASK     0x00000078      /* memory/cache access mode  */
#define _MMU_LBAT_WIMG_SHIFT    3
#define _MMU_LBAT_WRITE_THROUGH 0x00000040      /* W - write-through         */
#define _MMU_LBAT_CACHE_INHIBIT 0x00000020      /* I - caching-inhibited     */
#define _MMU_LBAT_MEM_COHERENT  0x00000010      /* M - memory coherence      */
#define _MMU_LBAT_GUARDED       0x00000008      /* G - guarded (DBAT only)   */

#define _MMU_LBAT_PP_MASK       0x00000003      /* protection bits           */
#define _MMU_LBAT_PP_SHIFT      0
#define _MMU_LBAT_PP_NOACC      0x00000000      /* no access                 */
#define _MMU_LBAT_PP_RONLY      0x00000001      /* read (b30 is don't-care)  */
#define _MMU_LBAT_PP_RW         0x00000002      /* read/write                */
#define _MMU_LBAT_PP_SHIFT      0
#define _MMU_LBAT_PP_NOACC      0x00000000      /* no access                 */

#define _MMU_LBAT_BXPN_MASK     0x00000e00 /* Block Extended Physical Page num*/
                                           /* Bits 0-2 of the physical address*/
#define _MMU_LBAT_BX_MASK       0x00000004 /* Block Extended Physical Page num*/
                                           /* Bit 3 of the physical address*/
#define _MMU_LBAT_BXPN_SHIFT    9        
#define _MMU_LBAT_BX_SHIFT      2       


/* _MMU_SEG                      Segment Descriptors                         */

#define _MMU_SEG_T              0x80000000      /* T=0 for page, T=1 for I/O */

#define _MMU_SEG_KS             0x40000000      /* sup-state protect   (T=0) */

#define _MMU_SEG_KP             0x20000000      /* user-state protect  (T=0) */

#define _MMU_SEG_N              0x10000000      /* no-execute protect  (T=0) */

#define _MMU_SEG_VSID_MASK      0x00ffffff      /* virtual segment ID  (T=0) */
#define _MMU_SEG_VSID_SHIFT     0


/* _MMU_PTE                      Page Table Entries                          */

#define _MMU_PTE0_V             0x80000000      /* entry valid (v=1)         */

#define _MMU_PTE0_VSID_MASK     0x7fffff80      /* virtual segment ID        */
#define _MMU_PTE0_VSID_SHIFT    7

#define _MMU_PTE0_H             0x00000040      /* hash function identifier  */

#define _MMU_PTE0_API_MASK      0x00000003      /* abbreviated page index    */
#define _MMU_PTE0_API_SHIFT     0

#define _MMU_PTE1_RPN_MASK      0xfffff000      /* physical page number      */
#define _MMU_PTE1_RPN_SHIFT     12

#define _MMU_PTE1_R             0x00000100      /* referenced                */

#define _MMU_PTE1_C             0x00000080      /* changed                   */

#define _MMU_PTE1_WIMG_MASK     0x00000078      /* memory/cache access mode  */
#define _MMU_PTE1_WIMG_SHIFT    3
#define _MMU_PTE1_WRITE_THROUGH 0x00000040      /* W - write-through         */
#define _MMU_PTE1_CACHE_INHIBIT 0x00000020      /* I - caching-inhibited     */
#define _MMU_PTE1_MEM_COHERENT  0x00000010      /* M - memory coherence      */
#define _MMU_PTE1_GUARDED       0x00000008      /* G - guarded               */

#define _MMU_PTE1_PP_MASK       0x00000003      /* page protection           */
#define _MMU_PTE1_PP_SHIFT      0
#define _MMU_PTE1_PP_00         0x00000000      /* PP bits = 00              */
#define _MMU_PTE1_PP_01         0x00000001      /* PP bits = 01              */
#define _MMU_PTE1_PP_10         0x00000002      /* PP bits = 10              */
#define _MMU_PTE1_PP_11         0x00000003      /* PP bits = 11              */


#ifndef _ASMLANGUAGE

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS mmu603LibInit (int pageSize);

#else   /* __STDC__ */

extern STATUS mmu603LibInit ();

#endif  /* __STDC__ */

#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* _INCmmu603Libh */
