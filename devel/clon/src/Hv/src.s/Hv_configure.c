/*
==================================================================================
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
*/


/*
 * This file deals with reading configuratios -- so that views
 * can be arranged and their locations and zoom states "saved".
 *
 * At startup, the file .hvconfig is searched for in
 * $HOME. If found, this file is processed. If the global
 * function pointer Hv_UserConfigure is set, then the file is
 * rewound and given to the user routine. This way, application
 * specific stuff that will be ignored by Hv can be used for
 * additional, custom configuration.
 */


/*---------------------------------------
 * Hv_ReadConfigurationFile
 * Reads a file and tries to configure accordingly.
 * 
 * fname (char *) FULL pathname. If NULL, will
 *                try $HOME/.hvconfig 
 *--------------------------------------*/

#include "Hv.h"

static int Hv_SaveOption();


static char * getHome();

void Hv_ReadConfigurationFile(char *fname) {

    int     i, nl, numnc;
    char    *home;
    FILE    *fp;
    char    *recname = NULL;
    char    *line = NULL;
    Hv_View temp = NULL;

/* NULL fname means look for the config file, .hvconfig
   in your home directory */

    if (fname == NULL) {

/* called if you hit the "never again" button */

/* use the HOME env variable to get the home directory.
   If can't get it, just exit quietly and ignore the
   whole UpdateMessage process */

	    home = getHome();
	    if (home == NULL)
	        return;

/* construct the file name */

	    fname = (char *)Hv_Malloc(strlen(home) + 15);
	    strcpy(fname, home);
	    strcat(fname, "/.hvconfig");
        Hv_FixFileSeparator(fname);  /* this speeds up file opens on WIN98 */
	    fp = fopen(fname, "r");

	    Hv_Free(fname);

	    if (fp == NULL) {   /* file does not exist */
	        return;
	    }

    } /*NULL config file name */

    else { /*passed config file*/
        Hv_FixFileSeparator(fname);  /* this speeds up file opens on WIN98 */
	    fp = fopen(fname, "r");

	    if (fp == NULL) {   /* file does not exist */
	        Hv_Println("Could not open configuration file %s", fname);
	        return;
	    }
    }


/* free previous config file */

    if (Hv_configuration != NULL) {

	    i = 0;
	    while(Hv_configuration[i] != NULL) {
	        Hv_Free(Hv_configuration[i]);
	        i++;
	    }
	    Hv_Free(Hv_configuration);
	    Hv_configuration = NULL;
    }

/* get the record name for this app */

    recname = (char *)Hv_Malloc(strlen(Hv_appName) + 25);
    sprintf(recname, "[RECORD %s]", Hv_appName);

    line = (char *)Hv_Malloc(Hv_LINESIZE);

    if (!Hv_FindRecord(fp, recname, False, line)) {
	fclose(fp);
	Hv_Free(recname);
	Hv_Free(line);
	return;
    }


/* process new file */

    Hv_RecordLineCount(fp, recname, "ENDREC", &nl, &numnc);

    if (numnc > 0) {

	Hv_configuration = (char **)Hv_Malloc((numnc+1)*sizeof(char *));

/* pass to user routine, if there is one */

	i = 0;
	while ((i < numnc) && (Hv_ReadNextLine(fp, line, recname, "ENDREC") == 0)) {
	    if (Hv_NotAComment(line)) {
		Hv_InitCharStr(&(Hv_configuration[i]), line);
		Hv_ReplaceNewLine(Hv_configuration[i]);
		i++;
	    }

	}

	Hv_configuration[numnc] = NULL; /*NULL terminated array*/

/* give to user routine */

	if (Hv_UserReadConfiguration != NULL)
	    Hv_UserReadConfiguration(Hv_configuration);


/* apply to any views already created */

	for (temp = Hv_views.first; temp != NULL; temp = temp->next)
	    Hv_ApplyConfiguration(temp);

    }
    else 
	Hv_Println("There were no meaningful lines in the %s record of the configuration file.", 
		   recname);



    fclose(fp);
    Hv_Free(recname);
    Hv_Free(line);
}



/*-----------------------------------------------------------
 *  Hv_ApplyConfiguration
 * Applies current config to the given view
 *-----------------------------------------------------------*/

void Hv_ApplyConfiguration(Hv_View View) {

    char    *line;
    int      i;
    char     dum1[64], dum2[64];
    Boolean  active;
    int      left, top, width, height;
    float    xmin, ymin, fw, fh;
    int      dx, dy, dw, dh;

    if (Hv_configuration == NULL)
	return;

    if ((View->tag == Hv_WELCOMEVIEW) || (View->tag == Hv_VIRTUALVIEW)) 
	return;

    i = 0;
    while (Hv_configuration[i] != NULL) {

	line = Hv_configuration[i];

	if (Hv_Contained(line, "VIEW") && Hv_Contained(line, Hv_GetViewTitle(View))) {

	    sscanf(line, "%s %s %d %d %d %d %f %f %f %f",
		   dum1, dum2,
		   &left, &top,  &width, &height,
		   &xmin, &ymin, &fw, &fh);


	    active = Hv_Contained(dum2, "rue");

	    dx = left - View->local->left;
	    dy = top  - View->local->top;

	    if ((dx != 0) || (dy != 0))
		Hv_MoveView(View, (short)dx, (short)dy);

	    dw = width  - View->local->width;
	    dh = height - View->local->height;

	    if ((dw != 0) || (dh != 0))
		Hv_GrowView(View, (short)dw, (short)dh);


	    Hv_QuickZoomView(View, xmin, xmin+fw, ymin, ymin+fh);


	    if (active && !(Hv_ViewIsActive(View)))
		Hv_OpenView(View);
	    else if (!active && Hv_ViewIsActive(View))
		Hv_CloseView(View);

	    break;
	}

	i++;
    }


}


/*---------------------------------------
 * Hv_OpenConfiguration
 * Provides a file dialog to select a config
 * file and passes it to Hv_ReadConfigurationFile
 *--------------------------------------*/

void Hv_OpenConfiguration() {

    char *fn = NULL;

    fn = Hv_FileSelect("Open a configuration file.",
		       "*.config",
		       NULL);

    if (fn != NULL) {
	Hv_ReadConfigurationFile(fn);
	Hv_Free(fn);
    }
}

/*---------------------------------------
 * Hv_SaveConfiguration
 * Provides a file dialog to select a location
 * for the current configuration. Passes
 * along to an optional user routine so
 * that it can provide some additional info.
 *--------------------------------------*/

void Hv_SaveConfiguration() {
  
  char     *fn;
  char     *home;
  
  char     *recname = NULL;
  FILE     *fp = NULL;
  
  Hv_View   temp;
  char     *bstr[2] = {"true", "false"};
  char     *tstr;
  char     *deftxt;
  Hv_FRect world;
  int      saveopt;
  
  if (Hv_appName == NULL)
    return;
  
  /* get the record name for this app */
  
  recname = (char *)Hv_Malloc(strlen(Hv_appName) + 25);
  sprintf(recname, "[RECORD %s]", Hv_appName);
  
  saveopt = Hv_SaveOption();
  
  if (saveopt < 0) {
    Hv_Free(recname);
    return;
  }
  
  else if (saveopt == 0) { /* save default file */
    
                           /* if new default, delete and .hvconfig.bak,
    mv .hvconfig to .hvconfig.bak and create new one*/
    
    home = getHome();
    if (home == NULL)
      return;
    
    fn = (char *)Hv_Malloc(strlen(home) + 15);
    strcpy(fn, home);
    strcat(fn, "/.hvconfig");
  } /* end default config */
  
  else { /* custom config */
    deftxt = (char *)Hv_Malloc(strlen(Hv_appName) + 25);
    sprintf(deftxt, "%s.config", Hv_appName);
    
    fn = Hv_FileSelect("Save a configuration file.",
			   "*.config",
         deftxt);
    Hv_Free(deftxt);
    
    if (fn == NULL)
      return;
  } /* end custom save */
  
  /* regardless, now write the current config into fp and close it when done*/
  
  fp = fopen(fn, "w");
  Hv_Free(fn);
  
  if (fp == NULL) {
    Hv_Println("Could not open config file.");
    return;
  }
  
  fprintf(fp, "\n!----------------\n");
  fprintf(fp, "%s\n", recname);
  
  
  for (temp = Hv_views.first; temp != NULL; temp = temp->next) {
    if ((temp->tag != Hv_WELCOMEVIEW) && (temp->tag != Hv_VIRTUALVIEW)) {
      
      
      if (Hv_ViewIsActive(temp)) {
       tstr = bstr[0];
      }
      else {
       tstr = bstr[1];
      }
      
      Hv_LocalRectToWorldRect(temp, temp->hotrect, &world);
      
      fprintf(fp, "VIEW %s %d %d %d %d %f %f %f %f %s\n",
        tstr,
        temp->local->left,
        temp->local->top,
        temp->local->width,
        temp->local->height,
        world.xmin,
        world.ymin,
        world.width,
        world.height,
        Hv_GetViewTitle(temp));
    }
  }
  
  /* give to user writer */
  
  if (Hv_UserWriteConfiguration != NULL)
    Hv_UserWriteConfiguration(fp);
  
  
  fprintf(fp, "[ENDRECORD]\n");
  
  fclose(fp);
  
  if (recname != NULL)
    Hv_Free(recname);
}


/*----------- Hv_SaveOption -----------*/


static int Hv_SaveOption() {

    Hv_Widget   rc, rowcol, dummy;
    Hv_Widget   dialog;
    int         answer;

    static Hv_RadioHeadPtr  configlist = NULL;

    Hv_VaCreateDialog(&dialog, Hv_TITLE, " Save Current Configuration ", NULL);
	
    rowcol = Hv_DialogColumn(dialog, 6);
	
    dummy = Hv_StandardLabel(rowcol, "Save as the default or custom configuration?", 0);
    
        dummy = Hv_VaCreateDialogItem(rowcol,
				  Hv_TYPE,         Hv_RADIODIALOGITEM,
				  Hv_RADIOLIST,    &configlist,
				  Hv_FONT,         Hv_helvetica14,
				  Hv_OPTION,       "Default",
				  Hv_OPTION,       "Custom",
				  Hv_NOWON,        1,   /* sets FP as default on */
				  NULL);

/* close out */
	
    rc = Hv_StandardActionButtons(rowcol, 30, Hv_OKBUTTON + Hv_CANCELBUTTON );

     
    Hv_DoDialog(dialog, NULL);
    answer = Hv_GetDialogAnswer(dialog);

    Hv_DestroyWidget(dialog);

    if (answer == Hv_OK)
	return Hv_GetActiveRadioButtonPos(configlist) -1;
    else 
	return -1;
    
    
}



/**
 *  getHome
 * @return The HOME environment variable. On win platforms,
 * will return C:\ if no HOME env var.
 */

static char * getHome() {

    char *home = NULL;
    home = (char *)getenv("HOME");

    if (home == NULL) {
#ifdef WIN32
        Hv_InitCharStr(&home, "C:\\");
#endif
    }
    return home;

}


