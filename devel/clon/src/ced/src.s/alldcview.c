/*
  ----------------------------------------------------
  -							
  -  File:    alldcview.c	
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


static Hv_Item AvgTDCItem(Hv_View View,
			  Hv_Item parent,
			  Hv_Item place,
			  int     rgn,
			  short   pos,
			  short   gap);



static Hv_Item AddAllDCFeedback(Hv_View View,
				Hv_Item pitem);


static Hv_Item AllDCOptionButtons(Hv_View View,
				  Hv_Item parent);

extern short optionarmcolor;
extern short radioarmcolor;
extern short boxcolor;
extern short fbcolor;


/*------ AllDCOptionButtons ------*/

static Hv_Item
AllDCOptionButtons(Hv_View View, Hv_Item parent)
{
    ViewData      viewdata;
    Hv_Item       Item;
    
    viewdata = GetViewData(View);

    Item = Hv_VaCreateItem(View,
			   Hv_PARENT,             parent,
			   Hv_LEFT,               parent->area->left + 4,
			   Hv_TOP,                parent->area->top + 4,
			   Hv_TYPE,               Hv_OPTIONBUTTONITEM,
			   Hv_CONTROLVAR,         (void *)(&(viewdata->suppressNoise)),
			   Hv_TEXT,               " Suppress DC Noise",
			   Hv_FONT,               Hv_timesBold11,
			   Hv_ARMCOLOR,           optionarmcolor,
			   Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			   Hv_SINGLECLICK,        OptionButtonCallback,
			   NULL);
    
    Item = StandardOptionButton(View, parent, &(viewdata->displayDead),
				" Dead Wires", 0, Hv_timesBold11);

    Item = StandardOptionButton(View, parent, &(viewdata->displayDC0),
				" Display DC0", 0, Hv_timesBold11);
    
    Item = StandardOptionButton(View, parent, &(viewdata->displayDC1),
				" Display DC1", 0, Hv_timesBold11);

    return Item;
}


/*----- AddAllDCFeedback ----------*/

static Hv_Item
AddAllDCFeedback(Hv_View View, Hv_Item pitem)
{
  Hv_Item Item;
  short fbfont = Hv_fixed2;

  Item = Hv_VaCreateItem(View,
		   Hv_TYPE,                Hv_FEEDBACKITEM,
		   Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT,
		   Hv_RELATIVEPLACEMENT,   Hv_POSITIONBELOW,
   	       Hv_PLACEMENTITEM,       pitem,
	       Hv_FEEDBACKDATACOLS,    23,
		   Hv_FILLCOLOR,           fbcolor,
		   Hv_PLACEMENTGAP,        0,

	       Hv_FEEDBACKENTRY,  RUNNUMBER,        fbfont, Hv_black,  fb_text44, 
	       Hv_FEEDBACKENTRY,  EVSOURCE,         fbfont, Hv_black,  fb_text42, 
	       Hv_FEEDBACKENTRY,  TRIGGERFILTER,    fbfont, Hv_black,  fb_text53, 
	       Hv_FEEDBACKENTRY,  EVENT,            fbfont, Hv_maroon, fb_text2, 
	       Hv_FEEDBACKENTRY,  WHATSECTOR,       fbfont, Hv_maroon, fb_text1, 
	  	   Hv_FEEDBACKENTRY,  Hv_SEPARATORTAG,
			   
		   Hv_FEEDBACKENTRY,  WHATSCINT,        fbfont, Hv_darkBlue, fb_text12,
		   Hv_FEEDBACKENTRY,  WHATSUPERLAYER,   fbfont, Hv_darkBlue, fb_text13,
		   Hv_FEEDBACKENTRY,  WHATLAYER,        fbfont, Hv_darkBlue, fb_text14,
		   Hv_FEEDBACKENTRY,  WHATWIRE,         fbfont, Hv_darkBlue, fb_text15,
	       Hv_FEEDBACKENTRY,  OCCUPANCY,        fbfont, Hv_darkBlue, fb_text16,
			   
			   
		   Hv_FEEDBACKENTRY,  Hv_SEPARATORTAG,
		   Hv_FEEDBACKENTRY,  MIDPLANECOORD,    fbfont, Hv_black, fb_text32,
		   Hv_FEEDBACKENTRY,  ZPOSITION,        fbfont, Hv_black, fb_text4, 
		   Hv_FEEDBACKENTRY,  XPOSITION,        fbfont, Hv_black, fb_text5, 
		   Hv_FEEDBACKENTRY,  DISTANCE,         fbfont, Hv_black, fb_text7, 
		   Hv_FEEDBACKENTRY,  ANGLE,            fbfont, Hv_black, fb_text8, 
		   Hv_FEEDBACKENTRY,  Hv_SEPARATORTAG,
		   Hv_FEEDBACKENTRY,  TDCL,             fbfont, Hv_red, fb_text20, 
		   Hv_FEEDBACKENTRY,  ADCL,             fbfont, Hv_red, fb_text21, 
		   Hv_FEEDBACKENTRY,  TDCR,             fbfont, Hv_red, fb_text22, 
		   Hv_FEEDBACKENTRY,  ADCR,             fbfont, Hv_red, fb_text23, 

		   Hv_FEEDBACKENTRY,  Hv_SEPARATORTAG,
		   Hv_FEEDBACKENTRY,  ACCUMCNT,         fbfont, Hv_black, fb_text40, 
		   Hv_FEEDBACKENTRY,  AVGTDC,           fbfont, Hv_black, fb_text41, 
		   Hv_FEEDBACKENTRY,  HOTWIRE,          fbfont, Hv_black, fb_text43, 
			   
		   NULL);
    
  return(Item);
}


/*------- AllDCViewSetup --------*/

void AllDCViewSetup(Hv_View View)
    
{
    Hv_Item  grid, Item;
    Hv_Item  Feedback;
    Hv_Item  dummy, box, rbox, rbox2, rbox3, box4, box5;
    ViewData viewdata;

    viewdata = GetViewData(View);
    
    grid = MallocSectorGrid(View);
    
    rbox = Hv_VaCreateItem(View,
			   Hv_TYPE,                Hv_CHOICESETITEM,
			   Hv_GAP,                 2,
			   Hv_ORIENTATION,         Hv_VERTICAL,
			   Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT+Hv_FRAMEAREA,
			   Hv_NOWON,               SINGLEEVENT,
			   Hv_COLOR,               Hv_gray9,
			   Hv_ARMCOLOR,            radioarmcolor,
			   Hv_LEFT,                View->hotrect->right + 5,
			   Hv_TOP,                 View->hotrect->top - 4,
			   Hv_FILLCOLOR,           boxcolor,
			   Hv_FONT,                Hv_fixed2,
			   Hv_OPTION,             " Single Event",
			   Hv_OPTION,             " Accumulated Data (counts)",
			   Hv_OPTION,             " Accumulated Data (average TDC)",
			   Hv_SINGLECLICK,         ShowWhatCB,
			   Hv_FONT,               Hv_timesBold11,
			   NULL);
    
    rbox2 = Hv_VaCreateItem(View,
			    Hv_TYPE,                Hv_CHOICESETITEM,
			    Hv_GAP,                 0,
			    Hv_ORIENTATION,         Hv_VERTICAL,
			    Hv_NOWON,               1,
			    Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT+Hv_FRAMEAREA,
			    Hv_COLOR,               Hv_gray9,
			    Hv_ARMCOLOR,            radioarmcolor,
			    Hv_PLACEMENTITEM,       rbox,
			    Hv_FILLCOLOR,           boxcolor,
			    Hv_RELATIVEPLACEMENT,   Hv_POSITIONBELOW,
			    Hv_FONT,                Hv_fixed2,
			    Hv_TITLE,              " Color coding",
			    Hv_OPTION,             " Linear",
			    Hv_OPTION,             " Square Root",
			    Hv_OPTION,             " Exponetial",
			    Hv_WIDTH,              125,
			    Hv_SINGLECLICK,         AlgorithmCB,
			    Hv_FONT,               Hv_timesBold11,
			    NULL);


    rbox3 = Hv_VaCreateItem(View,
			    Hv_TYPE,                Hv_BOXITEM,
			    Hv_GAP,                 0,
			    Hv_HEIGHT,              rbox2->area->height,
			    Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT,
			    Hv_FILLCOLOR,           boxcolor,
			    Hv_PLACEMENTITEM,       rbox2,
			    Hv_RELATIVEPLACEMENT,   Hv_POSITIONRIGHT,
			    NULL);
    
    dummy = AllDCOptionButtons(View, rbox3);

/* accumulated rainbow scales */

    box4 = Hv_VaCreateItem(View,
			   Hv_TYPE,                Hv_BOXITEM,
			   Hv_GAP,                 1,
			   Hv_HEIGHT,              100,
			   Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT,
			   Hv_FILLCOLOR,           boxcolor,
			   Hv_PLACEMENTITEM,       rbox2,
			   Hv_RELATIVEPLACEMENT,   Hv_POSITIONBELOW,
			   Hv_FONT,                Hv_timesBold11,
			   Hv_FONT2,               Hv_timesBold11,
			   NULL);

/* dc wire color buttons */

    AddDCColorButtons(View, box4->area->top+10, box4);

    box5 = DC1Controls(View,
		       box4->area->left + 160,
		       box4->area->top,
		       100,
		       Hv_timesBold11);
    

/* accumulated rainbow scales */

    box = Hv_VaCreateItem(View,
			  Hv_TYPE,                Hv_BOXITEM,
			  Hv_GAP,                 1,
			  Hv_HEIGHT,              120,
			  Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT,
			  Hv_FILLCOLOR,           boxcolor,
			  Hv_PLACEMENTITEM,       box4,
			  Hv_RELATIVEPLACEMENT,   Hv_POSITIONBELOW,
			  Hv_FONT,                Hv_timesBold11,
			  Hv_FONT2,               Hv_timesBold11,
			  NULL);

    
    Item = Hv_VaCreateItem(View,
			   Hv_PARENT,            box,
			   Hv_TYPE,              Hv_RAINBOWITEM,
/*			   Hv_EDITABLE,          Hv_LIMITEDEDIT, */
			   Hv_DRAWCONTROL,       Hv_REDRAWHOTRECTWHENCHANGED,
			   Hv_WIDTH,             15,
			   Hv_NUMLABELS,         2,
			   Hv_PRECISION,         0,
			   Hv_MINCOLOR,          Hv_yellow+30,
			   Hv_MAXCOLOR,          Hv_red,
			   Hv_MINVALUE,          0,
			   Hv_MAXVALUE,          500,
			   Hv_COLORWIDTH,        2,
			   Hv_TITLE,             "Counts",
			   Hv_LEFT,              box->area->left+25,
			   Hv_TOP,               box->area->top+25,
			   Hv_FONT,              Hv_timesBold11,
			   Hv_FONT2,             Hv_timesBold11,
			   NULL);
    viewdata->AccumulatedHitsItem[0] = Item;


    viewdata->AverageTDCItem[0] = AvgTDCItem(View, box, Item, 0, Hv_POSITIONRIGHT, 60);
    viewdata->AverageTDCItem[1] = AvgTDCItem(View, box, Item, 1, Hv_POSITIONBELOW, 50);
    viewdata->AverageTDCItem[2] = AvgTDCItem(View, box, viewdata->AverageTDCItem[1], 2, Hv_POSITIONRIGHT, 60);

    Feedback = AddAllDCFeedback(View, box);
    Hv_FitFeedback(View, 10);
    
    rbox->area->right = Feedback->area->right;
    Hv_FixRectWH(rbox->area);
    
    rbox3->area->right = Feedback->area->right;
    Hv_FixRectWH(rbox3->area);

    box->area->right = Feedback->area->right;
    Hv_FixRectWH(box->area);
    
    box4->area->right = box5->area->left-1;
    Hv_FixRectWH(box4->area);
    
    box5->area->right = Feedback->area->right;
    box5->area->bottom = box4->area->bottom;
    Hv_FixRectWH(box5->area);
    
/* add the detector packages */
    
    AddScintillatorItems(View, NULL);
    AddDCItems(View);
}


/*-------- AllDCInitSharedFeedback ------*/

void AllDCInitSharedFeedback() {
}


/*-------- AllDCFeedback --------*/

void AllDCFeedback(Hv_View View,
		   Hv_Point  pp)
    
{
    short    sect, scint;
    float    theta, r, z, x;
    short    supl, lay, wire, trgn, tsupl;
    char     text[60], text2[80];
    short    tdcl, tdcr, adcl, adcr;
    float    occupancy = 0.0;
    ViewData viewdata = GetViewData(View);

    unsigned char    slayer, swire;


    int      nhit, hotid;
    int      acccnt;       /* accumulated count */
    int      avgtdc;
    float    avgocc;

    GetAllDCCoordinates(View, pp, &sect, &r, &theta,
			&z, &x, &supl, &lay, &wire, &scint);
    sect++;
    scint++;
    lay++;
    wire++;
    
    if (sect <= 0) {
	ClearFeedback(View, CLEAR_ALLDC);
	return;
    }
    
    StandardIntChangeFeedback(View,   WHATSECTOR,  sect,   NULL);
    StandardFloatChangeFeedback(View, ANGLE,       theta,  "deg");
    StandardFloatChangeFeedback(View, DISTANCE,    r,      "cm");
    StandardFloatChangeFeedback(View, ZPOSITION,   z,      "cm");
    StandardFloatChangeFeedback(View, XPOSITION,   x,      "cm");
    
    PosDefIntChangeFeedback(View, WHATSCINT, scint);
    PosDefIntChangeFeedback(View, WHATWIRE,  wire);
    
    if (lay < 1)
	Hv_ChangeFeedback(View, WHATLAYER,  " ");
    else {
	sprintf(text, "%d (or %d/36)", lay, 6*supl + lay);
	Hv_ChangeFeedback(View, WHATLAYER, text);
    }

    adcl = -1;
    adcr = -1;
    tdcl = -1;
    tdcr = -1;
    acccnt = -1;
    avgtdc = -1;
    avgocc = 0.0;
    nhit = 0;
    
/* get occupancy/hit info */

    if (wire > 0) {
	trgn = supl/2;   /*0..2*/
	tsupl = supl%2;  /*0..1*/

	if (viewdata->showwhat == SINGLEEVENT) {
	    GetDCTDCValue(sect-1, trgn, tsupl, lay-1,
			  wire-1, &occupancy, &nhit, &tdcl);
	    
	    occupancy = Hv_fMax(0.0, occupancy);
	    sprintf(text, "%6.2f%% (%d hits)", occupancy, nhit);
	    Hv_ChangeFeedback(View, OCCUPANCY, text);
	    Hv_ChangeFeedback(View, ACCUMCNT, " ");
	    Hv_ChangeFeedback(View, AVGTDC, " ");
	    Hv_ChangeFeedback(View, HOTWIRE, " ");
	}
	else {  /* feedback when ACCUMULATING */
	    GetDCAccumValue(sect-1, trgn, tsupl, lay-1,
			    wire-1, &acccnt, &avgtdc,
			    &avgocc);

	    sprintf(text, "%7.3f%% AVG %d evts", avgocc, (int)accumEventCount);
	    Hv_ChangeFeedback(View, OCCUPANCY, text);

	    PosDefIntChangeFeedback(View, ACCUMCNT, acccnt);
	    PosDefIntChangeFeedback(View, AVGTDC, avgtdc);

	    hotid = accdc[sect-1][trgn].hotid;

	    BreakShort(hotid, &swire, &slayer);

	    sprintf(text, "Rgn %d Lay %d Wire %d", 
		    trgn+1, slayer, swire);
	    Hv_ChangeFeedback(View, HOTWIRE, text);
	    
	}
    }

    else if (scint > 0) {
	GetScintHit(sect-1, scint-1, &tdcl, &adcl, &tdcr, &adcr);
    }

    PosDefIntChangeFeedback(View, TDCL, tdcl);
    PosDefIntChangeFeedback(View, ADCL, adcl);
    PosDefIntChangeFeedback(View, TDCR, tdcr);
    PosDefIntChangeFeedback(View, ADCR, adcr);
    
    
    SuperLayerNameFeedback(View, supl, text);

    if (supl >= 0) {

/* what we display depends on what mode we are in */

	if (viewdata->showwhat == ACCUMULATEDEVENTS) {
	    if (wire > 0)
		sprintf(text2, "supl %s  layer %d  wire %d    cnt %d", text, slayer, wire, acccnt);
	    else
		sprintf(text2, "superlayer %s ", text);
	}
	else if (viewdata->showwhat == ACCUMULATEDEVENTS) {
	    if (wire > 0)
		sprintf(text2, "supl %s  layer %d  wire %d    avgtdc %d", text, slayer, wire, avgtdc);
	    else
		sprintf(text2, "superlayer %s ", text);
	}

	else { /* single event */
	    sprintf(text2, "superlayer %s   occupancy: %6.2f%% (%d hits)", text, occupancy, nhit); 
	}

	UpdateMessageBox(View, text2);
    }

    else if (scint > 0) {
	sprintf(text, "scint %d", scint);
	if (tdcl > 0) {
	    sprintf(text2, "%s  [tdcL: %d  adcL: %d]  [tdcR: %d  adcR: %d]", text, tdcl, adcl, tdcr, adcr);
	    UpdateMessageBox(View, text2);
	}
	else {
	    UpdateMessageBox(View, text);
	}
    }
    
    else {
	if ((eventsource == CED_FROMFILE) && (bosFileName != NULL))
	    UpdateMessageBox(View, bosFileName);
	else
	    UpdateMessageBox(View, "nothing interesting to report...");
    }

}


/*------ AvgTDCItem -------------*/

static Hv_Item AvgTDCItem(Hv_View View,
			  Hv_Item parent,
			  Hv_Item place,
			  int     rgn,
			  short   pos,
			  short   gap) {

    Hv_Item Item;
    char    tst[25];

    sprintf(tst, "Reg%-1d avg TDC", rgn+1);

    Item = Hv_VaCreateItem(View,
			   Hv_PARENT,            parent,
			   Hv_TYPE,              Hv_RAINBOWITEM,
/*			   Hv_EDITABLE,          Hv_LIMITEDEDIT, */
			   Hv_DRAWCONTROL,       Hv_REDRAWHOTRECTWHENCHANGED,
			   Hv_NUMLABELS,         2,
			   Hv_PRECISION,         0,
			   Hv_MINCOLOR,          Hv_yellow+30,
			   Hv_MAXCOLOR,          Hv_red,
			   Hv_MINVALUE,          dcmintime[rgn],
			   Hv_MAXVALUE,          dcmaxtime[rgn],
			   Hv_COLORWIDTH,        2,
			   Hv_TITLE,             tst,
			   Hv_PLACEMENTITEM,     place,
			   Hv_RELATIVEPLACEMENT, pos,
			   Hv_PLACEMENTGAP,      gap,
			   Hv_FONT,              Hv_timesBold11,
			   Hv_FONT2,             Hv_timesBold11,
			   NULL);
    

    return Item;
}

