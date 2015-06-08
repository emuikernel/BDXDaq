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
//	Command X-Window Interface
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaComdXInterface.cc,v $
//   Revision 1.3  1998/04/08 17:29:52  heyes
//   get in there
//
//   Revision 1.2  1997/06/06 19:00:43  heyes
//   new RC
//
//   Revision 1.1.1.1  1996/10/10 19:24:57  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <codaComdXInterface.h>
#include <codaComd.h>

codaComdXInterface::codaComdXInterface ( Widget parent, codaComd *cmd ) :
XcodaUi( cmd->name() )
{
#ifdef _TRACE_OBJECTS
  printf("Create codaComdXInterface Object \n");
#endif
  _active = TRUE;
  _cmd    = cmd;
  _parent = parent;
  cmd->registerXInterface ( this );
}

codaComdXInterface::~codaComdXInterface()
{
#ifdef _TRACE_OBJECTS
  printf(" Delete codaComdXInterface Object \n");
#endif
}

void codaComdXInterface::executeComdCallback ( Widget, 
					     XtPointer clientData,
					     XtPointer )
{
    codaComdXInterface *obj = (codaComdXInterface *) clientData;
    
    obj->_cmd->execute();     
}

Widget codaComdXInterface::realBaseWidget()
{
  return _w;
} 

void codaComdXInterface::activate()
{
    if ( _w )
	XtSetSensitive ( _w, TRUE );
    _active = TRUE;
}

void codaComdXInterface::deactivate()
{
    if ( _w )
	XtSetSensitive ( _w, FALSE );
    _active = FALSE;
}

void codaComdXInterface::widgetDestroyed()
{
  _w = NULL;
}

const char *codaComdXInterface::className() const
{
  return "codaComdXInterface";
}
