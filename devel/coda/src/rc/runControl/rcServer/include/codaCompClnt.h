//-----------------------------------------------------------------------------
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
//      CODA remote component C interface
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaCompClnt.h,v $
//   Revision 1.6  1998/11/05 20:11:40  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.5  1997/02/03 13:47:32  heyes
//   add ask command
//
//   Revision 1.4  1996/11/27 15:06:10  chen
//   fix CDEV and Set internal itcl component state before doing state transition
//
//   Revision 1.3  1996/10/31 15:56:06  chen
//   Fixing boot stage bug + reorganize code
//
//   Revision 1.2  1996/10/28 14:23:02  heyes
//   communication with ROCs changed
//
//   Revision 1.1.1.1  1996/10/11 13:39:19  chen
//   run control source
//
//
#ifndef _CODA_COMP_CLNT_H
#define _CODA_COMP_CLNT_H

extern int codaDaCreate     (char* name, int version, int id, 
			     char* type, char* host, int interval);
extern int codaDaConnect    (char* name, int version, int id,
			     char* type, char* host);
extern int codaDaRemove     (char* name);
extern int codaDaDownload   (char* name, char* paramL);
extern int codaDaPrestart   (char* name, int rn, int rt);
extern int codaDaEnd        (char* name, int special);
extern int codaDaPause      (char* name, int mask);
extern int codaDaGo         (char* name, int mask);
extern int codaDaConfigure  (char* name, int type);
extern int codaDaReset      (char* name);
extern int codaDaTerminate  (char* name);
extern int codaDaReport     (char* name, int how);
extern int codaDaReadInt    (char* name, char* attr, long* value);
extern int codaDaWriteInt   (char* name, char* attr, long* value);
extern int codaDaZap        (char* name);
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
// functions related to itcl coda daq component
extern int codaCreateDaComp (char* name);
extern int codaDeleteDaComp (char* name);
extern int codaDaCompConfigure (char* name);
extern int codaDaCompSetState  (char* name, int state);
extern int codaAskComponent (char *command, char **res);
#endif

extern int compTimeOut;

#endif
  
