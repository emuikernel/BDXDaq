/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
******************************************************************************/

#ifndef _PMC280_PCI_H_
#define _PMC280_PCI_H_

struct _pcidevice
{
        int deviceid;
        int vendorid;
        int deviceno;
        int functionno;
        int busno;
};

#define PCI_BASEADDR_IO              (1<<0)  /* PCI I/O space */
#define PCI_BASEADDR_MEM             (0<<0)  /* PCI Memory space */
#define PCI_BASEADDR_MEM_TYPE        (3<<1)  /* memory type mask */
#define PCI_BASEADDR_MEM_32BIT       (0<<1)  /* map anywhere in 32-bit addr
                                                space */
#define PCI_BASEADDR_MEM_ONEMEG      (1<<1)  /* map below 1MB */
#define PCI_BASEADDR_MEM_64BIT       (2<<1)  /* map anywhere in 64-bit addr i
                                                space */
#define PCI_BASEADDR_MEM_PREFETCH    (1<<3)  /* prefetchable */

#define PSWAP(val)                   LONGSWAP(val)

#define PCI_CLINE_SZ                 0x20

/* Latency Timer value - 255 PCI clocks */

#define PCI_LAT_TIMER                0xff
        /* No translation */
#define PCI2DRAM_BASE_ADRS           0x00000000   /* memory seen from PCI bus */
#define PCI_BUS_DEFAULT              0        /* Default PCI Bus number */

#define PCI_CMDREG_IOSP      (1<<0)  /* Enable IO space accesses */
#define PCI_CMDREG_MEMSP     (1<<1)  /* Enable MEM space accesses */
#define PCI_CMDREG_MASTR     (1<<2)  /* Enable PCI Mastership */

#define TARGET_INTR_A_VEC    ((void *) INUM_TO_IVEC (PCI0_INTR_A_VEC))
#define TARGET_INTR_B_VEC    ((void *) INUM_TO_IVEC (PCI0_INTR_B_VEC))
#define TARGET_INTR_C_VEC    ((void *) INUM_TO_IVEC (PCI0_INTR_C_VEC))
#define TARGET_INTR_D_VEC    ((void *) INUM_TO_IVEC (PCI0_INTR_D_VEC))

#endif /* _PMC280_PCI_H_ */
