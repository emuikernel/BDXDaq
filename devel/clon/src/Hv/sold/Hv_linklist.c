

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





