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
//     Generic Command Class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaComd.cc,v $
//   Revision 1.3  1998/04/08 17:29:50  heyes
//   get in there
//
//   Revision 1.2  1997/07/30 14:12:20  heyes
//   add more xpm support
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <codaComd.h>
#include <codaComdList.h>
#include <codaComdXInterface.h>

codaComd::codaComd ( char *name, int active )
{
#ifdef _TRACE_OBJECTS
  printf("Create codaComd Object \n");
#endif
  // Initialize all data members
    
  _name              = name;  
  _active            = active;  
  _ci                = NULL;
  _activationList    = NULL;
  _deactivationList  = NULL;
  _hasUndo           = TRUE;
  _redo              = 0;
}

codaComd::~codaComd()
{
#ifdef _TRACE_OBJECTS
  printf("Destroy codaComd Object \n");
#endif
  delete _activationList;	
  delete _deactivationList;	
  // do not delete _ci here 
  // _ci is only a pointer to a subclass of codaComdXInterface
}

void codaComd::registerXInterface ( codaComdXInterface *ci )
{
  _ci =  ci;
    
  if ( ci )
    if ( _active )
      ci->activate();
    else
      ci->deactivate();      
}

codaComdXInterface *codaComd::getXInterface()
{
  return _ci;
}

int codaComd::hasXInterface()
{
  if (_ci != NULL)
    return 1;
  else
    return 0;
}

void codaComd::activate()
{
  // Activate the associated interfaces
    
  _ci->activate ();
  
  // Save the current value of active before setting the new state
  
  _previouslyActive = _active;
  _active = TRUE;
}

void codaComd::deactivate()
{
  // Deactivate the associated interfaces
  
  _ci->deactivate ();
    
  // Save the current value of active before setting the new state
  
  _previouslyActive = _active;
  _active = FALSE;
}

void codaComd::revert()
{
  // Activate or deactivate, as necessary, 
  // to return to the previous state
    
  if ( _previouslyActive )
    activate();
  else
    deactivate();
}

void codaComd::addToActivationList ( codaComd *cmd )
{
  if ( !_activationList )
    _activationList = new codaComdList();
    
  _activationList->add ( cmd );
}

void codaComd::addToDeactivationList ( codaComd *cmd )
{
  if ( !_deactivationList )
    _deactivationList = new codaComdList();
  
  _deactivationList->add ( cmd );
}

void codaComd::execute()
{
  int i;      
  
  // If a command is inactive, it cannot be executed
  
  if ( !_active )
    return;
    
  // Call the derived class's doit member function to 
  // perform the action represented by this object
    
  // turn off redo flag here
  _redo = 0;
  doit();
  
  // Process the commands that depend on this one
  
  if ( _activationList )    
    for ( i = 0; i < _activationList->size(); i++ )
      (*_activationList)[i]->activate();
  
  if ( _deactivationList )    
    for ( i = 0; i < _deactivationList->size(); i++ )
      (*_deactivationList)[i]->deactivate();
}

void codaComd::undo()
{
  int i;
  
  // Call the derived class's undoit() member function.
  
  if (!_redo){
    _redo = 1;
    undoit();
    
    // Reverse the effects of the execute() member function by 
    // reverting all dependent objects to their previous state
    
    if ( _activationList )        
      for ( i = 0; i < _activationList->size(); i++ )
	(*_activationList)[i]->revert();
    
    if ( _deactivationList )    
      for ( i = 0; i < _deactivationList->size(); i++ )
	(*_deactivationList)[i]->revert();
  }
  else{
    _redo = 0;
    execute();
  }
}


void codaComd::doit()
{
}

void codaComd::undoit()
{
}
