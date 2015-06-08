h41729
s 00002/00002/00883
d D 1.7 08/04/21 13:10:08 heddle 8 7
c removed annoying splash dialog
c 
e
s 00001/00001/00884
d D 1.6 08/04/16 17:39:19 heddle 7 6
c new version number
c 
e
s 00003/00002/00882
d D 1.5 07/11/12 16:41:06 heddle 6 5
c new start counter
e
s 00020/00009/00864
d D 1.4 07/10/26 10:02:45 heddle 5 4
c added more scints
e
s 00002/00001/00871
d D 1.3 06/06/28 17:16:01 boiarino 4 3
c use WITH_OFFLINE
e
s 00002/00000/00870
d D 1.2 03/04/17 16:42:53 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 02/09/09 16:27:11 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 ced/init.c
e
s 00870/00000/00000
d D 1.1 02/09/09 16:27:10 boiarino 1 0
c date and time created 02/09/09 16:27:10 by boiarino
e
u
U
f e 0
t
T
I 1
/*
----------------------------------------------------
-							
-  File:    init.c				
-							
-  Summary:						
-            Primary Initialization
-					       
-                     				
-  Revision history:				
-                     				
-  Date       Programmer     Comments		
------------------------------------------------------
-  10/19/94	DPH		Initial Version	
-
-    *** version 2100
-        F1 magnify
-        TB tracks
-        Geo from map
-        PART bank
-        CEDDATADIR env var 
-    *** version 2032
-        dead wires from the map
-    *** version 2031
-        start counter fixed, svae config works
-    *** version 2030
-        tagger working
-    *** version 2029 skipped
-    *** version 2028
-        fixed event writing 
-    *** verison 2027
-        draw hits from dc1 used to make tracks
-    *** version 2026
-        new track banks HBLA and HDPL
-    *** version 2025
-      -> added tagger support                                             	
------------------------------------------------------
*/

#include "ced.h"
#include "Hv_init.h"

/*=====================================
      Global Variables 
=======================================*/

int            runnumber = 1;       /* current run number */
Boolean        DCGeomBanks = False; /* will -> True if uses DC geo banks */


D 6
float          cedversion = 2.100      /* current version */;
E 6
I 6
D 7
float          cedversion = 2.200      /* current version */;
E 7
I 7
D 8
float          cedversion = 2.201      /* current version */;
E 8
I 8
float          cedversion = 2.202      /* current version */;
E 8
E 7
E 6
FieldMap       fmap;	               /* current field map */
short          eventsource;            /* where events come from */

int            eventwhen;              /* on demand or from timer */
int            eventtime;              /* seconds to view when "on timer" */    
Hv_IntervalId  eventinterval;          /* for the timer */
Boolean        precountEvents = False; /* if True, scan the event file when opened */
char          *eventFileFilter;        /* filter used when looking for event files */

Boolean        showsplash = True;      /*controls whether splash screen is displayed */

/* these are added to catch event format problems */

Boolean        eventBadFormat = False;
int            eventBadCode = -1;
int            eventBadFlag = -1;

/* tagger data */

int             tagemin = 10;
int             tagemax = 4000;
int             tagtmin = 10;
int             tagtmax = 10000;
short           taglowcolor;
short           taghigcolor;
short           tagtjust1color;
short           tagtbothcolor;
short           tageinanytrangecolor;
short           tageinnotrangecolor;

/* startc counter */

int             starttdcmin = 10;
int             starttdcmax = 20000;
int             startadcmin = 10;
int             startadcmax = 20000;
short           starthitcolor;
short           starttaglowcolor;
short           starttaghighcolor;


/* dc data */
    
short           dcframecolor;
short           dcfillcolor;
short           dchitcolor;
short           dcdcacolor;
short           dctrigsegcolor;
short           dcaddedbytrigcolor;
short           dcnoisecolor;
short           dcfromdc1color;
short           dcbadtimecolor;
short           dcdeadwirecolor;
int             dcmintime[3];
int             dcmaxtime[3];

/* other detector colors */

short           ecframecolor[3]; /* array for inner, outer */
short           ecfillcolor[3];  /* array for inner, outer */
short           echitcolor;

short           scintframecolor;
short           scintfillcolor;
short           scinthitcolor;
short           scintdeadcolor;

short           ec1framecolor; /* array for inner, outer */
short           ec1fillcolor[2];  /* array for inner, outer */
short           ec1hitcolor;

/* drift chamber DCA constants */

float           dca_a[3];
float           dca_b[3];
    
/* pixel densities control when various dc features are drawn */
    
short           dcbighitwires[MAX_REGIONS];
short           dcnormalwire[MAX_REGIONS];
short           dchitcircle[MAX_REGIONS];
short           dchexagon[MAX_REGIONS];
    
/* drift chamber noise reduction parameters */
    
Boolean         reducenoise;
int             missinglayers[3][2];  /* rgn, supl */
int             layershifts[6];

/* standardize some colors */

short boxcolor;
short fbcolor;
short radioarmcolor;
short optionarmcolor;

/* ------ local prototypes -------*/

static void MapDump();

static char *InitFileName();

static void SplashDialog();

static void InitColors();

static void DCBitDump();

static void AppendLogFile();

static void DisplayUsage();

static void WindowTile(void);

static void FinalInit(void);

static void InitControls(void);

static void AppendLogFile(void);

static void CommandLine();

static Boolean RecognizedFlag();

static void       UpdateMessage();

static void WriteInitFile(char *fname,
			  Boolean ss);

static void SplashCallback();

/*------ Init() ----*/

void Init(void)

{

D 6
  Hv_ChangeBackgroundColor(Hv_blue-9);
E 6
I 6
  /*  Hv_ChangeBackgroundColor(Hv_blue-9); */
  Hv_ChangeBackgroundColor(Hv_gray12);
E 6

  AppendLogFile();
  MakeMenus();
  InitControls();
  InitGeometry();
  InitBOS();
  InitField();
  CommandLine();

  InitDCData();
  InitColors();
  FinalInit();

}

/*---------- InitColors ---------*/

static void InitColors() {

/* ec colors for ec fanned view */

    ecframecolor[ECINNER] = Hv_black;
    ecfillcolor[ECINNER] = Hv_gray14;
    ecframecolor[ECOUTER] = Hv_black;
    ecfillcolor[ECOUTER] = Hv_gray12;
    echitcolor = Hv_red;

    ec1framecolor = Hv_black;
    ec1fillcolor[0] = Hv_gray14;
    ec1fillcolor[1] = Hv_gray12;
    ec1hitcolor = Hv_red;

/* scints */

    scintframecolor = Hv_black;
    scintfillcolor = Hv_white;
    scinthitcolor = Hv_red;
    scintdeadcolor = Hv_gray8;


/*------ some item defaults --*/

    boxcolor = Hv_gray11;
    fbcolor  = Hv_gray14;
    radioarmcolor = -1;
    optionarmcolor = -1;


    taglowcolor = Hv_gray9;
    taghigcolor = Hv_white;
    tagtjust1color = Hv_red;
    tagtbothcolor = Hv_darkGreen;
    tageinanytrangecolor = Hv_red;
    tageinnotrangecolor = Hv_black;
    
    starthitcolor = Hv_red;
    starttaglowcolor = Hv_gray9;
    starttaghighcolor = Hv_white;

/*---- dialog box colors ---*/
    
    Hv_simpleTextFg = Hv_black;
    Hv_simpleTextBg = -1;     /* used to be Hv_honeydew */
    Hv_simpleTextBorder = -1; /* used to be default */

}


/*-------- WindowTile -----------*/

static void WindowTile(void)

{
  Hv_tileX = 340;
  Hv_tileY = 170;
  Hv_TileDraw = DrawTile;
}

/*----- FinalInit -------*/

static void FinalInit(void)

{
  Hv_View  view;


  Hv_AddFunctionKeyCallback(6, BosGeometryDump);
  Hv_AddFunctionKeyCallback(7, DCBitDump);
  Hv_AddFunctionKeyCallback(1, NextEvent);
  Hv_AddFunctionKeyCallback(2, PrevEvent);
  Hv_AddFunctionKeyCallback(3, MapDump);
  
/* initial Views */

  view = NewView(SECTOR);
  view = NewView(SECTOR);
  view = NewView(SECTOR);
  view = NewView(CALORIMETER);
  view = NewView(ALLDC);
  view = NewView(SCINT);

  InitFeedback();

  WindowTile();
  EventButtonSelectability();

  UpdateMessage();

D 8
  Hv_FinalUserInit = (Hv_FunctionPtr)SplashDialog;
E 8
I 8
  //  Hv_FinalUserInit = (Hv_FunctionPtr)SplashDialog;
E 8

#ifdef NO_ET
fprintf(stderr, "\n****************************************\n");
fprintf(stderr, "*                                       *\n");
fprintf(stderr, "*  THIS VERSION WAS NOT LINKED WITH THE *\n");
fprintf(stderr, "*  ET LIBRARIES. OBTAINING EVENTS FROM  *\n");
fprintf(stderr, "*  THE ET BUFFER WILL NOT BE POSSIBLE.  *\n");
fprintf(stderr, "*                                       *\n");
fprintf(stderr, "****************************************\n\n");

#endif

  Hv_magfact = 3; 
}


/*-------- MapDump ---------*/

static void MapDump() {
D 4

E 4
I 4
#ifdef WITH_OFFLINE
E 4
    char      *base;
    char      *dir;
    char      *fn;
    char      subsystemname[256];
    char      nextsubsystemname[256];
    char      itemname[256];
    char      nextitemname[256];
    int       nitems;
    int       arraylength, arraytype, narry;
    int       status;
    int       i, j;
    float     *vals;
    int       firsttime;

    char    *mapnames []  = {"DC_STATUS.map",
			     "DC_GEOM.map"};

/* test the map manager -- first try to
   get the name of the parms dir*/

    base = (char *)getenv("CLAS_PARMS");
    if (base == NULL) {
	base = (char *)getenv("CLON_PARMS");
	if (base == NULL)
	    return;
    }

    dir = (char *)Hv_Malloc(strlen(base) + 10);
    sprintf(dir, "%s/Maps/", base);

    fprintf(stderr, "Maps directory: %s\n", dir);

/* dc geometry */


    for (i = 0; i < 2; i++) {

	fn = (char *)Hv_Malloc(strlen(dir) + 20);

	sprintf(fn, "%s%s", dir, mapnames[i]);

	fprintf(stderr, "\n\t+++ Map Name: %s\n", mapnames[i]);

/* list all the subsystems */

	strcpy(subsystemname, "*");

	while (True) {
	    status = map_next_sub(fn, subsystemname, nextsubsystemname, &nitems);
	    if (status < 0) {
		fprintf(stderr, "Error in map_next_sub status = %d\n", status);
		break;
	    }
	    
	    if (strcmp(nextsubsystemname, "*") == 0) {
		fprintf(stderr, "-----------------------\n");
		break;
	    }
	    
	    strcpy(subsystemname, nextsubsystemname);
	    fprintf(stderr, "%s has %d items.\n", subsystemname, nitems);

/* now get the items */

	    strcpy(itemname, "*");
	    
	    
	    while (True) {
		status = map_next_item(fn,
				       subsystemname,
				       itemname,
				       nextitemname,
				       &arraylength, &arraytype, &narry);
		if (status < 0) {
		    fprintf(stderr, "\tError in map_next_item status = %d\n", status);
		    break;
		}
		
		if (strcmp(nextitemname, "*") == 0) {
		    fprintf(stderr, "\t==============\n");
		    break;
		}
		
		strcpy(itemname, nextitemname);
		fprintf(stderr, "\t%s\tarraylen %d\tarraytype %d\tnarry %d.\n",
			itemname, arraylength, arraytype, narry);

/* note the narry is irrelevant, it simply states how many of the arrays
   are in the map database, i.e. narry will grow without bounds 
   (atime  should be run number, firsttime will be unix time*/


		if (i == 1) {

		    vals = (float *)Hv_Malloc(arraylength*sizeof(float));


		
		    status = map_get_float(fn, subsystemname, itemname,
				       arraylength, vals, 0, &firsttime);
		
		    for (j = 0; j < arraylength; j++)
			fprintf(stderr, "\t\t%f\n", vals[j]);
		
		    Hv_Free(vals);
		}
		
	    }
	    
	}

	fprintf(stderr, "end of loop i = %d\n", i);
	Hv_Free(fn);
    }
    Hv_Free(dir);
I 4
#endif
E 4
}


/*-------- UpdateMessage ------*/

static void UpdateMessage()

/********************************************
  Looks for the file .cedinit in the user's
  home directory and finds the last version
  of ced run by this user. If it is earlier
  than the present version, give a message 
  telling the user that he should read the
  README.
*********************************************/

{

/* set lastversion to 2.03, the last version that did NOT
   use this version check facility */

  float     lastversion = 2.023;
  Boolean   update = False;
  char      *fname;
  FILE      *fp;
  char      *versrecname, *line;


  fname = InitFileName();
  if (fname == NULL)
      return;

  fp = fopen(fname, "r");

  if (fp == NULL)   /* file does not exist */
    update = True;
  else  if ((fp = fopen(fname, "r")) != NULL) {
    line = (char *)Hv_Malloc(256);
    Hv_InitCharStr(&versrecname, "[VERSIONREC]");
    if (Hv_FindRecord(fp, versrecname, False, line)) {

	showsplash = True;

/* 1st line of version record contains the version */


	if (Hv_GetNextNonComment(fp,  line))
	    sscanf(line, "%f", &lastversion);

/* second line contains the splash screen instructions */

	if (Hv_GetNextNonComment(fp,  line))
	    if (Hv_Contained(line, "splash") && Hv_Contained(line, "false"))
		showsplash = False;
    }

    update = (cedversion > lastversion);
    Hv_Free(versrecname);
    Hv_Free(line);
  }

/* update message? */

  if (update)
      WriteInitFile(fname, True);

  Hv_Free(fname);
}


/*-------- SplashCallback -------*/

static void SplashCallback() {

    char  *fname;

    fname = InitFileName();
    if (fname == NULL)
	return;
    
    WriteInitFile(fname, False);
}


/* ----- InitFileName -------*/

static char *InitFileName() {

    char     *fname;
    char     *home;

/* called if you hit the "never again" button */

/* use the HOME env variable to get the home directory.
   If can't get it, just exit quietly and ignore the
   whole UpdateMessage process */

    home = (char *)getenv("HOME");
    if (home == NULL)
	return NULL;
    
/* construct the file name */

    fname = (char *)Hv_Malloc(strlen(home) + 15);
    strcpy(fname, home);
    strcat(fname, "/.cedinit");
    return fname;
}


/*------- SplashDialog --------*/

static void SplashDialog() {
    char       *lines[] = {"ced version 2.10",
			   "F1-key magnify box",
			   "Time based tracks",
			   "Continuous Zoom",
			   "Real DOCA",
			   "ET support",
			   "Run-based DC Geometry from Map",
			   "CEDDATADIR env var"};
    
    int         flags[] = {99, 0, 0, 0, 0, 0, 0, 0};
    
    if (!showsplash)
	return;
    

    Hv_SelfDestructInformation(lines, flags, 8, 15,
			       "Welcome",
			       "Do NOT show me this message ever again.",
			       (Hv_FunctionPtr)SplashCallback);

}


/*------ WriteInitFile --------*/

static void WriteInitFile(char *fname,
			  Boolean ss)

{
  FILE   *fp;
  char   *command;

  command = (char *)Hv_Malloc(20 + strlen(fname));
  strcpy(command, "rm -f ");
  strcat(command, fname);
  system(command);

  fp = fopen(fname, "w");
  if (fp != NULL) {
    fprintf(fp, "[VERSIONREC]\n");
    fprintf(fp, "  %-8.3f\n", cedversion);

    if (ss)
	fprintf(fp, "  showsplash   TRUE\n");
    else
	fprintf(fp, "  showsplash   FALSE\n");

    fprintf(fp, "[ENDREC]\n");
    fclose(fp);
  }


  Hv_Free(command);
}

/*------- DCBitDump -----------*/

static void DCBitDump()

/* diagnostic procedure for dumping Bit form of
   dc data used in noise removal */

{
    int sect, rgn, supl;
    int nlay, lay;

I 3
	/*Sergey: temporary
E 3
    for (sect = 0; sect < 6; sect++)
	for (rgn = 0; rgn < 3; rgn++)
	    for (supl = 0; supl < 2; supl++) {
		fprintf(stderr, "\n---------- (sect, rgn, supl) = (%d, %d, %d) --------\n",
			sect+1, rgn+1, supl+1);

		nlay = dcbitdata[sect][rgn][supl].nlayer;
		
		for (lay = 0; lay < nlay; lay++) 
		    PrintHugeWord(dcbitdata[sect][rgn][supl].data[lay]); 
	    }    
I 3
	*/
E 3
}


/*------------ CommandLine  ----------*/

static void CommandLine() 
{
    int      i;
    Boolean  badargs   = False;
    Boolean  showeduse = False;
    char     *fname    = NULL;


    eventsource = CED_FROMFILE;
    eventwhen = CED_ONDEMAND;
    eventtime = 2;
    eventinterval = 0;

/* process left over command line arguments */

    if (Hv_leftOverArgs == NULL)
	return;


/* look for command lines that I understand 
   NOTE argv[0] will be "ced" so start at 1*/

    for (i = 1; i < Hv_argc; i++) {
	if ((strcmp(Hv_argv[i], "-u") == 0) || (strcmp(Hv_argv[i], "-h") == 0)) {
	    if (!showeduse)
		DisplayUsage();
	    showeduse = True;
	}

/* from a file flag? */

	else if (strcmp(Hv_argv[i], "-f") == 0) {
	    eventsource = CED_FROMFILE;
	    if ((i < (Hv_argc-1)) && !RecognizedFlag(Hv_argv[i+1])) {
		Hv_InitCharStr(&fname, Hv_argv[i+1]);
		OpenEventFile(fname);
		Hv_Free(fname);
		i++;
	    }
	}


/* from et flag? */

	else if (strcmp(Hv_argv[i], "-et") == 0) {
	    eventsource = CED_FROMET;
	}


	else {
	    badargs = True;
	    fprintf(stderr, "WARNING: did not understand \"%s\" on the command line\n", Hv_argv[i]);
	}

    }

/* id there were any bad args, display usage (unless already did) */

    if (badargs && !showeduse)
	DisplayUsage();

}


/*-------- RecognizedFlag -------*/

static Boolean RecognizedFlag(char *str)

{
    if ((strcmp(str, "-et") == 0) || 
	(strcmp(str, "-e") == 0) ||
	(strcmp(str, "-f") == 0) ||
	(strcmp(str, "-h") == 0) ||
	(strcmp(str, "-u") == 0))
	return True;
    else
	return False;
}


/*------- DisplayUsage ----------*/


static void DisplayUsage()

{
    fprintf(stderr, "\n=============================================================\n");
    fprintf(stderr, "ced has no required arguments. The optional arguments are:\n\n");
    fprintf(stderr, "\n\t-et \n");
    fprintf(stderr, "\t\tSets \"from et buffer\" as the default source for events.\n");
    
    fprintf(stderr, "\n\t-f  [name]\n");
    fprintf(stderr, "\t\tSets \" from file\" as the default source for events.\n");
    fprintf(stderr, "\t\tIf \"name\" is not provided, you will have to use the event\n");
    fprintf(stderr, "\t\tmenu to select a file.\n");

    fprintf(stderr, "\n\t-h\n");
    fprintf(stderr, "\t\tPrints this message.\n");

    fprintf(stderr, "\n\t-u\n");
    fprintf(stderr, "\t\tPrints this message.\n");

    fprintf(stderr, "if combinations of the -et, -e, and -f arguments are given\n");
    fprintf(stderr, "the priority in terms of the default event source is\n\n");
    fprintf(stderr, "\t(1) ET\n");
    fprintf(stderr, "\t(2) File\n");
    fprintf(stderr, "\n=============================================================\n");
}



/* ----- InitControls -----*/

static void  InitControls(void)

/* Initialize the variables that define the state and determine flow */

{ 

  char     *tstr;
  char     *ts2;
  char     *dst;
  Boolean  parmsenv = False;

/* intialize all control variables */

/* add paths for file searching */

/* first from environment variables */

I 5
  tstr = (char *)getenv("PWD");
  if (tstr != NULL) {
      fprintf(stderr, "Environment variable PWD: %s\n", tstr);
      fprintf(stderr, "That's one place I will search for clas.geometry.\n");
      Hv_AddPath(tstr);
      parmsenv = True;
  }
  else {
      fprintf(stderr, "Environment variable CLAS_PARMS not set.\n");
  }

E 5
  tstr = (char *)getenv("CLAS_PARMS");
  if (tstr != NULL) {
      fprintf(stderr, "Environment variable CLAS_PARMS: %s\n", tstr);
D 5
      fprintf(stderr, "That's one place I will search for the geometry file.\n");
E 5
I 5
      fprintf(stderr, "That's one place I will search for clas.geometry.\n");
E 5
      Hv_AddPath(tstr);
      parmsenv = True;
  }
D 5
  else
E 5
I 5
  else {
E 5
      fprintf(stderr, "Environment variable CLAS_PARMS not set.\n");
I 5
  }
E 5

D 5

E 5
  tstr = (char *)getenv("CLON_PARMS");
  if (tstr != NULL) {
      fprintf(stderr, "Environment variable CLON_PARMS: %s\n", tstr);
D 5
      fprintf(stderr, "That's one place I will search for the geometry file.\n");
E 5
I 5
      fprintf(stderr, "That's one place I will search for clas.geometry.\n");
E 5
      Hv_AddPath(tstr);
      parmsenv = True;
  }
D 5
  else
E 5
I 5
  else {
E 5
      fprintf(stderr, "Environment variable CLON_PARMS not set.\n");
I 5
  }
E 5

D 5

E 5
/* neither set? */

D 5
  if (!parmsenv)
E 5
I 5
  if (!parmsenv) {
E 5
      fprintf(stderr, "Neither CLAS_PARMS or CLON_PARMS set. That's rarely a good sign\n");
I 5
  }
E 5

D 5

E 5
  tstr = (char *)getenv("CEDGEODIR");
  if (tstr != NULL) {
    fprintf(stderr, "Environment variable CEDGEODIR: %s\n", tstr);
D 5
    fprintf(stderr, "That's one place I will search for the geometry file.\n");
E 5
I 5
    fprintf(stderr, "That's one place I will search for clas.geometry.\n");
E 5
    Hv_AddPath(tstr);
  }

  tstr = (char *)getenv("HOME");
  if (tstr != NULL) {
      fprintf(stderr, "Environment variable HOME: %s\n", tstr);
      fprintf(stderr, "Additional directories searched for the geometry and help files:\n");

      Hv_InitCharStr(&ts2, tstr);

      dst = (char *)Hv_Malloc(strlen(ts2) + 40);

      sprintf(dst, "%s/ced2.0", ts2);
      Hv_AddPath(dst);
      fprintf(stderr, "\t%s\n", dst);

      sprintf(dst, "%s/ced2.0/help", ts2);
      Hv_AddPath(dst);
      fprintf(stderr, "\t%s\n", dst);

      sprintf(dst, "%s/ced2.0/data", ts2);
      Hv_AddPath(dst);
      fprintf(stderr, "\t%s\n", dst);

      Hv_Free(ts2);
      Hv_Free(dst);
  }

/* set specialized user functions for various actions */

  Hv_CustomizeQuickZoom = CustomizeQuickZoom;   /* to modify labels */
  Hv_QuickZoom = DoQuickZoom;                   /* to handle selection */

}


/*------- AppendLogFile --------*/

static void AppendLogFile(void)

/*------ keep a log file of who is using ced ------*/

{
  FILE      *log;
  time_t    temp_tp;
  char      *logname = NULL;

  Hv_InitCharStr(&logname, "/home/heddle/ced.log");

  if (logname == NULL)
    return;

  log = fopen(logname, "a");
  Hv_Free(logname);

  if (log == NULL)
    return;

/* keep myself out of the log file */

  if ((Hv_userName != NULL) && Hv_Contained(Hv_userName, "heddle")) {
    fclose(log);
    return;
  }
  
  
  if (Hv_userName == NULL)
    fprintf(log, "?? ");
  else {
    fprintf(log, "%s@%s v%-6.3f ", Hv_userName, Hv_hostName, cedversion);
  }

/* add time string */

  time(&temp_tp);
  fprintf(log, "  %s", ctime(&temp_tp));

  fclose(log);

}
E 1
