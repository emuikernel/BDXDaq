/*
  ----------------------------------------------------
  -							
  -  File:    calview.c	
  -							
  -  Summary:						
  -           sets up calorimeter view
  -					       
  -                     				
  -  Revision history:				
  -                     				
  -  Date       Programmer     Comments		
  ------------------------------------------------------
  -  10/2/97	DPH		Initial Version	
  -                                              	
  ------------------------------------------------------
  */

#include "ced.h"
#include "feedback.h"

/* for use in shared  feedbacks */

#define   EC_SLOT          0
#define   LAC_SLOT         1

extern int calfeedbackWidth;

static Hv_Item CalViewRainbow(Hv_View View,
			      Hv_Item parent,
			      int     minval,
			      int     maxval,
			      char   *title);

static void ECPlaneLabel(short plane,
			 char *text);

static Boolean ECViewFeedback(Hv_View       View,
			      Hv_Point      pp);

static Boolean ECViewLACFeedback(Hv_View       View,
				 Hv_Point      pp);


static Hv_Item CalorimeterViewOptionButtons(Hv_View View,
					    Hv_Item parent,
					    short top);


static Hv_Item AddCalorimeterFeedback(Hv_View View,
				      Hv_Item pitem);



extern Boolean PlaneBorderOnly;

extern short optionarmcolor;
extern short radioarmcolor;
extern short boxcolor;
extern short fbcolor;


static SharedFeedback      uvwxy;
static SharedFeedback      uxlefttdc;
static SharedFeedback      uxleftadc;
static SharedFeedback      vylefttdc;
static SharedFeedback      vyleftadc;
static SharedFeedback      wxrighttdc;
static SharedFeedback      wxrightadc;
static SharedFeedback      yrighttdc;  /* no corresponding ec */
static SharedFeedback      yrightadc;  /* no corresponding ec */

/* ------- ECAccumCB -------*/

static void ECAccumCB(Hv_Event hvevent) {

    Hv_Item              Item = hvevent->item;
    Hv_View              View = hvevent->view;
    ViewData             viewdata;
    
    short oldshowwhat, newshowwhat;

    viewdata = GetViewData(View);
    oldshowwhat = viewdata->ecshowwhat;
    newshowwhat = Hv_HandleRadioButton(Item)-1; /* -1 to convert to C Index*/

    if (newshowwhat == oldshowwhat)
	return;

    viewdata->ecshowwhat = newshowwhat;

    if (viewdata->showwhat != SINGLEEVENT)
	Hv_DrawViewHotRect(hvevent->view); 
}

/* ------- EC1AccumCB -------*/

static void EC1AccumCB(Hv_Event hvevent) {

    Hv_Item              Item = hvevent->item;
    Hv_View              View = hvevent->view;
    ViewData             viewdata;
    
    short oldshowwhat, newshowwhat;

    viewdata = GetViewData(View);
    oldshowwhat = viewdata->ec1showwhat;
    newshowwhat = Hv_HandleRadioButton(Item)-1; /* -1 to convert to C Index*/

    if (newshowwhat == oldshowwhat)
	return;

    viewdata->ec1showwhat = newshowwhat;

    if (viewdata->showwhat != SINGLEEVENT)
	Hv_DrawViewHotRect(hvevent->view); 

}



/* ------- PlaneCB -------*/

static void PlaneCB(Hv_Event hvevent)
    
/*------------------------
  The user has changed the plane on a
  fanned EC view.  (Chose among
  inner, outer, and both.)
  ----------------------------*/
    
{
    Hv_Item              Item = hvevent->item;
    Hv_View              View = hvevent->view;
    ViewData             viewdata;
    short                oldplane, newplane;
    Hv_WorldPolygonData  *wpoly;
    Hv_Item              temp;
    Hv_Region            hrreg = NULL;
    int                  i;
    
    viewdata = GetViewData(View);
    oldplane = viewdata->ecplane;
    newplane = Hv_HandleRadioButton(Item)-1; /* -1 to convert to C Index*/
    
    if (newplane == oldplane)
	return;
    
    hrreg = Hv_ClipHotRect(View, 1);
    EraseViewEvent(View);
    
    viewdata->ecplane = newplane;
    PlaneBorderOnly = True;
    
/* loop over all ec items and change the bg color */
    
    for (i = 0; i < NUM_SECT; i++)
	if ((temp = viewdata->ForwardECViewECItems[i]) != NULL) {
	    wpoly = (Hv_WorldPolygonData *)(temp->data);
	    temp->color = viewdata->fannedecbgcolor[viewdata->ecplane];
	    Hv_DrawItem(temp, NULL);
	}
    
    for (i = 0; i < NUM_SECT; i++)
	if ((temp = viewdata->LAECItems[i]) != NULL) {
	    wpoly = (Hv_WorldPolygonData *)(temp->data);
	    temp->color = viewdata->fannedecbgcolor[viewdata->ecplane];
	    Hv_DrawItem(temp, NULL);
	}
    
    PlaneBorderOnly = False;
    Hv_SaveViewBackground(View, hrreg);
    DrawViewEvent(View, hrreg);
    Hv_DestroyRegion(hrreg);
}


/*------ CalorimeterViewOptionButtons ------*/

static Hv_Item CalorimeterViewOptionButtons(Hv_View View,
					    Hv_Item parent,
					    short top)
{
    ViewData      viewdata;
    Hv_Item       Item;
    
    viewdata = GetViewData(View);


    Item = Hv_VaCreateItem(View,
			   Hv_PARENT,             parent,
			   Hv_TYPE,               Hv_OPTIONBUTTONITEM,
			   Hv_CONTROLVAR,         (void *)(&(viewdata->displayU)),
			   Hv_LEFT,               parent->area->left + 10,
			   Hv_TOP,                top,
			   Hv_TEXT,               " U Strips",
			   Hv_FONT,               Hv_fixed2,
			   Hv_ARMCOLOR,           optionarmcolor,
			   Hv_SINGLECLICK,        OptionButtonCallback,
			   NULL);
    
    Item = StandardOptionButton(View, parent, &(viewdata->displayV), " V Strips", 0, Hv_fixed2);
    Item = StandardOptionButton(View, parent, &(viewdata->displayW), " W Strips", 0, Hv_fixed2);

    Item = StandardOptionButton(View, parent, &(viewdata->displayX), " X Strips", 0, Hv_fixed2);
/*    Hv_OffsetItem(Item, 0, 5, True); */
    Item = StandardOptionButton(View, parent, &(viewdata->displayY), " Y Strips", 0, Hv_fixed2);

    return Item;
}

/*-------- CalorimeterViewFeedback -------------*/

void CalorimeterViewFeedback(Hv_View  View,
			     Hv_Point pp) {

    char      text[80];
    Boolean   insomething = False;

    ViewData  viewdata = GetViewData(View);

    ECPlaneLabel(viewdata->ecplane, text);
    Hv_ChangeFeedback(View, WHATSHOWER, text);
    insomething = ECViewFeedback(View, pp);
    if (!insomething) {
	ClearFeedback(View, CLEAR_EC);
	insomething = ECViewLACFeedback(View, pp);
	if (!insomething)
	    ClearFeedback(View, CLEAR_LAC);
    }

}



/*-------- CalorimeterInitSharedFeedback ------*/

void CalorimeterInitSharedFeedback() {

/*
 *    initialize the shared feedbacks
 */    


    InitSharedFeedback(&uvwxy, WHATXY, fb_text19b, fb_text19c);
    InitSharedFeedback(&uxlefttdc,  XTDCL,  fb_text26, fb_text26a);
    InitSharedFeedback(&uxleftadc,  XADCL,  fb_text27, fb_text27a);
    InitSharedFeedback(&vylefttdc,  YTDCL,  fb_text28, fb_text28a);
    InitSharedFeedback(&vyleftadc,  YADCL,  fb_text29, fb_text29a);
    InitSharedFeedback(&wxrighttdc, XTDCR,  fb_text30, fb_text26b);
    InitSharedFeedback(&wxrightadc, XADCR,  fb_text31, fb_text27b);
    InitSharedFeedback(&yrighttdc,  YTDCR,  NULL,      fb_text26b);
    InitSharedFeedback(&yrightadc,  YADCR,  NULL,      fb_text27b);
}



/*----- CalorimeterViewSetup ------*/

void CalorimeterViewSetup(Hv_View View)
    
{
    Hv_Item   Item, Box1, OPBox, Box2, rbox, rbox2, rbox3, Feedback;
    ViewData  viewdata;
    
    viewdata = GetViewData(View);
    
/* a box container for some child items */
    
    Box1 = Hv_VaCreateItem(View,
			   Hv_TYPE,                Hv_CHOICESETITEM,
			   Hv_ORIENTATION,         Hv_VERTICAL,
			   Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT+Hv_FRAMEAREA,
			   Hv_LEFT,                View->hotrect->left - calfeedbackWidth- 10,
			   Hv_TOP,                 View->hotrect->top - 4,
			   Hv_RESIZEPOLICY,        Hv_DONOTHINGONRESIZE,
			   Hv_NOWON,               viewdata->ecplane+1,
			   Hv_COLOR,               Hv_gray9,
			   Hv_ARMCOLOR,            radioarmcolor,
			   Hv_FILLCOLOR,           boxcolor,
			   Hv_FONT,                Hv_fixed2,
			   Hv_OPTION,             " Inner Plane",
			   Hv_OPTION,             " Outer Plane",
			   Hv_OPTION,             " Both Planes",
			   Hv_SINGLECLICK,         PlaneCB,
			   Hv_WIDTH,               140,
			   NULL);
    
    
    OPBox = Hv_VaCreateItem(View,
			    Hv_TYPE,                Hv_BOXITEM,
			    Hv_GAP,                 1,
			    Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT,
			    Hv_FILLCOLOR,           boxcolor,
			    Hv_PLACEMENTITEM,       Box1,
			    Hv_RESIZEPOLICY,        Hv_DONOTHINGONRESIZE,
			    Hv_RELATIVEPLACEMENT,   Hv_POSITIONBELOW,
			    Hv_WIDTH,               140,
			    NULL);


    Item = CalorimeterViewOptionButtons(View, OPBox, OPBox->area->top + 8);
    OPBox->area->bottom = Item->area->bottom + 6;
    Hv_FixRectWH(OPBox->area);
    

/* a single-event accumulated event radio set */

    rbox = Hv_VaCreateItem(View,
			   Hv_TYPE,                Hv_CHOICESETITEM,
			   Hv_ORIENTATION,         Hv_VERTICAL,
			   Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT+Hv_FRAMEAREA,
			   Hv_NOWON,               SINGLEEVENT,
			   Hv_COLOR,               Hv_gray9,
			   Hv_RESIZEPOLICY,        Hv_DONOTHINGONRESIZE,
			   Hv_ARMCOLOR,            radioarmcolor,
			   Hv_FILLCOLOR,           boxcolor,
			   Hv_FONT,                Hv_fixed2,
			   Hv_OPTION,             " Single Event",
			   Hv_OPTION,             " Accum counts",
			   Hv_SINGLECLICK,         ShowWhatCB,
			   Hv_PLACEMENTITEM,       OPBox,
			   Hv_RELATIVEPLACEMENT,   Hv_POSITIONBELOW,
			   Hv_PLACEMENTGAP,        1,
			   Hv_WIDTH,               Box1->area->width,
			   NULL);


    rbox2 = Hv_VaCreateItem(View,
			   Hv_TYPE,                Hv_CHOICESETITEM,
			   Hv_ORIENTATION,         Hv_VERTICAL,
			   Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT+Hv_FRAMEAREA,
			   Hv_RELATIVEPLACEMENT,   Hv_POSITIONBELOW,
			   Hv_PLACEMENTGAP,        1,
			   Hv_NOWON,               viewdata->ecshowwhat+1,
			   Hv_COLOR,               Hv_gray9,
			   Hv_ARMCOLOR,            radioarmcolor,
			   Hv_RESIZEPOLICY,        Hv_DONOTHINGONRESIZE,
			   Hv_FILLCOLOR,           boxcolor,
			   Hv_FONT,                Hv_fixed2,
			   Hv_OPTION,             " Accumulated U",
			   Hv_OPTION,             " Accumulated V",
			   Hv_OPTION,             " Accumulated W",
			   Hv_SINGLECLICK,         ECAccumCB,
			   Hv_WIDTH,               140,
			   NULL);

    rbox3 = Hv_VaCreateItem(View,
			   Hv_TYPE,                Hv_CHOICESETITEM,
			   Hv_ORIENTATION,         Hv_VERTICAL,
			   Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT+Hv_FRAMEAREA,
			   Hv_RELATIVEPLACEMENT,   Hv_POSITIONBELOW,
			   Hv_RESIZEPOLICY,        Hv_DONOTHINGONRESIZE,
			   Hv_PLACEMENTGAP,        1,
			   Hv_NOWON,               viewdata->ecshowwhat+1,
			   Hv_COLOR,               Hv_gray9,
			   Hv_ARMCOLOR,            radioarmcolor,
			   Hv_FILLCOLOR,           boxcolor,
			   Hv_FONT,                Hv_fixed2,
			   Hv_OPTION,             " Accumulated X",
			   Hv_OPTION,             " Accumulated Y",
			   Hv_SINGLECLICK,         EC1AccumCB,
			   Hv_WIDTH,               140,
			   NULL);

    

    Box2 = Hv_VaCreateItem(View,
			   Hv_TYPE,                Hv_BOXITEM,
			   Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT,
			   Hv_FILLCOLOR,           boxcolor,
			   Hv_RESIZEPOLICY,        Hv_DONOTHINGONRESIZE,
			   Hv_PLACEMENTITEM,       Box1,
			   Hv_RELATIVEPLACEMENT,   Hv_POSITIONRIGHT,
			   Hv_HEIGHT,              rbox3->area->bottom-Box1->area->top,
			   Hv_PLACEMENTGAP,        0,
			   NULL);
    
    Feedback = AddCalorimeterFeedback(View, rbox3);

    Feedback->area->width = calfeedbackWidth;
    Hv_FixRectRB(Feedback->area);

/*    Hv_FitFeedback(View, 10); */

    Box2->area->bottom = rbox3->area->bottom;
    Box2->area->right  = Feedback->area->right;
    Box2->area->left +=1;
    Hv_FixRectWH(Box2->area);
    
    
/* various rainbow scales */

/* pixel energy rainbow */
    
    Item = Hv_VaCreateItem(View,
			   Hv_PARENT,            Box2,
			   Hv_LEFT,              Box2->area->left+20,
			   Hv_TOP,               Box2->area->top+23,
			   Hv_TYPE,              Hv_RAINBOWITEM,
			   Hv_DRAWCONTROL,       Hv_REDRAWHOTRECTWHENCHANGED,
			   Hv_WIDTH,             15,
			   Hv_NUMLABELS,         2,
			   Hv_PRECISION,         2,
			   Hv_MINCOLOR,          viewdata->fannedeccolor-1,
			   Hv_MAXCOLOR,          Hv_purple,
			   Hv_MINVALUE,          0,
			   Hv_MAXVALUE,          30,
			   Hv_COLORWIDTH,        2,
			   Hv_TITLE,             "Energy (Gev)",
			   Hv_SCALEFACTOR,       1.0e-3,
			   Hv_FONT,              Hv_timesBold11,
			   Hv_FONT2,             Hv_timesBold11,
			   NULL);

    viewdata->EnergyRainbowItem = Item;

    viewdata->InnerADCRainbowItem    = CalViewRainbow(View, Box2, 0, 1000, "Inner ADC (EC)");
    viewdata->OuterADCRainbowItem    = CalViewRainbow(View, Box2, 0, 1000, "Outer ADC (EC)");
    viewdata->EC1InnerADCRainbowItem = CalViewRainbow(View, Box2, 0, 1000, "Inner ADC (LAC)");
    viewdata->EC1OuterADCRainbowItem = CalViewRainbow(View, Box2, 0, 1000, "Outer ADC (LAC)");
    viewdata->AccumulatedHitsItem[0] = CalViewRainbow(View, Box2, 0, 250,  "Counts (EC)");
    viewdata->AccumulatedHitsItem[1] = CalViewRainbow(View, Box2, 0, 250,  "Counts (LAC)");
    
    Item = MallocSectorHex(View); 
    AddECItems(View, Item);
}


/* -------- CalViewRainbow ---------*/

static Hv_Item CalViewRainbow(Hv_View View,
			      Hv_Item parent,
			      int     minval,
			      int     maxval,
			      char   *title) {

    ViewData viewdata = GetViewData(View);


    return Hv_VaCreateItem(View,
			   Hv_PARENT,            parent,
			   Hv_TYPE,              Hv_RAINBOWITEM,
			   Hv_EDITABLE,          Hv_LIMITEDEDIT,
			   Hv_DRAWCONTROL,       Hv_REDRAWHOTRECTWHENCHANGED,
			   Hv_WIDTH,             15,
			   Hv_NUMLABELS,         2,
			   Hv_PRECISION,         0,
			   Hv_MINCOLOR,          viewdata->fannedeccolor-1,
			   Hv_MAXCOLOR,          Hv_purple,
			   Hv_MINVALUE,          minval,
			   Hv_MAXVALUE,          maxval,
			   Hv_COLORWIDTH,        2,
			   Hv_TITLE,             title,
			   Hv_RELATIVEPLACEMENT,   Hv_POSITIONBELOW,
			   Hv_PLACEMENTGAP,      39,
			   Hv_FONT,              Hv_timesBold11,
			   Hv_FONT2,             Hv_timesBold11,
			   NULL);
}

/*------ AddCalorimeterFeedback ----------*/

static Hv_Item AddCalorimeterFeedback(Hv_View View,
				    Hv_Item pitem)
{
    Hv_Item   Item;
    short fbfont = Hv_fixed2;
    
    Item = Hv_VaCreateItem(View,
			   Hv_TYPE,                Hv_FEEDBACKITEM,
			   Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT,
			   Hv_RELATIVEPLACEMENT,   Hv_POSITIONBELOW,
			   Hv_PLACEMENTITEM,       pitem,
			   Hv_FEEDBACKDATACOLS,    33,
			   Hv_PLACEMENTGAP,        1,
			   Hv_FILLCOLOR,           fbcolor,
			   Hv_RESIZEPOLICY,        Hv_DONOTHINGONRESIZE,
			   Hv_FEEDBACKENTRY,  EVSOURCE,        fbfont, Hv_black, fb_text42, 
			   Hv_FEEDBACKENTRY,  TRIGGERFILTER,   fbfont, Hv_black, fb_text53, 
			   Hv_FEEDBACKENTRY,  EVENT,           fbfont, Hv_maroon, fb_text2, 
			   Hv_FEEDBACKENTRY,  WHATSECTOR,      fbfont, Hv_maroon, fb_text1, 
			   Hv_FEEDBACKENTRY,  WHATSHOWER,      fbfont, Hv_maroon, fb_text18b, 
			   Hv_FEEDBACKENTRY,  ZPOSITION,       fbfont, Hv_black, fb_text4, 
			   Hv_FEEDBACKENTRY,  XPOSITION,       fbfont, Hv_black, fb_text5, 
			   Hv_FEEDBACKENTRY,  YPOSITION,       fbfont, Hv_black, fb_text6, 
			   Hv_FEEDBACKENTRY,  IJCOORD,         fbfont, Hv_darkBlue, fb_text39, 
			   Hv_FEEDBACKENTRY,  Hv_SEPARATORTAG,
			   Hv_FEEDBACKENTRY,  DISTANCE,        fbfont, Hv_black, fb_text7, 
			   Hv_FEEDBACKENTRY,  ANGLE,           fbfont, Hv_black, fb_text8, 
			   Hv_FEEDBACKENTRY,  PHI,             fbfont, Hv_black, fb_text9, 
			   Hv_FEEDBACKENTRY,  Hv_SEPARATORTAG,
			   Hv_FEEDBACKENTRY,  WHATXY,          fbfont, Hv_forestGreen, fb_text19c, 
			   Hv_FEEDBACKENTRY,  XTDCL,           fbfont, Hv_forestGreen, fb_text26a, 
			   Hv_FEEDBACKENTRY,  XADCL,           fbfont, Hv_forestGreen, fb_text27a, 
			   Hv_FEEDBACKENTRY,  YTDCL,           fbfont, Hv_forestGreen, fb_text28a, 
			   Hv_FEEDBACKENTRY,  YADCL,           fbfont, Hv_forestGreen, fb_text29a, 
			   Hv_FEEDBACKENTRY,  XTDCR,           fbfont, Hv_forestGreen, fb_text26b, 
			   Hv_FEEDBACKENTRY,  XADCR,           fbfont, Hv_forestGreen, fb_text27b, 
			   Hv_FEEDBACKENTRY,  YTDCR,           fbfont, Hv_forestGreen, fb_text28b, 
			   Hv_FEEDBACKENTRY,  YADCR,           fbfont, Hv_forestGreen, fb_text29b, 
			   Hv_FEEDBACKENTRY,  Hv_SEPARATORTAG,
			   Hv_FEEDBACKENTRY,  ACCUMCNT,        fbfont, Hv_darkBlue, fb_text40, 
			   Hv_FEEDBACKENTRY,  PIXID,           fbfont, Hv_darkBlue, fb_text25 , 
			   Hv_FEEDBACKENTRY,  PIXENERGY,       fbfont, Hv_darkBlue, fb_text24, 
			   NULL);
    
    return Item;
}


/* ------- ECViewFeedback ---- */

static Boolean ECViewFeedback(Hv_View       View,
			      Hv_Point      pp)

/*
 * Feedback for the combined EC and EC1 (LAC) 
 * "fanned" view 
 */

{
    ViewData    viewdata;
    Hv_Item     temp;
    short       sect;
    short       u, v, w;
    char        text[40];
    float       x, y, z, rad, theta, phi;
    short       utdc, uadc, vtdc, vadc, wtdc, wadc;
    short       htdc, hadc;
    int         i, nlh, val;
    ijPoint     rij;
    PiDataPtr  *localhits;

    viewdata = GetViewData(View);
    
    utdc = -1;   uadc = -1;
    vtdc = -1;   vadc = -1;
    wtdc = -1;   wadc = -1;
    
    
    for (i = 0; i < NUM_SECT; i++)
	if ((temp = viewdata->ForwardECViewECItems[i]) != NULL) {
	    if (Hv_PointInRegion(pp, temp->region)) {
		
		sect = (short)temp->user2;
		StandardIntChangeFeedback(View, WHATSECTOR, sect+1, NULL);
		GetShowerUVW(View, pp, sect, &u, &v, &w, &z, &x, &y, &rad, &theta, &phi, &rij);
		
		if ((u > 0) && (v > 0) && (w > 0)) {
		    sprintf(text, "[%-2d, %-2d, %-2d]", u, v, w);
		    GetShowerUVWHit(sect, viewdata->ecplane,
				    u, v, w,
				    &utdc, &vtdc, &wtdc,
				    &uadc, &vadc, &wadc);

/* uwv is a shared feedback */


		    ChangeSharedFeedback(View, &uvwxy, EC_SLOT, text);

		    StandardIntChangeFeedback(View, PIXID, PixelFromUVW(u, v, w), NULL);
		    StandardFloatChangeFeedback(View, ZPOSITION, z,     "cm");
		    
		    StandardFloatChangeFeedback(View, DISTANCE,  rad,   "cm");
		    StandardFloatChangeFeedback(View, ANGLE,     theta, "deg");
		    StandardFloatChangeFeedback(View, PHI,       phi,   "deg");

		    sprintf(text, "(%6.3f, %6.3f)", rij.i, rij.j);
		    Hv_ChangeFeedback(View, IJCOORD, text);

		    ECPIHitsHere(sect, viewdata->ecplane, &nlh, &localhits);
		    if ((nlh == 0) || (localhits == NULL))
			Hv_ChangeFeedback(View, PIXENERGY, " ");
		    else {
			sprintf(text, "%8.5f",
				ijToEnergy(rij.i, rij.j, nlh, localhits));
			Hv_ChangeFeedback(View, PIXENERGY, text);
			Hv_Free(localhits);
		    }
		}
		else {
		    
/*  we might be in the histo region. Use fact that we know the sect */
		    
		    GetShowerHistoHit(View, pp, sect, &u, &v, &w, &htdc, &hadc);
		    if (u > 0) {
			if (u > 99)
			    sprintf(text, "[outer u = %-2d]", u-100);
			else
			    sprintf(text, "[inner u = %-2d]", u);
			utdc = htdc;
			uadc = hadc;
			Hv_ChangeFeedback(View, WHATUVW, text);
			ChangeSharedFeedback(View, &uvwxy, EC_SLOT, text);
		    }

		    else if (v > 0) {
			if (v > 99) 
			    sprintf(text, "[outer v = %-2d]", v-100);
			else
			    sprintf(text, "[inner v = %-2d]", v);
			
			vtdc = htdc;
			vadc = hadc;
			ChangeSharedFeedback(View, &uvwxy, EC_SLOT, text);
		    }
		    else if (w > 0) {
			if (w > 99)
			    sprintf(text, "[outer w = %-2d]", w-100);
			else
			    sprintf(text, "[inner w = %-2d]", w);
			wtdc = htdc;
			wadc = hadc;
			ChangeSharedFeedback(View, &uvwxy, EC_SLOT, text);
		    }
		    else
			ChangeSharedFeedback(View, &uvwxy, EC_SLOT, NULL);
		    
		} /* end of (no u, v, w) */
		

		if (viewdata->showwhat == SINGLEEVENT) {
		    Hv_ChangeFeedback(View, ACCUMCNT, " ");
		    PosDefChangeSharedFeedback(View, &uxlefttdc,  EC_SLOT, utdc);
		    PosDefChangeSharedFeedback(View, &uxleftadc,  EC_SLOT, uadc);
		    PosDefChangeSharedFeedback(View, &vylefttdc,  EC_SLOT, vtdc);
		    PosDefChangeSharedFeedback(View, &vyleftadc,  EC_SLOT, vadc);
		    PosDefChangeSharedFeedback(View, &wxrighttdc, EC_SLOT, wtdc);
		    PosDefChangeSharedFeedback(View, &wxrightadc, EC_SLOT, wadc);
		    PosDefChangeSharedFeedback(View, &yrighttdc,  EC_SLOT, -1);
		    PosDefChangeSharedFeedback(View, &yrightadc,  EC_SLOT, -1);

		}
		else {

		    val = 0;

		    switch (viewdata->ecshowwhat) {
		    case USTRIP:
			if ((u > 0) && (u < 37)) {
			    if (viewdata->ecplane == ECBOTH)
				val = accec[sect].counts[ECINNER][USTRIP][u-1] +
				    accec[sect].counts[ECOUTER][USTRIP][u-1];
			    else
				val = accec[sect].counts[viewdata->ecplane][USTRIP][u-1];
			}
			break;

		    case VSTRIP:
			if ((v > 0) && (v < 37)) {
			    if (viewdata->ecplane == ECBOTH)
				val = accec[sect].counts[ECINNER][VSTRIP][v-1] +
				    accec[sect].counts[ECOUTER][VSTRIP][v-1];
			    else
				val = accec[sect].counts[viewdata->ecplane][VSTRIP][v-1];
			}
			break;

		    case WSTRIP:
			if ((w > 0) && (w < 37)) {
			    if (viewdata->ecplane == ECBOTH)
				val = accec[sect].counts[ECINNER][USTRIP][w-1] +
				    accec[sect].counts[ECOUTER][USTRIP][w-1];
			    else
				val = accec[sect].counts[viewdata->ecplane][WSTRIP][w-1];
			}
			break;

		    }

		    
		    sprintf(text, "%d (%d evnts)", val, (int)accumEventCount);
		    Hv_ChangeFeedback(View, ACCUMCNT, text);
		    
		}

		return True;
	    }  /* end point in region */
	} /* end item loop */
    return False;
}



/* ------- ECViewLACFeedback ---- */

static Boolean ECViewLACFeedback(Hv_View       View,
				  Hv_Point      pp)
    
/**************************************
  This is for the LAC part of Calorimeter view
  ************************************/
{
    ViewData    viewdata;
    Hv_Item     temp;
    short       sect;
    short       xstrip, ystrip;
    char        text[40];
    short       xtdcl, xadcl, xtdcr, xadcr, ytdcl, yadcl, ytdcr, yadcr;
    short       htdc, hadc;
    int         i, val;
    Boolean     left;

    viewdata = GetViewData(View);
    
    xtdcl = -1;   xadcl = -1;
    xtdcr = -1;   xadcr = -1;
    ytdcl = -1;   yadcl = -1;
    ytdcr = -1;   yadcr = -1;
    
    
    for (i = 0; i < NUM_SECT; i++)
	if ((temp = viewdata->LAECItems[i]) != NULL) {
	    if (Hv_PointInRegion(pp, temp->region)) {
		
		sect = (short)temp->user2;
		StandardIntChangeFeedback(View, WHATSECTOR, sect+1, NULL);
		GetLAECXY(View, pp, sect, &xstrip, &ystrip);
		
		if ((xstrip > 0) && (ystrip > 0)) {
		    sprintf(text, "[%-2d (x), %-2d (y)]", xstrip, ystrip);
		    GetLAECHit(sect, viewdata->ecplane, 
			       xstrip, ystrip,
			       &xtdcl, &xadcl, &xtdcr, &xadcr,
			       &ytdcl, &yadcl, &ytdcr, &yadcr);
		    
		    ChangeSharedFeedback(View, &uvwxy, LAC_SLOT, text);
		} /* end (x > 0) && (y > 0)) */
		else {

		    
/*  we might be in the histo region. Use fact that we know the sect */
		    
		    GetLAECHistoHit(View, pp, sect, &xstrip, &ystrip, &htdc, &hadc, &left);
		    if (xstrip > 0) {
			if (xstrip > 99)
			    sprintf(text, "[outer x = %-2d]", xstrip-100);
			else
			    sprintf(text, "[inner x = %-2d]", xstrip);

			if (left) {
			    strcat(text, " (left)");
			    xtdcl = htdc;
			    xadcl = hadc;
			}
			else {
			    strcat(text, " (right)");
			    xtdcr = htdc;
			    xadcr = hadc;
			}
			ChangeSharedFeedback(View, &uvwxy, LAC_SLOT, text);
		    }
		    else if (ystrip > 0) {
			if (ystrip > 99) 
			    sprintf(text, "[outer y = %-2d]", ystrip-100);
			else
			    sprintf(text, "[inner y = %-2d]", ystrip);
			

			if (left) {
			    strcat(text, " (left)");
			    ytdcl = htdc;
			    yadcl = hadc;
			}
			else {
			    strcat(text, " (right)");
			    ytdcr = htdc;
			    yadcr = hadc;
			}
			ChangeSharedFeedback(View, &uvwxy, LAC_SLOT, text);
		    }
		    else
			ChangeSharedFeedback(View, &uvwxy, LAC_SLOT, NULL);
		}

		if (viewdata->showwhat == SINGLEEVENT) {
		    Hv_ChangeFeedback(View, ACCUMCNT, " ");
		    PosDefChangeSharedFeedback(View, &uxlefttdc,  LAC_SLOT, xtdcl);
		    PosDefChangeSharedFeedback(View, &uxleftadc,  LAC_SLOT, xadcl);
		    PosDefChangeSharedFeedback(View, &vylefttdc,  LAC_SLOT, ytdcl);
		    PosDefChangeSharedFeedback(View, &vyleftadc,  LAC_SLOT, yadcl);
		    PosDefChangeSharedFeedback(View, &wxrighttdc, LAC_SLOT, xtdcr);
		    PosDefChangeSharedFeedback(View, &wxrightadc, LAC_SLOT, xadcr);
		    PosDefChangeSharedFeedback(View, &yrighttdc,  LAC_SLOT, ytdcr);
		    PosDefChangeSharedFeedback(View, &yrightadc,  LAC_SLOT, yadcr);
		}
		else {

		    val = 0;

		    switch (viewdata->ec1showwhat) {
		    case XSTRIP:
			if ((xstrip > 0) && (xstrip < 25)) {
			    if (viewdata->ecplane == ECBOTH)
				val = accec1[sect].counts[ECINNER][XSTRIP][xstrip-1] +
				    accec1[sect].counts[ECOUTER][XSTRIP][xstrip-1];
			    else
				val = accec1[sect].counts[viewdata->ecplane][XSTRIP][xstrip-1];
			}
			break;

		    case YSTRIP:
			if ((ystrip > 0) && (ystrip < 41)) {
			    if (viewdata->ecplane == ECBOTH)
				val = accec1[sect].counts[ECINNER][YSTRIP][ystrip-1] +
				    accec1[sect].counts[ECOUTER][YSTRIP][ystrip-1];
			    else
				val = accec1[sect].counts[viewdata->ecplane][YSTRIP][ystrip-1];
			}
			break;

		    }

		    
		    sprintf(text, "%d (%d evnts)", val, (int)accumEventCount);
		    Hv_ChangeFeedback(View, ACCUMCNT, text);
		    
		}

		
		return True;
	    }  /* end point in region */
	} /* end item loop */
    return False;
}

/*------- ECPlaneLabel -----*/

static void ECPlaneLabel(short plane,
			 char *text)
    
{
    if (plane == ECINNER)
	sprintf(text, "inner plane");
    else if (plane == ECOUTER)
	sprintf(text, "outer plane");
    else
	sprintf(text, "both planes");
}

