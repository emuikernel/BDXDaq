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
-  08/24/95	DPH		Initial Version	
-                                              	
------------------------------------------------------
*/

#include "Hv.h"
#include "Hv_maps.h"
#include "maps.h"

/*------ local prototypes -----*/

static void         EditColors();

/* some static variables for editing colors */


static Hv_Widget spacelabel, wflabel, wblabel, lflabel;
static Hv_Widget lblabel, glabel, gtlabel;

static short     outerspace, waterfill, waterborder;
static short     landfill, landborder, gridcolor, gridtextcolor;

/*------------ DoAboutDlog ----------- */

void DoAboutDlog()

/* create an dialog for use by the about maps.. */

{
  static Hv_Widget        dialog = NULL;

  Hv_Widget rowcol, dummy;

  char      *lab1;
  char      *lab2 = "Developed by the Company Formerly Known As Prince";
  char      *lab3 = "comments & inquiries to:";
  char      *lab4 = "xyz@xyz.com";

  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, " About ", NULL);
    
/* create a rowcol to hold all the other widgets */

    rowcol = Hv_VaCreateDialogItem(dialog,
				   Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
				   Hv_ORIENTATION,  Hv_VERTICAL,
				   Hv_SPACING,      10,
				   Hv_ALIGNMENT,    Hv_CENTER,
				   NULL);

    Hv_InitCharStr(&lab1, Hv_programName);

    dummy = Hv_VaCreateDialogItem(rowcol,
				  Hv_TYPE,     Hv_LABELDIALOGITEM,
				  Hv_LABEL,    lab1,
				  Hv_FONT,     Hv_helvetica14,
				  NULL);
    Hv_Free(lab1);

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

    dummy = Hv_StandardActionButtons(rowcol, 0, Hv_OKBUTTON);
  }

/* the dialog has been created */

  Hv_DoDialog(dialog, NULL);
}

/*--------- GetNewView -------*/

void GetNewView()

{
  Hv_View      view;

  view = NewView(MAPVIEW);
  Hv_OpenView(view);
}



/* -------- ViewCustomize --------*/

void ViewCustomize(View)

Hv_View     View;

{
  static Hv_Widget       dialog = NULL;
  Hv_Widget              dummy, rowcol, rc, rca, rcb, rc1, rc2;
  static Hv_Widget       dlat, dlong;
  Hv_ViewMapData         mdata;
  int                    answer;

  mdata = Hv_GetViewMapData(View);
  
  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, " Customize ", NULL);

    rowcol = Hv_DialogColumn(dialog, 3);

    rc  = Hv_DialogRow(rowcol, 8);
    rca = Hv_DialogColumn(rc, 3);
    rcb = Hv_DialogColumn(rc, 3);
    rc1 = Hv_DialogTable(rca, 4, 2);
    rc2 = Hv_DialogTable(rcb, 3, 2);

    glabel     = Hv_SimpleColorLabel(rc1, "        Grid ", EditColors);
    gtlabel    = Hv_SimpleColorLabel(rc1, " Grid Labels ", EditColors);
    wflabel    = Hv_SimpleColorLabel(rc1, "  Water fill ", EditColors);
    wblabel    = Hv_SimpleColorLabel(rc1, "Water border ", EditColors);
    lflabel    = Hv_SimpleColorLabel(rc2, "   Land fill ", EditColors);
    lblabel    = Hv_SimpleColorLabel(rc2, " Land border ", EditColors);
    spacelabel = Hv_SimpleColorLabel(rc2, " Outer Space ", EditColors);

    dummy = Hv_SimpleDialogSeparator(rowcol);
    rc = Hv_DialogTable(rowcol, 2, 4);
    dlat   = Hv_SimpleTextEdit(rc, "Latitide  grid size", NULL, 4);
    dlong  = Hv_SimpleTextEdit(rc, "Longitude grid size", NULL, 4);

    rc = Hv_StandardActionButtons(rowcol, 80, Hv_OKBUTTON + Hv_APPLYBUTTON);
 }


  outerspace    = mdata->outerspace;
  waterfill     = mdata->waterfill;
  waterborder   = mdata->waterborder;
  landfill      = mdata->landfill;
  landborder    = mdata->landborder;
  gridcolor     = mdata->gridcolor;
  gridtextcolor = mdata->gridlabelcolor;

/* set the colors to their present values */

  Hv_ChangeLabelColors(wflabel, -1, waterfill);
  Hv_ChangeLabelColors(wblabel, -1, waterborder);
  Hv_ChangeLabelColors(lflabel, -1, landfill);
  Hv_ChangeLabelColors(lblabel, -1, landborder);
  Hv_ChangeLabelColors(glabel,  -1, gridcolor);
  Hv_ChangeLabelColors(gtlabel, -1, gridtextcolor);
  Hv_ChangeLabelColors(spacelabel,  -1, outerspace);

/* set the text edit fields to their present values */

  Hv_SetFloatText(dlat,  mdata->latstep,  2);
  Hv_SetFloatText(dlong, mdata->longstep, 2);

/* ready to go! */

  while ((answer = Hv_DoDialog(dialog, NULL)) > 0) {

    mdata->waterfill      = waterfill;
    mdata->waterborder    = waterborder;
    mdata->landfill       = landfill;
    mdata->landborder     = landborder;
    mdata->gridcolor      = gridcolor;
    mdata->gridlabelcolor = gridtextcolor;
    mdata->outerspace     = outerspace;

    if (mdata->projection == Hv_MERCATOR)
      View->hotrectborderitem->scheme->fillcolor = mdata->waterfill;
    else
      View->hotrectborderitem->scheme->fillcolor = mdata->outerspace;

    mdata->latstep  = Hv_fMax(0.1, Hv_GetFloatText(dlat));
    mdata->longstep = Hv_fMax(0.1, Hv_GetFloatText(dlong));

    Hv_SetViewDirty(View);
    Hv_DrawViewHotRect(View);

    if (answer != Hv_APPLY)
      break;
  }
}


/* ------ EditColors --- */

static void EditColors(w)

Hv_Widget    w;          /* widget doing callback */

{
  if (w == wflabel)
    Hv_ModifyColor(w, &waterfill, "WaterFill Color", False);
  else if (w == glabel)
    Hv_ModifyColor(w, &gridcolor, "Grid Color", False);
  else if (w == gtlabel)
    Hv_ModifyColor(w, &gridtextcolor, "Grid Text Color", False);
  else if (w == wblabel)
    Hv_ModifyColor(w, &waterborder, "Water Border Color", False);
  else if (w == lflabel)
    Hv_ModifyColor(w, &landfill, "Land Fill Color", False);
  else if (w == lblabel)
    Hv_ModifyColor(w, &landborder, "Land Border Color", False);
  else if (w == spacelabel)
    Hv_ModifyColor(w, &outerspace, "Space Background Color", False);
}


