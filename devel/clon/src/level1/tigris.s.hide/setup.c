
#ifndef VXWORKS

/* setup.c */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "Hv.h"
#include "ced.h"
#include "tigris.h"

#define  simpleANGLETAG    1001
#define SMALL_BOX_SIZE     12


/* 6 sectors with 12 SE per sector */
char *JJ_SEtextItem[6][12]; /* global */
char *SECommentText[6][12]; /* global */
TriggerInfo Jti; /* global */


extern VisSectorDef secdef[6]; /* defined in init.c */
extern int SDbits4sgl[6][2][96]; /* defined in scintbuts.c */
extern int SPbits4sgl[6][2][64]; /* defined in scintbuts.c */
extern int EDbits4sgl[6][2][96]; /* defined in scintbuts.c */
extern Hv_View view[7]; /* defined in init.c */



/* local variables */

static int numberOView = 0;



/* local prototypes */

static void SS_ButtonClick(Hv_Event hevent);
static void CopySectorSetup(Hv_View View, Hv_Rect *hotrect);
static void SelectSector(Hv_View View, Hv_Rect *hotrect);
static void InitViewData(Hv_View View);
static void DrawBeamlineItem(Hv_Item Item, Hv_Region region);
static void FixBeamlineArea(Hv_Item Item);


/*------- GetViewData --------*/

ViewData
GetViewData(Hv_View View)
{
  return (ViewData)(View->data);
}

/*--------- MidplaneXtoProjectedX ------*/

float
MidplaneXtoProjectedX(Hv_View View, float x)
{
  ViewData    viewdata;

  viewdata = GetViewData(View);

  if (viewdata->dphi == 0)
    return x;
  else
    return x/cos(Hv_DEGTORAD*(viewdata->dphi));
}

/* --------- SetupSimpleView -------- */

void
SetupScintView(Hv_View View)
{
  Hv_Item      Feedback, beam, sebox, bbox, it ;
  Hv_Rect      *hotrect = View->hotrect;
  Hv_Item      ts1 ;
  int          vs, i ;
  ViewData     vdata;


/* feedback strings */

  char      *text0 = "Scint #: ";

/* create a feedback item */

  Feedback = Hv_VaCreateItem(View,
			     Hv_TYPE,              Hv_FEEDBACKITEM,
			     Hv_LEFT,              hotrect->right + 15,
			     Hv_TOP,               hotrect->top-45,
			     Hv_FEEDBACKDATACOLS,  28,
			     Hv_FEEDBACKENTRY,     SIMPLE_FB_SCINT, Hv_times20, Hv_black, text0,
			     Hv_FEEDBACKMARGIN,    25,
			     NULL);

  InitViewData (View) ;

  vdata = GetViewData (View) ;

  vs = abs (vdata->visiblesectors) - 1 ;

  for (i = 0; i < 4; i++) {
    vdata->ArrayOScintillators[i] = MallocScintillator(View, vs, i);
  }
  SetScintGeometrySector (View) ;

  beam = Hv_VaCreateItem(View,
                         Hv_TAG,         BEAMLINE,
                         Hv_BALLOON,     (char *)"Visual representation of the beam.",
                         Hv_DRAWCONTROL, Hv_ZOOMABLE,
                         Hv_USERDRAW,    DrawBeamlineItem,
                         Hv_FIXREGION,   FixBeamlineArea,
                         NULL);

  FixBeamlineArea(beam);

  buildEventArea(View);
  SelectSector (View, hotrect) ;
  CopySectorSetup (View, hotrect) ;
  DoBigViewSetup (View, hotrect);
}

/* --------------- SS_ButtonClick ----------------- */

static void
SS_ButtonClick(Hv_Event hevent)
{
  ViewData vdata, vdata2;
  int i,j,k;
  int indx, snum;

  vdata = (ViewData)hevent->view->data;

  Hv_Warning ("Are You Sure You Want to copy over that sector?");
  indx = hevent->item->user1 - 1;
  snum = vdata->sectorNumber;

  printf ("U pushed a button! [%d, %d]\n", hevent->item->user1, vdata->sectorNumber);
  for (i=0; i<2; i++) {
    for (j=0; j<3; j++) {
      for (k=0; k<48; k++) {
	secdef[indx].sd.bits[i][j][k] = secdef[snum].sd.bits[i][j][k];
	secdef[indx].sd.state[i][j][k] = secdef[snum].sd.state[i][j][k];
      }
    }
  }
  /* copy the Dave's SD bits and FC/CC bits */
  for (j=0; j<2; j++) {
    for (k=0; k<96; k++) {
      SDbits4sgl [indx][j][k] = SDbits4sgl [snum][j][k];
      EDbits4sgl [indx][j][k] = EDbits4sgl [snum][j][k];
    }
  }
  for (j=0; j<2; j++) {
    for (k=0; k<64; k++) {
      SPbits4sgl [indx][j][k] = SPbits4sgl [snum][j][k];
    }
  }

  flush_those_textItems(vdata);

  for (j=0; j<12; j++) {
    if (JJ_SEtextItem [indx][j] != NULL)
      free (JJ_SEtextItem [indx][j]);

    if (JJ_SEtextItem [snum][j] != NULL)
	Hv_InitCharStr(&(JJ_SEtextItem [indx][j]), JJ_SEtextItem [snum][j]);
/*      JJ_SEtextItem [indx][j] = strdup (JJ_SEtextItem [snum][j]); */
    else
	JJ_SEtextItem [indx][j] = NULL;
  }

  vdata2 = view[indx]->data;  /* view range 0 - 6 or 1 thru 7: views 1 - 6 are scints */

  for (j=0; j<6; j++) {
    Hv_SetItemText (vdata2->eitem[j], JJ_SEtextItem[snum][j]);
    Hv_DrawItem (vdata2->eitem[j], NULL);
  }

  for (j=0; j<12; j++) {
    if (SECommentText [indx][j] != NULL) 
      free (SECommentText [indx][j]);

    if (SECommentText[snum][j] != NULL)
	Hv_InitCharStr(&(SECommentText [indx][j]), SECommentText[snum][j]);
/*      SECommentText [indx][j] = strdup (SECommentText[snum][j]) ; */
    else
      SECommentText[indx][j] = NULL;
  }

  Hv_SetItemText (vdata2->evcommentbox, SECommentText [indx][0]) ;
}

/* ------------- CopySectorSetup ------------------ */

static void
CopySectorSetup(Hv_View View, Hv_Rect *hotrect)
{
  Hv_Item     cbox ;

  cbox = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BOXITEM,
                         Hv_BALLOON,           (char *)"Copy data to a new sector",
                         Hv_DOMAIN,            Hv_OUTSIDEHOTRECT,
                         Hv_LEFT,              hotrect->left+(hotrect->width/2)+13,
                         Hv_TOP,               hotrect->top-45,
                         Hv_WIDTH,             hotrect->width/2 - 10,
                         Hv_HEIGHT,            30,
                         NULL) ;
}

/* ------------- SelectSector ---------------------- */

static void
SelectSector(Hv_View View, Hv_Rect *hotrect)
{
  Hv_Item   sbox, stitle, sb[6] ;
  ViewData     vdata;

  vdata = GetViewData (View) ;

  sbox = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BOXITEM,
                         Hv_BALLOON,           (char *)"sector copy functions.",
                         Hv_DOMAIN,            Hv_OUTSIDEHOTRECT,
                         Hv_LEFT,              hotrect->left-4,
                         Hv_TOP,               hotrect->top-45,
                         Hv_WIDTH,             hotrect->width/2 - 10,
                         Hv_HEIGHT,            30,
                         NULL) ;

  stitle = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            sbox,
                         Hv_LEFT,              sbox->area->left+10,
                         Hv_TOP,               sbox->area->top+8,
                         Hv_TEXT,              (char *)"Copy to Sector",
                         NULL) ;

  sb[0] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,

                         Hv_PARENT,            sbox,
                         Hv_LABEL,             "1",
			 Hv_USER1,             1,
                         Hv_LEFT,              sbox->area->left+120,
                         Hv_TOP,               sbox->area->top+6,
                         Hv_WIDTH,             18,
                         Hv_HEIGHT,            18,
                         Hv_SINGLECLICK,       SS_ButtonClick,
                         NULL) ;

  sb[1] = Hv_VaCreateItem (View,
                       Hv_TYPE,              Hv_BUTTONITEM,
                       Hv_PARENT,            sbox,
                       Hv_LABEL,             "2",
			 Hv_USER1,             2,
                       Hv_RELATIVEPLACEMENT,  Hv_POSITIONRIGHT, 
                       Hv_PLACEMENTGAP,      5,
                       Hv_WIDTH,             18,
                       Hv_HEIGHT,            18,
                       Hv_SINGLECLICK,       SS_ButtonClick,
                       NULL) ;

  sb[2] = Hv_VaCreateItem (View,
                       Hv_TYPE,              Hv_BUTTONITEM,
                       Hv_PARENT,            sbox,
                       Hv_LABEL,             "3",
			 Hv_USER1,             3,
                       Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT, 
                       Hv_PLACEMENTGAP,      5,
                       Hv_WIDTH,             18,
                       Hv_HEIGHT,            18,
                       Hv_SINGLECLICK,       SS_ButtonClick,
                       NULL) ;

  sb[3] = Hv_VaCreateItem (View,
                       Hv_TYPE,              Hv_BUTTONITEM,
                       Hv_PARENT,            sbox,
                       Hv_LABEL,             "4",
			 Hv_USER1,             4,
                       Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT, 
                       Hv_PLACEMENTGAP,      5,
                       Hv_WIDTH,             18,
                       Hv_HEIGHT,            18,
                       Hv_SINGLECLICK,       SS_ButtonClick,
                       NULL) ;

  sb[4] = Hv_VaCreateItem (View,
                       Hv_TYPE,              Hv_BUTTONITEM,
                       Hv_PARENT,            sbox,
                       Hv_LABEL,             "5",
			 Hv_USER1,             5,
                       Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT, 
                       Hv_PLACEMENTGAP,      5,
                       Hv_WIDTH,             18,
                       Hv_HEIGHT,            18,
                       Hv_SINGLECLICK,       SS_ButtonClick,
                       NULL) ;

  sb[5] = Hv_VaCreateItem (View,
                       Hv_TYPE,              Hv_BUTTONITEM,
                       Hv_PARENT,            sbox,
                       Hv_LABEL,             "6",
			 Hv_USER1,             6,
                       Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT, 
                       Hv_PLACEMENTGAP,      5,
                       Hv_WIDTH,             18,
                       Hv_HEIGHT,            18,
                       Hv_SINGLECLICK,       SS_ButtonClick,
                       NULL) ;

  Hv_SetItemSensitivity (sb[vdata->sectorNumber], False, True) ;

}

/* ----- InitViewData -------*/

static void
InitViewData(Hv_View View)
{
  ViewData        viewdata;
  static short    vissect = -1;
  char            *oldvtitle, *vtitle, sectstr[7];

  if (View->tag == SECTOR)
    vissect++;

  viewdata = (ViewData)Hv_Malloc(sizeof(ViewDataRec));

/* set the Items to NULL */

  viewdata->AnchorItem = NULL;
  viewdata->FieldRainbowItem = NULL;
  viewdata->InnerADCRainbowItem = NULL;
  viewdata->OuterADCRainbowItem = NULL;
  viewdata->EnergyRainbowItem = NULL;
  viewdata->FieldmapItem = NULL;
  viewdata->FieldDisplayItem = NULL;
  viewdata->SectorHexItem = NULL;
  viewdata->SectorBoxItem = NULL;

  viewdata->ScintItemList.first = NULL;
  viewdata->ScintItemList.last  = NULL;

  viewdata->ECItemList.first = NULL;
  viewdata->ECItemList.last  = NULL;

/* colors */

  viewdata->scintframecolor = Hv_black;
  viewdata->scintfillcolor = Hv_aquaMarine;
  viewdata->scinthitcolor = Hv_red;

  viewdata->ecframecolor[ECINNER] = Hv_black;
  viewdata->ecfillcolor[ECINNER] = Hv_skyBlue;
  viewdata->ecframecolor[ECOUTER] = Hv_black;
  viewdata->ecfillcolor[ECOUTER] = Hv_powderBlue;
  viewdata->echitcolor = Hv_red;
  viewdata->fannedeccolor = Hv_yellow + 8;
  viewdata->fannedecbgcolor = Hv_skyBlue;
  viewdata->fannedecstripcolor = Hv_darkBlue;
  viewdata->fannedechitcolor = Hv_aliceBlue;

/* display controls */

  viewdata->displayEvent    = True;
  viewdata->displayTracks   = True;
  viewdata->displayBeamLine = True;
  viewdata->displayField    = False;
  viewdata->displayDC = True;
  viewdata->displayEC = True;
  viewdata->displaySC = True;
  viewdata->displayCC = True;
  viewdata->displayU = False;
  viewdata->displayV = False;
  viewdata->displayW = False;

  viewdata->ecplane = ECINNER;   /* relevant for EC Views */


/* note that visiblesectors below is NOT a C index. A negative
   value means that this is really a "most data sector" view */

  viewdata->visiblesectors = (vissect % 3) + 1;  /* relevant for sector views only */
  sprintf(sectstr, " [%1d,%1d]", viewdata->visiblesectors, viewdata->visiblesectors+3);

/* make the view title show the sectors being sisplayed */

  oldvtitle = View->containeritem->str->text;
  vtitle = Hv_Malloc(strlen(oldvtitle) + 10);
  strcpy(vtitle, oldvtitle);
  strcat(vtitle, sectstr);

  Hv_Free(vtitle);

  viewdata->phi = Hv_DEGTORAD*60.0*(viewdata->visiblesectors - 1);
  viewdata->dphi = 0;

  viewdata->sectorNumber = numberOView ;
  viewdata->sglphase = 0;
  viewdata->ButState6_12 = 0;
  viewdata->OldCommentIndex = 0;
  numberOView += 1 ;

  View->data = (void *)viewdata;

}

/* ------- SetupSectView --------------- */

void
SetupSectView(Hv_View View)
{
  Hv_Rect  *hotrect = View->hotrect;
  ViewData vdata ;

  InitViewData(View);
  vdata = GetViewData(View);

  Jti.SItem1 = MallocSectorHex(View, 1);

  DrawTrigButtons(View, Jti.SItem1, 0);

  Hv_SetLineWidth(2);
  Hv_DrawLine(Jti.SItem1->area->left+20, 
     Jti.SItem1->area->top+30, Jti.SItem1->area->right-30, Jti.SItem1->area->bottom-30, Hv_black) ;
  Hv_SetLineWidth(0);

  Jti.SItem2 = MallocSectorHex(View, 3);

  DrawTrigButtons(View, Jti.SItem2, 3);

  Hv_SetLineWidth(2);
  Hv_DrawLine (Jti.SItem2->area->left+20, 
     Jti.SItem2->area->top+30, Jti.SItem2->area->right-30, Jti.SItem2->area->bottom-30, Hv_black) ;
  Hv_SetLineWidth(0);

  Jti.RItem = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_RADIOCONTAINERITEM,
                         Hv_LEFT,              hotrect->left+5,
                         Hv_TOP,               hotrect->top+325,
                         Hv_FILLCOLOR,         Hv_white,
                         Hv_OPTION,            (char *)" SE 1-6",
                         Hv_OPTION,            (char *)" SE 7-12",
                         Hv_SINGLECLICK,       SERadioClick,
                         NULL) ;

  Jti.tgboundbox1 = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BOXITEM,
                         Hv_BALLOON,           (char *)"Tools for Trigger creation.",
                         Hv_DOMAIN,            Hv_OUTSIDEHOTRECT,
                         Hv_LEFT,              hotrect->left+360,
                         Hv_TOP,               hotrect->top-5,
                         Hv_WIDTH,             View->local->width - View->hotrect->width - 140,
                         Hv_HEIGHT,            (View->local->height/2) - 56,
                         NULL) ;


  Jti.bAnd1 = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_PARENT,            Jti.tgboundbox1,
                         Hv_LABEL,             "And",
                         Hv_LEFT,              Jti.tgboundbox1->area->left+20,
                         Hv_TOP,               Jti.tgboundbox1->area->top+10,
                         Hv_USER1,             1,  /* mark it as the top set of controls */
                         Hv_WIDTH,             30,
                         Hv_HEIGHT,            18,
                         Hv_SINGLECLICK,       trigAndButClick,
                         NULL) ;

  Jti.bOr1 = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_PARENT,            Jti.tgboundbox1,
                         Hv_LABEL,             "Or",
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_USER1,             1,  /* mark it as the top set of controls */
                         Hv_PLACEMENTGAP,      10,
                         Hv_WIDTH,             30,
                         Hv_HEIGHT,            18,
                         Hv_SINGLECLICK,       trigOrButClick,
                         NULL) ;

  Jti.bNot1 = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_PARENT,            Jti.tgboundbox1,
                         Hv_LABEL,             "Not",
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_USER1,             1,  /* mark it as the top set of controls */
                         Hv_PLACEMENTGAP,      10,
                         Hv_WIDTH,             30,
                         Hv_HEIGHT,            18,
                         Hv_SINGLECLICK,       trigNotButClick,
                         NULL) ;

  Jti.bClr1 = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_PARENT,            Jti.tgboundbox1,
                         Hv_LABEL,             "Clr",
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_USER1,             1,  /* mark it as the top set of controls */
                         Hv_PLACEMENTGAP,      10,
                         Hv_WIDTH,             30,
                         Hv_HEIGHT,            18,
                         Hv_SINGLECLICK,       trigClrButClick,
                         NULL) ;

  Hv_InvertColorScheme (Jti.bAnd1->scheme) ;
  Hv_DrawItem (Jti.bAnd1, NULL) ;




  /*
    Hv_VaCreateView(&View,
		    Hv_TAG,            Hv_XYPLOTVIEW,
		    Hv_DRAWCONTROL,    Hv_ZOOMONRESIZE +Hv_ZOOMABLE
		    + Hv_ENLARGEABLE + Hv_SCROLLABLE,
		    Hv_POPUPCONTROL,   Hv_PRINTVIEW + Hv_SENDBACKVIEW +
		    + Hv_ZOOMRESIZETOGGLE + Hv_REFRESHVIEW + Hv_UNDOVIEW,
		    Hv_TITLE,          tchr,
		    Hv_INITIALIZE,     Hv_BasePlotSetup,
		    Hv_LEFT,           left,
		    Hv_TOP,            top,
		    Hv_HOTRECTWIDTH,   width,
		    Hv_HOTRECTHEIGHT,  height,
		    Hv_HOTRECTCOLOR,   Hv_white, 
		    Hv_HOTRECTRMARGIN, hrrmargin,
		    Hv_MOTIONLESSFB,   True,
		    Hv_MOTIONLESSFBINTERVAL, 1000,
            NULL);

   */
  
  /*
  {
    static Hv_Widget evdialog = NULL;
    static Hv_Widget evtext, rowcol;

    if(!evdialog)
    {
	  Hv_VaCreateDialog(&evdialog,
	 		  Hv_TYPE,      Hv_MODELESS,
	 		  Hv_TITLE,     " Event Control ",
	 		  NULL);
      rowcol = Hv_TightDialogColumn(evdialog, 0);
	  evtext = Hv_VaCreateDialogItem(rowcol,
		 		       Hv_TYPE,        Hv_SCROLLEDTEXTDIALOGITEM,
				       Hv_NUMROWS,     18,
				       Hv_BACKGROUND,  Hv_aliceBlue,
				       Hv_TEXTCOLOR,   Hv_black,
				       Hv_NUMCOLUMNS,  101,
				       NULL);
    }
  }
  */

  Jti.titem[0] = Hv_VaCreateItem(View,
                    Hv_TYPE, Hv_TEXTENTRYITEM,
                    Hv_DOMAIN,        Hv_OUTSIDEHOTRECT,
                    Hv_PARENT,        Jti.tgboundbox1,
                    Hv_LEFT,          Jti.tgboundbox1->area->left+5,
                    Hv_TOP,           Jti.tgboundbox1->area->top+35,
                    Hv_WIDTH,         Jti.tgboundbox1->area->width - 10,
                    Hv_HEIGHT,        Jti.tgboundbox1->area->height - 195,
                    Hv_DRAWCONTROL,   Hv_FRAMEAREA + Hv_DONTAUTOSIZE,
              Hv_USER1,         0,
              Hv_EDITABLE,      True,
              Hv_TEXTENTRYMODE, Hv_MULTILINEEDIT,
              Hv_SINGLECLICK,   TRIG_SingleClick,
                    NULL);

  Jti.titem[1] = Hv_VaCreateItem(View,
                         Hv_TYPE,          Hv_TEXTENTRYITEM,
                         Hv_DOMAIN,        Hv_OUTSIDEHOTRECT, 
                         Hv_PARENT,        Jti.tgboundbox1,
                         Hv_LEFT,          Jti.tgboundbox1->area->left+5,
                         Hv_TOP,           Jti.tgboundbox1->area->top+190,
                         Hv_WIDTH,         Jti.tgboundbox1->area->width - 10,
                         Hv_HEIGHT,        Jti.tgboundbox1->area->height - 205,
                         Hv_DRAWCONTROL,   Hv_FRAMEAREA + Hv_DONTAUTOSIZE,
			 Hv_USER1,         1,
			 Hv_EDITABLE,      True,
			 Hv_TEXTENTRYMODE, Hv_MULTILINEEDIT,
			 Hv_SINGLECLICK,   TRIG_SingleClick,
                         NULL) ;


  Jti.tgboundbox2 = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BOXITEM,
                         Hv_DOMAIN,            Hv_OUTSIDEHOTRECT,
                         Hv_LEFT,              hotrect->left+360,
                         Hv_TOP,               hotrect->top+360,
                         Hv_WIDTH,             View->local->width - View->hotrect->width - 140,
                         Hv_HEIGHT,            (View->local->height/2)-56,
                         NULL) ;


  Jti.bAnd2 = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_PARENT,            Jti.tgboundbox2,
                         Hv_LABEL,             "And",
                         Hv_LEFT,              Jti.tgboundbox2->area->left+20,
                         Hv_TOP,               Jti.tgboundbox2->area->top+10,
                         Hv_WIDTH,             30,
                         Hv_USER1,             3,   
                         Hv_HEIGHT,            18,
                         Hv_SINGLECLICK,       trigAndButClick,
                         NULL) ;

  Jti.bOr2 = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_PARENT,            Jti.tgboundbox2,
                         Hv_LABEL,             "Or",
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_USER1,             3,  
                         Hv_PLACEMENTGAP,      10,
                         Hv_WIDTH,             30,
                         Hv_HEIGHT,            18,
                         Hv_SINGLECLICK,       trigOrButClick,
                         NULL) ;

  Jti.bNot2 = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_PARENT,            Jti.tgboundbox2,
                         Hv_LABEL,             "Not",
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_USER1,             3,  
                         Hv_PLACEMENTGAP,      10,
                         Hv_WIDTH,             30,
                         Hv_HEIGHT,            18,
                         Hv_SINGLECLICK,       trigNotButClick,
                         NULL) ;

  Jti.bClr2 = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_PARENT,            Jti.tgboundbox2,
                         Hv_LABEL,             "Clr",
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_USER1,             3,  
                         Hv_PLACEMENTGAP,      10,
                         Hv_WIDTH,             30,
                         Hv_HEIGHT,            18,
                         Hv_SINGLECLICK,       trigClrButClick,
                         NULL) ;


  Hv_InvertColorScheme (Jti.bAnd2->scheme) ;
  Hv_DrawItem (Jti.bAnd2, NULL) ;


  Jti.titem[2] = Hv_VaCreateItem (View,
                         Hv_TYPE,          Hv_TEXTENTRYITEM,
                         Hv_DOMAIN,        Hv_OUTSIDEHOTRECT, 
                         Hv_PARENT,        Jti.tgboundbox2,
                         Hv_LEFT,          Jti.tgboundbox2->area->left+5,
                         Hv_TOP,           Jti.tgboundbox2->area->top+35,
                         Hv_WIDTH,         Jti.tgboundbox2->area->width-10,
                         Hv_HEIGHT,        (Jti.tgboundbox2->area->height - 195),
                         Hv_DRAWCONTROL,   Hv_FRAMEAREA + Hv_DONTAUTOSIZE,
			 Hv_USER1,         2,
			 Hv_EDITABLE,      True,
			 Hv_TEXTENTRYMODE, Hv_MULTILINEEDIT,
			 Hv_SINGLECLICK,   TRIG_SingleClick,
                         NULL) ;


  Jti.titem[3] = Hv_VaCreateItem (View,
                         Hv_TYPE,          Hv_TEXTENTRYITEM,
                         Hv_DOMAIN,        Hv_OUTSIDEHOTRECT, 
                         Hv_PARENT,        Jti.tgboundbox2,
                         Hv_LEFT,          Jti.tgboundbox2->area->left+5,
                         Hv_TOP,           Jti.tgboundbox2->area->top+190,
                         Hv_WIDTH,         Jti.tgboundbox2->area->width-10,
                         Hv_HEIGHT,        (Jti.tgboundbox2->area->height - 205),
                         Hv_DRAWCONTROL,   Hv_FRAMEAREA + Hv_DONTAUTOSIZE,
			 Hv_USER1,         3,
			 Hv_EDITABLE,      True,
			 Hv_TEXTENTRYMODE, Hv_MULTILINEEDIT,
			 Hv_SINGLECLICK,   TRIG_SingleClick,
                         NULL) ;



}

/*----------- DrawBeamlineItem -----------*/

static void
DrawBeamlineItem(Hv_Item Item, Hv_Region region)
{
  Hv_View         View = Item->view;
  short           xc, yc;
  Hv_Rect        *hr;
  Hv_Point        points[8];
  ViewData        viewdata;

  viewdata = GetViewData(View);

  if (!(viewdata->displayBeamLine))
    return;

  FixBeamlineArea(Item);
  hr = View->hotrect;

  Hv_WorldToLocal(View, 0.0, 0.0, &xc, &yc);

  if ((yc > (hr->top - 10)) && (yc < (hr->bottom + 10))) {
    Hv_DrawLine(hr->left, yc-2, hr->right, yc-2, Hv_black);

    Hv_DrawLine(hr->left, yc-1, hr->right, yc-1, Hv_lavender);
    Hv_DrawLine(hr->left, yc,   hr->right, yc,   Hv_gray11);
    Hv_DrawLine(hr->left, yc+1, hr->right, yc+1, Hv_purple);
    Hv_DrawLine(hr->left, yc+2, hr->right, yc+2, Hv_black);
  }

/* now the nominal target indicator */

    points [0].x = xc-2;        points[0].y = yc-5;
    points [1].x = xc+2;        points[1].y = yc-5;
    points [2].x = xc+5;        points[2].y = yc-2;
    points [3].x = xc+5;        points[3].y = yc+2;
    points [4].x = xc+2;        points[4].y = yc+5;
    points [5].x = xc-2;        points[5].y = yc+5;  
    points [6].x = xc-5;        points[6].y = yc+2;
    points [7].x = xc-5;        points[7].y = yc-2;  
      
    Hv_FillPolygon(points, 8, True, Hv_purple, Hv_black);
    
    Hv_DrawLine(xc, yc+4, xc, yc-4, Hv_white);
    Hv_DrawLine(xc-4, yc, xc+4, yc, Hv_white);

}

/*--------- FixBeamlineArea -----------*/

static void
FixBeamlineArea(Hv_Item Item)
{
  Hv_View         View = Item->view;
  short           xc, yc;
  Hv_Rect        *hr;


  hr = View->hotrect;
  Hv_WorldToLocal(View, 0.0, 0.0, &xc, &yc);
  Hv_SetRect(Item->area, hr->left, yc - 6, hr->width, 12);
  Hv_LocalRectToWorldRect(View, Item->area, Item->worldarea);
}

#else /* UNIX only */

void
setup_dummy()
{
}

#endif
