/* %CLASSNAME%CImpl.c - %CLASSNAME% header file */

/* Copyright 2001 <your company name here> */

/*
modification history
--------------------
01b,12feb02,nel  Add struct to vptr def in CoClass def.
01a,22oct01,nel  Created.
*/

/*
DESCRIPTION
This module implements the %CLASSNAME% CoClass.
*/

#ifndef __INC%CLASSNAME%CImpl_h
#define __INC%CLASSNAME%CImpl_h

typedef struct _%CLASSNAME%Class
    {
    const struct %IFNAME%Vtbl *lpVtbl;		/* vptr in C++ terms */
    DWORD               dwRefCount;	/* reference counter */
    } %CLASSNAME%_CLASS;

void %CLASSNAME%_Destructor
    (
    IUnknown *		pUnk
    );

HRESULT %CLASSNAME%_QueryInterface
    (
    IUnknown *          pUnk,
    REFIID              riid,
    void**              ppvObject
    );

ULONG %CLASSNAME%_AddRef
    (
    IUnknown *          pUnk
    );

ULONG %CLASSNAME%_Release
    (
    IUnknown *          pUnk
    );

%PROTO_BLOCK%
HRESULT %CLASSNAME%_%METHOD%
    (
    %IFNAME% *         pUnk%COMMA_METHOD%
    %METHOD_PROTO%
    );
%PROTO_END%

#endif /* __INC%CLASSNAME%_h */



