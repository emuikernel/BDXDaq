
/*
==================================================================================
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or and compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================

---------------------------------------------------------------------------
-
-   	File:     linklist.h
-
-
-										
-  Revision history:								
-                     							        
-  Date       Programmer     Comments						
--------------------------------------------------------------------------------
-  10/19/94	DPH		Initial Version					
-------------------------------------------------------------------------------
*/


#ifndef __LINKLISTH__
#define __LINKLISTH__

typedef struct hvgenericlistnode  *Hv_ListPtr;     

typedef struct hvgenericlistnode
{
  Hv_ListPtr   next;
} Hv_GenericListNode; 

#ifdef __cplusplus
extern "C"
{
#endif

/*------- Hv_linklist.c -------------*/

extern  int           Hv_CountListItems(Hv_ListPtr);

extern  Hv_ListPtr    Hv_PosGetListItem(Hv_ListPtr,
					int);

extern  void          Hv_SwapListItems(Hv_ListPtr *,
				       Hv_ListPtr,
				       Hv_ListPtr);

extern  void          Hv_InsertListItem(Hv_ListPtr *,
					Hv_ListPtr);

extern  void          Hv_RemoveListItem(Hv_ListPtr *,
					Hv_ListPtr);

extern  Hv_ListPtr    Hv_PreviousListItem(Hv_ListPtr,
					  Hv_ListPtr);

#ifdef __cplusplus
}
#endif

#endif







