// VXWError/vxwErr.h - error handling class

// Copyright 1995-1999 Wind River Systems, Inc.

//
// modification history
// --------------------
// 01c,08jun99,dgp  doc: add descrip for correct refgen processing
// 01b,23feb99,fle  doc : made it refgen parsable
// 01a,15jun95,srh  written.
//

// DESCRIPTION
// The `VXWError' class provides a generic error handling facility.
//
// INCLUDE FILE: vxwErr.h
//
// SEE ALSO: <VxWorks Programmer's Guide: C++ Development>
//

#ifndef vxwErr_h
#define vxwErr_h

#include "errnoLib.h"

class VXWError
    {
  public:
    VXWError (const char *);
    VXWError (const int);
    VXWError (const VXWError &);
    virtual ~VXWError ();

    const char * why () const
	{
	return errorMessage_;
	}

  protected:
    VXWError ()
	{
	}
    void makeCopy (const char * s);
    const char * errorMessage_;
    };

void vxwSetErrHandler (void (*)(const VXWError &));
void vxwThrow (const VXWError & err);

inline void vxwThrowErrno ()
    {
    vxwThrow (VXWError (errnoGet ()));
    }

#endif /* #ifndef vxwErr_h */
