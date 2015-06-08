/*
----------------------------------------------------
-							
-  File:    tagger.c				
-							
-  Summary:						
-           Tagger Drawing
-					       
-                     				
-  Revision history:				
-                     				
-  Date       Programmer     Comments		
------------------------------------------------------
-  8/19/97	DPH		Initial Version	
-                                              	
------------------------------------------------------
*/

#include "ced.h"


/* ------ local prototypes -------*/

static void DrawTCRect(Hv_View View,
		       int     id);

static float effectiveElow(int tid);

static float effectiveEhigh(int tid);

static void Hv_CheckTaggerEnlarge(Hv_Item    Item,
				  Hv_Point   StartPP,
				  Boolean    ShowCorner,
				  Boolean   *enlarged);

static void SetGeo(Hv_Item Item);

static void Hv_TaggerAfterDrag(Hv_Item  Item,
			       short   dh,
			       short   dv);

static void DrawString(short x,
		       short y,
		       char *text,
		       int   opt);

static void DrawTagger(Hv_Item Item,
		       Hv_Region region);

static void EditTagger(Hv_Event event);

static Boolean correspondingT(int eid);

/*------- GetTaggerCoordinates -------*/

void  GetTaggerCoordinates(Hv_View   View,
			   Hv_Point  pp, 
			   short     *ec,
			   short     *tc) {
}

static Boolean correspondingT(int eid) {

/* see if there is a t counter hit that
  has this ecounter id (not C based) in
 its range */

    TAGTDataPtr    thits;
    int            j;
    int            num;
    unsigned char  id, dummy;


    thits = bosTAGT;
    num = bosNumTAGT;

    if ((thits != NULL) && (num > 0)) {
	for (j = 0; j < num; j++) {
	    BreakShort(thits->id, &id, &dummy);

	    if ((id > 1) && (id <= Tagger.numTC)) {
		id--;

fprintf(stderr, "eid: %d  lo: %d  hi: %d\n", eid, Tagger.e_lo1[id], Tagger.e_hi1[id]); 
		if ((eid >= Tagger.e_lo1[id]) && (eid <= Tagger.e_hi1[id])) { 
		    return True;
		}
	    }
	    thits++;
	}
    }
    

    return False;

}


/*-------- DrawTaggerHits ---------*/

void DrawTaggerHits(Hv_View   View,
		    Hv_Region region) {

    TAGEDataPtr    ehits;
    TAGTDataPtr    thits;
    int            j;
    int            num;
    unsigned char  id, dummy;
    Hv_Rect        rect;
    short          yc;
    ViewData       viewdata = GetViewData(View);

/* first the E counters */

    ehits = bosTAGE;
    num = bosNumTAGE;


    if (!viewdata->displayTagger)
	return;

    if ((ehits != NULL) && (num > 0)) {
	GetTaggerMainERect(View, &rect);

	for (j = 0; j < num; j++) {
	    if ((ehits->id > 0) && (ehits->id <= Tagger.numEC)) { 
		GetTaggerERect(View, ehits->id, &rect);
		Hv_ShrinkRect(&rect, 0, 1);

		if (correspondingT(ehits->id)) {
		    fprintf(stderr, "FOUND t for e = %d\n", ehits->id);
		    Hv_FillRect(&rect, tageinanytrangecolor);
		}
		else {
		    fprintf(stderr, "did NOT find t for e = %d\n", ehits->id);
		    Hv_FillRect(&rect, tageinnotrangecolor);
		}
	    }
	    ehits++;
	}
    }


/* now the T counters */

    thits = bosTAGT;
    num = bosNumTAGT;
    
    if ((thits != NULL) && (num > 0)) {
	GetTaggerMainTRect(View, &rect);

	for (j = 0; j < num; j++) {
	    BreakShort(thits->id, &id, &dummy);
	    if ((id > 0) && (id <= Tagger.numTC)) { 

		GetTaggerTRect(View, id, &rect);

		if ((thits->tdcleft > 0) && (thits->tdcright > 0)) {
		    Hv_FillRect(&rect, Hv_darkGreen);
		}
		else {
		    yc = (rect.top + rect.bottom)/2;
		    if (thits->tdcleft > 0) {
			rect.bottom = yc-2;
			Hv_FixRectWH(&rect);
			Hv_FillRect(&rect, Hv_red);
		    }
		    else if (thits->tdcright > 0) {
			rect.top = yc+2;
			Hv_FixRectWH(&rect);
			Hv_FillRect(&rect, Hv_red);
		    }
		}
		DrawTCRect(View, id);
	    }
	    thits++;
	}
    }

}



/*-------- MallocTagger ----------*/

Hv_Item MallocTagger(Hv_View  View) {

/* a tagger is alloced for each sector view. 
   it is placed initially at an arbitrary place,
   but it can be dragged about and resized */

  Hv_Item      Item;

  char         balloon[60];
  Hv_WorldRectData     *wrect;
  ViewData     viewdata = GetViewData(View);

  sprintf(balloon, "Photon Tagger");

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,         Hv_WORLDRECTITEM,
			 Hv_AFTERDRAG,    Hv_TaggerAfterDrag,
			 Hv_CHECKENLARGE, Hv_CheckTaggerEnlarge,
			 Hv_TAG,          TAGGERITEM,
			 Hv_NUMPOINTS,    4,
			 Hv_XMIN,         -375.0,
			 Hv_XMAX,         -90.0,
			 Hv_YMIN,         -62.0,
			 Hv_YMAX,         62.0,
			 Hv_NUMROWS,      1,
			 Hv_NUMCOLUMNS,   1,
			 Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			 Hv_DOUBLECLICK,  EditTagger,
			 Hv_DRAWCONTROL,  Hv_INBACKGROUND + Hv_DRAGGABLE + Hv_ZOOMABLE + Hv_ENLARGEABLE,
			 Hv_BALLOON,      balloon,
			 Hv_FILLCOLOR,    Hv_skyBlue + 4,
			 Hv_USERDRAW,     DrawTagger,
			 NULL);

  
  if (viewdata->displayTagger)
      Hv_SetItemToDraw(Item);
  else
      Hv_SetItemNotToDraw(Item);

  wrect = (Hv_WorldRectData *)(Item->data);
  wrect->frame = True;

  return Item;
}


/*--------- SetGeo -----------*/

static void SetGeo(Hv_Item Item) {

    float gap = 8.0;

    float xdel;
    Hv_FRect *wa = Item->worldarea;
    ViewData  viewdata;


    if (Item == NULL)
	return;

    viewdata = GetViewData(Item->view);

    viewdata->tg_axisx    = wa->ymin + 30.0;
    viewdata->tg_axiszmin = wa->xmin + 20.0;
    viewdata->tg_axiszmax = wa->xmax - 50.0;

    xdel = (wa->height - 3.0*gap - 7.0 - 30.0)/4.0;

    viewdata->tg_x[0] = viewdata->tg_axisx + 7.0 + gap; 
    viewdata->tg_x[1] = viewdata->tg_x[0] + xdel; 
    viewdata->tg_x[2] = viewdata->tg_x[1] + xdel; 
    viewdata->tg_x[3] = viewdata->tg_x[2] + gap; 
    viewdata->tg_x[4] = viewdata->tg_x[3] + xdel; 
    viewdata->tg_x[5] = viewdata->tg_x[4] + xdel; 


}


/*-------- DrawTagger ---------*/

static void DrawTagger(Hv_Item Item,
		       Hv_Region region) {

    short        L, T, R, B,  yc, xc;
    short        x1, x2, y1, y2, h4;
    int          i;
    float        z, del;
    char        *text1 = "k/Eo";
    char        *textval[] = {"0.2", "0.4", "0.6", "0.8", "1.0"};

    Hv_Rect       rect;
    

    Hv_View View = Item->view;
    ViewData viewdata;

    SetGeo(Item);

    viewdata = GetViewData(View);

/* draw the axis */

    Hv_WorldToLocal(View, viewdata->tg_axiszmin, viewdata->tg_axisx, &x1, &y1);
    Hv_WorldToLocal(View, viewdata->tg_axiszmax, viewdata->tg_axisx, &x2, &y2);

    Hv_DrawLine(x1, y1, x2, y2, Hv_white);
    Hv_DrawLine(x1, y1+1, x2, y2+1, Hv_black);

/* vertical ticks */

    del = (viewdata->tg_axiszmax - viewdata->tg_axiszmin)/8.0;

    Hv_WorldToLocal(View, viewdata->tg_axiszmax, viewdata->tg_axisx + 7.0, &x2, &y2);

    for (i = 0; i < 9; i++) {
	z = viewdata->tg_axiszmin + i*del;
	Hv_WorldToLocal(View, z, viewdata->tg_axisx, &x1, &y1);
	Hv_DrawLine(x1, y1, x1, y2, Hv_white);
	Hv_DrawLine(x1+1, y1, x1+1, y2, Hv_black);

	if ((i % 2) == 0) {
	    DrawString(x1, y1, textval[i/2], 0);
	}

    }

/* label the axis */

    DrawString(x2+6, y2, text1, 1);

/* rects for E & T */


    GetTaggerMainTRect(View, &rect);
    Hv_GetRectCenter(&rect, &xc, &yc);
    Hv_GetRectLTRB(&rect, &L, &T, &R, &B);
    h4 = (B-T)/4 + 1;

    Hv_Simple3DRect(&rect, False, Hv_gray12);


    DrawString(R+4, yc, "T", 1);
    DrawString(L,   yc-h4, "l", 2);
    DrawString(L,   yc+h4, "r", 2);

/* now the E, which is also split (odd, even) */

    GetTaggerMainERect(View, &rect);
    Hv_GetRectCenter(&rect, &xc, &yc);
    Hv_GetRectLTRB(&rect, &L, &T, &R, &B);
    h4 = (B-T)/4 + 1;

    Hv_Simple3DRect(&rect, False, Hv_gray12);

    Hv_DrawLine(L, yc-1, R, yc-1, Hv_white);
    Hv_DrawLine(L, yc,   R, yc,   Hv_black);

    DrawString(R+4, yc, "E", 1);
    DrawString(L ,  yc+h4, "e", 2);
    DrawString(L,   yc-h4, "o", 2);

    for (i = 1; i < Tagger.numTC; i++) {
	DrawTCRect(View, i);
    }


}

/*------- DrawTCRect -----------*/

static void DrawTCRect(Hv_View View,
		       int     id)

{
    int  i;
    Hv_Rect  rect;
    short    yc;

    if (id > Tagger.numTC)
	return;

    if (id <= 0) { /* draw 'em all */
	for (i = 1; i < Tagger.numTC; i++) {
	    DrawTCRect(View, i);
	}
	return;
    }


    GetTaggerTRect(View, id, &rect);
    Hv_FrameRect(&rect, Hv_gray9);

    yc = (rect.top + rect.bottom)/2;
    Hv_DrawLine(rect.left, yc-1, rect.right, yc-1, Hv_white);
    Hv_DrawLine(rect.left, yc,   rect.right, yc,   Hv_black);

}



/*------ GetTaggerMainERect ------*/


void GetTaggerMainERect(Hv_View View,
			Hv_Rect *rect) {

    short     x1, x2, y1, y2;
    ViewData  viewdata = GetViewData(View);

    Hv_WorldToLocal(View, viewdata->tg_axiszmin, viewdata->tg_x[3], &x1, &y1);
    Hv_WorldToLocal(View, viewdata->tg_axiszmax, viewdata->tg_x[5], &x2, &y2);

    Hv_SetRect(rect, x1, y2, x2-x1, y1-y2);
}

/*------ GetTaggerMainTRect ------*/


void GetTaggerMainTRect(Hv_View View,
			Hv_Rect *rect) {
    
    short     x1, x2, y1, y2;
    ViewData  viewdata = GetViewData(View);

    Hv_WorldToLocal(View, viewdata->tg_axiszmin, viewdata->tg_x[0], &x1, &y1);
    Hv_WorldToLocal(View, viewdata->tg_axiszmax, viewdata->tg_x[2], &x2, &y2);

    Hv_SetRect(rect, x1, y2, x2-x1, y1-y2);
}

/*------ GetTaggerERect ------*/


void GetTaggerERect(Hv_View View,
		    int     id,
		    Hv_Rect *rect) {


/* id is NOT a C INDEX !!! */

/* also note the numbering is from RIGHT TO LEFT */

    int       j;
    float     zd, zmin, zmax, xmin, xmax;
    short     x1, x2, y1, y2;
    ViewData  viewdata = GetViewData(View);
    

    if ((id < 1) || (id > Tagger.numEC)) {
	fprintf(stderr, "Bad index %d in GetTaggerERect\n", id);
	return;
    }


/*
 * KEEP in mind that idis NOT a C index, it starts at 1
 */

    if ((id % 2) == 0) { /* even*/
	xmin = viewdata->tg_x[3];
	xmax = viewdata->tg_x[4];
    }
    else {
	xmin = viewdata->tg_x[4];
	xmax = viewdata->tg_x[5];
    }


/* obtain horizontal by division */

    j = (id-1)/2;
    zd = (viewdata->tg_axiszmax - viewdata->tg_axiszmin)/(Tagger.numEC/2);
    zmax = viewdata->tg_axiszmax - j*zd;
    zmin = zmax - zd;

    Hv_WorldToLocal(View, zmin, xmin, &x1, &y1);
    Hv_WorldToLocal(View, zmax, xmax, &x2, &y2);

    Hv_SetRect(rect, x1, y2, x2-x1, y1-y2);
}

/*------ GetTaggerTRect ------*/


void  GetTaggerTRect(Hv_View View,
		     int     id,
		     Hv_Rect *rect) {
    

/* id is NOT a C INDEX !!! */

/* also note the numbering is from RIGHT TO LEFT */

    float     zd, zmin, zmax, xmin, xmax;
    short     x1, x2, y1, y2;
    ViewData  viewdata = GetViewData(View);
    float     emax;
    float     emin;


    if ((id < 1) || (id > Tagger.numTC)) {
	fprintf(stderr, "Bad index %d in GetTaggerTRect\n", id);
	return;
    }


    xmin = viewdata->tg_x[0];
    xmax = viewdata->tg_x[2];

/*
 * KEEP in mind that idis NOT a C index, it starts at 1
 */

/* cannot use simple division, higher energy (lower
   id) t counters corresponf to more e counters */

/* watch out for right to left */    

    emax = 1.0 - ((float)(effectiveElow(id)-1.0))/(Tagger.numEC-1);
    emin = 1.0 - ((float)(effectiveEhigh(id)-1.0))/(Tagger.numEC-1);

    zd = (viewdata->tg_axiszmax - viewdata->tg_axiszmin);

    zmin = viewdata->tg_axiszmin + emin*zd; 
    zmax = viewdata->tg_axiszmin + emax*zd; 


    Hv_WorldToLocal(View, zmin, xmin, &x1, &y1);
    Hv_WorldToLocal(View, zmax, xmax, &x2, &y2);

    Hv_SetRect(rect, x1, y2+1, x2-x1, y1-y2-2);
}


/*------- effectiveElow -------*/

static float effectiveElow(int tid) {

    int j = tid-1;

    if ((tid < 1) || (tid > Tagger.numTC))
	return 0.0;

    if (tid == 1)
	return (float)(Tagger.e_lo1[j]);
    else
	return 0.5*((float)(Tagger.e_lo1[j]) + (float)(Tagger.e_hi1[j-1]));

}

/*------- effectiveEhigh -------*/

static float effectiveEhigh(int tid) {

    int j = tid-1;

    if ((tid < 1) || (tid > Tagger.numTC))
	return 0.0;

    if (tid == Tagger.numTC)
	return (float)(Tagger.e_hi1[j]);
    else
	return 0.5*((float)(Tagger.e_hi1[j]) + (float)(Tagger.e_lo1[j+1]));

}

/*-------- DrawString ---------*/

static void DrawString(short x,
		       short y,
		       char *text,
		       int  opt) {

    Hv_String   *str;
    short        sh, sw, xt, yt;

    if (text == NULL)
	return;

    str = Hv_CreateString(text);
    str->strcol = Hv_white;
    str->font = Hv_timesBold11;

    Hv_CompoundStringDimension(str, &sw, &sh);

    if (opt == 0) {
	yt = y + 4;
	xt = x - sw/2;
    }
    else if (opt == 1) {
	yt = y - (sh/2) + 4;
	xt = x;
    }
    else if (opt == 2) {
	xt = x - sw - 2;
	yt = y - (sh/2) + 2;
    }


    Hv_DrawCompoundString(xt, yt, str);
    Hv_DestroyString(str);

}
		       

/*-------- EditTagger ----------*/

static void EditTagger(Hv_Event event) {
}


/*--------- GetTaggerHit ---------*/

void GetTaggerHit(short   ecount,
		  short   tcount,
		  short   *etdc,
		  short   *ttdcl,
		  short   *ttdcr) {

    TAGEDataPtr    ehits;
    TAGTDataPtr    thits;
    int            j;
    int            num;
    unsigned char  id, dummy;

    *etdc  = -1;
    *ttdcr = -1;
    *ttdcl = -1;

    if ((ecount > 0) && (ecount <= Tagger.numEC)) {
	ehits = bosTAGE;
	num = bosNumTAGE;

	if ((ehits != NULL) && (num > 0)) {
	    for (j = 0; j < num; j++) {

		if (ehits->id == ecount) { 
		    *etdc = ehits->tdc;
		    break;
		}
		ehits++;
	    }
	}
    }
    

    if ((tcount > 0) && (tcount <= Tagger.numTC)) {
	thits = bosTAGT;
	num = bosNumTAGT;

	if ((thits != NULL) && (num > 0)) {
	    for (j = 0; j < num; j++) {
		BreakShort(thits->id, &id, &dummy);
		if (id == tcount) { 
		    *ttdcl = thits->tdcleft;
		    *ttdcr = thits->tdcright;
		    break;
		}
		thits++;
	    }
	}
    }


}



/* -------- Hv_TaggerAfterDrag -----*/

static void Hv_TaggerAfterDrag(Hv_Item  Item,
			       short   dh,
			       short   dv)
{
    Hv_FRect *wa;
    if (Item == NULL)
	return;

    if ((dh == 0) && (dv == 0))
	return;

    Hv_LocalRectToWorldRect(Item->view, Item->area, Item->worldarea);
    wa = Item->worldarea;

    SetGeo(Item);
    Hv_SetViewDirty(Item->view);
    Hv_DrawViewHotRect(Item->view); 
}

/*------- Hv_CheckTaggerEnlarge ---------*/

static void Hv_CheckTaggerEnlarge(Hv_Item    Item,
				  Hv_Point   StartPP,
				  Boolean    ShowCorner,
				  Boolean   *enlarged) {

    Hv_CheckWorldRectEnlarge(Item, StartPP, ShowCorner, enlarged);

    if (*enlarged) {
	SetGeo(Item);
	Hv_SetViewDirty(Item->view);
	Hv_DrawViewHotRect(Item->view); 
    }

}



