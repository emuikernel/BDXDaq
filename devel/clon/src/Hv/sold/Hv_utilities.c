/*
  -----------------------------------------------------------------------
  File:     Hv_utilities.c
  ==================================================================================
  
  The Hv library was developed at CEBAF under contract to the
  Department of Energy and is the property of the U.S. Government.
  
  It may be used to develop commercial software, but the Hv
  source code and/or compiled modules/libraries may not be sold.
  
  Questions or comments should be directed to heddle@cebaf.gov
  ==================================================================================
  */

/* This file contains various utility functions of general interest */

#include "Hv.h"	  /* contains all necessary include files */
#include <ctype.h>


/*------  local prototypes ----------*/

static char *Hv_ConcatArgs(int	n,
			   char	*argv[]);

static int  Hv_MaskFromPosition(int  pos);

static void reverse (char  s[]);


/*************************************
  function and variable associated with
  generic pause routine:
  **************************************/

static void Hv_PauseTimeOut(XtPointer    xtptr,
			    XtIntervalId  *id);

static Boolean  Hv_pauseFlag;

extern Window  Hv_trueMainWindow;  /* unchanging (even while off screen draws) */

/*-------- Hv_SetUnits ------*/

void Hv_SetUnits(int units) {
  Hv_units = units;
}


/*------- Hv_GetCurrentPointerLocation ------*/

void  Hv_GetCurrentPointerLocation(short *x,
				   short *y)
    
{
    Boolean      samescreen;
    Window       root, child;
    
    int          rootx, rooty, wx, wy;
    unsigned int keys;
    
    samescreen = XQueryPointer(Hv_display,
			       Hv_mainWindow,
			       &root, &child,
			       &rootx, &rooty,
			       &wx, &wy,
			       &keys);
    
    
    if (samescreen) {
	*x = (short)wx;
	*y = (short)wy;
    }
    else {
	*x = -32766;
	*y = -32766;
    }
}



/*------- Hv_SymbolFromText ----*/

short   Hv_SymbolFromText(char *line)
    
{
    if (Hv_Contained(line, "square")) 
	return Hv_RECTSYMBOL;
    else if (Hv_Contained(line, "diamond"))
	return Hv_DIAMONDSYMBOL;
    else if (Hv_Contained(line, "blocki"))
	return Hv_BLOCKISYMBOL;
    else if (Hv_Contained(line, "blockh"))
	return Hv_BLOCKHSYMBOL;
    else if (Hv_Contained(line, "circle"))
	return Hv_CIRCLESYMBOL;
    else if (Hv_Contained(line, "cross"))
	return Hv_CROSSSYMBOL;
    else if (Hv_Contained(line, "uptriang"))
	return Hv_TRIANGLEUPSYMBOL;
    else if (Hv_Contained(line, "downtriang"))
	return Hv_TRIANGLEDOWNSYMBOL;
    else if (Hv_Contained(line, "bigpoint"))
	return Hv_BIGPOINTSYMBOL;
    else if (Hv_Contained(line, "point"))
	return Hv_POINTSYMBOL;
    else if (Hv_Contained(line, "defocus"))
	return Hv_DEFOCUSSYMBOL;
    else if (Hv_Contained(line, "focus"))
	return Hv_FOCUSSYMBOL;
    else if (Hv_Contained(line, "hex"))
	return Hv_HEXAGONSYMBOL;
    else if (Hv_Contained(line, "uparrow"))
	return Hv_UPARROWSYMBOL;
    else if (Hv_Contained(line, "downarrow"))
	return Hv_DOWNARROWSYMBOL;
    else if (Hv_Contained(line, "leftarrow"))
	return Hv_LEFTARROWSYMBOL;
    else if (Hv_Contained(line, "rightarrow"))
	return Hv_RIGHTARROWSYMBOL;
    else
	return Hv_NOSYMBOL;
}

/*------- Hv_ColorFromText ----*/

short   Hv_ColorFromText(char *line)
    
{
    int offset;
    
    if (Hv_Contained(line, "black")) 
	return Hv_black;
    else if (Hv_Contained(line, "blue"))
	return Hv_blue;
    else if (Hv_Contained(line, "nocolor"))
	return -1;
    else if (Hv_Contained(line, "no color"))
	return -1;
    else if (Hv_Contained(line, "none"))
	return -1;
    else if (Hv_Contained(line, "red"))
	return Hv_red;
    else if (Hv_Contained(line, "green"))
	return Hv_forestGreen;
    else if (Hv_Contained(line, "orange"))
	return Hv_orange;
    else if (Hv_Contained(line, "yellow"))
	return Hv_yellow;
    else if (Hv_Contained(line, "purple"))
	return Hv_purple;
    else if (Hv_Contained(line, "gray"))
	return Hv_gray10;
    else if (Hv_Contained(line, "white"))
	return Hv_white;
    else if (Hv_Contained(line, "lightblue"))
	return Hv_powderBlue;
    else if (Hv_Contained(line, "brown"))
	return Hv_brown;
    else if (Hv_Contained(line, "forestGreen"))
	return Hv_forestGreen;
    else if (Hv_Contained(line, "darkBlue"))
	return Hv_darkBlue;
    else if (Hv_Contained(line, "gray13"))
	return Hv_gray13;
    else if (Hv_Contained(line, "gray6"))
	return Hv_gray6;
    else if (Hv_Contained(line, "gray4"))
	return Hv_gray4;
    else if (Hv_Contained(line, "olive"))
	return Hv_olive;
    else if (Hv_Contained(line, "slateBlue"))
	return Hv_slateBlue;
    else if (Hv_Contained(line, "aquaMarine"))
	return Hv_aquaMarine;
    else if (Hv_Contained(line, "peru"))
	return Hv_peru;
    else if (Hv_Contained(line, "violet"))
	return Hv_violet;
    else if (Hv_Contained(line, "brown"))
	return Hv_brown;
    else if (Hv_Contained(line, "darkSeaGreen"))
	return Hv_darkSeaGreen;
    else if (Hv_Contained(line, "mistyRose"))
	return Hv_mistyRose;
    else if (Hv_Contained(line, "peachPuff"))
	return Hv_peachPuff;
    else if (Hv_Contained(line, "lavender"))
	return Hv_lavender;
    else if (Hv_Contained(line, "powderBlue"))
	return Hv_powderBlue;
    else if (Hv_Contained(line, "aliceBlue"))
	return Hv_aliceBlue;
    else if (Hv_Contained(line, "rosyBrown"))
	return Hv_rosyBrown;
    else if (Hv_Contained(line, "darkGreen"))
	return Hv_darkGreen;
    else if (Hv_Contained(line, "honeydew"))
	return Hv_honeydew;
    else if (Hv_Contained(line, "coral"))
	return Hv_coral;
    else if (Hv_Contained(line, "maroon"))
	return Hv_maroon;
    else if (Hv_Contained(line, "skyBlue"))
	return Hv_skyBlue;
    else if (Hv_Contained(line, "steelBlue"))
	return Hv_steelBlue;
    else if (Hv_Contained(line, "tan"))
	return Hv_tan;
    else if (Hv_Contained(line, "wheat"))
	return Hv_wheat;
    else if (Hv_Contained(line, "lightSeaGreen"))
	return Hv_lightSeaGreen;
    else if (Hv_Contained(line, "tan3"))
	return Hv_tan3;
    else if (Hv_Contained(line, "chocolate"))
	return Hv_chocolate;
    else if (Hv_Contained(line, "mediumForestGreen"))
	return Hv_mediumForestGreen;
    else if (Hv_Contained(line, "seaGreen"))
	return Hv_seaGreen;
    else if (Hv_Contained(line, "lawnGreen"))
	return Hv_lawnGreen;
    else if (Hv_Contained(line, "navyBlue"))
	return Hv_navyBlue;
    else if (Hv_Contained(line, "cadetBlue"))
	return Hv_cadetBlue;
    else if (Hv_Contained(line, "royalBlue"))
	return Hv_royalBlue;
    else if (Hv_Contained(line, "dodgerBlue"))
	return Hv_dodgerBlue;
    else if (Hv_Contained(line, "cornFlowerBlue"))
	return Hv_cornFlowerBlue;
    
/* if nothing, assume that it is an offset from Hv_red */
    
    else {
	sscanf(line, "%d", &offset);
	if ((offset < 0) || (offset >= Hv_numColors))
	    return Hv_black;
	else
	    return Hv_red + ((short)offset);
    }
}


/*------- Hv_PatternFromText ----*/

short   Hv_PatternFromText(char *line)
    
{
    if (Hv_Contained(line, "solid")) 
	return Hv_FILLSOLIDPAT;
    else if (Hv_Contained(line, "80"))
	return Hv_80PERCENTPAT;
    else if (Hv_Contained(line, "50"))
	return Hv_50PERCENTPAT;
    
    else if (Hv_Contained(line, "20"))
	return Hv_20PERCENTPAT;
    else if (Hv_Contained(line, "hatch"))
	return Hv_HATCHPAT;
    else if (Hv_Contained(line, "diagonal1"))
	return Hv_DIAGONAL1PAT;
    
    else if (Hv_Contained(line, "diagonal"))
	return Hv_DIAGONAL2PAT;
    else if (Hv_Contained(line, "hstripe"))
	return Hv_HSTRIPEPAT;
    else if (Hv_Contained(line, "stripe"))
	return Hv_VSTRIPEPAT;
    else if (Hv_Contained(line, "scale"))
	return Hv_SCALEPAT;
    else if (Hv_Contained(line, "hollow"))
	return Hv_HOLLOWPAT;
    else if (Hv_Contained(line, "none"))
	return Hv_HOLLOWPAT;
    else if (Hv_Contained(line, "star"))
	return Hv_STARPAT;
    else
	return Hv_FILLSOLIDPAT;
}



/*------ Hv_Flush -----*/

void  Hv_Flush(void)
    
{
    XFlush(Hv_display);
}

/*--------- Hv_SetCursor --------*/

void  Hv_SetCursor(Cursor curs)
    
/****************************************
  Note: the extra care taken below with
  Hv_trueMainWindo is beacuse the "usual"
  Hv_mainWindow may have been temporarily
  reset due to offscreen drawing.
  ******************************************/
    
{
    XDefineCursor(Hv_display, Hv_trueMainWindow, curs);
}

/*-------- Hv_DestroyWidget ------*/

void  Hv_DestroyWidget(Widget w)
    
{
    if (w == NULL)
	return;
    
    XtDestroyWidget(w);
}


/*--------- Hv_FileExists -------*/

Boolean   Hv_FileExists(char *fname)
    
/* simpler (and more intuitive) version of Hv_CheckFileExists
   which simply checks whether the file exists by checking
   to see if it can be opened for reading */
    
    
{
    FILE     *fp;
    
    fp = fopen(fname, "r");
    
    if (fp == NULL)
	return False;     /*file does not exist */
    
    fclose(fp);
    return True;
}

/*--------- Hv_CheckFileExists -------*/

Boolean   Hv_CheckFileExists(char *fname)
    
/* returns true if file does NOT exist or user says OK to overwrite.
   i.e., a true indicated you shoul proceed. */
    
{
    FILE     *fp;
    char     *message;
    Boolean  writeit;
    
    fp = fopen(fname, "r");
    
    if (fp == NULL)
	return True;     /*file does not exist */
    
    message = (char *)Hv_Malloc(strlen(fname) + 40);
    strcpy(message, fname);
    strcat(message, " already exists. Overwrite?");
    writeit = Hv_Question(message);
    Hv_Free(message);
    fclose(fp);
    return writeit;
}

/*-------- Hv_NewVector -------*/

float   *Hv_NewVector(int n)
    
{
    return (float *)Hv_Malloc(n*sizeof(float));
}

/*-------- Hv_NewIVector -------*/

int   *Hv_NewIVector(int n)
    
{
    return (int *)Hv_Malloc(n*sizeof(int));
}


/*-------- Hv_NewPoints -------*/

Hv_Point  *Hv_NewPoints(int n)
    
{
    return (Hv_Point *)Hv_Malloc(n*sizeof(Hv_Point));
}


/*-------- Hv_NewFPoints -------*/

Hv_FPoint  *Hv_NewFPoints(int n)
    
{
    return (Hv_FPoint *)Hv_Malloc(n*sizeof(Hv_FPoint));
}


/*-------- Hv_NewRect --------*/

Hv_Rect  *Hv_NewRect(void)
    
{
    Hv_Rect  *ptr;
    
    ptr = (Hv_Rect *)Hv_Malloc(sizeof(Hv_Rect));
    Hv_SetRect(ptr, 0, 0, 0, 0);
    
    return ptr;
}

/*-------- Hv_NewFRect --------*/

Hv_FRect  *Hv_NewFRect(void)
    
{
    Hv_FRect  *ptr;
    
    ptr = (Hv_FRect *)Hv_Malloc(sizeof(Hv_FRect));
    Hv_SetFRect(ptr, 0.0, 0.0, 0.0, 0.0);
    
    return ptr;
}


/*--------- Hv_InitCharStr --------*/

void Hv_InitCharStr(char  **dst,
		    char  *sst)
    
{
    if (sst == NULL)
	*dst = NULL;
    else {
	*dst = Hv_Malloc(strlen(sst) + 1);
	strcpy(*dst, sst);
    }
}

/* ----- Hv_PointerReport --------*/

void Hv_PointerReport(char *str)
    
{
    if (str != NULL)
	fprintf(stderr, "%s\n", str);
    fprintf(stderr, "Total number of active pointers = %d\n", Hv_activePointers);
}

/*------ Hv_DisableWidget -------*/

void  Hv_DisableWidget(Widget w)
    
{
    XtSetSensitive((Widget)w, False);
}

/*------ Hv_EnableWidget -------*/

void  Hv_EnableWidget(Widget w)
    
{
    XtSetSensitive((Widget)w, True);
}

/*---------- Hv_SetSensitivity ---------*/

void Hv_SetSensitivity(Hv_Widget  w,
		       Boolean   val)
{
    XtSetSensitive((Widget)w, val);
}


/*---------- Hv_SetWidgetBackgroundColor ------ */

void Hv_SetWidgetBackgroundColor(Hv_Widget   w,
				 short     color)
    
{
    unsigned long dbg;
    
    if (w == NULL)
	return;
    
    if (color >= 0)
	XtVaSetValues(w, XmNbackground, Hv_colors[color], NULL);
    else {
	XtVaGetValues(XtParent(w), XmNbackground, &dbg, NULL);
	XtVaSetValues(w, XmNbackground, dbg, NULL);
    }
    
}
/*---------- Hv_SetWidgetForegroundColor ------ */

void Hv_SetWidgetForegroundColor(Hv_Widget   w,
				 short     color)
    
{
    unsigned long dbg;
    
    if (w == NULL)
	return;
    
    if (color >= 0)
	XtVaSetValues(w, XmNforeground, Hv_colors[color], NULL);
    else {
	XtVaGetValues(XtParent(w), XmNforeground, &dbg, NULL);
	XtVaSetValues(w, XmNforeground, dbg, NULL);
    }
    
}

/*---------- Hv_UnixTimeToHvTime ------ */

char   *Hv_UnixTimeToHvTime(char *unixdate)
    
{
    char  *hvdate;
    
    if (unixdate == NULL)
	return NULL;
    
    hvdate = (char *)Hv_Malloc(23);
    strncpy(hvdate, unixdate, 11);          /*"Thu Nov 17 "*/
    strncpy(hvdate+11, unixdate+20, 4);     /*"Thu Nov 17 1994"*/
    strcpy(hvdate+15, "  ");                /*"Thu Nov 17 1994  "*/
    strncpy(hvdate+17, unixdate+11, 5);     /*"Thu Nov 17 1994  12:45"*/
    hvdate[22] = '\0';
    return hvdate;
}

/*---------- Hv_GetHvTime ------*/

char  *Hv_GetHvTime(void)
    
{
    time_t	temp_tp;
    char         *datestr;
    
    time(&temp_tp);
    datestr = (char *)ctime(&temp_tp);
    datestr[24] = '\0';
    return Hv_UnixTimeToHvTime(datestr);
}

/*--------- Hv_Pause ------*/

void  Hv_Pause(int    interval,
	       Hv_Region region)
    
/* interval: in milliseconds*/
    
{
    XtIntervalId         waitid;
    
    
/**********************************************
  New Algorithm 6/1/95
  
  1) Set Hv_pauseFlag to True
  2) Set A Timeout for given interval
  3) Event loop blocking all but timeouts
  Note: only timeout set in step 2 will reset
  Hv_pauseFlag to false.
  
  **********************************************/
    
    XFlush(Hv_display);
    
    Hv_pauseFlag = True;
    
    waitid = XtAppAddTimeOut(Hv_appContext, (unsigned long)(interval), 
			     (XtTimerCallbackProc)Hv_PauseTimeOut,  NULL); 
    
    while (Hv_pauseFlag)
	XtAppProcessEvent(Hv_appContext, XtIMTimer);
    
    Hv_SetClippingRegion(region);
}


/*------ Hv_Malloc ------*/

void  *Hv_Malloc(size_t size)
    
/* calls usual malloc and exits on failure.*/
    
{
    void     *ptr;
    
    if (size == 0)
	return NULL;
    
    ptr = (void *)malloc(size); 
    /*    ptr = (void *)calloc(size, 1);*/

    if (ptr == NULL) {
	fprintf(stderr, "Memory allocation failure in Hv_Malloc.\n");
	fprintf(stderr, "Size Requested: %d\nExiting\n", size);
	exit (-1);
    }
    
    Hv_activePointers++;
    return ptr;
}

/*------- Hv_Free ------*/

void  Hv_Free(void *ptr)
    
{
    if (ptr == NULL)
	return;
    
    Hv_activePointers--;
    free(ptr);
}


/*----------- Hv_CopyString ------------ */

void Hv_CopyString(Hv_String **dstr,
		   Hv_String *sstr)
    
/* copies Hv_String sstr onto destination dstr. dstr
   is created here -- do NOT malloc in call
   
   sstr: malloced by calling prog */
    
{
    if (sstr == NULL) {
	*dstr = NULL;
	return;
    }
    
    (*dstr) = Hv_CreateString(sstr->text);
    (*dstr)->font = sstr->font;
    (*dstr)->strcol = sstr->strcol;
    (*dstr)->fillcol = sstr->fillcol;
    (*dstr)->horizontal = sstr->horizontal;
    (*dstr)->offset = sstr->offset;
}


/*------ Hv_CreateString -----*/

Hv_String   *Hv_CreateString(char *text)
    
{
    Hv_String    *str;
    
    str = (Hv_String *)Hv_Malloc(sizeof(Hv_String));
    
    if (str == NULL) {
	fprintf(stderr, "Memory allocation failure in Hv_CreateString. Exiting.\n");
	exit (-1);
    }
    
    Hv_activePointers++;
    
    str->font =       Hv_fixed2;   /* default font */
    str->strcol =     Hv_black;    /* default color */
    str->fillcol =    -1;          /* default no fill */
    str->horizontal = True;        /* default orientation */
    str->offset = 0;
    
    if (text != NULL)
	Hv_InitCharStr(&(str->text), text);
    else
	str->text = NULL;
    
    return str;
}

/*------- Hv_DestroyString -----*/

void Hv_DestroyString(Hv_String *str)
    
{
    if (str == NULL)
	return;
    
    if (str->text != NULL)
	Hv_Free(str->text);
    
    Hv_activePointers--;
    
    Hv_Free(str);
}

/*------ Hv_Contained ----*/

Boolean Hv_Contained(char *str,
		     char *substr)
    
/* case insensitive substr search */
    
{
    char      *STR, *SUBSTR;
    Boolean   contained;
    
    if (str == NULL)
	return False;
    
    Hv_InitCharStr(&STR, str);
    Hv_InitCharStr(&SUBSTR, substr);
    
    Hv_UpperCase(STR);
    Hv_UpperCase(SUBSTR);
    
    contained = (strstr(STR, SUBSTR) != NULL);
    
    Hv_Free(STR);
    Hv_Free(SUBSTR);
    
    return contained;
}

/* ------- Hv_SysBeep -------- */

void  Hv_SysBeep(void)
    
/* A beeper for diagnostics */
    
    
{
    Display    *dpy = XtDisplay(Hv_toplevel);
    
    XBell(dpy, 50);
}

/* -------- Hv_itoa ------------- */

void Hv_itoa(int   n,
	     char   s[])
    
/* replica of usual itoa (int to ascii) function */
    
{
    int i, sign;
    
    if ((sign = n) < 0)
	n = -n;
    
    i = 0;
    
    do
    {
	s[i++] = n % 10 + '0';
    }
    while ((n /= 10) > 0);
    
    if (sign < 0)
	s[i++] = '-';
    
    s[i] = '\0';
    reverse (s);
}

/* ------- Hv_positoa ----------------*/

void Hv_positoa(int   n,
		char  s[],
		int   numblanks)
    
/* if n  > 0, just calls usual Hv_itoa. if n <= 0, fills
   s with numblanks spaces (' ') */
    
    
{
    int i;
    
    if (n > 0)
	Hv_itoa(n, s);
    else {
	for (i = 0; i < numblanks; i++)
	    s[i] = ' ';
	s[numblanks] = '\0';
    }
}


/* ------- Hv_nonnegitoa ----------------*/

void Hv_nonnegitoa(int   n,
		   char  s[],
		   int   numblanks)
    
/* if n  >= 0, just calls usual Hv_itoa. if n < 0, fills
   s with numblanks spaces (' ') */
    
    
{
    int i;
    
    if (n >= 0)
	Hv_itoa(n, s);
    else {
	for (i = 0; i < numblanks; i++)
	    s[i] = ' ';
	s[numblanks] = '\0';
    }
}

/* ------- Hv_RemoveFileExtension --------*/

void Hv_RemoveFileExtension(char *fname)
    
{
    int    lastslash = -1;
    int    lastdot = -1;
    int    i;
    
    if (fname == NULL)
	return;
    
/*  must worry about "."s appearing in the path
    e.g. /usr2/users/heddle/Hv3.2/Hv_io.c
    so trivial algorithm based on first "."
    is not good enough */
    
/* ALGORITHM:  if LAST "." occurs AFTER last "/",
   the replace it with a '\0', else do NOTHING */
    
    for (i = 0; i < strlen(fname); i++)
	if (fname[i] == '.')
	    lastdot = i;
	else if (fname[i] == '/')
	    lastslash = i;
    
    if (lastdot > lastslash)
	fname[lastdot] = '\0';
}



/* ------- Hv_StripLeadingDirectory --------*/

void Hv_StripLeadingDirectory(char **path)
    
{
    char  *pos;
    
    while((pos = strstr(*path, "/")) != NULL)
	*path = pos+1;
}

/*--------- Hv_CheckFont -------*/

void Hv_CheckFont(short font)
    
{
    if ((font < 0) || (font >= Hv_NUMFONTS)) {
	fprintf(stderr, "Hv error: font out of range [%d]. Exiting\n", font);
	exit(0);
    }
    
    if (Hv_fontLoaded[font])
	return;
    
    Hv_AddNewFont(font);
    
}


/*-------- Hv_CreateMotifString --------*/

XmString Hv_CreateMotifString(char  *str,
			      short  font)
    
{
    XmString   xmst;
    
    if (str == NULL) {
	fprintf(stderr, "Hv benign warning: tried to create a NULL Motif string.\n");
	return NULL;
    }
    
    if (font == Hv_DEFAULT)
	xmst = XmStringCreateSimple(str);
    else {
	Hv_CheckFont(font);
	xmst = XmStringCreate(str, Hv_charSet[font]);
    }
    
    return  xmst;
}

/* ---- Hv_CreateAndSizeMotifString ------- */

XmString  Hv_CreateAndSizeMotifString(char    *text,
				      short   font,
				      short   *w,
				      short   *h)
    
{
    XmString   xmst;
    
    xmst = Hv_CreateMotifString(text, font);
    
    if (xmst == NULL) {
	*w = 0;
	*h = 0;
	return NULL;
    }
    
    if (w)
	*w = (short)XmStringWidth (Hv_fontList, xmst);
    
    if (h)
	*h = (short)XmStringHeight(Hv_fontList, xmst);
    
    return  xmst;
}


/*-------- Hv_ParseString -------*/

int   Hv_ParseString(char *str)
    
{
    int   stop;
    
    if (str == NULL)
	return -1;
    
/* first the string length */
    
    stop = 0;
    while (1) {
	
/* see if we bump against a backslash */
	
	if (str[stop] == '\0')
	    return 0;
	
	if (str[stop] == '\\') {
	    
/* first check: next char is another backslash. If it
   is, the parsing will treat all chars up to and including
   the first '\' as a segement; the next segment will start
   after the second '\' */
	    
	    if (str[stop+1] == '\\')
		stop++;
	    str[stop] = '\0';
	    
	    return (int)str[stop+1];
	}
	stop++;
    }
}


/*------ Hv_CorrespondingFont -------*/

short Hv_CorrespondingFont(short font,
			   int  control)
    
{
    short   family, size, style;
    
    Hv_FontToFontParameters(font, &family, &size, &style);
    
    
    switch (control) {
    case Hv_NEXTBIGGERFONT:
	size = Hv_sMin(Hv_FONTSIZE25, size+1);
	break;
	
    case Hv_NEXTSMALLERFONT:
	size = Hv_sMax(Hv_FONTSIZE11, size-1);
	break;
	
    case Hv_GREEKFONT:
	family = Hv_SYMBOLFAMILY;
	break;
	
    case Hv_HELVETICAFONT: case Hv_ROMANFONT:
	family = Hv_HELVETICAFAMILY;
	break;
	
    case Hv_BOLDFONT:
	style = Hv_BOLDFACE;
	
    case Hv_PLAINFONT:
	style = Hv_PLAINFACE;
	break;
    }
    
    return Hv_FontParametersToFont(family, size, style);
}


/* ------- Hv_StringDimension ------------- */

void	Hv_StringDimension(Hv_String    *str,
			   short     *w,
			   short     *h)
    
/* calculate the pixel width and height of the string,
   given its specified text and font */
    
    
{
    XmString	xmst;     /* XmString */
    short 	xmw, xmh; /* motif width and height */
    
    if (str->text == NULL) {
	fprintf(stderr, "Hv benign warning: tried to size a NULL string.\n");
	*w = 0;
	*h = 0;
	return;
    }
    
    xmst = Hv_CreateAndSizeMotifString(str->text, str->font, &xmw, &xmh);
    
    *w = (short) xmw;  /* return the width with the proper typecast */
    *h = (short) xmh;  /* return the height with the proper typecast */
    XmStringFree(xmst);  /* free the motif string structure */
}

/*------- Hv_CharDimension ---------*/

void  Hv_CharDimension(char  *text,
		       short  font,
		       short  *w,
		       short  *h)
    
{
    Hv_String    *hvstr;
    
    if (text == NULL) {
	fprintf(stderr, "Hv benign warning: tried to char-dim a NULL string.\n");
	*w = 0;
	*h = 0;
	return;
    }
    
    
    hvstr = Hv_CreateString(text);
    hvstr->font = font;
    Hv_StringDimension(hvstr, w, h);
    Hv_DestroyString(hvstr);
}


/*---------- Hv_ReplaceCommas --------*/

void Hv_ReplaceCommas(char *line)
    
/* replace commas with blanks */
    
{
    int   i;
    
    if (line == NULL)
	return;
    
    for (i = 0; i < strlen(line); i++)
	if (line[i] == ',')
	    line[i] = ' ';
}


/*---------- Hv_ReplaceNewLine --------*/

void Hv_ReplaceNewLine(char *line)
    
/* replace newline with NULL */
    
{
    int   i;
    
    if (line == NULL)
	return;
    
    for (i = 0; i < strlen(line); i++)
	if (line[i] == '\n') {
	    line[i] = '\0';
	    return;
	}
}

/* ------- Hv_RemoveLeadingBlanks ------*/

void Hv_RemoveLeadingBlanks(char **s)
    
{
    if ((s == NULL) || (*s == NULL))
	return;
    
    while (isspace(**s) && (**s != '\0'))
	(*s)++;
}

/* ------- Hv_RemoveTrailingBlanks ------*/

void Hv_RemoveTrailingBlanks(char *s)
    
{
    char  *t;
    
    if ((s == NULL) || (strlen(s) == 0))
	return;
    
    t = s + strlen(s) - 1;
    
    while ((*t == ' ') && (t > s)) {
	*t = '\0';
	t--;
    }
    
}

/* -------- Hv_NumberToString ------------- */

void	Hv_NumberToString(char	*text,
			  float	v,
			  short	p,
			  float  eps)
    
/* converts a float to a string, writing with the specified
   precision. (usually p --> number of decimal pnts)
   
   char	*text  holds converted text -- up to caller to insure it's big enough
   float	v      real value to be converted to string
   short	p      precision -- i.e. places after decimal in mantissa
   float   eps    if abs < eps, then print as zero
   
   */
    
{
    
/*NOTE: p < 0 --> print as nearest integer
  p > 100, print in SciNot with prec p - 100*/
    
    float	   vabs;	  /* absolute val of number to be converted */
    float	   lowlim;	  /* below this we switch to scientific notation */
    float	   uplim;	  /* above this we switch to scientific notation */
    Boolean  forcedsci = False;
    
    if (text == NULL) {
	fprintf(stderr, "Hv warning: Called Hv_NumberToString with NULL text.\n");
	return;
    }
    
/* see if integer print is requested */
    
    if (p < 0) {
	sprintf(text, "%-d", Hv_nint(v));
	return;
    }
    
    if (p > 100) {
	forcedsci = True;
	p = p - 100;
    }
    
    p = Hv_sMax(0, Hv_sMin(6, p));
    
/* to see if the number is effectively zero */
    
    vabs = fabs(v);
    
/* if magnitude is less than eps, convert to exactly zero */
    
    if ((vabs < eps) || (vabs == 0.0))
	sprintf(text, "%-*.*f", p+7, p, 0.0);
    else {
	
/* these numbers are the limits before scientific notation is used*/
/* use scientific notation based on limits */  
	
	uplim = Hv_sciNotHiLimit;
	lowlim = Hv_sciNotLowLimit;
	
	if ((p > 0) && (forcedsci || (vabs < lowlim) || (vabs > uplim)))
	    sprintf(text, "%-*.*e", p+7, p, v);
	else
	    sprintf(text, "%-*.*f", p+7, p, v);
    }
    Hv_RemoveTrailingBlanks(text);
}



/* ----- Warning message for invalid command line options */

void Hv_InvalidOptions(unsigned int	argc,
		       char	*argv[])
    
{
    
/* print remaining options and denote as invalid */
    
    char		*s;		/* concatenated unknown options */
    char		*message;	/* complete message sent to warning dialog */
    char		*whead = "INVALID COMMAND LINE OPTIONS: ";	/* warning header */
    
    if ((s  = Hv_ConcatArgs(argc, argv)) != NULL) {
	if (Hv_trapCommandArgs) {
	    Hv_InitCharStr(&Hv_leftOverArgs, s);
	    Hv_argc = argc;
	    Hv_argv = argv;
	}
	else {
	    message = (char *) Hv_Malloc(strlen(whead) + strlen(s) + 1);
	    strcpy(message, whead);
	    strcat(message, s);
	    Hv_Warning(message);
	    Hv_Free(message);
	}
	Hv_Free(s);
    }
}

/*------- Hv_CountBits -------*/

int Hv_CountBits(int control)
    
{
    int cnt = 0;
    int i;
    
    for (i = 0; i < 8*sizeof(int); i++) {
	if (Hv_CheckBitPosition(control, 0))
	    cnt++;
	
	control = (control >> 1);
    }
    
    
    return cnt;
}

/* ------------ Hv_FirstSetBit -------------------------------- */

int  Hv_FirstSetBit(int control)
    
{
    int maxbit = 8*sizeof(int);
    int bit = 0;
    
    if (control == 0)
	return -1;
    
    while (((control % 2) == 0) && (bit < maxbit)) {
	bit++;
	control = control >> 1;
    }
    
    return bit;
}


/* ------------ Hv_SetBit ---------------- */

void Hv_SetBit(int *Control,
	       int BitInt)
    
{
    *Control = *Control | BitInt;
}

/* ------------ Hv_ClearBit -----------------*/

void Hv_ClearBit(int *Control,
		 int BitInt)
    
{
    *Control &= ~BitInt;
}


/* ------------ Hv_ToggleBit ----------------*/

void Hv_ToggleBit(int *Control,
		  int BitInt)
    
{
    *Control = *Control ^ BitInt;
}

/* ------------ Hv_CheckBit ------------------*/

Boolean Hv_CheckBit(int Control,
		    int BitInt)
    
/* assumes that option is one of possibly many options
   that might be set bitwise in control */
    
{
    return (((Control & BitInt) == BitInt));
}

/* ------------ Hv_SetBitPosition ---------------- */

void Hv_SetBitPosition(int *Control,
		       int  pos)
    
{
    int mask;
    mask = Hv_MaskFromPosition(pos);
    Hv_SetBit(Control, mask);
}

/* ------------ Hv_ClearBitPosition -----------------*/

void Hv_ClearBitPosition(int *Control,
			 int  pos)
    
{
    int mask;
    mask = Hv_MaskFromPosition(pos);
    Hv_ClearBit(Control, mask);
}


/* ------------ Hv_ToggleBitPosition ----------------*/

void Hv_ToggleBitPosition(int *Control,
			  int  pos)
    
{
    int mask;
    mask = Hv_MaskFromPosition(pos);
    Hv_ToggleBit(Control, mask);
}

/* ------------ Hv_CheckBitPosition ------------------*/

Boolean Hv_CheckBitPosition(int  Control,
			    int  pos)
    
/* assumes that option is one of possibly many options
   that might be set bitwise in control */
    
{
    int mask;
    mask = Hv_MaskFromPosition(pos);
    return Hv_CheckBit(Control, mask);
}


/*------- Hv_ClickType --------*/

int Hv_ClickType(Hv_Item Item,
		 short  button)
    
{
    int       click = 0;
    XEvent    testevent;
    
    Hv_Pause(XtGetMultiClickTime(Hv_display), NULL); 
    
    if (XCheckMaskEvent(Hv_display, ButtonPressMask, &testevent))
	if (testevent.xbutton.button == button) 
	    return 2;  /* double click */
    
    if ((Item != NULL) && (Item->domain == Hv_OUTSIDEHOTRECT))
	return 1;
    
    if (XCheckMaskEvent(Hv_display, ButtonReleaseMask, &testevent)) 
	if (testevent.xbutton.button == button)
	    return 1;  /* single click */
    
    return click;
}


/*--------- Hv_PauseTimeOut -------*/

static void Hv_PauseTimeOut(XtPointer    xtptr,
			    XtIntervalId  *id)
{
    Hv_pauseFlag = False;
}

/* -------- Hv_ConcatArgs ---------------- */

static char *Hv_ConcatArgs(int	n,
			   char	*argv[])
    
/* concatenate command line arguments into one string 
   cf. Douglas A Young "X Window Systems Programming ..." p. 27*/
    
{
    int	i;		/* loop index */
    int	len = 0;	/* string length */
    char	*buffer;	/* buffer for returned string */
    
/* If no args (other than program name) just return empty string */
    
    if (n <= 1)
	return("");
    
/* Get total length of string -- allow for a space between words */
    
    for (i = 1; i < n; i++)
	len += strlen(argv[i]);
    len += (n-1);		/* for space between words */
    
/* allocate memory for return string. Account for not enough memory */
    
    buffer = (char *)Hv_Malloc(len+1);		/* + 1 to allow for NULL */	
    
    if (buffer == NULL) {
	fprintf(stderr, "Out of memory in concat_args() \n");
	exit(1);
    }
    
    buffer[0] = '\0';
    
    for (i = 1; i < n; i++) {
	if (i > 1)
	    strcat(buffer," ");
	strcat(buffer,argv[i]);
    }
    
    return buffer;
}


/*------- Hv_MaskFromPosition ---- */


static int    Hv_MaskFromPosition(int pos)
    
{
    if ((pos < 0) || (pos > 31))
	return 0;
    
    return (1 << pos);
}


/* --------- reverse ----------- */

static void reverse (s)
    
    char   s[];
    
{
    /* from kernighan & ritchie's The C Programming Language */
    int c, i, j;
    
    for (i=0, j = strlen(s)-1; i<j; i++, j--) {
	c = s[i];
	s[i] = s[j];
	s[j] = c;
    }
}
