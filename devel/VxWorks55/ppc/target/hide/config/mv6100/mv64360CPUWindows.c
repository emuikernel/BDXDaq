/* mv64360CPUWindows.c - MV64360 CPU Addressing Window Utility */

/*
modification history
--------------------
01b,02feb04,cak  Modified mv64360CPUWindows() to make the output more
		 "readable".
01a,17dec03,cak  Ported from ver 01a, rtss source base.
*/

/*
DESCRIPTION

The purpose of this utility is to display all of the enabled
CPU addressing windows of the Discovery II (MV64360).
*/

/* includes */

#include "mv64360.h"
#include "vxWorks.h"
#include "config.h"
#include "mv6100A.h"

/* defines */

/* typedefs */

/* globals */

/* locals */

/* forward declarations */

VOID mv64360CPUWindowShow (void);

/*******************************************************************************
*
* mv64360CPUWindowShow - display enabled windows of the MV64360 
*
* RETURNS: N/A
*/

void mv64360CPUWindowShow ()
    {
    UINT32 enable;
    UINT32 base;
    UINT32 size;

    /* Base Address Enable Register */

    enable = MV64360_READ32(MV64360_REG_BASE, CPUIF_BASE_ADDR_ENABLE);

    /* set all enabled windows to 1 instead of 0 */

    enable = ~(enable);

    printf("Window             Base-Address,Size\n\r");

    if ((enable & 0x00000001) == 0x00000001)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_CS0_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_CS0_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("CS0                0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00000002) == 0x00000002)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_CS1_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_CS1_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("CS1                0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00000004) == 0x00000004)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_CS2_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_CS2_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("CS2                0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00000008) == 0x00000008)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_CS3_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_CS3_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("CS3                0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00000010) == 0x00000010)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_DEVCS0_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_DEVCS0_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("DevCS0             0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00000020) == 0x00000020)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_DEVCS1_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_DEVCS1_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("DevCS1             0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00000040) == 0x00000040)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_DEVCS2_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_DEVCS2_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("DevCS2             0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00000080) == 0x00000080)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_DEVCS3_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_DEVCS3_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("DevCS3             0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00000100) == 0x00000100)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_BOOTCS_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_BOOTCS_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("BootCS             0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00000200) == 0x00000200)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI0_IO_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI0_IO_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("PCI Bus 0 IO       0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00000400) == 0x00000400)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI0_MEM0_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI0_MEM0_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("PCI Bus 0 Mem 0    0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00000800) == 0x00000800)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI0_MEM1_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI0_MEM1_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("PCI Bus 0 Mem 1    0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00001000) == 0x00001000)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI0_MEM2_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI0_MEM2_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("PCI Bus 0 Mem 2    0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00002000) == 0x00002000)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI0_MEM3_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI0_MEM3_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("PCI Bus 0 Mem 3    0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00004000) == 0x00004000)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI1_IO_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI1_IO_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("PCI Bus 1 IO       0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00008000) == 0x00008000)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI1_MEM0_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI1_MEM0_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("PCI Bus 1 Mem 0    0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00010000) == 0x00010000)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI1_MEM1_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI1_MEM1_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("PCI Bus 1 Mem 1    0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00020000) == 0x00020000)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI1_MEM2_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI1_MEM2_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("PCI Bus 1 Mem 2    0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00040000) == 0x00040000)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI1_MEM3_BASE_ADDR);
        size = MV64360_READ32(MV64360_REG_BASE, CPUIF_PCI1_MEM3_SIZE);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        size = MV64360_SIZE_FIELD_INV(size);
        printf("PCI Bus 1 Mem 3    0x%08x,0x%08x\n\r", base, size);
        }

    if ((enable & 0x00080000) == 0x00080000)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_INTEGRATED_SRAM_BASE_ADDR);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        printf("Integrated SRAM    0x%08x\n\r", base);
        }

    if ((enable & 0x00100000) == 0x00100000)
        {
        base = MV64360_READ32(MV64360_REG_BASE, CPUIF_INTERNAL_SPACE_BASE_ADDR);
        base = MV64360_BASE_DECODE_ADDR_INV(base);
        printf("Internal Space     0x%08x\n\r", base);
        }
    }
