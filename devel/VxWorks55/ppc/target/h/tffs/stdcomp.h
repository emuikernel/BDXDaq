
/*
 * $Log:   P:/user/amir/lite/vcs/stdcomp.h_v  $
   
      Rev 1.1   08 Jan 1998 16:40:00   Hdei
   Added i28f008BajaIdentify routine for hkbaja47.
   
      Rev 1.1   18 Aug 1997 15:49:00   danig
   PCIC registration routine receives window base address
   
      Rev 1.0   07 Jul 1997 15:24:04   amirban
   Initial revision.
 */

/************************************************************************/
/*                                                                      */
/*		FAT-FTL Lite Software Development Kit			*/
/*		Copyright (C) M-Systems Ltd. 1995-1997			*/
/*									*/
/************************************************************************/

#ifndef STDCOMP_H
#define STDCOMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flbase.h"
#include "fltl.h"
#include "flflash.h"


/* MTD Identify Routines */

FLStatus amdMTDIdentify (FLFlash vol);
FLStatus i28f008Identify (FLFlash vol);
FLStatus i28f008BajaIdentify (FLFlash vol);
FLStatus i28f016Identify (FLFlash vol);
FLStatus cdsnIdentify (FLFlash vol);
FLStatus doc2Identify (FLFlash vol);
FLStatus cfiscsIdentify (FLFlash vol);
FLStatus cfiAmdIdentify (FLFlash vol);
FLStatus flwAmdMTDIdentify (FLFlash vol);

/* TL Mount and Format Routines */

FLStatus mountFTL (FLFlash *flash, TL *tl, FLFlash **volForCallback);
FLStatus formatFTL (FLFlash *flash, FormatParams FAR1 *formatParams);
FLStatus mountNFTL (FLFlash *flash, TL *tl, FLFlash **volForCallback);
FLStatus formatNFTL (FLFlash *flash, FormatParams FAR1 *formatParams);
FLStatus mountSSFDC (FLFlash *flash, TL *tl, FLFlash **volForCallback);
FLStatus formatSSFDC (FLFlash *flash, FormatParams FAR1 *formatParams);


/*----------------------------------------------------------------------*/
/*    	    Component registration routine in CUSTOM.C			*/
/*----------------------------------------------------------------------*/

void	flRegisterComponents(void);

#ifdef __cplusplus
}
#endif

#endif /* STDCOMP_H */
