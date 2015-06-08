h33288
s 00000/00000/00000
d R 1.2 02/08/25 23:21:07 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_configure.c
e
s 00488/00000/00000
d D 1.1 02/08/25 23:21:06 boiarino 1 0
c date and time created 02/08/25 23:21:06 by boiarino
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

	home = (char *)getenv("HOME");
	if (home == NULL)
	    return;

/* construct the file name */

	fname = (char *)Hv_Malloc(strlen(home) + 15);
	strcpy(fname, home);
	strcat(fname, "/.hvconfig");

	fp = fopen(fname, "r");

	Hv_Free(fname);

	if (fp == NULL) {   /* file does not exist */
	    fprintf(stderr, "No configuration file.\n");
	    return;
	}

    } /*NULL config file name */

    else { /*passed config file*/

	fp = fopen(fname, "r");

	if (fp == NULL) {   /* file does not exist */
	    fprintf(stderr, "Could not open configuration file %s\n", fname);
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
	fprintf(stderr, "Did NOT find record %s in configuration file.\n", recname);
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
	fprintf(stderr, "There were no mainingful lines in the %s record of the configuration file.\n", recname);



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
		Hv_MoveView(View, dx, dy);

	    dw = width  - View->local->width;
	    dh = height - View->local->height;

	    if ((dw != 0) || (dh != 0))
		Hv_GrowView(View, dw, dh);


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
    char     *fnold = NULL;
    char     *home;
    char     command[1024];
    char     *recname = NULL;
    FILE     *fp = NULL;
    FILE     *fpold = NULL;
    char     *line;
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
    fprintf(stderr, "Configuration record name: %s\n", recname);

    saveopt = Hv_SaveOption();

    if (saveopt < 0) {
	Hv_Free(recname);
	return;
    }

    else if (saveopt == 0) {

/* if new default, delete and .hvconfig.bak,
   mv .hvconfig to .hvconfig.bak and create new one*/

	home = (char *)getenv("HOME");
	if (home == NULL)
	    return;

	fn = (char *)Hv_Malloc(strlen(home) + 15);
	strcpy(fn, home);
	strcat(fn, "/.hvconfig");

	fprintf(stderr, "Saving to [%s]\n", fn);

	if (Hv_FileExists(fn)) {
	    fnold = (char *)Hv_Malloc(strlen(home) + 20);
	    strcpy(fnold, home);
	    strcat(fnold, "/.hvconfig.bak");
	    sprintf(command, "mv %s %s", fn, fnold);
	    system(command);

	    fp    = fopen(fn ,    "w");
	    fpold = fopen(fnold , "r");

	    if ((fp == NULL) || (fpold == NULL)) {
		fprintf(stderr, "ERROR in Hv_SaveConfiguration.");
		sprintf(command, "mv %s %s", fnold, fn);
		system(command);
		return;
	    }

	    fprintf(stderr, "Moving old .hvconfig to .hvconfig.bak\n");

/* now copy, except for this apps record */

	    line = (char *)Hv_Malloc(Hv_LINESIZE);

	    while (fgets(line, Hv_LINESIZE, fpold)) {

		if (Hv_Contained(line, recname)) { /* SKIP */
		    while (fgets(line, Hv_LINESIZE, fpold)) {
			if (Hv_Contained(line, "ENDREC"))
			    break;
		    }
		}
		else
		    fprintf(fp, "%s", line);
		
	    }


	    Hv_Free(line);
	    fclose(fpold);
	} /* done copying old config */
	else
	    fp    = fopen(fn ,    "w");


	Hv_Free(fn);

    } /* end default config */

    else { /* custom config */


	deftxt = (char *)Hv_Malloc(strlen(Hv_appName) + 25);
	sprintf(deftxt, "%s.config", Hv_appName);
	
	fn = Hv_FileSelect("Save a configuration file.",
			   "*.config",
			   deftxt);
	Hv_Free(deftxt);

	if (Hv_FileExists(fn)) {
	    sprintf(command, "rm %s", fn);
	    system(command);
	}

	fp = fopen(fn, "w");

	if (fp)
	    fprintf(stderr, "Saving to: %s", fn);
	Hv_Free(fn);

	if (fp == NULL) {
	    fprintf(stderr, "Could not open config file.");
	    return;
	}

    }

/* regardless, now write the current config into fp and close it when done*/

    fprintf(fp, "\n!----------------\n");
    fprintf(fp, "%s\n", recname);


    for (temp = Hv_views.first; temp != NULL; temp = temp->next) {
	if ((temp->tag != Hv_WELCOMEVIEW) && (temp->tag != Hv_VIRTUALVIEW)) {


	    if (Hv_ViewIsActive(temp)) {
		fprintf(stderr, "ACTIVE view\n");
		tstr = bstr[0];
	    }
	    else {
		fprintf(stderr, "INACTIVE view\n");
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


    Hv_VaCreateDialog(&dialog, Hv_TITLE, " Save Current Configuration ", NULL);
	
    rowcol = Hv_DialogColumn(dialog, 6);
	
    dummy = Hv_StandardLabel(rowcol, "Save as the default or custom configuration?", 0);
    
    
/* close out */
	
    rc = Hv_StandardActionButtons(rowcol, 30, Hv_OKBUTTON + Hv_CANCELBUTTON + Hv_APPLYBUTTON);

    dummy = Hv_GetDialogActionButton(dialog, Hv_OKBUTTON);
    Hv_ChangeLabel(dummy, " Default ", Hv_DEFAULT);
    
    dummy = Hv_GetDialogActionButton(dialog, Hv_CANCELBUTTON);
    Hv_ChangeLabel(dummy, " Custom ", Hv_DEFAULT);

    dummy = Hv_GetDialogActionButton(dialog, Hv_APPLYBUTTON);
    Hv_ChangeLabel(dummy, " Cancel ", Hv_DEFAULT);
    
    Hv_DoDialog(dialog, NULL);

    answer = Hv_GetDialogAnswer(dialog);

    XtDestroyWidget(dialog);

    if (answer == Hv_OK)
	return 0;
    else if (answer == Hv_CANCEL)
	return 1;
    else
	return -1;
    
    
}







E 1
