/* mpc107pci.h - MPC107 register Definitions  for PCI  */

/* Copyright 1984-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,11sep00,rcs  fix includes
01a,06jun00,bri written
*/

#ifndef	__INCmpc107Pcih
#define	__INCmpc107Pcih

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "drv/multi/mpc107.h"

/* defines */


/* PCI Definitions */

/* PCI CONFIG_ADDR & CONFIG_DATA */

#define MPC107_PCI_MSTR_CFG_ADRS    mpc107PciMstrCfgAdrs  /* Config Address */
#define MPC107_PCI_MSTR_CFG_ADRS_A  0x80800000  /* Config Address  MAP A */
#define MPC107_PCI_MSTR_CFG_ADRS_B  0xfec00000  /* Config Address  MAP B */
#define MPC107_PCI_MSTR_CFG_SIZE    mpc107PciMstrCfgSize  /* Config size */
#define MPC107_PCI_MSTR_CFG_SIZE_A  0x007fffff  /* 8MB for PREP map */
#define MPC107_PCI_MSTR_CFG_SIZE_B  0x002fffff  /* 3MB for CHRP map */

/* PCI IO space ,CPU View */

#define MPC107_PCI_MSTR_IO_LOCAL    mpc107PciMstrIoLocal/* CPU to PCI IO addr */
#define MPC107_PCI_MSTR_IO_LOCAL_A  0x81000000  /* I/O Address MAP A */
#define MPC107_PCI_MSTR_IO_LOCAL_B  0xfe800000  /* I/O Address MAP B */
#define MPC107_PCI_MSTR_IO_SIZE	   mpc107PciMstrIoSize	/* I/O SIZE */
#define MPC107_PCI_MSTR_IO_SIZE_A   0x3E7FFFFF /* I/O SIZE MAP A */
#define MPC107_PCI_MSTR_IO_SIZE_B   0x003fffff /* I/O SIZE MAP B */

/* PCI IO space ,PCI Bus  View */

#define MPC107_PCI_MSTR_IO_BUS	mpc107PciMstrIoBus /* PCI bus view I/O space */
#define MPC107_PCI_MSTR_IO_BUS_A  0x01000000  /* PCI bus view I/O space MAP A */
#define MPC107_PCI_MSTR_IO_BUS_B  0x00800000  /* PCI bus view I/O space MAP B */


/* PCI IACK space (read to generate PCI IACK) */

#define MPC107_PCI_MSTR_IACK_LOCAL   mpc107PciMstrIackLocal /* IACK space */
#define MPC107_PCI_MSTR_IACK_LOCAL_A 0xbffffff0  /* IACK space  MAP A */
#define MPC107_PCI_MSTR_IACK_LOCAL_B 0xfef00000  /* IACK space  MAP B */
#define MPC107_PCI_MSTR_IACK_SIZE_A  0xf	  /* IACK size MAP A  */
#define MPC107_PCI_MSTR_IACK_SIZE_B  0xfffff     /* IACK size for MAP B */
#define MPC107_PCI_MSTR_IACK_SIZE    mpc107PciMstrIackSize /* IACK size */

/* PCI (non-prefetchable) memory space  CPU view */

#define MPC107_PCI_MSTR_MEMIO_LOCAL	mpc107PciMstrMemIoLocal /* PCI memio */
#define MPC107_PCI_MSTR_MEMIO_LOCAL_A	0xc0000000         /* Mem space MAP A */
#define MPC107_PCI_MSTR_MEMIO_LOCAL_B	0x80000000         /* Mem space MAP B */
#define MPC107_PCI_MSTR_MEMIO_SIZE     mpc107PciMemorySize /* Mem size  */
#define MPC107_PCI_MSTR_MEMIO_SIZE_A	0x3EFFFFFF          /* Mem size MAP A */
#define MPC107_PCI_MSTR_MEMIO_SIZE_B	0x7CFFFFFF          /* Mem size MAP B */

/* PCI (non-prefetchable) memory space  PCI view */

#define MPC107_PCI_MSTR_MEMIO_BUS     mpc107PciMstrMemIoBus/* PCI view Memory */
#define MPC107_PCI_MSTR_MEMIO_BUS_A   0x0        /* PCI bus view Memory MAP A */
#define MPC107_PCI_MSTR_MEMIO_BUS_B   0x80000000 /* PCI bus view Memory MAP B */

/*
 * Slave window that makes local (60x bus) memory visible to PCI
 * devices.
 */

#define MPC107_PCI_SLV_MEM_BUS    mpc107PciSlvMemBus /* PCI to CPU memory */
#define MPC107_PCI_SLV_MEM_BUS_A  0x80000000 /* PCI to CPU memory MAP A */
#define MPC107_PCI_SLV_MEM_BUS_B  0x00000000 /* PCI to CPU memory MAP B */
#define MPC107_PCI_SLV_MEM_SIZE	 MPC107_LOCAL_MEM_SIZE /* memory SIZE */

/* macro definitions specifically for pciConfigLib and pciAutoConfigLib */

/* pciAutoConfigLib */

/* PCI prefetchable memory space */

#define MPC107_PCI_MEM_ADRS		0 /* Prefetchable memory Address */
#define MPC107_PCI_MEM_SIZE		0 /* Prefetchable Memory Size  */

/* PCI (non-prefetchable) memory space */

#define MPC107_PCI_MEMIO_ADRS	(MPC107_PCI_MSTR_MEMIO_BUS  + \
                               MPC107_PCI_RSRVD_MEM)/* Address */

#define MPC107_PCI_MEMIO_SIZE	(MPC107_PCI_MSTR_MEMIO_SIZE - \
                               MPC107_PCI_RSRVD_MEM)/* Size */


/* PCI IO space */

#define MPC107_PCI_IO_ADRS \
 	(MPC107_PCI_MSTR_IO_BUS  +  MPC107_PCI_RSRVD_IO) /* Address */

#define MPC107_PCI_IO_SIZE \
              (MPC107_PCI_MSTR_IO_SIZE - MPC107_PCI_RSRVD_IO) /* Size */

#define MPC107_PCI_LAT_TIMER	0xff /* default latency timer value */
#define MPC107_PCI_MAX_BUS		0xf  /* MAX # of PCI buses expected */
#define MPC107_NUM_PCI_SLOTS	0x4  /* 4 PCI slots: 0 to 3 */


/* Reserved from pciAutoConfig */

#define MPC107_PCI_RSRVD_MEM	0 /* PCI  Memory Reserved from pciAutoConfig */
#define MPC107_PCI_RSRVD_IO	0 /* PCI I/O reserved from pciAutoConfig */


/* PCI Access macro's */

/* PCI (non-prefetchable) memory adrs to CPU (60x bus) adrs */

#define MPC107_PCI_MEMIO2LOCAL(x) \
     ((INT32)(x) + MPC107_PCI_MSTR_MEMIO_LOCAL - MPC107_PCI_MSTR_MEMIO_BUS)

/* PCI IO memory adrs to CPU (60x bus) adrs */

#define MPC107_PCI_IO2LOCAL(x) \
     ((INT32)(x) + MPC107_PCI_MSTR_IO_LOCAL - MPC107_PCI_MSTR_IO_BUS)

/* 60x bus adrs to PCI (non-prefetchable) memory address */

#define MPC107_LOCAL2PCI_MEMIO(x) \
     ((INT32)(x) + MPC107_PCI_SLV_MEM_BUS)

/* PCI device configuration definitions */

#ifndef PCI_CFG_FORCE
#    define PCI_CFG_FORCE		0x0     /* UNSUPPORTED */
#endif

#ifndef PCI_CFG_AUTO
#     define PCI_CFG_AUTO		0x1	/* IS SUPPORTED */
#endif

#ifndef PCI_CFG_NONE
#    define PCI_CFG_NONE		0x2	/* UNSUPPORTED */
#endif

/* PCI configuration type */

#ifndef PCI_CFG_TYPE
#define PCI_CFG_TYPE		PCI_CFG_AUTO
#endif

/* function declrations */

IMPORT void 	mpc107MemMapDetect(void);

#ifdef __cplusplus
}
#endif

#endif	/* __INCmpc107Pcih */

