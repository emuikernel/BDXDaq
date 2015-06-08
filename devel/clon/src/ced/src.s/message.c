/*
----------------------------------------------------
-							
-  File:    message.c				
-							
-  Summary:						
-           Messagebox, stderr and log messages
-					       
-                     				
-  Revision history:				
-                     				
-  Date       Programmer     Comments		
------------------------------------------------------
-  2/19/95	DPH		Initial Version	
-                                              	
------------------------------------------------------
*/

#include "ced.h"


/*-------- MultilineQuestion ------------*/

int MultilineQuestion(char **lines,
		      int  flags[],
		      int  nline) {

    Hv_Widget    dialog, dummy, rowcol;
    int          i, answer;


    if ((nline < 1) || (nline > 15))
	return;

    if (nline == 1)
	return Hv_Question(lines[0]);


    Hv_VaCreateDialog(&dialog, Hv_TITLE, " Question ", NULL);

    rowcol = Hv_DialogColumn(dialog, 0);

    for (i = 0; i < nline; i++) {
	if (flags == NULL)
	    dummy = Hv_StandardLabel(rowcol, lines[i], 0);
	else
	    dummy = Hv_StandardLabel(rowcol, lines[i], flags[i]);
    }

    dummy = Hv_StandardActionButtons(rowcol, 120, Hv_OKBUTTON + Hv_CANCELBUTTON);

    answer = Hv_DoDialog(dialog, NULL);

    Hv_DestroyWidget(dialog);
    return (answer == Hv_OK);
}



/*------- StandardLogMessage --------*/

void  StandardLogMessage(char  *prompt,
			 char  *message)

/* convenience routine */


{
  int   len = 0;
  char *line;

  if (prompt)
    len += strlen(prompt); 

  if (message)
    len += strlen(message); 

  if (len < 1)
    return;
      
  line = (char *)Hv_Malloc(len + 10);

  if (prompt) {
    strcpy(line, prompt);
    strcat(line, " ");
  }

  if (message)
    strcat(line, message);
  
  LogMessage(line, CEDMESSAGE, False, False, True, False);
  
  Hv_Free(line);
}

/*-------- BroadcastMessage ---------*/

void BroadcastMessage(char *str) {

/* updates all view's message box */

    Hv_View  temp;

    for (temp = Hv_views.first;  temp != NULL;  temp = temp->next)
	if ((temp->tag > 0) && (temp->tag < 100))
	    if (Hv_ViewIsVisible(temp))
		UpdateMessageBox(temp, str);
}


/*------- UpdateMessageBox ------*/

void UpdateMessageBox(Hv_View  View,
		      char    *str)
{
  ViewData        viewdata;
  char           *errstr;
  Hv_Item         MBox;
  char *bform[7]  = {"Generic bad format",
		     "Bad DC Layer",
		     "Bad DC Wire",
		     "Bad Scint ID",
		     "Bad EC U Value",
		     "Bad EC V Value",
		     "Bad EC W Value"};

  viewdata = GetViewData(View);
  
  MBox = viewdata->MessageBoxItem;

  if (MBox != NULL) {

/* bad format preempts all message */

      if (eventBadFormat && (eventBadCode >= 0)) {
	  if (eventBadCode > 6)
	      eventBadCode = 0;
	  
	  errstr = (char *)Hv_Malloc(128);

	  if (eventBadFlag < 0)
	      sprintf(errstr, "BAD EVENT FORMAT: %s", bform[eventBadCode]);
	  else
	      sprintf(errstr, "BAD EVENT FORMAT: %s (%d)",
		      bform[eventBadCode],
		      eventBadFlag);

	  Hv_UpdateBoxItem(MBox, errstr);
	  Hv_Free(errstr);
      }
      else
	  Hv_UpdateBoxItem(MBox, str);

  }
}


/*------ LogMessage ------*/

void LogMessage(char   *message,
		int    type,
		Boolean  timestamp,
		Boolean  username,
		Boolean  echo  ,
		Boolean  echotohv)

{
  FILE   *fp[2];
  int    i;
  char   *hvdate = NULL;
  
  
  fp[0] = fopen("cedmsg.log", "a");
  
  if (echo)
    fp[1] = stderr;
  else
    fp[1] = NULL; 
  
/* username and/or timestamp ? */

  if (timestamp)
    hvdate = Hv_GetHvTime();

  username = (username && (Hv_userName != NULL));
  
  for (i = 0; i < 2; i++)
    if (fp[i]) {
      if (timestamp)
	fprintf(fp[i], "\n%s ", hvdate);
      if (username)
	fprintf(fp[i], "\t[%s]\n", Hv_userName);
      if (timestamp & !username)
	fprintf(fp[i], "\n");
    }

  Hv_Free(hvdate);
  

  if (message != NULL)
    for (i = 0; i < 2; i++)
      if (fp[i])
	
	switch (type) {
	case CEDMESSAGE:
	  fprintf(fp[i], "%s\n", message);
	  break;
      
	case CEDWARNING:
	  fprintf(fp[i], "\t[ced WARNING] %s\n", message);
	  break;
	  
	case CEDERROR:
	  fprintf(fp[i], "\t[** ced ERROR **] %s\n", message);
	  break;
	}  /* end switch */


/* echo to Hv ? */

  if (echotohv)
    
    switch (type) {
    case CEDMESSAGE:
      Hv_Information(message);
      break;
      
    case CEDWARNING:
      Hv_Warning(message);
      break;
      
    case CEDERROR:
      Hv_Warning(message);   /* there is no Hv_Error() */
      break;

    }  /* end switch */
  
  
  if (fp[0])
    fclose(fp[0]);
}

