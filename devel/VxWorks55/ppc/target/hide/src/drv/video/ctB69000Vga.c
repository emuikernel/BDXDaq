/* ctB69000Vga.c - a CHIPS B69000 initialization source module */

/* Copyright 1984-2000 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,26mar00,ms  written.
*/

/*
DESCRIPTION
The 69000 is the first product in the CHIPS family of portable graphics
accelerator product line that integrates high performance memory technology
for the graphics frame buffer. Based on the proven HiQVideo graphics
accelerator core, the 69000 combines state-of-the-art flat panel controller
capabilities with low power, high performance integrated memory. The result
is the start of a high performance, low power, highly integrated solution for
the premier family of portable graphics products.

.IP "High Performance Integrated Memory"
The 69000 is the first member of the HiQVideo family to provide integrated high
performance synchronous DRAM (SDRAM) memory technology. Targeted at the
mainstream notebook market, the 69000 incorporates 2MB of proprietary integrated
SDRAM for the graphics/video frame buffer. The integrated SDRAM memory can
support up to 83MHz operation, thus increasing the available memory bandwidth
for the graphics subsystem. The result is support for additional high color /
high resolution graphics modes combined with real-time video acceleration. This
additional bandwidth also allows more flexibility in the other graphics
functions intensely used in Graphical User Interfaces (GUIs) such as Microsoft
Windows.

.IP "Frame-Based AGP Compatibility"
The 69000 graphics is designed to be used with either 33MHz PCI, or with AGP as a
frame-based AGP device, allowing it to be used with the AGP interface provided by
the latest core logic chipsets.

.IP "HiQColor TM Technology"
The 69000 integrates CHIPS breakthrough HiQColor technology. Based on the CHIPS
proprietary TMED (Temporal Modulated Energy Distribution) algorithm, HiQColor
technology is a unique process that allows the display of 16.7 million true
colors on STN panels without using Frame Rate Control (FRC) or dithering. In
addition, TMED also reduces the need for the panel tuning associated with
current FRC-based algorithms. Independent of panel response, the TMED algorithm
eliminates all of the flaws (such as shimmer, Mach banding, and other motion
artifacts) normally associated with dithering and FRC. Combined with the new
fast response, high-contrast, and low-crosstalk technology found in new STN
panels, HiQColor technology enables the best display quality and color fidelity
previously only available with TFT technology.

.IP "Versatile Panel Support"
The HiQVideo family supports a wide variety of monochrome and color Single-
Panel, Single-Drive (SS) and Dual-Panel, Dual Drive (DD), standard and high-
resolution, passive STN and active matrix TFT/MIM LCD, and EL panels. With
HiQColor technology, up to 256 gray scales are supported on passive STN LCDs. Up
to 16.7M different colors can be displayed on passive STN LCDs and up to 16.7M
colors on 24- bit active matrix LCDs.

The 69000 offers a variety of programmable features to optimize display quality.
Vertical centering and stretching are provided for handling modes with less than
480 lines on 480-line panels. Horizontal and vertical stretching capabilities
are also available for both text and graphics modes for optimal display of VGA
text and graphics modes on 800x600, 1024x768 and 1280x1024 panels.

.IP "Television NTSC/PAL Flicker Free Output"
The 69000 uses a flicker reduction process which makes text of all fonts and
sizes readable by reducing the flicker and jumping lines on the display.

.IP "HiQVideo T Multimedia Support"
The 69000 uses independent multimedia capture and display systems on-chip.
The capture system places data in display memory (usually off screen) and
the display system places the data in a window on the screen.

.IP "Low Power Consumption"
The 69000 uses a variety of advanced power management features to reduce power
consumption of the display sub-system and to extend battery life. Optimized for
3.3V operation, the 69000 internal logic, bus and panel interfaces operate at
3.3V but can tolerate 5V operation.

.IP "Software Compatibility / Flexibility"
The HiQVideo controllers are fully compatible with the VGA standard at both the
register and BIOS levels. CHIPS and third-party vendors supply a fully VGA
compatible BIOS, end-user utilities and drivers for common application programs.

.IP "Acceleration for All Panels and All Modes"
The 69000 graphics engine is designed to support high performance graphics and
video acceleration for all supported display resolutions, display types, and
color modes. There is no compromise in performance operating in 8, 16, or 24 bpp
color modes allowing true acceleration while displaying up to 16.7M colors.

.LP

.I USAGE
This library provides initialization routines to configure CHIPS B69000 (VGA)
in alphanumeric mode.

The functions addressed here include:

.IP "   -"
Initialization of CHIPS B69000 IC.

.LP

.I USER INTERFACE

.CS
STATUS ctB69000VgaInit
    (
    VOID
    )
.CE

This routine will initialize the VGA card if present in PCI connector,
sets up register set in VGA 3+ mode and loads the font in plane 2.

INCLUDE FILES: NONE.
*/

/* includes */
#include "vxWorks.h"
#include "sysLib.h"
#include "drv/pci/pciConfigLib.h"

#include "vgaInit.c"

/* defines */
#define CTB69000VGA_VENID          0x102C
#define CTB69000VGA_DEVID          0x00C0

#define CTB69000VGA_XR_PORT        0x3D6  /* Extended Register Index Port */
#define CTB69000VGA_XR_DATA_PORT   0x3D7  /* Extended Register Data Port */
#define CTB69000VGA_FR_PORT        0x3D0  /* Flat Panel Index Port */
#define CTB69000VGA_FR_DATA_PORT   0x3D1  /* Flat Panel Data Port */
#define CTB69000VGA_MR_PORT        0x3D2  /* Multimedia Index Port */


/*******************************************************************************
*
* ctB69000VgaInit - initializes the B69000 chip and loads font in memory.
*
* This routine will initialize the VGA card if present in PCI connector,
* sets up register set in VGA 3+ mode and loads the font in plane 2.
*
* RETURNS: OK/ERROR
*/

STATUS ctB69000VgaInit
    (
    void
    )
    {
    int pciBus = 0;
    int pciDevice = 0;
    int pciFunc = 0;

    /* check PCI BASED - CHIPS B69000's presence on the bus */
    if (pciFindDevice (CTB69000VGA_VENID, CTB69000VGA_DEVID, 0,
                       &pciBus, &pciDevice, &pciFunc) != OK)
        {
        return ERROR;
        }
    
    /* enable mapped memory and IO addresses */
    pciConfigOutLong (pciBus, pciDevice, pciFunc, 0x04, 0x7);

    /*
     * Intialize CHIPS B69000 specific registers.
     */
    
    /* Extension Registers */
    vgaRegSet (CTB69000VGA_XR_PORT, 0x0005);
    vgaRegSet (CTB69000VGA_XR_PORT, 0xC006);
    vgaRegSet (CTB69000VGA_XR_PORT, 0x0308); 
    
    /* [0] = Paged; [1] = Linear Mapping; */
    vgaRegSet (CTB69000VGA_XR_PORT, 0x000A);
    vgaRegSet (CTB69000VGA_XR_PORT, 0x010B); 
    vgaRegSet (CTB69000VGA_XR_PORT, 0x000E);
    
    vgaRegSet (CTB69000VGA_XR_PORT, 0x0040);
    
    vgaRegSet (CTB69000VGA_XR_PORT, 0xBC63);
    
    vgaRegSet (CTB69000VGA_XR_PORT, 0x0080);
    vgaRegSet (CTB69000VGA_XR_PORT, 0x0081);
    
    /* Fout for DCLK0 = 25 Mhz */
    vgaRegSet (CTB69000VGA_XR_PORT, 0xD1C0);
    vgaRegSet (CTB69000VGA_XR_PORT, 0x1CC1);
    vgaRegSet (CTB69000VGA_XR_PORT, 0x00C2);
    vgaRegSet (CTB69000VGA_XR_PORT, 0x24C3);
    
    /* Fout for DCLK1 = 28 Mhz */ 
    vgaRegSet (CTB69000VGA_XR_PORT, 0x55C4);
    vgaRegSet (CTB69000VGA_XR_PORT, 0x09C5);
    vgaRegSet (CTB69000VGA_XR_PORT, 0x00C6); 
    vgaRegSet (CTB69000VGA_XR_PORT, 0x24C7);
    
    /* Fout for DCLK2 =    Mhz */
    vgaRegSet (CTB69000VGA_XR_PORT, 0x6BC8);    
    vgaRegSet (CTB69000VGA_XR_PORT, 0x3CC9);
    vgaRegSet (CTB69000VGA_XR_PORT, 0x00CA); 
    vgaRegSet (CTB69000VGA_XR_PORT, 0x21CB);
    
    /* Fout for MCLK = 83 Mhz */
    vgaRegSet (CTB69000VGA_XR_PORT, 0x10CE); 
    vgaRegSet (CTB69000VGA_XR_PORT, 0x38CC);    
    vgaRegSet (CTB69000VGA_XR_PORT, 0x03CD);
    vgaRegSet (CTB69000VGA_XR_PORT, 0x90CE);
    
    /* Enable MCLK and DCLK synthesizers */
    vgaRegSet (CTB69000VGA_XR_PORT, 0x06CF);
    
    /* Enable DAC, Palette, DCLK-Vco and MCLK-Vco */
    vgaRegSet (CTB69000VGA_XR_PORT, 0x0FD0);
    vgaRegSet (CTB69000VGA_XR_PORT, 0x01D1);
    
    /* Flat Panel Registers */
    vgaRegSet (CTB69000VGA_FR_PORT, 0x0101);

    /*
     * Intialize VGA compatible register set on B69000,
     * and download font in memory for alphanumeric mode.
     */
    vgaInit ();
    
    return OK;
    }

 



















