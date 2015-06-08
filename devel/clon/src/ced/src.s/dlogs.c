/*
----------------------------------------------------
-							
-  File:    dlogs.c					
-							
-  Summary:						
-            dialog routines
-					       
-                     				
-  Revision history:				
-                     				
-  Date       Programmer     Comments		
------------------------------------------------------
-  10/19/94	DPH		Initial Version	
-                                              	
------------------------------------------------------
*/

#include "ced.h"
#include "bosced.h"

Hv_Widget        bits[16];
Hv_RadioHeadPtr  TrigChoose;
Hv_Widget        maxtries;

static void BitCallback();
static void TriggerDlogSensitivity();

/*--------- EventFilterDialog --------*/
/*
 *   sets what kind of events we can get
 *   via "next event" or accumulate event,
 *   only those with the correct filter
 *   
 */

void
EventFilterDialog()
{
  Hv_Widget rc, dialog, rowcol, dummy;
  int       i, choice;
  char      text[40];
  char     *help[] = {"Use this to filter on the trigger bits. This",
			  "will effect all subsequent \"Next Events\" and",
			  "event accumulation. Selecting no bits is the ",
			  "same as accepting any event.",
			  NULL};

  Hv_VaCreateDialog(&dialog, Hv_TITLE, " Trigger Filter ", NULL);
  rowcol = Hv_DialogColumn(dialog, 0);

  i = 0;
  while (help[i] != NULL)
	dummy = Hv_StandardLabel(rowcol, help[i++], 0);

  dummy = Hv_SimpleDialogSeparator(rowcol);

  dummy = Hv_VaCreateDialogItem(rowcol,
				  Hv_TYPE,         Hv_RADIODIALOGITEM,
				  Hv_RADIOLIST,    &TrigChoose,
				  Hv_FONT,         Hv_helvetica14,
				  Hv_OPTION,       "Accept any event",
				  Hv_CALLBACK,     BitCallback,
				  Hv_OPTION,       "Only with at least one of these bits",
				  Hv_NOWON,        1,   /* sets first as default */
				  NULL);


  dummy = Hv_SimpleDialogSeparator(rowcol);


  rc = Hv_DialogColumn(rowcol, 0);
    
  for(i=0; i<16; i++)
  {
	sprintf(text, " Bit %d", i+1);
	bits[i] = Hv_SimpleToggleButton(rc, text);
	if ((i == 0) || (i == 1) || (i == 13))
	    Hv_SetWidgetForegroundColor(bits[i], Hv_blue);
  }

  dummy = Hv_SimpleDialogSeparator(rowcol);

  Hv_StandardLabel(rowcol, "MaxScan is the max number of events ced will", 0);
  Hv_StandardLabel(rowcol, "scan to find a match (before giving up).", 0);

  rc = Hv_DialogRow(rowcol, 10);
  maxtries = Hv_SimpleTextEdit(rc, " Max Scan ", NULL, 8);

  dummy = Hv_StandardActionButtons(rowcol, 80, Hv_OKBUTTON + Hv_CANCELBUTTON);
    

  if(triggerFilter == 0)
  {
	Hv_PosSetActiveRadioButton(1, TrigChoose);
  }
  else
  {
	Hv_PosSetActiveRadioButton(2, TrigChoose);
	for(i=0; i<16; i++)
      Hv_SetToggleButton(bits[i], Hv_CheckBit(triggerFilter, triggerBits[i]));
  }

  Hv_SetIntText(maxtries, triggerMaxTries);

  TriggerDlogSensitivity();

  if(Hv_DoDialog(dialog, NULL) == Hv_OK)
  {
	triggerMaxTries = Hv_GetIntText(maxtries);

	triggerMaxTries = Hv_iMax(10, Hv_iMin(100000, triggerMaxTries));

	choice = Hv_GetActiveRadioButtonPos(TrigChoose);
	triggerFilter = 0;
	if(choice == 2)
    {
	  for(i=0; i<16; i++)
      {
		if(Hv_GetToggleButton(bits[i]))
        {
          Hv_SetBit(&triggerFilter, triggerBits[i]);
        }
      }
    }
  }
    
}


/*--------- TriggerDlogSensitivity -------*/

static void TriggerDlogSensitivity() {
    int choice, i;


    choice = Hv_GetActiveRadioButtonPos(TrigChoose);

    for (i = 0; i < 16; i++)
	Hv_SetSensitivity(bits[i], (choice == 2));

    Hv_SetSensitivity(maxtries, (choice == 2));
}

/*------- BitCallback ---------*/

static void BitCallback(Hv_Widget w) {
    Hv_SetActiveRadioButton(w, TrigChoose);

    TriggerDlogSensitivity();
}


/*------------ DoAboutDlog ----------- */

void DoAboutDlog(void)

/* create an dialog for use by the about maps.. */

{
  static Hv_Widget        dialog = NULL;

  Widget    rowcol, dummy;
  char     *lab1 = "ced";
  char     *lab2 = "Developed at Christopher Newport University";
  char     *lab3 = "comments & inquiries to:";
  char     *lab4 = "heddle@cebaf.gov";

  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, " About ced ", NULL);
    
/* create a rowcol to hold all the other widgets */

    rowcol = Hv_VaCreateDialogItem(dialog,
				   Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
				   Hv_ORIENTATION,  Hv_VERTICAL,
				   Hv_SPACING,      10,
				   Hv_ALIGNMENT,    Hv_CENTER,
				   NULL);

    dummy = Hv_VaCreateDialogItem(rowcol,
				  Hv_TYPE,     Hv_LABELDIALOGITEM,
				  Hv_LABEL,    lab1,
				  Hv_FONT,     Hv_helvetica14,
				  NULL);

    dummy = Hv_VaCreateDialogItem(rowcol,
				  Hv_TYPE,     Hv_LABELDIALOGITEM,
				  Hv_LABEL,    lab2,
				  Hv_FONT,     Hv_fixed2,
				  NULL);

    dummy = Hv_VaCreateDialogItem(rowcol,
				  Hv_TYPE,     Hv_LABELDIALOGITEM,
				  Hv_LABEL,    lab3,
				  Hv_FONT,     Hv_fixed2,
				  NULL);

    dummy = Hv_VaCreateDialogItem(rowcol,
				  Hv_TYPE,     Hv_LABELDIALOGITEM,
				  Hv_LABEL,    lab4,
				  Hv_FONT,     Hv_fixed2,
				  NULL);

/* close out with just an ok button */

    dummy = Hv_StandardActionButtons(rowcol, 20, Hv_OKBUTTON);
  }

/* the dialog has been created */

  Hv_DoDialog(dialog, NULL);
}

/*--------- StandardInfoPair -----------*/

Hv_Widget StandardInfoPair(Hv_Widget parent,
			   char     *label)

{
  Hv_Widget    rc;
  Hv_Widget    w;
  
  rc = Hv_DialogRow(parent, 6);
  w = Hv_StandardLabel(rc, label, 0);

  w = Hv_VaCreateDialogItem(parent,
			    Hv_TYPE,         Hv_TEXTDIALOGITEM,
			    Hv_EDITABLE,     False,
			    Hv_DEFAULTTEXT,  " ",
			    NULL);

  return w;
}


/*--------- GetNewView -------*/

void GetNewView(void)

{
  Hv_View      view;

  static Hv_Widget        dialog = NULL;
  Hv_Widget               dummy, rowcol;
  static Hv_RadioHeadPtr  viewlist = NULL;

  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, " New View ", NULL);
    rowcol = Hv_DialogColumn(dialog, 6);
       
    dummy = Hv_VaCreateDialogItem(rowcol,
				  Hv_TYPE,         Hv_RADIODIALOGITEM,
				  Hv_RADIOLIST,    &viewlist,
				  Hv_FONT,         Hv_helvetica14,
				  Hv_OPTION,       (char *)"Sector Slice",
				  Hv_OPTION,       (char *)"Calorimeters",
				  Hv_OPTION,       (char *)"Driftchambers",
				  Hv_OPTION,       (char *)"Scints & Start Counter",
				  Hv_OPTION,       (char *)"Tagger",
				  Hv_NOWON,        1,   /* sets first as default */
				  NULL);

    dummy = Hv_StandardActionButtons(rowcol, 20, Hv_OKBUTTON + Hv_CANCELBUTTON);
  }

  Hv_PosSetActiveRadioButton(1, viewlist);

/*  Hv_RadioListSensitivity(viewlist, 3, False); */

  if (Hv_DoDialog(dialog, NULL) == Hv_OK) {
    view = NewView(Hv_GetActiveRadioButtonPos(viewlist));
    Hv_OpenView(view);
  }
}








