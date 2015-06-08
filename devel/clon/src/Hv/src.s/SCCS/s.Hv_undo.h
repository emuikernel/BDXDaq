h44191
s 00000/00000/00000
d R 1.2 02/09/09 11:30:53 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/s/Hv_undo.h
e
s 00059/00000/00000
d D 1.1 02/09/09 11:30:52 boiarino 1 0
c date and time created 02/09/09 11:30:52 by boiarino
e
u
U
f e 0
t
T
I 1
/*
 *  Hv_private.h is the header containing definitions for the undo menu
 *
 *  These should not be used by developers, but since this file is
 *  included in Hv.h, they are available. (There is no implied
 *  promise that these routines will be available in future releases)
 *  
 *  The Hv library was developed at CEBAF under contract to the
 *  Department of Energy and is the property of they U.S. Government.
 *  
 *  It may be used to develop commercial software, but the Hv
 *  source code and/or compiled modules/libraries may not be sold.
 *  
 *  Questions or comments should be directed to heddle@cebaf.gov
 */


#ifndef  __HVUNDO__
#define __HVUNDO__

/* entries in the view undo submenu */

#define Hv_LASTITEMDRAG     0
#define Hv_LASTITEMROTATE   1

#define Hv_NUMUNDOITEMS     2

extern Hv_Widget    Hv_lastItems[Hv_NUMUNDOITEMS];

#ifdef __cplusplus
extern "C"
{
#endif

extern void	Hv_DoUndo(Hv_Widget	w,
			  Hv_Pointer	client_data,
			  Hv_Pointer	call_data);


extern void     Hv_CheckUndoMenu(Hv_View   View );

#endif
#ifdef __cplusplus
}
#endif














E 1
