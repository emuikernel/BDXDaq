
#ifndef VXWORKS

/* init.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Hv.h"
#include "ced.h"
#include "tigris.h"

#define  SCINTILLATOR_VIEW     1
#define  SECTOR_VIEW           2

char *fname; /* global */
int trigColor[6]; /* global */
VisSectorDef secdef[6]; /* global */
Hv_View view[7]; /* global */

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


/* local prototypes */

static void initScintStructures();
static void initScintColors();
static void initED_SP_Structures();
static void WindowTile();
static void DrawTile();
static void CenterWrite(char *str, short xc, short *y, short font,
                        short color);
static void initTrigColors();
static void copyIntoHv();

/*----- simpleInit -------*/

void
simpleInit()
{
  Hv_Widget        menu, DownloadMenu, ControlMenu;
  /*Hv_Item          item;*/
  static  short    sectcnt = 0;
  static short     viewcnt = 0;
  short            left, top, width, height, hrcolor, j;
  float            xmin, xmax, ymin, ymax;
  char             buf1[80];
  int              i;

  for(i=0; i<6; i++)
  {
    for(j=0; j<12; j++)
    {
      JJ_SEtextItem[i][j] = NULL;
      SECommentText[i][j] = NULL;
    }
  }

  left = (viewcnt + 1)*15;
  top = Hv_virtualView->containeritem->area->bottom + 6 + viewcnt*15;
  viewcnt ++;

  sectcnt ++;
  xmin = -400.0;
  xmax =  650.0;
  ymin =  0.0;
  ymax =  560.0;
  width = 700;
  hrcolor = Hv_aliceBlue ;
  height =  (short)((width*(ymax-ymin))/(xmax-xmin)) ;

  printf("Reading the geometry file\n");

  InitGeometry();
  initScintColors();
  initTrigColors();
  initScintStructures();
  initED_SP_Structures();



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

  /*item = */Hv_VaCreateMenuItem(Hv_actionMenu,
                             Hv_LABEL,            (char *)"Open Trigger File",
                             Hv_CALLBACK,         ReadFromFile,
                             NULL);


  /*item = */Hv_VaCreateMenuItem(Hv_actionMenu,
                             Hv_LABEL,            "Save Trigger File",
                             Hv_CALLBACK,         SaveToFile,
                             NULL);

  /* Sergey: new menu item for Async Inputs control */
  Hv_VaCreateMenu(&ControlMenu, Hv_TITLE, " Control", 0);
  menu = Hv_VaCreateMenuItem(ControlMenu,
                             Hv_LABEL,            "Sync Inputs",
                             Hv_CALLBACK,         SyncControl,
                             Hv_ACCELERATORCHAR,  'S',
                             NULL);
  menu = Hv_VaCreateMenuItem(ControlMenu,
                             Hv_LABEL,            "Async Inputs",
                             Hv_CALLBACK,         AsyncControl,
                             Hv_ACCELERATORCHAR,  'A',
                             NULL);


  for (j=0; j<6; j++) {
    sprintf (buf1, "Sector #%d", j+1);
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
		    Hv_TITLE,           buf1,
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
  
  WindowTile();

}


/* ------------- ReadFromFile -------------- */

void
ReadFromFile()
{
  Hv_View temp;
  int     i, mxse, j, sector ;

  printf("read from a file \n");

  initScintStructures();
  initED_SP_Structures();

  fname = Hv_FileSelect("Select Configuration File", "*", NULL);
  if(fname != NULL)
  {
    strcpy(InFileName, fname);
 
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

char *
newViewTitle(char *oldtitle, int opt) 
{
  char *ntitle;
  char *subs;
  char *tfn;
  int len;

  if((InFileName == NULL) || (oldtitle == NULL)) return;

  subs = strstr(oldtitle, "[");
  if(subs != NULL) *subs = '\0';

  ntitle = (char *)Hv_Malloc(strlen(InFileName) + strlen(oldtitle) + 30);

  Hv_InitCharStr(&tfn, InFileName);

fprintf(stderr, "fn = %s\n", InFileName);
fprintf(stderr, "tfn = %s\n", tfn);

  Hv_StripLeadingDirectory(&tfn);

fprintf(stderr, "tfn = %s\n", tfn);

  if(opt == 0)
    sprintf(ntitle, "%s   [%s]", oldtitle, tfn);
  else
    sprintf(ntitle, "%s   [** %s]", oldtitle, tfn);

  return(ntitle);
}

/* --------- SaveToFile ------------------ */

void
SaveToFile()
{
  int i,j, k, l;
/* 
 * these have to be called with 0 & 1 - SaveFinal Values is different from 
 * download
 */

  fname = Hv_FileSelect("Save Configuration File", "*", NULL);
  if(fname != NULL)
  { 
    strcpy(OutFileName, fname) ;
  }
  else
  {
    strcpy(OutFileName, "ja");
  }

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
      }
    }
  }

  movSE2sgl();

  for(i=1; i<7; i++)
  {
    for(j=0; j<12; j++)
    {
      if(SECommentText[i-1][j] != 0)
      {
        if(Sector[i].comment[j]) free(Sector[i].comment[j]);
        Hv_InitCharStr(&(Sector[i].comment[j]),SECommentText[i-1][j]);
      }
    }
  }

  for(i=1; i<7; i++)
  {
    for(j=0; j<12; j++)
    {
      if(Sector[i].comment[j] != 0) printf("comment >%s<\n",Sector[i].comment[j]);
    }
  }

  TrigParser();

  SaveFinalValues();

}

/* -------- initScintStructures ----------- */

static void
initED_SP_Structures()
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

/*-------- initScintStructures -----------*/

static void
initScintStructures()
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

  memset(Jti.can_not_do, 0, (8*2*6));

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


  /* 'phase3' data variables set to 2 (X), or proper initial values */
printf("3===============================\n");
  for(i=0; i<MaxTrig; i++)
  {
    for(j=0; j<MaxTrigProducts; j++)
    {
      for(k=0; k<19; k++)
      {
        phase3.Trig[i][j][k] = 2;
        JAMphase3.Trig[i][j][k] = 2;
      }
    }
    phase3.Trig_Delay[i] = 1;
    phase3.Enable_Async[i] = 0;
  }

  for(i=1; i<9; i++) 
    for(j=0; j<MaxTrigProducts; j++)
      for(k=0; k<19; k++)
        JAMphase3.Trig[i][j][k] = phase3.Trig[i][j][k];

}

/* ------------- initScintEventStructs ------------------ */

void
initScintEventStructs(int sector)
{
  int j;
  
  for(j=0; j<12; j++) JJ_SEtextItem[sector][j] = NULL;
}

/*-------- WindowTile -----------*/

static void
WindowTile()
{
  Hv_tileX = 300;
  Hv_tileY = 80;
  Hv_TileDraw = DrawTile;
}

/*------ DrawTile --------*/

static void
DrawTile()
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

static void
CenterWrite(char *str, short xc, short *y, short font, short color)
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

static void
initScintColors()
{
  int i;

  scintColor[0] = Hv_green + 3;
  for (i=1; i<5; i++) 
    scintColor [i] = scintColor[i-1] + 3 ;

  scintColor[5] = Hv_peru ;

}

/* ------------------ init trigger colors ----------------------*/

static void
initTrigColors()
{

  trigColor[0] = Hv_red;
  trigColor[1] = Hv_yellow;
  trigColor[2] = Hv_black;
  trigColor[3] = Hv_lawnGreen;
  trigColor[4] = Hv_tan3;
  trigColor[5] = Hv_purple;

}

/* ----------------------- copy into JAM struct --------------------- */

static void
copyIntoHv()
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
  memset(bfr, 0, 8192);
  oneMore=0;
  jump = 0;
  for(jtrig=0, itrig = 1; itrig < 9; itrig++)
  {
    for(j=0; j<MaxTrigProducts; j++)
    {
      if((j != 0) && (j != (MaxTrigProducts-1))) jump = 1;

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

        if(++SEN > 3)
        {
          sector++; 
          SEN = 1; 
        }

        JAMphase3.Trig[itrig][j][k] = phase3.Trig[itrig][j][k];
      }
    }

    if(!(itrig % 2)) oneMore += 3 ;

    if(bfr[0] != 0)
    {
      printf("\n%s\n",bfr);
      if(itrig > 4)
      {
        Hv_InitCharStr(&(Jti.trigString[1][jtrig]),bfr);
        /*Jti.trigString[1][jtrig] = strdup(bfr);*/
      }
      else
      {
	    Hv_InitCharStr(&(Jti.trigString[0][jtrig]), bfr);
        /*Jti.trigString[0][jtrig] = strdup(bfr);*/
      }
    }
    if(++jtrig > 3) jtrig = 0;
    memset(bfr, 0, 8192);
  }

  refresh_trigr(0);
}

#else /* UNIX only */

void
init_dummy()
{
}

#endif



