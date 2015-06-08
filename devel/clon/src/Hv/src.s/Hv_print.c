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


static Hv_RadioHeadPtr contentlist, orientlist, lplist;
static Hv_Widget       fname = NULL;
static Hv_Widget       note = NULL;

static void Hv_PrintToFileCB(Hv_Widget w);

static void Hv_UseFooterCB(Hv_Widget w);

#define    USE_LP    1
#define    USE_LPR   2

/* ----------- Hv_PrinterSetup -------------*/

void Hv_PrinterSetup(Hv_View View)

{
  static  Hv_Widget    dialog = NULL;
  static  Hv_Widget    pname;
  static  Hv_Widget    fileonly;
  static  Hv_Widget    delay;
  Hv_Widget            dummy;
  Hv_Widget            rowcol;
  static  Hv_Widget    bwprint;


  static  Hv_Widget    tile = NULL;
  static  Hv_Widget    usefooter = NULL;

  static  Hv_Widget    expandtofit = NULL;
  Hv_Widget            rc, rca, rcb, rcc;
  char                 pwd[MAXPATHLEN + 1];

  char                 *defaultprinter;
  static  char         *printername = NULL;
  extern Boolean       Hv_toPrinter;

  int                  delaytime;
  char                 *command, *tpname, *tfname;
  static  int          prcmd = -1;

  if (prcmd < 0) {
    prcmd = USE_LP;  /* most use lp */
#ifdef __linux__
    prcmd = USE_LPR;
#endif
#ifdef ULTRIX
    prcmd = USE_LPR;
#endif
  }
  

  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, " Printer Setup ", NULL);
    rowcol = Hv_DialogColumn(dialog, 6);


    rc = Hv_DialogRow(rowcol, 20);
    rca = Hv_DialogRow(rc, 5);
    rcb = Hv_DialogRow(rc, 5);

    defaultprinter = (char *)getenv("PRINTER");

    if (defaultprinter == NULL) {
      tpname = (char *)Hv_Malloc(20);
      strcpy(tpname, "mypsprinter");
    }
    else {
      tpname = (char *)Hv_Malloc(strlen(defaultprinter) + 1);
      strcpy(tpname, defaultprinter);
    }

    pname = Hv_SimpleTextEdit(rca, "Printer Name", tpname, 4);
    Hv_Free(tpname);

/* add a delay, so that yuo can run to the printer and put in transparencies */

    delay = Hv_SimpleTextEdit(rcb, " Delay (sec)", NULL, 4);

    dummy = Hv_SimpleDialogSeparator(rowcol);
    expandtofit = Hv_SimpleToggleButton(rowcol, "Expand to fill");

    dummy = Hv_SimpleDialogSeparator(rowcol);

/* footer comment */

    rc        = Hv_DialogColumn(rowcol, 0);
    tile      = Hv_StandardToggleButton(rc, "Tile open views", Hv_UseFooterCB);
    usefooter = Hv_StandardToggleButton(rc, "Use footer", Hv_UseFooterCB);
    note      = Hv_SimpleTextEdit(rc, NULL, NULL, 40);
    dummy     = Hv_SimpleDialogSeparator(rowcol);

    rc = Hv_DialogColumn(rowcol, 10);
    fileonly = Hv_StandardToggleButton(rc, "Print (in eps format) to file:",
				       Hv_PrintToFileCB);
    
/* fname holds the file name */

    getcwd((char *)pwd, MAXPATHLEN + 1);

    fname = Hv_VaCreateDialogItem(rc,
				  Hv_TYPE,        Hv_TEXTDIALOGITEM,
				  Hv_DEFAULTTEXT, pwd,
				  Hv_NUMCOLUMNS,  45,
				  NULL);
    
    dummy = Hv_SimpleDialogSeparator(rowcol);

    rc = Hv_DialogRow(rowcol, 10);
    bwprint = Hv_SimpleToggleButton(rc, "print all colors as black");
    dummy = Hv_SimpleDialogSeparator(rowcol);

    rc = Hv_DialogRow(rowcol, 8);
    rca = Hv_DialogColumn(rc, 5);
    rcb = Hv_DialogColumn(rc, 5);
    rcc = Hv_DialogColumn(rc, 5);


    dummy = Hv_VaCreateDialogItem(rca,
				  Hv_TYPE,         Hv_RADIODIALOGITEM,
				  Hv_RADIOLIST,    &orientlist,
				  Hv_FONT,         Hv_fixed2,
				  Hv_OPTION,       (char *)"landscape",
				  Hv_OPTION,       (char *)"portrait",
				  Hv_NOWON,        1,
				  NULL);


    dummy = Hv_VaCreateDialogItem(rcb,
				  Hv_TYPE,         Hv_RADIODIALOGITEM,
				  Hv_RADIOLIST,    &contentlist,
				  Hv_FONT,         Hv_fixed2,
				  Hv_OPTION,       (char *)"contents only",
				  Hv_OPTION,       (char *)"entire view",
				  Hv_NOWON,        1,
				  NULL);

    dummy = Hv_VaCreateDialogItem(rcc,
				  Hv_TYPE,         Hv_RADIODIALOGITEM,
				  Hv_RADIOLIST,    &lplist,
				  Hv_FONT,         Hv_fixed2,
				  Hv_OPTION,       (char *)"print via \"lp\"",
				  Hv_OPTION,       (char *)"print via \"lpr\"",
				  Hv_NOWON,        prcmd,
				  NULL);


    dummy = Hv_StandardActionButtons(rowcol, 180, Hv_OKBUTTON + Hv_CANCELBUTTON);
  }

/* dialog has ben created */

  
  if (Hv_psDefaultFile != NULL) {

    getcwd((char *)pwd, MAXPATHLEN + 1);

    tfname = (char *)Hv_Malloc(strlen(pwd) + strlen(Hv_psDefaultFile) + 10);
    strcpy(tfname, pwd);
    strcat(tfname, "/");
    strcat(tfname, Hv_psDefaultFile);
    Hv_SetString(fname, tfname);
    Hv_Free(tfname);
  }

  Hv_PosSetActiveRadioButton((short)prcmd, lplist);

  Hv_PosSetActiveRadioButton((short)Hv_psOrientation, orientlist);
  if (Hv_psEntireView)
    Hv_PosSetActiveRadioButton(2, contentlist);
  else
    Hv_PosSetActiveRadioButton(1, contentlist);

/* cannot tile unless whole screen printing */

  if (!Hv_psWholeScreen)
      Hv_psTile = False;

  Hv_SetToggleButton(tile, Hv_psTile);

  Hv_SetToggleButton(expandtofit, Hv_psExpandToFit);
  Hv_SetToggleButton(usefooter, Hv_psUseFooter);
  Hv_SetToggleButton(fileonly,  !Hv_toPrinter);

/* If not to a file, then cannot select filename.
   If not using footer, then can't edit footer string */

  Hv_SetSensitivity(fname, !Hv_toPrinter);
  Hv_SetSensitivity(note,  Hv_psUseFooter);
  Hv_SetSensitivity(tile,  Hv_psWholeScreen);

/*  Hv_SetSensitivity(tile,  False);*/  /* not ready for prime time*/

  Hv_SetToggleButton(bwprint,  Hv_printAllColorsBlack);
  Hv_SetIntText(delay, 0);   /* always default no delay */

/* do NOT delete any previous "note" */

  if (Hv_psNote == NULL) {
      Hv_InitCharStr(&Hv_psNote, " ");
  }
  Hv_SetString(note, Hv_psNote);      /*set to empty string*/

/* see if printer name was set prior */

  if (printername)
    Hv_SetString(pname, printername);


  if (Hv_DoDialog(dialog, NULL) != Hv_OK)
    return;

  delaytime = Hv_GetIntText(delay);

  Hv_psUseFooter   = Hv_GetToggleButton(usefooter);
  Hv_psExpandToFit = Hv_GetToggleButton(expandtofit);
  Hv_psTile        = Hv_GetToggleButton(tile);

  Hv_psNote = Hv_GetString(note);   /* footer comment */
  Hv_toPrinter = !Hv_GetToggleButton(fileonly);
  Hv_printAllColorsBlack = Hv_GetToggleButton(bwprint);
  
  prcmd = Hv_GetActiveRadioButtonPos(lplist);
  Hv_psOrientation = Hv_GetActiveRadioButtonPos(orientlist);
  Hv_psEntireView = (Hv_GetActiveRadioButtonPos(contentlist) == 2);

/* save the printer name so it can be used as default next time */

  if (printername)
    Hv_Free(printername);

  printername = Hv_GetString(pname);

  if (Hv_toPrinter) {
    Hv_psFileName = (char *)Hv_Malloc(L_tmpnam);
    tmpnam(Hv_psFileName);
  }
  else {
    Hv_psFileName = Hv_GetString(fname);
  }

/* print whole screen or just one view? */

  if (Hv_psWholeScreen)
    Hv_PrintScreen();
  else
    Hv_PrintView(View, True);
  
  if (Hv_toPrinter) {
    
/* a delay so that we can run to the printer? */

    if (delaytime > 0) {
      delaytime = Hv_iMin(delaytime, 600);
      Hv_Println("paused for %d seconds before issuing print command...", delaytime);

/* Hv_Wait returns 0 if we aborted */

      if (Hv_Wait(delaytime) == 0) {
	Hv_Free(Hv_psFileName);
	Hv_Println(" CANCELLED PRINT.");
	return;
      }
    }
    
    command = (char *)Hv_Malloc(15 + strlen(printername) + strlen(Hv_psFileName));

    if (prcmd == USE_LP) {
      strcpy(command, "lp -d");
      strcat(command, printername);
      strcat(command, " ");
      strcat(command, Hv_psFileName);
    }
    else if (prcmd == USE_LPR) {
      strcpy(command, "lpr -P");
      strcat(command, printername);
      strcat(command, " ");
      strcat(command, Hv_psFileName);
    }

    system(command);
    Hv_Free(command);
    
    command = (char *)Hv_Malloc(12 + strlen(Hv_psFileName));
    strcpy(command, "chmod 777 ");
    strcat(command, Hv_psFileName);
    system(command);
    Hv_Free(command);
  }

  Hv_Free(Hv_psFileName);
}

/*--------- Hv_PrintToFileCB ----------*/

static void Hv_PrintToFileCB(Hv_Widget w)

{
  if (w == NULL)
    return;

  Hv_SetSensitivity(fname, Hv_GetToggleButton(w));
}


/*--------- Hv_UseFooterCB ----------*/

static void Hv_UseFooterCB(Hv_Widget w)

{
  if (w == NULL)
    return;

  Hv_SetSensitivity(note, Hv_GetToggleButton(w));
}


#undef    USE_LP 
#undef    USE_LPR
