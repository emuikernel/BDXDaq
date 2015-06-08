/* CoErrorDecodeImpl.h -- auto-generated COM class header */

/* Copyright (c) 2001 Wind River Systems, Inc. */

/*

modification history
--------------------
01c,15nov01,nel  Merge from /main/LATEST.
01a,09aug01,nel  created

*/

#ifndef _CoErrorDecodeImpl_h
#define _CoErrorDecodeImpl_h

#include "comObjLib.h"			// COM-object template lib
#include "CoErrorDecode.h"		// IDL-output interface defs

/*

DESCRIPTION

This file implements the CoErrorDecode CoClass which has one interface
call IDecode. This contains methods for decoding an HRESULT into 
a string representation.

*/

class CoErrorDecodeImpl
    : public CComObjectRoot,
      public CComCoClass<CoErrorDecodeImpl, &CLSID_CoErrorDecode>,
      public IDecode
    {
  public:

    DECLARE_CLASSFACTORY_SINGLETON ();

    CoErrorDecodeImpl () {}
    ~CoErrorDecodeImpl () {}
    
    // Interface methods go here...
    STDMETHOD (GetErrorString) (HRESULT errorNum, BSTR * pStr);
	
    // COM Interface map
    BEGIN_COM_MAP(CoErrorDecodeImpl)
        COM_INTERFACE_ENTRY(IDecode)
    END_COM_MAP()

  private:
    // Private instance variables go here...

    };

typedef CComObject<CoErrorDecodeImpl> CoErrorDecodeClass;

#endif

