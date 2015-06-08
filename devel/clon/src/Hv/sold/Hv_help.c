
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

static   Widget     helpText;
static   FILE      *helpfp = NULL;

/*------ local prototypes -------*/

extern int strncasecmp(const char *s1, const char *s2, size_t n);

static void     Hv_ChooseHelpTopic(Widget,
				   XtPointer,
				   XmListCallbackStruct	*);

static void     Hv_ReadHelp(char *,
			    char **);

static void     Hv_ReadHelpInfo(FILE *,
				char *);


/*----- Hv_DoHelp -----*/

void         Hv_DoHelp(void)

#define  N_COLS1      63
#define  N_ROWS1      20

/* the help dialog */

{
  static    Hv_Widget     dialog = NULL;
  static    Hv_Widget     topicsList;
  static    int           numtopics = -1;
  static    char          **topics;
  Hv_Widget               rowcol, rc, dummy;
  int                     i, j;
  char                    *tstr;


  char	*fname0;
  char  *helpfilename;
  char  *line;

/* if there is no file, open it and get the help
   topics */

  if (!helpfp) {

/* look in several places for the help file */

    fname0 = (char *)Hv_Malloc(strlen(Hv_programName) + 6);
    strcpy(fname0, Hv_programName);
    strcat(fname0, ".help");

    helpfp = Hv_LookForNamedFile(fname0, Hv_ioPaths, "r");

    Hv_Free(fname0);
    if (!helpfp) {
      if (Hv_Question("Can't find the help file. Do you want to look for it?")) {
	helpfilename = Hv_FileSelect("Please locate the help file.", "*.help", NULL);
	helpfp = fopen(helpfilename, "r");
	Hv_Free(helpfilename);
      }

/* if still don't have it, give up */

      if (!helpfp)
	return;
      
    }
  }

/* count the number of topics */

  if (numtopics < 1) {
    line = (char *)Hv_Malloc(Hv_LINESIZE);

    numtopics = 0;
    rewind(helpfp);

    while(Hv_FindRecord(helpfp, "help", False, line))
      numtopics++;


    topics = (char **)Hv_Malloc(numtopics*sizeof(char *));
    
    i = 0;
    rewind(helpfp);
    while(Hv_FindRecord(helpfp, "help_", False, line)) {
      topics[i] = (char *)Hv_Malloc(strlen(line) + 1);
      strncpy(topics[i], line + 6, strlen(line) - 7);
      topics[i][strlen(line) - 8] = '\0';
      i++;
    }
    Hv_Free(line);


/* bubble sort the topics */

    if (numtopics > 1) {
      for (i = 0; i < (numtopics - 1); i++)
	for (j = i+1; j < numtopics; j++)
	  if (strncasecmp(topics[i], topics[j], 15) > 0) {
	    tstr = topics[i];
	    topics[i] = topics[j];
	    topics[j] = tstr; 
	  }
    }
  }


/* if we have help topics, it is worthwhile to
   create the dialog box */

  if (numtopics > 0) {

    if (!dialog) {
      Hv_VaCreateDialog(&dialog,
			Hv_TITLE,  "help",
			Hv_TYPE,   Hv_MODELESS,
			NULL);

      rowcol = Hv_DialogColumn(dialog, 10);  /* rowcol to hold all other widgets */

/* a horizontal rowcol to hold the lists and text */

      rc = Hv_VaCreateDialogItem(rowcol,
				 Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
				 Hv_SPACING,      10,
				 NULL);

/* now the list of topics */

      topicsList = Hv_VaCreateDialogItem(rc,
					 Hv_TYPE,             Hv_LISTDIALOGITEM,
					 Hv_LIST,             (void *)topics,
					 Hv_NUMITEMS,         numtopics,
					 Hv_NUMVISIBLEITEMS,  Hv_sMin(N_ROWS1, numtopics),
					 Hv_CALLBACK,         Hv_ChooseHelpTopic,
					 NULL);


/* now the scrolled text for the help itself */

      helpText = Hv_VaCreateDialogItem(rc,
				       Hv_TYPE,        Hv_SCROLLEDTEXTDIALOGITEM,
				       Hv_NUMROWS,     N_ROWS1,
				       Hv_NUMCOLUMNS,  N_COLS1,
				       NULL);

/* now the ok button */

      dummy = Hv_SimpleDialogSeparator(rowcol);
      rc = Hv_DialogRow(rowcol, 10);
      dummy = Hv_StandardDoneButton(rc, " close ");
      
      for (i = 0; i < numtopics; i++)
	Hv_Free(topics[i]);
    }


/* dialog has been created */

    Hv_DoDialog(dialog, NULL);

  }
  
}

#undef  N_COLS1
#undef  N_ROWS1


/* ----- Hv_ChooseHelpTopic ------- */

static void Hv_ChooseHelpTopic(Widget w,
			       XtPointer client_data,
			       XmListCallbackStruct *cbs)

/* callback for selection of a help topic */
 
{
  char *choice;
  char *text;


  XmStringGetLtoR(cbs->item, XmSTRING_DEFAULT_CHARSET, &choice);
  Hv_ReadHelp(choice, &text);   /*mallocs space for text */

  Hv_SetString(helpText, text);
  XtFree(choice);
  Hv_Free(text);
}


/* ---- Hv_ReadHelp ------- */

static void Hv_ReadHelp(char *choice,
			char **text)
     
{
  char *RecName;
  char *line;
  int  numlines, numchars;

  line = (char *)Hv_Malloc(Hv_LINESIZE);
  RecName  = (char *)Hv_Malloc(strlen(choice) + 8);
  strcpy(RecName, "[HELP_");
  strcat(RecName, choice);

/* read the appropriate help blurb */

  if (Hv_FindRecord(helpfp, RecName, True, line)) {
    Hv_NextRecordSize(helpfp, "[HELP_", "[END]", &numlines, &numchars);
    Hv_FindRecord(helpfp, RecName, True, line);
    *text = (char *)Hv_Malloc(numchars);
    Hv_ReadHelpInfo(helpfp, *text);
  }
  else
    fprintf(stderr, "could not find record %s\n", choice);

  

  Hv_Free(RecName);
  Hv_Free(line);
}


/* ---- Hv_ReadHelpInfo --- */

static void Hv_ReadHelpInfo(FILE *fp,
			    char *text)

{
  char            *line;      /*input line*/
  char            *RECNAME_END      = "[END]";
  Boolean         found;

  line = (char *) Hv_Malloc(Hv_LINESIZE);

  strcpy (text, " ");

/*keep in mind that "C" forces us to start indices at zero*/
  
  found = False;
  while (fgets(line, Hv_LINESIZE, fp) && (!found)) {
    found = (strstr(line, RECNAME_END) != NULL);
    if (!found)
      strcat(text, line);
  }

  Hv_Free(line);
}
