/* vxwObject.h - behavior common to all VXW clases */

/* Copyright 1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,15jun95,srh  written.
*/

#ifndef vxwObject_h
#define vxwObject_h

class VXWIdObject
    {
  public:
    virtual void * myValue () = 0;
    };

#endif /* ifndef vxwObject_h */
