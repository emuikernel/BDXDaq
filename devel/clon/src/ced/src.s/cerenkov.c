/*
----------------------------------------------------
-							
-  File:    cerenkov.c				
-							
-  Summary:						
-           Cerenkov Drawing
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

/* ------ local prototypes -------*/


static void   DrawCerenkovItem(Hv_Item  Item,
			       Hv_Region region);

static void GetCerenkovFPolygon(Hv_View  View,
				short   sect,
				short   ID,
				Hv_FPoint *fpts);

static void GetCerenkovInnerFrame(Hv_View      View,
				  short       sect,
				  short       id,
				  Hv_FPoint     *fpin);

static void  EditCerenkovItem(Hv_Event   hvevent);


static void FrameSectorCerenkovs(Hv_View  View,
				 short   sect,
				 Hv_Region region);

static void EditCerenkovColor(Widget  w);


static Hv_Widget hifcolorlab, lofcolorlab;
static Hv_Widget framecolorlabel, hitcolorlabel;
static short     hifillcolor, lofillcolor, framecolor, hitcolor;
static Boolean   FrameOnly = False;


/*-------- DrawCerenkovHits -----*/

void DrawCerenkovHits(Hv_View  View,
		      short   sect ,   /* C Index */
		      Hv_Region region)

{
  ViewData       viewdata;
  short          hifcolor, lofcolor;
  int            i;
  NormalDataPtr  hits;

  short          id;     /* real id [0..35] */
  short          cedid;  /* ced id [0..17] */ 

  Hv_Point       outpoly[4];
  Hv_Point       inpoly[4];

  if (bosIndexCC[sect] == 0)
    return;

  viewdata = GetViewData(View);

  if (!(viewdata->displayCC))
    return;

  hifcolor = viewdata->cerenkovhitcolor;
  lofcolor = viewdata->cerenkovhitcolor;

  switch (View->tag) {
  case SECTOR:

/* go through once for even and once for odd, The odd
   (non C index) cc's  use the outer frame */

    hits = bosCC[sect];

    for (i = 0; i < bosNumCC[sect]; i++) {
      id = hits->id;

/* look for bad IDs */

      if ((id < 1) || (id > 36))
	  fprintf(stderr, "Bad CC ID in sector %d found ID of %d\n", sect+1, id);
      else {
	  if ((id % 2) != 0) {
	      cedid = (id-1)/2;
	      
	      GetCerenkovPolygon(View, sect, cedid, outpoly);
	      Hv_FillPolygon(outpoly, 4, True, lofcolor,
			     viewdata->cerenkovframecolor);

/* now fix the inner */

	      GetCerenkovInnerPolygon(View, sect, cedid, inpoly);
	      Hv_FillPolygon(inpoly, 4, True, viewdata->cerenkovhifillcolor,
			     Hv_black);

	  }	
      }
      hits++;
    }  /* end of for loop */

    hits = bosCC[sect];

/* now do the even cc's, which use the inner frame */

    for (i = 0; i < bosNumCC[sect]; i++) {
      id = hits->id;

/* check for bad ID */

      if ((id < 1) || (id > 36))
	  fprintf(stderr, "Bad CC ID in sector %d found ID of %d\n", sect+1, id);
      else {
	  if ((id % 2) == 0) {
	      cedid = (id - 1)/2;
	      GetCerenkovInnerPolygon(View, sect, cedid, inpoly);
	      Hv_FillPolygon(inpoly, 4, True, hifcolor, Hv_black);
	  }
      }
      hits++;
    }   /* end of for loop */


/* reframe the cerenkovs */

    FrameSectorCerenkovs(View, sect, region);
    break;

  } /* end switch on tag */
}

/*------ MallocCerenkov--------*/

Hv_Item MallocCerenkov(Hv_View  View,
		       int    sect,
		       short   ID)

/************************************
  Mallocs a cerenkov item. We will
  only alloc 18 in ced, even though
  there are actually 36. We will use one
  definition to work for both low phi
  abd high phi. ID = 0..17 (C Index)
*************************************/


{
  Hv_Item      Item = NULL;
  Hv_FPoint    *fpts;

  fpts = (Hv_FPoint *)Hv_Malloc(4*sizeof(Hv_FPoint));
  GetCerenkovFPolygon(View, (short)sect, ID, fpts);

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,         Hv_WORLDPOLYGONITEM,
			 Hv_TAG,          CERENKOVITEM,
			 Hv_NUMPOINTS,    4,
			 Hv_DATA,         fpts,
			 Hv_NUMROWS,      1,
			 Hv_NUMCOLUMNS,   1,
			 Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			 Hv_DOUBLECLICK,  EditCerenkovItem,
			 Hv_DRAWCONTROL,  Hv_ZOOMABLE + Hv_INBACKGROUND,
			 Hv_AFTEROFFSET,  Hv_PolygonAfterOffset,
			 Hv_FIXREGION,    FixWPolygonBasedRegion,
			 Hv_USER1,        ID,
			 Hv_USER2,        (short)sect,
			 Hv_BALLOON,      (void *)"Cerenkov Counter",
			 NULL);
  
  Item->type = Hv_USERITEM;
  Item->standarddraw = DrawCerenkovItem;
  return  Item;
}

/*-------- SetCerenkovGeometrySector -----*/

void SetCerenkovGeometrySector(Hv_View View)

{
  int                  i, id;
  Hv_Item              temp;
  Hv_FPoint            *fpts;
  Hv_WorldPolygonData  *wpoly;
  ViewData             viewdata;
  short                sect;

  if (View->tag != SECTOR)
    return;

  viewdata = GetViewData(View);


  for (i = 0; i < NUM_CERENK2; i++)
    if ((temp = viewdata->CerenkovItems[i]) != NULL) {

/*  Because these are the cerenkovs for  two sectors,
    we need to mod the id count so that it repeats 0 to
    17 for both sectors */

      id = i % 18;

      sect = SetItemSector(temp);

      wpoly = (Hv_WorldPolygonData *)(temp->data);
      fpts = wpoly->fpts;

      fpts[0] = Cerenkovs[sect].cerenks[id].P[0];
      fpts[1] = Cerenkovs[sect].cerenks[id].P[1];
      fpts[2] = Cerenkovs[sect].cerenks[id].P[2];
      fpts[3] = Cerenkovs[sect].cerenks[id].P[3];
      temp->fixregion(temp);
    }

}

/*------- GetCerenkovPolygon -------*/

void GetCerenkovPolygon(Hv_View View,
			short  sect,     /*C Index*/
			short  ID,       /*C Index [0..17] */
			Hv_Point *poly)  /*poly previously alloced */

{
  Hv_FPoint    fpts[4];

  GetCerenkovFPolygon(View, sect, ID, fpts);
  Hv_WorldPolygonToLocalPolygon(View, 4, poly, fpts);
}


/*------- GetCerenkovInnerPolygon -------*/

void GetCerenkovInnerPolygon(Hv_View View,
			     short  sect,     /*C Index*/
			     short  ID,       /*C Index [0..17] */
			     Hv_Point *poly)  /*poly previously alloced */
{
  Hv_FPoint    fpin[4];

  GetCerenkovInnerFrame(View, sect, ID, fpin);
  Hv_WorldPolygonToLocalPolygon(View, 4, poly, fpin);
}

/*------ GetCerenkovHit -------*/

void GetCerenkovHit(short   sect,
		    short   id,
		    short   *tdcl,
		    short   *adcl)

/*
  sect   C Index [0..5] 
  id     NON C Index [1..36] 
*/

{
  NormalDataPtr    hits;
  int              i;

  *tdcl = -1;
  *adcl = -1;

  if (bosIndexCC[sect] == 0)
    return;

  hits = bosCC[sect];
  for (i = 0; i < bosNumCC[sect]; i++) {
    if (hits->id == id) {
      *tdcl = hits->tdc;
      *adcl = hits->adc;
      break;
    }

    hits++;
  }

}


/*---------- DrawCerenkovItem --------*/

static void   DrawCerenkovItem(Hv_Item  Item,
			       Hv_Region region)

{
  Hv_View               View = Item->view;
  ViewData              viewdata;
  Hv_WorldPolygonData  *wpoly;
  Hv_Point              inpoly[4];
  short                 sect;

  viewdata = GetViewData(View);
  
  if (!(viewdata->displayCC))
    return;

  sect = Item->user2;

  wpoly = (Hv_WorldPolygonData *)(Item->data);
  if (wpoly->poly == NULL)
    return;

/* get the "inner" poly */

  GetCerenkovInnerPolygon(View, sect, Item->user1, inpoly);

  if (FrameOnly) {
    Hv_FramePolygon(wpoly->poly, 4, viewdata->cerenkovframecolor);
    Hv_FramePolygon(inpoly, 4, Hv_black);
  }
  else {
    Hv_FillPolygon(wpoly->poly, 4, True, viewdata->cerenkovlofillcolor,
		   viewdata->cerenkovframecolor);
    Hv_FillPolygon(inpoly, 4, True, viewdata->cerenkovhifillcolor,
		   Hv_black);
  }

}

/*------ GetCerenkovFPolygon -------*/

static void GetCerenkovFPolygon(Hv_View  View,
				short   sect,
				short   ID,
				Hv_FPoint *fpts)

/***************************************
Obtains the raw floating point polygon
right from the database, corrected for
midplane rotation.

sect and ID are C Indices
****************************************/

{
  int    j;

  for (j = 0; j < 4; j++) {
    fpts[j] = Cerenkovs[sect].cerenks[ID].P[j];
    fpts[j].v = MidplaneXtoProjectedX(View, fpts[j].v);
  }

}


/*-------- GetCerenkovInnerFrame ----------*/

static void GetCerenkovInnerFrame(Hv_View    View,
				  short      sect,
				  short      id,
				  Hv_FPoint *fpin)

/***************************************
  This gets an inner frame of the cerenkovs
  that is used to represent the "other" side.
***************************************/

{
  Hv_FPoint  fpout[4];
  float      facth = 0.3;
  float      factv = 0.1;

  float      dh1, dh2, dv1, dv2;
  Hv_FPoint  temp[4]; 

  GetCerenkovFPolygon(View, sect, id, fpout);

  dh1 = fpout[1].h - fpout[0].h;
  dv1 = fpout[1].v - fpout[0].v;

  dh2 = fpout[3].h - fpout[2].h;
  dv2 = fpout[3].v - fpout[2].v;

  temp[0].h = fpout[0].h + facth*dh1;
  temp[0].v = fpout[0].v + facth*dv1;
  temp[1].h = fpout[1].h - facth*dh1;
  temp[1].v = fpout[1].v - facth*dv1;

  temp[2].h = fpout[2].h + facth*dh2;
  temp[2].v = fpout[2].v + facth*dv2;
  temp[3].h = fpout[3].h - facth*dh2;
  temp[3].v = fpout[3].v - facth*dv2;

/* now in the other direction */

  dh1 = temp[3].h - temp[0].h;
  dv1 = temp[3].v - temp[0].v;

  dh2 = temp[2].h - temp[1].h;
  dv2 = temp[2].v - temp[1].v;

  fpin[0].h = temp[0].h + factv*dh1;
  fpin[0].v = temp[0].v + factv*dv1;
  fpin[3].h = temp[3].h - factv*dh1;
  fpin[3].v = temp[3].v - factv*dv1;

  fpin[1].h = temp[1].h + factv*dh2;
  fpin[1].v = temp[1].v + factv*dv2;
  fpin[2].h = temp[2].h - factv*dh2;
  fpin[2].v = temp[2].v - factv*dv2;
}

/*-------- EditCerenkovItem -------*/

static void   EditCerenkovItem(Hv_Event hvevent)

/* item edit for CERENKITEMs  */

{
  Hv_View                 View = hvevent->view;
  static Widget           dialog = NULL;
  Widget                  dummy, rowcol, rc;
  ViewData                viewdata;
  int                     answer;

  viewdata = GetViewData(View);
  
  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, " Cerenkov Editor ", NULL);
    rowcol = Hv_DialogColumn(dialog, 6);

    rc = Hv_DialogTable(rowcol, 4, 6);
    lofcolorlab  = Hv_SimpleColorLabel(rc,    " low phi", (Hv_FunctionPtr)EditCerenkovColor);
    hifcolorlab  = Hv_SimpleColorLabel(rc,    "high phi", (Hv_FunctionPtr)EditCerenkovColor);
    framecolorlabel = Hv_SimpleColorLabel(rc, "   frame", (Hv_FunctionPtr)EditCerenkovColor);
    hitcolorlabel   = Hv_SimpleColorLabel(rc, "    hits", (Hv_FunctionPtr)EditCerenkovColor);

    dummy = Hv_StandardActionButtons(rowcol, 20, Hv_OKBUTTON + Hv_CANCELBUTTON + Hv_APPLYBUTTON);
  }

/* the dialog has been created */

  lofillcolor  = viewdata->cerenkovlofillcolor;
  hifillcolor  = viewdata->cerenkovhifillcolor;
  framecolor = viewdata->cerenkovframecolor;
  hitcolor   = viewdata->cerenkovhitcolor;
  Hv_ChangeLabelColors(lofcolorlab,  -1, lofillcolor);
  Hv_ChangeLabelColors(hifcolorlab,  -1, hifillcolor);
  Hv_ChangeLabelColors(framecolorlabel, -1, framecolor);
  Hv_ChangeLabelColors(hitcolorlabel,   -1, hitcolor);

  while ((answer = Hv_DoDialog(dialog, NULL)) > 0) {
    viewdata->cerenkovlofillcolor = lofillcolor;
    viewdata->cerenkovhifillcolor = hifillcolor;
    viewdata->cerenkovframecolor = framecolor;
    viewdata->cerenkovhitcolor = hitcolor;
    
    Hv_SetViewDirty(View);
    Hv_DrawViewHotRect(View); 

    if (answer != Hv_APPLY)
      break;
  }
}

/*------- FrameSectorCerenkovs ----- */

static void FrameSectorCerenkovs(Hv_View  View,
				 short   sect,   /* C Index */
				 Hv_Region region)

{
  ViewData      viewdata;
  Hv_Item       temp;
  int           i;
  
  viewdata =  GetViewData(View);

  FrameOnly = True;

  for (i = 0; i < NUM_CERENK2; i++)
    if ((temp = viewdata->CerenkovItems[i]) != NULL)
      if (temp->user2 == sect)
	DrawCerenkovItem(temp, region);
  
  FrameOnly = False;
}


/* ------ EditCerenkovColor --- */

static void EditCerenkovColor(Hv_Widget w)

{
  
  if (w == lofcolorlab)
    Hv_ModifyColor(w, &lofillcolor, " Low Phi", False);
  else if (w == hifcolorlab)
    Hv_ModifyColor(w, &hifillcolor, "High Phi", False);
  else if (w == framecolorlabel)
    Hv_ModifyColor(w, &framecolor, "Frame", False);
  else if (w == hitcolorlabel)
    Hv_ModifyColor(w, &hitcolor, "Hits", False);
  
}




