#ifdef USE_TK

/*-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
//       coda@cebaf.gov  Tel: (804) 249-7030     Fax: (804) 249-5800
//-----------------------------------------------------------------------------
//
// Description:
//      CODA RunControl Interface to Possible Tcl Command
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcTclInterface.h,v $
//   Revision 1.6  1998/11/10 16:37:16  timmer
//   Linux port
//
//   Revision 1.5  1998/11/05 20:11:53  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.4  1996/10/31 15:56:13  chen
//   Fixing boot stage bug + reorganize code
//
//   Revision 1.3  1996/10/28 14:23:06  heyes
//   communication with ROCs changed
//
//   Revision 1.2  1996/10/14 20:02:53  heyes
//   changed message system
//
//   Revision 1.1.1.1  1996/10/11 13:39:20  chen
//   run control source
//
*/
#ifndef _RC_TCL_INTERFACE_H
#define _RC_TCL_INTERFACE_H

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
#include <stdio.h>
#include <assert.h>
#include <tcl.h>
#include <tk.h>
#include <dp.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* init a tcl interpeter                      */
extern int         initTcl      (void);
/* evaluate a tcl script                      */
extern char*       evalScript   (char* script, int* status);
/* event loop                                 */
extern void        tkOneEvent   (void);
/* a pointer to Tcl Interperter               */
extern Tcl_Interp  *Main_Interp;
/* interactive Tcl shell flag                 */
extern int         interactive;

#ifdef __cplusplus
};
#endif

#endif /* coda_2_0 */

#endif

#endif /*USE_TK*/