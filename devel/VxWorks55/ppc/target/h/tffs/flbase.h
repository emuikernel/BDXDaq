
/*
 * $Log:   P:/user/amir/lite/vcs/flbase.h_v  $
   
      Rev 1.15   06 Oct 1997 19:39:44   danig
   Changed LEushort\long & Unaligned\4 to arrays
   
      Rev 1.14   10 Sep 1997 16:11:40   danig
   Got rid of generic names
   
      Rev 1.13   31 Aug 1997 14:27:32   danig
   flTooManyComponents
   
      Rev 1.12   28 Aug 1997 16:46:00   danig
   Moved SectorNo definition from fltl.h
   
      Rev 1.11   21 Aug 1997 14:06:42   unknown
   Unaligned4
   
      Rev 1.10   31 Jul 1997 14:28:22   danig
   UNALIGNED -> Unaligned
   
      Rev 1.9   28 Jul 1997 15:15:58   danig
   Standard typedefs
   
      Rev 1.8   24 Jul 1997 18:05:40   amirban
   New include file org
   
      Rev 1.7   07 Jul 1997 15:23:26   amirban
   Ver 2.0
   
      Rev 1.5   18 May 1997 17:34:30   amirban
   Defined dataError
   
      Rev 1.4   03 Oct 1996 11:56:16   amirban
   New Big-Endian
   
      Rev 1.3   18 Aug 1996 13:47:34   amirban
   Comments
   
      Rev 1.2   12 Aug 1996 15:46:58   amirban
   Defined incomplete and timedOut codes
   
      Rev 1.1   16 Jun 1996 14:03:00   amirban
   Added iFLite compatibility mode
   
      Rev 1.0   20 Mar 1996 13:33:20   amirban
   Initial revision.
 */

/************************************************************************/
/*                                                                      */
/*		FAT-FTL Lite Software Development Kit			*/
/*		Copyright (C) M-Systems Ltd. 1995-1996			*/
/*									*/
/************************************************************************/


#ifndef FLBASE_H
#define FLBASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flsystem.h"
#include "flcustom.h"

/* standard type definitions */
typedef int 		FLBoolean;

/* Boolean constants */

#ifndef FALSE
#define FALSE	0
#endif
#ifndef TRUE
#define	TRUE	1
#endif

#ifndef TFFS_ON
#define	TFFS_ON		1
#endif
#ifndef TFFS_OFF
#define	TFFS_OFF	0
#endif

#define SECTOR_SIZE		(1 << SECTOR_SIZE_BITS)

/* define SectorNo range according to media maximum size */
#if (MAX_VOLUME_MBYTES * 0x100000l) / SECTOR_SIZE > 0x10000l
typedef unsigned long SectorNo;
#define	UNASSIGNED_SECTOR 0xffffffffl
#else
typedef unsigned short SectorNo;
#define UNASSIGNED_SECTOR 0xffff
#endif

#if FAR_LEVEL > 0
  #define FAR0	far
#else
  #define FAR0
#endif

#if FAR_LEVEL > 1
  #define FAR1	far
#else
  #define FAR1
#endif

#if FAR_LEVEL > 2
  #define FAR2	far
#else
  #define FAR2
#endif


#define vol (*pVol)


#ifndef TFFS_BIG_ENDIAN

typedef unsigned short LEushort;
typedef unsigned long LEulong;

#define LE2(arg)	arg
#define	toLE2(to,arg)	(to) = (arg)
#define LE4(arg)	arg
#define	toLE4(to,arg)	(to) = (arg)
#define COPY2(to,arg)	(to) = (arg)
#define COPY4(to,arg)	(to) = (arg)

typedef unsigned char Unaligned[2];
typedef Unaligned Unaligned4[2];

#define UNAL2(arg)	fromUNAL(arg)
#define toUNAL2(to,arg)	toUNAL(to,arg)

#define UNAL4(arg)	fromUNALLONG(arg)
#define toUNAL4(to,arg)	toUNALLONG(to,arg)

extern void toUNAL(unsigned char FAR0 *unal, unsigned n);

extern unsigned short fromUNAL(unsigned char const FAR0 *unal);

extern void toUNALLONG(Unaligned FAR0 *unal, unsigned long n);

extern unsigned long fromUNALLONG(Unaligned const FAR0 *unal);

#else

typedef unsigned char LEushort[2];
typedef unsigned char LEulong[4];

#define LE2(arg)	fromLEushort(arg)
#define	toLE2(to,arg)	toLEushort(to,arg)
#define LE4(arg)	fromLEulong(arg)
#define	toLE4(to,arg)	toLEulong(to,arg)
#define COPY2(to,arg)	copyShort(to,arg)
#define COPY4(to,arg)	copyLong(to,arg)

#define	Unaligned	LEushort
#define	Unaligned4	LEulong

extern void toLEushort(unsigned char FAR0 *le, unsigned n);

extern unsigned short fromLEushort(unsigned char const FAR0 *le);

extern void toLEulong(unsigned char FAR0 *le, unsigned long n);

extern unsigned long fromLEulong(unsigned char const FAR0 *le);

extern void copyShort(unsigned char FAR0 *to,
		      unsigned char const FAR0 *from);

extern void copyLong(unsigned char FAR0 *to,
		     unsigned char const FAR0 *from);

#define	UNAL2		LE2
#define	toUNAL2		toLE2

#define	UNAL4		LE4
#define	toUNAL4		toLE4

#endif /* TFFS_BIG_ENDIAN */


#ifndef IFLITE_ERROR_CODES
typedef enum {flOK 		= 0,	/* Status code for operation.
					   A zero value indicates success,
					   other codes are the extended
					   DOS codes. */
	     flBadFunction	= 1,
	     flFileNotFound	= 2,
	     flPathNotFound	= 3,
	     flTooManyOpenFiles = 4,
	     flNoWriteAccess	= 5,
	     flBadFileHandle	= 6,
	     flDriveNotAvailable = 9,
	     flNonFATformat	= 10,
	     flFormatNotSupported = 11,
	     flNoMoreFiles	= 18,
	     flWriteProtect 	= 19,
	     flBadDriveHandle	= 20,
	     flDriveNotReady 	= 21,
	     flUnknownCmd 	= 22,
	     flBadFormat	= 23,
	     flBadLength	= 24,
	     flDataError	= 25,
	     flUnknownMedia 	= 26,
	     flSectorNotFound 	= 27,
	     flOutOfPaper 	= 28,
	     flWriteFault 	= 29,
	     flReadFault	= 30,
	     flGeneralFailure 	= 31,
	     flDiskChange 	= 34,
	     flVppFailure	= 50,
	     flBadParameter	= 51,
	     flNoSpaceInVolume	= 52,
	     flInvalidFATchain	= 53,
	     flRootDirectoryFull = 54,
	     flNotMounted	= 55,
	     flPathIsRootDirectory = 56,
	     flNotADirectory	= 57,
	     flDirectoryNotEmpty = 58,
	     flFileIsADirectory	= 59,
	     flAdapterNotFound	= 60,
	     flFormattingError	= 62,
	     flNotEnoughMemory	= 63,
	     flVolumeTooSmall	= 64,
	     flBufferingError	= 65,
	     flFileAlreadyExists = 80,

	     flIncomplete	= 100,
	     flTimedOut		= 101,
	     flTooManyComponents = 102
#else

#include "type.h"

typedef enum {flOK 		= ERR_NONE,
					/* Status code for operation.
					   A zero value indicates success,
					   other codes are the extended
					   DOS codes. */
	     flBadFunction	= ERR_SW_HW,
	     flFileNotFound	= ERR_NOTEXISTS,
	     flPathNotFound	= ERR_NOTEXISTS,
	     flTooManyOpenFiles	= ERR_MAX_FILES,
	     flNoWriteAccess	= ERR_WRITE,
	     flBadFileHandle	= ERR_NOTOPEN,
	     flDriveNotAvailable = ERR_SW_HW,
	     flNonFATformat	= ERR_PARTITION,
	     flFormatNotSupported = ERR_PARTITION,
	     flNoMoreFiles	= ERR_NOTEXISTS,
	     flWriteProtect 	= ERR_WRITE,
	     flBadDriveHandle	= ERR_SW_HW,
	     flDriveNotReady 	= ERR_PARTITION,
	     flUnknownCmd 	= ERR_PARAM,
	     flBadFormat	= ERR_PARTITION,
	     flBadLength	= ERR_SW_HW,
	     flDataError	= ERR_READ,
	     flUnknownMedia 	= ERR_PARTITION,
	     flSectorNotFound 	= ERR_READ,
	     flOutOfPaper 	= ERR_SW_HW,
	     flWriteFault 	= ERR_WRITE,
	     flReadFault	= ERR_READ,
	     flGeneralFailure 	= ERR_SW_HW,
	     flDiskChange 	= ERR_PARTITION,
	     flVppFailure	= ERR_WRITE,
	     flBadParameter	= ERR_PARAM,
	     flNoSpaceInVolume	= ERR_SPACE,
	     flInvalidFATchain	= ERR_PARTITION,
	     flRootDirectoryFull = ERR_DIRECTORY,
	     flNotMounted	= ERR_PARTITION,
	     flPathIsRootDirectory = ERR_DIRECTORY,
	     flNotADirectory	= ERR_DIRECTORY,
	     flDirectoryNotEmpty = ERR_NOT_EMPTY,
	     flFileIsADirectory	= ERR_DIRECTORY,
	     flAdapterNotFound	= ERR_DETECT,
	     flFormattingError	= ERR_FORMAT,
	     flNotEnoughMemory	= ERR_SW_HW,
	     flVolumeTooSmall	= ERR_FORMAT,
	     flBufferingError	= ERR_SW_HW,
	     flFileAlreadyExists = ERR_EXISTS,

	     flIncomplete	= ERR_DETECT,
	     flTimedOut		= ERR_SW_HW,
	     flTooManyComponents = ERR_PARAM
#endif

	     } FLStatus;

/* call a procedure returning status and fail if it fails. This works only in
   routines that return Status */

#define checkStatus(exp)      {	FLStatus status = (exp); \
				if (status != flOK)	 \
				  return status; }

#include "flsysfun.h"

#ifdef __cplusplus
}
#endif

#endif
