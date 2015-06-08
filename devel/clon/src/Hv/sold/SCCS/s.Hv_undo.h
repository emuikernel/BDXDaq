h33222
s 00000/00000/00000
d R 1.2 02/08/25 23:21:24 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_undo.h
e
s 00057/00000/00000
d D 1.1 02/08/25 23:21:23 boiarino 1 0
c date and time created 02/08/25 23:21:23 by boiarino
e
u
U
f e 0
t
T
I 1
/*	
-----------------------------------------------------------------------
File:     Hv_undo.h
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
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

extern void	Hv_DoUndo(Widget	w,
			  XtPointer	client_data,
			  XtPointer	call_data);


extern void     Hv_CheckUndoMenu(Hv_View   View );

#endif
#ifdef __cplusplus
}
#endif














E 1
