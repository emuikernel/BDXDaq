/* %CLASSNAME%CImpl.c - %CLASSNAME% implementation file */

/* Copyright 2001 <your company name here> */

/*
modification history
--------------------
01b,12feb02,nel  Correct formating of destructor parameter comments.
01a,22oct01,nel  Created.
*/

/*
DESCRIPTION
This module implements the %CLASSNAME% CoClass.

INCLUDE FILES: comCoreTypes.h comCoreLib.h %CLASSNAME%.h %CLASSNAME%CImpl.h
*/

/* includes */

#include "comCoreTypes.h"               /* base types */
#include "comCoreLib.h"                 /* core COM funcs */
#include "%CLASSNAME%.h"
#include "%CLASSNAME%CImpl.h"		/* CoClass header file */
#include "stdio.h"

/* globals */

%BLOCK%
COM_VTABLE(%IFNAME%) _%CLASSNAME%_vtbl = {
    COM_VTBL_HEADER
    /* IUnknown methods */
    COM_VTBL_METHOD (&%CLASSNAME%_QueryInterface),
    COM_VTBL_METHOD (&%CLASSNAME%_AddRef),
    COM_VTBL_METHOD (&%CLASSNAME%_Release),
    /* %IFNAME% methods */
%PROTO_BLOCK%
    COM_VTBL_METHOD (&%CLASSNAME%_%METHOD%)%COMMA_VT%
%PROTO_END%
};

%PROTO_BLOCK%
/**************************************************************************
*
* %CLASSNAME%_%METHOD% - This function implements %METHOD% of %IFNAME%.
*
* <A description of the routine should go here>
*
* RETURNS: S_OK on success, of an HRESULT on failure.
*
*/

HRESULT %CLASSNAME%_%METHOD%
    (
    %IFNAME% *		    pUnk%COMMA_METHOD%
    %METHOD_PROTO%
    )
    {
    %CLASSNAME%_CLASS *     pThis = (%CLASSNAME%_CLASS*) pUnk;

    return S_OK;
    }

%PROTO_END%
%END%

/**************************************************************************
*
* %CLASSNAME%_Destructor - This routine implements a C++ style destructor
*
* This function implements a C++ destructor for the CoClass. User code to
* tidy up any data should be written in this routine in the indicated
* place.
*
* RETURNS: N/A
*
*/

void %CLASSNAME%_Destructor 
    (
    IUnknown * 		pUnk		/* Pointer to the instance of this */
					/* class */
    )
    {
    %CLASSNAME%_CLASS *     pThis = (%CLASSNAME%_CLASS*) pUnk;

    /* User destructor code should go here */
    }

/**************************************************************************
*
* %CLASSNAME%_QueryInterface - This routine implements QueryInterface for the
* CoClass %CLASSNAME%
*
* This function implements a the QueryInterface method for the CoClass
* %CLASSNAME%
*
* RETURNS: S_OK on success, E_NOINTERFACE on failure.
*
*/

HRESULT %CLASSNAME%_QueryInterface
    (
    IUnknown *          pUnk,		/* A pointer to the instance of */
    					/* this class. */
    REFIID	        iid,		/* IID of the interface to bind */
					/* to. */
    void**	        ppv		/* Pointer to return result in */
    )
    {
%BLOCK%
    /* Is it one of our own interfaces? */
    if (comGuidCmp (iid, &IID_IUnknown) || comGuidCmp (iid, &IID_%IFNAME%))
	{
	*ppv = pUnk;
        IUnknown_AddRef (pUnk);
	return S_OK;
	}
%END%
    return E_NOINTERFACE;
    }

/**************************************************************************
*
* %CLASSNAME%_AddRef - This routine implements AddRef for the CoClass
*
* This function implements a the AddRef method for the CoClass
* %CLASSNAME%
*
* RETURNS: The reference count after it has been incremeneted.
*
*/

ULONG %CLASSNAME%_AddRef
    (
    IUnknown *          pUnk		/* A pointer to the instance of */
    					/* this class. */
    )
    {
    %CLASSNAME%_CLASS *     pThis = (%CLASSNAME%_CLASS*) pUnk;

    return ++pThis->dwRefCount;
    }

/**************************************************************************
*
* %CLASSNAME%_Release - This routine implements Release for the CoClass
*
* This function implements a the Release method for the CoClass
* %CLASSNAME%
*
* RETURNS: The reference count after it has been decremeneted.
*
*/

ULONG %CLASSNAME%_Release
    (
    IUnknown *          pUnk
    )
    {
    %CLASSNAME%_CLASS *     pThis = (%CLASSNAME%_CLASS*) pUnk;
    DWORD               n;

    n = --pThis->dwRefCount;
    if (n == 0)
        {
        /* Destroy this object */
	%CLASSNAME%_Destructor (pUnk);
        free (pThis);
        }
    return n;
    }
