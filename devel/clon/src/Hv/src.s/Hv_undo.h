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














