//-----------------------------------------------------------------------------
// Copyright (c) 1991,1992 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
// Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
//-----------------------------------------------------------------------------
// 
// Description:
//	Coda Xpm PushButton Command Header file
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaXpmpbtnComd.h,v $
//   Revision 1.3  1998/04/08 17:29:56  heyes
//   get in there
//
//   Revision 1.2  1997/07/30 14:12:22  heyes
//   add more xpm support
//
//   Revision 1.1.1.1  1996/10/10 19:25:00  chen
//   coda motif C++ library
//
//

#ifndef _CODA_XPMBTN_COMD
#define _CODA_XPMBTN_COMD

#include <codaPbtnComd.h>

class XcodaXpmpbtnInterface;

class codaXpmpbtnComd: public codaPbtnComd
{
public:
  codaXpmpbtnComd(char *, char **, int);
  codaXpmpbtnComd(char *, char **, int, char *);
  virtual ~codaXpmpbtnComd ();
  
  virtual void createXInterface(Widget);
  virtual const char *className (void) const {return "codaXpmpbtnComd";}

private:
  char *_name;
  char **_xpm_data;
  char *_bgsymbol;
};
#endif
