#include "Hv.h"
#include "Hv_xyplot.h"
#include "plot.h"

static void       WindowTile();
static void       CenterWrite();
static void       DrawTile();
static void       MakeMenus();
static void       NewPlotView();
static void       AppendLogFile();
static void       InitDemoFile();
static void       UpdateMessage();
static void       ShowUpdateMessage();
static void       WriteInitFile();

float             version = 1.0017;

extern char **Hv_demoFile;

/*----- init -------*/

void init(void)

{
  Hv_View     view;

  InitDemoFile();
  Hv_AddPath("/home/heddle/Hv/demos/hvplot/help");
  AppendLogFile();
  MakeMenus();
  WindowTile();
  NewPlotView();
  UpdateMessage();

  Hv_simpleTextFg = Hv_black;
  Hv_simpleTextBg = -1;
  Hv_simpleTextBorder = -1;

}


/*-------- UpdateMessage ------*/

static void UpdateMessage()

/********************************************
  Looks for the file .hvplotinit in the user's
  home directory and finds the last version
  of hvplot run by this user. If it is earlier
  than the present version, give a message 
  telling the user that he should read the
  README.
*********************************************/

{
  char      *home;
  float     lastversion = -1.0;
  Boolean   update = False;
  char      *fname;
  FILE      *fp;
  char      *versrecname, *line;

/* use the HOME env variable to get the home directory.
   If can't get it, just exit quietly and ignore the
   whole UpdateMessage process */

  home = (char *)getenv("HOME");
  if (home == NULL)
    return;

/* construct the file name */

  fname = (char *)Hv_Malloc(strlen(home) + 15);
  strcpy(fname, home);
  strcat(fname, "/.hvplotinit");

/* assume that if the file can't be open that
   it doesn't exist -- perhaps this is the first
   time this user has run hvplot */

  fp = fopen(fname, "r");

  if (fp == NULL)   /* file does not exist */
    update = True;
  else  if ((fp = fopen(fname, "r")) != NULL) {
    line = (char *)Hv_Malloc(256);
    Hv_InitCharStr(&versrecname, "[VERSIONREC]");
    if (Hv_FindRecord(fp, versrecname, False, line))
      if (Hv_GetNextNonComment(fp,  line))
	sscanf(line, "%f", &lastversion);

    update = (version > lastversion);
    Hv_Free(versrecname);
    Hv_Free(line);
  }

/* update message? */

  if (update) {
    ShowUpdateMessage(lastversion);
    WriteInitFile(fname);
  }

  Hv_Free(fname);
}


/*------ ShowUpdateMessage ------*/

static void ShowUpdateMessage(lastversion)

float   lastversion;

{
  Hv_Widget    dialog = NULL;
  Hv_Widget    rowcol, dummy;
  char        *userstr, *lvstr;

  Hv_VaCreateDialog(&dialog, Hv_TITLE, "Update", NULL);
  rowcol = Hv_DialogColumn(dialog, 10);

/* add a label for the users name */

  userstr = (char *)Hv_Malloc(strlen(Hv_userName) + 20);
  strcpy(userstr, "user: ");
  strcat(userstr, Hv_userName);
  dummy = Hv_StandardLabel(rowcol, userstr, 4);
  Hv_Free(userstr);

/* add a label for the last version used */

  lvstr = (char *)Hv_Malloc(50);
  if (lastversion < 0.0)
    sprintf(lvstr, "last version used: (new user)");
  else
    sprintf(lvstr, "last version used: %-8.4f", lastversion);
  dummy = Hv_StandardLabel(rowcol, lvstr, 4);
  Hv_Free(lvstr);

  dummy = Hv_SimpleDialogSeparator(rowcol);

  dummy = Hv_StandardLabel(rowcol, "hvplot has been updated since the", 0);
  dummy = Hv_StandardLabel(rowcol, "last time you ran it.  We suggest", 0);
  dummy = Hv_StandardLabel(rowcol, "read the  \"README\" topic from the", 0);
  dummy = Hv_StandardLabel(rowcol, "on-line Help menu.               ", 0);


  dummy = Hv_SimpleDialogSeparator(rowcol);
  dummy = Hv_StandardDoneButton(rowcol, " OK ");

  Hv_DoDialog(dialog, NULL);
}

/*------ WriteInitFile --------*/

static void WriteInitFile(fname)

char   *fname;

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
    fprintf(fp, "  %-8.4f\n", version);
    fprintf(fp, "[ENDREC]\n");
  }

  Hv_Free(command);
}

/*-------- WindowTile -----------*/

static void WindowTile()

{
  Hv_tileX = 300;
  Hv_tileY = 80;
  Hv_TileDraw = DrawTile;
}


/*------ DrawTile --------*/

static void DrawTile()

{
  short      xc, y;
  Hv_Rect    rect;
  Hv_Region  region;
  char       versstr[40];

  Hv_SetRect(&rect, 0, 0, (short)Hv_tileX, (short)Hv_tileY);
  region = Hv_RectRegion(&rect);
  Hv_SetClippingRegion(region);

  Hv_FillRect(&rect, Hv_canvasColor);

  xc = ((short)Hv_tileX)/2;
  y = 2;

  CenterWrite("CHRISTOPHER NEWPORT UNIVERSITY", xc, &y, Hv_helvetica14, Hv_blue);
  CenterWrite("Jefferson Laboratory", xc, &y, Hv_helvetica14, Hv_blue);

  sprintf(versstr, "hvplot version %-7.4f", version);
  CenterWrite(versstr, xc, &y, Hv_helvetica11, Hv_aquaMarine);

  
  Hv_DestroyRegion(region);
}


/*------- CenterWrite ---------*/

static void CenterWrite(str, xc, y, font, color)

char           *str;
short          xc;
short          *y;
short          font;
short          color;

{
  Hv_String      *hvstr;
  short          w, h;

  hvstr = Hv_CreateString(str);

  hvstr->font = font;
  hvstr->strcol = color; 

  Hv_CompoundStringDimension(hvstr, &w, &h);
  Hv_DrawCompoundString(xc - w/2, *y, hvstr);

  *y += h;
  Hv_DestroyString(hvstr);
}

/*---- MakeMenus -----*/

static void MakeMenus()

{
  Hv_Widget   item;

/* add a "new view" item to the action menu */


  item = Hv_VaCreateMenuItem(Hv_actionMenu,
			     Hv_LABEL,            "New Plot View",  
			     Hv_CALLBACK,         NewPlotView,
			     NULL);


}


/*----- NewPlotView -----*/

static void NewPlotView()

{
  Hv_View   View;

  View = Hv_NewXYPlotView(NULL, False);

/* modify the plot view to add on-screen controls */

  /*  ModifyPlotView(View); */
  Hv_OpenView(View);

}


/*------- AppendLogFile --------*/

static void AppendLogFile()

/*------ keep a log file of who is using hvplot ------*/

{
  FILE      *log;
  time_t    temp_tp;
  char      *logname = NULL;

  Hv_InitCharStr(&logname, "/home/heddle/hvplot.log");

  if (logname == NULL)
    return;

  log = fopen(logname, "a");
  Hv_Free(logname);

  if (log == NULL)
    return;

/* keep myself out of the log file */

  if ((Hv_userName != NULL) && Hv_Contained(Hv_userName, "dheddle")) {
    fclose(log);
    return;
  }
  
  
  if (Hv_userName == NULL)
    fprintf(log, "?? ");
  else {
    fprintf(log, "%s ", Hv_userName);
  }

/* add time string */

  time(&temp_tp);
  fprintf(log, "  %s", ctime(&temp_tp));

  fclose(log);

}


/*------ InitDemoFile --------*/

static void InitDemoFile()

{
  static  char *defaultfile[] = {
    "[PLOTREC]",
    "style left 0.75 in",
    "style curve 1 name LAUNCH ",
    "style curve 2 name UNDER CLD",
    "style curve 3 name OVER CLD",
    "style curve 4 name ENGAGE",
    "style curve 5 name LASE",
    "style curve 6 name BO",
    "style plottype hstack",
    "style legend on",
    "style title      \\c1 ABL\\n21 Missle Raid\\nLauches 0 to 29 Seconds (U)",
    "style xlabel     Time (sec)",
    "style ylabel     MISSLE NUMBER",
    "style xprecision  0",
    "style yprecision  0",
    "style curve 1 linecolor lightblue",
    "style curve 2 linecolor orange",
    "style curve 3 linecolor green",
    "style curve 4 linecolor blue",
    "style curve 5 linecolor red",
    "style curve 6 linecolor yellow",
    "style curve 2 pattern 50%",
    "style curve 3 pattern vstripe",
    "style curve 4 pattern hatch",
    "style curve 5 pattern diagonal2",
    "style curve 6 pattern weave",
    "style attachlegend on",
    "style ynozoom on",
    "style width 500",
    "style height 450",
    "style majxgrid on",
    "style majxtick 11",
    "style order c y y y y y y",
    "2      21     35     12     20     8     4",
    "8      15     22     14     22     7     9",
    "13      16     31     16     21     6     6",
    "18      10     27     18     14     5     10",
    "21       8     19     19     20     6     2",
    "19       3     39     17     15     7     7",
    "12      22     27     15     17     8     0",
    "14      15     13     13     12     7     7",
    "NULL  0 0 0 0 0 0",
    "1       3     18     11     19     7     7",
    "3      0     33     12     25     5     5",
    "4     12     12     14     27     9     7",
    "5     16     10     19     14     5     5",
    "6     12     19     17     17     3     3",
    "7     29     31     20     17     7     10",
    "9     13     37     10     12     4     8",
    "10     10     25      7     23     3     3",
    "11     18     22      0     22     8     8",
    "15     10     26     13     10     0     6",
    "16      6     19     14     15     4     4",
    "17     17     21     15     14     3     9",
    "20      2     22     19     12     8     4",
    "[ENDREC]",
    "[PLOTREC]",
    "style filename on", 
    "style legendtop .91",
    "style legendleft -.3",
    "style label .6 .9 large blue 0.7\\sGeV",
    "style title \\gg\\s\\dn --> \\gp\\d\\^-\\dp",
    "style ylabel E\\s(\\gq\\d)",
    "style yzeroline on",
    "style left 1.0 in",
    "style top 7.0 in",
    "style width 4.0 in",
    "style height 3.0 in",
    "style xmin -1.0",
    "style xmax 1.0",
    "style ymin -1.0",
    "style ymax 1.0",
    "style majxtick 3",
    "style minxtick 4",
    "style majytick 3",
    "style minytick 4",
    "style curve 1 name VPI",
    "style errorbartop on",
    "style legendfont medium",
    "style axesfont small",
    "style order x y v",
    "style legend on",
    "style legendbox off",
    "style curve 1 algorithm spline",
    "-.95 .90 .05",
    "-.85 .70 .04",
    "-.75 .47 .03",
    "-.65 .24 .03",
    "-.55 .02 .03",
    "-.45 -.17 .03",
    "-.35 -.34 .03",
    "-.25 -.47 .03",
    "-.15 -.58 .03",
    "-.05 -.65 .03",
    ".05 -.69 .03",
    ".15 -.71 .02",
    ".25 -.70 .02",
    ".35 -.67 .02",
    ".45 -.61 .02",
    ".55 -.52 .02",
    ".65 -.40 .02",
    ".75 -.21 .02",
    ".85 .08 .02",
    ".95 .66 .02",
    "[ENDREC]",
    "[PLOTREC]",
    "style continue",
    "style order x y v",
    "style curve 1 onlegend off",
    "style curve 1 algorithm nolines",
    "-.95 0 .05",
    "-.85 0 .04",
    "-.75 0 .02",
    "-.65 0 .02",
    "-.55 0 .01",
    "-.45 0 -.01",
    "-.35 0 -.02",
    "-.25 0 -.02",
    "-.15 0 -.03",
    "-.05 0 -.03",
    ".05 0 -.04",
    ".15 0 -.04",
    ".25 0 -.04",
    ".35 0 -.04",
    ".45 0 -.03",
    ".55 0 -.03",
    ".65 0 -.02",
    ".75 0 -.01",
    ".85 0 .01",
    ".95 0 .04",
    "[ENDREC]",
    "[PLOTREC]",
    "style continue",
    "style order x y",
    "style curve 1 linestyle dash",
    "style curve 1 linecolor red",
    "style curve 1 name Berends",
    "style curve 1 algorithm spline",
    "-1.0 1",
    "-.9 .55",
    "-.8 .19",
    "-.7 -.07",
    "-.6 -.26",
    "-.5 -.39",
    "-.4 -.49",
    "-.3 -.58",
    "-.2 -.65",
    "-.1 -.71",
    ".0 -.76",
    ".1 -.81",
    ".2 -.84",
    ".3 -.85",
    ".4 -.86",
    ".5 -.84",
    ".6 -.81",
    ".7 -.73",
    ".8 -.58",
    ".9 -.16",
    "1.0 1",
    "[ENDREC]",
    "[PLOTREC]",
    "style continue",
    "style order x y",
    "style curve 1 linestyle dotdash",
    "style curve 1 linecolor brown",
    "style curve 1 name Moorehouse",
    "style curve 1 algorithm spline",
    "-.95 .83",
    "-.85 .51",
    "-.75 .23",
    "-.65 -.01",
    "-.55 -.23",
    "-.45 -.42",
    "-.35 -.59",
    "-.25 -.72",
    "-.15 -.83",
    "-.05 -.90",
    ".05 -.94",
    ".15 -.94",
    ".25 -.91",
    ".35 -.85",
    ".45 -.75",
    ".55 -.61",
    ".65 -.44",
    ".75 -.22",
    ".85 .10",
    ".95 .67",
    "[ENDREC]",
    "[PLOTREC]",
    "style legendtop .91",
    "style legendleft -.3",
    "style label .6 .9 large blue 1.0\\sGeV",
    "style xlabel cos\\s(\\gq\\d)",
    "style ylabel E\\s(\\gq\\d)",
    "style yzeroline on",
    "style left 1.0 in",
    "style top 11.0 in",
    "style width 4.0 in",
    "style height 3.0 in",
    "style xmin -1.0",
    "style xmax 1.0",
    "style ymin -1.0",
    "style ymax 1.0",
    "style majxtick 3",
    "style minxtick 4",
    "style majytick 3",
    "style minytick 4",
    "style curve 1 name VPI",
    "style errorbartop on",
    "style legendfont medium",
    "style axesfont small",
    "style order x y v",
    "style legend on",
    "style legendbox off",
    "style curve 1 algorithm spline",
    "-.95 .97 .06 ",
    "-.85 .81 .04",
    "-.75 .39 .05",
    "-.65 -.29 .04",
    "-.55 -.81 .04",
    "-.45 -.84 .04",
    "-.35 -.61 .04",
    "-.25 -.32 .04",
    "-.15 -.06 .04",
    "-.05 .12 .04",
    ".05 .13 .04",
    ".15 -.08 .04",
    ".25 -.41 .04",
    ".35 -.65 .04",
    ".45 -.77 .03",
    ".55 -.81 .03",
    ".65 -.81 .03",
    ".75 -.79 .03",
    ".85 -.75 .03",
    ".95 -.44 .03",
    "[ENDREC]",
    "[PLOTREC]",
    "style continue",
    "style order x y v",
    "style curve 1 onlegend off",
    "style curve 1 algorithm nolines",
    " -.95 0 .05",
    "-.85 0 .04",
    "-.75 0 .02",
    "-.65 0 -.01",
    "-.55 0 -.04",
    "-.45 0 -.05",
    "-.35 0 -.03",
    "-.25 0 -.02",
    "-.15 0 .01",
    "-.05 0 .01",
    ".05 0 .01",
    ".15 0 -.01",
    ".25 0 -.03",
    ".35 0 -.04",
    ".45 0 -.05",
    ".55 0 -.05",
    ".65 0 -.05",
    ".75 0 -.05",
    ".85 0 -.05",
    ".95 0 -.03",
    "[ENDREC]",
    "[PLOTREC]",
    "style continue",
    "style order x y",
    "style curve 1 linestyle dotdash",
    "style curve 1 linecolor brown",
    "style curve 1 name Moorehouse",
    "style curve 1 algorithm spline",
    " -.95 .89",
    "-.85 .63",
    "-.75 .29",
    "-.65 -.04",
    "-.55 -.30",
    "-.45 -.47",
    "-.35 -.57",
    "-.25 -.60",
    "-.15 -.61",
    "-.05 -.63",
    ".05 -.70",
    ".15 -.78",
    ".25 -.84",
    ".35 -.87",
    ".45 -.89",
    ".55 -.89",
    ".65 -.87",
    ".75 -.85",
    ".85 -.78",
    ".95 -.33",
    "[ENDREC]",
    NULL
  };

  int   numlines, i;
  char  *temp;

/* first count the number of lines */

  for (numlines = 0; defaultfile[numlines] != NULL; numlines++)
    ;

/* malloc the array */

  Hv_demoFile = (char **)Hv_Malloc((numlines+1)*sizeof(char *));

  for (i = 0; i < numlines; i++)
    Hv_InitCharStr(&(Hv_demoFile[i]), defaultfile[i]);

  Hv_demoFile[numlines] = NULL;

}
