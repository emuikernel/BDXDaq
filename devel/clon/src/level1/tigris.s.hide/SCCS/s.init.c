h05520
s 00009/00000/00727
d D 1.8 06/09/01 00:35:07 boiarino 9 8
c dummy VXWORKS
c 
e
s 00006/00001/00721
d D 1.7 02/08/28 08:13:38 boiarino 8 7
c separate menu items for sync and async input control
e
s 00226/00217/00496
d D 1.6 02/08/25 19:44:08 boiarino 7 6
c big cleanup (globals, locals, prototypes)
c implement new menu item for Async Inputs control
c fix some place(s) to make async stuff works
e
s 00004/00005/00709
d D 1.5 02/08/22 22:07:39 boiarino 6 5
c seems ok now ...
e
s 00008/00004/00706
d D 1.4 02/08/22 21:32:31 boiarino 5 4
c still under development ...
c 
e
s 00012/00002/00698
d D 1.3 02/08/22 21:08:08 boiarino 4 3
c comment out CREG menu,
c implement Control menu
c 
e
s 00003/00003/00697
d D 1.2 02/08/22 20:53:05 boiarino 3 1
c fix warnings:  /*Hv_Item item;*/ etc
e
s 00000/00000/00000
d R 1.2 02/03/29 12:13:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/tigris/init.c
e
s 00700/00000/00000
d D 1.1 02/03/29 12:13:01 boiarino 1 0
c date and time created 02/03/29 12:13:01 by boiarino
e
u
U
f e 0
t
T
I 5

I 9
#ifndef VXWORKS

E 9
/* init.c */

I 7
#include <stdio.h>
E 7
E 5
I 1
#include <stdlib.h>
#include <string.h>
#include "Hv.h"
D 7
#include "simple.h"
#include "tgeometry.h"
E 7
#include "ced.h"
D 7
#include "scintlook.h"
#include "sgl.h"
E 7
I 7
#include "tigris.h"
E 7

#define  SCINTILLATOR_VIEW     1
#define  SECTOR_VIEW           2

D 7
static void DrawTile () ;
static void CenterWrite () ;
static void WindowTile () ;
E 7
I 7
char *fname; /* global */
int trigColor[6]; /* global */
VisSectorDef secdef[6]; /* global */
Hv_View view[7]; /* global */
E 7

D 7
static void initScintColors () ;
static void initED_SP_Structures ();
E 7
I 7
extern char *JJ_SEtextItem[6][12]; /* defined in setup.c */
extern char *SECommentText[6][12]; /* defined in setup.c */
extern TriggerInfo Jti; /* defined in setup.c */
extern char buf[SIZEOBUF]; /* defined in evbutton.c */
extern BitsLookup bitslookup[48]; /* defined in scintillator.c */
extern char InFileName[512]; /* defined in fileio.c */
extern char OutFileName[512]; /* defined in fileio.c */
extern int SDbits4sgl[6][2][96]; /* defined in scintbuts.c */
extern int SPbits4sgl[6][2][64]; /* defined in scintbuts.c */
extern int EDbits4sgl[6][2][96]; /* defined in scintbuts.c */
extern int scintColor[6]; /* defined in scintillator.c */
extern struct SLsecdef Sector[7]; /* defined in fileio.c */
extern struct phase3_def phase3; /* defined in fileio.c */
extern struct phase3_def JAMphase3; /* defined in fileio.c */
E 7

D 6
extern scintColor [6] ;
extern trigColor [6] ;
extern char * JJ_SEtextItem [6][12];
extern char * SECommentText [6][12];
E 6
I 6
D 7
extern scintColor[6] ;
extern trigColor[6] ;
extern char *JJ_SEtextItem [6][12];
extern char *SECommentText [6][12];
E 6
void initTrigColors () ;
E 7

D 7
void                       ReadFromFile ();
extern int                 ReadValues ();
void                       SaveToFile ();
void                       copyIntoHv ();
static void                initScintStructures ();
E 7
I 7
/* local prototypes */
E 7

D 7
static void                initED_SP_Structures ();
E 7
I 7
static void initScintStructures();
static void initScintColors();
static void initED_SP_Structures();
static void WindowTile();
static void DrawTile();
static void CenterWrite(char *str, short xc, short *y, short font,
                        short color);
static void initTrigColors();
static void copyIntoHv();
E 7

D 7

extern char                buf[SIZEOBUF];
extern struct phase3_def   phase3;
extern struct phase3_def   JAMphase3;

extern VisSectorDef        secdef[] ;
extern void                DownloadControl () ;
D 5
extern void                CREGControl () ;
I 4
extern void                AsyncControl () ;
E 5
I 5
/*extern void                CREGControl();*/
extern void                AsyncControl();
E 5
E 4
extern struct SLsecdef     Sector[] ;
extern BitsLookup          bitslookup [] ;

extern TriggerInfo         Jti;

extern char                InFileName[] ;
extern char                OutFileName[] ;

extern int                 EV_case ();
Hv_View                    view[7];
extern int                 SDbits4sgl[6][2][96];
extern int                 SPbits4sgl[6][2][64];
extern int                 EDbits4sgl[6][2][96];
extern void                mov2sgl ();
extern void                movPhase12sgl ();
extern void                movSE2sgl ();
extern void                TrigParser ();
extern int                 SaveFinalValues ();
extern void                init_bitslookup ();

E 7
/*----- simpleInit -------*/

D 7
void simpleInit()
E 7
I 7
void
simpleInit()
E 7
D 6

E 6
{
D 4
  Hv_Widget        menu, DownloadMenu, CregConfig;
E 4
I 4
D 5
  Hv_Widget        menu, DownloadMenu, CregConfig, ControlMenu;
E 5
I 5
D 7
  Hv_Widget        menu, DownloadMenu/*, CregConfig*/, ControlMenu;
E 7
I 7
  Hv_Widget        menu, DownloadMenu, ControlMenu;
E 7
E 5
E 4
D 3
  Hv_Item          item;
E 3
I 3
  /*Hv_Item          item;*/
E 3
  static  short    sectcnt = 0;
  static short     viewcnt = 0;
  short            left, top, width, height, hrcolor, j;
  float            xmin, xmax, ymin, ymax;
D 7
  char             buf[80];
E 7
I 7
  char             buf1[80];
E 7
  int              i;

D 7
  for (i = 0; i < 6; i++)
    for (j = 0; j < 12; j++) {
E 7
I 7
  for(i=0; i<6; i++)
  {
    for(j=0; j<12; j++)
    {
E 7
      JJ_SEtextItem[i][j] = NULL;
      SECommentText[i][j] = NULL;
    }
I 7
  }
E 7

D 7


E 7
  left = (viewcnt + 1)*15;
  top = Hv_virtualView->containeritem->area->bottom + 6 + viewcnt*15;
D 7
  viewcnt++;
E 7
I 7
  viewcnt ++;
E 7

D 7
  sectcnt++;
E 7
I 7
  sectcnt ++;
E 7
  xmin = -400.0;
  xmax =  650.0;
  ymin =  0.0;
  ymax =  560.0;
  width = 700;
  hrcolor = Hv_aliceBlue ;
  height =  (short)((width*(ymax-ymin))/(xmax-xmin)) ;

D 7
  printf ("Reading the geometry file\n") ;
E 7
I 7
  printf("Reading the geometry file\n");
E 7

D 7
  InitGeometry () ;
  initScintColors () ;
  initTrigColors () ;
  initScintStructures () ;
  initED_SP_Structures ();
E 7
I 7
  InitGeometry();
  initScintColors();
  initTrigColors();
  initScintStructures();
  initED_SP_Structures();
E 7



  Hv_VaCreateMenu(&DownloadMenu, Hv_TITLE, " Download", NULL);

  menu = Hv_VaCreateMenuItem(DownloadMenu,
                             Hv_LABEL,            "Download ...",
                             Hv_CALLBACK,         DownloadControl,
                             Hv_ACCELERATORCHAR,  'D',
                             NULL);

  Hv_VaCreateMenuItem(Hv_actionMenu,
                             Hv_TYPE,
                             Hv_SEPARATOR,
                             NULL);

D 3
  item = Hv_VaCreateMenuItem(Hv_actionMenu,
E 3
I 3
  /*item = */Hv_VaCreateMenuItem(Hv_actionMenu,
E 3
                             Hv_LABEL,            (char *)"Open Trigger File",
                             Hv_CALLBACK,         ReadFromFile,
                             NULL);


D 3
  item = Hv_VaCreateMenuItem(Hv_actionMenu,
E 3
I 3
  /*item = */Hv_VaCreateMenuItem(Hv_actionMenu,
E 3
                             Hv_LABEL,            "Save Trigger File",
                             Hv_CALLBACK,         SaveToFile,
                             NULL);

I 7
  /* Sergey: new menu item for Async Inputs control */
E 7
D 4
  Hv_VaCreateMenu (&CregConfig, Hv_TITLE, " CREG Config", 0);
E 4
I 4
  Hv_VaCreateMenu(&ControlMenu, Hv_TITLE, " Control", 0);
  menu = Hv_VaCreateMenuItem(ControlMenu,
D 8
                             Hv_LABEL,            "Async Control",
E 8
I 8
                             Hv_LABEL,            "Sync Inputs",
                             Hv_CALLBACK,         SyncControl,
                             Hv_ACCELERATORCHAR,  'S',
                             NULL);
  menu = Hv_VaCreateMenuItem(ControlMenu,
                             Hv_LABEL,            "Async Inputs",
E 8
                             Hv_CALLBACK,         AsyncControl,
                             Hv_ACCELERATORCHAR,  'A',
                             NULL);
E 4

I 4
D 5
  /*
E 5
I 5

D 7
  /*  
E 5
  Hv_VaCreateMenu (&CregConfig, Hv_TITLE, " CREG Config", 0);
E 4
  menu = Hv_VaCreateMenuItem(CregConfig,
                             Hv_LABEL,            "CREG config",
                             Hv_CALLBACK,         CREGControl,
                             Hv_ACCELERATORCHAR,  'C',
                             NULL);
I 4
  */
E 4

I 4

E 7
E 4
  for (j=0; j<6; j++) {
D 7
    sprintf (buf, "Sector #%d", j+1);
E 7
I 7
    sprintf (buf1, "Sector #%d", j+1);
E 7
    Hv_VaCreateView(&(view[j]),
		    Hv_TAG,             SCINTILLATOR_VIEW,
		    Hv_INITIALIZE,      SetupScintView,
		    Hv_LEFT,            left,
		    Hv_TOP,             top,
		    Hv_XMIN,            xmin,
		    Hv_XMAX,            xmax,
		    Hv_YMIN,            ymin,
		    Hv_YMAX,            ymax,
		    Hv_HOTRECTWIDTH,    width,
		    Hv_HOTRECTHEIGHT,   height+20,
		    Hv_HOTRECTVMARGIN,  50,
		    Hv_HEIGHT,          height*2+41,
		    Hv_HOTRECTCOLOR,    hrcolor,
D 7
		    Hv_TITLE,           buf,
E 7
I 7
		    Hv_TITLE,           buf1,
E 7
		    Hv_DRAWCONTROL,     Hv_SQUARE + Hv_ZOOMONRESIZE,
		    Hv_FEEDBACK,        scintViewFeedback,
		    NULL);
  }
  
  Hv_VaCreateView(&(view[6]),
		  Hv_TAG,             SECTOR_VIEW,
		  Hv_HOTRECTHMARGIN,  53,
		  Hv_XMIN,            0.0,
		  Hv_XMAX,            1.0,
		  Hv_YMIN,            0.0,
		  Hv_YMAX,            2.0,
		  Hv_HOTRECTHEIGHT,   700,
		  Hv_HOTRECTWIDTH,    350,
		  Hv_HEIGHT,          800,
		  Hv_WIDTH,           950,
		  Hv_HOTRECTCOLOR,    hrcolor,
		  Hv_INITIALIZE,      SetupSectView,
		  Hv_TITLE,           " Trigger View ",                         
		  Hv_DRAWCONTROL,     Hv_SQUARE + Hv_ZOOMONRESIZE,
		  Hv_FEEDBACK,        scintViewFeedback,
		  NULL);
  
D 7
  WindowTile () ;
E 7
I 7
  WindowTile();
E 7

}

I 7

E 7
/* ------------- ReadFromFile -------------- */

D 7
void ReadFromFile ()
E 7
I 7
void
ReadFromFile()
E 7
{
D 7
  extern  char * fname;
E 7
  Hv_View temp;
  int     i, mxse, j, sector ;

D 7
  printf ("read from a file \n");
E 7
I 7
  printf("read from a file \n");
E 7

D 7
  initScintStructures () ;
  initED_SP_Structures () ;
E 7
I 7
  initScintStructures();
  initED_SP_Structures();
E 7

  fname = Hv_FileSelect("Select Configuration File", "*", NULL);
D 7
  if (fname != NULL) {

    strcpy (InFileName, fname) ;
    
E 7
I 7
  if(fname != NULL)
  {
    strcpy(InFileName, fname);
 
E 7
    for (sector=0; sector<7; sector++) 
      for (mxse=0; mxse<12; mxse++) 
	for (i=0; i<32; i++) 
	  for (j=0; j<16; j++) 
	    Sector[sector].SE[mxse][i][j] = 2;
    
    if ((ReadValues ()) == 0)
      copyIntoHv ();
    
    for (temp = Hv_views.first;  temp != NULL;  temp = temp->next) {
	if (temp->tag != Hv_VIRTUALVIEW)
	    Hv_SetViewTitle(temp, newViewTitle(Hv_GetViewTitle(temp), 0)); 
	Hv_DrawView (temp, NULL);
    }
  }
  /* done or no file selected */
}

/*------------ newViewtitle ---------*/

D 7
char *newViewTitle(oldtitle, opt) 

char *oldtitle;
int   opt;


E 7
I 7
char *
newViewTitle(char *oldtitle, int opt) 
E 7
{
D 7
    char *ntitle;
    char *subs;
    char *tfn;
E 7
I 7
  char *ntitle;
  char *subs;
  char *tfn;
  int len;
E 7

D 7
    int  len;
E 7
I 7
  if((InFileName == NULL) || (oldtitle == NULL)) return;
E 7

D 7
    if ((InFileName == NULL) || (oldtitle == NULL))
	return;
E 7
I 7
  subs = strstr(oldtitle, "[");
  if(subs != NULL) *subs = '\0';
E 7

D 7
    subs = strstr(oldtitle, "[");
    if (subs != NULL)
	*subs = '\0';
E 7
I 7
  ntitle = (char *)Hv_Malloc(strlen(InFileName) + strlen(oldtitle) + 30);
E 7

D 7
    ntitle = (char *)Hv_Malloc(strlen(InFileName) + strlen(oldtitle) + 30);
E 7
I 7
  Hv_InitCharStr(&tfn, InFileName);
E 7

D 7

    Hv_InitCharStr(&tfn, InFileName);

E 7
fprintf(stderr, "fn = %s\n", InFileName);
fprintf(stderr, "tfn = %s\n", tfn);

D 7
    Hv_StripLeadingDirectory(&tfn);
E 7
I 7
  Hv_StripLeadingDirectory(&tfn);
E 7

fprintf(stderr, "tfn = %s\n", tfn);

D 7
    if (opt == 0)
	sprintf(ntitle, "%s   [%s]", oldtitle, tfn);
    else
	sprintf(ntitle, "%s   [** %s]", oldtitle, tfn);
E 7
I 7
  if(opt == 0)
    sprintf(ntitle, "%s   [%s]", oldtitle, tfn);
  else
    sprintf(ntitle, "%s   [** %s]", oldtitle, tfn);
E 7

D 7
    return ntitle;
E 7
I 7
  return(ntitle);
E 7
}

D 7

E 7
/* --------- SaveToFile ------------------ */

D 7
void SaveToFile ()
E 7
I 7
void
SaveToFile()
E 7
{
  int i,j, k, l;
D 7
  char    *fname;
E 7
/* 
 * these have to be called with 0 & 1 - SaveFinal Values is different from 
 * download
 */

  fname = Hv_FileSelect("Save Configuration File", "*", NULL);
D 7
  if (fname != NULL) 
    strcpy (OutFileName, fname) ;
E 7
I 7
  if(fname != NULL)
  { 
    strcpy(OutFileName, fname) ;
  }
E 7
  else
D 7
    strcpy (OutFileName, "ja");
E 7
I 7
  {
    strcpy(OutFileName, "ja");
  }
E 7

D 7
  mov2sgl (0) ;
  mov2sgl (1) ;    /* both phases */
 
  for (k=0; k<6; k++) {
    for (l=0; l<12; l++) {
      for (i=0; i<32; i++) {
	for (j=0; j<16; j++) {
	  Sector[k+1].SE[l][i][j] = 2;
	}
E 7
I 7
  mov2sgl(0);
  mov2sgl(1);    /* both phases */

  for(k=0; k<6; k++)
  {
    for(l=0; l<12; l++)
    {
      for(i=0; i<32; i++)
      {
        for(j=0; j<16; j++)
        {
          Sector[k+1].SE[l][i][j] = 2;
        }
E 7
      }
    }
  }
D 7
  movSE2sgl () ;
E 7

D 7
  for (i=1; i<7; i++) {
    for (j=0; j<12; j++) {
      if (SECommentText[i-1][j] != 0) {
	if (Sector[i].comment [j])
	  free (Sector[i].comment [j]) ;
E 7
I 7
  movSE2sgl();
E 7

D 7
	Hv_InitCharStr(&(Sector[i].comment[j]),SECommentText[i-1][j]); 
E 7
I 7
  for(i=1; i<7; i++)
  {
    for(j=0; j<12; j++)
    {
      if(SECommentText[i-1][j] != 0)
      {
        if(Sector[i].comment[j]) free(Sector[i].comment[j]);
        Hv_InitCharStr(&(Sector[i].comment[j]),SECommentText[i-1][j]);
E 7
      }
    }
  }
D 7
  for (i=1; i<7; i++) {
    for (j=0; j<12; j++) {
      if (Sector[i].comment[j] != 0) 
	printf ("-- %s\n", Sector[i].comment[j]);
E 7

I 7
  for(i=1; i<7; i++)
  {
    for(j=0; j<12; j++)
    {
      if(Sector[i].comment[j] != 0) printf("comment >%s<\n",Sector[i].comment[j]);
E 7
    }
  }

D 7
  TrigParser ();
E 7
I 7
  TrigParser();
E 7

D 7
  SaveFinalValues ();
E 7
I 7
  SaveFinalValues();

E 7
}

/* -------- initScintStructures ----------- */

D 7
void initED_SP_Structures ()
E 7
I 7
static void
initED_SP_Structures()
E 7
{
  int i,j,k,l,m;

  for (i=0; i<6; i++)
    for (j=0; j<2; j++)
      for (k=0; k<96; k++) {
	if (k<64)
	  SPbits4sgl [i][j][k] = 0;
	SDbits4sgl [i][j][k] = 0;
	EDbits4sgl [i][j][k] = 0;
      }
}
I 7

E 7
/*-------- initScintStructures -----------*/

D 7
void initScintStructures ()
E 7
I 7
static void
initScintStructures()
E 7
{
  int i, j, k, l;

  init_bitslookup ();

  Jti.which6 = 0;
  Jti.rstate = 0;
  Jti.butSet1 = 0;
  Jti.butSet2 = 0;

  for (l=0; l<2; l++)
    for (k=0; k<4; k++)
      Jti.trigString [l][k] = 0 ;

D 7
  memset (Jti.can_not_do, 0, (8*2*6));
E 7
I 7
  memset(Jti.can_not_do, 0, (8*2*6));
E 7

  for (i=0; i<4; i++)
    Jti.operator[i] = '*' ;

  for(l=0; l<6; l++)
    for (i=0; i<2; i++)
      for (j=0; j<3; j++)
	for (k=0; k<48; k++) {
	  secdef[l].sd.bits[i][j][k] = 0;
	  secdef[l].sd.state[i][j][k] = 0;
	}

  /* init SE to 2 (the off state) */
  for (l=0; l<6; l++) {
    for (k=0;k<12; k++) {
      for (j=0; j<32; j++) {
	for (i=0; i<16; i++) {
	  secdef[l].se.products[k][j][i] = 2;
	  Sector[l].SE[k][j][i] = 2;
	}
      }
    }
  }


D 7
  /* Phase3 data variables set to 2 (X), or proper initial values */
  for (i=0;i<MaxTrig;i++)
E 7
I 7
  /* 'phase3' data variables set to 2 (X), or proper initial values */
printf("3===============================\n");
  for(i=0; i<MaxTrig; i++)
E 7
  {
D 7
    for (j=0;j<MaxTrigProducts;j++)
      for (k=0;k<19;k++) {
	phase3.Trig[i][j][k] = 2;
	JAMphase3.Trig[i][j][k] = 2;
E 7
I 7
    for(j=0; j<MaxTrigProducts; j++)
    {
      for(k=0; k<19; k++)
      {
        phase3.Trig[i][j][k] = 2;
        JAMphase3.Trig[i][j][k] = 2;
E 7
      }
I 7
    }
E 7
    phase3.Trig_Delay[i] = 1;
    phase3.Enable_Async[i] = 0;
  }

D 7
  for (i = 1; i < 9; i++) 
    for (j=0; j<MaxTrigProducts; j++)
      for (k=0; k<19; k++)
	JAMphase3.Trig[i][j][k] = phase3.Trig[i][j][k];
E 7
I 7
  for(i=1; i<9; i++) 
    for(j=0; j<MaxTrigProducts; j++)
      for(k=0; k<19; k++)
        JAMphase3.Trig[i][j][k] = phase3.Trig[i][j][k];
E 7

D 7

E 7
}
I 7

E 7
/* ------------- initScintEventStructs ------------------ */
D 7
void initScintEventStructs (sector)
int sector;
E 7
I 7

void
initScintEventStructs(int sector)
E 7
{
  int j;
  
D 7
  for (j=0; j<12; j++)
    JJ_SEtextItem[sector][j] = NULL;
E 7
I 7
  for(j=0; j<12; j++) JJ_SEtextItem[sector][j] = NULL;
E 7
}
I 7

E 7
/*-------- WindowTile -----------*/

D 7
static void WindowTile()

E 7
I 7
static void
WindowTile()
E 7
{
  Hv_tileX = 300;
  Hv_tileY = 80;
  Hv_TileDraw = DrawTile;
}

D 7

E 7
/*------ DrawTile --------*/

D 7
static void DrawTile()

E 7
I 7
static void
DrawTile()
E 7
{
  short      xc, y;
  short      color;
  Hv_Rect    rect;
  Hv_Region  region;

  Hv_SetRect(&rect, 0, 0, (short)Hv_tileX, (short)Hv_tileY);
  region = Hv_RectRegion(&rect);
  Hv_SetClippingRegion(region);

  Hv_FillRect(&rect, Hv_canvasColor);

  xc = ((short)Hv_tileX)/2;
  y = 2;

  if (Hv_canvasColor == Hv_blue)
    color = Hv_wheat;
  else
    color = Hv_blue;

  CenterWrite("CHRISTOPHER NEWPORT UNIVERSITY", xc, &y, Hv_helvetica14, color);

  Hv_DestroyRegion(region);
}

/*------- CenterWrite ---------*/

D 7
static void CenterWrite(str, xc, y, font, color)

char           *str;
short          xc;
short          *y;
short          font;
short          color;

E 7
I 7
static void
CenterWrite(char *str, short xc, short *y, short font, short color)
E 7
{
  Hv_String      *hvstr;
  short          w, h;

  hvstr = Hv_CreateString(str);

  hvstr->font = font;
  hvstr->strcol = color;

  Hv_StringDimension(hvstr, &w, &h);
  Hv_DrawString(xc - w/2, *y, hvstr);

  *y += h;
  Hv_DestroyString(hvstr);
}

/* ------------------ init scint colors ----------------------*/

D 7
void initScintColors () 
E 7
I 7
static void
initScintColors()
E 7
{
D 7
int i;
E 7
I 7
  int i;
E 7

  scintColor[0] = Hv_green + 3;
  for (i=1; i<5; i++) 
    scintColor [i] = scintColor[i-1] + 3 ;

  scintColor[5] = Hv_peru ;

}
I 7

E 7
/* ------------------ init trigger colors ----------------------*/

D 7
void initTrigColors () 
E 7
I 7
static void
initTrigColors()
E 7
{

  trigColor[0] = Hv_red;
  trigColor[1] = Hv_yellow;
  trigColor[2] = Hv_black;
  trigColor[3] = Hv_lawnGreen;
  trigColor[4] = Hv_tan3;
  trigColor[5] = Hv_purple;

}

/* ----------------------- copy into JAM struct --------------------- */

D 7
void copyIntoHv ()
E 7
I 7
static void
copyIntoHv()
E 7
{
  int phase, isd, i, j, k, m, zz, h, ss;
  char opr ;
  ViewData vd;
  int itrig, jtrig, l, sector, SEN, jump, oneMore;
  char bfr[8192];

  /*
   * Over all the sectors compute ...
   *   Over both phases compute ...
   *     1st the Spare Data bits - do them BTS. there is only 64 of 'em.
   *     2nd the SD bits
   *     3rd the ED bits - note I do SD and ED at the same time.
   *
   * BTS - By Them Self
   */
  for (ss=0; ss < 6; ss++) {
    for (phase=0; phase<2; phase++) {
      for (k=0; k<4; k++) {
	for (j=0; j<16; j++) {
	  if (Sector[ss+1].SP [phase][k][j+1] == 1)
	    SPbits4sgl [ss][phase][ j+(k*16) ] = 1;
	}
      }
      for (isd=0; isd<3; isd++) {
	for (i=0; i<17; i++) {
	  if (Sector[ss+1].SD[phase][isd][i+1] == 1) {
	    for (h=0; h<bitslookup[i].howMany; h++) 
	      secdef[ss].sd.bits[phase][isd][ bitslookup[i].litBits[h] ] += 1;
	    vd = (ViewData) view[ss]->data;

	    SDbits4sgl [ vd->sectorNumber ][phase][ i+(isd*16) ] = 1;
	    secdef[ss].sd.state[phase][isd][i] = 1;
	  }
	  if (Sector[ss+1].ED [phase][isd][i+1] == 1) 
	    EDbits4sgl [ ss ][phase][ i+(isd*16) ] = 1;

	}
      }
      /* 
       * Start on the back (gray) set of scints. We turn on bits using
       * each even number (16, 18, 20, ...) in the lookup table (which 
       * only turns on 3 bits at a time). For the drawing routines to
       * work we must set the "[shared] controller bit" (see 
       * scintillator.c ... backBitsOn ... howMany == 5).
       *
       * Notice that this also does the ED's. They are the same shape
       * so I compute them in here also.
       *
       */
      for (isd=3, zz=0; isd<6; isd++, zz++) {
	k=0; j=16; i=17;
	if (Sector[ss+1].SD [phase][isd][i] == 1) {
	  secdef [ss].sd.bits [phase][zz][ bitslookup[j].litBits[ 0 ] ] = 1;
	  secdef [ss].sd.bits [phase][zz][ bitslookup[j].litBits[ 1 ] ] = 1;
	  vd = (ViewData) view[ss]->data;
	  SDbits4sgl [ vd->sectorNumber ][phase][ k+(isd*16) ] = 1;
	  secdef[ss].sd.state[phase][isd-3][j] = 1; 
	}

	for (k=1, j=18, i=18; i<33; k++, j+=2, i++) {
	  if (Sector[ss+1].SD [phase][isd][i] == 1) {
	    for(h=0; h<bitslookup[j].howMany; h++)
	      secdef [ss].sd.bits [phase][zz][ bitslookup[j].litBits[ h ] ] += 1;

	    vd = (ViewData) view[ss]->data;
	    SDbits4sgl [ vd->sectorNumber ][phase][ k+(isd*16) ] = 1;
	    if (j==18)
	      secdef[ss].sd.state[phase][isd-3][j-1] = 1;  /* lite the 17'th one also */
	    secdef[ss].sd.state[phase][isd-3][j] = 1; 
	  }
	} 
	for (k=0, i=17; i<33; k++, i++) {
	  if (Sector[ss+1].ED [phase][isd][i] == 1) 
	    EDbits4sgl [ ss ][phase][ k+(isd*16) ] = 1;
	}
      }
    }

    /*
     * loop thru sgl's SE struct and build up a string in
     *      buf that represents the terms encoded (by position) 
     *      in the sgl struct.
     * SO:
     *  buf is global.
     *  EV_case does all the string building
     * ultimately, buf is strdup'ed onto JJ_SETextItem
     * then the whole thing is displayed.
     *
     */
    for (m=0; m<12; m++) {
      memset (buf, 0, SIZEOBUF);
      for (opr = ' ', i=0; i<32; i++) {
	if ((i > 0) && (i < 32)) 
	  opr = '+' ;
	for (j=0; j<16; j++) {
	  secdef [ss].se.products[m][i][j] = Sector[ss+1].SE[m][i][j] ;
	  if ( (EV_case (secdef [ss].se.products[m][i][j], j, &opr)) )
	    opr = '*' ;
	}
      }
      if (buf[0] != 0)
	sprintf (buf, "%s *", buf) ;
      if (JJ_SEtextItem[ss][m])
	free (JJ_SEtextItem[ss][m]) ;

      Hv_InitCharStr(&(JJ_SEtextItem[ss][m]), buf);
/*      JJ_SEtextItem[ss][m] = strdup (buf) ; */
      printf ("%d <%s>\n", m, JJ_SEtextItem[ss][m]) ;

      vd = (ViewData)view[ss]->data;
      if (m < 6)
	Hv_SetItemText (vd->eitem[m], JJ_SEtextItem[ss][m]) ;
    }
  }
  /*
   * JAM - add support for VERY long SE comments 
   */
  for (i=0; i<6; i++) {
    for (j=0; j<12; j++) {
      if (SECommentText[i][j] != NULL) {
	free (SECommentText[i][j]) ;
	SECommentText [i][j] = NULL;
      }
      if (Sector[i+1].comment[j]) {
	printf ("--- %d %d %s\n", i,j, Sector[i+1].comment[j]);

	Hv_InitCharStr(&(SECommentText[i][j]), Sector[i+1].comment[j]);

/*	SECommentText[i][j] = strdup (Sector[i+1].comment[j]) ; */
	/* make it so ! ... */
	vd = (ViewData)view[i]->data;
	Hv_SetItemText (vd->evcommentbox,SECommentText[i][0]);
      }
    }
  }


  /*
   * now build up the trigger equations from the sgl struct.
   * 
   * jump - flags to signal new product (an or of 2 terms)
   * the phase3.Trig memory should be init to:
   *   0 = not some thing
   *   1 = and some thing
   *   2 = the "zero state".
   *
   */
D 7
  memset (bfr, 0, 8192);
E 7
I 7
  memset(bfr, 0, 8192);
E 7
  oneMore=0;
  jump = 0;
D 7
  for (jtrig=0, itrig = 1; itrig < 9; itrig++) {
    for (j=0; j<MaxTrigProducts; j++) {
      if ((j != 0) && (j != (MaxTrigProducts-1))) 
	jump = 1;
E 7
I 7
  for(jtrig=0, itrig = 1; itrig < 9; itrig++)
  {
    for(j=0; j<MaxTrigProducts; j++)
    {
      if((j != 0) && (j != (MaxTrigProducts-1))) jump = 1;
E 7

D 7
      for (sector=1, SEN=1, k=0; k<19; k++) {
	switch (phase3.Trig[itrig][j][k]) {
	case 0: { 
	  if (jump==1) {
	    l = strlen (bfr);
	    bfr[l-2] = '+';
	    jump = 0;
	  }
	  sprintf (bfr, "%s/S%d.SE%d * ", bfr, sector, (oneMore+SEN)); 
	  break; 
	}
	case 1: { 
	  if (jump==1) {
	    l = strlen (bfr);
	    bfr[l-2] = '+';
	    jump = 0;
	  }
	  sprintf (bfr, "%sS%d.SE%d * ", bfr, sector, (oneMore+SEN)); 
	  break; 
	}
	default: break;
	}
	if (++SEN > 3) {
	  sector++; 
	  SEN = 1; 
	} 
E 7
I 7
      /* for(sector=1, SEN=1, k=0; k<19; k++)
      Sergey: '19' here is a BUG: phase3.Trig[itrig][j][18]
      is SYNC/EXT flag which has nothing to do with the current procedure */
      for(sector=1, SEN=1, k=0; k<18; k++)
      {
        switch(phase3.Trig[itrig][j][k])
        {
          case 0:
          { 
            if(jump==1)
            {
              l = strlen(bfr);
              bfr[l-2] = '+';
              jump = 0;
            }
            sprintf(bfr,"%s/S%d.SE%d * ",bfr,sector,(oneMore+SEN)); 
            break; 
          }
          case 1:
          { 
            if(jump==1)
            {
              l = strlen (bfr);
              bfr[l-2] = '+';
              jump = 0;
            }
            sprintf(bfr,"%sS%d.SE%d * ",bfr,sector,(oneMore+SEN)); 
            break;
          }
          default:
          {
            break;
          }
        }
E 7

D 7
	JAMphase3.Trig[itrig][j][k] = phase3.Trig[itrig][j][k];
E 7
I 7
        if(++SEN > 3)
        {
          sector++; 
          SEN = 1; 
        }

        JAMphase3.Trig[itrig][j][k] = phase3.Trig[itrig][j][k];
E 7
      }
    }
D 7
    if (!(itrig % 2))
      oneMore += 3 ;
E 7

D 7
    if (bfr[0] != 0) {
      if (itrig > 4)
	  Hv_InitCharStr(&(Jti.trigString [1][ jtrig ]), bfr);
/*	Jti.trigString [1][ jtrig ] = strdup (bfr); */
E 7
I 7
    if(!(itrig % 2)) oneMore += 3 ;

    if(bfr[0] != 0)
    {
      printf("\n%s\n",bfr);
      if(itrig > 4)
      {
        Hv_InitCharStr(&(Jti.trigString[1][jtrig]),bfr);
        /*Jti.trigString[1][jtrig] = strdup(bfr);*/
      }
E 7
      else
D 7
	  Hv_InitCharStr(&(Jti.trigString [0][ jtrig ]), bfr);
/*	Jti.trigString [0][ jtrig ] = strdup (bfr); */
E 7
I 7
      {
	    Hv_InitCharStr(&(Jti.trigString[0][jtrig]), bfr);
        /*Jti.trigString[0][jtrig] = strdup(bfr);*/
      }
E 7
    }
D 7
    if (++jtrig > 3) jtrig = 0;
    memset (bfr, 0, 8192);
E 7
I 7
    if(++jtrig > 3) jtrig = 0;
    memset(bfr, 0, 8192);
E 7
  }
D 7
  refresh_trigr (0);
E 7
I 7

  refresh_trigr(0);
E 7
}
I 7

I 9
#else /* UNIX only */
E 9

I 9
void
init_dummy()
{
}
E 9

I 9
#endif
E 9

I 9

E 9
E 7

E 1
