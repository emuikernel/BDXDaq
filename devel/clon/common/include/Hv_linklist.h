
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

#define Hv_ENDLIST   2
#define Hv_STARTLIST 1

typedef struct hvgenericlistnode  *Hv_ListPtr;     

typedef struct hvgenericlistnode
{
  Hv_ListPtr   next;
} Hv_GenericListNode; 

typedef struct hvlinklist 
{
  unsigned long               value;   /* for insertion sorts */
  void               *data;
  struct hvlinklist *next;
  struct hvlinklist *prev;
} Hv_LinkList;


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

extern  Hv_LinkList	*Hv_DlistCreateList ( void );
extern  Hv_LinkList	*Hv_DlistEnque ( Hv_LinkList *, void * );
extern  void		*Hv_DlistDeque ( Hv_LinkList * );
extern  Hv_LinkList	*Hv_DlistInsert ( Hv_LinkList *, void *, short int );
extern  Hv_LinkList *Hv_DlistInsertSort(Hv_LinkList *timerList,
									  void *timeStruct,
									  unsigned long expireTime);

extern  void	     Hv_DlistFree ( Hv_LinkList * );
extern  Hv_LinkList	*Hv_DlistCreateNode ( void );


#ifdef __cplusplus
}
#endif

#endif







