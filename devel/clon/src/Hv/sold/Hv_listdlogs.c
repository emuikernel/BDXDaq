/*
==================================================================================
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include "Hv.h"

/*-------- Hv_CheckListItem --------*/

extern Boolean         Hv_CheckListItem(Hv_Widget list,
					char *str,
					int  **poslist,
					int  *num)
/* Hv wrapper to Motif function that
   sees if given str appears in the list.
   Caller must free poslist */

{
  XmString    xmst;
  Boolean     check;
 
  xmst = Hv_CreateMotifString(str, Hv_fixed2);
  check =  XmListGetMatchPos(list, xmst, poslist, num);

/* Motif seems to have num as undefined if not found which seems
   a little silly */

  if (!check) {
    *num = 0;
    *poslist = NULL;
  }

  XmStringFree(xmst);
  return check;
}




/*----------- Hv_AddListItem ---------*/

extern void    Hv_AddListItem(Hv_Widget list,
			      char      *str,
			      Boolean   selected)

{
  XmString    xmst;
 
  xmst = Hv_CreateMotifString(str, Hv_fixed2);
 
  if (selected) {
    XmListAddItem(list, xmst, 0);
    XmListSelectItem(list, xmst, True);
  }
  else
    XmListAddItemUnselected(list, xmst, 0);

  XmStringFree(xmst);
}

/*--------- Hv_PosReplaceListItem -------*/

void  Hv_PosReplaceListItem(Hv_Widget list,
			    int       pos,
			    char      *str)

{
  XmString    xmst;
 
  xmst = Hv_CreateMotifString(str, Hv_fixed2);
 
  XmListReplaceItemsPos(list, &xmst, 1, pos);
  XmStringFree(xmst);
}

/*----------- Hv_PosSelectListItem ---------*/

void  Hv_PosSelectListItem(Hv_Widget list,
			   int       pos)

/* not pos is NOT a C index */

{
  if (pos <= 0) {
    Hv_DeselectAllListItems(list);
    return;
  }
  
  XmListSelectPos(list, pos, True);
}

/*----------- Hv_NewPosSelectListItem ---------*/

void  Hv_NewPosSelectListItem(Hv_Widget list,
			      int       pos,
			      Boolean   CB)

/* note pos is NOT a C index */

{
  XmListSelectPos(list, pos, CB);
}

/*----------- Hv_SelectListItem ---------*/

extern void    Hv_SelectListItem(Hv_Widget list,
				 char     *str)

{
  XmString    xmst;
 
  if (str == NULL) {
    Hv_DeselectAllListItems(list);
    return;
  }

  xmst = Hv_CreateMotifString(str, Hv_fixed2);
  XmListSelectItem(list, xmst, True);
  XmStringFree(xmst);
}

/*----------- Hv_DeleteAllListItems ---------*/

void    Hv_DeleteAllListItems(Hv_Widget list)

{
  XmListDeleteAllItems(list);
}

/*----------- Hv_DeselectAllListItems ---------*/

void    Hv_DeselectAllListItems(Hv_Widget list)

{
  XmListDeselectAllItems(list);
}


/*----------- Hv_GetListSelectedPos ---------*/

int  Hv_GetListSelectedPos(Hv_Widget list)

/* note pos is NOT a C index */

{
  int     *pos;
  int     count;
  int     result;

  if (XmListGetSelectedPos(list, &pos, &count))
    result =  pos[0];
  else
    result =  -1;

  XtFree((char *)pos);
  return result;
} 


/*---------- Hv_GetAllListSelectedPos ----------*/


int  *Hv_GetAllListSelectedPos(Hv_Widget list)

/*************************************************
  Replacement for Hv_GetListSelectedPos. This routine
  returns all selected items so that it can be used
  in lists where you need to choose more than one.

  Returns:

    NULL if none selected;

    An int array with one entry for each selection,
    terminated by a -1
*************************************************/

{
  int     *pos;
  int     count;
  int     *result;
  int     i;

  if (XmListGetSelectedPos(list, &pos, &count)){

/* copy to the returned array */

    result = (int *)Hv_Malloc((count+1)*sizeof(int));
    for (i = 0; i < count; i++)
      result[i] = pos[i];
    result[count] = -1;
    XtFree((char *)pos);
    return result;
  }
  else
    return NULL;

}





