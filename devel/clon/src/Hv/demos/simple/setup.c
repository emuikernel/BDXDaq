#include "Hv.h"
#include "simple.h"

#define  simpleANGLETAG    1001

/* local prototypes */

static void             simpleDraw1();
static void             simpleDraw2();
static void             simpleDraw3();
static void             simpleEdit();
static void             simpleCB();
static void             simpleChoiceCB();
static void             simpleWheelCB();
static void             simpleAddOptionButtons();
static Hv_Item          simpleStandardOptionButton();
static void             simpleOptionButtonCB();
static void             simpleSliderCB();
static void             simpleControlItems();
static void             simpleHotRectItems();
static void             simpleFDUpdate();
static void             simpleLEDUpdate();
static void             ConcatTest();
static void             IconDraw(Hv_Item Item,
				 short   choice,
				 Hv_Rect *rect);

static void             TextEntryCB(Hv_Widget  w,
				    Hv_Pointer data);

static void             MenuCB(Hv_Item Item,
			       short   current);

static void             MenuIconCB(Hv_Event hvevent);

static Hv_Item          TextEntry1, TextEntry2;
static Hv_Item          menu1, menu2;
 
/* --------- SetupSimpleView -------- */

void SetupSimpleView(Hv_View View)

/* setup the View */

{
  Hv_Item   Item, Feedback;
  Hv_Rect   *hotrect = View->hotrect;

/* create a camera button for printing */


  Hv_AddDrawingTools(View,
		     hotrect->left - 45,
		     hotrect->top,
		     Hv_VERTICAL,
		     Hv_DONOTHINGONRESIZE,
		     Hv_cadetBlue,
		     True,
		     &Item,
		     &Item);


/* create an example of a round button under the tools */

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,               Hv_BUTTONITEM,
			 Hv_DRAWCONTROL,        Hv_ROUNDED,
			 Hv_RADIUS,             16,
			 Hv_TEXT,              "Hi",
			 Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			 Hv_PLACEMENTGAP,       10,
			 Hv_RESIZEPOLICY,       Hv_DONOTHINGONRESIZE,
			 NULL);


/* create a feedback item */

  Feedback = Hv_VaCreateItem(View,
			     Hv_TYPE,              Hv_FEEDBACKITEM,
			     Hv_LEFT,              hotrect->right + 15,
			     Hv_TOP,               hotrect->top-15,
			     Hv_FEEDBACKDATACOLS,  24,
			     Hv_FEEDBACKENTRY,     LOCALX, Hv_fixed2, Hv_black, "Local X: ",
			     Hv_FEEDBACKENTRY,     LOCALY, Hv_fixed2, Hv_black, "Local Y: ",
			     Hv_FEEDBACKENTRY,     Hv_SEPARATORTAG,
			     Hv_FEEDBACKENTRY,     WORLDX, Hv_fixed2, Hv_blue, "World X: ",
			     Hv_FEEDBACKENTRY,     WORLDY, Hv_fixed2, Hv_blue, "World Y: ",
			     Hv_FEEDBACKMARGIN,    15,
			     NULL);

  simpleControlItems(View, Feedback);
  simpleHotRectItems(View);
}



/*------ simpleDraw1 ------*/

static void simpleDraw1(Item, region)

Hv_Item     Item;
Hv_Region   region;

{
  int     nrb;
  int     nrb2;
  short   color;
  Hv_Rect area;

  Hv_CopyRect(&area, Hv_GetItemArea(Item));

  if (Item->drawinterval != NULL) {
    nrb   = Hv_rainbowLast - Hv_rainbowFirst + 1;
    nrb2  = 2*nrb;
    color = Hv_rainbowFirst + (short)((Item->drawinterval->timedredrawcount % nrb2));
    if (color > Hv_rainbowLast)
      color = Hv_rainbowLast - (color - Hv_rainbowLast);
  }
  else
    color = Hv_yellow;

  Hv_FrameRect(&area, Hv_black);
  Hv_LeftTopResizeRect(&area, 1, 1);
  Hv_FillRect(&area,  color);
}


/*------ simpleDraw2 ------*/

static void simpleDraw2(Item, region)

Hv_Item     Item;
Hv_Region   region;

{
  short      xc, yc, rad;
  short      rrsize = 8, rrx, rry, rrs2;
  Hv_Rect    area;
  Hv_Item    anglestr;
  char       text[20];

  Hv_CopyRect(&area, Hv_GetItemArea(Item));
  rad = area.width/2 - 1;
  Hv_GetItemCenter(Item, &xc, &yc);

/* now the "rotate rect" */

  rrs2 = rrsize/2;
  rad -= rrs2;
  rrx = xc + rad*sin(Item->azimuth);
  rry = yc - rad*cos(Item->azimuth);

/* we can be polite and check Hv_extraFastPlease, which will
   be set to true when we are rotating */

  Hv_SetLineWidth(2);


  if (Hv_extraFastPlease)
    Hv_FrameCircle(xc, yc, rad, Hv_white);
  else 
    Hv_FillCircle(xc, yc, rad, True, Hv_honeydew, Hv_coral);
  
  Hv_DrawLine(xc, yc, rrx, rry, Hv_red);
  Hv_SetLineWidth(0);
  
  Hv_SetRect(Item->rotaterect, rrx-rrs2, rry-rrs2, rrsize, rrsize);

  if (Hv_extraFastPlease)
    Hv_FillRect(Item->rotaterect, Hv_white);
  else
    Hv_FillRect(Item->rotaterect, Hv_black);

  anglestr = Hv_GetItemFromTag(Item->children, simpleANGLETAG);
  sprintf(text, "%-3.0f deg", Hv_RADTODEG*Item->azimuth);
  Hv_SetItemText(anglestr, text);
}


/*------ simpleDraw3 ------*/

static void simpleDraw3(Item, region)

Hv_Item     Item;
Hv_Region   region;

{
  Hv_Rect area;
  short   color = Hv_GetItemColor(Item);

  Hv_CopyRect(&area, Hv_GetItemArea(Item));

  Hv_FrameRect(&area, Hv_black);
  Hv_LeftTopResizeRect(&area, 1, 1);
  Hv_FillRect(&area,  color);
}

/*------- simpleEdit  -------*/

static void simpleEdit(hvevent)

Hv_Event       hvevent;  /*event that initiated the callback*/

{
  Hv_Item                 Item = hvevent->item;
  Hv_View                 View = hvevent->view;
  short                   color = Hv_GetItemColor(Item);

  Hv_ModifyColor(NULL, &color, "Change fill color", False);
  Hv_SetItemColor(Item, color);
  Hv_UpdateItem(Item);
}


/* ------- simpleChoiceCB -------*/

static void simpleChoiceCB(hvevent)

Hv_Event    hvevent;

{
  Hv_Item    Item = hvevent->item;
  short      nowon;
  char       report[20];

  nowon = Hv_HandleChoice(Item);

  if (nowon == 0)  /* signals same button */
    Hv_Information("That button is already on!");
  else {
    sprintf(report, "Hit button %2d", nowon);
    Hv_Information(report);
  }

}


/* ------- simpleCB -------*/

static void simpleCB(hvevent)

Hv_Event    hvevent;

{
  Hv_Item   Item = hvevent->item;

  Hv_SysBeep();
  Hv_ToggleButton(Item);
  Hv_Information("Why not?");
  Hv_ToggleButton(Item);
}

/* ------- simpleWheelCB -------*/

static void simpleWheelCB(hvevent)

Hv_Event    hvevent;

{
  Hv_Item   Item = hvevent->item;

  fprintf(stderr, "in simple wheel cb\n");
}


/* ------- simpleAddOptionButtons -----*/

static void simpleAddOptionButtons(View, parent)

Hv_View     View;
Hv_Item     parent;

{
  Hv_Item       Item;
  static Boolean       dummy[7];

/* note: place your own boolean in place of "duumy"; 
   its value will automatically be toggled. */

  dummy[0] = True;
  dummy[1] = False;
  dummy[2] = True;
  dummy[3] = False;
  dummy[4] = True;
  dummy[5] = True;
  dummy[6] = True;


  Item = Hv_VaCreateItem(View,
			 Hv_PARENT,             parent,
			 Hv_TYPE,               Hv_OPTIONBUTTONITEM,
                         Hv_CONTROLVAR,         (void *)(dummy),
			 Hv_LEFT,               parent->area->left + 10,
			 Hv_TOP,                parent->area->top + 5,
			 Hv_TEXT,               "Option 1",
			 Hv_FONT,               Hv_fixed2,
	                 Hv_ARMCOLOR,           Hv_red,
                         Hv_SINGLECLICK,        simpleOptionButtonCB,
			 NULL);

  Item = simpleStandardOptionButton(View, parent, dummy+1, "Option 2");
  Item = simpleStandardOptionButton(View, parent, dummy+2, "Option 3");
  Item = simpleStandardOptionButton(View, parent, dummy+3, "Option 4");
  Item = simpleStandardOptionButton(View, parent, dummy+4, "Option 5");
  Item = simpleStandardOptionButton(View, parent, dummy+5, "Option 6");
  Item = simpleStandardOptionButton(View, parent, dummy+6, "Option 7");

  parent->area->bottom = Item->area->bottom + 4;
  Hv_FixRectWH(parent->area);
}


/* ------ simpleStandardOptionButton ------*/

static Hv_Item    simpleStandardOptionButton(View, parent, control, label)

Hv_View   View;
Hv_Item   parent;
Boolean   *control;
char      *label;

{
  Hv_Item   Item;

  Item = Hv_VaCreateItem(View,
			 Hv_PARENT,             parent,
			 Hv_TYPE,               Hv_OPTIONBUTTONITEM,
                         Hv_CONTROLVAR,         (void *)control,
			 Hv_TEXT,               label,
			 Hv_FONT,               Hv_fixed2,
	                 Hv_ARMCOLOR,           Hv_red,
			 Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
                         Hv_SINGLECLICK,        simpleOptionButtonCB,
			 Hv_PLACEMENTGAP,       0,
			 NULL);

  return  Item;
}


/* ------- simpleOptionButtonCB ------*/

static void simpleOptionButtonCB(hvevent)

Hv_Event     hvevent;

{
   Hv_OptionButtonCallback(hvevent);

/* user code here ! */
}


/* ------- simpleControlItems ------*/

static void   simpleControlItems(View, Feedback)

Hv_View     View;
Hv_Item     Feedback;

{

  Hv_Item    Box1, Box2, Box3, Box3b, Box4, Box5, Box6, Box7, Box8, Box9;
  Hv_Item    LED, Wheel, Slider, Choice, Rainbow, FractionDone;


/* a box for an LED Item */

  Box1 = Hv_VaCreateItem(View,
			 Hv_TYPE,               Hv_BOXITEM,
			 Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			 Hv_PLACEMENTGAP,       0,
			 Hv_WIDTH,              Feedback->area->width,
			 Hv_HEIGHT,             40,
			 NULL);

/* an LED Bar */


  LED = Hv_VaCreateItem(View,
			Hv_TYPE,             Hv_LEDBARITEM,
			Hv_FONT,             Hv_helveticaBold11,
			Hv_PARENT,           Box1,
			Hv_CHILDPLACEMENT,   Hv_POSITIONCENTER,
			Hv_OPTION,           "1",
			Hv_OPTION,           "2",
			Hv_OPTION,           "3",
			Hv_OPTION,           "4",
			Hv_OPTION,           "5",
			Hv_OPTION,           "6",
			Hv_OPTION,           "7",
			Hv_OPTION,           "8",
			Hv_OPTION,           "9",
			Hv_USERDRAW,          simpleLEDUpdate,
			Hv_REDRAWTIME,        1000,
			NULL);
  

/* add a set of do nothing choice buttons */

  Choice = Hv_VaCreateItem(View,
			  Hv_TYPE,                Hv_CHOICESETITEM,
			  Hv_RELATIVEPLACEMENT,   Hv_POSITIONBELOW,
			  Hv_PLACEMENTITEM,       Box1,
			  Hv_PLACEMENTGAP,        1,
			  Hv_NOWON,               2,
			  Hv_FILLCOLOR,           Hv_powderBlue,
			  Hv_ARMCOLOR,            Hv_blue,
			  Hv_COLOR,               Hv_gray10,
			  Hv_FONT,                Hv_fixed2,
			  Hv_OPTION,             "Choice 1",
			  Hv_OPTION,             "Choice 2",
			  Hv_OPTION,             "Choice 3",
			  Hv_OPTION,             "Choice 4",
			  Hv_OPTION,             "Choice 5",
			  Hv_OPTION,             "Choice 6",
			  Hv_OPTION,             "Choice 7",
			  Hv_SINGLECLICK,         simpleChoiceCB,
			  NULL);

/* a box container for some child items */

  Box1 = Hv_VaCreateItem(View,
			Hv_TYPE,               Hv_BOXITEM,
			Hv_RELATIVEPLACEMENT,  Hv_POSITIONRIGHT,
			Hv_PLACEMENTGAP,       0,
			Hv_WIDTH,              70,
			Hv_HEIGHT,             Choice->area->height,
			NULL);

/* a vertical thumb wheel */

  Wheel = Hv_VaCreateItem(View,
			  Hv_TYPE,                  Hv_WHEELITEM,
			  Hv_PARENT,                Box1,
			  Hv_ORIENTATION,           Hv_VERTICAL,
			  Hv_BALLOON,               (char *)"Spin the wheel or use the arrows.",
			  Hv_DRAWCONTROL,           Hv_REDRAWHOTRECTWHENCHANGED,
			  Hv_CHILDPLACEMENT,        Hv_POSITIONCENTER,
			  Hv_TITLE,                 "spin me",
			  Hv_GRANDCHILDPLACEMENT,   Hv_POSITIONBELOW,
			  Hv_CALLBACK,              simpleWheelCB,
			  NULL);

/* a container for some option buttons */


  Box2 = Hv_VaCreateItem(View,
			 Hv_TYPE,               Hv_BOXITEM,
			 Hv_PLACEMENTITEM,      Choice,
			 Hv_BALLOON,            "Select an option.",
			 Hv_WIDTH,              Choice->area->width,
			 Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			 Hv_PLACEMENTGAP,       0,
			 NULL);

/* add some option buttons */

  simpleAddOptionButtons(View, Box2);

/* box for a vertical slider */

  
  Box3 = Hv_VaCreateItem(View,
			 Hv_TYPE,               Hv_BOXITEM,
			 Hv_PLACEMENTITEM,      Box2,
			 Hv_WIDTH,              Box1->area->width,
			 Hv_HEIGHT,             Box2->area->height,
			 Hv_RELATIVEPLACEMENT,  Hv_POSITIONRIGHT,
			 Hv_PLACEMENTGAP,       0,
			 NULL);



  Slider = Hv_VaCreateItem(View,
			   Hv_PARENT,              Box3,
			   Hv_CHILDPLACEMENT,      Hv_POSITIONCENTER,
			   Hv_DRAWCONTROL,         Hv_REDRAWHOTRECTWHENCHANGED,
			   Hv_CALLBACK,            simpleSliderCB,
			   Hv_TYPE,                Hv_SLIDERITEM,
			   Hv_ORIENTATION,         Hv_VERTICAL,
			   Hv_HEIGHT,              100,
			   Hv_BALLOON,             (char *)"An example of a vertical slider.",
			   Hv_TITLE,               "slide me",
			   Hv_MINVALUE,            -1000,
			   Hv_MAXVALUE,            1000,
			   Hv_CURRENTVALUE,        250,
			   NULL);

/* a box for a vertical rainbow */


  Box3b = Hv_VaCreateItem(View,
			Hv_TYPE,               Hv_BOXITEM,
			Hv_PLACEMENTITEM,      Box1,
			Hv_RELATIVEPLACEMENT,  Hv_POSITIONRIGHT,
			Hv_PLACEMENTGAP,       0,
			Hv_WIDTH,              Feedback->area->right - Box1->area->right,
			Hv_HEIGHT,             Box3->area->bottom - Box1->area->top,
			NULL);


  Rainbow = Hv_VaCreateItem(View,
			    Hv_PARENT,            Box3b,
			    Hv_TYPE,              Hv_RAINBOWITEM,
			    Hv_ORIENTATION,       Hv_VERTICAL, 
			    Hv_EDITABLE,          True,
			    Hv_DRAWCONTROL,       Hv_REDRAWHOTRECTWHENCHANGED,
			    Hv_WIDTH,             15,
			    Hv_NUMLABELS,         4,
			    Hv_PRECISION,         0,
			    Hv_MINVALUE,          0,
			    Hv_MAXVALUE,          80,
			    Hv_COLORWIDTH,        4,
			    Hv_TITLE,             "Scale",
			    Hv_MINCOLOR,          Hv_blue+3,
			    Hv_MAXCOLOR,          Hv_red,
			    Hv_CHILDPLACEMENT,    Hv_POSITIONCENTER,
			    Hv_BALLOON,           (char *)"A vertical rainbow scale.",
			    NULL);



/* another box for a horizontal wheel*/

  Box4 = Hv_VaCreateItem(View,
			Hv_TYPE,               Hv_BOXITEM,
			Hv_PLACEMENTITEM,      Box2,
			Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			Hv_PLACEMENTGAP,       0,
			Hv_WIDTH,              Box3b->area->right - Choice->area->left,
			Hv_HEIGHT,             64,
			NULL);

/* a horizontal thumb wheel */

  Wheel = Hv_VaCreateItem(View,
			  Hv_TYPE,                Hv_WHEELITEM,
			  Hv_DRAWCONTROL,         Hv_REDRAWHOTRECTWHENCHANGED,
			  Hv_PARENT,              Box4,
			  Hv_CHILDPLACEMENT,      Hv_POSITIONCENTER,
			  Hv_TITLE,                 "spin me",
			  Hv_GRANDCHILDPLACEMENT,      Hv_POSITIONRIGHT,
			  Hv_BALLOON,             (char *)"Spin the wheel or use the arrows.",
			  Hv_CALLBACK,            simpleWheelCB,
			  Hv_MINVALUE,            -1000,
			  Hv_MAXVALUE,            1000,
			  Hv_CURRENTVALUE,        0,
			  Hv_WIDTH,               120,
			  NULL);

/* a box for a horizontal slider */


  Box5 = Hv_VaCreateItem(View,
			Hv_TYPE,               Hv_BOXITEM,
			Hv_PLACEMENTITEM,      Box4,
			Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			Hv_PLACEMENTGAP,       0,
			Hv_WIDTH,              Box4->area->width,
			Hv_HEIGHT,             80,
			NULL);


  Slider = Hv_VaCreateItem(View,
			   Hv_PARENT,              Box5,
			   Hv_BALLOON,             (char *)"An example of a horizontal slider.",
			   Hv_DRAWCONTROL,         Hv_REDRAWHOTRECTWHENCHANGED,
			   Hv_CHILDPLACEMENT,      Hv_POSITIONCENTER,
			   Hv_CALLBACK,            simpleSliderCB,
			   Hv_TYPE,                Hv_SLIDERITEM,
			   Hv_WIDTH,               180,
			   Hv_TITLE,               "slide me",
			   Hv_MINVALUE,            -400,
			   Hv_MAXVALUE,            100,
			   Hv_CURRENTVALUE,        -150,
			   Hv_GRANDCHILDPLACEMENT,      Hv_POSITIONBELOW,
			   NULL);


/* a box for a horizontal rainbow scale */


  Box6 = Hv_VaCreateItem(View,
			Hv_TYPE,               Hv_BOXITEM,
			Hv_PLACEMENTITEM,      Box5,
			Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			Hv_PLACEMENTGAP,       0,
			Hv_WIDTH,              Box5->area->width,
			Hv_HEIGHT,             70,
			NULL);

  Rainbow = Hv_VaCreateItem(View,
			    Hv_PARENT,            Box6,
			    Hv_TYPE,              Hv_RAINBOWITEM,
			    Hv_EDITABLE,          True,
			    Hv_DRAWCONTROL,       Hv_REDRAWHOTRECTWHENCHANGED,
			    Hv_HEIGHT,            15,
			    Hv_NUMLABELS,         3,
			    Hv_PRECISION,         2,
			    Hv_MINVALUE,          0,
			    Hv_MAXVALUE,          20000,
			    Hv_COLORWIDTH,        3,
			    Hv_TITLE,             "Rainbow Scale",
			    Hv_CHILDPLACEMENT,    Hv_POSITIONCENTER,
			    Hv_SCALEFACTOR,       1.0e-4,
			    Hv_BALLOON,           (char *)"A horizontal rainbow scale.",
			    NULL);

/* Box for FractionDoneItem */


  Box7 = Hv_VaCreateItem(View,
			Hv_TYPE,               Hv_BOXITEM,
			Hv_PLACEMENTITEM,      Box6,
			Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			Hv_PLACEMENTGAP,       0,
			Hv_WIDTH,              Box5->area->width,
			Hv_HEIGHT,             40,
			NULL);

  FractionDone = Hv_VaCreateItem(View,
				 Hv_PARENT,         Box7,
				 Hv_TYPE,           Hv_FRACTIONDONEITEM,
				 Hv_WIDTH,          150,
				 Hv_CHILDPLACEMENT, Hv_POSITIONCENTER,
				 Hv_BALLOON,        (char *)"Fraction Done item.",
				 Hv_USERDRAW,       simpleFDUpdate,
				 Hv_REDRAWTIME,     1000,
				 NULL);

/* Box for TextEntryItems */

  Box8 = Hv_VaCreateItem(View,
			Hv_TYPE,               Hv_BOXITEM,
			Hv_PLACEMENTITEM,      Box7,
			Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			Hv_PLACEMENTGAP,       0,
			Hv_WIDTH,              Box5->area->width,
			Hv_HEIGHT,             50,
			NULL);


  TextEntry1 = Hv_VaCreateItem(View,
			       Hv_DRAWCONTROL,    Hv_DONTAUTOSIZE+Hv_FRAMEAREA,
			       Hv_PARENT,         Box8,
			       Hv_TYPE,           Hv_TEXTENTRYITEM,
			       Hv_WIDTH,          100,
			       Hv_HEIGHT,         35,
			       Hv_TOP,            Box8->area->top+6,
			       Hv_LEFT,           Box8->area->left+10,
			       Hv_BALLOON,        "In place text editor.",
			       Hv_TEXT,           "Text Entry",
			       Hv_FILLCOLOR,      Hv_canvasColor,
			       Hv_FONT,           Hv_fixed2,
			       Hv_VALUECHANGED,   TextEntryCB,
			       NULL);

  TextEntry2 = Hv_VaCreateItem(View,
			       Hv_DRAWCONTROL,    Hv_DONTAUTOSIZE+Hv_FRAMEAREA,
			       Hv_PARENT,         Box8,
			       Hv_TYPE,           Hv_TEXTENTRYITEM,
			       Hv_WIDTH,          100,
			       Hv_HEIGHT,         35,
			       Hv_TOP,            Box8->area->top+6,
			       Hv_LEFT,           TextEntry1->area->right+10,
			       Hv_BALLOON,        "In place text editor.",
			       Hv_TEXT,           "Me Too",
			       Hv_FILLCOLOR,      Hv_skyBlue,
			       Hv_TEXTCOLOR,      Hv_yellow,
			       Hv_FONT,           Hv_fixed2,
			       NULL);


/* Box for MenuButtonItems */

  Box9 = Hv_VaCreateItem(View,
			 Hv_TYPE,               Hv_BOXITEM,
			 Hv_PLACEMENTITEM,      Box8,
			 Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			 Hv_PLACEMENTGAP,       0,
			 Hv_WIDTH,              Box8->area->width,
			 Hv_HEIGHT,             46,
			 Hv_BACKGROUND,         Hv_royalBlue,
			 NULL);

/* the two menus, one a text based and one an iconbeased */

  menu1 = Hv_VaCreateItem(View,
			  Hv_TYPE,               Hv_MENUBUTTONITEM,
			  Hv_PARENT,             Box9,
			  Hv_LEFT,               Box9->area->left + 6,
			  Hv_TOP,                Box9->area->top + 6,
			  Hv_FILLCOLOR,          Hv_lightSeaGreen,
			  Hv_MENUBUTTONSTYLE,    Hv_ICONBASED,
			  Hv_NUMROWS,            2,
			  Hv_NUMCOLUMNS,         8,
			  Hv_MBCOLOR,            Hv_skyBlue,
			  Hv_ICONDRAW,           IconDraw,
			  Hv_CALLBACK,           MenuIconCB,
			  Hv_MENUCHOICECB,       MenuCB,
			  NULL);

  menu2 = Hv_VaCreateItem(View,
			  Hv_TYPE,               Hv_MENUBUTTONITEM,
			  Hv_PARENT,             Box9,
			  Hv_LEFT,               Box9->area->left + 60,
			  Hv_TOP,                Box9->area->top + 9,
			  Hv_WIDTH,              160,
			  Hv_HEIGHT,             28,
			  Hv_ARMCOLOR,           Hv_powderBlue,
			  Hv_MENUBUTTONSTYLE,    Hv_TEXTBASED,
			  Hv_CURRENTCHOICE,      2,
			  Hv_MENUBUTTONENTRY,    Hv_MENUBUTTONCHOICE, Hv_fixed2, Hv_black, "Choice 1",
			  Hv_MENUBUTTONENTRY,    Hv_MENUBUTTONCHOICE, Hv_fixed2, Hv_red, "Choice 2",
			  Hv_MENUBUTTONENTRY,    Hv_SEPARATORTAG,
			  Hv_MENUBUTTONENTRY,    Hv_MENUBUTTONCHOICE, Hv_helveticaBold17, Hv_blue, "Choice 3",
			  Hv_MENUBUTTONENTRY,    Hv_MENUBUTTONCHOICE, Hv_timesBold17, Hv_brown, "Choice 4",
			  Hv_MENUCHOICECB,       MenuCB,
			  NULL);

  Hv_AddFunctionKeyCallback(7, ConcatTest);

}

/*--------- ConcatTest -------*/

static void ConcatTest()

{
  char  text[10];
  static int cnt = 0;

  cnt = (cnt + 1) % 10;
  sprintf(text, "abc%1d", cnt);

  Hv_ConcatTextToTextItem(TextEntry1, text);
}


/*-------- simpleLEDUpdate --------*/

static void simpleLEDUpdate(Item, region)

Hv_Item     Item;

{
  Hv_LEDData    *led;
  int           testint;
  int           numled;
  int           i;

  led = (Hv_LEDData *)(Item->data);
  numled = led->numled;

  testint = (Item->drawinterval->timedredrawcount) % (numled+1);

  for (i = 0; i < numled; i++)
    led->states[i] = (short)(testint > i);
}


/*-------- simpleFDUpdate --------*/

static void simpleFDUpdate(Item, region)

Hv_Item     Item;
Hv_Region    region;

{
  short                pcdone;
  Hv_FractionDoneData  *fdone;

  fdone = (Hv_FractionDoneData *)Item->data;
  pcdone = (Item->drawinterval->timedredrawcount) % 111;
  fdone->fraction = (((float)pcdone)/100.0);

}


/* ------- simpleHotRectItems ------*/

static void   simpleHotRectItems(View)

Hv_View     View;

{
  Hv_Item    Item, Child;
  Hv_Rect   *hotrect = View->hotrect;

/***********************************************

Create a bunch of rather silly user items (with text children) just
to illustrate the process. 

======= NOTE ===========

Making use of default domains:

a) Hv_INSIDEHOTRECT for UserItems
b) Inherited fro parent for child items
c) Hv_OUTSIDEHOTRECT for all others

************************************************/

/* a simple user item to illustrate balloons, rotation and children */

  Item = Hv_VaCreateItem(View,
			 Hv_DRAWCONTROL, Hv_ROTATABLE + Hv_FANCYDRAG,
			 Hv_USERDRAW,    simpleDraw2,
			 Hv_BALLOON,     (char *)"Use the small rectangle for rotating. Hold the shift button for updates.",
			 Hv_LEFT,        hotrect->left + 80,
			 Hv_TOP,         hotrect->top  + 300,
			 Hv_WIDTH,       80,
			 Hv_HEIGHT,      80,
			 NULL);

/* add some text children */

  Child = Hv_VaCreateItem(View,
			  Hv_TYPE,            Hv_TEXTITEM,
			  Hv_PARENT,          Item,
			  Hv_DRAWCONTROL,     Hv_FRAMEAREA + Hv_FANCYDRAG,
			  Hv_FONT,            Hv_fixed2,
			  Hv_TEXT,            (char *)"Drag/Rotate Me",
			  Hv_TEXTCOLOR,       Hv_red,
			  Hv_FILLCOLOR,       Hv_peachPuff,
			  Hv_CHILDPLACEMENT,  Hv_POSITIONBELOW,
			  NULL);

/* a second child for the same parent item */

  Child = Hv_VaCreateItem(View,
			  Hv_TYPE,            Hv_TEXTITEM,
			  Hv_DRAWCONTROL,     Hv_FRAMEAREA + Hv_FANCYDRAG,
			  Hv_PARENT,          Item,
			  Hv_FONT,            Hv_fixed2,
			  Hv_TEXT,            (char *)"    deg",
			  Hv_TAG,             simpleANGLETAG,
			  Hv_CHILDPLACEMENT,  Hv_POSITIONABOVE,
			  Hv_PLACEMENTGAP,    6,
			  Hv_FILLCOLOR,       Hv_honeydew,
			  NULL);

/* another user item -- this one can be doubleclicked to edit! */


  Item = Hv_VaCreateItem(View,
			 Hv_USERDRAW,    simpleDraw3,
			 Hv_DOUBLECLICK, simpleEdit,
			 Hv_LEFT,        hotrect->left + 250,
			 Hv_TOP,         hotrect->top  + 400,
			 Hv_WIDTH,       70,
			 Hv_HEIGHT,      30,
			 Hv_DRAWCONTROL, Hv_FANCYDRAG,
			 Hv_COLOR,       Hv_powderBlue,
			 NULL);


  Child = Hv_VaCreateItem(View,
			  Hv_TYPE,            Hv_TEXTITEM,
			  Hv_PARENT,          Item,
			  Hv_TEXT,            (char *)"Double Click Text and/or Box",
			  Hv_CHILDPLACEMENT,  Hv_POSITIONBELOW,
			  NULL);


/* Create two similar user items with text children.
   Make them a litte different for illustrative purposes. 
   ESPECIALLY note that the first one will have a timed redraw!*/


  Item = Hv_VaCreateItem(View,
			 Hv_USERDRAW,    simpleDraw1,
			 Hv_DRAWCONTROL, Hv_DRAGGABLE,
			 Hv_LEFT,        hotrect->left + 30,
			 Hv_TOP,         hotrect->top  + 50,
			 Hv_WIDTH,       40,
			 Hv_HEIGHT,      40,
			 Hv_REDRAWTIME,  200,
			 NULL);


/* a similar item, but with some different draw controls */


  Item = Hv_VaCreateItem(View,
			 Hv_USERDRAW,    simpleDraw1,
			 Hv_DRAWCONTROL, Hv_FEEDBACKWHENDRAGGED + Hv_FANCYDRAG,
			 Hv_LEFT,        hotrect->left + 90,
			 Hv_TOP,         hotrect->top  + 140,
			 Hv_WIDTH,       50,
			 Hv_HEIGHT,      60,
			 NULL);

  Child = Hv_VaCreateItem(View,
			  Hv_TYPE,            Hv_TEXTITEM,
			  Hv_PARENT,          Item,
			  Hv_TEXT,            (char *)"Drag Me Too",
			  Hv_TEXTCOLOR,       Hv_brown,
			  Hv_CHILDPLACEMENT,  Hv_POSITIONABOVE,
			  NULL);

/* and just to be completely absurd, lets put a button in the hotrect.
   SINCE DEFAULT DOMAIN FOR BUTTONS IS OUTSIDE HOTRECT, NEED TO SET
   IT EXPLICITLY  */

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,        Hv_BUTTONITEM,
			 Hv_DOMAIN,      Hv_INSIDEHOTRECT,
			 Hv_SINGLECLICK, simpleCB,
			 Hv_DRAWCONTROL, Hv_DRAGGABLE,
			 Hv_LEFT,        hotrect->left + 190,
			 Hv_TOP,         hotrect->top  + 120,
			 Hv_FONT,        Hv_fixed,
			 Hv_AUTOSIZE,    True,
			 Hv_TEXT,        " why ask why? ",
			 NULL);

}


/* ------- simpleSliderCB -------*/

static void simpleSliderCB(hvevent)

Hv_Event    hvevent;

{
  Hv_Item          Item = hvevent->item;
  Hv_View          View = hvevent->view;
  Hv_SliderData    *slider;

  slider = (Hv_SliderData *)(Item->data);

/* user code here */
}


/*---------- IconDraw ------------*/

static void   IconDraw(Hv_Item   Item,
		       short     choice,
		       Hv_Rect   *rect)

/**********************************
  example of an icon drawer for the
  iconbased menu button. The routine
  should CONFINE its drawing to the
  given rect.
***********************************/

{
  short row, col;

  Hv_ShrinkRect(rect, 2, 2);
  row = choice / 256;
  col = choice % 256;

  if (row == 2) {
    Hv_FillRect(rect, Hv_red+2*col);
    Hv_FrameRect(rect, Hv_black);
  }
  else {
    Hv_ShrinkRect(rect, 0, 3);
    Hv_FillOval(rect, True, Hv_red+2*col, Hv_black);
  }
}

/*--------------- TextEntryCB -------------*/

static void             TextEntryCB(Hv_Widget  w,
				    Hv_Pointer data)

/*****************************************
  callback routine demonstrating how to
  uses text entries as they occur.

  w: the underlying MOTIF widget

  data: the Hv_Item  (obtain by a cast:
        Item = (Hv_Item)data)

*****************************************/

{
  char *str;

  str = Hv_GetString(w);
  fprintf(stderr, "%s\n", str);
  Hv_Free(str);
}



/*-------- MenuCB ------------*/

static void MenuCB(Hv_Item  Item,
		   short    current)

/***********************************
  callback routine for menu buttons
***********************************/

{
  fprintf(stderr, "MB Callback: current choice: %d\n", current);
}


/*-------- MenuIconCB ------------*/

static void MenuIconCB(Hv_Event hvevent)

/***********************************
  callback routine for "button" part
  of menu button
***********************************/

{
  fprintf(stderr, "Icon MB Callback\n");
}


