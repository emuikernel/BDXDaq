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
//	Generic Command Class Header File
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaComd.h,v $
//   Revision 1.3  1998/04/08 17:29:50  heyes
//   get in there
//
//   Revision 1.2  1997/07/30 14:12:21  heyes
//   add more xpm support
//
//   Revision 1.1.1.1  1996/10/10 19:24:59  chen
//   coda motif C++ library
//
//

#ifndef _CODA_COMD_H
#define _CODA_COMD_H

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

class codaComdList;
class codaComdXInterface;

class codaComd
{
public:
  // Destructor    
  virtual ~codaComd ();                     
    
  // public interface for executing and undoing commands
  virtual void       execute();  
  void               undo();
    
  void               activate();   // Activate this object
  void               deactivate(); // Deactivate this object
    
  // Functions to register dependent commands
    
  void               addToActivationList ( codaComd * );
  void               addToDeactivationList ( codaComd * );
    
  // Register an Xinterface  used to execute this command
    
  void               registerXInterface ( codaComdXInterface * );
  codaComdXInterface *getXInterface();
  int                hasXInterface();
  virtual void       createXInterface (Widget) = 0;
    
  // Access functions 
    
  int active ()      { return _active; }
  int hasUndo()      { return _hasUndo; }
  const char *name () const { return _name; }

  // class name
  virtual const char *className () const { return "codaComd"; } ;

protected:
  // protect constructor
  codaComd ( char *,  int );                 

  int                _hasUndo;          // True if this object supports undo
  virtual void       doit();      // Specific actions must be defined
  virtual void       undoit();      // by derived classes
    
private:
    
  // Lists of other commands to be activated or deactivated
  // when this command is executed or "undone"
    
  codaComdList       *_activationList;
  codaComdList       *_deactivationList;
  void               revert();          // Reverts object to previous state
  int                _active;           // Is this command currently active?
  int                _previouslyActive; // Previous value of _active
  char               *_name;            // Name of this Cmd
  int                _redo;             // Redo this command after undo
  codaComdXInterface *_ci;              // one interface with one command
};
#endif
