#include "Hv.h"
#include "Hv_xyplot.h"
#include "plot.h"
#define DEVEL

static void Histo1SliderCB(Hv_Event hvevent);

static void MenuCB(Hv_Item  Item,
		   short    current);

static void PageDraw(Hv_View   View,
		     Hv_Region region);

/*------ ModifyPlotView ----------*/

void ModifyPlotView(Hv_View View)

#define  EXTRAGAP  100
#define  WIDTH0    750
#define  WIDTH1    170
#define  WIDTH2    120
#define  WIDTH3    200

{
  Hv_Rect          newlocal;
  Hv_Item          box, invisbox, Item, sep1, sep2, sep3;
  short            left, top;
  Hv_PlotViewData  pvdata;

/* first resize */

#ifndef DEVEL
  return;
#endif

  Hv_CopyRect(&newlocal, View->local);
  newlocal.bottom += EXTRAGAP;
  Hv_FixRectWH(&newlocal);

  Hv_ChangeViewLocal(View, &newlocal);
  Hv_OffsetRect(View->hotrect, 0, EXTRAGAP);
  Hv_OffsetItem(View->hotrectborderitem, 0, EXTRAGAP, True);

/* change the userdraw */

  if (View->userdraw == NULL)
    View->userdraw = PageDraw;

/* add some new items */

  pvdata = Hv_GetPlotViewData(View);

  left = View->hotrect->left-4;
  top  = View->hotrect->top - EXTRAGAP - 5;

/* box will be the parent of all on view edit items */

  box = Hv_VaCreateItem(View,
			Hv_TYPE,               Hv_BOXITEM,
			Hv_LEFT,               left,
			Hv_TOP,                top,
			Hv_HEIGHT,             EXTRAGAP,
			Hv_WIDTH,              WIDTH0,
			Hv_BACKGROUND,         Hv_darkSeaGreen,
			Hv_RESIZEPOLICY,       Hv_DONOTHINGONRESIZE,
			NULL);

  pvdata->viewbox = box;

/* a simple label */

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,            Hv_TEXTITEM,
			 Hv_PARENT,          box,
			 Hv_FONT,            Hv_fixed2,
			 Hv_TEXT,            "Active plot title",
			 Hv_LEFT,            box->area->left+6,
			 Hv_TOP,             box->area->top+6,
			 Hv_WIDTH,           WIDTH1-10,
			 NULL);

/* a separator */

  sep1 = Hv_VaCreateItem(View,
			Hv_TYPE,            Hv_SEPARATORITEM,
			Hv_PARENT,          box,
			Hv_TOP,             Item->area->bottom+6,
			Hv_LEFT,            box->area->left+4,
			Hv_WIDTH,           WIDTH1-4,
			Hv_ORIENTATION,     Hv_HORIZONTAL, 
			NULL);


/* add the active plot name text item */

  invisbox = Hv_VaCreateItem(View,
			     Hv_TYPE,               Hv_BOXITEM,
			     Hv_PARENT,             box,
			     Hv_LEFT,               left,
			     Hv_TOP,                top,
			     Hv_HEIGHT,             EXTRAGAP,
			     Hv_WIDTH,              WIDTH1-8,
			     Hv_RESIZEPOLICY,       Hv_DONOTHINGONRESIZE,
			     Hv_DRAWCONTROL,        Hv_DONTDRAW,
			     Hv_BACKGROUND,         box->scheme->fillcolor,
			     NULL);

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,            Hv_TEXTITEM,
			 Hv_PARENT,          invisbox,
			 Hv_FONT,            Hv_fixed2,
			 Hv_TEXT,            "No active plot",
			 Hv_LEFT,            box->area->left+6,
			 Hv_TOP,             sep1->area->bottom+6,
			 Hv_WIDTH,           WIDTH1-10,
			 NULL);

  Hv_SetItemDrawControlBit(Item, Hv_CLIPPARENT);
  pvdata->activename = Item;

/* a separator */

  sep2 = Hv_VaCreateItem(View,
			 Hv_TYPE,            Hv_SEPARATORITEM,
			 Hv_PARENT,          box,
			 Hv_TOP,             box->area->top+4,
			 Hv_LEFT,            box->area->left+WIDTH1,
			 Hv_HEIGHT,          box->area->height-8,
			 Hv_ORIENTATION,     Hv_VERTICAL, 
			 NULL);


/* a simple label */

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,            Hv_TEXTITEM,
			 Hv_PARENT,          box,
			 Hv_FONT,            Hv_fixed2,
			 Hv_TEXT,            "Active curve",
			 Hv_LEFT,            sep2->area->right+6,
			 Hv_TOP,             box->area->top+6,
			 Hv_WIDTH,           WIDTH1-10,
			 NULL);

/* active curve selector */

  Item = Hv_VaCreateItem(View,
			  Hv_TYPE,               Hv_MENUBUTTONITEM,
			  Hv_PARENT,             box,
			  Hv_LEFT,               sep2->area->left + 8,
			  Hv_TOP,                Item->area->bottom + 2,
			  Hv_WIDTH,              WIDTH2-15,
			  Hv_HEIGHT,             24,
			  Hv_ARMCOLOR,           Hv_lightSeaGreen,
			  Hv_MENUBUTTONSTYLE,    Hv_TEXTBASED,
			  Hv_CURRENTCHOICE,      1,
			  Hv_MENUBUTTONENTRY,    Hv_MENUBUTTONCHOICE, Hv_fixed2, Hv_black, " no active",
			  Hv_MENUCHOICECB,       MenuCB,
			  NULL);

  pvdata->curvemenu = Item;

/* a separator */

  sep3 = Hv_VaCreateItem(View,
			 Hv_TYPE,            Hv_SEPARATORITEM,
			 Hv_PARENT,          box,
			 Hv_TOP,             box->area->top+4,
			 Hv_LEFT,            sep2->area->right+WIDTH2,
			 Hv_HEIGHT,          box->area->height-8,
			 Hv_ORIENTATION,     Hv_VERTICAL, 
			 NULL);


/* add a slider for histo bin */


  Item = Hv_VaCreateItem(View,
			 Hv_PARENT,              box,
			 Hv_LEFT,                sep3->area->left + 15,
			 Hv_TOP,                 box->area->top + 25,
			 Hv_CALLBACK,            Histo1SliderCB,
			 Hv_TYPE,                Hv_SLIDERITEM,
			 Hv_WIDTH,               WIDTH3-50,
			 Hv_TITLE,               "#bins",
			 Hv_MINVALUE,            1,
			 Hv_MAXVALUE,            250,
			 Hv_CURRENTVALUE,        100,
			 NULL);

  pvdata->bin1slider = Item;
  Hv_SetItemSensitivity(Item, False, False);
}

/*------- Histo1SliderCB --------*/

static void Histo1SliderCB(Hv_Event hvevent)

{
  Hv_Item          Item = hvevent->item;
  Hv_View          View = hvevent->view;
  Hv_PlotViewData  pvdata;
  Hv_PlotData      plotdata;
  Hv_Region        preg;
  int              i;
  Hv_CurveData     curve;

  pvdata = Hv_GetPlotViewData(View);

  fprintf(stderr, "in CB\n");

  plotdata = Hv_GetPlotData(pvdata->activeplot);

  
  for (i = 1, curve = plotdata->curves;
       ((curve != NULL) && (i < plotdata->activecurve));
       i++, curve = curve->next)
    ;

  if (curve == NULL) {
    fprintf(stderr, "Error in Histo1SliderCB\n");
    return;
  }

  curve->numbin1 = Hv_GetSliderCurrentValue(Item);
  curve->dirtybin = True;
  Hv_ReasonableHistoPlotLimits(plotdata);

  preg = Hv_TotalPlotRegion(pvdata->activeplot);

  Hv_OffScreenViewUpdate(View, preg);
  Hv_DestroyRegion(preg);
}

/*-------- MenuCB ------------*/

static void MenuCB(Hv_Item  Item,
		   short    current)

/***********************************
  callback routine for menu buttons
***********************************/

{
  Hv_View          View;
  Hv_PlotViewData  pvdata;
  Hv_PlotData      plotdata;

  View = Item->view;
  pvdata = Hv_GetPlotViewData(View);
  plotdata = Hv_GetPlotData(pvdata->activeplot);

  plotdata->activecurve = current;

  fprintf(stderr, "MB Callback: current choice: %d\n", current);

}


/*---------- PageDraw ----------------*/

static void PageDraw(Hv_View   View,
		     Hv_Region region)

/***********************************************
  routine that draws the page delimiters
*************************************************/

{
  Hv_FRect    *dw;

  if (Hv_inPostscriptMode)
    return;

  dw = View->defaultworld;
  fprintf(stderr, "[%f, %f] [%f, %f]   [%f, %f]\n", 
	  dw->xmin, dw->xmax, dw->ymin, dw->ymax, dw->width, dw->height);
  
  if (Hv_psOrientation == Hv_PSLANDSCAPE) {
    fprintf(stderr, "in LS mode\n");

  }
  else {
    fprintf(stderr, "in PO mode\n");
  }

}

#undef EXTRAGAP
#undef WIDTH0
#undef WIDTH1
#undef WIDTH2
#undef WIDTH3
