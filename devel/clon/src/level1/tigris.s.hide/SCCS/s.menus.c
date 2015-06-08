h53926
s 00011/00000/00280
d D 1.7 06/09/01 00:34:51 boiarino 8 7
c dummy VXWORKS
c 
e
s 00085/00030/00195
d D 1.6 02/08/28 08:12:48 boiarino 7 6
c separate sync and async functions
e
s 00090/00103/00135
d D 1.5 02/08/25 19:46:08 boiarino 6 5
c big cleanup (globals, locals, prototypes)
c async inputs control menu function
e
s 00002/00002/00236
d D 1.4 02/08/22 22:07:50 boiarino 5 4
c seems ok now ...
e
s 00012/00066/00226
d D 1.3 02/08/22 21:35:49 boiarino 4 3
c still working with AsyncControl ...
e
s 00078/00001/00214
d D 1.2 02/08/22 21:07:41 boiarino 3 1
c comment out CREGControl,
c implement AsyncControl
c 
e
s 00000/00000/00000
d R 1.2 02/03/29 12:13:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/tigris/menus.c
e
s 00215/00000/00000
d D 1.1 02/03/29 12:13:01 boiarino 1 0
c date and time created 02/03/29 12:13:01 by boiarino
e
u
U
f e 0
t
T
I 1

I 8
#ifndef VXWORKS

E 8
D 4
#include "Hv.h"
#include "ced.h"
E 4
I 4
/* menus.c */

E 4
#include <string.h>
#include <strings.h>
I 4
#include "Hv.h"
#include "ced.h"
E 4
D 6
#include "sgl.h"
#include "def.h"
E 6
I 6
#include "tigris.h"
E 6

D 6
Hv_RadioHeadPtr   triglist;
char              *dlhn;
E 6
I 6
extern char InFileName[512]; /* defined in fileio.c */
extern char dlhn[1024]; /* defined in fileio.c */
extern struct phase3_def phase3; /* defined in fileio.c */
E 6

D 6
extern Hv_View view[7];
extern char *  JJ_SEtextItem[6][12];
I 3
extern struct phase3_def phase3;
E 3
extern struct phase3_def JAMphase3;
E 6
I 6
/* local variables */
E 6

D 6
extern void   movPhase12sgl ();
extern void   movSE2sgl ();
extern void   downloadSE ();
extern void   TrigParser ();
extern void   downloadTrig () ;
E 6
I 6
static Hv_RadioHeadPtr triglist;
E 6

D 6
extern char                InFileName[] ;

D 3
/* ----- adjust the CREG values --------------- */
E 3
I 3
D 4
/* ----- adjust the CREG values ---------------
E 4
I 4
/* ----- adjust the CREG values --------------- 
E 4
E 3
void CREGControl (w)
Hv_Widget w;
E 6
I 6
/*Sergey: async inputs control menu function - probably do not need it ... */
void
AsyncRadioClick(Hv_Event hevent)
E 6
{
D 6
  static Hv_Widget dialog = NULL;
  Hv_Widget        dummy, rowcol, rc;
  Hv_Widget        et[8];
  int j;

  if (!dialog) {
    Hv_VaCreateDialog (&dialog, Hv_TITLE,  "CREG config", NULL);
    rowcol = Hv_DialogColumn (dialog, 0);

    rc = Hv_DialogColumn(rowcol, 0);

    et[0]  = Hv_SimpleToggleButton(rc,   " T1 Async enable");
    et[1]  = Hv_SimpleToggleButton(rc,   " T2 Async enable");
    dummy = Hv_SimpleDialogSeparator(rc);

    et[2]  = Hv_SimpleToggleButton(rc,   " T3 Async enable");
    et[3]  = Hv_SimpleToggleButton(rc,   " T4 Async enable");
    dummy = Hv_SimpleDialogSeparator(rc);

    et[4]  = Hv_SimpleToggleButton(rc,   " T5 Async enable");
    et[5]  = Hv_SimpleToggleButton(rc,   " T6 Async enable");
    dummy = Hv_SimpleDialogSeparator(rc);

    et[6]  = Hv_SimpleToggleButton(rc,   " T7 Async enable");
    et[7]  = Hv_SimpleToggleButton(rc,   " T8 Async enable");

    dummy = Hv_StandardActionButtons(rowcol, 20, Hv_OKBUTTON + Hv_CANCELBUTTON);
  }

  if ((Hv_DoDialog (dialog, NULL)) == Hv_OK) {

    for (j=0; j<8; j++) {
      JAMphase3.Enable_Async[j] = Hv_GetToggleButton(et[j]);
      printf ("%d ", JAMphase3.Enable_Async[j]);
    }
    printf ("\n");
  }
  

  printf ("yup, um, yup!\n");
E 6
I 6
  printf("AsyncRadioClick reached\n");
E 6
I 3
D 4
} */
E 4
I 4
}
D 6
*/
E 6
E 4

I 7
/*Sergey: sync inputs control menu function */
void
SyncControl(Hv_Widget w)
{
  static Hv_Widget dialog = NULL;
  static Hv_Widget dummy, rowcol, rc;
  static Hv_Widget async_alone[8];
  int i, j, k, se_yes;

  if(!dialog)
  {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, "Sync Control", NULL);
    rowcol = Hv_DialogColumn(dialog, 0);

    rc = Hv_DialogColumn(rowcol, 0);

    dummy = Hv_StandardLabel(rc, "  Sync input 1", NULL);
    dummy = Hv_SimpleDialogSeparator(rc);
    async_alone[0]  = Hv_SimpleToggleButton(rc," Coincidence with TS bit 1");
    dummy = Hv_SimpleDialogSeparator(rc);
    async_alone[1]  = Hv_SimpleToggleButton(rc," Coincidence with TS bit 2");
    dummy = Hv_SimpleDialogSeparator(rc);
    dummy = Hv_SimpleDialogSeparator(rc);

    dummy = Hv_StandardLabel(rc, "  Sync input 2", NULL);
    dummy = Hv_SimpleDialogSeparator(rc);
    async_alone[2]  = Hv_SimpleToggleButton(rc," Coincidence with TS bit 3");
    dummy = Hv_SimpleDialogSeparator(rc);
    async_alone[3]  = Hv_SimpleToggleButton(rc," Coincidence with TS bit 4");
    dummy = Hv_SimpleDialogSeparator(rc);
    dummy = Hv_SimpleDialogSeparator(rc);

    dummy = Hv_StandardLabel(rc, "  Sync input 3", NULL);
    dummy = Hv_SimpleDialogSeparator(rc);
    async_alone[4]  = Hv_SimpleToggleButton(rc," Coincidence with TS bit 5");
    dummy = Hv_SimpleDialogSeparator(rc);
    async_alone[5]  = Hv_SimpleToggleButton(rc," Coincidence with TS bit 6");
    dummy = Hv_SimpleDialogSeparator(rc);
    dummy = Hv_SimpleDialogSeparator(rc);

    dummy = Hv_StandardLabel(rc, "  Sync input 4", NULL);
    dummy = Hv_SimpleDialogSeparator(rc);
    async_alone[6]  = Hv_SimpleToggleButton(rc," Coincidence with TS bit 7");
    dummy = Hv_SimpleDialogSeparator(rc);
    async_alone[7]  = Hv_SimpleToggleButton(rc," Coincidence with TS bit 8");

    dummy = Hv_StandardActionButtons(rowcol,60,Hv_OKBUTTON+Hv_CANCELBUTTON);
  }
  
  for(j=0; j<8; j++)
  {
    /* second index is product number, from 0 to 'MaxTrigProduct' */
    /* in disable state must be =2 !!! */
    if(phase3.Trig[j+1][0][18] == 2)
    {
      Hv_SetToggleButton(async_alone[j],0);
    }
    else
    {
      Hv_SetToggleButton(async_alone[j],1);
    }
  }


  if((Hv_DoDialog(dialog, NULL)) == Hv_OK)
  {
    printf("SyncControl: \n");

    for(j=0; j<8; j++)
    {
      k = Hv_GetToggleButton(async_alone[j]);
      if(k==0) k = 2; /* in disable state must be =2 !!! */
      for(i=0; i<MaxTrigProducts; i++)
      {
        phase3.Trig[j+1][i][18] = k; /* set all of them */
      }

      printf("%d ",phase3.Trig[j+1][0][18]);
    }
    printf("\n");
  }
}


E 7
D 4

E 4
D 6
/* async inputs control menu function */

E 6
I 6
/*Sergey: async inputs control menu function */
E 6
void
AsyncControl(Hv_Widget w)
{
  static Hv_Widget dialog = NULL;
  static Hv_Widget dummy, rowcol, rc;
D 6
  static Hv_Widget async_enable[4];
  static Hv_Widget async_alone[4];
  int i, j;
E 6
I 6
  static Hv_Widget async_enable[8];
D 7
  static Hv_Widget async_alone[8];
E 7
  int i, j, k, se_yes;
E 6

  if(!dialog)
  {
D 6
    Hv_VaCreateDialog(&dialog, Hv_TITLE,  "Async Control", NULL);
E 6
I 6
    Hv_VaCreateDialog(&dialog, Hv_TITLE, "Async Control", NULL);
E 6
    rowcol = Hv_DialogColumn(dialog, 0);

    rc = Hv_DialogColumn(rowcol, 0);

D 6
    async_enable[0] = Hv_SimpleToggleButton(rc,   " Async1 enable");
    async_alone[0]  = Hv_SimpleToggleButton(rc,   " Async1 alone");
E 6
I 6
    dummy = Hv_StandardLabel(rc, "  Async input 1", NULL);
E 6
    dummy = Hv_SimpleDialogSeparator(rc);
I 6
    async_enable[0] = Hv_SimpleToggleButton(rc," Enable with TS bit 1");
D 7
    async_alone[0]  = Hv_SimpleToggleButton(rc," Coincidence with TS bit 1");
E 7
    dummy = Hv_SimpleDialogSeparator(rc);
    async_enable[1] = Hv_SimpleToggleButton(rc," Enable with TS bit 2");
D 7
    async_alone[1]  = Hv_SimpleToggleButton(rc," Coincidence with TS bit 2");
E 7
    dummy = Hv_SimpleDialogSeparator(rc);
    dummy = Hv_SimpleDialogSeparator(rc);
E 6

D 6
    async_enable[1] = Hv_SimpleToggleButton(rc,   " Async2 enable");
    async_alone[1]  = Hv_SimpleToggleButton(rc,   " Async2 alone");
E 6
I 6
    dummy = Hv_StandardLabel(rc, "  Async input 2", NULL);
E 6
    dummy = Hv_SimpleDialogSeparator(rc);
I 6
    async_enable[2] = Hv_SimpleToggleButton(rc," Enable with TS bit 3");
D 7
    async_alone[2]  = Hv_SimpleToggleButton(rc," Coincidence with TS bit 3");
E 7
    dummy = Hv_SimpleDialogSeparator(rc);
    async_enable[3] = Hv_SimpleToggleButton(rc," Enable with TS bit 4");
D 7
    async_alone[3]  = Hv_SimpleToggleButton(rc," Coincidence with TS bit 4");
E 7
    dummy = Hv_SimpleDialogSeparator(rc);
    dummy = Hv_SimpleDialogSeparator(rc);
E 6

D 6
    async_enable[2] = Hv_SimpleToggleButton(rc,   " Async3 enable");
    async_alone[2]  = Hv_SimpleToggleButton(rc,   " Async3 alone");
E 6
I 6
    dummy = Hv_StandardLabel(rc, "  Async input 3", NULL);
E 6
    dummy = Hv_SimpleDialogSeparator(rc);
I 6
    async_enable[4] = Hv_SimpleToggleButton(rc," Enable with TS bit 5");
D 7
    async_alone[4]  = Hv_SimpleToggleButton(rc," Coincidence with TS bit 5");
E 7
    dummy = Hv_SimpleDialogSeparator(rc);
    async_enable[5] = Hv_SimpleToggleButton(rc," Enable with TS bit 6");
D 7
    async_alone[5]  = Hv_SimpleToggleButton(rc," Coincidence with TS bit 6");
E 7
    dummy = Hv_SimpleDialogSeparator(rc);
    dummy = Hv_SimpleDialogSeparator(rc);
E 6

D 6
    async_enable[3] = Hv_SimpleToggleButton(rc,   " Async4 enable");
    async_alone[3]  = Hv_SimpleToggleButton(rc,   " Async4 alone");
E 6
I 6
    dummy = Hv_StandardLabel(rc, "  Async input 4", NULL);
    dummy = Hv_SimpleDialogSeparator(rc);
    async_enable[6] = Hv_SimpleToggleButton(rc," Enable with TS bit 7");
D 7
    async_alone[6]  = Hv_SimpleToggleButton(rc," Coincidence with TS bit 7");
E 7
    dummy = Hv_SimpleDialogSeparator(rc);
    async_enable[7] = Hv_SimpleToggleButton(rc," Enable with TS bit 8");
D 7
    async_alone[7]  = Hv_SimpleToggleButton(rc," Coincidence with TS bit 8");
E 7
E 6

D 6
    dummy = Hv_StandardActionButtons(rowcol, 20, Hv_OKBUTTON+Hv_CANCELBUTTON);
E 6
I 6
    dummy = Hv_StandardActionButtons(rowcol,60,Hv_OKBUTTON+Hv_CANCELBUTTON);
E 6
  }
D 4

E 4
I 4
D 5
  /*
E 5
I 5
  
E 5
E 4
D 6
  for(j=0; j<4; j++)
E 6
I 6
  for(j=0; j<8; j++)
E 6
  {
D 6
    if(phase3.Enable_Async[1+j*2] == 1)
E 6
I 6
    if(phase3.Enable_Async[j+1] == 1)
E 6
    {
      Hv_SetToggleButton(async_enable[j],1);
    }
    else
    {
      Hv_SetToggleButton(async_enable[j],0);
    }
D 7

D 6
    Hv_SetToggleButton(async_alone[j],0);
E 6
I 6
    /* second index is priduct number, from 0 to 'MaxTrigProduct' */
    /* in disable state must be =2 !!! */
    if(phase3.Trig[j+1][0][18] == 2)
    {
      Hv_SetToggleButton(async_alone[j],0);
    }
    else
    {
      Hv_SetToggleButton(async_alone[j],1);
    }
E 7
E 6
  }
I 5
D 6
  
E 6
E 5

I 6

E 6
  if((Hv_DoDialog(dialog, NULL)) == Hv_OK)
  {
    printf("AsyncControl: \n");

D 6
    i = 1;
    for(j=0; j<4; j++)
E 6
I 6
    for(j=0; j<8; j++)
E 6
    {
D 6
      phase3.Enable_Async[i++] = Hv_GetToggleButton(async_enable[j]);
      phase3.Enable_Async[i++] = Hv_GetToggleButton(async_enable[j]);
E 6
I 6
      phase3.Enable_Async[j+1] = Hv_GetToggleButton(async_enable[j]);
D 7

      k = Hv_GetToggleButton(async_alone[j]);
      if(k==0) k = 2; /* in disable state must be =2 !!! */
      for(i=0; i<MaxTrigProducts; i++)
      {
        phase3.Trig[j+1][i][18] = k; /* set all of them */
      }

      printf("(%d %d) ",phase3.Enable_Async[j+1],phase3.Trig[j+1][0][18]);
E 7
I 7
      printf("%d ",phase3.Enable_Async[j+1]);
E 7
E 6
D 4

      /*BOYphase3.Async_Alone[j] = Hv_GetToggleButton(async_alone[j]);*/

      /*printf("(%1d,%1d %1d) ",
        phase3.Enable_Async[i-2],phase3.Enable_Async[i-1],
        BOYphase3.Async_Alone[j]);*/
E 4
    }
    printf("\n");
  }
I 6
D 7
printf("1=============================== %d\n",phase3.Trig[1][0][18]);
E 7
E 6
D 4

E 4
I 4
D 5
  */
E 5
E 4
E 3
}
I 3




E 3
/* ----- the scintillator event download button */
D 4
void DownloadControl (w)
E 4

D 4
Hv_Widget     w;
E 4
I 4
void
DownloadControl(Hv_Widget w)
E 4
{
D 4
    Hv_View               temp;
E 4
I 4
  Hv_View               temp;
E 4
  FILE                   *fp;
  static Hv_Widget        dialog = NULL ;
  static Hv_Widget        downloadTarget = NULL;
  Hv_Widget               dummy, rowcol, rc;
  int                     _z;
/*  char                   *outname = "test.config"; */
   char                   *outname = "/usr/local/clas/parms/ts/Current.Config"; 


  fp = fopen(outname, "w");

  fprintf(stderr, "Initiating the download.\n");
  fprintf(stderr, "Trigger file: %s\n", InFileName);
  
  if (fp == NULL)
    fprintf(stderr, "Could NOT write the trigger file name to %s\n", outname);
  else {
    fprintf(fp, "%s\n", InFileName);
    fprintf(fp, "(only ~trigger/tigris and ~trigger/tigrisLite update this info)\n");
    fprintf(stderr, "The trigger file name was written to: %s\n", outname);
    fclose(fp);
  }

  
  if (!dialog) {

    Hv_VaCreateDialog (&dialog, Hv_TITLE,  "Confg Download", NULL);
    rowcol = Hv_DialogColumn (dialog, 6);

    dummy = Hv_VaCreateDialogItem(rowcol,
				  Hv_TYPE,         Hv_RADIODIALOGITEM,
				  Hv_RADIOLIST,    &triglist,
				  Hv_FONT,         Hv_helvetica14,
				  Hv_OPTION,       (char *)"CLAS Trig 1",
				  Hv_OPTION,       (char *)"CLAS Trig 2",
				  Hv_OPTION,       (char *)"Other",
				  Hv_NOWON,        2,   /* sets default On */
				  NULL);


    rc = Hv_DialogRow(rowcol, 6);
    downloadTarget = Hv_SimpleTextEdit(rc, "     Host    ", NULL, 2);
    dummy = Hv_StandardActionButtons(rowcol, 20, Hv_OKBUTTON + Hv_CANCELBUTTON);
  }


D 6
  if ((Hv_DoDialog (dialog, NULL)) == Hv_OK) {
    _z = Hv_PosGetActiveRadioButton (triglist);
    if ((dlhn = malloc (1024)) == 0) {
      printf ("DownLoad_Phase1: no memory for hostname string\n");
      exit (0);
    }
E 6
I 6
  if((Hv_DoDialog (dialog, NULL)) == Hv_OK)
  {
    _z = Hv_PosGetActiveRadioButton(triglist);
E 6
    switch (_z) {
    case 1:
D 6
      strcpy (dlhn, "clastrig1");
      movPhase12sgl (0);
      movPhase12sgl (1);
      movSE2sgl ();
      downloadSE ();
E 6
I 6
      strcpy(dlhn,"clastrig2");
      movPhase12sgl(0);
      movPhase12sgl(1);
      movSE2sgl();
      downloadSE();
E 6
      TrigParser ();
      downloadTrig ();
      break;
    case 2:
D 6
      strcpy (dlhn, "clastrig2.cebaf.gov");
      movPhase12sgl (0);
      movPhase12sgl (1);
      movSE2sgl ();
      downloadSE ();
E 6
I 6
      strcpy(dlhn,"clastrig2.jlab.org");
      movPhase12sgl(0);
      movPhase12sgl(1);
      movSE2sgl();
      downloadSE();
E 6
      TrigParser ();
      downloadTrig ();
      break;
    case 3:
D 6
      strcpy(dlhn, Hv_GetString(downloadTarget));
      movPhase12sgl (0);
      movPhase12sgl (1);
      movSE2sgl ();
      downloadSE ();
E 6
I 6
      strcpy(dlhn,Hv_GetString(downloadTarget));
      movPhase12sgl(0);
      movPhase12sgl(1);
      movSE2sgl();
      downloadSE();
E 6
      TrigParser ();
      downloadTrig ();
      break;
    }

    for (temp = Hv_views.first;  temp != NULL;  temp = temp->next) {
	if (temp->tag != Hv_VIRTUALVIEW)
	    Hv_SetViewTitle(temp, newViewTitle(Hv_GetViewTitle(temp), 1)); 
    }
    
  }
  
  /* must have pressed Hv_CANCEL */
}
I 8

#else /* UNIX only */

void
menus_dummy()
{
}

#endif
E 8
D 4

/* -==========================================================================- */
/*

    if (Hv_GetToggleButton (tb[1])) {
      movPhase12sgl (1);
    }
    if (Hv_GetToggleButton (tb[2])) {
      movSE2sgl ();
      downloadSE ();
    }
    if (Hv_GetToggleButton (tb[3])) {
      TrigParser (1);
      downloadTrig ();
    }
    if (Hv_GetToggleButton (tb[4])) {
      movPhase12sgl (0);
      movPhase12sgl (1);
      movSE2sgl ();
      downloadSE ();
      TrigParser ();
      downloadTrig ();
    }

    tb[0]  = Hv_SimpleToggleButton (rowcol, "Scintillator Data (Phase1a)");

    it     = Hv_SimpleDialogSeparator (rowcol); 
    tb[1]  = Hv_SimpleToggleButton (rowcol, "Scintillator Data (Phase1b)");

    it     = Hv_SimpleDialogSeparator (rowcol); 
    tb[2]  = Hv_SimpleToggleButton (rowcol, "Event Data (Phase2)");

    it     = Hv_SimpleDialogSeparator (rowcol); 
    tb[3]  = Hv_SimpleToggleButton (rowcol, "Trigger Data (Phase3)");

    it     = Hv_SimpleDialogSeparator (rowcol); 
    tb[0]  = Hv_SimpleToggleButton (rowcol, "Download All Data");

    it     = Hv_SimpleDialogSeparator (rowcol); 
    tb[0]  = Hv_SimpleToggleButton (rowcol, "ClasTrig 1");

    it     = Hv_SimpleDialogSeparator (rowcol); 
    tb[1]  = Hv_SimpleToggleButton (rowcol, "ClasTrig 2");

    it     = Hv_SimpleDialogSeparator (rowcol); 
    tb[2]  = Hv_SimpleToggleButton (rowcol, "Other");
*/
/* -========================================================================- */


E 4
E 1
