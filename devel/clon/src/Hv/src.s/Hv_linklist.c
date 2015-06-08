

/*
==================================================================================
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================


----------------------------------------------------
-							
-  File:     Hv_linklist.c					
-							
-  Summary:						
-     list functions applicable to lists modeled
-     after the generic list: The FIRST field MUST
-     be the next pointer, the second field must
-     be the name.
-

================= NOTE =====================

  THESE ROUTINES WORK ON SINGLY LINKED LISTS WITH 
  THE FOLLOWING ASSUMPTION

  THE FIRST FIELD OF THE LIST NODE MUST BE
  THE "next" POINTER.  YOUR NODE MUST BE CAST
  TO THE Hv_ListPtr TYPE:


     typedef struct hvgenericlistnode  *Hv_ListPtr;     
       
     typedef struct hvgenericlistnode
     {
       ListPtr   next;
     } Hv_GenericListNode; 


-  Revision history:				
-                     				
-  Date       Programmer     Comments		
------------------------------------------------------
-  2/20/95	DPH		Initial Version	
-                                              	
------------------------------------------------------
*/

#include "Hv.h"
#include "Hv_linklist.h"

/*-------- Hv_SwapListItems -------*/

void  Hv_SwapListItems(Hv_ListPtr *start,
		       Hv_ListPtr  a,
		       Hv_ListPtr  b)

/* start will be altered if it is equal to a or b */

{
  Hv_ListPtr     aprev, bprev, temp;

/* if they are they same, just return */

  if (a == b)
    return;

/* if b follows a, special treatment */

  if (a->next == b) {
    temp = b->next;
    b->next = a;
    a->next = temp;
    aprev = Hv_PreviousListItem(*start, a);
    if (aprev != NULL)
      aprev->next = b; 
    else
      *start = b;
    return;
  }

/* if a follows b, special treatment */
  
  if (b->next == a) {
    temp = a->next;
    a->next = b;
    b->next = temp;
    bprev = Hv_PreviousListItem(*start, b);
    if (bprev != NULL)
      bprev->next = a; 
    else
      *start = a;
    return;
  }
  
  aprev = Hv_PreviousListItem(*start, a);
  bprev = Hv_PreviousListItem(*start, b);

  temp = a->next;
  a->next = b->next;
  b->next = temp;

  if (aprev != NULL)
    aprev->next = b; 
  else
    *start = b;
  
  if (bprev != NULL)
    bprev->next = a;
  else
    *start = a;

}

/*-------- Hv_InsertListItem -------*/

void  Hv_InsertListItem(Hv_ListPtr *start,
			Hv_ListPtr a)

/* adds an item TO THE END OF THE LIST */

{
  Hv_ListPtr    temp;

  if (*start == NULL)
    *start = a;
  else {
    for (temp = *start; temp->next != NULL; temp = temp->next)
      ;
    temp->next = a;
  }

  a->next = NULL;
}

/*------ Hv_CountListItems ------*/

int  Hv_CountListItems(Hv_ListPtr start)

{
  Hv_ListPtr   temp;

  int count = 0;

  for (temp = start; temp != NULL; temp = temp->next)
    count++;

  return count;
}


/*----- Hv_PosGetListItem ----*/


Hv_ListPtr    Hv_PosGetListItem(Hv_ListPtr start,
				int        pos)

/* returns the NON-C indexed pos'th 
   item, or NULL if it doesn't find it */


{
  Hv_ListPtr  temp;
  int count = 1;

  if ((start == NULL) || (pos < 1))
    return NULL;

  for (temp = start; temp != NULL; temp = temp->next) {
    if (count == pos)
      return temp;
    count++;
  }
  
  return NULL;
}


/*-------- Hv_PreviousListItem -------*/

Hv_ListPtr   Hv_PreviousListItem(Hv_ListPtr start,
				 Hv_ListPtr  a) 

{
  Hv_ListPtr   prev = NULL;
  Hv_ListPtr   temp;

  for (temp = start; ((temp != a) &&  (temp != NULL)); temp = temp->next)
    prev = temp;

  return prev;

}

/*-------- Hv_RemoveListItem -----*/

void  Hv_RemoveListItem(Hv_ListPtr *start,
			Hv_ListPtr  a)

/* removes a from the list but does NOT free any space
    -- it is up to you to free the space after removal */

/* NOTE: does NOT check that a is actually on starts list.
   if a is part of a different list, disasterous cross
   linkage will ensue! */

{
  Hv_ListPtr    prev;

  if ((*start == NULL) || (a == NULL))
    return;

/* the easiest case is if we are removing the
   first item */

  if (*start == a) {
    *start = a->next;
    return;
  }

  prev = Hv_PreviousListItem(*start, a);
  prev->next = a->next;
}


Hv_LinkList	*Hv_DlistCreateList ( void )
{
  Hv_LinkList *temp;

  temp = (Hv_LinkList *) Hv_Malloc(sizeof(Hv_LinkList));
  temp->data = NULL;
  temp->next = NULL;
  temp->prev = NULL;
  return temp;

}

Hv_LinkList	*Hv_DlistEnque ( Hv_LinkList *List, void *data )
{
	return NULL;
}

void		*Hv_DlistDeque ( Hv_LinkList *List )
{
  Hv_LinkList	*ptemp;
  Hv_LinkList	*ntemp;


/* head pointer has the property that next points to itself*/

  if ((List == NULL) || (List->next == List))
	  return NULL;


  ptemp = List->prev;
  ntemp = List->next;

  /* check to see if we are deleteing the only item in the list */
  /* if so then set the head of the list to point to NULL       */

  /* dph: I do not think this should actually happen!! */

  if(ptemp == ntemp)
  {
	  ptemp->next = NULL;
	  ptemp->prev = NULL;
  }


  ptemp->next =ntemp;
  ntemp->prev =ptemp;
  return List;
}

Hv_LinkList	*Hv_DlistInsert ( Hv_LinkList *List, void *data, short where )
{
Hv_LinkList *lp;
Hv_LinkList *ln;
Hv_LinkList *temp;

  temp = Hv_DlistCreateNode();

  if(temp == NULL)
	  return NULL;

  lp = List->prev;
  ln = List->next;

  if(List->next == NULL) /* empty list so add to the front    */
	                     /* or the back if you are left handed */
  {
	  List->next       = temp;
      List->next->next = List;
	  List->prev       = List->next;
      List->prev->prev = List;
	  List->next->data = data;
  }
  else
  {
    if(where == Hv_STARTLIST) /* add the the front of the list */
	{
	  List->next       = temp;
      List->next->next = ln;
      ln->prev         = List->next;
	  List->next->prev = List;
	  List->next->data = data;
	}
    else if (where == Hv_ENDLIST) /* add the the end of the list */
	{
	  List->prev       = temp;
      List->prev->next = List;
      lp->next         = List->prev;
	  List->prev->prev = lp;
	  List->prev->data = data;
	}
  }
  return temp;
}

Hv_LinkList *Hv_DlistInsertSort(Hv_LinkList *timerList,void *timeStruct,unsigned long expireTime)
{
	Hv_LinkList *start;
	Hv_LinkList *lp;
    Hv_LinkList *temp;

  temp = Hv_DlistCreateNode();
  if(temp == NULL)
	  return NULL;
  temp->data = timeStruct;
  temp->value = expireTime;

  start = timerList;
  if(start->next == NULL) /* empty list so add to the front    */
	                     /* or the back if you are left handed */

  {
	  start->next       = temp;
      start->next->next = start;
	  start->prev       = start->next;
      start->prev->prev = start;
	  start->next->data = timeStruct;

	  return temp;
  }

  while(start->value < expireTime) /* find the place to insert */
  {
	  start = start->next;
	  if(start == timerList) /* scaned the whole list so insert at the end */
	  {
		lp = timerList->prev;
		timerList->prev       = temp;
		timerList->prev->next = timerList;
		lp->next              = timerList->prev;
		timerList->prev->prev = lp;
		timerList->prev->data = timeStruct;
		return temp;
	  }
  }


  temp->prev  = start->prev;
  start->prev = temp;
  temp->next  = start;
  temp->prev->next = temp;

  
  return temp;

}

void	     Hv_DlistFree ( Hv_LinkList *List )
{
Hv_LinkList *lp;
Hv_LinkList *temp;

	for (lp = List->next; lp != List; )
	{
		temp = lp;
		lp = lp->next;
		Hv_Free(temp);
	}
	Hv_Free(List);
}

Hv_LinkList	*Hv_DlistCreateNode ( void )
{
	return Hv_DlistCreateList();
}



