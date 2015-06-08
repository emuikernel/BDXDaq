/*
  ----------------------------------------------------
  -							
  -  File:    scintview.c	
  -							
  -  Summary:						
  -           sets up "fanned scint" views
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

#define  ANGDEL  8.0
#define  STARTSIZE 200.0

static void StartCounterFeedback(Hv_View View,
				 Hv_Point pp);

static void DrawStartItem(Hv_Item Item,
			  Hv_Region region);

static Hv_Item ScintViewOptionButtons(Hv_View View,
				      Hv_Item parent,
				      short   top);


static Hv_Item AddScintFeedback(Hv_View View,
				Hv_Item pitem);


extern short optionarmcolor;
extern short radioarmcolor;
extern short boxcolor;
extern short fbcolor;

/* for use in shared  feedbacks */


/*--------- ScintViewSetup -------*/

void ScintViewSetup(Hv_View View){

    Hv_Item   Item;
    Hv_Item   rbox = NULL;
    Hv_Item   box1 = NULL;
    Hv_Item   Feedback;
    Hv_Item   sep1;

    ViewData  viewdata = GetViewData(View);


/* a single-event accumulated event radio set */

    rbox = Hv_VaCreateItem(View,
			   Hv_TYPE,                Hv_CHOICESETITEM,
			   Hv_ORIENTATION,         Hv_VERTICAL,
			   Hv_GAP,                 4,
			   Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT+Hv_FRAMEAREA,
			   Hv_NOWON,               SINGLEEVENT,
			   Hv_COLOR,               Hv_gray9,
			   Hv_ARMCOLOR,            radioarmcolor,
			   Hv_LEFT,                View->hotrect->right + 5,
			   Hv_TOP,                 View->hotrect->top - 4,
			   Hv_FILLCOLOR,           boxcolor,
			   Hv_FONT,                Hv_fixed2,
			   Hv_OPTION,             " Single Event",
			   Hv_OPTION,             " Accumulated (counts)",
/*			   Hv_OPTION,             " Accumulated (avg TDC)", */
			   Hv_SINGLECLICK,         ShowWhatCB,
			   NULL);


    sep1 = StandardSeparator(View, rbox, 6);

    Item = ScintViewOptionButtons(View, rbox, sep1->area->bottom + 5);
    rbox->area->bottom = Item->area->bottom + 8;
    Hv_FixRectWH(rbox->area);

/* rainbow scales */

    box1 = Hv_VaCreateItem(View,
			   Hv_TYPE,                Hv_BOXITEM,
			   Hv_GAP,                 1,
			   Hv_HEIGHT,              70,
			   Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT,
			   Hv_FILLCOLOR,           boxcolor,
			   Hv_PLACEMENTITEM,       rbox,
			   Hv_RELATIVEPLACEMENT,   Hv_POSITIONBELOW,
			   Hv_FONT,                Hv_timesBold11,
			   Hv_FONT2,               Hv_timesBold11,
			   NULL);


    Item = Hv_VaCreateItem(View,
			   Hv_PARENT,            box1,
			   Hv_TYPE,              Hv_RAINBOWITEM,
/*			   Hv_EDITABLE,          Hv_LIMITEDEDIT, */
			   Hv_DRAWCONTROL,       Hv_REDRAWHOTRECTWHENCHANGED,
			   Hv_WIDTH,             15,
			   Hv_NUMLABELS,         2,
			   Hv_PRECISION,         0,
			   Hv_MINCOLOR,          Hv_yellow+20,
			   Hv_MAXCOLOR,          Hv_red,
			   Hv_MINVALUE,          0,
			   Hv_MAXVALUE,          500,
			   Hv_COLORWIDTH,        2,
			   Hv_TITLE,             "Counts",
			   Hv_LEFT,              box1->area->left+25,
			   Hv_TOP,               box1->area->top+25,
			   Hv_FONT,              Hv_timesBold11,
			   Hv_FONT2,             Hv_timesBold11,
			   NULL);
    viewdata->AccumulatedHitsItem[0] = Item;

    Item = Hv_VaCreateItem(View,
			   Hv_PARENT,            box1,
			   Hv_TYPE,              Hv_RAINBOWITEM,
/*			   Hv_EDITABLE,          Hv_LIMITEDEDIT, */
			   Hv_DRAWCONTROL,       Hv_REDRAWHOTRECTWHENCHANGED,
			   Hv_WIDTH,             15,
			   Hv_NUMLABELS,         2,
			   Hv_PRECISION,         0,
			   Hv_MINCOLOR,          Hv_yellow+20,
			   Hv_MAXCOLOR,          Hv_red,
			   Hv_MINVALUE,          0,
			   Hv_MAXVALUE,          1500,
			   Hv_COLORWIDTH,        2,
			   Hv_TITLE,             "ADC",
			   Hv_LEFT,              box1->area->left+25,
			   Hv_TOP,               box1->area->top+25,
			   Hv_FONT,              Hv_timesBold11,
			   Hv_RELATIVEPLACEMENT,   Hv_POSITIONRIGHT,
			   Hv_PLACEMENTGAP,        40,
			   Hv_FONT2,             Hv_timesBold11,
			   NULL);

    viewdata->ADCRainbowItem = Item;

/* now the feedback */
    
    Feedback = AddScintFeedback(View, box1);
    Hv_FitFeedback(View, 10);

    rbox->area->right = Feedback->area->right;
    Hv_FixRectWH(rbox->area);

    Hv_SeparatorResizeToParent(sep1);

    box1->area->right = Feedback->area->right;
    Hv_FixRectWH(box1->area);

    Item = MallocSectorHex(View); 
    Item->color = Hv_skyBlue - 3;
    AddScintillatorItems(View, Item);
    Item = MallocStart(View);
}


/*------- StartCounterSubPolys -------*/

void StartCounterSubPoly(Hv_View   View,
			 short     sect,
			 Hv_Point  *poly1,
			 Hv_Point  *poly2) {

    Hv_FPoint  fpts[4];

/* poly1 is the inner 3 point poly */

    fpts[0].h = 0.0;
    fpts[0].v = 0.50*STARTSIZE;

    fpts[1].h = 0.0;
    fpts[1].v = 0.50*STARTSIZE;

    fpts[2].h = 0.0;
    fpts[2].v = 0.0;


    Hv_RotateFPoint(Hv_DEGTORAD*(((2-sect)*60.0)-ANGDEL),  fpts+1);
    Hv_RotateFPoint(Hv_DEGTORAD*(((1-sect)*60.0)+ANGDEL),  fpts);

    Hv_WorldPolygonToLocalPolygon(View, 3, poly1, fpts);

/* poly 2 is the outer 4 pt poly */

    fpts[0].h = 0.0;
    fpts[0].v = 0.55*STARTSIZE;

    fpts[1].h = 0.0;
    fpts[1].v = 0.55*STARTSIZE;

    fpts[2].h = 0.0;
    fpts[2].v = 0.85*STARTSIZE;

    fpts[3].h = 0.0;
    fpts[3].v = 0.85*STARTSIZE;

    Hv_RotateFPoint(Hv_DEGTORAD*(((2-sect)*60.0)-ANGDEL),  fpts+1);
    Hv_RotateFPoint(Hv_DEGTORAD*(((2-sect)*60.0)-ANGDEL),  fpts+2);
    Hv_RotateFPoint(Hv_DEGTORAD*(((1-sect)*60.0)+ANGDEL),  fpts);
    Hv_RotateFPoint(Hv_DEGTORAD*(((1-sect)*60.0)+ANGDEL),  fpts+3);

    Hv_WorldPolygonToLocalPolygon(View, 4, poly2, fpts);

}



/*------ MallocStart -------*/

Hv_Item MallocStart(Hv_View View) {

  Hv_Item      Item;
  Hv_FPoint    *fpts;
  short        npts = 12;
  Hv_FRect     world;
  float        xc, yc;
  int          i, j;
  ViewData     viewdata;

  viewdata = GetViewData(View);

  fpts = (Hv_FPoint *)Hv_Malloc(npts*sizeof(Hv_FPoint));

  Hv_SetFRect(&world, -STARTSIZE, STARTSIZE, -STARTSIZE, STARTSIZE);

  xc = (world.xmin + world.xmax)/2.0;
  yc = (world.ymin + world.ymax)/2.0;

/* set the first point RELATIVE TO THE CENTER */

  fpts[0].h = 0.0;
  fpts[0].v = 0.97*(world.ymax - yc);

  for (i = 1; i < 6; i++) {
    fpts[i].h = fpts[0].h;
    fpts[i].v = fpts[0].v;
  }
    
  Hv_RotateFPoint(Hv_DEGTORAD*ANGDEL, fpts);
  Hv_RotateFPoint(Hv_DEGTORAD*(60.0-ANGDEL),  fpts+1);
  Hv_RotateFPoint(Hv_DEGTORAD*(60.0+ANGDEL),  fpts+2);
  Hv_RotateFPoint(Hv_DEGTORAD*(120.0-ANGDEL), fpts+3);
  Hv_RotateFPoint(Hv_DEGTORAD*(120.0+ANGDEL), fpts+4);
  Hv_RotateFPoint(Hv_DEGTORAD*(180.0-ANGDEL), fpts+5);

  for (i = 6; i < 12; i++) {
    j = 11 - i;
    fpts[i].h = -fpts[j].h;
    fpts[i].v = fpts[j].v;
  }

/* now add the center offset back in */


  for (i = 0; i < 12; i++) {
    fpts[i].h += xc;
    fpts[i].v += yc;
  }

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,         Hv_WORLDPOLYGONITEM,
			 Hv_TAG,          STARTITEM,
			 Hv_NUMPOINTS,    npts,
			 Hv_DATA,         fpts,
			 Hv_NUMROWS,      1,
			 Hv_NUMCOLUMNS,   1,
			 Hv_COLOR,        Hv_cadetBlue,
			 Hv_FILLCOLOR,    -1,
			 Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			 Hv_DRAWCONTROL,  Hv_ZOOMABLE + Hv_INBACKGROUND,
			 Hv_AFTEROFFSET,  Hv_PolygonAfterOffset,
			 Hv_FIXREGION,    Hv_FixPolygonRegion,
			 Hv_USERDRAW,     DrawStartItem,
			 NULL);


  viewdata->SectorHexItem = Item;
  Hv_SetViewBrickWallItem(Item); 


  return Item;
}


/*--------- splitStartCounterPoly2 ----------*/

void splitStartCounterPoly2(Hv_Point *poly2,
			    Hv_Point poly[4][4]) {

  int i, j;
  double t, u, h0, h1, delh_inner, v0, v1, delv_inner, delh_outer, delv_outer;
  

  delh_inner = (double)(poly2[0].x - poly2[1].x);
  delv_inner = (double)(poly2[0].y - poly2[1].y);
  
  delh_outer = (double)(poly2[3].x - poly2[2].x);
  delv_outer = (double)(poly2[3].y - poly2[2].y);
  
  for (i = 0; i < 4; i++) {
    t = i * 0.25;
    u = t + 0.25;

    h0 = poly2[1].x + t*delh_inner;
    h1 = poly2[1].x + u*delh_inner;

    v0 = poly2[1].y + t*delv_inner;
    v1 = poly2[1].y + u*delv_inner;

    poly[i][1].x = (short)h0;
    poly[i][0].x = (short)h1;

    poly[i][1].y = (short)v0;
    poly[i][0].y = (short)v1;


    h0 = poly2[2].x + t*delh_outer;
    h1 = poly2[2].x + u*delh_outer;

    v0 = poly2[2].y + t*delv_outer;
    v1 = poly2[2].y + u*delv_outer;

    poly[i][2].x = (short)h0;
    poly[i][3].x = (short)h1;

    poly[i][2].y = (short)v0;
    poly[i][3].y = (short)v1;


  }

}




/*--------- splitStartCounterPoly1 ----------*/

void splitStartCounterPoly1(Hv_Point *poly1,
			    Hv_Point poly[4][3]) {

  int i, j;
  double t, u, h0, h1, delh, v0, v1, delv;
  
  /* start all points the same */

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 3; j++) {
      poly[i][j].x = poly1[j].x;
      poly[i][j].y = poly1[j].y;
    }
  }

  delh = (double)(poly1[0].x - poly1[1].x);
  delv = (double)(poly1[0].y - poly1[1].y);
  
  for (i = 0; i < 4; i++) {
    t = i * 0.25;
    u = t + 0.25;

    h0 = poly1[1].x + t*delh;
    h1 = poly1[1].x + u*delh;

    v0 = poly1[1].y + t*delv;
    v1 = poly1[1].y + u*delv;

    poly[i][1].x = (short)h0;
    poly[i][0].x = (short)h1;

    poly[i][1].y = (short)v0;
    poly[i][0].y = (short)v1;

  }
  
}


/*-------- DrawStartItem ---------*/

static void DrawStartItem(Hv_Item Item,
			  Hv_Region region) {

    short sect, j;
    Hv_Point poly1[3];
    Hv_Point poly2[4];
    Hv_Point subpoly1[4][3];
    Hv_Point subpoly2[4][4];
    short colors[2] = {Hv_gray12, Hv_gray13};

    DrawSectorHexItem(Item, region);
    
    for (sect = 0; sect < 6; sect++) {
      StartCounterSubPoly(Item->view, sect, poly1, poly2);
      /*      Hv_FillPolygon(poly1, 3, True, Hv_gray12, Hv_black); */
      /*      Hv_FillPolygon(poly2, 4, True, Hv_gray13, Hv_black);*/
      
      splitStartCounterPoly1(poly1, subpoly1);
      splitStartCounterPoly2(poly2, subpoly2);

      for (j = 0; j < 4; j++) {
	Hv_FillPolygon(subpoly1[j], 3, True, colors[j%2], Hv_black); 
	Hv_FillPolygon(subpoly2[j], 4, True, colors[j%2], Hv_black); 
     }

    }
    
}



/*-------- ScintViewFeedback --------*/

void ScintViewFeedback(Hv_View  View,
		       Hv_Point pp) {


    ViewData    viewdata;
    Hv_Item     temp;
    short       sect;
    char        text[50];
    int         j;
    short       scint = -1;
    float       x, y, z, r, theta, phi;
    short       tdcl, adcl, tdcr, adcr;

    viewdata = GetViewData(View);

    sect = -1;

/* start counter? */

    for (j = 0; j < NUM_SECT; j++)
	if ((temp = viewdata->FannedScintItems[j]) != NULL)
	    if (Hv_PointInRegion(pp, temp->region)) {
		sect = (short)temp->user2;
		
		
		GetScintCoordinates(View, pp, sect,
				    &scint, &x, &y, &z,
				    &r, &theta, &phi);
		
		
		
	    }

    if (sect > -1) {
	PosIntChangeFeedback(View, WHATSECTOR, sect+1);
    }

/* note scint is not a c index */

    if (scint < 1) {
	ClearFeedback(View, CLEAR_SCINT);
	StartCounterFeedback(View, pp);
	return;
    }
    
    else {
	ClearFeedback(View, CLEAR_START);
	PosIntChangeFeedback(View, WHATSCINT, scint);
	StandardFloatChangeFeedback(View, ANGLE,       theta,  "deg");
	StandardFloatChangeFeedback(View, DISTANCE,    r,      "cm");
	StandardFloatChangeFeedback(View, ZPOSITION,   z,      "cm");
	StandardFloatChangeFeedback(View, XPOSITION,   x,      "cm");
	StandardFloatChangeFeedback(View, YPOSITION,   y,      "cm");
	StandardFloatChangeFeedback(View, PHI,         phi,    "deg");

/* showing a single event or accumulated? */

	
	if (viewdata->showwhat == SINGLEEVENT) {

	    Hv_ChangeFeedback(View, ACCUMCNT, " ");

	    GetScintViewHit(sect, 
			    scint,
			    &tdcl, &adcl, &tdcr, &adcr);
	
	    PosDefIntChangeFeedback(View, TDCL, tdcl);
	    PosDefIntChangeFeedback(View, ADCL, adcl);
	    PosDefIntChangeFeedback(View, TDCR, tdcr);
	    PosDefIntChangeFeedback(View, ADCR, adcr);

	} /* end single event */

	else {
	    Hv_ChangeFeedback(View, TDCL, " ");
	    Hv_ChangeFeedback(View, ADCL, " ");
	    Hv_ChangeFeedback(View, TDCR, " ");
	    Hv_ChangeFeedback(View, ADCR, " ");

	    sprintf(text, "%d (%d evnts)",
		    accsc[sect].counts[scint-1],
		    (int)accumEventCount);
	    Hv_ChangeFeedback(View, ACCUMCNT, text);
	}

	
    } /* end of good scint */
}

/*-------- StartCounterFeedback -----------*/

static void StartCounterFeedback(Hv_View View,
				 Hv_Point pp) {

    short      sect = -1;
    short      sc_id = -1;
    short      i, j;
    Hv_Point   poly1[3];
    Hv_Point   poly2[4];
    STN0DataPtr  hitsN0;
    STN1DataPtr  hitsN1;
    ViewData   viewdata = GetViewData(View);
    Hv_Point   subpoly1[4][3];
    Hv_Point   subpoly2[4][4];


    for (i = 0; i < 6; i++) {
	StartCounterSubPoly(View, i, poly1, poly2);
	if (Hv_PointInPolygon(pp, poly1, 3)) {
	    sect = i;
	    splitStartCounterPoly1(poly1, subpoly1);
	    for (j = 0; j < 4; j++) {
	      if (Hv_PointInPolygon(pp, subpoly1[j], 3)) {
		sc_id = 1 + (sect*4) + j;
		break;
	      }
	    }
	    break;
	}
	if (Hv_PointInPolygon(pp, poly2, 4)) {
	    sect = i;
	    splitStartCounterPoly2(poly2, subpoly2);
	    for (j = 0; j < 4; j++) {
	      if (Hv_PointInPolygon(pp, subpoly2[j], 4)) {
		sc_id = 1 + (sect*4) + j;
		break;
	      }
	    }
	    break;
	}

    }


    if ((sect == -1) || (sc_id < 1)) {
	ClearFeedback(View, CLEAR_START);
	UpdateMessageBox(View, " ");
	return;
    }


    PosIntChangeFeedback(View, WHATSECTOR, sect+1);
    PosIntChangeFeedback(View, WHATSTARTCNT, sc_id);

    if ((sect < 0) || ((bosIndexSTN0 == 0) && (bosIndexSTN1 == 0))) {
	return;
    }

    hitsN0 = bosSTN0;
    for (i = 0; i < bosNumSTN0; i++) {
	if (sc_id == (short)(hitsN0->id)) {


	    if (viewdata->showwhat == SINGLEEVENT) {
		Hv_ChangeFeedback(View, ACCUMCNT, " ");
		PosDefIntChangeFeedback(View, TDC, hitsN0->tdc);
	    } /* end single event */


	    break;
	}
	hitsN0++;
    }

    hitsN1 = bosSTN1;
    for (i = 0; i < bosNumSTN1; i++) {
	if (sc_id == (short)(hitsN1->id)) {


	    if (viewdata->showwhat == SINGLEEVENT) {
		Hv_ChangeFeedback(View, ACCUMCNT, " ");
		PosDefIntChangeFeedback(View, ADC, hitsN1->adc);
	    } /* end single event */


	    break;
	}
	hitsN1++;
    }

}


/*-------- ScintViewInitSharedFeedback -----*/

void ScintViewInitSharedFeedback() {
}



/*------ ScintViewOptionButtons ------*/

static Hv_Item ScintViewOptionButtons(Hv_View View,
				      Hv_Item parent,
				      short   top)
{
    ViewData      viewdata;
    Hv_Item       Item;
    
    viewdata = GetViewData(View);
    
    viewdata->displaySC = False;

    Item = Hv_VaCreateItem(View,
			   Hv_PARENT,             parent,
			   Hv_TYPE,               Hv_OPTIONBUTTONITEM,
			   Hv_CONTROLVAR,         (void *)(&(viewdata->displaySC)),
			   Hv_LEFT,               parent->area->left + 10,
			   Hv_TOP,                top,
			   Hv_TEXT,               " Strip Outlines",
			   Hv_FONT,               Hv_fixed2,
			   Hv_ARMCOLOR,           optionarmcolor,
			   Hv_SINGLECLICK,        OptionButtonCallback,
			   NULL);


    return Item;
}

/*----- AddScintFeedback ----------*/

static Hv_Item AddScintFeedback(Hv_View View,
				Hv_Item pitem)
    
{
    Hv_Item   Item;
    short fbfont = Hv_fixed2;
    
    Item = Hv_VaCreateItem(View,
			   Hv_TYPE,                Hv_FEEDBACKITEM,
			   Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT,
			   Hv_FEEDBACKDATACOLS,    16,
			   Hv_FILLCOLOR,           fbcolor,
			   Hv_RELATIVEPLACEMENT,   Hv_POSITIONBELOW,
			   Hv_PLACEMENTITEM,       pitem,
			   Hv_PLACEMENTGAP,        0,
			   Hv_FEEDBACKENTRY,  EVSOURCE,         fbfont, Hv_black,  fb_text42, 
			   Hv_FEEDBACKENTRY,  TRIGGERFILTER,    fbfont, Hv_black,  fb_text53, 
			   Hv_FEEDBACKENTRY,  EVENT,            fbfont, Hv_maroon, fb_text2, 
			   Hv_FEEDBACKENTRY,  WHATSECTOR,       fbfont, Hv_maroon, fb_text1, 
			   Hv_FEEDBACKENTRY,  Hv_SEPARATORTAG,
			   Hv_FEEDBACKENTRY,  ZPOSITION,        fbfont, Hv_black, fb_text4, 
			   Hv_FEEDBACKENTRY,  XPOSITION,        fbfont, Hv_black, fb_text5, 
			   Hv_FEEDBACKENTRY,  YPOSITION,        fbfont, Hv_black, fb_text6, 
			   Hv_FEEDBACKENTRY,  DISTANCE,         fbfont, Hv_black, fb_text7, 
			   Hv_FEEDBACKENTRY,  ANGLE,            fbfont, Hv_black, fb_text8, 
			   Hv_FEEDBACKENTRY,  PHI,              fbfont, Hv_black, fb_text9, 
			   Hv_FEEDBACKENTRY,  Hv_SEPARATORTAG,
			   Hv_FEEDBACKENTRY,  WHATSCINT,        fbfont, Hv_darkBlue, fb_text12, 
			   Hv_FEEDBACKENTRY,  TDCL,             fbfont, Hv_red, fb_text20, 
			   Hv_FEEDBACKENTRY,  ADCL,             fbfont, Hv_red, fb_text21, 
			   Hv_FEEDBACKENTRY,  TDCR,             fbfont, Hv_red, fb_text22, 
			   Hv_FEEDBACKENTRY,  ADCR,             fbfont, Hv_red, fb_text23, 
			   Hv_FEEDBACKENTRY,  Hv_SEPARATORTAG,
			   Hv_FEEDBACKENTRY,  WHATSTARTCNT,     fbfont, Hv_darkBlue, fb_text55, 
			   Hv_FEEDBACKENTRY,  TDC ,             fbfont, Hv_red, fb_text56, 
			   Hv_FEEDBACKENTRY,  ADC,              fbfont, Hv_red, fb_text57, 
			   Hv_FEEDBACKENTRY,  Hv_SEPARATORTAG,
			   Hv_FEEDBACKENTRY,  ACCUMCNT,         fbfont, Hv_black, fb_text40, 
			   NULL);
    
    
    return Item;
}

#undef  ANGDEL
#undef  STARTSIZE
