h62484
s 00003/00002/01051
d D 1.3 07/11/12 16:41:07 heddle 4 3
c new start counter
e
s 00005/00005/01048
d D 1.2 02/09/25 16:11:18 boiarino 3 1
c Trig Filter -> Trigger Bits
e
s 00000/00000/00000
d R 1.2 02/09/09 16:27:14 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 ced/setup.c
e
s 01053/00000/00000
d D 1.1 02/09/09 16:27:13 boiarino 1 0
c date and time created 02/09/09 16:27:13 by boiarino
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
  -  File:    setup.c	
  -							
  -  Summary:						
  -           sets up ced views
  -					       
  -                     				
  -  Revision history:				
  -                     				
  -  Date       Programmer     Comments		
  ------------------------------------------------------
  -  10/19/94	DPH		Initial Version	
  -                                              	
  ------------------------------------------------------
  */

#include "ced.h"
#include "feedback.h"

#define  BUTTONWIDTH       40
#define  BIGBUTTONWIDTH    64
#define  BUTTONHEIGHT      28
#define  BIGBUTTONHEIGHT   20


int calfeedbackWidth;

/*------ local prototypes ---------*/

static void FinalInit(Hv_View View);

static void CommonItems(Hv_View View);

static void ViewSetup(Hv_View  View);

static void InitViewData(Hv_View View);

static void MyDrawView(Hv_View View,
		       Hv_Region region);

static void ViewCustomize(Hv_View View);

static void CustomColors(Hv_Widget w);

static void AddMessageBox(Hv_View View);

static Hv_Item Camera;

extern short optionarmcolor;
extern short radioarmcolor;
extern short boxcolor;
extern short fbcolor;

/* feedback strings */

char      *fb_text1   = "      sector: ";
char      *fb_text2   = "       event: ";
char      *fb_text3   = " anchor dist: ";
char      *fb_text4   = "    (beam) Z: ";
char      *fb_text5   = "           X: ";
char      *fb_text6   = "           Y: ";
char      *fb_text7   = "           R: ";
char      *fb_text8   = "       theta: ";
char      *fb_text9   = "         phi: ";
char      *fb_text10  = "   B (Tesla): ";
char      *fb_text11  = "(Bx, By, Bz): ";
char      *fb_text12  = "Scintillator: ";
char      *fb_text13  = "  Superlayer: ";
char      *fb_text14  = "    DC Layer: ";
char      *fb_text15  = "        Wire: ";
char      *fb_text16  = "Supl Occpncy: ";
char      *fb_text17  = "    Cerenkov: ";
char      *fb_text18  = "      Shower: ";
char      *fb_text18b = "       Plane: ";
char      *fb_text18c =  "    Tagger E: ";
char      *fb_text19  = "       Strip: ";
char      *fb_text19b = "     [u,v,w]: ";
char      *fb_text19c = "[long,short]: ";
char      *fb_text19d =  "    Tagger T: ";
char      *fb_text20  = " TDC  (left): ";
char      *fb_text21  = " ADC  (left): ";
D 4
char      *fb_text20b = "         TDC: ";
char      *fb_text21b = "         ADC: ";
E 4
char      *fb_text22  = " TDC (right): ";
char      *fb_text23  = " ADC (right): ";
char      *fb_text24  = "      Energy: ";
char      *fb_text25  = "    Pixel ID: ";
char      *fb_text26a = "   X TDC (L): ";
char      *fb_text27a = "   X ADC (L): ";
char      *fb_text28a = "   Y TDC (L): ";
char      *fb_text29a = "   Y ADC (L): ";
char      *fb_text26b = "   X TDC (R): ";
char      *fb_text27b = "   X ADC (R): ";
char      *fb_text28b = "   Y TDC (R): ";
char      *fb_text29b = "   Y ADC (R): ";
char      *fb_text26  = " U strip TDC: ";
char      *fb_text27  = " U strip ADC: ";
char      *fb_text28  = " V strip TDC: ";
char      *fb_text29  = " V strip ADC: ";
char      *fb_text30  = " W strip TDC: ";
char      *fb_text31  = " W strip ADC: ";
char      *fb_text32  = " midplane coords";

char      *fb_text33  =  "    track ID: ";
char      *fb_text34  =  "       chisq: ";
char      *fb_text35  =  "    momentum: ";
char      *fb_text36  =  "      vertex: ";
char      *fb_text37  =  "  direct cos: ";
char      *fb_text38  =  "      charge: ";
char      *fb_text39  =  "   ij coords: ";
char      *fb_text40  =  "Accum Counts: ";
char      *fb_text41  =  "     avg TDC: ";

char      *fb_text42  =  "Event Source: ";

char      *fb_text43  =  "    Hot Wire: ";
char      *fb_text44  =  "  Run Number: ";

D 3
char      *fb_text53  =  " Trig Filter: ";
E 3
I 3
char      *fb_text53  =  "Trigger Bits: ";
I 4
char      *fb_text55  =  "   Start Cnt: ";
char      *fb_text56  =  "         TDC: ";
char      *fb_text57  =  "         ADC: ";
E 4
E 3

static    Hv_Widget       bglabel;
short                     bgcolor;
Boolean                   colorchanged;



/*------- GetViewData --------*/

ViewData GetViewData(Hv_View View)
    
{
    return  (ViewData)Hv_GetViewData(View);
}

#define CALORIMETERSIZE 980.0
#define SCINTVIEWSIZE 1500.0

/* need extra space for italial LAC in sects 1 & 2 */

/*------- NewView ----------*/

Hv_View NewView(short tag)
    
{
    static  short    sectcnt = 0;
    static  short    feccnt = 0;
    static  short    alldccnt = 0;
    static  short    scintcnt = 0;

    short   hrlmargin;
    short   hrtmargin;
    short   hrrmargin;
    short   hrbmargin;

    char             tchr[60];
    Hv_View          view;
    static short     viewcnt = 0;
    short            left, top, width, height;
    float            xmin, xmax, ymin, ymax; 
    short            hrcolor;
    ViewData         viewdata;
    Hv_FunctionPtr   VC;

    calfeedbackWidth = 255;
    
    left = 12 + 10*(viewcnt % 10);
    top = left + 30;

    hrlmargin = 35;
    hrtmargin = 5;
    hrrmargin = 15;
    hrbmargin = 15;
    
    viewcnt++;
    
    switch (tag) {
    case SECTOR:
	sectcnt++;
	sprintf(tchr, "Sector  %d", sectcnt);
	xmin = -0.97*400.0;
	xmax =  0.97*650.0;
	ymin = -0.97*560.0;
	ymax =  0.97*560.0;
	width = 700;
	hrcolor = Hv_powderBlue+4;
	VC = ViewCustomize;
	break;
	
    case ALLDC:
	alldccnt++;
	sprintf(tchr, "Drift Chambers  %d", alldccnt);
	xmin = -ALLDCWWIDTH2;
	xmax =  ALLDCWWIDTH2;
	ymin = -ALLDCWHEIGHT2;
	ymax =  ALLDCWHEIGHT2;
	VC = ViewCustomize;
	hrcolor = Hv_gray10;
	width = 740;
	break;
	
    case SCINT:
	scintcnt++;
	sprintf(tchr, "Scintillators & Start Counter  %d", scintcnt);
	xmin = -SCINTVIEWSIZE;
	xmax =  SCINTVIEWSIZE;
	ymin = -SCINTVIEWSIZE;
	ymax =  SCINTVIEWSIZE;
	width = 720;
	hrcolor = Hv_wheat;
	VC = ViewCustomize;
	break;
	
    case CALORIMETER:
	feccnt++;
	sprintf(tchr, "Calorimeters  %d", feccnt);
	xmin = -CALORIMETERSIZE;
	xmax =  CALORIMETERSIZE;
	ymin = -CALORIMETERSIZE;
	ymax =  CALORIMETERSIZE;
	width = 700;
	hrcolor = Hv_gray13;
	hrlmargin = calfeedbackWidth + 13;
	hrrmargin = 40;
	VC = ViewCustomize;
	break;
	
    default:
	fprintf(stderr, "Unrecognized view type in NewView. Exiting.\n");
	exit(-1);
	
    }
    
    height =  (short)((width*(ymax-ymin))/(xmax-xmin));
    viewdata = (ViewData)Hv_Malloc(sizeof(ViewDataRec));
    
    
    Hv_VaCreateView(&view, 
		    Hv_TAG,               tag,
		    Hv_DATA,              viewdata,
		    Hv_TITLE,             tchr,
		    Hv_DRAWCONTROL,       Hv_STANDARDDRAWCONTROL+Hv_SAVEBACKGROUND+Hv_MAGNIFY,
		    Hv_POPUPCONTROL,      Hv_STANDARDPOPUPCONTROL,
		    Hv_INITIALIZE,        ViewSetup,
		    Hv_CUSTOMIZE,         VC, 
		    Hv_FEEDBACK,          ViewFeedback,
		    Hv_LEFT,              left,
		    Hv_TOP,               top,
		    Hv_XMIN,              xmin,
		    Hv_XMAX,              xmax,
		    Hv_YMIN,              ymin,
		    Hv_YMAX,              ymax,
		    Hv_HOTRECTWIDTH,      width,
		    Hv_HOTRECTHEIGHT,     height,
		    Hv_HOTRECTCOLOR,      hrcolor,
		    Hv_USERDRAW,          MyDrawView,
		    Hv_HOTRECTLMARGIN,    hrlmargin,
		    Hv_HOTRECTTMARGIN,    hrtmargin,
		    Hv_HOTRECTRMARGIN,    hrrmargin,
		    Hv_HOTRECTBMARGIN,    hrbmargin,
		    Hv_MOTIONLESSFB,      True,
		    Hv_VIEWFINALINIT,     FinalInit,
		    NULL);
    
    
    return view;
}



/* ---------- FinalInit ----------*/

static void FinalInit(Hv_View View) {

/*
 *  Last thing call befor the view is completely ready.
 *  Here is where we should do any pre zooming.
 */

    float xmin, xmax, ymin, ymax;

    if (View->tag == CALORIMETER) {
	
	xmin = -0.90*CALORIMETERSIZE;
	ymin = -0.67*CALORIMETERSIZE;

	xmax =  0.70*CALORIMETERSIZE;
	ymax =  0.93*CALORIMETERSIZE;

	Hv_QuickZoomView(View, xmin, xmax, ymin, ymax);
    }

}


/* ------- SetSelectability -------*/

void SetSelectability(Hv_View View)
    
{
    ViewData   viewdata;
    
    viewdata = GetViewData(View);
    
}


/*------- UpdateViewTitle -------*/

void UpdateViewTitle(Hv_View View)
    
{
    char       *oldvtitle;
    char       *vtitle;
    char       sectstr[80];
    ViewData   viewdata;
    char       *teststr;
    short      vs, vsm;
    
    
    if (View->tag != SECTOR)
	return;
    
    viewdata = GetViewData(View);
    vs = viewdata->visiblesectors;
    
    if ((vs > 0) && (vs < 99))
	sprintf(sectstr, "[%1d,%1d]", vs, vs+3);
    else if (vs > 99) {
	vsm = vs - 100;
	sprintf(sectstr, "[%1d,%1d] (Largest EC ADC Sum)", vsm, vsm+3);
    }
    else
	sprintf(sectstr, "[%1d,%1d] (Most Data)", -vs, (-vs)+3);
    
    
    
    oldvtitle = View->containeritem->str->text;
    teststr = strstr(oldvtitle, "[");
    if (teststr == NULL)
	return;
    
    *teststr = '\0';
    
    vtitle = Hv_Malloc(strlen(oldvtitle) + 80);
    strcpy(vtitle, oldvtitle);
    strcat(vtitle, sectstr);
    Hv_SetViewTitle(View, vtitle);
    Hv_SetMenuItemString(View->menu, vtitle, Hv_DEFAULT);
    Hv_Free(vtitle);

    if (Hv_ViewIsVisible(View)) {
	Hv_DrawViewTitleBorder(View, True, True);
	Hv_DrawViewTitle(View);
    }
}

/*------ CommonItems -------*/

static void CommonItems(Hv_View View)
    
{
    Hv_Item         Item;
    ViewData        viewdata;
    
    viewdata = GetViewData(View);  
    
/* better to add nonhotrect items first */

/* calorimeter view gets tools on right side */

    if (View->tag == CALORIMETER)

	Hv_StandardTools(View, View->hotrect->right + 5, View->hotrect->top - 4,
			 Hv_VERTICAL, Hv_SHIFTXONRESIZE,
			 Hv_ALLTOOLS, &Camera, &Item);
    
    else 
	Hv_StandardTools(View, View->hotrect->left - 35, View->hotrect->top - 4,
			 Hv_VERTICAL, Hv_DONOTHINGONRESIZE,
			 Hv_ALLTOOLS, &Camera, &Item);
    
    
/* only sector view gets an anchor */
    
    if (View->tag == SECTOR) {
	Item = Hv_VaCreateItem(View,
			       Hv_TYPE,                Hv_BUTTONITEM,
			       Hv_RESIZEPOLICY,        Hv_DONOTHINGONRESIZE,
			       Hv_SINGLECLICK,         AnchorButtonCallback,
			       Hv_USERDRAW,            DrawAnchorButton,
			       Hv_RELATIVEPLACEMENT,   Hv_POSITIONBELOW,
			       Hv_BALLOON,             "Use this to place an anchor.",
			       NULL);
    }
}


/* -------- ViewSetup ----------*/

static void ViewSetup(Hv_View View)
    
{
    
/* initialize the views private data */
    
    InitViewData(View);
    CommonItems(View);
    
    switch (View->tag) {
	
    case SECTOR:
	SectorViewSetup(View);
	AddMessageBox(View);
	break;
	
    case CALORIMETER:
	CalorimeterViewSetup(View);
	AddMessageBox(View);
	break;
	
    case ALLDC:
	AllDCViewSetup(View);
	AddMessageBox(View);
	break;
	
    case SCINT:
	ScintViewSetup(View);
	AddMessageBox(View);
	break;
	
    }
    
    SetSelectability(View);
}

/* ----- InitViewData --------*/

static void InitViewData(Hv_View View)
    
{
    ViewData        viewdata;
    static short    vissect = -1;
    char            *oldvtitle, *vtitle, sectstr[7];
    int             i, j;
    
    if (View->tag == SECTOR)
	vissect++;
    
    viewdata = GetViewData(View);
    
/* set the Items to NULL */

    viewdata->TaggerItem = NULL;
    viewdata->BeamLineItem = NULL;
    viewdata->MessageBoxItem = NULL;
    viewdata->AnchorItem = NULL;


    for (i = 0; i < 10; i++) {
	viewdata->ColorButtons[i] = NULL;
    }

    
    for (i = 0; i < 3; i++)
	viewdata->AccumulatedHitsItem[i] = NULL;
    viewdata->ADCRainbowItem = NULL;

    for (i = 0; i < 3; i++)
	viewdata->AverageTDCItem[i] = NULL;

    viewdata->FieldRainbowItem = NULL;
    viewdata->InnerADCRainbowItem = NULL;
    viewdata->OuterADCRainbowItem = NULL;
    viewdata->EnergyRainbowItem = NULL;
    viewdata->FieldmapItem = NULL;
    viewdata->FieldDisplayItem = NULL;
    viewdata->SectorHexItem = NULL;
    viewdata->SectorBoxItem = NULL;
    
    for (i = 0; i < NUM_CERENK2; i++)
	viewdata->CerenkovItems[i] = NULL;
    
    for (i = 0; i < NUM_SCINTPLANE2; i++)
	viewdata->ScintPlaneItems[i] = NULL;
    
    for (i = 0; i < NUM_SECT; i++)
	viewdata->ForwardECViewECItems[i] = NULL;
    
    for (i = 0; i < NUM_SECT; i++)
	viewdata->FannedScintItems[i] = NULL;
    
    for (i = 0; i < NUM_SHOWER2; i++)
	viewdata->ForwardECSectViewItems[i] = NULL;

    for (i = 0; i < NUM_SECT; i++)
	viewdata->LAECItems[i] = NULL;
    
    for (i = 0; i < 2; i++)
	viewdata->SectViewEC1Items[i] = NULL;
    
    for (i = 0; i < NUM_SUPERLAYER2; i++)
	viewdata->DCItems[i] = NULL;
    
    for (i = 0; i < 6; i++)
	for (j = 0; j < NUM_SUPERLAYER; j++)
	    viewdata->AllDCItems[i][j] = NULL;
    
/* colors */
    
    viewdata->cerenkovframecolor = Hv_black;
    viewdata->cerenkovlofillcolor = Hv_gray15;
    viewdata->cerenkovhifillcolor = Hv_gray12;
    viewdata->cerenkovhitcolor = Hv_red;
    

    viewdata->fannedeccolor = Hv_blue-8;

    
/* different bg color for each plane */
    
    viewdata->fannedecbgcolor[ECINNER] = Hv_skyBlue-3;
    viewdata->fannedecbgcolor[ECOUTER] = Hv_skyBlue-6;
    viewdata->fannedecbgcolor[ECBOTH] = Hv_gray5;
    
    viewdata->fannedecstripcolor = Hv_skyBlue+3;
    viewdata->fannedechitcolor = Hv_honeydew;
    

/* for fanned scint view */

    viewdata->fannedscintcolor = Hv_blue-8;
    viewdata->fannedscintbgcolor = Hv_gray7;
    viewdata->fannedscintstripcolor = Hv_gray6;
    viewdata->fannedscinthitcolor = Hv_honeydew;
    
/* display controls */
    
    viewdata->displayDCA      = True;
    viewdata->displayTagger   = False;
    viewdata->displayEvent    = True;
    viewdata->displayBeamLine = True;
    viewdata->displayField    = False;
    viewdata->displayDC = True;
    viewdata->displayEC = True;
    viewdata->displaySC = True;
    viewdata->displayCC = True;
    viewdata->displayU = False;
    viewdata->displayV = False;
    viewdata->displayW = False;
    viewdata->displayX = False;
    viewdata->displayY = False;
    viewdata->suppressTDC = False;
    viewdata->suppressNoise = False;

    viewdata->displayDC0 = True;
    viewdata->displayDC1 = False;
    viewdata->dc1control = JUSTTRACKS;

    viewdata->displayHBT = True;
    viewdata->displayTBT = True;

    viewdata->displayHex = True;
    viewdata->displayDead = ((View->tag == SECTOR) || (View->tag == ALLDC));
    viewdata->showwhat = SINGLEEVENT;  /* default: display single evet */
    viewdata->algorithm = 1;
    
    viewdata->ecplane = ECINNER;   /* relevant for EC Views */

    viewdata->hb_color = Hv_red;
    viewdata->tb_color = Hv_blue;


/* some vars relevant for accumulated histos in cal view */

    viewdata->ecshowwhat  = USTRIP;
    viewdata->ec1showwhat = XSTRIP;
    
/* note that visiblesectors below is NOT a C index. A negative
   value means that this is really a "most data sector" view */
    
    viewdata->visiblesectors = (vissect % 3) + 1;  /* relevant for sector views only */
    
    if (View->tag == SECTOR) {
	sprintf(sectstr, " [%1d,%1d]", viewdata->visiblesectors, viewdata->visiblesectors+3);
	
/* make the view title show the sectors being sisplayed */
	
	oldvtitle = View->containeritem->str->text;
	vtitle = Hv_Malloc(strlen(oldvtitle) + 10);
	strcpy(vtitle, oldvtitle);
	strcat(vtitle, sectstr);
	Hv_SetViewTitle(View, vtitle);
	Hv_SetMenuItemString(View->menu, vtitle, Hv_DEFAULT);
	Hv_Free(vtitle);
	
	viewdata->phi = Hv_DEGTORAD*60.0*(viewdata->visiblesectors - 1);
	viewdata->dphi = 0;
    }
    
    View->data = (void *)viewdata;
}


/* ------ StandardOptionButton ------*/

Hv_Item StandardOptionButton(Hv_View View,
			     Hv_Item parent,
			     Boolean *control,
			     char *label,
			     short gap,
			     short font)
{
    Hv_Item         Item;
    
    Item = Hv_VaCreateItem(View,
			   Hv_PARENT,             parent,
			   Hv_TYPE,               Hv_OPTIONBUTTONITEM,
			   Hv_CONTROLVAR,         (void *)control,
			   Hv_TEXT,               label,
			   Hv_FONT,               font,
			   Hv_ARMCOLOR,           optionarmcolor,
			   Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			   Hv_SINGLECLICK,        OptionButtonCallback,
			   Hv_PLACEMENTGAP,       gap,
			   NULL);
    
    return  Item;
}

/* ------- OptionButtonCallback ------*/

void OptionButtonCallback(Hv_Event hvevent)
    
{
    Hv_Region  hrrgn = NULL;
    Hv_Region  crgn = NULL;
    Hv_Region  sxrgn = NULL;
    Hv_View    View = hvevent->view;
    ViewData   viewdata;
    
    
    viewdata = GetViewData(View);
    
    Hv_OptionButtonCallback(hvevent);
    hrrgn = Hv_ClipHotRect(View, 1);
    Hv_CopyRegion(&crgn, hrrgn);
    
    Hv_SetViewDirty(View);
    Hv_SaveViewBackground(View, crgn);
    
    if (View->tag == CALORIMETER) {
	Hv_CopyRegion(&sxrgn, viewdata->SectorHexItem->region);
	Hv_ShrinkRegion(sxrgn, -2, -2);
	Hv_SubtractRegionFromRegion(&crgn, sxrgn); 
    }
    
    Hv_DrawControlItems(View, crgn);
    Hv_DrawHotRectItems(View, hrrgn);
    if (View->userdraw != NULL)
	View->userdraw(View, hrrgn);
    
/*   Hv_DrawView(View, crgn);*/
    UpdateMessageBox(View, " ");
    Hv_DestroyRegion(crgn);
    Hv_DestroyRegion(hrrgn);
    Hv_DestroyRegion(sxrgn);
}


/* ------- ShowWhatCB -------*/

void ShowWhatCB(Hv_Event hvevent)
    
/*---------------------------------------
  The use has toggled between show single event
  and show accumulated event
  ------------------------------------------*/
    
{
    Hv_Item              Item = hvevent->item;
    Hv_View              View = hvevent->view;
    ViewData             viewdata;
    int                  choice;
    Hv_Region            hrreg = NULL;
    
    viewdata = GetViewData(View);
    choice = Hv_HandleRadioButton(Item); 
    
    if (viewdata->showwhat == choice)
	return;
    
    viewdata->showwhat = choice;
    
    hrreg = Hv_ClipHotRect(View, 1);
    EraseViewEvent(View);
    Hv_SaveViewBackground(View, hrreg);
    DrawViewEvent(View, hrreg);
    Hv_DestroyRegion(hrreg);
}


/* ------- AlgorithmCB -------*/

void AlgorithmCB(Hv_Event hvevent)
    
/*---------------------------------------
  The use has toggled between show single event
  and show accumulated event
  ------------------------------------------*/
    
{
    Hv_Item              Item = hvevent->item;
    Hv_View              View = hvevent->view;
    ViewData             viewdata;
    int                  choice;
    Hv_Region            hrreg = NULL;
    
    viewdata = GetViewData(View);
    choice = Hv_HandleRadioButton(Item);
    
    if (viewdata->algorithm == choice)
	return;
    
    viewdata->algorithm = choice;
    fprintf(stderr, "toggled alg  ");
    
    if (viewdata->showwhat == SINGLEEVENT)
	return;
    
    hrreg = Hv_ClipHotRect(View, 1);
    EraseViewEvent(View);
    Hv_SaveViewBackground(View, hrreg);
    DrawViewEvent(View, hrreg);
    Hv_DestroyRegion(hrreg);
}


/*------ AddScintillatorItems -----*/

void   AddScintillatorItems(Hv_View View,
			    Hv_Item parent)
    
{
    ViewData     viewdata;
    short        i;
    int          vs;
    
    viewdata = GetViewData(View);
    
/* recall: vs < 0 for mostdata; 
 *         vs > 100 for ec sum
 *         vs is NOT a C index
*/
    
    vs = GoodSector(View);
    
    if (View->tag == SECTOR) {
	
/* set up one poly for each plane */
	
	for (i = 0; i < NUM_SCINTPLANE; i++)
	    viewdata->ScintPlaneItems[i] = MallocScintillator(View, vs, i);
	
	for (i = 0; i < NUM_SCINTPLANE; i++)
	    viewdata->ScintPlaneItems[i+4] = MallocScintillator(View, vs+3, i);
	
D 3
/* now all the scints have been malloced, attach the apropriate
E 3
I 3
/* now all the scints have been alloced, attach the apropriate
E 3
   sector geometry */
	
	SetScintGeometrySector(View);  
    }
    
/* all dc view */
    
    else if (View->tag == ALLDC)
	for (i = 0; i < 6; i++)
	    viewdata->ScintPlaneItems[i] = MallocAllDCScint(View, i);

    else if (View->tag == SCINT) {
	for (i = 0; i < NUM_SECT; i++)
	    viewdata->FannedScintItems[i] = MallocFannedScint(View, i, parent);
    }
	
}

/*------ AddCerenkovItems -----*/

void   AddCerenkovItems(Hv_View View)
    
{
    ViewData     viewdata;
    short        i;
    int          vs;
    
    viewdata = GetViewData(View);
    
/* recall: vs < 0 for mostdata; 
           vs > 100 for ec sum
           vs is NOT a C index */
    
    vs = GoodSector(View);
    
    if (View->tag == SECTOR) {
	for (i = 0; i < NUM_CERENK; i++)
	    viewdata->CerenkovItems[i] = MallocCerenkov(View, vs, i);
	
	for (i = 0; i < NUM_CERENK; i++)
	    viewdata->CerenkovItems[i+NUM_CERENK] = MallocCerenkov(View, vs+3, i);
	
D 3
/* now all the cerenkovs have been malloced, attach the apropriate
E 3
I 3
/* now all the cerenkovs have been alloced, attach the apropriate
E 3
   sector geometry */
	
	SetCerenkovGeometrySector(View);  
    }
}

/*------ AddDCItems -----*/

void   AddDCItems(Hv_View View)
    
{
    ViewData     viewdata;
    short        i, j;
    int          vs;
    
    viewdata = GetViewData(View);

/* recall: vs < 0 for mostdata; 
           vs > 100 for ec sum
           vs is NOT a C index */
    
    vs = GoodSector(View);
    
    if (View->tag == SECTOR) {
	for (i = 0; i < NUM_SUPERLAYER; i++)
	    viewdata->DCItems[i] = MallocDC(View, vs, i);
	
	for (i = 0; i < NUM_SUPERLAYER; i++)
	    viewdata->DCItems[i+NUM_SUPERLAYER] = MallocDC(View, vs+3, i);
	
D 3
/* now all the dcs have been malloced, attach the apropriate
E 3
I 3
/* now all the dcs have been alloced, attach the apropriate
E 3
   sector geometry */
	
	SetDCGeometrySector(View);  
    }
    
    else if (View->tag == ALLDC) {
	for (i = 0; i < 6; i++)
	    for (j = 0; j < NUM_SUPERLAYER; j++)
		viewdata->AllDCItems[i][j] = MallocAllDC(View, i, j);
    }
    
}

/*------ AddECItems -----*/

void AddECItems(Hv_View View,
		Hv_Item parent)
{
    ViewData     viewdata;
    short        i;
    int          vs;
    
    viewdata = GetViewData(View);
    
    switch (View->tag){
    case SECTOR:
	
	vs = GoodSector(View);
	
	for (i = 0; i < NUM_SHOWER; i++)
	    viewdata->ForwardECSectViewItems[i] = MallocShower(View, vs, i, NULL);
	
	for (i = 0; i < NUM_SHOWER; i++)
	    viewdata->ForwardECSectViewItems[i+2] = MallocShower(View, vs+3, i, NULL);

/* italian ec -- only in sectors 1 and 2*/	

	viewdata->SectViewEC1Items[0] = MallocEC1Shower(View, vs);
/*	viewdata->SectViewEC1Items[1] = MallocEC1Shower(View, vs+3); */
	
D 3
/* now all the sector view ec's have been malloced, attach the apropriate
E 3
I 3
/* now all the sector view ec's have been alloced, attach the apropriate
E 3
   sector geometry */
	
	SetShowerGeometrySector(View);  
	break;
	
    case CALORIMETER:
	for (i = 0; i < NUM_SECT; i++)
	    viewdata->ForwardECViewECItems[i] = MallocForwardEC(View, i, parent);

	for (i = 0; i < 2; i++)
	    viewdata->LAECItems[i] = MallocLAEC(View, i,  parent);
	break;
	
    }
}


/*------ StandardOptionButtonBox ------*/

Hv_Item StandardOptionButtonBox(Hv_View View,
				short w)
{
    Hv_Item     Box;
    
    
    Box = Hv_VaCreateItem(View,
			  Hv_TYPE,                Hv_BOXITEM,
			  Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT,
			  Hv_LEFT,                View->hotrect->right + 5,
			  Hv_TOP,                 View->hotrect->top - 4,
			  Hv_FILLCOLOR,           boxcolor,
			  Hv_PLACEMENTGAP,        2,
			  Hv_WIDTH,               w,
			  NULL);
    
    
    return Box;
}


/*------ MyDrawView ------*/

static void MyDrawView(Hv_View View,
		       Hv_Region region)
    
/* called after Hv has called its draw view routine */
    
{
    if (Hv_inPostscriptMode || (region == NULL) || Hv_RectInRegion(View->hotrect, region))
	DrawViewEvent(View, region);
}


/*------ StandardSeparator --------*/

Hv_Item StandardSeparator(Hv_View View,
			  Hv_Item parent,
			  int gap)
    
/* add a separator */
    
{
    Hv_Item   Item;
    
    Item = Hv_VaCreateItem(View,
			   Hv_PARENT,             parent,
			   Hv_TYPE,               Hv_SEPARATORITEM,
			   Hv_PLACEMENTGAP,       gap,
			   Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			   NULL);
    
    return Item;
}


/* -------- ViewCustomize --------*/

static void ViewCustomize(Hv_View View)
    
{
    static Hv_Widget       dialog = NULL;
    Hv_Widget              dummy, rowcol, rc;
    ViewData               viewdata;
    
    viewdata = GetViewData(View);  
    colorchanged = False;
    
    if (!dialog) {
	Hv_VaCreateDialog(&dialog, Hv_TITLE, "View Customize ", NULL);
	rowcol = Hv_DialogColumn(dialog, 3);
	rc = Hv_DialogRow(rowcol, 10);
	bglabel  = Hv_SimpleColorLabel(rc,      " Background ",
				       (Hv_FunctionPtr)CustomColors);
	dummy = Hv_StandardActionButtons(rowcol, 80, Hv_OKBUTTON + Hv_APPLYBUTTON);
    }
    
    bgcolor = View->hotrectborderitem->color;
    Hv_ChangeLabelColors(bglabel, -1, bgcolor);
    
    while (Hv_DoDialog(dialog, NULL) > 0) {
	
	if (colorchanged) {
	    View->hotrectborderitem->color = bgcolor;
	    Hv_SetViewDirty(View);
	    Hv_DrawViewHotRect(View);
	}
	
	if (Hv_GetDialogAnswer(dialog) != Hv_APPLY)
	    break;
    }
    
}

/* ------ CustomColors --- */

static void CustomColors(Hv_Widget w)
    
{
    if (w == bglabel)
	Hv_ModifyColor(w, &bgcolor, "Edit Background Color", False);
    
    colorchanged = True;
}



/*------ AddMessageBox -----*/

static void AddMessageBox(Hv_View View)
    
{
    ViewData    viewdata;
    Hv_Item     Item;
    Hv_Rect     newlocal;
    Hv_Rect     *hr;
    
    viewdata = GetViewData(View);
    hr = View->hotrect;
    
    
    Item = Hv_VaCreateItem(View,
			   Hv_TYPE,                Hv_BOXITEM,
			   Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT,
			   Hv_LEFT,                hr->left - 4,
			   Hv_TOP,                 hr->bottom + 8,            
			   Hv_HEIGHT,              27,
			   Hv_WIDTH,               550,
			   Hv_FONT,                Hv_helveticaBold14,
			   Hv_RESIZEPOLICY,        Hv_SHIFTYONRESIZE,
			   Hv_FILLCOLOR,           Hv_honeydew,
			   Hv_TEXT,                "message area",
			   NULL);
    
    viewdata->MessageBoxItem = Item;
    
    
/* fix the local system to fit */
    
    Hv_CopyRect(&newlocal, View->local);
    newlocal.bottom = Item->area->bottom
	+ 10 + View->containeritem->scheme->borderwidth;
    Hv_FixRectWH(&newlocal);
    Hv_ChangeViewLocal(View, &newlocal);
    
}




E 1
