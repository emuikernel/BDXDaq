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
//	Cascade Button Widget Interface
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaCbtnInterface.cc,v $
//   Revision 1.3  1998/04/08 17:28:50  heyes
//   get in there
//
//   Revision 1.2  1997/07/30 14:12:12  heyes
//   add more xpm support
//
//   Revision 1.1.1.1  1996/10/10 19:24:55  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <XcodaCbtnInterface.h>
#include <codaComd.h>
#include <codaComdList.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>

XcodaCbtnInterface::XcodaCbtnInterface(Widget parent,
				       codaComd *cmd): 
				       codaComdXInterface (parent,
							   cmd)
{
#ifdef _TRACE_OBJECTS
  printf("Create XcodaCbtnInterface Object \n");
#endif
  // empty
}
void XcodaCbtnInterface::init()
{
  int    i;
  Arg    arg[10];
  int    ac = 0;
  Pixmap map;

  XtSetArg (arg[ac], XmNbackgroundPixmap, &map); ac++;
  XtGetValues (_parent, arg, ac); 

  ac = 0;  

  if (map) {
    XtSetArg (arg[ac], XmNbackgroundPixmap, map); ac++;
  }
  _pulldown = XmCreatePulldownMenu (_parent, "pulldown", arg, ac);

  ac = 0;  
  XtSetArg (arg[ac], XmNsubMenuId, _pulldown); ac++;
  if (map) {
    XtSetArg (arg[ac], XmNbackgroundPixmap, map); ac++;
  }
  _w = XtCreateWidget(_name, 
		      xmCascadeButtonWidgetClass,
		      _parent,
		      arg, ac);
  ac = 0;
  
  installDestroyHandler();

  if(_active)
    activate();
  else
    deactivate();
  XtManageChild (_w);
}

XcodaCbtnInterface::~XcodaCbtnInterface()
{
#ifdef _TRACE_OBJECTS
  printf("Create XcodaCbtnInterface \n");
#endif
}


void XcodaCbtnInterface::addCommands(codaComdList *list)
{
  int i;
  codaComd *cmd;

  for (i=0; i < list->size(); i++){
    cmd = (*list)[i];
    cmd->createXInterface(_pulldown);
  }
}

const char *XcodaCbtnInterface::className() const
{
  return "XcodaCbtnInterface";
}



