/*
 *  Hv_private.h is the header containing "private" function prototypes
 *
 *  These should not be used by developers, but since this file is
 *  included in Hv.h, they are available. (There is no implied
 *  promise that these routines will be available in future releases)
 *  
 *  The Hv library was developed at CEBAF under contract to the
 *  Department of Energy and is the property of they U.S. Government.
 *  
 *  It may be used to develop commercial software, but the Hv
 *  source code and/or compiled modules/libraries may not be sold.
 *  
 *  Questions or comments should be directed to heddle@cebaf.gov
 */

#ifndef __HVPRIVATEH__
#define __HVPRIVATEH__

/* private global variables */

extern Boolean         Hv_usedNewDrawTools; /* used to distinguish old & new drawing tools */
extern Boolean         Hv_okSetColor;  /* used in conjunction with pats in PS */
extern unsigned char * Hv_bitmaps[Hv_NUMPATTERNS];
extern Hv_Pixmap       Hv_patterns[Hv_NUMPATTERNS];

extern Boolean         Hv_addWelcomeRainbow;   /* False-> no welcome rainbow */ 
extern unsigned int    Hv_activePointers;      /* total #active pointers malloced */

extern BalloonData     Hv_theBalloon;                      /* one global balloon for ced */

/* balloon displayer breaks down the one giant string int these bite sized pieces */

extern char             *Hv_balloonStrs[Hv_MAXBALLOONSTR];   
extern Hv_CompoundString Hv_balloonCstr[Hv_MAXBALLOONSTR];   /* Motif strings for balloons*/
extern Boolean         Hv_showBalloons;                    /* global on/off for balloon help*/
extern Boolean         Hv_balloonUp;                       /* program name */
extern char           *Hv_programName;
extern char           *Hv_versionString;
extern char           *Hv_appName;

extern short            Hv_scrollInactiveColor;       /* for inactive scroll arrows */
extern short            Hv_scrollActiveColor;         /* for active scroll arrows */

extern Hv_ColorMap	Hv_colorMap;	   /* this program's color map */
extern short		Hv_rainbowFirst;   /* index of first color used for "thermal" plots*/
extern short		Hv_rainbowLast;	   /* index of last color used for "thermal" plots*/
extern short            Hv_scrollBarColor;        /*color used by active scroll bars*/

extern char            *Hv_ioPaths[Hv_NUMSEARCHPATH + 1];

/* postscript related private global variables */

extern  FILE           *Hv_psfp;                 /* postscript file pointer */
extern  Hv_FRect        Hv_psWorld;              /*world system for ps page, in 72nds of inch*/
extern  Hv_Rect         Hv_psLocal;              /*local system for postscript page, in PS pixels*/
extern  float           Hv_psResolutionFactor;   /* how much better PS screen is than real screen*/
extern  float           Hv_psUnitsPerInch;       /* default = 72 -> one unit = 1/72inch*/
extern  Boolean         Hv_psWholeScreen;        /* True to capture the whole screen */
extern  short           Hv_psCurrentColor;
extern  char           *Hv_psFileName;           /* file name of postscript file */
extern  Boolean         Hv_psFirstPattern;       /* to delay pattern init until necessary */
extern  Boolean         Hv_psnomoveto;           /* used to align compound strings */
extern  short           Hv_psrelx;               /* for relative offset (compound strings) */
extern  short           Hv_psrely;               /* for relative offset (compound strings) */

extern  Boolean         Hv_psUseFooter;          /* use footer on printout? */
extern  char           *Hv_psNote;               /* a extra string added at the bottom */
extern  Boolean         Hv_psTile;
extern  Boolean         Hv_psEntireView;         /* caputure everthing or just hot_rect */
extern  Boolean         Hv_psExpandToFit;        /* expand a small picture? */
extern  int             Hv_psOrientation;        /* landscape or portrait */
extern  char           *Hv_psDefaultFile;

extern Hv_DialogData    Hv_dialogList;           /* global linked list of dialogs created */


/* global screen Vars */

extern Hv_VisualInfo    Hv_visualInfo;      /* Visual info structure */
extern unsigned int	Hv_colorDepth;	    /* color depth of this monitor */
extern short		Hv_mainWidth;	    /* width of main window in pixels */
extern short		Hv_mainHeight;	    /* height of main window in pixels */
extern unsigned long	Hv_mainFg;	    /* foreground color of main window */
extern Hv_Rect       	Hv_canvasRect;      /* canvas geometery */
extern short            Hv_root_width;
extern short            Hv_root_height;
extern Hv_GC	        Hv_gc;              /* main graphics context */

extern Hv_AppContext    Hv_appContext;      /* Application Context */

/* private global widgets */

extern Hv_Widget	Hv_toplevel;	     /* highest level widget  (Shell widget) */
extern Hv_Widget        Hv_form;              /* form widget for main window */
extern Hv_Widget	Hv_canvas;	     /* main drawing area */
extern Hv_Widget	Hv_menuBar;	     /* Main Menu bar in main window */

/* variable length arg list vars */

extern char            **Hv_vaText;
extern int             *Hv_vaTags;
extern int             *Hv_vaFonts;
extern int             *Hv_vaColors;
extern short            Hv_vaNumOpt;

/* private global menus */

extern Hv_Widget          Hv_popupMenu;
extern Hv_Widget          Hv_popupMenu2;
extern Hv_Widget          Hv_undoSubmenu;

#ifdef __cplusplus
extern "C"
{
#endif

/*---------- Hv_attributes.c ------*/

extern void     Hv_GetAttributes(va_list ap,
				 Hv_AttributeArray attributes,
				 char **optlist,
				 int *tags,
				 int *fonts,
				 int *colors,
				 short *numopt);
  
/*---------- Hv_balloon.c ---------*/

extern void            Hv_InitBalloons(void);

extern void            Hv_CreateBalloon(Hv_Item,
					char *);

extern void            Hv_DestroyBalloon(Hv_Item);

extern void            Hv_StuffBalloon(Hv_Point,
				       char *);

extern void            Hv_ShowBalloon(void);

extern void            Hv_HideBalloon(void);

extern void            Hv_BalloonPoint(Hv_View,
				       Hv_Item,
				       Hv_Point *,
				       short);

extern void            Hv_PrepareBalloon(Hv_View,
					 Hv_Item,
					 short,
					 char *);

/*--------- Hv_box.c -------*/

extern void            Hv_BoxInitialize(Hv_Item Item,
					Hv_AttributeArray attributes);

/*--------- Hv_button.c -------*/

extern void            Hv_DrawButtonText(Hv_Item,
					 Hv_Region);

extern void            Hv_ButtonItemInitialize(Hv_Item,
					       Hv_AttributeArray);

/*--------- Hv_buttongroup.c -------*/

extern void            Hv_ButtonGroupInitialize(Hv_Item      Item,
						Hv_AttributeArray attributes);

extern void            Hv_DestroyButtonGroup(Hv_Item Item);

extern void            Hv_ButtonGroupMouseMotion(Hv_Item     Item,
						 short       x,
						 short       y);

extern void            Hv_ButtonGroupExit(Hv_Item Item);


/*--------- Hv_callback.c -------*/

extern void            Hv_CameraCallback(Hv_Event hvevent);

extern void            Hv_DefaultSingleClickCallback(Hv_Event hvevent);


/*-------- Hv_choice.c --------*/

extern void            Hv_DrawChoiceItem(Hv_Item   Item,
					 Hv_Region  region);

extern void            Hv_DrawChoiceSetItem(Hv_Item   Item,
					    Hv_Region  region);

extern void            Hv_ChoiceInitialize(Hv_Item      Item,
					   Hv_AttributeArray attributes);

extern void            Hv_ChoiceSetInitialize(Hv_Item      Item,
					      Hv_AttributeArray attributes);

/****************** BACKWARD COMPATIBILITY WRAPPERS *******************/

extern void            Hv_DrawRadioButtonItem(Hv_Item   Item,
					      Hv_Region  region);

extern void            Hv_DrawRadioContainerItem(Hv_Item   Item,
						 Hv_Region  region);

extern void            Hv_RadioButtonInitialize(Hv_Item      Item,
						Hv_AttributeArray attributes);

extern void            Hv_RadioContainerInitialize(Hv_Item      Item,
						   Hv_AttributeArray attributes);

/**********************************************************************/

/*--------- Hv_color.c ----------*/

extern void            Hv_AdjustAdjacentColors(short,
					       short);

extern void            Hv_ChangeBackgroundColor(short);


/*------- Hv_colorbutton.c -------*/

extern void            Hv_ColorButtonInitialize(Hv_Item,
						Hv_AttributeArray);

/*-------- Hv_compound.c ------------*/

extern void            Hv_DrawCompoundString(short x,
					     short y,
					     Hv_String *str);

extern void            Hv_CompoundStringProcess(Hv_String    *str,
						short        x,
						short        y,
						short        *w,
						short        *h,
						int          option);


/*-------- Hv_configure.c -----------*/

extern void           Hv_ReadConfigurationFile(char *fname);

extern void           Hv_OpenConfiguration();

extern void           Hv_SaveConfiguration();

extern void           Hv_ApplyConfiguration(Hv_View View);


/*-------- Hv_connect.c ----------*/

extern void            Hv_ConnectionInitialize(Hv_Item,
					       Hv_AttributeArray);

/*--------- Hv_container.c --------*/

extern void            Hv_InsideContainerArea(Hv_View   View,
					      Hv_Rect   *carea);

extern void            Hv_DrawContainerItem(Hv_Item,
					    Hv_Region);

extern void 	       Hv_DrawViewTitleBorder(Hv_View,
					      Boolean,
					      Boolean);

extern void 	       Hv_DrawViewTitle(Hv_View);			

/*--------- Hv_drag.c -----*/

extern void            Hv_DragItem(Hv_Item,
				   Hv_Rect *,
				   Hv_Point,
				   Boolean);
  
/*--------- Hv_dlogs.c --------*/

extern Hv_DialogData   Hv_GetDialogData(Hv_Widget);


/*--------- Hv_drawsupport.c -------*/

extern void            Hv_RectWithinContainer(Hv_View,
					      Hv_Rect *);

extern unsigned long   Hv_SwapForeground(short);

extern unsigned long   Hv_SwapBackground(short);

/*-------- Hv_feedback.c -------*/

extern void            Hv_DrawFeedbackItem(Hv_Item,
					   Hv_Region);

extern void            Hv_FeedbackInitialize(Hv_Item,
					     Hv_AttributeArray);

extern void            Hv_GetFeedbackArea(Hv_Item,
					  Hv_Rect *);

/*-------- Hv_fractdoneitem.c --------*/

extern void            Hv_DrawFractionDoneItem(Hv_Item,
					       Hv_Region);

extern void            Hv_FractionDoneInitialize(Hv_Item,
						 Hv_AttributeArray);

/*--------- Hv_help.c ----------*/

extern void            Hv_DoHelp(void);

/*-------- Hv_io.c -----*/

extern void            Hv_UpperCase(char *);


/*-------- Hv_items.c --------*/

extern void           Hv_ItemChangedRedraw(Hv_Item     Item,
					   Hv_Region   oldrgn);


extern void            Hv_ToggleItemState(Hv_Item Item);

extern void            Hv_ItemStringAttributes(Hv_Item          Item,
					       Hv_AttributeArray attributes);

extern void            Hv_ItemName(Hv_Item Item,
				   char   *name);

extern void            Hv_RepositionHotRectItems(Hv_ItemList);

extern Hv_Item         Hv_SetupHotRectBorderItem(Hv_View);

extern void            Hv_DefaultSchemeIn(Hv_ColorScheme  **);

extern void            Hv_DefaultSchemeOut(Hv_ColorScheme  **);

/*-------- Hv_led.c -------*/

extern void            Hv_DestroyLEDItem(Hv_Item Item);

extern void            Hv_DrawLEDItem(Hv_Item,
				      Hv_Region);

extern void            Hv_LEDInitialize(Hv_Item,
					Hv_AttributeArray);

/*-------- Hv_math.c -------*/

extern float          Hv_fMin(float i,
			      float j);

extern float          Hv_fMax(float i,
			      float j);

extern short          Hv_sMin(short i,
			      short j);

extern short          Hv_sMax(short i,
			      short j);

extern int            Hv_iMin(int i,
			      int j);

extern int            Hv_iMax(int i,
			      int j);

extern int            Hv_nint(float   f);

extern void           Hv_Spline(float x[],
				float y[],
				int n,
				float yp1,
				float ypn,
				float y2[]);

extern void           Hv_Splint(float xa[],
				float ya[],
				float y2a[],
				int n,
				float x,
				float *y);

extern void           Hv_Polint(float xa[],
				float ya[],
				int n,
				float x,
				float *y,
				float *dy);

extern void           Hv_Fit(float x[],
			     float y[],
			     int ndata,
			     float sig[],
			     int mwt,
			     float *a,
			     float *b,
			     float *siga,
			     float *sigb,
			     float *chi2,
			     float *q);

extern void           Hv_LFit(float x[],
			      float y[],
			      float sig[],
			      int ndata,
			      float a[],
			      int ma,
			      float *chisq,
			      void (*funcs)(float, float*, int));

extern void           Hv_SVDFit(float x[],
				float y[],
				float sig[],
				int ndata,
				float a[],
				int ma,
				float *chisq,
				void (*funcs)(float, float*, int));

extern void           Hv_MRQMin(float x[],
				float y[],
				float sig[],
				int ndata,
				float a[],
				int ma,
				float *chisq,
				void (*funcs)(float x, float a[], float *y, float dyda[], int na));

/*--------- Hv_menubutton.c -------*/

extern void            Hv_MenuButtonInitialize(Hv_Item Item,
					       Hv_AttributeArray attributes);

extern void            Hv_DestroyMenuButtonItem(Hv_Item Item);

extern void            Hv_DrawMenuButtonItem(Hv_Item,
					     Hv_Region);


/*--------- Hv_menus.c --------*/

extern void             Hv_CreateMenuBar(void);

extern void             Hv_CreatePopups(void);

/*-------- Hv_optionbutton.c --------*/

extern void             Hv_OptionButtonInitialize(Hv_Item      Item,
						  Hv_AttributeArray attributes);



/*--------- Hv_postscript.c --------*/

extern void         Hv_PSPatternInit(void);

extern void         Hv_PSGetRGB(short color,
				double *r,
				double *g,
				double *b);

extern char         *Hv_MakeProperPSString(char  *str);

extern void          Hv_WorldToPSLocal(float     fx,
				       float     fy,
				       short     *x,
				       short     *y,
				       Hv_FRect	*world,
				       Hv_Rect     *local);

extern void          Hv_LocalToPSLocal(short     xloc,
				       short     yloc,
				       short     *xps,
				       short     *yps);   

extern void          Hv_PSLocalToPS(short  x,
				    short  y,
				    float  *psx,
				    float  *psy);

extern void           Hv_PSToPSLocal(float psx,
				     float psy,
				     short *x,
				     short *y);

extern void            Hv_LocalPolygonToPSLocalPolygon(Hv_Point *xp,
						       Hv_Point *psxp,
						       int   np);

extern void            Hv_LocalRectToPSLocalRect(Hv_Rect    *local,
						 Hv_Rect    *ps);

extern void            Hv_LocalSegmentsToPSLocalSegments(Hv_Segment    *local,
							 Hv_Segment    *ps,
							 short      n);

extern void            Hv_LocalRectangleToPSLocalRectangle(Hv_Rectangle *local,
							   Hv_Rectangle *ps);

extern void            Hv_PSFont(short    font,
				 char    *fontname,
				 float    *fontsize);

extern void            Hv_PSSetColor(short color);

extern void            Hv_PSComment(char *comment);

extern void            Hv_PSSave(void);

extern void            Hv_PSRestore(void);

extern void            Hv_PSFullClip(void);

extern void            Hv_PSClipPolygon(Hv_Point    *xp,
					short     np);

extern void            Hv_PSClipRect(Hv_Rect  *rect);

extern void            Hv_PSClipHotRect(Hv_View   View);

extern void            Hv_PrintView (Hv_View   View,
				     Boolean   NotWholeScreen);

extern void            Hv_PrintScreen (void);

/*--------- Hv_print.c -----------*/

extern void            Hv_PrinterSetup(Hv_View);


/*--------- Hv_privdraw.c -------*/


extern void            Hv_GetTemporaryWorld(Hv_FRect *,
					    Hv_Rect *,
					    Hv_FRect *,
					    Hv_Rect *);


extern void            Hv_DrawWorldGrid(Hv_View,
					Hv_FRect *,
					short,
					short,
					short,
					short);

extern void            Hv_DrawClockItem(Hv_Item,
					Hv_Region);

extern void            Hv_DrawHotRectBorderItem(Hv_Item,
						Hv_Region);

extern void            Hv_DrawCamera(Hv_Item,
				     Hv_Region);

extern void            Hv_FrameXorPolygon(Hv_Point *xp,
					  short     np);

extern void            Hv_DrawXorRect(Hv_Rect *);

extern void            Hv_DrawXorOval(Hv_Rect *);

extern void            Hv_DrawXorLine(Hv_Point *,
				      Hv_Point *);

extern void            Hv_DrawXorLines(Hv_Point *,
				       short np);

extern void 	       Hv_DrawXorItem(Hv_Item,
				      Hv_Region);

extern void            Hv_Draw3DRectangle(short,
					  short,
					  short,
					  short,
					  Hv_ColorScheme *);

extern void	       Hv_PolygonEnclosingRectangle(Hv_Rectangle *,
						    Hv_Point *,
						    int);


extern void            Hv_Simple3DRectangle(short,
					    short,
					    short,
					    short,
					    Boolean,
					    short);


/*-------- Hv_psdraw.c ----------------*/

extern void Hv_PSFrameWedge(short xc,
			    short yc,
			    short rad,
			    int   start,
			    int   del,
			    short color);

extern void          Hv_PSDrawPoint(short  x,
				    short  y,
				    short  color);

extern void          Hv_PSDrawPoints(Hv_Point *p,
				     short  n,
				     short  color);

extern void          Hv_PSFillRect (Hv_Rect   *rect,
				    short     color);

extern void           Hv_PSFrameRect (Hv_Rect  *ps,
				      short    color);

extern void            Hv_PSFrameOval(Hv_Rect *rect,
				      short    color);

extern void 	       Hv_PSFillOval(Hv_Rect *rect,
				     short    color);
 
extern void	      Hv_PSDrawLine(short	x1,
				    short	y1,
				    short	x2,
				    short	y2,
				    short      color);          

extern void           Hv_PSFrameCircle(short     xc,
				       short     yc,
				       short     rad,
				       short     color);

extern void            Hv_PSFillCircle(short     xc,
				       short     yc,
				       short     rad,
				       short     color);

extern void            Hv_PSDrawLines(Hv_Point *xp,
				      short   n,
				      short   color);

extern void            Hv_PSFramePolygon(Hv_Point    *xp,
		       short     np,
		       short     color);

extern void            Hv_PSFillPolygon(Hv_Point    *xp,
					short     np,
					short     color);

extern void            Hv_PSSetPattern(int    pattern,
				       short  color);

extern void            Hv_PSSetLineWidth(int w);

extern void            Hv_PSSetLineStyle(int w,
					 int style);

extern void            Hv_PSMoveTo(float  x ,
				   float  y);

extern void            Hv_PSRMoveTo(float  x ,
				   float  y);

extern void            Hv_PSDrawHCenteredString(float xleft,
						float xright ,
						float y,
						float fontsize,
						char  *fontname,
						char  *string,
						short color);

extern void            Hv_PSDrawCenteredString(float  xl,
					       float  xr,
					       float  yb,
					       float  yt,
					       float  fontsize,
					       char   *fontname,
					       char   *string,
					       short  color,
					       Boolean shadow,
					       short  shadowcolor);

extern void            Hv_PSDrawString(float  x ,
				       float  y,
				       float  fontsize,
				       char   *fontname,
				       char   *string,
				       short  orientation,
				       short  color,
				       Boolean shadow,
				       short  shadowcolor);


extern void            Hv_PSDrawSegments(Hv_Segment  *segs,
					 short    n,
					 short    color);

extern void            Hv_PSDrawWorldCircle(Hv_View View,
					    float   fx,
					    float   fy,
					    float   frad,     
					    Boolean  Fill,
					    short   framecolor,
					    short   fillcolor);

/*--------- Hv_radiodlogs.c ---------*/


extern Boolean         Hv_WidgetInRadioList(Hv_Widget      w,
					    Hv_RadioHeadPtr   radiohead);

extern Hv_Widget       Hv_PosGetRadioWidget(short      pos,
					    Hv_RadioHeadPtr radiohead);

extern short           Hv_GetActiveRadioButtonPos(Hv_RadioHeadPtr radiohead);

extern Hv_Widget       Hv_GetActiveRadioWidget(Hv_RadioHeadPtr radiohead);


/*-------- Hv_rainbowscale.c -------*/

extern void            Hv_DrawRainbowItem(Hv_Item    Item,
					  Hv_Region   region);

extern void            Hv_RainbowInitialize(Hv_Item      Item,
					    Hv_AttributeArray attributes);


/*--------- Hv_rectsupport.c --------*/

extern void	      Hv_SetXRectangle(Hv_Rectangle *r,
				       short  	     left,
				       short	     top,
				       short	     width,		
				       short         height);

extern Boolean        Hv_FPointInRectangle(Hv_FPoint	pp,
					   float	x,
					   float	y,
					   float        w,
					   float        h);

extern Boolean        Hv_PointInRectangle(Hv_Point 	pp,
					  short	        x,
					  short	        y,
					  short         w,
					  short         h);

extern void            Hv_GoodRect(Hv_Rect  *rect);

extern void            Hv_LeftTopResizeRect(Hv_Rect    *r,
					    short       dh,
					    short       dv);

extern short           Hv_WhichPointOnRect(short   x,
					   short   y,
					   Hv_Rect  *r);

extern void            Hv_GetRectCorner(short   corner,
					Hv_Rect  *area,
					Hv_Point *p);

extern void            Hv_KeepRectInsideRect(Hv_Rect  *theR,
					     Hv_Rect  *testR);


/*--------- Hv_region.c --------*/

extern Hv_Region       Hv_RectangleRegion(short  x,
					  short  y,
					  short  w,
					  short  h);

/*-------  Hv_scroll.c -------*/

extern  void Hv_UpdateThumbs(Hv_View    View,
			    Boolean    HorizontalScroll,
			    short     OldTLen,
			    short     OldTWid);

extern void Hv_CheckHShift(Hv_View  View,
			   short    *dh,
			   float    *fdh);

extern void Hv_CheckVShift(Hv_View  View,
			   short    *dv,
			   float    *fdv);

extern short Hv_GetHScrollStep(Hv_View  View,
			       int     ScrollPart,
			       Hv_Region    region,
			       Hv_Point   StartPP,
			       Hv_Point   StopPP,
			       short    efflen);

extern short Hv_GetVScrollStep(Hv_View   View,
			       int     ScrollPart,
			       Hv_Region    region,
			       Hv_Point   StartPP,
			       Hv_Point   StopPP,
			       short    efflen);

extern void           Hv_SetScrollActivity(Hv_View  View);

extern void           Hv_VertThumbInfo(Hv_View  View,
				       short    *t,
				       short    *h,
				       short    *eff_height);

extern void           Hv_HorizThumbInfo(Hv_View   View,
					short     *l,
					short     *w,
					short     *eff_width);

extern void            Hv_DrawScrollBars(Hv_View     View,
					 Hv_Region      region);

extern void            Hv_HandleScroll(Hv_View  View,
				       int    ScrollPart,
				       Hv_Point  StartPP);

extern void            Hv_DrawScrollArrows(Hv_View   View,
					   Hv_Region   region,
					   short    control);

/*-------- Hv_slider.c --------*/

extern void           Hv_DrawSliderItem(Hv_Item    Item,
					Hv_Region   region);

extern void           Hv_SliderInitialize(Hv_Item      Item,
					  Hv_AttributeArray attributes);

/*-------- Hv_separator.c --------*/

extern void           Hv_SeparatorInitialize(Hv_Item      Item,
					     Hv_AttributeArray attributes);

/*-------- Hv_standard.c --------*/

extern void           Hv_StandardToolBackground(Hv_Item Item,
						Hv_Rect *area);

extern Hv_Widget           Hv_StandardMainRowCol(Hv_Widget  parent,
						 short    spacing);

extern Hv_Widget           Hv_StandardPackingRowCol(Hv_Widget  parent,
						    short    numrow,
						    short    spacing);

/*-------- Hv_textentryitem.c --------*/

extern void            Hv_DrawTextEntryItem(Hv_Item    Item,
					    Hv_Region   region);

/*-------- Hv_textitem.c --------*/

extern void            Hv_CheckDanglingTextItem(void);

extern void            Hv_EditTextItem(Hv_Event   hvevent);

extern void            Hv_TextButtonCallback(Hv_Event  hvevent);

extern void            Hv_DrawTextItem(Hv_Item    Item,
				       Hv_Region   region);

extern void            Hv_DrawTextButton(Hv_Item    Item,
					 Hv_Region   region);

extern void            Hv_TextItemInitialize(Hv_Item      Item,
					     Hv_AttributeArray attributes);

/*-------- Hv_utilities.c --------*/

extern Boolean         Hv_FileExists(char  *fname);

extern Boolean         Hv_CheckFileExists(char  *fname);

extern float          *Hv_NewVector(int n);

extern int            *Hv_NewIVector(int n);

extern void            Hv_PointerReport(char *str);

extern char           *Hv_UnixTimeToHvTime(char *unixdate);

extern void            Hv_itoa(int   n,
			       char   s[]);

extern void            Hv_positoa(int   n,
				  char  s[],
				  int   numblanks);

extern void            Hv_nonnegitoa(int   n,
				     char  s[],
				     int   numblanks); 

extern void            Hv_RemoveFileExtension(char  *fname);

extern void            Hv_StripLeadingDirectory(char  **path);

extern void            Hv_CheckFont(short font);

extern int             Hv_ParseString(char  *str); 

extern short           Hv_CorrespondingFont(short font,
					    int  control);

extern void            Hv_ReplaceCommas(char  *line);

extern void            Hv_ReplaceNewLine(char  *line);

extern void            Hv_RemoveLeadingBlanks(char **s);

extern void            Hv_RemoveTrailingBlanks(char *s);

extern void            Hv_InvalidOptions(unsigned int	argc,
					 char	*argv[]);

extern int             Hv_CountBits(int control);

extern int             Hv_FirstSetBit(int control);

extern void            Hv_SetBit(int     *Control,
				 int     BitInt);

extern void            Hv_ClearBit(int    *Control,
				   int     BitInt);

extern void            Hv_ToggleBit(int     *Control,
				    int     BitInt);

extern Boolean         Hv_CheckBit(int    Control,
				   int    BitInt);

extern void            Hv_SetBitPosition(int     *Control,
					 int     pos);

extern void            Hv_ClearBitPosition(int    *Control,
					   int     pos);

extern void            Hv_ToggleBitPosition(int     *Control,
					    int     pos);

extern Boolean         Hv_CheckBitPosition(int    Control,
					   int    pos);

/*-------- Hv_views.c ----------*/

extern void           Hv_ResizeView(Hv_View  View,
				    short    dh,
				    short    dv);

extern void           Hv_OffsetView (Hv_View  View,
				     short    dh,
				     short    dv);

extern void           Hv_SetViewDrawControl(Hv_View   View,
					    int     drawcontrol);


extern void           Hv_MoveView(Hv_View  View,
				  short    dh,
				  short    dv);

extern void           Hv_GrowView(Hv_View   View,
				  short     dh,
				  short     dv);

extern void           Hv_FlipView(Hv_View  View);


extern Hv_Region      Hv_GetInsideContainerRegion(Hv_View  View);

extern void           Hv_SendViewToFront(Hv_View View);

extern void           Hv_SendViewToBack(Hv_View  View);

extern void           Hv_RestoreDefaultWorld(Hv_View  View);

extern void           Hv_AutosizeView(Hv_View   View);

extern void           Hv_UpdateViews(Hv_Region   region);

extern void           Hv_ZoomView(Hv_View   View,
				  Hv_Rect   *rect);

extern void           Hv_PercentZoom (Hv_View   View,
				      float     amount);

extern void           Hv_QuickZoomView(Hv_View View,
				       float   xmin,
				       float   xmax,
				       float   ymin,
				       float   ymax);

extern Hv_Region      Hv_TotalViewRegion(void);

extern void           Hv_CloseView(Hv_View View);

extern void           Hv_OpenView(Hv_View View);

extern void           Hv_DumpView(Hv_View  View,
				  FILE   *fp);

extern void           Hv_DumpViewConnections(Hv_View  View,
					     FILE   *fp);

extern void           Hv_WhichView(Hv_View  *View,
				   Hv_Point   pp);

extern void           Hv_SetupWelcomeView(Hv_View  View);

/*-------- Hv_virtual.c --------*/

extern void           Hv_UpdateVirtualView(void);

extern void           Hv_CreateVirtualView(Hv_AttributeArray attributes);

extern void           Hv_SendVVRepToFront(Hv_View View);

extern void           Hv_SendVVRepToBack(Hv_View View);

extern void           Hv_CenterVVAroundView(Hv_View View);

extern void           Hv_DeleteVVRep(Hv_View View);

/*-------- Hv_wheel.c --------*/

extern void           Hv_DrawWheelItem(Hv_Item    Item,
				       Hv_Region   region);

extern void           Hv_WheelInitialize(Hv_Item      Item,
					 Hv_AttributeArray attributes);

extern void           Hv_EditWheelValues(Hv_Event   hvevent);

/*-------- Hv_worldimage.c ----------*/

extern void           Hv_WorldImageInitialize(Hv_Item      Item,
					      Hv_AttributeArray attributes);

extern void           Hv_DestroyWorldImage(Hv_Item Item);

/*-------- Hv_worldline.c ----------*/

extern void           Hv_DrawWorldLineButton(Hv_Item    Item,
					     Hv_Region   region);

extern void           Hv_WorldLineButtonCallback(Hv_Event  hvevent);

extern void           Hv_WorldLineInitialize(Hv_Item      Item,
					     Hv_AttributeArray attributes);

extern void           Hv_SetWorldLineBasedOnEndPoints(Hv_Item   Item,
						      float    startx,
						      float    starty,
						      float    stopx,
						      float    stopy);

extern short          Hv_WhichPointOnFLine(float   x,
					    float   y,
					    Hv_FRect  *wr);

extern void           Hv_GetFLineCorner(short   corner,
					Hv_FRect *wr,
					Hv_FPoint *fp);

extern short          Hv_WhichPointOnLine(short   x,
					  short   y,
					  Hv_Rect  *r);

extern void           Hv_GetLineCorner(short   corner,
				       Hv_Rect  *r,
				       Hv_Point *p);


/*-------- Hv_worldpolygon.c ----------*/

extern void Hv_ArrowPolygonFromWorldLine(Hv_View    View,
					 Hv_FPoint  *fp1,
					 Hv_FPoint  *fp2,
					 short      thickness,
					 Hv_Point   *poly);

extern void           Hv_DrawWorldPolygonButton(Hv_Item    Item,
						Hv_Region   region);

extern void           Hv_WorldPolygonButtonCallback(Hv_Event  hvevent);

extern void           Hv_WorldPolygonInitialize(Hv_Item      Item,
						Hv_AttributeArray attributes);

extern void           Hv_DestroyWorldPolygon(Hv_Item Item);

/*-------- Hv_worldrect.c ----------*/

extern void           Hv_WorldRectButtonCallback(Hv_Event  hvevent);

extern void           Hv_DrawWorldRectButton(Hv_Item    Item,
					     Hv_Region   region);

extern void           Hv_WorldRectInitialize(Hv_Item      Item,
					     Hv_AttributeArray attributes);

extern void           Hv_DrawWorldOvalButton(Hv_Item    Item,
					     Hv_Region   region);

extern void           Hv_WorldOvalButtonCallback(Hv_Event  hvevent);


/*-------- Hv_worldwedge.c ----------*/

extern void           Hv_DestroyWorldWedge(Hv_Item Item);


extern void           Hv_DrawWorldWedgeButton(Hv_Item    Item,
						Hv_Region   region);

extern void           Hv_WorldWedgeButtonCallback(Hv_Event  hvevent);

extern void           Hv_WorldWedgeInitialize(Hv_Item      Item,
					      Hv_AttributeArray attributes);


#ifdef __cplusplus
}
#endif

#endif











