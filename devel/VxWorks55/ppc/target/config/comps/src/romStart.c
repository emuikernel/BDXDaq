/* romStart.c - ROM initialization module */

/* Copyright 1989-2003 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01g,13jan03,j_b  add ARM THUMB support for ROM_COPY and ROM_COMPRESSED images
                 (SPR 80008)
01f,26mar02,sbs  correcting data section relocation for ROM compressed images
                 (SPR 73785)
01e,29nov01,sbs  Adding dual copy mechanism for copying data sections from
                 ROM to RAM using new WRS labels for sdata and edata.
01d,27nov01,tpw  Manually unroll copyLongs and fillLongs.
01c,06jul98,jmp  added USER_RESERVED_MEM definition if USER_RESERVED_MEM is
                 not defined.
01b,20may98,ms   minor cleanup to docs and code.
01a,22aug97,ms   derived from 04p of bootInit.c and simplified.
*/

/*
DESCRIPTION
This module provides a generic ROM startup facility.  The target-specific
romInit.s module performs the minimal preliminary board initialization and
then jumps to the C routine romStart(). It is assumed that the romInit.s
module has already set the initial stack pointer to STACK_ADRS.
This module optionally clears memory to avoid parity errors, initializes
the RAM data area, and then jumps to the routine "relocEntry".
This module is a replacement for config/all/bootInit.c.

CLEARING MEMORY
One feature of this module is to optionally clear memory to avoid parity
errors.
Generally, we clear for the bottom of memory (the BSP's LOCAL_MEM_LOCAL_ADRS
macro), to the top of memory (LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE),
but avoiding the stack area. In order to avoid trashing the stack area,
the BSP's romInit.s must have set the stack pointer to the macro STACK_ADRS.
There are three configuration macros which can be overridden in the
BSP's config.h file to change how memory is cleared:
ROMSTART_BOOT_CLEAR	undefine to not clear memory when booting
USER_RESERVED_MEM	area at the top of memory to not clear
RESERVED		area at the bottom of memory to not clear
.bS 22
	RAM
    --------------  0x00100000 = LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE
    |            |
    |------------|		= " - USER_RESERVED_MEM
    |            |
    |  0 filled  |
    |            |
    |------------|  0x00001000  = RAM_DATA_ADRS
    | initial sp |
    |------------|		= RAM_DATA_ADRS - STACK_SAVE
    |            |
    | 0 filled   |
    |            |  exc vectors, bp anchor, exc msg, bootline
    |            |
    |------------|		= LOCAL_MEM_LOCAL_ADRS + RESERVED
    |            |
    --------------  0x00000000  = LOCAL_MEM_LOCAL_ADRS
.bE
Note: The STACK_ADRS macro defaults to either RAM_DATA_ADRS, or
RAM_DATA_ADRS - STACK_SAVE, depending on if the stack grows up
or down for the given processor.
Note: The RAM_DATA_ADRS macro is passed to us from the Make system.
It represents the address to which the data segment is linked
and defaults to the BSP Makefile's RAM_LOW_ADRS macro. For details on
how to change the addresses to which VxWorks is linked, refer to the
documentation in h/make/defs.vxWorks.

INITIALIZING DATA
The other main feture of this module is to initialize the RAM data
area.
The way this is done depends on the kind of image you are building.
There are three ROM image types that are supported by the Make
system:
  ROM_RESIDENT	run from ROM, with only the data segment placed in RAM.
  ROM_COPY	copy the main image from ROM to RAM and jump to it.
  ROM_COMPRESS	uncompress the main image from ROM to RAM and jump to it.
The make system will define one of these macros for you, depending on the
name of the target you are building (%.rom_res, %.rom_cpy, or %.rom_cmp).
There are separate version of romStart for each of these cases.
Let's start with the simplest case; ROM_RESIDENT.

ROM_RESIDENT IMAGES
When the board is powered on, the processor starts executing instructions
from ROM. The ROM contains some text (code) and a copy of the data
segment. The pictured below is a typical example:
.bS 6
	ROM
    --------------
    |            |
    |------------|
    |    data    |
    |------------|  0xff8xxxxx  = ROM_DATA_ADRS
    |    text    |
    |            |  0xff800008  = ROM_TEXT_ADRS
    --------------  0xff800000  = ROM_BASE_ADRS
.bE
romStart() copies the data segment from ROM_DATA_ADRS to RAM_DATA_ADRS -
the address to which it is really linked.
The picture below is an example of RAM addresses on a 1 meg board.
.bS 22
	RAM
    --------------  0x00100000 = LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE
    |            |
    |            |
    |------------|		= RAM_DATA_ADRS + data segment size
    |            |
    |data segment|
    |            |
    |------------|  0x00001000  = RAM_DATA_ADRS
    | initial sp |
    |------------|		= RAM_DATA_ADRS - STACK_SAVE
    |            |
    |            |
    --------------  0x00000000  = LOCAL_MEM_LOCAL_ADRS
.bE

ROM_COPY and ROM_COMPRESS IMAGES
These images are slightly more complex than ROM_RESIDENT images.
The Make system produces an intermediate image, called %.ram_reloc,
which runs from RAM. It then embeds the binary for this subimage within
a ROM image. The subimage is located between the global symbols
"binArrayStart" and "binArrayEnd". The main difference between
the ROM_COPY and the ROM_COMPRESS images is how the subimage is
stored.
For ROM_COMPRESS, the subimage is compressed to save ROM space.
A modified version of the Public Domain \f3zlib\fP library is used to
uncompress the VxWorks boot ROM executable linked with it.  Compressing
object code typically achieves over 55% compression, permitting much
larger systems to be burned into ROM.  The only expense is the added few
seconds delay while the first two stages complete.
For ROM_COPY, the subimage is not compressed to make the
startup sequence shorter.
In both cases, the ROM image initially looks like this, with
binArrayStart and binArrayEnd at the end of the ROM data area.
The pictured below is a typical example:
.bS 6
	ROM
    --------------
    |            |
    |------------|  0xff8xxxxx	= binArrayEnd
    |  subImage  |
    |------------|  0xff8xxxxx	= binArrayStart
    |    data    |
    |------------|  0xff8xxxxx  = ROM_DATA_ADRS
    |    text    |
    |            |  0xff800008  = ROM_TEXT_ADRS
    --------------  0xff800000  = ROM_BASE_ADRS
.bE
For ROM_COPY images, romStart() copies the subimage from binArrayStart to
RAM_DST_ADRS
For ROM_UNCOMPRESS images, romStart() first copies the uncompression
code's data segment to RAM_DATA_ADRS, and then runs the uncompression
routine to place the subimage at RAM_DST_ADRS.
Both the RAM_DST_ADRS and RAM_DATA_ADRS macros are link addressed
defined by the Make system, which default to the BSP Makefile's
RAM_LOW_ADRS and RAM_HIGH_ADRS respectively.
For information on how to change link addresses, refer to the file
target/h/make/rules.bsp.
The picture below is an example of RAM addresses on a 1 meg board.
"temp data" below refers to the uncompression code's data segment.
.bS 22
        RAM
    --------------  0x00100000 = LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE
    |            |
    |            |
    |------------|
    | temp data  |
    |------------|  0x00090000  = RAM_DATA_ADRS
    | initial sp |
    |------------|              = RAM_DATA_ADRS - STACK_SAVE
    |            |
    |            |
    |------------|
    |            |
    |  subimage  |
    |            |
    |------------|  0x00001000  = RAM_DST_ADRS (for non-resident images)
    |            |
    |            |
    --------------  0x00000000  = LOCAL_MEM_LOCAL_ADRS
.bE

SEE ALSO:
inflate(), romInit(), and deflate.

AUTHOR
The original compression software was written by Jean-loup Gailly
and Mark Adler. See the manual pages of inflate and deflate for
more information on their freely available compression software.

SUMMARY OF CONFIGURATION MACROS
Macro		defined		synopsys
-----		-------		--------
ROM_BASE_ADRS	config.h	Start of ROM
ROM_TEXT_ADRS	Makefile	Start of text segment within ROM, typically
				a CPU-dependant offset from ROM_BASE_ADRS.
ROM_DATA_ADRS	Here		Location in ROM containing a copy of the
				data segment. This must eventually be copied
				to the RAM address to which it is linked.
RAM_DATA_ADRS	rules.bsp	Location in RAM to which the data segment is
				really linked.
STACK_ADRS	configAll.h	Initial stack pointer.
ROMSTART_BOOT_CLEAR configAll.h	Undefine to not clear memory when booting
USER_RESERVED_MEM configAll.h	Area at the top of memory to not clear
RESERVED	configAll.h	Area at the bottom of memory to not clear
LOCAL_MEM_LOCAL_ADRS config.h
LOCAL_MEM_SIZE	config.h

CHANGES from bootInit.c:
Three separate version of romStart to make it readable.
One each for ROM_COMPRESS, ROM_RESIDENT, and ROM_COPY images.
* The ROM_COPY image copys the main image from binArrayStart to
  RAM_DST_ADRS and then jumps to it.
  The array binArrayStart and the macro RAM_DST_ADRS are provided by the
  Make system.
* The ROM_COMPRESS image uncompresses the main image from binArrayStart to
  RAM_DST_ADRS and then jumps to it.
  Before the uncompression can run, this image needs to initialize the
  uncompression code's data segment. It does this by copying data from
  ROM_DATA to RAM_DATA_ADRS.
  The array binArrayStart and the macros RAM_DST_ADRS and RAM_DATA_ADRS
  are provided by the Make system. The macro ROM_DATA is a toolchain
  dependant macro, which defaults to the global symbol "etext".
* The ROM_RESIDENT image copys its data segment from ROM to RAM. As with
  the ROM_COMPRESS image, it does this by copying from ROM_DATA to
  RAM_DATA_ADRS.
*/

#include "vxWorks.h"
#include "sysLib.h"
#include "config.h"
#include "usrConfig.h"

/* default uncompression routine for compressed ROM images */

#ifndef	UNCMP_RTN
#   define	UNCMP_RTN	inflate
#endif

/* toolchain dependant location of the data segment within ROM */

#ifndef	ROM_DATA_ADRS
#   define	ROM_DATA_ADRS	(UINT *)(etext)
#endif

/* MIPS needs to write to uncached memory, others just do normal writes */

#ifndef UNCACHED
#    define	UNCACHED(adr)	adr
#endif

/* USER_RESERVED_MEM must be set to 0 if not defined */
#ifndef USER_RESERVED_MEM
#    define USER_RESERVED_MEM 0
#endif

/* handy macros */

#define ROM_DATA(adrs) ((UINT)adrs + ((UINT)ROM_DATA_ADRS - RAM_DATA_ADRS))

#define	SYS_MEM_TOP \
	(LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE - USER_RESERVED_MEM)

#define SYS_MEM_BOTTOM \
	(LOCAL_MEM_LOCAL_ADRS + RESERVED)

#define BINARRAYEND_ROUNDOFF    (ROUND_DOWN(binArrayEnd, sizeof(long)))

/* forward declarations */

LOCAL void copyLongs (FAST UINT *source, FAST UINT *destination, UINT nlongs);

#ifdef	ROMSTART_BOOT_CLEAR
LOCAL void fillLongs (FAST UINT *buf, UINT nlongs, FAST UINT val);
LOCAL void bootClear (void);
#endif

/* imports */

IMPORT STATUS   UNCMP_RTN ();
IMPORT void     relocEntry ();
IMPORT UCHAR    binArrayStart [];       /* compressed binary image */
IMPORT UCHAR    binArrayEnd [];		/* end of compressed binary image */
IMPORT char     etext [];               /* defined by the loader */
IMPORT char     end [];                 /* defined by the loader */
IMPORT char     wrs_kernel_data_end []; /* defined by the loader */

#ifdef	ROM_COMPRESS
/*******************************************************************************
*
* romStart - generic ROM initialization for compressed images
*
* This is the first C code executed after reset.
*
* This routine is called by the assembly start-up code in romInit().
* It clears memory, copies ROM to RAM, and invokes the uncompressor.
* It then jumps to the entry point of the uncompressed object code.
*
* RETURNS: N/A
*/

void romStart
    (
    FAST int startType		/* start type */
    )
    {
    volatile FUNCPTR absEntry = (volatile FUNCPTR)RAM_DST_ADRS;

    /* relocate the data segment of the decompression stub */

    copyLongs (ROM_DATA_ADRS, (UINT *)UNCACHED(RAM_DATA_ADRS),
              ((UINT)binArrayStart - (UINT)RAM_DATA_ADRS) / sizeof (long));

    copyLongs ((UINT *)((UINT)ROM_DATA_ADRS + ((UINT)BINARRAYEND_ROUNDOFF -
        (UINT)RAM_DATA_ADRS)), (UINT *)UNCACHED(BINARRAYEND_ROUNDOFF),
	((UINT)wrs_kernel_data_end - (UINT)binArrayEnd) / sizeof (long));

    /* If cold booting, clear memory to avoid parity errors */

#ifdef	ROMSTART_BOOT_CLEAR
    if (startType & BOOT_CLEAR)
	bootClear();
#endif

    /* decompress the main image */

    if (UNCMP_RTN (ROM_DATA(binArrayStart),
		UNCACHED(RAM_DST_ADRS),
		binArrayEnd - binArrayStart) != OK)
	return;

#if     ((CPU_FAMILY == ARM) && ARM_THUMB)
     absEntry = (FUNCPTR)((UINT32)absEntry | 1);         /* force Thumb state */
#endif  /* CPU_FAMILY == ARM */

    /* and jump to it */

    absEntry (startType);
    }
#endif	/* ROM_COMPRESS */

#ifdef	ROM_COPY
/*******************************************************************************
*
* romStart - generic ROM initialization for uncompressed ROM images
*
* This is the first C code executed after reset.
*
* This routine is called by the assembly start-up code in romInit().
* It clears memory, copies ROM to RAM, and then jumps to the entry
* point of the copied object code.
*
* RETURNS: N/A
*/

void romStart
    (
    FAST int startType		/* start type */
    )
    {
    volatile FUNCPTR absEntry = (volatile FUNCPTR)RAM_DST_ADRS;

    /* If cold booting, clear memory to avoid parity errors */

#ifdef ROMSTART_BOOT_CLEAR
    if (startType & BOOT_CLEAR)
        bootClear();
#endif

    /* copy the main image into RAM */

    copyLongs ((UINT *)ROM_DATA(binArrayStart),
		(UINT *)UNCACHED(RAM_DST_ADRS),
		(binArrayEnd - binArrayStart) / sizeof (long));

#if     ((CPU_FAMILY == ARM) && ARM_THUMB)
     absEntry = (FUNCPTR)((UINT32)absEntry | 1);         /* force Thumb state */
#endif  /* CPU_FAMILY == ARM */

    /* and jump to it */

    absEntry (startType);
    }
#endif	/* ROM_COPY */

#ifdef	ROM_RESIDENT
/*******************************************************************************
*
* romStart - generic ROM initialization for ROM resident images
*
* This is the first C code executed after reset.
*
* This routine is called by the assembly start-up code in romInit().
* It clears memory, copies ROM to RAM, and invokes the uncompressor.
* It then jumps to the entry point of the uncompressed object code.
*
* RETURNS: N/A
*/

void romStart
    (
    FAST int startType		/* start type */
    )
    {
    /* relocate the data segment into RAM */

    copyLongs ((UINT *)ROM_DATA_ADRS, (UINT *)UNCACHED(RAM_DATA_ADRS),
		((UINT)end - (UINT)RAM_DATA_ADRS) / sizeof (long));

    /* If cold booting, clear memory to avoid parity errors */

#ifdef ROMSTART_BOOT_CLEAR
    if (startType & BOOT_CLEAR)
        bootClear();
#endif

    /* and jump to the entry */

    usrInit (startType);
    }
#endif	/* ROM_RESIDENT */

#ifdef	ROMSTART_BOOT_CLEAR
/******************************************************************************
*
* bootClear - clear memory
*
* If cold booting, clear memory not loaded with text & data.
*
* We are careful about initializing all memory (except
* STACK_SAVE bytes) due to parity error generation (on
* some hardware) at a later stage.  This is usually
* caused by read accesses without initialization.
*/

LOCAL void bootClear (void)
    {
    /* fill from the bottom of memory to the load image */

    fillLongs ((UINT *)SYS_MEM_BOTTOM,
	((UINT)RAM_DATA_ADRS - STACK_SAVE - (UINT)SYS_MEM_BOTTOM) /
	sizeof (long), 0);

    /* fill from the load image to the top of memory */

    fillLongs ((UINT *)end, ((UINT)SYS_MEM_TOP - (UINT)end) / sizeof(long), 0);
    }

/*******************************************************************************
*
* fillLongs - fill a buffer with a value a long at a time
*
* This routine fills the first <nlongs> longs of the buffer with <val>.
*/

LOCAL void fillLongs
    (
    FAST UINT *	buf,	/* pointer to buffer              */
    UINT	nlongs,	/* number of longs to fill        */
    FAST UINT	val	/* char with which to fill buffer */
    )
    {
    FAST UINT *bufend = buf + nlongs;
    FAST UINT nchunks;

    /* Hop by chunks of longs, for speed. */
    for (nchunks = nlongs / 8; nchunks; --nchunks)
	{
#if (CPU_FAMILY == MC680X0)
	*buf++ = val;	/* 0 */
	*buf++ = val;	/* 1 */
	*buf++ = val;	/* 2 */
	*buf++ = val;	/* 3 */
	*buf++ = val;	/* 4 */
	*buf++ = val;	/* 5 */
	*buf++ = val;	/* 6 */
	*buf++ = val;	/* 7 */
#else
	buf[0] = val;
	buf[1] = val;
	buf[2] = val;
	buf[3] = val;
	buf[4] = val;
	buf[5] = val;
	buf[6] = val;
	buf[7] = val;
	buf += 8;
#endif
	}

    /* Do the remainder one long at a time. */
    while (buf < bufend)
	*buf++ = val;
    }
#endif

/*******************************************************************************
*
* copyLongs - copy one buffer to another a long at a time
*
* This routine copies the first <nlongs> longs from <source> to <destination>.
*/

LOCAL void copyLongs
    (
    FAST UINT *	source,		/* pointer to source buffer      */
    FAST UINT *	destination,	/* pointer to destination buffer */
    UINT 	nlongs		/* number of longs to copy       */
    )
    {
    FAST UINT *dstend = destination + nlongs;
    FAST UINT nchunks;

    /* Hop by chunks of longs, for speed. */
    for (nchunks = nlongs / 8; nchunks; --nchunks)
	{
#if (CPU_FAMILY == MC680X0)
	*destination++ = *source++;	/* 0 */
	*destination++ = *source++;	/* 1 */
	*destination++ = *source++;	/* 2 */
	*destination++ = *source++;	/* 3 */
	*destination++ = *source++;	/* 4 */
	*destination++ = *source++;	/* 5 */
	*destination++ = *source++;	/* 6 */
	*destination++ = *source++;	/* 7 */
#else
	destination[0] = source[0];
	destination[1] = source[1];
	destination[2] = source[2];
	destination[3] = source[3];
	destination[4] = source[4];
	destination[5] = source[5];
	destination[6] = source[6];
	destination[7] = source[7];
	destination += 8, source += 8;
#endif
	}

    /* Do the remainder one long at a time. */
    while (destination < dstend)
	*destination++ = *source++;
    }


