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
//	Coda Xpm toggle Button Command Header file
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaXpmtbtnComd.h,v $
//   Revision 1.2  1998/04/08 17:29:58  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:25:00  chen
//   coda motif C++ library
//
//
#ifndef _CODA_XPMTBTN_H
#define _CODA_XPMTBTN_H

#include <codaTbtnComd.h>

class XcodaXpmtbtnInterface;

class codaXpmtbtnComd: public codaTbtnComd
{
public:
  codaXpmtbtnComd(char *name, char **pix, int active, int state = 0);
  codaXpmtbtnComd(char *name, char **pix, int active, int notoggle, 
		  int state = 0);
  codaXpmtbtnComd(char *name, char **pix, int active, char *bgsymbol, 
		  int notoggle, int state = 0);
  codaXpmtbtnComd(char *name, char **pix, char **sel_pix, int active,
		  char *bgsymbol, int state = 0);
  virtual ~codaXpmtbtnComd ();
  virtual void createXInterface(Widget parent);
  virtual const char *className() const {return "codaXpmtbtnComd";};

private:
  char **_xpm_data;
  char **_sel_xpm;
  char *_bgsymbol;
  int  _initState;
  int  _notoggle;
};
#endif
