h10709
s 00000/00000/00000
d R 1.2 02/08/25 23:21:20 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_radiodlogs.c
e
s 00234/00000/00000
d D 1.1 02/08/25 23:21:19 boiarino 1 0
c date and time created 02/08/25 23:21:19 by boiarino
e
u
U
f e 0
t
T
I 1
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


/* ----------- data structs shown for reference ------------------

typedef struct hvradiolist  *Hv_RadioListPtr;

typedef struct hvradiolist
{
  Hv_Widget            radio;
  Hv_RadioListPtr      next;
  void                *data;
  short                pos;
} Hv_RadioList;

typedef struct  hvradiohead  *Hv_RadioHeadPtr;

typedef struct  hvradiohead
{
  Hv_Widget        activewidget;
  short            activepos;       !not a C INDEX
  Hv_RadioListPtr  head;
} Hv_RadioHead;

--------------------*/

/*----------- Hv_RadioListSensitivity ----------*/

void Hv_RadioListSensitivity(Hv_RadioHeadPtr   radiohead,
			     short        pos,
			     Boolean       val)

/* if pos <= 0, applies to all */


{
  Hv_Widget          w;
  Hv_RadioListPtr    head;

  if (radiohead == NULL)
    return;

  if (pos > 0) {
    w = Hv_PosGetRadioWidget(pos, radiohead);
    if (w != NULL)
      Hv_SetSensitivity(w, val);
  }
  else {
    for (head = radiohead->head; head != NULL; head = head->next)
      if (head->radio != NULL)
        Hv_SetSensitivity(head->radio, val);
  }
}

/*----- Hv_WidgetInRadioList ------*/

Boolean Hv_WidgetInRadioList(Hv_Widget      w,
			     Hv_RadioHeadPtr   radiohead)

{
  Hv_RadioListPtr    head = radiohead->head;
  Hv_RadioListPtr    temp;

  for (temp = head; temp != NULL; temp = temp->next)
    if (temp->radio == w) 
      return True;

  return False;
}

/*----- Hv_SetActiveRadioButton ------*/

void Hv_SetActiveRadioButton(Hv_Widget      nowon,
			     Hv_RadioHeadPtr   radiohead)

/* set the active radio button based on the Widget provided */

{
  Hv_RadioListPtr    head = radiohead->head;
  Hv_RadioListPtr    temp;
  short              count = 1;


  radiohead->activepos = 0;
  radiohead->activewidget = NULL;

  if (nowon == NULL)
    return;
  
  for (temp = head; temp != NULL; temp = temp->next) {
    if (temp->radio == nowon) {
      XtVaSetValues(temp->radio, XmNset, True, NULL);
      radiohead->activepos = count;
      radiohead->activewidget = nowon;
    }
    else
      XtVaSetValues(temp->radio, XmNset, False, NULL);
    count++;
  }
}


/*----- Hv_PosSetActiveRadioButton ------*/

void Hv_PosSetActiveRadioButton(short       pos,
				Hv_RadioHeadPtr  radiohead)


/* Sets active radio button based on the position
   passed. NOTE: Pos is NOT a C index */

{
  Hv_RadioListPtr    head = radiohead->head;
  Hv_RadioListPtr    temp;
  short              count = 1;

  radiohead->activepos = 0;
  radiohead->activewidget = NULL;

  for (temp = head; temp != NULL; temp = temp->next) {
    if (count == pos) {
      XtVaSetValues(temp->radio, XmNset, True, NULL);
      radiohead->activepos = pos;
      radiohead->activewidget = temp->radio;
    }
    else
      XtVaSetValues(temp->radio, XmNset, False, NULL);
    count++;
  }

}

/*------- Hv_PosGetRadioWidget -------*/

Hv_Widget Hv_PosGetRadioWidget(short      pos,
			       Hv_RadioHeadPtr radiohead)


/* returns the widget corresponding to the 
   given position (NOT a C index) */


{
  Hv_RadioListPtr    head  = radiohead->head;
  Hv_RadioListPtr    temp;
  short              count = 1;

  if (pos <= 0)
    return NULL;

  for (temp = head; temp != NULL; temp = temp->next) {
    if (count == pos)
      return temp->radio;
    count++;
  }

  return NULL;
}


/*-------- Hv_GetActiveRadioButton ------*/

Hv_RadioListPtr Hv_GetActiveRadioButton(Hv_RadioHeadPtr radiohead)

/* returns the RadioListPtr of the active button */

{
  Hv_RadioListPtr  head = radiohead->head;
  Hv_RadioListPtr  temp;

  for (temp = head; temp != NULL; temp = temp->next)
    if (XmToggleButtonGadgetGetState(temp->radio))
      return temp;

  return NULL;
}


/*-------- Hv_GetActiveRadioButtonData -------*/

void  *Hv_GetActiveRadioButtonData(Hv_RadioHeadPtr  radiohead)

{
  Hv_RadioListPtr    radiolist;

  radiolist = Hv_GetActiveRadioButton(radiohead);

  if (radiolist == NULL)
    return NULL;
  else
    return radiolist->data;
}


/*-------- Hv_GetActiveRadioButtonPos ------*/

short Hv_GetActiveRadioButtonPos(Hv_RadioHeadPtr radiohead)

/******* ARCHAIC FORM **********/

{
  return radiohead->activepos;
}

/*-------- Hv_PosGetActiveRadioButton ------*/

short  Hv_PosGetActiveRadioButton(Hv_RadioHeadPtr radiohead)

{
  return radiohead->activepos;
}

/*-------- Hv_GetActiveRadioWidget ------*/

Hv_Widget  Hv_GetActiveRadioWidget(Hv_RadioHeadPtr radiohead)

{
  return radiohead->activewidget;
}



E 1
