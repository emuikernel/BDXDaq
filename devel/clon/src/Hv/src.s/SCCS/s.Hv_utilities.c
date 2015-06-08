h43696
s 00082/00034/01436
d D 1.2 04/02/06 17:24:21 boiarino 3 1
c set memory check stuff
e
s 00000/00000/00000
d R 1.2 02/09/09 11:30:54 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/s/Hv_utilities.c
e
s 01470/00000/00000
d D 1.1 02/09/09 11:30:53 boiarino 1 0
c date and time created 02/09/09 11:30:53 by boiarino
e
u
U
f e 0
t
T
I 1
/**
 * <EM>Various utility functions of general interest.</EM>
 * <P>
 * The Hv library was developed at The Thomas Jefferson National
 * Accelerator Facility under contract to the
 * Department of Energy and is the property of they U.S. Government.
 * Partial support came from the National Science Foundation.
 * <P>
 * It may be used to develop commercial software, but the Hv
 * source code and/or compiled modules/libraries may not be sold.
 * <P>
 * Questions or comments should be directed
 * to <a href="mailto:heddle@cebaf.gov">heddle@cebaf.gov</a> <HR>
 */


D 3
 #include "Hv.h"
 #include <ctype.h>
E 3
I 3
#include "Hv.h"
#include <ctype.h>
E 3

I 3
/*
#define MEM_ALLOC_SCAN 1
*/
E 3

D 3
 /*------  local prototypes ----------*/
E 3
I 3
#ifdef MEM_ALLOC_SCAN
E 3

D 3
 
E 3
I 3
/* memory alloc statistic */
E 3

D 3
 static char *Hv_ConcatArgs(int	n,
			    char	*argv[]);
E 3
I 3
#define NMEMALLOCS 100000
int imemallocs;
static int nmemallocs = 0;
static int total_memallocs = 0;
static int memallocs_ptr[NMEMALLOCS];
static int memallocs_len[NMEMALLOCS];
E 3

D 3
 static int  Hv_MaskFromPosition(int  pos);
E 3
I 3
#endif
E 3

D 3
 static void reverse (char  s[]);
E 3
I 3
/*------  local prototypes ----------*/
E 3

I 3
static char     *Hv_ConcatArgs(int n, char *argv[]);
static int       Hv_MaskFromPosition(int pos);
static void      reverse(char s[]);
extern Hv_Window Hv_trueMainWindow;  /* unchanging (even while off screen draws) */
E 3

D 3
 extern Hv_Window  Hv_trueMainWindow;  /* unchanging (even while off screen draws) */
E 3
I 3
/*-------- Hv_SetUnits ------*/
E 3

D 3
 /*-------- Hv_SetUnits ------*/

E 3
 void Hv_SetUnits(int units) {
   Hv_units = units;
 }


/* 
 * Hv_SymbolFromText    Extracts an Hv Symbol from a line of text.
 * If no symbols is found, it returns Hv_NOSYMBOL.
 * @param    line      The line to parse
 * @return   the symbol it found, or Hv_NOSYMBOL if none
 */

 short   Hv_SymbolFromText(char *line) {

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

/**
 * Parses a line of text and returns the color it finds. Used so
 * that imput files can be constructed with color keywords.
 * @param  line    The line of text.
 * @return Color index corresponding to text, or Hv_black by default.
 */

 short   Hv_ColorFromText(char *line) {
   int offset;
   
   if (Hv_Contained(line, "black")) 
     return Hv_black;
   else if (Hv_Contained(line, "nocolor"))
     return -1;
   else if (Hv_Contained(line, "no color"))
     return -1;
   else if (Hv_Contained(line, "none"))
     return -1;
   else if (Hv_Contained(line, "red"))
     return Hv_red;
   else if (Hv_Contained(line, "orange"))
     return Hv_orange;
   else if (Hv_Contained(line, "yellow"))
     return Hv_yellow;
   else if (Hv_Contained(line, "purple"))
     return Hv_purple;
/*
 * many shades of gray
 */

   else if (Hv_Contained(line, "gray")) {
     if (Hv_Contained(line, "gray15"))
       return Hv_gray15;
     else if (Hv_Contained(line, "gray14"))
       return Hv_gray14;
     else if (Hv_Contained(line, "gray13"))
       return Hv_gray13;
     else if (Hv_Contained(line, "gray12"))
       return Hv_gray12;
     else if (Hv_Contained(line, "gray11"))
       return Hv_gray11;
     else if (Hv_Contained(line, "gray10"))
       return Hv_gray10;
     else if (Hv_Contained(line, "gray9"))
       return Hv_gray9;
     else if (Hv_Contained(line, "gray8"))
       return Hv_gray8;
     else if (Hv_Contained(line, "gray7"))
       return Hv_gray7;
     else if (Hv_Contained(line, "gray6"))
       return Hv_gray6;
     else if (Hv_Contained(line, "gray5"))
       return Hv_gray5;
     else if (Hv_Contained(line, "gray4"))
       return Hv_gray4;
     else if (Hv_Contained(line, "gray3"))
       return Hv_gray3;
     else if (Hv_Contained(line, "gray2"))
       return Hv_gray2;
     else if (Hv_Contained(line, "gray1"))
       return Hv_gray1;
     else
       return Hv_gray10;
   }


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
   else if (Hv_Contained(line, "blue"))
     return Hv_blue;
   else if (Hv_Contained(line, "green"))
     return Hv_forestGreen;
   
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

 short   Hv_PatternFromText(char *line) {
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

 /*------ Hv_DisableWidget -------*/

 void  Hv_DisableWidget(Hv_Widget w) {
   Hv_SetSensitivity(w, False);
 }

 /*------ Hv_EnableWidget -------*/

 void  Hv_EnableWidget(Hv_Widget w) {
   Hv_SetSensitivity(w, True);
 }


 /*--------- Hv_FileExists -------*/

Boolean   Hv_FileExists(char *fname) {

 /* simpler (and more intuitive) version of Hv_CheckFileExists
    which simply checks whether the file exists by checking
    to see if it can be opened for reading */


    FILE     *fp;

	if(fname == NULL)
		return False;
    fp = fopen(fname, "r");
    
    if (fp == NULL)
	return False;     /*file does not exist */
    
    fclose(fp);
    return True;
}

 /*--------- Hv_CheckFileExists -------*/

Boolean   Hv_CheckFileExists(char *fname) {
    
/* returns true if file does NOT exist or user says OK to overwrite.
   i.e., a true indicates you should proceed. */

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
	 Hv_Println("%s", str);
     Hv_Println("Total number of active pointers = %d", Hv_activePointers);
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


D 3
 /*------ Hv_Malloc ------*/
E 3
I 3
/*------ Hv_Malloc ------*/
E 3

D 3
 void  *Hv_Malloc(size_t size)
E 3
I 3
/* calls usual malloc and exits on failure.*/
E 3

D 3
 /* calls usual malloc and exits on failure.*/
E 3
I 3
void *
Hv_Malloc(size_t size)
{
  void *ptr;
E 3

D 3
 {
     void     *ptr;
E 3
I 3
  if(size == 0) return NULL;
E 3

D 3
     if (size == 0)
	 return NULL;
E 3
I 3
  /* ptr = (void *)malloc(size); */
  ptr = (void *)calloc(size, 1);
E 3

D 3
 /*    ptr = (void *)malloc(size);  */
	 ptr = (void *)calloc(size, 1);
E 3
I 3
  if(ptr == NULL)
  {
    Hv_Println("Memory allocation failure in Hv_Malloc.");
    Hv_Println("Size Requested: %d\nExiting", size);
    exit(-1);
  }
E 3

D 3
    if (ptr == NULL) {
	Hv_Println("Memory allocation failure in Hv_Malloc.");
	Hv_Println("Size Requested: %d\nExiting", size);
	exit (-1);
E 3
I 3
  Hv_activePointers ++;
  /*
  printf("Hv_Malloc: [%6d] 0x%08x -> %8d bytes\n",Hv_activePointers,ptr,size);
  */

#ifdef MEM_ALLOC_SCAN
  nmemallocs ++;
  total_memallocs += size;
  for(imemallocs=0; imemallocs<NMEMALLOCS; imemallocs++)
  {
    if(memallocs_ptr[imemallocs] == 0)
    {
      memallocs_ptr[imemallocs] = (int)ptr;
      memallocs_len[imemallocs] = size;
      break;
E 3
    }
D 3
    
    Hv_activePointers++;
    return ptr;
E 3
I 3
  }
  printf("Hv_Malloc: total_memallocs=%8d bytes (%6d)\n",
    total_memallocs,nmemallocs);
#endif


  return(ptr);
E 3
}

/*------- Hv_Free ------*/

D 3
void  Hv_Free(void *ptr)
    
E 3
I 3
void
Hv_Free(void *ptr)
E 3
{
D 3
    if (ptr == NULL)
	return;
    
    Hv_activePointers--;
    free(ptr);
E 3
I 3
  if(ptr == NULL) return;

  Hv_activePointers --;
  /*
  printf("Hv_Free: [%6d] 0x%08x\n",Hv_activePointers,ptr);
  */

#ifdef MEM_ALLOC_SCAN
  nmemallocs --;
  for(imemallocs=0; imemallocs<NMEMALLOCS; imemallocs++)
  {
    if(memallocs_ptr[imemallocs] == (int)ptr)
    {
      memallocs_ptr[imemallocs] = 0;
      total_memallocs -= memallocs_len[imemallocs];
      break;
    }
  }
  printf("Hv_Free: total_memallocs=%8d bytes(%6d)\n",
    total_memallocs,nmemallocs);
#endif

  free(ptr);
E 3
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
	Hv_Println("Memory allocation failure in Hv_CreateString. Exiting.");
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

/**
 * Hv_itoa    Usual itoa function.
 * @param     n   the integer to covert
 * @parat     s   where the string will be stored
 */

void Hv_itoa(int   n,
	     char  s[]) {

    int i, sign;
    
    if ((sign = n) < 0)
	n = -n;
    
    i = 0;
    
    do {
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


/**
 * Hv_RemoveFileExtenstion
 * @purpose Remove the file extenstion from a filename
 * @param fname   The file name -- modified by this routine
 */



void Hv_RemoveFileExtension(char *fname) {

    int    lastslash = -1;
    int    lastdot = -1;
    int    i;
    
    if (fname == NULL)
	return;
    
/*  must worry about "."s appearing in the path
    e.g. /usr2/users/heddle/Hv3.2/Hv_io.c
    so trivial algorithm based on first "."
    is not good enough */
    
/* ALGORITHM:  if LAST "." occurs AFTER last file separator,
   the replace it with a '\0', else do NOTHING */

/* ARGGH now that we ported to windows look for both slashes */
    
    for (i = 0; i < (int)(strlen(fname)); i++) {
		if (fname[i] == '.')
			lastdot = i;
		else if ((fname[i] == '/') || ((fname[i] == '\\')))
			lastslash = i;
    }

    if (lastdot > lastslash)
		fname[lastdot] = '\0';
}


/**
 * Hv_StripLeadingDirectory
 * @purpose Strip the leading directory of a filename, leaving the "bare" filename
 * @param  path   Pointer to the filename
 * @note Example <BR>
 * char  *fname; <BR>
 * char  *barename; <BR>
 * (get the full file name somehow) <BR>
 * barename = fname;  <BR>
 * Hv_StripLeadingDirectory(&barename); <BR>
 */


 void Hv_StripLeadingDirectory(char **path) {
    char  *pos;

/* notice that we look for both types of file separators */

    while((pos = strstr(*path, "/")) != NULL)
		*path = pos+1;

	while((pos = strstr(*path, "\\")) != NULL)
		*path = pos+1;

}

/*--------- Hv_CheckFont -------*/

void Hv_CheckFont(short font)
    
{
    if ((font < 0) || (font >= Hv_NUMFONTS)) {
	Hv_Println("Hv error: font out of range [%d]. Exiting", font);
	exit(0);
    }
    
    if (Hv_fontLoaded[font])
	return;
    
    Hv_AddNewFont(font);
    
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
	size = Hv_sMin(Hv_FONTSIZE25, (short)(size+1));
	break;
	
    case Hv_NEXTSMALLERFONT:
	size = Hv_sMax(Hv_FONTSIZE11, (short)(size-1));
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
			   short        *w,
			   short        *h) {
    
/* calculate the pixel width and height of the string,
   given its specified text and font */

    Hv_CompoundString	cs;
    short 	xmw, xmh;
    
    if (str->text == NULL) {
	Hv_Println("Hv benign warning: tried to size a NULL string.");
	*w = 0;
	*h = 0;
	return;
    }
    
    cs = Hv_CreateAndSizeCompoundString(str->text, str->font, &xmw, &xmh);
    
    *w = (short) xmw;  /* return the width with the proper typecast */
    *h = (short) xmh;  /* return the height with the proper typecast */
    Hv_CompoundStringFree(cs);  /* free the motif string structure */
}

/*------- Hv_CharDimension ---------*/

void  Hv_CharDimension(char  *text,
		       short  font,
		       short  *w,
		       short  *h)
    
{
    Hv_String    *hvstr;
    
    if (text == NULL) {
	Hv_Println("Hv benign warning: tried to char-dim a NULL string.");
	*w = 0;
	*h = 0;
	return;
    }
    
    
    hvstr = Hv_CreateString(text);
    hvstr->font = font;
    Hv_StringDimension(hvstr, w, h);
    Hv_DestroyString(hvstr);
}



/**
 * Hv_ReplaceSubstring
 * @purpose Returns a string with all occurances of substr
 * replaced by newss.
 * @param   s      Original string, NOT modified.
 * @param   substr Substring to search for.
 * @param   newss  Replacement substring, may be NULL.
 * @return  The new string with replacements.
 */

char * Hv_ReplaceSubstring(char *s,
			   char *substr,
			   char *newss) {
  char *t, *t2;
  char *result;
  char *token;
  char *tmp;
  int  newsslen = 0;


  if (s == NULL)
    return NULL;

  result = (char *)malloc(4);
  strcpy(result, "");

  t = (char *)malloc(strlen(s) + 10);
  strcpy(t, s);
  t2 = t;

  if (substr == NULL) {
    free(result);
    return t;
  }


  if (newss != NULL)
    newsslen = strlen(newss);

/* use the tokenizer */

  while ((token = strtok(t, substr)) != NULL) {
/*    fprintf(stderr, "token: [%s]\n", token); */
    if (t != NULL)
      t = NULL;

    tmp = result;
    result = (char *)malloc(strlen(tmp) + strlen(token) + newsslen);

    if (newss != NULL)
      sprintf(result, "%s%s%s", tmp, token, newss);
    else
      sprintf(result, "%s%s", tmp, token);
  
    free(tmp); 
  }

  free(t2);
/*  fprintf(stderr, "result: [%s]\n", result); */
  return result;

}


/*---------- Hv_ReplaceCommas --------*/

void Hv_ReplaceCommas(char *line)
    
/* replace commas with blanks */
    
{
    int   i;
    
    if (line == NULL)
	return;
    
    for (i = 0; i < (int)(strlen(line)); i++)
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
    
    for (i = 0; i < (int)(strlen(line)); i++)
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
	Hv_Println("Hv warning: Called Hv_NumberToString with NULL text.");
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
    
    vabs = (float)fabs(v);
    
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


/**
 *
 */

/* ----- Warning message for invalid command line options */

void Hv_InvalidOptions(unsigned int	argc,
		       char	       *argv[]) {
    
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
    
    for (i = 1; i < n; i++) {
      if (argv[i] != NULL)
	len += strlen(argv[i]);
      else {
	fprintf(stderr, "NULL string in argv, argument number: %d (zero based))\n", i);
      }
    }
     
    len += (n-1);		/* for space between words */
    
/* allocate memory for return string. Account for not enough memory */
    
    buffer = (char *)Hv_Malloc(len+1);		/* + 1 to allow for NULL */	
    
    if (buffer == NULL) {
	Hv_Println("Out of memory in concat_args() ");
	exit(1);
    }
    
    buffer[0] = '\0';
    
    for (i = 1; i < n; i++) {
	if (i > 1)
	    strcat(buffer," ");

	if (argv[i] != NULL)
	  strcat(buffer,argv[i]);
    }
    
    return buffer;
}


/*------- Hv_MaskFromPosition ---- */


static int    Hv_MaskFromPosition(int pos) {
    if ((pos < 0) || (pos > 31))
	return 0;
    
    return (1 << pos);
}



/**
 * reverse  Reverse a string (in place) Strauight from K & R's C Book.
 * @param   s   the string to reverse.
 */


static void reverse (char s[]) {

    /* from kernighan & ritchie's The C Programming Language */
    int c, i, j;
    
    for (i=0, j = strlen(s)-1; i<j; i++, j--) {
	c = s[i];
	s[i] = s[j];
	s[j] = c;
    }
}

/**
 * Hv_AlphaSortDirectory
 * @purpose  return an alphabetized list of files in a directory
 * @param    dirname    The full path name of the directory
 * @param    extension  The extension to mask for
 * @param    names      The returned array of file names
 * @param    prepend    If true, prepend the dir name
 * @param    remtext    If true, remove the file extension
 * @return   The number of files that match 
 */
 
 int   Hv_AlphaSortDirectory(char *dirname,
					     char *extension,
					     char ***names,
					     Boolean prepend,
						 Boolean remext) {

/*
 *  Boolean  prepend   if true, then prepend directory
 *  Boolean  remext    if true, then remove extension
 */

  DIR     *dp;
  int     num = 0;
  int     i, j;
  FILE    *fp;
  char    fname[256];
  char    **narry;
  char    *tstr;

/* open the directory and count the files that match */

  if ((dp = Hv_OpenDirectory(dirname)) == NULL)
    return 0;

  while(Hv_GetNextFile(dp, dirname, extension, &fp, fname))
    if (fp != NULL) {
      num++;
      fclose(fp);
    }
  
  Hv_CloseDirectory(dp);

/* now allocate the names array */

  *names = (char **)Hv_Malloc(num * sizeof(char *));
  narry = *names;

/* now fill the names array */

  dp = Hv_OpenDirectory(dirname);
  i = 0;

  while(Hv_GetNextFile(dp, dirname, extension, &fp, fname))
    if (fp != NULL) {

      if (prepend) {
	narry[i] = (char *)Hv_Malloc(strlen(extension) +
				     strlen(dirname) +
				     strlen(fname) + 6);
	strcpy(narry[i], dirname);
	if (dirname[strlen(dirname)-1] != '/')
	  strcat(narry[i], "/");
	strcat(narry[i], fname);
      }
      else {
	narry[i] = (char *)Hv_Malloc(strlen(extension) +
				     strlen(fname) + 6);
	strcpy(narry[i], fname);
      }

/* remove extension ? */

      if (!remext && (extension != NULL)) {
	if (extension[0] != '.')
	  strcat(narry[i], ".");
	strcat(narry[i], extension);
      }

      fclose(fp);
      i++;
    }
  
  Hv_CloseDirectory(dp);

/* now sort the names array */
  
  if (num > 1) {
    for (i = 0; i < (num - 1); i++)
      for (j = i+1; j < num; j++)
	if (strcmp(narry[i], narry[j]) > 0) {
	  tstr = narry[i];
	  narry[i] = narry[j];
	  narry[j] = tstr; 
	}
  }
  
  return num;
}

 /**
* Fix the file separator to match the OS.
* @param  s   A string, upon return all "wrong" seps replaced 
*             with the correct one.
*/

void Hv_FixFileSeparator(char *s) {
  
  int i;
  
  if (s == NULL)
    return;
  
  for (i = 0; i < Hv_StrLen(s); i++)
  {
    if ((s[i] == '/') || (s[i] == '\\'))
      s[i] = Hv_FileSeparator;
  }
  
  /* remove duplicate slashs */
  for (i = 0; i < Hv_StrLen(s)-1; i++)
  {
    if ((s[i] == Hv_FileSeparator)  && (s[i+1] == Hv_FileSeparator))
    {
      strcpy(&s[i],&s[i+1]);
    }
    
  }
  
}

int Hv_StrLen(char *s) {
	if (s == NULL)
		return 0;
	else
		return strlen(s);
}
E 1
