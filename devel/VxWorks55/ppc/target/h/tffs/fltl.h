
/*
 * $Log:   P:/user/amir/lite/vcs/fltl.h_v  $

      Rev 1.11   30 Mar 1997 16:31:40   Andray,Hdei
   Changed flMount's prototype

      Rev 1.11   10 Sep 1997 16:31:40   danig
   Got rid of generic names

      Rev 1.10   28 Aug 1997 16:46:50   danig
   Moved SectorNo definition to flbase.h

      Rev 1.9   28 Jul 1997 14:49:38   danig
   volForCallback

      Rev 1.8   24 Jul 1997 17:58:16   amirban
   FAR to FAR0

      Rev 1.7   07 Jul 1997 15:23:52   amirban
   Ver 2.0

      Rev 1.6   03 Jun 1997 17:08:02   amirban
   setBusy change

      Rev 1.5   21 Oct 1996 18:02:34   amirban
   Defragment i/f change

      Rev 1.4   10 Sep 1996 17:32:26   amirban
   Unsigned int --> unsigned short

      Rev 1.3   18 Aug 1996 13:47:32   amirban
   Comments

      Rev 1.2   12 Aug 1996 15:48:12   amirban
   Defined setBusy

      Rev 1.1   14 Jul 1996 16:48:52   amirban
   Format params

      Rev 1.0   20 Mar 1996 13:33:20   amirban
   Initial revision.
 */

/************************************************************************/
/*                                                                      */
/*		FAT-FTL Lite Software Development Kit			*/
/*		Copyright (C) M-Systems Ltd. 1995-1996			*/
/*									*/
/************************************************************************/


#ifndef FLTL_H
#define FLTL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flflash.h"

/* See interface documentation of functions in ftllite.c	*/

typedef struct tTL TL;		/* Forward definition */
typedef struct tTLrec TLrec; 	/* Defined by translation layer */

struct tTL {
  TLrec		*rec;

  const void FAR0 *(*mapSector)(TLrec *, SectorNo sectorNo, CardAddress *physAddr);
  FLStatus	(*writeSector)(TLrec *, SectorNo sectorNo, void FAR1 *fromAddress);
  FLStatus	(*deleteSector)(TLrec *, SectorNo sectorNo, int noOfSectors);
  void		(*tlSetBusy)(TLrec *, FLBoolean);
  void		(*dismount)(TLrec *);

  #if defined(DEFRAGMENT_VOLUME) || defined(SINGLE_BUFFER)
  FLStatus	(*defragment)(TLrec *, long FAR2 *bytesNeeded);
  #endif

  #ifdef FORMAT_VOLUME
  SectorNo 	(*sectorsInVolume)(TLrec *);
  #endif
};


#ifdef FORMAT_VOLUME
#include "dosformt.h"
#endif

/* Translation layer registration information */

extern int noOfTLs;	/* No. of translation layers actually registered */

typedef struct {
  FLStatus (*mountRoutine) (FLFlash *flash, TL *tl, FLFlash **volForCallback);
#ifdef FORMAT_VOLUME
  FLStatus (*formatRoutine) (FLFlash *flash, FormatParams FAR1 *formatParams);
#endif
} TLentry;

extern TLentry tlTable[];


extern FLStatus	flMount(unsigned volNo, TL *, FLFlash *);

#ifdef FORMAT_VOLUME
extern FLStatus	flFormat(unsigned, FormatParams FAR1 *formatParams);
#endif

#ifdef __cplusplus
}
#endif

#endif

