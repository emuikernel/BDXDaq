/* pci.h - PCI bus constants header file */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,02feb99,tm   written
*/

/*
DESCRIPTION

This file contains address space descriptor defines analogous to the
definitions for the VME bus contained in vme.h. There are two sets of
defines corresponding to a primary (PRI) PCI interface and a secondary
(SEC) interface.

The primary interface is defined as the interface that is closest to 
the system processor. The secondary interface is defined as the interface 
that is farther downstream from the processor than the primary bus, such 
as a CompactPCI bus that is accessed through a PCI-to-PCI bridge. 
*/

#ifndef __INCpcih
#define __INCpcih

#ifdef __cplusplus
extern "C" {
#endif

#define PCI_SPACE_IO_PRI     0x40
#define PCI_SPACE_MEMIO_PRI  0x41
#define PCI_SPACE_MEM_PRI    0x42
#define PCI_SPACE_IO16_PRI   0x43
#define PCI_SPACE_CFG_PRI    0x44
#define PCI_SPACE_IACK_PRI   0x45

#define PCI_SPACE_IO_SEC     0x50
#define PCI_SPACE_MEMIO_SEC  0x51
#define PCI_SPACE_MEM_SEC    0x52
#define PCI_SPACE_IO16_SEC   0x53
#define PCI_SPACE_CFG_SEC    0x54
#define PCI_SPACE_IACK_SEC   0x55

#define PCI_SPACE_IS_PRI(s) (((s) & 0xf0) == 0x40)
#define PCI_SPACE_IS_SEC(s) (((s) & 0xf0) == 0x50)

#define PCI_SPACE_IS_LOCAL PCI_SPACE_IS_PRI
#define PCI_SPACE_IS_CPCI  PCI_SPACE_IS_SEC

#ifdef __cplusplus
}
#endif

#endif /* __INCpcih */
