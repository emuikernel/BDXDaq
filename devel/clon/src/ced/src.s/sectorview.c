/*
  ----------------------------------------------------
  -							
  -  File:    sectorview.c	
  -							
  -  Summary:						
  -           sets up sector views
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
#include "geometry.h"

static void  MousePointWorldFeedback(Hv_View       View,
				     Hv_Point      pp);


static void FieldFeedback(Hv_View       View,
			  Hv_Point      pp);


static void AnchorFeedback(Hv_View       View,
			   Hv_Point      pp);

static void ComponentFeedback(Hv_View       View,
			      Hv_Point      pp);

static void TrackFeedback(Hv_View View,
			  Hv_Point pp);

static void FieldOptionButtonCallback(Hv_Event hvevent);

static void TaggerOptionButtonCallback(Hv_Event hvevent);

static void SectorCB(Hv_Event hvevent);

static void DC1ControlCB(Hv_Event hvevent);

static void PhiCB(Hv_Event hvevent);

static Hv_Item SectorViewOptionButtons(Hv_View View,
				       Hv_Item parent,
				       short   top);


static Hv_Item AddSectorFeedback(Hv_View View,
				 Hv_Item pitem);


static Boolean ScintFeedback(Hv_View       View,
			     Hv_Point      pp,
			     short        *tdcl,
			     short        *adcl,
			     short        *tdcr,
			     short        *adcr);

static Boolean TaggerFeedback(Hv_View       View,
			      Hv_Point      pp,
			      short        *tdcl,
			      short        *adcl,
			      short        *tdcr,
			      short        *adcr);

static Boolean EC1Feedback(Hv_View       View,
			   Hv_Point      pp,
			   short        *tdcl,
			   short        *adcl,
			   short        *tdcr,
			   short        *adcr);

static Boolean CerenkovFeedback(Hv_View       View,
				Hv_Point      pp,
				short        *tdcl,
				short        *adcl);

static Boolean DCFeedback(Hv_View       View,
			  Hv_Point      pp,
			  short        *tdcl,
			  short        *adcl);

static Boolean ShowerFeedback(Hv_View       View,
			      Hv_Point      pp,
			      short        *tdcl,
			      short        *adcl);


int theSector;

extern short optionarmcolor;
extern short radioarmcolor;
extern short boxcolor;
extern short fbcolor;



/* for use in shared  feedbacks */

#define   EC_SLOT          0
#define   TAG_SLOT         1

static SharedFeedback      showertage;
static SharedFeedback      striptagt;


/*--------- SectorViewSetup -------*/

extern void SectorViewSetup(Hv_View View){

    Hv_Item     Feedback, Item, Box1, Box1b, Box2;
    Hv_Item     sep1, sep2, sep2b, sep3;
    ViewData    viewdata;
    int         i;
    
    viewdata = GetViewData(View);
    
/* add a beamline and fieldmap for sector views */
    
    Item = MallocFieldmapItem(View);
    Item = MallocBeamlineItem(View);
    viewdata->BeamLineItem = Item;
    
/* a box container for some child items */
    
    Box1 = StandardOptionButtonBox(View, 175);
    Item = SectorViewOptionButtons(View, Box1, Box1->area->top + 2);
    Box1->area->bottom = Item->area->bottom + 1;
    Hv_FixRectWH(Box1->area);

/* Box1b will start life as a radio container but will
   eventually expand to hold other items */

    Box1b = DC1Controls(View,
			Box1->area->left,
			Box1->area->bottom+1,
			Box1->area->width,
			Hv_fixed2);

    sep1 = StandardSeparator(View, Box1b, 10);

/* dc wire color buttons */

    AddDCColorButtons(View, sep1->area->bottom+10, Box1b);

/* Box2 will start life as a radio container but will
   eventually hold other items */
    
    Box2 = Hv_VaCreateItem(View,
			   Hv_TYPE,                Hv_CHOICESETITEM,
			   Hv_GAP,                 4,
			   Hv_ORIENTATION,         Hv_VERTICAL,
			   Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT+Hv_FRAMEAREA,
			   Hv_LEFT,                Box1->area->right+1,
			   Hv_TOP,                 Box1->area->top,
			   Hv_COLOR,               Hv_gray9,
			   Hv_ARMCOLOR,            radioarmcolor,
			   Hv_FILLCOLOR,           boxcolor,
			   Hv_FONT,                Hv_fixed2,
			   Hv_OPTION,             " Sectors 1/4",
			   Hv_OPTION,             " Sectors 2/5",
			   Hv_OPTION,             " Sectors 3/6",
			   Hv_OPTION,             " Most Data",
			   Hv_OPTION,             " Largest EC Sum",
			   Hv_NOWON,               GoodSector(View) + 1, 
			   Hv_SINGLECLICK,         SectorCB,
			   NULL);

    viewdata->SectorBoxItem = Box2;

    sep2 = StandardSeparator(View, Box2, 7);

/* now a wheel for changing phi */
    
    Item = Hv_VaCreateItem(View,
			   Hv_TOP,                 sep2->area->bottom + 22,
			   Hv_LEFT,                Box2->area->left + 20,
			   Hv_TYPE,                Hv_WHEELITEM,
			   Hv_PARENT,              Box2,
			   Hv_TITLE,               "Midplane dPhi",
			   Hv_FONT,                Hv_timesBold11,
			   Hv_CALLBACK,            PhiCB,
			   Hv_MINVALUE,            -27,
			   Hv_MAXVALUE,            27,
			   Hv_WIDTH,               125,
			   Hv_CURRENTVALUE,        0,
			   NULL);


    sep2b = StandardSeparator(View, Box2, 8);
    sep2b->area->top = Box1->area->bottom;
    Hv_FixRectRB(sep2b->area);


    Item = Hv_VaCreateItem(View,
			   Hv_PARENT,            Box2,
			   Hv_LEFT,              Box2->area->left+22,
			   Hv_TOP,               sep2b->area->bottom+23,
			   Hv_TYPE,              Hv_RAINBOWITEM,
			   Hv_DRAWCONTROL,       Hv_REDRAWHOTRECTWHENCHANGED,
			   Hv_WIDTH,             15,
			   Hv_NUMLABELS,         2,
			   Hv_PRECISION,         0,
			   Hv_MINCOLOR,          Hv_yellow+25,
			   Hv_MAXCOLOR,          Hv_red,
			   Hv_EDITABLE,          Hv_LIMITEDEDIT,
			   Hv_MINVALUE,          0,
			   Hv_MAXVALUE,          1000,
			   Hv_COLORWIDTH,        3,
			   Hv_TITLE,             "Forward EC ADC",
			   Hv_FONT,              Hv_timesBold11,
			   Hv_FONT2,             Hv_timesBold11,
			   NULL);

    viewdata->EnergyRainbowItem = Item;

    sep3 = StandardSeparator(View, Box2, 16);
    sep3->area->top = sep1->area->top;
    Hv_FixRectRB(sep3->area);

/* now a horizontal rainbow for the magnetic field */
    
    Item = Hv_VaCreateItem(View,
			   Hv_PARENT,            Box2,
			   Hv_LEFT,              Box2->area->left + 30,
			   Hv_TOP,               sep3->area->top + 20,
			   Hv_TYPE,              Hv_RAINBOWITEM,
			   Hv_ORIENTATION,       Hv_HORIZONTAL,
			   Hv_TITLEPLACEMENT,    Hv_POSITIONRIGHT,
			   Hv_DRAWCONTROL,       Hv_REDRAWHOTRECTWHENCHANGED,
			   Hv_WIDTH,             15,
			   Hv_NUMLABELS,         3,
			   Hv_PRECISION,         2,
			   Hv_MINCOLOR,          Hv_white,
			   Hv_MAXCOLOR,          Hv_red,
			   Hv_MINVALUE,          0,
			   Hv_MAXVALUE,          2000,
			   Hv_COLORWIDTH,        1,
			   Hv_TITLE,             "B(T)",
			   Hv_SCALEFACTOR,       1.0e-3,
			   Hv_FONT,              Hv_timesBold11,
			   Hv_FONT2,             Hv_timesBold11,
			   NULL);
    
    
    viewdata->FieldRainbowItem = Item;
    
    Box2->area->bottom = Box1b->area->bottom;
    Hv_FixRectWH(Box2->area);

/* now the feedback */
    
    Feedback = AddSectorFeedback(View, Box1b);
    Hv_FitFeedback(View, 10);

    Box2->area->bottom = Box1b->area->bottom;
    Box2->area->right = Feedback->area->right;
    Hv_FixRectWH(Box2->area);
    
    Hv_SeparatorResizeToParent(sep1);
    Hv_SeparatorResizeToParent(sep2);
    Hv_SeparatorResizeToParent(sep2b);
    Hv_SeparatorResizeToParent(sep3);
    
    
/* add the detector packages */
    
    AddScintillatorItems(View, NULL);
    AddCerenkovItems(View);
    AddECItems(View, NULL);
    AddDCItems(View);

/* add the tagger */
    
    viewdata->TaggerItem = MallocTagger(View);

/* copy all the worlds */
    
    for (i = 0; i < 4; i++)
	Hv_LocalRectToWorldRect(View, View->hotrect, &(viewdata->worlds[i]));

}

/*-------- SectorViewFeedback --------*/

extern void SectorViewFeedback(Hv_View  View,
			       Hv_Point pp) {
    MousePointWorldFeedback(View, pp);
    FieldFeedback(View, pp);
    AnchorFeedback(View, pp);
    TrackFeedback(View, pp);
    ComponentFeedback(View, pp);
}


/*-------- SectorViewInitSharedFeedback -----*/

void SectorViewInitSharedFeedback() {
    InitSharedFeedback(&showertage, WHATSHOWER, fb_text18, fb_text18c);
    InitSharedFeedback(&striptagt,  WHATUVW,    fb_text19, fb_text19d);
}


/*------ AddSectorFeedback ----------*/

static Hv_Item AddSectorFeedback(Hv_View View,
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
			   Hv_FILLCOLOR,           fbcolor,
			   Hv_PLACEMENTGAP,        0,
			   Hv_FEEDBACKENTRY,  RUNNUMBER,        fbfont, Hv_black,  fb_text44, 
			   Hv_FEEDBACKENTRY,  EVSOURCE,         fbfont, Hv_black,  fb_text42, 
			   Hv_FEEDBACKENTRY,  TRIGGERFILTER,    fbfont, Hv_black,  fb_text53, 
			   Hv_FEEDBACKENTRY,  EVENT,            fbfont, Hv_maroon, fb_text2, 
			   Hv_FEEDBACKENTRY,  WHATSECTOR,       fbfont, Hv_maroon, fb_text1, 
			   Hv_FEEDBACKENTRY,  ANCHORDISTANCE,   fbfont, Hv_black, fb_text3, 
			   Hv_FEEDBACKENTRY,  ZPOSITION,        fbfont, Hv_black, fb_text4, 
			   Hv_FEEDBACKENTRY,  XPOSITION,        fbfont, Hv_black, fb_text5, 
			   Hv_FEEDBACKENTRY,  YPOSITION,        fbfont, Hv_black, fb_text6, 
			   Hv_FEEDBACKENTRY,  DISTANCE,         fbfont, Hv_black, fb_text7, 
			   Hv_FEEDBACKENTRY,  Hv_SEPARATORTAG,
			   Hv_FEEDBACKENTRY,  ANGLE,            fbfont, Hv_black, fb_text8, 
			   Hv_FEEDBACKENTRY,  PHI,              fbfont, Hv_black, fb_text9, 
			   Hv_FEEDBACKENTRY,  Hv_SEPARATORTAG,
			   Hv_FEEDBACKENTRY,  BMAG,             fbfont, Hv_brown, fb_text10, 
			   Hv_FEEDBACKENTRY,  BVECT,            fbfont, Hv_brown, fb_text11, 
			   Hv_FEEDBACKENTRY,  Hv_SEPARATORTAG,
			   Hv_FEEDBACKENTRY,  WHATSCINT,        fbfont, Hv_darkBlue, fb_text12, 
			   Hv_FEEDBACKENTRY,  WHATSUPERLAYER,   fbfont, Hv_darkBlue, fb_text13, 
			   Hv_FEEDBACKENTRY,  WHATLAYER,        fbfont, Hv_darkBlue, fb_text14, 
			   Hv_FEEDBACKENTRY,  WHATWIRE,         fbfont, Hv_darkBlue, fb_text15, 
			   Hv_FEEDBACKENTRY,  OCCUPANCY,        fbfont, Hv_darkBlue, fb_text16, 
			   Hv_FEEDBACKENTRY,  WHATCERENK,       fbfont, Hv_darkBlue, fb_text17, 
			   Hv_FEEDBACKENTRY,  WHATSHOWER,       fbfont, Hv_darkBlue, fb_text18, 
			   Hv_FEEDBACKENTRY,  WHATUVW,          fbfont, Hv_darkBlue, fb_text19, 
			   Hv_FEEDBACKENTRY,  Hv_SEPARATORTAG,
			   Hv_FEEDBACKENTRY,  TDCL,             fbfont, Hv_red, fb_text20, 
			   Hv_FEEDBACKENTRY,  ADCL,             fbfont, Hv_red, fb_text21, 
			   Hv_FEEDBACKENTRY,  TDCR,             fbfont, Hv_red, fb_text22, 
			   Hv_FEEDBACKENTRY,  ADCR,             fbfont, Hv_red, fb_text23, 
			   Hv_FEEDBACKENTRY,  Hv_SEPARATORTAG,
			   Hv_FEEDBACKENTRY,  TRACKID,      fbfont, Hv_brown, fb_text33,
			   Hv_FEEDBACKENTRY,  CHI2,         fbfont, Hv_brown, fb_text34,
			   Hv_FEEDBACKENTRY,  MOMENTUM,     fbfont, Hv_brown, fb_text35,
			   Hv_FEEDBACKENTRY,  VERTEX,       fbfont, Hv_brown, fb_text36,
			   Hv_FEEDBACKENTRY,  DIRCOSINE,    fbfont, Hv_brown, fb_text37,
			   Hv_FEEDBACKENTRY,  CHARGE,       fbfont, Hv_brown, fb_text38,
			   NULL);
    
    return Item;
}


/*------ SectorViewOptionButtons ------*/

static Hv_Item SectorViewOptionButtons(Hv_View View,
				       Hv_Item parent,
				       short   top)
    
{
    ViewData      viewdata;
    Hv_Item       Item;
    
    viewdata = GetViewData(View);
    
/* magnetic field display switch has a slightly more complicated callback */
    
    Item = Hv_VaCreateItem(View,
			   Hv_PARENT,             parent,
			   Hv_TYPE,               Hv_OPTIONBUTTONITEM,
			   Hv_CONTROLVAR,         (void *)(&(viewdata->displayField)),
			   Hv_TEXT,               " Magnetic Field",
			   Hv_FONT,               Hv_fixed2,
			   Hv_ARMCOLOR,           optionarmcolor,
			   Hv_LEFT,               parent->area->left + 10,
			   Hv_TOP,                top,
			   Hv_SINGLECLICK,        FieldOptionButtonCallback,
			   Hv_PLACEMENTGAP,       0,
			   NULL);
    
    viewdata->FieldDisplayItem = Item;

#define OBGAP 0

    Item = StandardOptionButton(View, parent, &(viewdata->suppressNoise), " Suppress DC Noise", OBGAP, Hv_fixed2);
    Item = StandardOptionButton(View, parent, &(viewdata->displayDead), " Dead Wires", OBGAP, Hv_fixed2);
    Item = StandardOptionButton(View, parent, &(viewdata->displayTagger), " Tagger", OBGAP, Hv_fixed2);

    Hv_SetItemSensitivity(Item, (Tagger.numEC > 0), False); 

    Item->singleclick = TaggerOptionButtonCallback; 

    Item = StandardOptionButton(View, parent, &(viewdata->displayDC0),
				" Display DC0", OBGAP, Hv_fixed2);
    
    Item = StandardOptionButton(View, parent, &(viewdata->displayDC1),
				" Display DC1", OBGAP, Hv_fixed2);

    Item = StandardOptionButton(View, parent, &(viewdata->displayHBT),
				" Hit Based Tracks", OBGAP, Hv_fixed2);

    Item = StandardOptionButton(View, parent, &(viewdata->displayTBT),
				" Time Based Tracks", OBGAP, Hv_fixed2);

    Item = StandardOptionButton(View, parent, &(viewdata->displayDCA), 
				" Calculated DOCA", OBGAP, Hv_fixed2);
    
#undef OBGAP
    
    return Item;
}


/* ------- FieldOptionButtonCallback ------*/

static void FieldOptionButtonCallback(Hv_Event hvevent)
    
{
    Hv_Item     Item = hvevent->item;
    
    
/* if it is off (so trying to turn on)  and there is no field, try to
   get one */
    
    if (!Hv_GetOptionButton(Item) && (fmap.fmapptr == NULL)) {
	UpdateMessageBox(hvevent->view, "Need to find a magnetic field table.");
	ReadField();
	
/* still no field ? */
	
	if (fmap.fmapptr == NULL)
	    return;
    }
    
    Hv_OptionButtonCallback(hvevent);
    Hv_SetViewDirty(hvevent->view);
    Hv_DrawViewHotRect(hvevent->view); 
    UpdateMessageBox(hvevent->view, " ");
}


/*-------- TaggerOptionButtonCallback ------*/

static void TaggerOptionButtonCallback(Hv_Event hvevent)
    
{
    Hv_View     View = hvevent->view;
    Hv_Item     tagger;

    ViewData    viewdata;

    viewdata = GetViewData(View);

    tagger = viewdata->TaggerItem;

    if (Hv_CheckItemDrawControlBit(tagger, Hv_DONTDRAW))
	Hv_SetItemToDraw(tagger);
    else
	Hv_SetItemNotToDraw(tagger);

    OptionButtonCallback(hvevent);
}


/* ------- SectorCB -------*/

static void SectorCB(Hv_Event hvevent)
    
{
    
/* note viewdata->visiblesectors is NOT a C index */
    
    Hv_Item    Item = hvevent->item;
    Hv_View    View = hvevent->view;
    ViewData   viewdata;
    short      oldvs, newvs;
    short      mostds;
    Hv_FRect   world;
    
    viewdata = GetViewData(View);
    
    oldvs = viewdata->visiblesectors;
    viewdata->visiblesectors = Hv_HandleRadioButton(Item);
    
    if (viewdata->visiblesectors == MOSTDATASECTOR) {
	mostds = MostDataSector();
	if (mostds < 1)
	    mostds = oldvs;
	viewdata->visiblesectors = -mostds; /* neg will flag this as "most data" */
    }

    if (viewdata->visiblesectors == MOSTECSUMSECTOR) {
	mostds = LargestECSumSector();
	if (mostds < 1)
	    mostds = abs(oldvs);
	viewdata->visiblesectors = mostds + 100; /* > 100 will flag this as "largest ecsum" */
    }

    
    if (viewdata->visiblesectors == oldvs)
	return;
    

    newvs = GoodSector(View);


    viewdata->phi = Hv_DEGTORAD*(60.0*newvs + (float)(viewdata->dphi));
    SetSelectability(View);
    UpdateViewTitle(View);
    SwitchSectorGeometry(View);
    
/* switch the "worlds" */
    
    oldvs = abs(oldvs) - 1;
    
    Hv_LocalRectToWorldRect(View, View->hotrect, &(viewdata->worlds[oldvs]));
    world = viewdata->worlds[newvs];
    
    Hv_QuickZoomView(View, world.xmin, world.xmax, world.ymin, world.ymax);
}


/* ------- DC1ControlCB -------*/

static void DC1ControlCB(Hv_Event hvevent)
    
{
    Hv_Item    Item = hvevent->item;
    Hv_View    View = hvevent->view;
    ViewData   viewdata;
    int        odcc;
    
    viewdata = GetViewData(View);

    odcc = viewdata->dc1control;
    viewdata->dc1control = Hv_HandleChoice(Item);

    if (odcc != viewdata->dc1control) {
	Hv_DrawViewHotRect(View);
    }
}


/* ------- PhiCB -------*/

static void PhiCB(Hv_Event hvevent)
    
{
    Hv_Item          Item = hvevent->item;
    Hv_View          View = hvevent->view;
    ViewData         viewdata;
    Hv_WheelData    *wheel;
    short            vs;

    
    wheel = (Hv_WheelData *)(Item->data);
    
    viewdata = GetViewData(View);
    viewdata->dphi = wheel->current;
    vs = GoodSector(View);
    
    viewdata->phi = Hv_DEGTORAD*(60.0*vs + (float)(viewdata->dphi));
    SetSelectability(View);
    
/* fix the regions of all items that have projections */
    
    FixProjectedRegions(View);
    
    Hv_SetViewDirty(View);
    Hv_DrawViewHotRect(View);
}

/*--------- TrackFeedback ---------*/

static void TrackFeedback(Hv_View View,
			  Hv_Point pp)
    
{
    Boolean     foundtrack = False;
    char        txt[60];

    ViewData    viewdata = GetViewData(View);
    HBTRDataPtr hbtr = NULL;

    TBTRDataPtr tbtr = NULL;


    int         i, trackid, j;
    Hv_Point    *xp;
    int         np;
    float       P;

    
    if (View->tag == SECTOR) {
	if ((bosHEAD == NULL) || (GetNumHBTracks() < 1)) {
	    ClearFeedback(View, CLEAR_TRACK);
	    return;
	}
	
/* now go looking for a hit based track */
	
	if (viewdata->displayHBT) {
	    for (i = 0; i < GetNumHBTracks(); i++) {
		trackid = i+1;
		
		if (foundtrack)
		    break;
		
		if (GetHBTrackSector(trackid) == (theSector-1)) {
		    
		    GetHBTrackPoly(View, trackid, &xp, &np, &hbtr);
		    
		    if (xp != NULL) {
			for (j = 0; j < np; j++)
			    if ((abs(pp.x - xp[j].x) < 4) && (abs(pp.y - xp[j].y) < 4)) {
				foundtrack = True;
				Hv_Free(xp);
				break;
			    }
		    }
		}  /* end tsect = sect */
	    } /* end loop on tracks */
	}

/* if did not find hit based, look for time based */

	if (!foundtrack && (viewdata->displayTBT)) {
	    for (i = 0; i < GetNumTBTracks(); i++) {
		trackid = i+1;

		if (foundtrack)
		    break;

		if (GetTBTrackSector(trackid) == (theSector-1)) {

		    GetTBTrackPoly(View, trackid, &xp, &np, &tbtr);

		    if (xp != NULL) {
			for (j = 0; j < np; j++)
			    if ((abs(pp.x - xp[j].x) < 4) && (abs(pp.y - xp[j].y) < 4)) {
				foundtrack = True;
				Hv_Free(xp);
				break;
			    }
		    }
		}  /* end tsect = sect */
	    } /* end loop on tracks */
	} /* end !found track */
	
	if ((foundtrack) && (hbtr != NULL)) {

	    StandardIntChangeFeedback(View,   TRACKID,     hbtr->itr_sec % 100,   NULL);
	    StandardFloatChangeFeedback(View, CHI2,        hbtr->chi2,   NULL);

	    P =sqrt(hbtr->px*hbtr->px +
		hbtr->py*hbtr->py +
		hbtr->pz*hbtr->pz);

	    StandardFloatChangeFeedback(View, MOMENTUM,    P,      "GeV");
	    sprintf(txt, "%5.2f %5.2f %5.2f [vtx in sect %1d]", hbtr->x, hbtr->y, hbtr->z,
		    XYtoSector(hbtr->x, hbtr->y)+1);


	    Hv_ChangeFeedback(View, VERTEX, txt);
	    sprintf(txt, "%6.2f %6.2f %6.2f", hbtr->px/P, hbtr->py/P, hbtr->pz/P);
	    Hv_ChangeFeedback(View, DIRCOSINE, txt);
	    StandardIntChangeFeedback(View,   CHARGE,     (int)(hbtr->q),   NULL);
	    return;
	}

	if ((foundtrack) && (tbtr != NULL)) {

	    StandardIntChangeFeedback(View,   TRACKID,     tbtr->itr_sec % 100,   NULL);
	    StandardFloatChangeFeedback(View, CHI2,        tbtr->chi2,   NULL);

	    P =sqrt(tbtr->px*tbtr->px +
		tbtr->py*tbtr->py +
		tbtr->pz*tbtr->pz);

	    StandardFloatChangeFeedback(View, MOMENTUM,    P,      "GeV");
	    sprintf(txt, "%5.2f %5.2f %5.2f [vtx in sect %1d]", tbtr->x, tbtr->y, tbtr->z,
		    XYtoSector(tbtr->x, tbtr->y)+1);


	    Hv_ChangeFeedback(View, VERTEX, txt);
	    sprintf(txt, "%6.2f %6.2f %6.2f", tbtr->px/P, tbtr->py/P, tbtr->pz/P);
	    Hv_ChangeFeedback(View, DIRCOSINE, txt);
	    StandardIntChangeFeedback(View,   CHARGE,     (int)(tbtr->q),   NULL);
	    return;
	}

	else /* no track at all */
	    ClearFeedback(View, CLEAR_TRACK);
	

    } /* end sector view */
    
}


/* ------- ComponentFeedback ---- */

static void ComponentFeedback(Hv_View       View,
			      Hv_Point      pp)

/*
 * Used only by the sector view
 */
    
{
    Boolean    insomething = False;
    short      tdcl=-1, adcl=-1, tdcr=-1, adcr=-1;
    ViewData   viewdata;
    
    viewdata  = GetViewData(View);
    
    insomething = TaggerFeedback(View, pp, &tdcl, &adcl, &tdcr, &adcr);
    if (!insomething) {
	insomething = ScintFeedback(View, pp, &tdcl, &adcl, &tdcr, &adcr); 
	if (!insomething) {
	    Hv_ChangeFeedback(View, WHATSCINT, " ");
	    insomething = CerenkovFeedback(View, pp, &tdcl, &adcl);
	    if (!insomething) {
		Hv_ChangeFeedback(View, WHATCERENK, " ");
		insomething = CerenkovFeedback(View, pp, &tdcl, &adcl);
		if (!insomething) {
		    insomething = ShowerFeedback(View, pp, &tdcl, &adcl);
		    if (!insomething) {
			insomething = EC1Feedback(View, pp, &tdcl, &adcl, &tdcr, &adcr);
			if (!insomething) {
			    Hv_ChangeFeedback(View, WHATSHOWER, " ");
			    Hv_ChangeFeedback(View, WHATUVW, " ");
			    insomething = DCFeedback(View, pp, &tdcl, &adcl);
			    if (!insomething) {
				ClearFeedback(View, CLEAR_DC);
			    }
			}
		    }
		}
	    }
	}
    }
    PosDefIntChangeFeedback(View, TDCL, tdcl);
    PosDefIntChangeFeedback(View, ADCL, adcl);
    PosDefIntChangeFeedback(View, TDCR, tdcr);
    PosDefIntChangeFeedback(View, ADCR, adcr);
	
    if (!insomething)
	if (Hv_PointInRect(pp, viewdata->BeamLineItem->area))
	    UpdateMessageBox(View, "That's the beamline.");
	else {
	    if ((eventsource == CED_FROMFILE) && (bosFileName != NULL))
		UpdateMessageBox(View, bosFileName);
	}

}

/*-------- AnchorFeedback ------*/

static void AnchorFeedback(Hv_View       View,
			   Hv_Point      pp)
    
{
    ViewData         viewdata;
    short            xc, yc;
    float            distance, phi;
    float            Z1, X1, Y1, Z2, X2, Y2;
    
    viewdata = GetViewData(View);
    
    if (viewdata->AnchorItem == NULL) {
	StandardChangeFeedback(View, ANCHORDISTANCE, "No Anchor");
	return;
    }
    
    LocaltoCLASXYZ(View, pp.x, pp.y, &phi, &X1, &Y1, &Z1);
    
    Hv_GetItemCenter(viewdata->AnchorItem, &xc, &yc);
    LocaltoCLASXYZ(View, xc, yc, &phi, &X2, &Y2, &Z2);
    
    distance = Distance3D(X1, Y1, Z1, X2, Y2, Z2);
    StandardFloatChangeFeedback(View, ANCHORDISTANCE, distance, "cm");
}


/*------ FieldFeedback ------*/

static void FieldFeedback(Hv_View       View,
			  Hv_Point      pp)
{
    float    X, Y, Z, phi;
    fvect    B;
    float    Bmag;
    Boolean  inbounds;  
    char     text[60];
    
/* see if there is any field to display */
    
    if (fmap.fmapptr == NULL) {
	StandardChangeFeedback(View, BMAG,  "No Field Data");
	StandardChangeFeedback(View, BVECT, "No Field Data");
	return;
    }
    
    LocaltoCLASXYZ(View, pp.x, pp.y, &phi, &X, &Y, &Z);
    
    inbounds = BField(X, Y, Z, B, &Bmag);
    
    if (inbounds) {
	sprintf(text, "%-7.4f", Bmag);
	Hv_ChangeFeedback(View, BMAG, text);
	sprintf(text, "(%-5.3f, %-5.3f, %-5.3f)", B[0], B[1], B[2]);
	Hv_ChangeFeedback(View, BVECT, text);
    }
    else {
	StandardChangeFeedback(View, BMAG,  "Out of Bounds");
	StandardChangeFeedback(View, BVECT, "Out of Bounds");
    }
}



/*-------- MousePointWorldFeedback ------*/

static void MousePointWorldFeedback(Hv_View       View,
				    Hv_Point      pp)
    
{
    float            x, y, z;
    float            theta, r, phi;
    
    theSector = -1;
    
/* LocaltoCLASCoord will get 3D coordinates taking the value
   of phi into account */

    LocaltoCLASCoord(View, pp.x, pp.y, &phi, &x, &y, &z, &r, &theta);
    phi   *= Hv_RADTODEG;
    theta *= Hv_RADTODEG;
    
    StandardFloatChangeFeedback(View, ZPOSITION, z,     "cm");
    StandardFloatChangeFeedback(View, XPOSITION, x,     "cm");
    StandardFloatChangeFeedback(View, YPOSITION, y,     "cm");

    StandardFloatChangeFeedback(View, DISTANCE,  r,     "cm");
    StandardFloatChangeFeedback(View, ANGLE,     theta, "deg");
    StandardFloatChangeFeedback(View, PHI,       phi,   "deg");
    
    theSector = XYtoSector(x, y) + 1;
    StandardIntChangeFeedback(View, WHATSECTOR, theSector, NULL);
}

/*------ EC1Feedback -----------*/

static Boolean EC1Feedback(Hv_View       View,
			   Hv_Point      pp,
			   short        *tdcl,
			   short        *adcl,
			   short        *tdcr,
			   short        *adcr)
{
    ViewData    viewdata;
    Hv_Item     temp;
    short       sect;
    short       layer;
    Hv_Point    poly[4];
    char        text[60];
    int         j, k, olcount;
    char        bigtext[80];
    char        smalltext[20];
    int         strip;
    int         hashit;
    short       overlaps[40];
    
    viewdata = GetViewData(View);

    for (j = 0; j < 1; j++)
	if ((temp = viewdata->SectViewEC1Items[j]) != NULL) {
	    sect  = temp->user2;

/* for now, only italian cals in sects 1 & 2 */

	    if (sect > 1)
		return False;
	    if (Hv_PointInRegion(pp, temp->region)) {

/* now see what layer we are in */
		
		for (layer = 0; layer < 4; layer++) {
		    EC1LayerPolygon(temp, sect, layer, poly);
		    if (Hv_PointInPolygon(pp, poly, 4)) {

			if ((layer == 0) || (layer == 2)) {

/* now see the long strip */			

			    for (strip = 0; strip < 24; strip++) {
				EC1SectorViewLongStripPoly(temp, sect, layer, strip, poly);
				
				if (Hv_PointInPolygon(pp, poly, 4)) {

/* get the hit data */

				    GetSectViewEC1Hit(sect, layer+1, strip+1, tdcl, adcl, tdcr, adcr);
				    
				    sprintf(bigtext, "Italian Calorimeter sect %d layer: %d long strip: %d",
					    sect + 1, layer+1, strip+1);
				    UpdateMessageBox(View, bigtext);
				    
				    if (layer < 2)
					sprintf(text, "Italian Inner, layer: %d", layer+1);
				    else
					sprintf(text, "Italian Outer, layer: %d", layer+1);
				    
				    ChangeSharedFeedback(View, &showertage, EC_SLOT, text);
				    
				    sprintf(text, "%d (long <x>)", strip+1);
				    ChangeSharedFeedback(View, &striptagt, EC_SLOT, text);
				    
				    return True;
				}
			    }
			}  /* end layer 0 & 2 */

			else {  /* layers 1 & 3 */

/* now see the short strip */			

			    for (strip = 39; strip >= 0; strip--) {
				EC1SectorViewShortStripPoly(temp, sect, layer, strip, poly);
				
				if (Hv_PointInPolygon(pp, poly, 4)) {

/* since the area is small, strips overlap here */

				    overlaps[0] = strip;
				    olcount = 1;

				    for (k = overlaps[0]-1; k >= 0; k--) {
					EC1SectorViewShortStripPoly(temp, sect, layer, k, poly);
					
					if (Hv_PointInPolygon(pp, poly, 4)) {
					    overlaps[olcount] = k;
					    olcount++;
					}
					else
					    break;

				    }

/* get the hit data -- ist one if overlap*/

				    hashit = -1;

				    for (k = 0; k < olcount; k++) {
					GetSectViewEC1Hit(sect, layer+1, overlaps[k]+1, tdcl, adcl, tdcr, adcr);
					if ((*tdcl > -1) || (*adcl > -1) || (*tdcr > -1) || (*adcr > -1)) {
					    hashit = k;
					    break;
					}
				    }

				    if (olcount == 1) /* no overlap */
					sprintf(bigtext, "Italian LAEC sect %d layer: %d short strip: %d",
						sect + 1, layer+1, strip+1);

				    else { /* cannot resolve which strip */
					sprintf(bigtext, "LAEC sect %d layer: %d short strips: ",
						sect + 1, layer+1);
					for (k = 0; k < olcount; k++) {
					    sprintf(smalltext, " %-d", overlaps[k] + 1);
					    if (k < (olcount - 1))
						strcat(smalltext, ",");
					    strcat(bigtext, smalltext);
					}
					strcat(bigtext, " (can't resolve)");

				    }

				    UpdateMessageBox(View, bigtext);
				    
				    if (layer < 2)
					sprintf(text, "Italian Inner, layer: %d", layer+1);
				    else
					sprintf(text, "Italian Outer, layer: %d", layer+1);
				    
				    Hv_ChangeFeedback(View, WHATSHOWER, text);
				    
/* if have a hit, show the adc and tdc for that strip (possibly
   other, overlapping strips with hits are missed may fix later --
  if no hits, show all unresolvalble ids */


				    if (hashit > -1) {
					sprintf(text, "(short <y>) %d", overlaps[hashit]+1);
				    }
				    else {
					sprintf(text, "(short <y>) ");
					for (k = 0; k < olcount; k++) {
					    sprintf(smalltext, " %-d", overlaps[k] + 1);
					    if (k < (olcount - 1))
						strcat(smalltext, ",");
					    strcat(text, smalltext);
					}
				    }


				    Hv_ChangeFeedback(View, WHATUVW, text);
				    
				    return True;
				}
			    }
			}
		    }
		    
		} /* end layer loop */
	    } /* point in region */

	}

    return False;
}

/* ------- TaggerFeedback ---- */

static Boolean TaggerFeedback(Hv_View       View,
			     Hv_Point      pp,
			     short        *tdcl,
			     short        *adcl,
			     short        *tdcr,
			     short        *adcr)
{
    Hv_Item     tagger;
    ViewData    viewdata = GetViewData(View);
    Hv_Rect     rect, sr;
    short       yc, dum1, dum2;
    int         j;
    char        text[100];
    float       z, x, zd;
    Boolean     inside;

    if (!viewdata->displayTagger)
	return False;

    tagger = viewdata->TaggerItem;

    if (!Hv_PointInRect(pp, tagger->area))
	return False;


/* see if pp is in the e rect */

    GetTaggerMainERect(View, &rect);

    if (Hv_PointInRect(pp, &rect)) { /* in E Rect */

	Hv_LocalToWorld(View, &z, &x, pp.x, pp.y);

	yc = (rect.top + rect.bottom)/2; 
	zd = (viewdata->tg_axiszmax - viewdata->tg_axiszmin)/(Tagger.numEC/2);

	j = 1 + (viewdata->tg_axiszmax-z)/zd;
	
	if (pp.y < yc) {  /* odd */
	    j = 2*j - 1;
	    j = Hv_iMax(1, Hv_iMin(Tagger.numEC-1, j));
	}
	else {
	    j = 2*j;
	    j = Hv_iMax(2, Hv_iMin(Tagger.numEC, j));
	}

	sprintf(text, "%d", j);
	ChangeSharedFeedback(View, &showertage, TAG_SLOT, text);
	ChangeSharedFeedback(View, &striptagt, TAG_SLOT, " ");

	GetTaggerHit(j, -1, tdcl, &dum1, &dum2);
	if (*tdcl > -1)
	    sprintf(text, "E Counter %d    TDC = %d", j, *tdcl);
	else
	    sprintf(text, "E Counter %d", j);
	UpdateMessageBox(View, text);

	return True;
    }


/* see if pp is in the t rect */

    GetTaggerMainTRect(View, &rect);

    if (Hv_PointInRect(pp, &rect)) {


	inside = False;
	for (j = 1; j <= Tagger.numTC; j++) {
	    GetTaggerTRect(View, j, &sr); 
	    if (Hv_PointInRect(pp, &sr)) {
		inside = True;
		break;
	    }

	}

	if (inside) {

	    sprintf(text, "%d", j);
	    ChangeSharedFeedback(View, &showertage, TAG_SLOT, " ");
	    ChangeSharedFeedback(View, &striptagt, TAG_SLOT, text);


	    GetTaggerHit(-1, j, &dum1, tdcl, tdcr);
	    
	    if ((*tdcl > -1) || (*tdcr > -1))
		sprintf(text, "T Counter %d    tdc_left = %d     tdc_right = %d",
			j, Hv_iMax(0, *tdcl), Hv_iMax(0, *tdcr));
	    else
		sprintf(text, "T Counter %d", j);

	    UpdateMessageBox(View, text);
	}

	return True;

    }



/* if here, then just in inert part */

    UpdateMessageBox(View, "That's the photon tagger.");
    return True;

}

/* ------- ScintFeedback ---- */

static Boolean ScintFeedback(Hv_View       View,
			     Hv_Point      pp,
			     short        *tdcl,
			     short        *adcl,
			     short        *tdcr,
			     short        *adcr)
{
    ViewData    viewdata;
    Hv_Item     temp;
    short       sect;
    short       plane;
    short       i, minid, maxid, scint = -1;
    Hv_Point    poly[4];
    char        text[50];
    int         j;
    char        bigtext[80];
    
    viewdata = GetViewData(View);
    
    for (j = 0; j < NUM_SCINTPLANE2; j++)
	if ((temp = viewdata->ScintPlaneItems[j]) != NULL) {
	    if (Hv_PointInRegion(pp, temp->region)) {
		
/* now we know that we are in a plane. we need to determine which slab */
		
		plane = temp->user1;
		sect  = temp->user2;
		ScintSlabLimits(plane, sect, &minid, &maxid);
		
		for (i = minid; i <= maxid; i++) {
		    GetSlabPolygon(View, sect, i, poly);
		    if (Hv_PointInPolygon(pp, poly, 4)) {
			
/* now see if there is any "hit" */
			
			GetScintHit(sect, i, tdcl, adcl, tdcr, adcr);
			scint = i+1;
			break;
		    }
		}
		
		if (scint > 0) {
		    sprintf(bigtext, "scintillator: %-2d ", scint);
		    sprintf(text, "%2d (Panel: %1d)", scint, PanelFromID(scint));
		    
		    Hv_ChangeFeedback(View, WHATSCINT, text);
		    UpdateMessageBox(View, bigtext);
		}
		
		return True;
	    }  /* end point in region */
	} /* end item loop */
    
    return False;
}


/* ------- DCFeedback ---- */

static Boolean DCFeedback(Hv_View       View,
			  Hv_Point      pp,
			  short        *tdcl,
			  short        *adcl)
    
{
    ViewData    viewdata;
    Hv_Item     temp;
    short       sect;
    char        text[50];
    char        *text2;
    int         j;
    short       superlayer;
    short       layer, wire;
    float       occupancy;
    short       lay;
    int         nhit;

    viewdata = GetViewData(View);
    superlayer = -1;
    
    for (j = 0; j < NUM_SUPERLAYER2; j++)
	if ((temp = viewdata->DCItems[j]) != NULL) {
	    if (Hv_PointInRegion(pp, temp->region)) {
		superlayer = (short)temp->user1; /* 0.. 5*/
		sect = (short)temp->user2;
		
/* now see if there is any "hit" */
		
		GetDCHit(temp, pp, sect, superlayer, &occupancy, &nhit, &layer, &wire, tdcl);
		wire++;
		layer++;
		
		if (wire > 0) {
		    PosDefIntChangeFeedback(View, WHATWIRE, wire);
		    lay = 6*superlayer + layer; /* 1..36 */

/* dead wire ? */

		    if (IsDead(sect, superlayer / 2, superlayer % 2, layer-1, wire-1))
			sprintf(text, "%d (or %d/36) ** DEAD WIRE **", layer, lay);
		    else
			sprintf(text, "%d (or %d/36)", layer, lay);
		    Hv_ChangeFeedback(View, WHATLAYER, text);
		}
		
		if (occupancy > -0.01) {
		    sprintf(text, "%6.2f%% (%d hits)", occupancy, nhit);
		    Hv_ChangeFeedback(View, OCCUPANCY, text);
		}
		
		SuperLayerNameFeedback(View, superlayer, text);
		text2 = (char *)Hv_Malloc(30 + strlen(text));
		sprintf(text2, "Driftchamber superlayer: %s", text);
		UpdateMessageBox(View, text2);
		
		return True;
	    }  /* end point in region */
	} /* end item loop */
    
    return False;
}





/* ------- CerenkovFeedback ---- */

static Boolean CerenkovFeedback(Hv_View       View,
				Hv_Point      pp,
				short        *tdcl,
				short        *adcl)
    
{
    ViewData    viewdata;
    Hv_Item     temp;
    short       sect, cedid, id;
    Hv_Point    inpoly[4];
    char        text[50];
    int         j;
    Boolean     inner = False;
    
    viewdata = GetViewData(View);
    id = -1;
    
    for (j = 0; j < NUM_CERENK2; j++)
	if ((temp = viewdata->CerenkovItems[j]) != NULL) {
	    if (Hv_PointInRegion(pp, temp->region)) {
		cedid = temp->user1;
		sect = temp->user2;
		GetCerenkovInnerPolygon(View, sect, cedid, inpoly);
		inner = Hv_PointInPolygon(pp, inpoly, 4);
		id = 2*cedid + 1;
		if (inner)
		    id++;
		
/* now see if there is any "hit" */
		
		GetCerenkovHit(sect, id, tdcl, adcl);
		if (inner)
		    UpdateMessageBox(View, "That area represents a high \\gF\\d Cerenkov");
		else
		    UpdateMessageBox(View, "That area represents a low  \\gF\\d Cerenkov");
		
		if (id > 0) {
		    if (inner)
			sprintf(text, "%2d (high phi)", id);
		    else
			sprintf(text, "%2d ( low phi)", id);
		    
		    Hv_ChangeFeedback(View, WHATCERENK, text);
		}
		
		
		return True;
	    }  /* end point in region */
	} /* end item loop */
    
    return False;
}


/* ------- ShowerFeedback ---- */

static Boolean ShowerFeedback(Hv_View       View,
			      Hv_Point      pp,
			      short        *tdc,
			      short        *adc)


{
    ViewData    viewdata;
    Hv_Item     temp;
    short       sect, plane, type, ID;
    char        text[50];
    int         j;
    
    viewdata = GetViewData(View);
    
    for (j = 0; j < NUM_SHOWER2; j++)
	if ((temp = viewdata->ForwardECSectViewItems[j]) != NULL)
	    if (Hv_PointInRegion(pp, temp->region)) {
		plane = temp->user1;
		sect = temp->user2;
		
/* now see if there is any "hit" */
		
		GetSectViewShowerHit(temp, pp, sect, plane, &type, &ID, tdc, adc);
		
		if (plane == ECINNER)
		    sprintf(text, "Inner Foward EC ");
		else
		    sprintf(text, "Outer Foward EC ");
		
		UpdateMessageBox(View, text);
		Hv_ChangeFeedback(View, WHATSHOWER, text);
		
/* now the strip ID */
		
		if (ID < 0)
		    sprintf(text, " ");
		else
		    switch(type) {
		    case USTRIP:
			sprintf(text, "u = %d", ID);
			break;
			
		    case VSTRIP:
			sprintf(text, "v = %d", ID);
			break;
			
		    case WSTRIP:
			sprintf(text, "w = %d", ID);
			break;
			
		    default:
			break;
		    }
		
		Hv_ChangeFeedback(View, WHATUVW, text);
		
		return True;
	    }  /* end point in region */
    return False;
}


/*--------- StandardColorButton --------*/

Hv_Item StandardColorButton(Hv_View    View,
			    Hv_Item   parent,
			    char     *label,
			    short     color,
			    int       pos,
			    short     gap,
			    Boolean   allownocol)

{
  Hv_Item       Item;

  Item = Hv_VaCreateItem(View,
			 Hv_PARENT,             parent,
                         Hv_SINGLECLICK,        ColorButtonCallback,
                         Hv_DOUBLECLICK,        ColorButtonCallback,
			 Hv_TYPE,               Hv_COLORBUTTONITEM,
			 Hv_TEXT,               label,
			 Hv_FONT,               Hv_timesBold11,
			 Hv_FILLCOLOR,          color,
			 Hv_RELATIVEPLACEMENT,  pos,
			 Hv_PLACEMENTGAP,       gap,
                         Hv_ALLOWNOCOLOR,       (short)(allownocol),
			 NULL);

  return Item;
}

/*------- ColorButtonCallback -----*/

void  ColorButtonCallback(Hv_Event    hvevent)

{
  Hv_View    View = hvevent->view;
  Hv_Item    Item = hvevent->item;
  ViewData   viewdata;

  short      oldcolor, newcolor;

  viewdata = GetViewData(View);
  oldcolor = Hv_GetColorButtonColor(Item);
  Hv_EditColorButtonItem(hvevent);
  newcolor = Hv_GetColorButtonColor(Item);

  if (oldcolor == newcolor)
    return;

  if (Item == viewdata->ColorButtons[0]) {
      dchitcolor = Hv_GetColorButtonColor(Item);
  }

  else if (Item == viewdata->ColorButtons[1]) {
      dcnoisecolor = Hv_GetColorButtonColor(Item);
  }

  else if (Item == viewdata->ColorButtons[2]) {
      dcbadtimecolor = Hv_GetColorButtonColor(Item);
  }

  else if (Item == viewdata->ColorButtons[3]) {
      dcfromdc1color = Hv_GetColorButtonColor(Item);
  }



  Hv_DrawViewHotRect(View);

}


/*-------- AddDCColorButtons ---------*/

void AddDCColorButtons(Hv_View View,
		       short   top,
		       Hv_Item box) {

    ViewData  viewdata = GetViewData(View);

    viewdata->ColorButtons[0] = Hv_VaCreateItem(View,
						Hv_PARENT,             box,
						Hv_LEFT,               box->area->left + 10,
						Hv_SINGLECLICK,        ColorButtonCallback,
						Hv_DOUBLECLICK,        ColorButtonCallback,
						Hv_TOP,                top,
						Hv_TYPE,               Hv_COLORBUTTONITEM,
						Hv_TEXT,               "DC0 Hit",
						Hv_FONT,               Hv_timesBold11,
						Hv_FILLCOLOR,          dchitcolor,
						Hv_ALLOWNOCOLOR,       (short)False,
						NULL);



    viewdata->ColorButtons[1] = StandardColorButton(View,
						    box,
						    "DC Noise",
						    dcnoisecolor,
						    Hv_POSITIONRIGHT,
						    70,
						    False);


    viewdata->ColorButtons[2] = Hv_VaCreateItem(View,
						Hv_PARENT,             box,
						Hv_LEFT,               box->area->left + 10,
						Hv_SINGLECLICK,        ColorButtonCallback,
						Hv_DOUBLECLICK,        ColorButtonCallback,
						Hv_TOP,                viewdata->ColorButtons[0]->area->bottom + 15,
						Hv_TYPE,               Hv_COLORBUTTONITEM,
						Hv_TEXT,               "Out of time",
						Hv_FONT,               Hv_timesBold11,
						Hv_FILLCOLOR,          dcbadtimecolor,
						Hv_ALLOWNOCOLOR,       (short)False,
						NULL);


    viewdata->ColorButtons[3] = StandardColorButton(View,
						    box,
						    "DC1",
						    dcfromdc1color,
						    Hv_POSITIONRIGHT,
						    70,
						    False);


    box->area->bottom = viewdata->ColorButtons[3]->area->bottom+10;
    Hv_FixRectWH(box->area);

}

/*-------- DC1Controls ----------*/

Hv_Item  DC1Controls(Hv_View View,
		     short left,
		     short top,
		     short width,
		     short font) {


    ViewData viewdata = GetViewData(View);
    
    return  Hv_VaCreateItem(View,
			    Hv_TYPE,                Hv_CHOICESETITEM,
			    Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT+Hv_FRAMEAREA,
			    Hv_ORIENTATION,         Hv_VERTICAL,
			    Hv_LEFT,                left,
			    Hv_TOP,                 top,
			    Hv_WIDTH,               width,
			    Hv_COLOR,               Hv_gray9,
			    Hv_ARMCOLOR,            radioarmcolor,
			    Hv_FILLCOLOR,           boxcolor,
			    Hv_FONT,                Hv_fixed2,
			    Hv_OPTION,             " Only \"Track\" DC1",
			    Hv_OPTION,             " All DC1 Banks",
			    Hv_NOWON,               viewdata->dc1control, 
			    Hv_FONT,                font,
			    Hv_SINGLECLICK,         DC1ControlCB,
			    NULL);

}




