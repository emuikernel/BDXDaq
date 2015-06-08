#ifndef __HVXMGATEWAYH__
#define __HVXMGATEWAYH__

/**
 * <EM>Where all X/Motif specific commands are prototyped.</EM>
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

#ifndef WIN32

extern void           Hv_XMDrawWorldImageItem(Hv_Item    Item,
					   Hv_Region   region);



extern Hv_Pixmap       Hv_XMDrawItemOnPixmap(Hv_Item   Item,
					   Hv_Region region);

extern void            Hv_XMSaveViewBackground(Hv_View   View,
					     Hv_Region  region);


extern Hv_Pixmap       Hv_XMSaveUnder(short x,
				                    short y,
				                    unsigned int  w,
				                    unsigned int  h);

extern void            Hv_XMRestoreSaveUnder(Hv_Pixmap pmap,
					                       short x,
					                       short y,
					                       unsigned int  w,
					                       unsigned int  h);

extern void            Hv_XMUpdateItemOffScreen(Hv_Item Item,
					                          Hv_Region region);

extern void            Hv_XMOffScreenViewControlUpdate(Hv_View    View,
						                             Hv_Region  region);

extern void            Hv_XMRestoreViewBackground(Hv_View  View);


extern void            Hv_XMOffScreenViewUpdate(Hv_View    View,
					      Hv_Region  region);


extern void           Hv_XMScrollView(Hv_View   View,
			                          int     ScrollPart,
			                          Hv_Region    region,
			                          Hv_Point   StartPP,
			                          Hv_Point   StopPP);

extern void            Hv_XMGo(void);

extern void            Hv_XMProcessEvent(int mask);

extern void            Hv_XMAddEventHandler(Hv_Widget       w,
					    int             mask,
					    Hv_EventHandler handler,
					    Hv_Pointer      data);

extern void            Hv_XMVaSetValues(Hv_Widget w,
					...);

extern void            Hv_XMVaGetValues(Hv_Widget w,
					...);

extern void            Hv_XMRemoveTimeOut(Hv_IntervalId id);

extern Hv_IntervalId   Hv_XMAddTimeOut(unsigned long          interval,
				       Hv_TimerCallbackProc   callback,
				       Hv_Pointer             data);

extern Boolean         Hv_XMCheckMaskEvent(int         mask,
					   Hv_XEvent  *event);

extern int             Hv_XMInitialize(unsigned int,
				       char **,
				       char *);

extern  void           Hv_XMSystemSpecificInit();

extern  void           Hv_XMInitWindow(void);

extern  void           Hv_XMCreateAppContext();

extern  void           Hv_XMCreateGraphicsContext(void);

extern  void           Hv_XMInitCursors();

extern void            Hv_XMInitPatterns(void);

extern  void           Hv_XMInitGraphics(void);

extern  void           Hv_XMInitColors(void);

extern  void           Hv_XMInitCanvas(void);

extern  void           Hv_XMInitGlobalWidgets(void);

extern Hv_Widget       Hv_XMCreateMainMenuBar(Hv_Widget parent);

extern Hv_Widget       Hv_XMCreatePopup(Hv_Widget  parent);

extern Hv_Widget       Hv_XMCreateSubmenu(Hv_Widget  parent);

extern void            Hv_XMPopup1Callback(Hv_Widget          widget,
					   Hv_Widget          popup,
					   Hv_CanvasCallback *cbs);

extern void            Hv_XMPopup2Callback(Hv_Widget          widget,
					   Hv_Widget          popup,
					   Hv_CanvasCallback *cbs);

extern void            Hv_XMCreateUserMenus(void);

extern void            Hv_XMSetMenuItemString(Hv_Widget menuitem,
					      char     *str,
					      short     font);

extern Hv_Widget       Hv_XMCreateHelpPullDown(void);

extern Hv_Widget       Hv_XMCreatePullDown(char *title);

extern Hv_Widget       Hv_XMAddMenuSeparator(Hv_Widget	menu);

extern Hv_Widget       Hv_XMAddSubMenuItem(char *label,
					   Hv_Widget    menu,
					   Hv_Widget	submenu);

extern Hv_Widget       Hv_XMAddMenuToggle(String	   label,
					  Hv_Widget	   menu,
					  long             ID,
					  Hv_CallbackProc  callback,
					  Boolean	   state,
					  unsigned char    btype,
					  int	           acctype,
					  char	           accchr);

extern Hv_Widget       Hv_XMAddMenuItem(char *label,
					Hv_Widget	menu,
					long            ID,
					Hv_CallbackProc callback,
					int	        acctype,
					char	        accchr);		

extern  void           Hv_XMGetCurrentPointerLocation(short *x,
						      short *y);

extern Hv_CompoundString Hv_XMCreateSimpleCompoundString(char  *str);

extern Hv_CompoundString Hv_XMCreateCompoundString(char  *str,
						   short  font);

extern Hv_CompoundString Hv_XMCreateAndSizeCompoundString(char    *text,
							  short   font,
							  short   *w,
							  short   *h);

extern void            Hv_XMCompoundStringFree(Hv_CompoundString cs);

extern short           Hv_XMCompoundStringWidth(Hv_CompoundString cs);

extern short           Hv_XMCompoundStringBaseline(Hv_CompoundString cs);

extern short           Hv_XMCompoundStringHeight(Hv_CompoundString cs);

extern void            Hv_XMCompoundStringDrawImage(Hv_CompoundString cs,
						    short             x,
						    short             y,
						    short             sw);

extern void            Hv_XMCompoundStringDraw(Hv_CompoundString cs,
					       short             x,
					       short             y,
					       short             sw);

extern void            Hv_XMUnionRectangleWithRegion(Hv_Rectangle  *xr,
						     Hv_Region     region);

extern Hv_Region       Hv_XMIntersectRegion(Hv_Region reg1,
					    Hv_Region reg2);

extern Hv_Region       Hv_XMSubtractRegion(Hv_Region reg1,
					   Hv_Region reg2);

extern Hv_Region       Hv_XMUnionRegion(Hv_Region reg1,
					Hv_Region reg2);

extern void            Hv_XMShrinkRegion(Hv_Region region,
					 short   h,
					 short   v);

extern void            Hv_XMOffsetRegion(Hv_Region region,
					 short   dh,
					 short   dv);

extern Hv_Region       Hv_XMCreateRegionFromPolygon(Hv_Point  *poly,
						    short      n);

extern Boolean         Hv_XMEmptyRegion(Hv_Region region);

extern Hv_Region       Hv_XMCreateRegion(void);

extern void            Hv_XMDestroyRegion(Hv_Region  region);

extern Boolean         Hv_XMPointInRegion(Hv_Point,
					  Hv_Region);

extern Boolean         Hv_XMRectInRegion(Hv_Rect *,
					 Hv_Region);

extern void            Hv_XMSetFunction(int funcbit);

extern void            Hv_XMGetGCValues(int *gcv);

extern void            Hv_XMClipBox(Hv_Region  region,
				    Hv_Rect   *rect);

extern void            Hv_XMCheckForExposures(int);

extern void            Hv_XMSetPattern(int pattern,
				       short color);

extern void            Hv_XMSetLineStyle(int,
				       int);


extern void 	       Hv_XMEraseRectangle(short,
					 short,
					 short,
					 short);


extern void            Hv_XMReTileMainWindow(void);

extern void            Hv_XMTileMainWindow(Hv_Pixmap);

extern void            Hv_XMSetBackground(unsigned long);

extern void            Hv_XMSetForeground(unsigned long);

extern void            Hv_XMSetFillStyle(int);

extern void            Hv_XMSetLineWidth(int);

extern void            Hv_XMSetClippingRegion(Hv_Region);

extern void            Hv_XMFreePixmap(Hv_Pixmap pmap);

extern Hv_Pixmap       Hv_XMCreatePixmap(unsigned int  w,
					 unsigned int  h);

extern void            Hv_XMClearArea(short x,
									  short y,
									  short w,
									  short h);

extern void            Hv_XMCopyArea(Hv_Window src,
				     Hv_Window dest,
				     int       src_x,
				     int       src_y,
				     unsigned  int w,
				     unsigned  int h,
				     int       dest_x,
				     int       dest_y);

extern void            Hv_XMDestroyImage(Hv_Image *image);

extern void            Hv_XMPutImage(Hv_Window window,
				     Hv_Image *image,
				     int       src_x,
				     int       src_y,
				     int       dest_x,
				     int       dest_y,
				     unsigned  int w,
				     unsigned  int h);

extern void            Hv_XMDrawTextItemVertical(Hv_Item    Item,
						 Hv_Region  region);

extern void            Hv_XMDrawPoint(short,
				      short,
				      short);

extern void            Hv_XMDrawPoints(Hv_Point *,
				       int,
				       short);

extern void	       Hv_XMFrameArc(short   x,
				     short     y,
				     short     width,
				     short     height,
				     int       ang1,
				     int       ang2,
				     short     color);

extern void            Hv_XMFillArc(short   x,
				    short   y,
				    short   width,
				    short   height,
				    int     ang1,
				    int     ang2,
				    Boolean frame,
				    short   color,
				    short   framec);

extern void            Hv_XMDrawLines(Hv_Point *xp,
				      short     np,
				      short     color);

extern void            Hv_XMFillPolygon(Hv_Point *xp,
					short     np,
					Boolean   frame,
					short     fillcolor,
					short     framecolor);

extern void            Hv_XMDrawSegments(Hv_Segment *xseg,
					 short nseg,
					 short color);

extern void            Hv_XMFrameRectangle(short,
					   short,
					   short,
					   short,
					   short);

extern void 	       Hv_XMFillRectangle(short,
					  short,
					  short,
					  short,
					  short);

extern void 	       Hv_XMFillRectangles(Hv_Rectangle *rects,
					   int           np,
					   short         color);

extern void            Hv_XMFillRect(Hv_Rect *rect,
				   short    color);

extern void            Hv_XMFrameRect(Hv_Rect *rect,
				    short    color);

extern void	       Hv_XMDrawLine(short x1,
				   short y1,
				   short x2,
				   short y2,
				   short color);

extern void            Hv_XMSimpleDrawString(short,
					     short,
					     Hv_String *);

extern void            Hv_XMDrawString(short       x,
				     short       y,
				     Hv_String  *str,
				     Hv_Region   region);

extern short          Hv_XMGetXEventX(Hv_XEvent *event);

extern short          Hv_XMGetXEventY(Hv_XEvent *event);

extern short          Hv_XMGetXEventButton(Hv_XEvent *event);

extern void           Hv_XMAddModifier(Hv_XEvent  *event,
				       int         modbit);

extern Boolean        Hv_XMShifted(Hv_XEvent  *event);

extern Boolean        Hv_XMAltPressed(Hv_XEvent  *event);

extern Boolean        Hv_XMControlPressed(Hv_XEvent  *event);

extern void           Hv_XMPause(int       interval,
				 Hv_Region region);

extern void           Hv_XMSysBeep(void);


extern void           Hv_XMFlush(void);

extern void           Hv_XMSync(void);

extern void           Hv_XMSetCursor(Hv_Cursor  curs);

extern void	      Hv_XMSetString(Hv_Widget,
				     char *);

extern char          *Hv_XMGetString(Hv_Widget);

extern Hv_Widget      Hv_XMParent(Hv_Widget w);

extern void           Hv_XMRemoveAllCallbacks(Hv_Widget w,
					      char *name);

extern Hv_Widget      Hv_XMVaCreateWidget(char            *name,
					  Hv_WidgetClass   widgetclass,
					  Hv_Widget        parent,
					  ...);

extern Hv_Widget      Hv_XMVaCreateManagedWidget(char            *name,
						 Hv_WidgetClass   widgetclass,
						 Hv_Widget        parent,
						 ...);

extern void           Hv_XMManageChild(Hv_Widget w);

extern void           Hv_XMUnmanageChild(Hv_Widget w);

extern void           Hv_XMAddCallback(Hv_Widget       w,
				       char           *cbname,
				       Hv_CallbackProc cbproc,
				       Hv_Pointer      data);

extern void           Hv_XMDestroyWidget(Hv_Widget  w);

extern void           Hv_XMSetSensitivity(Hv_Widget  w,
					  Boolean   val);

extern Boolean        Hv_XMIsSensitive(Hv_Widget  w);

extern void           Hv_XMSetWidgetBackgroundColor(Hv_Widget   w,
						    short     color);

extern void           Hv_XMSetWidgetForegroundColor(Hv_Widget   w,
						    short     color);
extern int            Hv_XMClickType(Hv_Item Item,
				     short   button);

extern void           Hv_XMPrintln(char   *fmt,
				   va_list args);

extern void            Hv_XMMakeColorMap(void);

extern unsigned long   Hv_XMGetForeground(void);

extern unsigned long   Hv_XMGetBackground(void);


extern unsigned long   Hv_XMNearestColor(unsigned short r,
					 unsigned short g,
					 unsigned short b);


extern void            Hv_XMDarkenColor(short color,
					unsigned short del);

extern void            Hv_XMBrightenColor(short color,
					  unsigned short del);

extern void            Hv_XMAdjustColor(short color,
					short r,
					short g,
					short b);

extern void            Hv_XMChangeColor(short color,
					unsigned short r,
					 unsigned short g,
					 unsigned short b);

extern void            Hv_XMCopyColor(short dcolor,
				      short scolor);

extern void            Hv_XMGetRGBFromColor(short color,
										    unsigned short *r,
										    unsigned short *g,
										    unsigned short *b);

extern void            Hv_XMQueryColor(Hv_Display   *dpy,
				       Hv_ColorMap  cmap,
				       Hv_Color     *xcolor);

extern void            Hv_XMDoubleFileSelect(char *title,
					     char *prompt1 ,
					     char *prompt2 ,
					     char *mask1 , 
					     char *mask2 , 
					     char **fname1 , 
					     char **fname2);

extern char           *Hv_XMFileSelect(char *,
				       char *,
				       char *);

extern char           *Hv_XMDirectorySelect(char *,
					    char *);

extern char           *Hv_XMGenericFileSelect(char *,
					      char *,
					      unsigned char,
					      char *,
					      Hv_Widget);

extern void            Hv_XMInitFonts(void);

extern void            Hv_XMAddNewFont(short);

extern void            Hv_XMTextEntryItemInitialize(Hv_Item      Item,
						    Hv_AttributeArray attributes);

extern void            Hv_XMTextEntrySingleClick(Hv_Event hevent);

extern void            Hv_XMInformation(char *);

extern void            Hv_XMWarning(char *);

extern int             Hv_XMQuestion(char *question);

extern Hv_Widget       Hv_XMCreateModalDialog(char *,
					      int);

extern Hv_Widget       Hv_XMCreateModelessDialog(char *,
						 int);

extern int             Hv_XMEditPattern(short *,
					char  *);

extern void            Hv_XMChangeLabel(Hv_Widget,
				      char *,
				      short);

extern Hv_Widget       Hv_XMCreateThinOptionMenu(Hv_Widget      rc,
						 short          font,
						 char          *title,
						 int            numitems,
						 char          *items[],
						 Hv_FunctionPtr CB,
						 short          margin);

extern Hv_Widget       Hv_XMGetOwnerDialog(Hv_Widget);

extern void            Hv_XMCloseOutCallback(Hv_Widget,
					     int);


extern Hv_Widget       Hv_XMActionAreaDialogItem(Hv_Widget         parent,
						 Hv_AttributeArray attributes);

extern Hv_Widget       Hv_XMButtonDialogItem(Hv_Widget         parent,
					     Hv_AttributeArray attributes);


extern Hv_Widget       Hv_XMCloseOutDialogItem(Hv_Widget         parent,
					       Hv_AttributeArray attributes);

extern Hv_Widget       Hv_XMColorSelectorDialogItem(Hv_Widget         parent,
						    Hv_AttributeArray attributes);

extern Hv_Widget       Hv_XMFileSelectorDialogItem(Hv_Widget         parent,
						   Hv_AttributeArray attributes);

extern Hv_Widget       Hv_XMLabelDialogItem(Hv_Widget         parent,
					    Hv_AttributeArray attributes);

extern Hv_Widget       Hv_XMListDialogItem(Hv_Widget         parent,
					   Hv_AttributeArray attributes);

extern Hv_Widget       Hv_XMRadioDialogItem(Hv_Widget         parent,
					    Hv_AttributeArray attributes);

extern Hv_Widget       Hv_XMScaleDialogItem(Hv_Widget         parent,
					    Hv_AttributeArray attributes);

extern Hv_Widget       Hv_XMSeparatorDialogItem(Hv_Widget         parent,
						Hv_AttributeArray attributes);

extern Hv_Widget       Hv_XMTextDialogItem(Hv_Widget         parent,
					   Hv_AttributeArray attributes);

extern Hv_Widget       Hv_XMToggleDialogItem(Hv_Widget         parent,
					     Hv_AttributeArray attributes);

extern Hv_Widget       Hv_XMRowColDialogItem(Hv_Widget         parent,
					     Hv_AttributeArray attributes);

extern Hv_Widget       Hv_XMScrolledTextDialogItem(Hv_Widget         parent,
						   Hv_AttributeArray attributes);

extern int             Hv_XMGetScaleValue(Hv_Widget   w);

extern void            Hv_XMSetScaleValue(Hv_Widget   w,
					  int      val);

extern int             Hv_XMListItemPos(Hv_Widget list,
					char     *str);

extern Boolean         Hv_XMCheckListItem(Hv_Widget list,
					  char *str,
					  int  **poslist,
					  int  *num);

extern void            Hv_XMPosReplaceListItem(Hv_Widget,
					       int,
					       char *);

extern void            Hv_XMDeleteAllListItems(Hv_Widget);

extern void            Hv_XMDeselectAllListItems(Hv_Widget);

extern void            Hv_XMAddListItem(Hv_Widget list,
					char *str,
					Boolean selected);

extern void            Hv_XMPosSelectListItem(Hv_Widget,
					      int);

extern void            Hv_XMNewPosSelectListItem(Hv_Widget,
						 int,
						 Boolean);

extern void            Hv_XMListSelectItem(Hv_Widget,
					   char *,
					   Boolean );

extern int             Hv_XMGetListSelectedPos(Hv_Widget);

extern int            *Hv_XMGetAllListSelectedPos(Hv_Widget);

extern void            Hv_XMChangeLabelPattern(Hv_Widget,
					       short);

extern void            Hv_XMAppendLineToScrolledText(Hv_Widget w,
						     char      *line);

extern void            Hv_XMDeleteLinesFromTop(Hv_Widget w,
					       int       nlines);

extern void            Hv_XMJumpToBottomOfScrolledText(Hv_Widget        w,
						       Hv_TextPosition *pos);

extern void            Hv_XMClearScrolledText(Hv_Widget,
					      Hv_TextPosition *);

extern void            Hv_XMAddLineToScrolledText(Hv_Widget,
						  Hv_TextPosition *,
						  char *);

extern void            Hv_XMDeleteScrolledTextLines(Hv_Widget,
						    int,
						    Hv_TextPosition *);
extern void            Hv_XMPopupDialog(Hv_Widget dialog);

extern void            Hv_XMPopdownDialog(Hv_Widget dialog);

extern void            Hv_XMDoHelp();

extern void            Hv_XMOverrideTranslations(Hv_Widget w,
						 char     *table);

extern void            Hv_XMSetupGLWidget(Hv_View View,char *title);

extern void            Hv_XMSet3DFrame(Hv_View View);

extern void            Hv_XMTextInsert(Hv_Widget          w,
				       Hv_TextPosition    pos,
				       char              *line);

extern void            Hv_XMSetInsertionPosition(Hv_Widget       w,
					       Hv_TextPosition pos);

extern void            Hv_XMShowPosition(Hv_Widget       w,
					 Hv_TextPosition pos);

extern Hv_TextPosition  Hv_XMGetInsertionPosition(Hv_Widget       w);

extern void             Hv_XMUpdateWidget(Hv_Widget w);

extern Hv_TextPosition  Hv_XMGetLastPosition(Hv_Widget  w);

extern Boolean          Hv_XMGetToggleButton(Hv_Widget);

extern void             Hv_XMSetToggleButton(Hv_Widget,
					     Boolean);

extern Hv_Image        *Hv_XMPic8ToImage(byte *pic8,
					 int wide,
					 int high,
					 unsigned long **mycolors,
					 byte *rmap,
					 byte *gmap,
					 byte *bmap);

extern void            Hv_XMGrabPointer();

extern void            Hv_XMUngrabPointer();

extern Boolean         Hv_XMGetNextFile(DIR *,
				      char *,
				      char *,
				      FILE **,
				      char *);

extern int             Hv_XMAlphaSortDirectory(char *dirname,
					     char *extension,
					     char ***names,
					     Boolean prepend,
					     Boolean remext);

extern DIR            *Hv_XMOpenDirectory(char *);
extern int 	       Hv_XMCloseDirectory(DIR *);
extern int             Hv_XMDoDialog(Hv_Widget,
				   Hv_Widget);


#endif

#endif


