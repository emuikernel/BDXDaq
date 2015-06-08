#ifndef __HVW32GATEWAYH__
#define __HVW32GATEWAYH__

#define Hv_stc32       0x045f 

#define SYSFONT_WIDTH    8.0
#define SYSFONT_HEIGHT   16.0

#define ALIGN_DWORD( type, p ) ( (type)(((DWORD)p+3) & ~3 ))
#define DLGTEMPLATE_WORKING_SIZE 18192
#define Hv_STDOUTWIN 3041
#define Hv_STDERRWIN 3042
#define Hv_PRINTFOK  3030

/* timer Hv_TIMERID is an arbitrary number used to check timeout event */
/* Hv_TIMERVALUE is the number of milliseconds that Hv uses for the timer */
#define Hv_TIMERID   2332
#define Hv_TIMERVALUE 100
/* some macros to access widget fields */

#define GETHANDLE(WIDGET) (WIDGET)->wdata
#define GETTYPE(WIDGET) (WIDGET)->wtype
#define SETHANDLE(WIDGET, HANDLE) (WIDGET)->wdata = (HANDLE)
#define GETWIN32ID(WIDGET) (WIDGET)->wcallback_id
#define GETMENUHANDLE(WIDGET) (HMENU)((WIDGET)->wdata)


#define   MENUITEM       1
#define   TOGGLEMENUITEM 2
#define   SUBMENUITEM    3
#define   MENU           4
#define   MENUBAR        5
#define   TIMER          2334

typedef	struct dibspec {
		BITMAPV4HEADER ih;
		RGBQUAD Colors[256];
	} DIBSpec;

/* used to keep track of all the outstanding timer events */


typedef	struct hvtimerstruct {
		long   id;
		DWORD timoutTime;
		Hv_TimerCallbackProc  callback;
		Hv_Pointer            data;
	} hvTimer;


/*
 * the following structure is used to hold the Widget and Handle of every open dialog box
 * this information is used to cross reference the HWND structure with the Widget
 * in order to process a WM_CLOSE message
 */
typedef	struct hvopendialogsstruct {
		Hv_Widget dialog;
		HWND      hwnd;
	} hvOpenDialog;

typedef hvOpenDialog *Hv_OpenDialogs;


/*
 * the following structure is used to hold the Widget and Handle of every verticval scroll bar
 * this information is used to cross reference the HWND structure with the Widget
 * in order to process a WM_VSCROLL message
 */
typedef	struct hvvscrolldialogsstruct {
		Hv_Widget w;
		HWND      hwnd;     // used to lookup the widget from the WM_VSCROLL message
		HWND      dialog;   // used to free this entry when the "dialog" is closed 
		short     id;
	} hvVscrollWidget;

typedef hvVscrollWidget *Hv_OpenVscrolls;

extern HFONT     *Hv_fonts;
extern Hv_CallbackData  *callbacks;
extern int               maxCallbacks;


/**
 * <EM>Where all Windows 9X/NT specific commands are prototyped.</EM>
 * <P>
 * The Hv library was developed at The Thomas Jefferson National
 * Accelerator Facility under contract to the
 * Department of Energy and is the property of they U.S. Government.
 * Partial support came from the National Science Foundation.
 * <P>
 * It may be used to develop commercial software, but the Hv
 * source code and/or compiled modules/libraries may not be sold.
 * <P>
 * Questions or comments should be directed
 * to <a href="mailto:heddle@cebaf.gov">heddle@cebaf.gov</a> <HR>
 */

extern void           Hv_W32DrawWorldImageItem(Hv_Item    Item,
					   Hv_Region   region);



extern Hv_Pixmap       Hv_W32DrawItemOnPixmap(Hv_Item   Item,
					   Hv_Region region);

extern void            Hv_W32SaveViewBackground(Hv_View   View,
					     Hv_Region  region);


extern Hv_Pixmap       Hv_W32SaveUnder(short x,
				                    short y,
				                    unsigned int  w,
				                    unsigned int  h);

extern void            Hv_W32RestoreSaveUnder(Hv_Pixmap pmap,
					                       short x,
					                       short y,
					                       unsigned int  w,
					                       unsigned int  h);

extern void            Hv_W32UpdateItemOffScreen(Hv_Item Item,
					                          Hv_Region region);

extern void            Hv_W32OffScreenViewControlUpdate(Hv_View    View,
						                             Hv_Region  region);

extern void            Hv_W32RestoreViewBackground(Hv_View  View);

extern void            Hv_W32OffScreenViewUpdate(Hv_View    View,
					      Hv_Region  region);

extern void           Hv_W32ScrollView(Hv_View   View,
			                           int     ScrollPart,
			                           Hv_Region    region,
			                           Hv_Point   StartPP,
			                           Hv_Point   StopPP);

extern void            Hv_W32Go(void);

extern void            Hv_W32ProcessEvent(int mask);

extern void            Hv_W32AddEventHandler(Hv_Widget  w,
					     int             mask,
					     Hv_EventHandler handler,
					     Hv_Pointer      data);

extern void            Hv_W32VaSetValues(Hv_Widget w,
					 ...);

extern void            Hv_W32VaGetValues(Hv_Widget w,
					 ...);

extern void            Hv_W32RemoveTimeOut(Hv_IntervalId id);

extern Hv_IntervalId   Hv_W32AddTimeOut(unsigned long          interval,
					Hv_TimerCallbackProc   callback,
					Hv_Pointer             data);

extern Boolean         Hv_W32CheckMaskEvent(int         mask,
					    Hv_XEvent  *event);

extern int             Hv_W32Initialize(unsigned int,
					char **,
					char *);
     
extern  void           Hv_W32SystemSpecificInit();

extern  void           Hv_W32InitWindow(void);

extern  void           Hv_W32CreateAppContext();

extern  void           Hv_W32CreateGraphicsContext(void);

extern  void           Hv_W32InitCursors();

extern void            Hv_W32InitPatterns(void);

extern  void           Hv_W32InitGraphics(void);

extern  void           Hv_W32InitColors(void);

extern  void           Hv_W32InitCanvas(void);

extern  void           Hv_W32InitGlobalWidgets(void);

extern Hv_Widget       Hv_W32CreateMainMenuBar(Hv_Widget parent);

extern Hv_Widget       Hv_W32CreatePopup(Hv_Widget  parent);

extern Hv_Widget       Hv_W32CreateSubmenu(Hv_Widget  parent);

extern void            Hv_W32Popup1Callback(Hv_Widget          widget,
					    Hv_Widget          popup,
					    Hv_CanvasCallback *cbs);

extern void            Hv_W32Popup2Callback(Hv_Widget          widget,
					    Hv_Widget          popup,
					    Hv_CanvasCallback *cbs);

extern void            Hv_W32CreateUserMenus(void);

extern void            Hv_W32SetMenuItemString(Hv_Widget menuitem,
					      char      *str,
					      short      font);

extern Hv_Widget       Hv_W32CreateHelpPullDown(void);

extern Hv_Widget       Hv_W32CreatePullDown(char *title);

extern Hv_Widget       Hv_W32AddMenuSeparator(Hv_Widget	menu);

extern Hv_Widget       Hv_W32AddSubMenuItem(char *label,
					   Hv_Widget    menu,
					   Hv_Widget	submenu);

extern Hv_Widget       Hv_W32AddMenuToggle(char *label,
					  Hv_Widget	   menu,
					  long             ID,
					  Hv_CallbackProc  callback,
					  Boolean	   state,
					  unsigned char    btype,
					  int	           acctype,
					  char	           accchr);

extern Hv_Widget       Hv_W32AddMenuItem(char *label,
					 Hv_Widget       menu,
					 long            ID,
					 Hv_CallbackProc callback,
					 int	         acctype,
					 char	         accchr);		

extern  void           Hv_W32GetCurrentPointerLocation(short *x,
						      short *y);

extern Hv_CompoundString Hv_W32CreateSimpleCompoundString(char  *str);

extern Hv_CompoundString Hv_W32CreateCompoundString(char  *str,
						   short  font);

extern Hv_CompoundString Hv_W32CreateAndSizeCompoundString(char    *text,
							   short   font,
							   short   *w,
							   short   *h);

extern void            Hv_W32CompoundStringFree(Hv_CompoundString cs);

extern void            Hv_W32CompoundStringDrawImage(Hv_CompoundString cs,
						     short             x,
						     short             y,
						     short             sw);

extern short           Hv_W32CompoundStringWidth(Hv_CompoundString cs);

extern short           Hv_W32CompoundStringBaseline(Hv_CompoundString cs);

extern short           Hv_W32CompoundStringHeight(Hv_CompoundString cs);

extern void            Hv_W32CompoundStringDraw(Hv_CompoundString cs,
						short             x,
						short             y,
						short             sw);

extern void            Hv_W32UnionRectangleWithRegion(Hv_Rectangle  *xr,
						      Hv_Region     region);

extern Hv_Region       Hv_W32IntersectRegion(Hv_Region reg1,
					     Hv_Region reg2);

extern Hv_Region       Hv_W32SubtractRegion(Hv_Region reg1,
					    Hv_Region reg2);

extern Hv_Region       Hv_W32UnionRegion(Hv_Region reg1,
					 Hv_Region reg2);

extern void            Hv_W32ShrinkRegion(Hv_Region region,
					  short   h,
					  short   v);

extern void            Hv_W32OffsetRegion(Hv_Region region,
					  short   h,
					  short   v);

extern Hv_Region       Hv_W32CreateRegionFromPolygon(Hv_Point  *poly,
						     short      n);

extern void            Hv_W32DestroyRegion(Hv_Region  region);

extern Boolean         Hv_W32EmptyRegion(Hv_Region region);

extern Hv_Region       Hv_W32CreateRegion(void);

extern Boolean         Hv_W32PointInRegion(Hv_Point,
					   Hv_Region);

extern Boolean         Hv_W32RectInRegion(Hv_Rect *,
					  Hv_Region);


extern void            Hv_W32SetFunction(int funcbit);

extern void            Hv_W32GetGCValues(int *gcv);

extern void            Hv_W32ClipBox(Hv_Region  region,
				     Hv_Rect   *rect);

extern void            Hv_W32CheckForExposures(int);

extern void            Hv_W32SetPattern(int pattern,
				       short color);

extern void            Hv_W32SetLineStyle(int,
				       int);

extern void 	       Hv_W32EraseRectangle(short,
					 short,
					 short,
					 short);


extern void            Hv_W32ReTileMainWindow(void);

extern void            Hv_W32TileMainWindow(Hv_Pixmap);

extern void            Hv_W32SetBackground(unsigned long);

extern void            Hv_W32SetForeground(unsigned long);

extern void            Hv_W32SetFillStyle(int);

extern void            Hv_W32SetLineWidth(int);

extern void            Hv_W32SetClippingRegion(Hv_Region);

extern void            Hv_W32FreePixmap(Hv_Pixmap pmap);

extern Hv_Pixmap       Hv_W32CreatePixmap(unsigned int  w,
					  unsigned int  h);

extern void            Hv_W32ClearArea(short x,
							   		   short y,
									   short w,
									   short h);

extern void            Hv_W32CopyArea(Hv_Window src,
				      Hv_Window dest,
				      int       src_x,
				      int       src_y,
				      unsigned  int w,
				      unsigned  int h,
				      int       dest_x,
				      int       dest_y);

extern void            Hv_W32DestroyImage(Hv_Image *image);

extern void            Hv_W32PutImage(Hv_Window window,
				      Hv_Image *image,
				      int       src_x,
				      int       src_y,
				      int       dest_x,
				      int       dest_y,
				      unsigned  int w,
				      unsigned  int h);


extern void            Hv_W32DrawTextItemVertical(Hv_Item    Item,
						  Hv_Region  region);

extern void            Hv_W32DrawPoint(short,
				       short,
				       short);

extern void            Hv_W32DrawPoints(Hv_Point *,
					int,
					short);

extern void	       Hv_W32FrameArc(short   x,
				      short     y,
				      short     width,
				      short     height,
				      int       ang1,
				      int       ang2,
				      short     color);

extern void            Hv_W32FillArc(short   x,
				     short   y,
				     short   width,
				     short   height,
				     int     ang1,
				     int     ang2,
				     Boolean frame,
				     short   color,
				     short   framec);

extern void            Hv_W32DrawLines(Hv_Point *xp,
				       short     np,
				       short     color);

extern void            Hv_W32FillPolygon(Hv_Point *xp,
					 short     np,
					 Boolean   frame,
					 short     fillcolor,
					 short     framecolor);

extern void            Hv_W32DrawSegments(Hv_Segment *xseg,
					  short nseg,
					  short color);

extern void            Hv_W32FrameRectangle(short,
					    short,
					    short,
					    short,
					    short);

extern void 	       Hv_W32FillRectangle(short,
					   short,
					   short,
					   short,
					   short);

extern void 	       Hv_W32FillRectangles(Hv_Rectangle *rects,
					    int           np,
					    short         color);

extern void            Hv_W32FillRect(Hv_Rect *rect,
				      short    color);

extern void            Hv_W32FrameRect(Hv_Rect *rect,
				       short    color);

extern void	       Hv_W32DrawLine(short x1,
				      short y1,
				      short x2,
				      short y2,
				      short color);

extern void            Hv_W32SimpleDrawString(short,
					      short,
					      Hv_String *);

extern void            Hv_W32DrawString(short       x,
					short       y,
					Hv_String  *str,
					Hv_Region   region);


extern short          Hv_W32GetXEventX(Hv_XEvent *event);

extern short          Hv_W32GetXEventY(Hv_XEvent *event);

extern short          Hv_W32GetXEventButton(Hv_XEvent *event);


extern void           Hv_W32AddModifier(Hv_XEvent  *event,
					int         modbit);


extern Boolean        Hv_W32Shifted(Hv_XEvent  *event);

extern Boolean        Hv_W32AltPressed(Hv_XEvent  *event);

extern Boolean        Hv_W32ControlPressed(Hv_XEvent  *event);

extern void           Hv_W32Pause(int       interval,
				  Hv_Region region);

extern void           Hv_W32SysBeep(void);

extern void           Hv_W32Flush(void);

extern void           Hv_W32Sync(void);

extern void           Hv_W32SetCursor(Hv_Cursor  curs);

extern void	      Hv_W32SetString(Hv_Widget,
				      char *);

extern char          *Hv_W32GetString(Hv_Widget);

extern Hv_Widget      Hv_W32Parent(Hv_Widget w);

extern void           Hv_W32RemoveAllCallbacks(Hv_Widget w,
					       char *name);

extern Hv_Widget      Hv_W32VaCreateWidget(char            *name,
					   Hv_WidgetClass   widgetclass,
					   Hv_Widget        parent,
					   ...);

extern Hv_Widget      Hv_W32VaCreateManagedWidget(char            *name,
						  Hv_WidgetClass   widgetclass,
						  Hv_Widget        parent,
						  ...);


extern void           Hv_W32ManageChild(Hv_Widget w);

extern void           Hv_W32UnmanageChild(Hv_Widget w);

extern void           Hv_W32AddCallback(Hv_Widget       w,
					char           *cbname,
					Hv_CallbackProc cbproc,
					Hv_Pointer      data);

extern void           Hv_W32DestroyWidget(Hv_Widget  w);

extern void           Hv_W32SetSensitivity(Hv_Widget  w,
					   Boolean   val);
extern Boolean        Hv_W32IsSensitive(Hv_Widget  w);

extern void           Hv_W32SetWidgetBackgroundColor(Hv_Widget   w,
						     short     color);

extern void           Hv_W32SetWidgetForegroundColor(Hv_Widget   w,
						     short     color);

extern int            Hv_W32ClickType(Hv_Item Item,
				      short   button);

extern void           Hv_W32Println(char   *fmt,
				    va_list args);

extern void            Hv_W32MakeColorMap(void);

extern unsigned long   Hv_W32GetForeground(void);

extern unsigned long   Hv_W32GetBackground(void);


extern unsigned long   Hv_W32NearestColor(unsigned short r,
					  unsigned short g,
					  unsigned short b);


extern void            Hv_W32DarkenColor(short color,
					 unsigned short del);

extern void            Hv_W32BrightenColor(short color,
					   unsigned short del);

extern void            Hv_W32AdjustColor(short color,
					 short r,
					 short g,
					 short b);

extern void            Hv_W32ChangeColor(short color,
					 unsigned short r,
					 unsigned short g,
					 unsigned short b);

extern void            Hv_W32CopyColor(short dcolor,
				       short scolor);

extern void            Hv_W32GetRGBFromColor(short color,
										     unsigned short *r,
										     unsigned short *g,
										     unsigned short *b);

extern void            Hv_W32QueryColor(Hv_Display   *dpy,
					Hv_ColorMap  cmap,
					Hv_Color     *xcolor);


extern void            Hv_W32DoubleFileSelect(char *title,
					      char *prompt1 ,
					      char *prompt2 ,
					      char *mask1 , 
					      char *mask2 , 
					      char **fname1 , 
					      char **fname2);

extern char           *Hv_W32FileSelect(char *,
					char *,
					char *);

extern char           *Hv_W32DirectorySelect(char *,
					     char *);

extern char           *Hv_W32GenericFileSelect(char *,
					       char *,
					       unsigned char,
					       char *,
					       Hv_Widget);

extern void            Hv_W32InitFonts(void);

extern void            Hv_W32AddNewFont(short);

extern void            Hv_W32TextEntryItemInitialize(Hv_Item      Item,
						    Hv_AttributeArray attributes);

extern void            Hv_W32TextEntrySingleClick(Hv_Event hevent);

extern void            Hv_W32Information(char *);

extern void            Hv_W32Warning(char *);

extern int             Hv_W32Question(char *question);

extern Hv_Widget       Hv_W32CreateModalDialog(char *,
					       int);

extern Hv_Widget       Hv_W32CreateModelessDialog(char *,
						  int);

extern int             Hv_W32EditPattern(short *,
					 char  *);

extern void            Hv_W32ChangeLabel(Hv_Widget,
				      char *,
				      short);

extern Hv_Widget       Hv_W32CreateThinOptionMenu(Hv_Widget      rc,
						  short          font,
						  char          *title,
						  int            numitems,
						  char          *items[],
						  Hv_FunctionPtr CB,
						  short          margin);

extern Hv_Widget       Hv_W32GetOwnerDialog(Hv_Widget);

extern void            Hv_W32CloseOutCallback(Hv_Widget,
					      int,HWND hwnd);

extern Hv_Widget       Hv_W32ActionAreaDialogItem(Hv_Widget         parent,
						  Hv_AttributeArray attributes);

extern Hv_Widget       Hv_W32ButtonDialogItem(Hv_Widget         parent,
					      Hv_AttributeArray attributes);


extern Hv_Widget       Hv_W32CloseOutDialogItem(Hv_Widget         parent,
						Hv_AttributeArray attributes);

extern Hv_Widget       Hv_W32ColorSelectorDialogItem(Hv_Widget         parent,
						     Hv_AttributeArray attributes);

extern Hv_Widget       Hv_W32FileSelectorDialogItem(Hv_Widget         parent,
						    Hv_AttributeArray attributes);

extern Hv_Widget       Hv_W32LabelDialogItem(Hv_Widget         parent,
					     Hv_AttributeArray attributes);

extern Hv_Widget       Hv_W32ListDialogItem(Hv_Widget         parent,
					    Hv_AttributeArray attributes);

extern Hv_Widget       Hv_W32RadioDialogItem(Hv_Widget         parent,
					     Hv_AttributeArray attributes);

extern Hv_Widget       Hv_W32ScaleDialogItem(Hv_Widget         parent,
					     Hv_AttributeArray attributes);

extern Hv_Widget       Hv_W32SeparatorDialogItem(Hv_Widget         parent,
						 Hv_AttributeArray attributes);

extern Hv_Widget       Hv_W32TextDialogItem(Hv_Widget         parent,
					    Hv_AttributeArray attributes);

extern Hv_Widget       Hv_W32ToggleDialogItem(Hv_Widget         parent,
					      Hv_AttributeArray attributes);

extern Hv_Widget       Hv_W32RowColDialogItem(Hv_Widget         parent,
					      Hv_AttributeArray attributes);

extern Hv_Widget       Hv_W32ScrolledTextDialogItem(Hv_Widget         parent,
						    Hv_AttributeArray attributes);

extern int             Hv_W32GetScaleValue(Hv_Widget   w);

extern void            Hv_W32SetScaleValue(Hv_Widget   w,
					   int      val);

extern int             Hv_W32ListItemPos(Hv_Widget list,
					 char     *str);

extern Boolean         Hv_W32CheckListItem(Hv_Widget list,
					   char *str,
					   int  **poslist,
					   int  *num);

extern void            Hv_W32PosReplaceListItem(Hv_Widget,
						int,
						char *);

extern void            Hv_W32DeleteAllListItems(Hv_Widget);

extern void            Hv_W32DeselectAllListItems(Hv_Widget);

extern void            Hv_W32AddListItem(Hv_Widget list,
					 char *str,
					 Boolean selected);

extern void            Hv_W32PosSelectListItem(Hv_Widget,
					       int);

extern void            Hv_W32NewPosSelectListItem(Hv_Widget,
						  int,
						  Boolean);

extern void            Hv_W32SelectListItem(Hv_Widget,
					   char *);

extern int             Hv_W32GetListSelectedPos(Hv_Widget);

extern int            *Hv_W32GetAllListSelectedPos(Hv_Widget);

extern void            Hv_W32ChangeLabelPattern(Hv_Widget,
						short);


extern void            Hv_W32AppendLineToScrolledText(Hv_Widget w,
						      char      *line);

extern void            Hv_W32DeleteLinesFromTop(Hv_Widget w,
						int       nlines);

extern void            Hv_W32JumpToBottomOfScrolledText(Hv_Widget        w,
							Hv_TextPosition *pos);

extern void            Hv_W32ClearScrolledText(Hv_Widget,
					       Hv_TextPosition *);

extern void            Hv_W32AddLineToScrolledText(Hv_Widget,
						   Hv_TextPosition *,
						   char *);

extern void            Hv_W32DeleteScrolledTextLines(Hv_Widget,
						     int,
						     Hv_TextPosition *);
extern void            Hv_W32PopupDialog(Hv_Widget dialog);

extern void            Hv_W32PopdownDialog(Hv_Widget dialog);

extern void            Hv_W32DoHelp();

extern void            Hv_W32OverrideTranslations(Hv_Widget w,
						  char     *table);

extern void            Hv_W32SetupGLWidget(Hv_View View,char *title);

extern void            Hv_W32Set3DFrame(Hv_View View);

extern void            Hv_W32TextInsert(Hv_Widget          w,
					Hv_TextPosition    pos,
					char              *line);

extern void            Hv_W32SetInsertionPosition(Hv_Widget       w,
					       Hv_TextPosition pos);

extern void            Hv_W32ShowPosition(Hv_Widget       w,
					  Hv_TextPosition pos);

extern Hv_TextPosition  Hv_W32GetInsertionPosition(Hv_Widget       w);

extern void             Hv_W32UpdateWidget(Hv_Widget w);

extern Hv_TextPosition  Hv_W32GetLastPosition(Hv_Widget w);

extern Boolean          Hv_W32GetToggleButton(Hv_Widget);

extern void          Hv_W32SetToggleButton(Hv_Widget btm,
											  Boolean State);

extern Hv_Image        *Hv_W32Pic8ToImage(byte *pic8,
					  int wide,
					  int high,
					  unsigned long **mycolors,
					  byte *rmap,
					  byte *gmap,
					  byte *bmap);

extern void            Hv_W32GrabPointer();

extern void            Hv_W32UngrabPointer();

extern Boolean         Hv_W32GetNextFile(DIR *,
				      char *,
				      char *,
				      FILE **,
				      char *);

extern int             Hv_W32AlphaSortDirectory(char *dirname,
					     char *extension,
					     char ***names,
					     Boolean prepend,
					     Boolean remext);

extern DIR            *Hv_W32OpenDirectory(char *);
extern int 	       Hv_W32CloseDirectory(DIR *);
extern int             Hv_W32DoDialog(Hv_Widget,
				   Hv_Widget);

extern int Hv_XModifiers(int w32param);

extern void HandlePopupMenus(int x,
							 int y,
							 POINT *clp,
							  HWND Hv_mainWindow);

extern void Hv_GetClientPosition(POINT *pp);

extern void widgetDispatcher(Hv_Widget w32data,
							 int userData,
							 HWND hwnd,
							 UINT notifyCode);

extern  HRESULT ANSIToUnicode(LPCSTR pcszANSI, LPWSTR *ppwszUnicode);

extern HBITMAP Hv_CreateBitMapFromPattern(short pat,
										  short color);

extern Hv_Widget createNewWidget(short wtype);


#endif




