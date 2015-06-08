// VXWSmName/vxwSmNameLib.h - naming behavior common to all shared memory classes

// Copyright 1995-1999 Wind River Systems, Inc.

// modification history
// --------------------
// 01b,23feb99,fle  doc : made it refgen compliant
// 01c,21feb99,jdi  added library section, checked in documentation.
// 01b,30sep95,rhp  documented.
// 01a,15jun95,srh  written.

// DESCRIPTION
// This class library provides facilities for managing entries in the shared
// memory objects name database.  The shared memory objects name
// database associates a name and object type with a value and makes
// that information available to all CPUs.  A name is an arbitrary,
// null-terminated string.  An object type is a small integer, and its
// value is a global (shared) ID or a global shared memory address.
//
// Names are added to the shared memory name database with
// VXWSmName::VXWSmName().  They are removed by VXWSmName::~VXWSmName().
//
// Name database contents can be viewed using smNameShow().
//
// The maximum number of names to be entered in the database SM_OBJ_MAX_NAME is
// defined in configAll.h.  This  value is used to determine the size of a
// dedicated shared memory partition from which name database fields are
// allocated.
//
// The estimated memory size required for the name database can be calculated
// as follows:
//
// .CS
//     <name database pool size> = SM_OBJ_MAX_NAME * 40 (bytes)
// .CE
//
// The display facility for the shared memory objects name database is provided
// by smNameShow.
//
// CONFIGURATION
// Before routines in this library can be called, the shared memory object
// facility must be initialized by calling usrSmObjInit(), which is found in
// src/config/usrSmObj.c.  This is done automatically from the root
// task, usrRoot(), in usrConfig.c if INCLUDE_SM_OBJ is defined in
// configAll.h.
//
// AVAILABILITY
// This module depends on code that is distributed as a component of
// the unbundled shared memory objects support option, VxMP.
//
// INCLUDE FILES: vxwSmNameLib.h
//
// SEE ALSO: smNameLib, smNameShow, vxwSmLib, smObjShow, usrSmObjInit(),
// .pG "Shared Memory Objects"
//
// SECTION: 1C
//

#ifndef vxwSmNameLib_h
#define vxwSmNameLib_h

#include "vxWorks.h"
#include "smNameLib.h"
#include "vxwErr.h"
#include "vxwObject.h"

class VXWSmName : virtual public VXWIdObject
    {
  public:

//_ VXWSmName Public Constructors

///////////////////////////////////////////////////////////////////////////////
//
// VXWSmName::~VXWSmName - remove an object from the shared memory objects name database (VxMP Option)
//
// This routine removes an object from the shared memory objects
// name database. 
// 
// AVAILABILITY
// This routine depends on code distributed as a component of the unbundled
// shared memory objects support option, VxMP.
// 
// RETURNS: OK, or ERROR if the database is not initialized, or the 
// name-database lock times out.
//
// ERRNO: 
//  S_smNameLib_NOT_INITIALIZED  
//  S_smObjLib_LOCK_TIMEOUT

    virtual ~VXWSmName ();

//_ VXWSmName Public Member Functions

///////////////////////////////////////////////////////////////////////////////
//
// VXWSmName::nameSet - define a name string in the shared-memory name database (VxMP Option)
//
// This routine adds a name of the type appropriate for each derived class
// to the database of memory object names.
//
// The <name> parameter is an arbitrary null-terminated string with a 
// maximum of 20 characters, including EOS.
//
// A name can be entered only once in the database, but there can be more
// than one name associated with an object ID. 
//
// AVAILABILITY
// This routine depends on the unbundled shared memory
// objects support option, VxMP.
// 
// RETURNS: OK, or ERROR if there is insufficient memory for <name> to be 
// allocated, if <name> is already in the database, or if the database is 
// already full.
//
// ERRNO: 
//  S_smNameLib_NOT_INITIALIZED  
//  S_smNameLib_NAME_TOO_LONG  
//  S_smNameLib_NAME_ALREADY_EXIST 
//  S_smNameLib_DATABASE_FULL 
//  S_smObjLib_LOCK_TIMEOUT

    virtual STATUS nameSet (char * name) = 0;

///////////////////////////////////////////////////////////////////////////////
//
// VXWSmName::nameGet - get name and type of a shared memory object (VxMP Option)
//
// This routine searches the shared memory name database for an object matching
// this VXWSmName instance.  If the object is found, its name and type are
// copied to the addresses pointed to by <name> and <pType>.  The value of
// <waitType> can be one of the following:
// .iP "NO_WAIT (0)"
// The call returns immediately, even if the object value is not in the database
// .iP "WAIT_FOREVER (-1)"
// The call returns only when the object value is available in the database.
// .LP
//
// AVAILABILITY
// This routine depends on the unbundled shared memory
// objects support option, VxMP.
// 
// RETURNS: OK, or ERROR if <value> is not found or if the wait type is invalid.
//
// ERRNO: 
//  S_smNameLib_NOT_INITIALIZED 
//  S_smNameLib_VALUE_NOT_FOUND 
//  S_smNameLib_INVALID_WAIT_TYPE  
//  S_smObjLib_LOCK_TIMEOUT

    STATUS nameGet (char * name, int * pType, int waitType)
	{
	return smNameFindByValue (myValue (), name, pType, waitType);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWSmName::nameGet - get name of a shared memory object (VxMP Option)
//
// This routine searches the shared memory name database for an object matching
// this VXWSmName instance.  If the object is found, its name is copied
// to the address pointed to by <name>.  The value of <waitType> 
// can be one of the following:
// .iP "NO_WAIT (0)"
// The call returns immediately, even if the object value is not in the database
// .iP "WAIT_FOREVER (-1)"
// The call returns only when the object value is available in the database.
// .LP
//
// AVAILABILITY
// This routine depends on the unbundled shared memory
// objects support option, VxMP.
// 
// RETURNS: OK, or ERROR if <value> is not found or if the wait type is invalid.
//
// ERRNO: 
//  S_smNameLib_NOT_INITIALIZED 
//  S_smNameLib_VALUE_NOT_FOUND 
//  S_smNameLib_INVALID_WAIT_TYPE  
//  S_smObjLib_LOCK_TIMEOUT

    STATUS nameGet (char * name, int waitType)
	{
	int ignore;
	return nameGet (name, &ignore, waitType);
	}
  protected:
    };

#endif /* ifndef vxwSmNameLib_h */
