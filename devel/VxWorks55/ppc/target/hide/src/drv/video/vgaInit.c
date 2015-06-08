/* vgaInit.c - a VGA 3+ mode initialization source module */

/* Copyright 1984-2000 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,26mar00,ms  written.
*/

/*
DESCRIPTION

.I USAGE
This library provides initialization routines to configure VGA in 3+
alphanumeric mode.

The functions addressed here include:

.IP "   -"
Initialization of the VGA specific register set.

.LP

.I USER INTERFACE

.CS
STATUS vgaInit
    (
    VOID
    )
.CE

This routine will initialize the VGA specific register set to bring a VGA
card in VGA 3+ mode and loads the font in plane 2.

REFERENCES
.I Programmer's Guide to the EGA, VGA, and Super VGA Cards - Ferraro.
.I Programmer's Guide to PC & PS/2 Video Systems - Richard Wilton.

INCLUDE FILES: NONE.
*/

/* includes */
#include "vxWorks.h"
#include "sysLib.h"
#include "drv/pci/pciConfigLib.h"
#include "intLib.h"

/* include font */
#include "vgaFont.c"

/* defines */
#define VGA_FEATURE_CTRL  	0x3DA  /* Feature Control Register */
#define VGA_MVGA_SR_PORT        0x3C2  /* Miscellaneous Output Port */
#define VGA_DAC_PEL_MASK	0x3C6  /* Color Palette Pixel Mask Register */
#define VGA_DAC_READ_INDEX	0x3C7  /* Color Palette Read-Mode Index Reg */
#define VGA_DAC_STATE		0x3C7  /* Color Palette State Register */
#define VGA_DAC_WRITE_INDEX	0x3C8  /* Color Palette Index Register */
#define VGA_DAC_DATA_REG        0x3C9  /* Color Palette Data Register */
#define VGA_AR_PORT             0x3C0  /* Attribute Controller Index Port */
#define VGA_CR_PORT             0x3D4  /* CRT Controller Index Port */
#define VGA_GR_PORT             0x3CE  /* Graphics Controller Index Port */
#define VGA_SR_PORT             0x3C4  /* Sequencer Index Port */
#define VGA_SR_DATA_PORT        0x3C5  /* Sequencer Data Port */

/* forward declarations */
LOCAL VOID    vgaRegSet (int ioPort, short value);
LOCAL STATUS  vgaDnldFont (char * font, int len);
STATUS        vgaInit (void);

/*******************************************************************************
*
* vgaRegSet - sets the given vga register with given value.
*
* This routine sets the given vga register with given value.
*
* RETURNS: N/A
*/

LOCAL VOID vgaRegSet
    (
    int ioPort,
    short value
    )
    {
    sysOutByte (ioPort, (char) (value & 0xFF)); 
    sysOutByte (ioPort + 1, (char) (value >> 8));
    }

/*******************************************************************************
*
* vgaDnldFont - writes given font in plane 2. 
*
* This routine downloads the user specified font in bit plane 2 of VGA planes.
*
* RETURNS: OK/ERROR
*/

LOCAL STATUS vgaDnldFont
    (
    char * font,
    int len
    )
    {
    int i, j, bytesPerChar;
    int intKey;
    unsigned char * vidPtr = (unsigned char *) 0xA0000;
    
    
    if ((bytesPerChar = len/CHARPERFONT) > MAXBYTESPERCH)
        return ERROR;

    intKey = intLock();
    
    /* sequencer is put to reset */
    vgaRegSet (VGA_SR_PORT, 0x0100); 
    /* write only to plane 2 */
    vgaRegSet (VGA_SR_PORT, 0x0402);
    /* sequential addressing */
    vgaRegSet (VGA_SR_PORT, 0x0704);
    /* out of reset */
    vgaRegSet (VGA_SR_PORT, 0x0300);
    
    /* read plane 2 only */
    vgaRegSet (VGA_GR_PORT, 0x0204);
    /* disable even-odd */
    vgaRegSet (VGA_GR_PORT, 0x0005);
    /* 0xA0000 */
    vgaRegSet (VGA_GR_PORT, 0x0006);

    intUnlock (intKey);
    
    /* Downloading a font into bit plane # 2 */
    i = 0;
    j = 0;
    while(i < FONTDATAMAX) {
    if (j < bytesPerChar)
        {
        *vidPtr++ = vgaFont[i];
        i++;
        j++;
        }
    else
        {
        vidPtr += (MAXBYTESPERCH - bytesPerChar);
        j = 0;
        }
    }
    /* Downloading over */

    intKey = intLock();
    
    /* sequencer is put to reset */
    vgaRegSet (VGA_SR_PORT, 0x0100);
    /* write only to plane 0 & 1 */
    vgaRegSet (VGA_SR_PORT, 0x0302);
    /* font 0 */
    vgaRegSet (VGA_SR_PORT, 0x0003);
    /* odd-even addressing */
    vgaRegSet (VGA_SR_PORT, 0x0304);
    /* out of reset */
    vgaRegSet (VGA_SR_PORT, 0x0300);
    
    /* read plane 0 only */
    vgaRegSet (VGA_GR_PORT, 0x0004);
    /* even-odd */
    vgaRegSet (VGA_GR_PORT, 0x1005);
    /* 0xB8000 */
    vgaRegSet (VGA_GR_PORT, 0x0E06);

    intUnlock (intKey);
    
    return OK;
    }


/*******************************************************************************
*
* vgaInit - initializes the VGA chip and loads font in memory.
*
* This routine will initialize the VGA specific register set to bring a VGA
* card in VGA 3+ mode and loads the font in plane 2.
*
* RETURNS: OK/ERROR
*/

STATUS vgaInit
    (
    void
    )
    {
    int intKey;

    intKey = intLock ();
    
    /* Sequencer Registers */
    vgaRegSet (VGA_SR_PORT, 0x0100); 
    vgaRegSet (VGA_SR_PORT, 0x0001);
    /* write only to plane 0 & 1 */
    vgaRegSet (VGA_SR_PORT, 0x0302);
    vgaRegSet (VGA_SR_PORT, 0x0003);
    /* enable odd/even addressing + extended */
    vgaRegSet (VGA_SR_PORT, 0x0204);
    vgaRegSet (VGA_SR_PORT, 0x0007);
    
    /* MSR: Select DCLK0, Enable RAM access and move I/O to 3Dx */
    sysOutByte (VGA_MVGA_SR_PORT, 0x67);
    
    vgaRegSet (VGA_SR_PORT, 0x0300);

    intUnlock (intKey);
    
    /* CRT Controller Registers */
    vgaRegSet (VGA_CR_PORT, 0x0E11);
    
    vgaRegSet (VGA_CR_PORT, 0x5F00);
    vgaRegSet (VGA_CR_PORT, 0x4F01);
    vgaRegSet (VGA_CR_PORT, 0x5002);
    vgaRegSet (VGA_CR_PORT, 0x8203);
    vgaRegSet (VGA_CR_PORT, 0x5504);
    vgaRegSet (VGA_CR_PORT, 0x8105);
    vgaRegSet (VGA_CR_PORT, 0xBF06);
    vgaRegSet (VGA_CR_PORT, 0x1F07);
    vgaRegSet (VGA_CR_PORT, 0x0008);
    vgaRegSet (VGA_CR_PORT, 0x4F09);
    vgaRegSet (VGA_CR_PORT, 0x0D0A);
    vgaRegSet (VGA_CR_PORT, 0x0E0B);
    vgaRegSet (VGA_CR_PORT, 0x000C);
    vgaRegSet (VGA_CR_PORT, 0x000D);
    vgaRegSet (VGA_CR_PORT, 0x070E);
    vgaRegSet (VGA_CR_PORT, 0x800F);
    vgaRegSet (VGA_CR_PORT, 0x9C10);
    
    /* Protect Registers CR0-7 */
    vgaRegSet (VGA_CR_PORT, 0x8E11);
    
    vgaRegSet (VGA_CR_PORT, 0x8F12);
    vgaRegSet (VGA_CR_PORT, 0x2813);    
    vgaRegSet (VGA_CR_PORT, 0x1F14);    
    vgaRegSet (VGA_CR_PORT, 0x9615);
    vgaRegSet (VGA_CR_PORT, 0xB916);
    vgaRegSet (VGA_CR_PORT, 0xA317);    
    vgaRegSet (VGA_CR_PORT, 0xFF18);
    
    vgaRegSet (VGA_CR_PORT, 0x4F70);
    
    /* Attribute Controller Registers */
    sysInByte (VGA_FEATURE_CTRL);
    sysOutByte (VGA_AR_PORT, 0x00);   
    sysOutByte (VGA_AR_PORT, 0x00);   
    sysOutByte (VGA_AR_PORT, 0x01);   
    sysOutByte (VGA_AR_PORT, 0x01);    
    sysOutByte (VGA_AR_PORT, 0x02);   
    sysOutByte (VGA_AR_PORT, 0x02);    
    sysOutByte (VGA_AR_PORT, 0x03);   
    sysOutByte (VGA_AR_PORT, 0x03);    
    sysOutByte (VGA_AR_PORT, 0x04);   
    sysOutByte (VGA_AR_PORT, 0x04);    
    sysOutByte (VGA_AR_PORT, 0x05);   
    sysOutByte (VGA_AR_PORT, 0x05);    
    sysOutByte (VGA_AR_PORT, 0x06);   
    sysOutByte (VGA_AR_PORT, 0x14);    
    sysOutByte (VGA_AR_PORT, 0x07);   
    sysOutByte (VGA_AR_PORT, 0x07);    
    sysOutByte (VGA_AR_PORT, 0x08);   
    sysOutByte (VGA_AR_PORT, 0x38);    
    sysOutByte (VGA_AR_PORT, 0x09);   
    sysOutByte (VGA_AR_PORT, 0x39);    
    sysOutByte (VGA_AR_PORT, 0x0A);   
    sysOutByte (VGA_AR_PORT, 0x3A);    
    sysOutByte (VGA_AR_PORT, 0x0B);   
    sysOutByte (VGA_AR_PORT, 0x3B);    
    sysOutByte (VGA_AR_PORT, 0x0C);   
    sysOutByte (VGA_AR_PORT, 0x3C);    
    sysOutByte (VGA_AR_PORT, 0x0D);   
    sysOutByte (VGA_AR_PORT, 0x3D);    
    sysOutByte (VGA_AR_PORT, 0x0E);   
    sysOutByte (VGA_AR_PORT, 0x3E);    
    sysOutByte (VGA_AR_PORT, 0x0F);   
    sysOutByte (VGA_AR_PORT, 0x3F);    
    sysOutByte (VGA_AR_PORT, 0x10);   
    sysOutByte (VGA_AR_PORT, 0x0C);    
    sysOutByte (VGA_AR_PORT, 0x11);   
    sysOutByte (VGA_AR_PORT, 0x00);    
    sysOutByte (VGA_AR_PORT, 0x12);   
    sysOutByte (VGA_AR_PORT, 0x0F);    
    sysOutByte (VGA_AR_PORT, 0x13);   
    sysOutByte (VGA_AR_PORT, 0x08);    
    sysOutByte (VGA_AR_PORT, 0x14);   
    sysOutByte (VGA_AR_PORT, 0x00);
    /* Enable Video */
    sysOutByte (VGA_AR_PORT, 0x20);
    
    /* Graphics Controller Registers */
    vgaRegSet (VGA_GR_PORT, 0x0000);
    vgaRegSet (VGA_GR_PORT, 0x0001);
    vgaRegSet (VGA_GR_PORT, 0x0002);
    vgaRegSet (VGA_GR_PORT, 0x0003);
    vgaRegSet (VGA_GR_PORT, 0x0004);
    vgaRegSet (VGA_GR_PORT, 0x1005);
    /* B8000-BFFFF */
    vgaRegSet (VGA_GR_PORT, 0x0E06);
    vgaRegSet (VGA_GR_PORT, 0x0007);
    vgaRegSet (VGA_GR_PORT, 0xFF08);
    
    /* Palette Registers */
    sysOutByte (VGA_DAC_PEL_MASK, 0xFF);
    sysOutByte (VGA_DAC_WRITE_INDEX, 0x00);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x00);         /* 0x00 - 0x0F */
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);         /* 0x00 - 0x0F */
    
    sysOutByte (VGA_DAC_DATA_REG, 0x00);         /* 0x10 - 0x1F */
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);         /* 0x10 - 0x1F */
    
    sysOutByte (VGA_DAC_DATA_REG, 0x15);         /* 0x20 - 0x2F */
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
 
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);         /* 0x20 - 0x2F */
    
    sysOutByte (VGA_DAC_DATA_REG, 0x15);         /* 0x30 - 0x3F */
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x00);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x2A);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x15);
    
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);
    sysOutByte (VGA_DAC_DATA_REG, 0x3F);         /* 0x30 - 0x3F */
    
    vgaDnldFont (vgaFont, FONTDATAMAX);
    
    return OK;
    }
