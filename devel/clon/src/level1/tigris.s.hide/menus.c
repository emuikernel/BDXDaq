
#ifndef VXWORKS

/* menus.c */

#include <string.h>
#include <strings.h>
#include "Hv.h"
#include "ced.h"
#include "tigris.h"

extern char InFileName[512]; /* defined in fileio.c */
extern char dlhn[1024]; /* defined in fileio.c */
extern struct phase3_def phase3; /* defined in fileio.c */

/* local variables */

static Hv_RadioHeadPtr triglist;

/*Sergey: async inputs control menu function - probably do not need it ... */
void
AsyncRadioClick(Hv_Event hevent)
{
  printf("AsyncRadioClick reached\n");
}

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


/*Sergey: async inputs control menu function */
void
AsyncControl(Hv_Widget w)
{
  static Hv_Widget dialog = NULL;
  static Hv_Widget dummy, rowcol, rc;
  static Hv_Widget async_enable[8];
  int i, j, k, se_yes;

  if(!dialog)
  {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, "Async Control", NULL);
    rowcol = Hv_DialogColumn(dialog, 0);

    rc = Hv_DialogColumn(rowcol, 0);

    dummy = Hv_StandardLabel(rc, "  Async input 1", NULL);
    dummy = Hv_SimpleDialogSeparator(rc);
    async_enable[0] = Hv_SimpleToggleButton(rc," Enable with TS bit 1");
    dummy = Hv_SimpleDialogSeparator(rc);
    async_enable[1] = Hv_SimpleToggleButton(rc," Enable with TS bit 2");
    dummy = Hv_SimpleDialogSeparator(rc);
    dummy = Hv_SimpleDialogSeparator(rc);

    dummy = Hv_StandardLabel(rc, "  Async input 2", NULL);
    dummy = Hv_SimpleDialogSeparator(rc);
    async_enable[2] = Hv_SimpleToggleButton(rc," Enable with TS bit 3");
    dummy = Hv_SimpleDialogSeparator(rc);
    async_enable[3] = Hv_SimpleToggleButton(rc," Enable with TS bit 4");
    dummy = Hv_SimpleDialogSeparator(rc);
    dummy = Hv_SimpleDialogSeparator(rc);

    dummy = Hv_StandardLabel(rc, "  Async input 3", NULL);
    dummy = Hv_SimpleDialogSeparator(rc);
    async_enable[4] = Hv_SimpleToggleButton(rc," Enable with TS bit 5");
    dummy = Hv_SimpleDialogSeparator(rc);
    async_enable[5] = Hv_SimpleToggleButton(rc," Enable with TS bit 6");
    dummy = Hv_SimpleDialogSeparator(rc);
    dummy = Hv_SimpleDialogSeparator(rc);

    dummy = Hv_StandardLabel(rc, "  Async input 4", NULL);
    dummy = Hv_SimpleDialogSeparator(rc);
    async_enable[6] = Hv_SimpleToggleButton(rc," Enable with TS bit 7");
    dummy = Hv_SimpleDialogSeparator(rc);
    async_enable[7] = Hv_SimpleToggleButton(rc," Enable with TS bit 8");

    dummy = Hv_StandardActionButtons(rowcol,60,Hv_OKBUTTON+Hv_CANCELBUTTON);
  }
  
  for(j=0; j<8; j++)
  {
    if(phase3.Enable_Async[j+1] == 1)
    {
      Hv_SetToggleButton(async_enable[j],1);
    }
    else
    {
      Hv_SetToggleButton(async_enable[j],0);
    }
  }


  if((Hv_DoDialog(dialog, NULL)) == Hv_OK)
  {
    printf("AsyncControl: \n");

    for(j=0; j<8; j++)
    {
      phase3.Enable_Async[j+1] = Hv_GetToggleButton(async_enable[j]);
      printf("%d ",phase3.Enable_Async[j+1]);
    }
    printf("\n");
  }
}




/* ----- the scintillator event download button */

void
DownloadControl(Hv_Widget w)
{
  Hv_View               temp;
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


  if((Hv_DoDialog (dialog, NULL)) == Hv_OK)
  {
    _z = Hv_PosGetActiveRadioButton(triglist);
    switch (_z) {
    case 1:
      strcpy(dlhn,"clastrig2");
      movPhase12sgl(0);
      movPhase12sgl(1);
      movSE2sgl();
      downloadSE();
      TrigParser ();
      downloadTrig ();
      break;
    case 2:
      strcpy(dlhn,"clastrig2.jlab.org");
      movPhase12sgl(0);
      movPhase12sgl(1);
      movSE2sgl();
      downloadSE();
      TrigParser ();
      downloadTrig ();
      break;
    case 3:
      strcpy(dlhn,Hv_GetString(downloadTarget));
      movPhase12sgl(0);
      movPhase12sgl(1);
      movSE2sgl();
      downloadSE();
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

#else /* UNIX only */

void
menus_dummy()
{
}

#endif
