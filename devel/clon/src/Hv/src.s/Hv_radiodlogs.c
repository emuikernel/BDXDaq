/**
 * <EM>Deals deals with radios buttons on <B>dialogs</B>.</EM>
 * <P>
 * The Hv library was developed at The Thomas Jefferson National
 * Accelerator Facility under contract to the
 * Department of Energy and is the property of they U.S. Government.
 * Partial support came from the National Science Foundation.
 * <P>
 * It may be used to develop commercial software, but the Hv
 * source code and/or compiled modules/libraries may not be sold.
 * <P>
 * Questions or comments should be directed
 * to <a href="mailto:heddle@cebaf.gov">heddle@cebaf.gov</a> <HR>
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

/**
 * Hv_RadioListSensitivity 
 * @purpose   Set selectability for members of a radio dialog.
 * @param   radiohead    The radio head pointer.
 * @param   pos          Affected button (1..N). If <= 0, the this will apply
 *                       to all buttons.
 * @param   val          Either True (selecatble) or False (unselectable).
 */

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

/*---------- Hv_StandardRadionButtonCallback -------------*/

void Hv_StandardRadioButtonCallback(Hv_Widget w,
					   Hv_Pointer client_data)

{
  Hv_RadioHeadPtr  radiohead = (Hv_RadioHeadPtr)(client_data);
  Hv_SetActiveRadioButton(w, radiohead);
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
      Hv_VaSetValues(temp->radio, Hv_Nset, True, NULL);
      radiohead->activepos = count;
      radiohead->activewidget = nowon;
    }
    else
      Hv_VaSetValues(temp->radio, Hv_Nset, False, NULL);
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
      Hv_VaSetValues(temp->radio, Hv_Nset, True, NULL);
      radiohead->activepos = pos;
      radiohead->activewidget = temp->radio;
    }
    else
      Hv_VaSetValues(temp->radio, Hv_Nset, False, NULL);
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
      if (Hv_GetToggleButton(temp->radio))
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



