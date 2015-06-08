/*-----------------------------------------------------------------------------
 * Copyright (c) 1991,1992 Southeastern Universities Research Association,
 *                         Continuous Electron Beam Accelerator Facility
 *
 * This software was developed under a United States Government license
 * described in the NOTICE file included as part of this distribution.
 *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
 * Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
 *-----------------------------------------------------------------------------
 * 
 * Description:
 *	CODA Editor drawing graph widget
 *	
 * Author: Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_graph.c,v $
 *   Revision 1.16  1998/06/18 12:28:29  heyes
 *   tidy up a lot of thingscd ../cedit
 *
 *   Revision 1.15  1997/12/03 16:56:09  heyes
 *   increase array bounds to pervent core dumps in graph etc
 *
 *   Revision 1.14  1997/11/25 16:27:02  rwm
 *   Eliminated dead code.
 *
 *   Revision 1.13  1997/11/24 15:09:27  heyes
 *   bombproof strsave
 *
 *   Revision 1.12  1997/09/08 15:19:26  heyes
 *   fix dd icon etc
 *
 *   Revision 1.11  1997/09/05 12:06:46  heyes
 *   almost final
 *
 *   Revision 1.10  1997/08/29 12:25:31  heyes
 *   fixed window positioning
 *
 *   Revision 1.9  1997/08/01 18:37:45  heyes
 *   fixed moving bug
 *
 *   Revision 1.8  1997/07/30 15:16:57  heyes
 *   add more xpm support
 *
 *   Revision 1.7  1997/07/22 12:16:39  heyes
 *   need to debug on solaris
 *
 *   Revision 1.6  1997/07/11 13:01:39  heyes
 *   tweak coloring
 *
 *   Revision 1.5  1997/07/09 19:05:31  heyes
 *   darnit marki
 *
 *   Revision 1.4  1997/07/08 15:30:37  heyes
 *   add stuff
 *
 *   Revision 1.3  1997/06/20 17:00:05  heyes
 *   clean up GUI!
 *
 *   Revision 1.2  1997/06/16 12:23:49  heyes
 *   various fixes and nicities!
 *
 *   Revision 1.1.1.2  1996/11/05 17:45:08  chen
 *   coda source
 *
 *	  
 */
#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/DrawingA.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>

#include "Ruler.h"

#include "Editor.h"
#include "Editor_layout.h"
#include "Editor_graph.h"
#include "Editor_drawing.h"
#include "Editor_database.h"
#include "Editor_xmisc.h"
#include "Editor_misc.h"
#include "Editor_graph_cmd.h"
#include "Editor_menu.h"
#include "Editor_widget_poll.h"
#include "Editor_color.h"
#include "Editor_color_sel.h"
#include "Editor_zoom.h"
#include "./xpm_icon/stone.xpm"
#include "XcodaXpm.h"

SwGeometry sw_geometry;
XpointerPosition xp_pos;
XGC              xgc;
XcodaEditorGraph coda_graph;

#if defined (__STDC__)
static void XcodaEditorInitXgc                     (Widget w);
static void XcodaEditorInitSwGeometry              (Widget h_sc,
						    Widget v_sc,
						    Widget drawArea,
						    Widget topRuler,
						    Widget leftRuler);
static void XcodaEditorInitGraph                   (Widget w);
static void scrolled                               (Widget w,
						    XtPointer data,
						    XmScrollBarCallbackStruct* c);
static void resize_callback                        (Widget w,
						    XtPointer data,
						    XmDrawingAreaCallbackStruct *);
static void zoom_scroll_graph                      (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void expose_callback                        (Widget w,
						    XtPointer data,
						    XmDrawingAreaCallbackStruct *);
static void updateIndicatorPositions               (void);
static void XcodaEditorBtndownAction               (Widget w,
						    XtPointer data,
						    XEvent* event);
static void XcodaEditorBtnupAction                 (Widget w,
						    XtPointer data,
						    XEvent* event);
static void XcodaEditorEnterWindowAction           (Widget w,
						    XtPointer data,
						    XEvent* event);
static void XcodaEditorSetAddNodeMode              (Widget w,
						    XtPointer data,
						    XmAnyCallbackStruct* cbs);
static void XcodaEditorActivateAdd                 (Widget w,
						    XtPointer data,
						    XEvent* event);
static void XcodaEditorDeactivateAdd               (Widget w,
						    XtPointer data,
						    XEvent* event);
static void XcodaEditorHighLightPort               (XcodaEditorGraph* graph,
						    ipPort* port);
static void XcodaEditorUnselectNode                (XcodaEditorGraph* graph,
						    drawComp* port);
static void XcodaEditorSelectNode                  (XcodaEditorGraph* graph,
						    drawComp* port);
static void XcodaEditorSelectArc                   (XcodaEditorGraph* graph,
						    Arc* arc);
static void XcodaEditorUnselectArc                 (XcodaEditorGraph* graph,
						    Arc* arc);
static drawComp *newDrawComp                       (drawComp* old,
						    int xpos, int ypos,
						    int col, int row,
						    int wd, int ht);
static drawComp *createDrawComp                    (XcodaEditorGraph* graph,
						    int type);
static drawComp *dupDrawComp                       (int type, 
						    drawComp* comp);
static void     setGraphStartingPosition           (XcodaEditorGraph* graph,
						    int x, int y);
static void     setDrawCompGeometry                (drawComp* comp,
						    int x, int y,
						    int wd, int ht);
static void     drawCompTransform                  (drawComp* comp,
						    int rol_off,
						    int col_off);
static void     freeDrawComp                       (drawComp* comp);
static drawComp *XcodaEditorClickedNode            (XcodaEditorGraph* graph,
						    XEvent* event);
static ipPort   *XcodaEditorClickedPort            (XcodaEditorGraph* graph,
						    XEvent* event);
static Arc      *XcodaEditorClickedArc             (XcodaEditorGraph* graph,
						    XEvent* event);
static void XcodaEditorHighLightAllConnectedPorts  (XcodaEditorGraph* graph,
						    ipPort* port);
static void XcodaEditorUnhighLightAllConnectedPorts(XcodaEditorGraph* graph,
						    ipPort* port);
static void StartAddNode                           (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void StartAddExistingNode                   (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void EndAddNode                             (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void EndAddExistingNode                     (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void StartMovingNode                        (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void StartMovingArc                         (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void EndMovingNode                          (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void EndMovingArc                           (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void StartConnectingPorts                   (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void EndConnectingPorts                     (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void HandleMotion                           (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void HandleDoubleClick                      (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void PositionNewNode                        (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void PositionNewArc                         (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void resize_comp                            (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void connect_2cols                           (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void connect_2cols_action                    (XcodaEditorGraph* graph,
						     int first,
						     int second);
static void redisplay                              (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static int select_arc_or_node                     (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void select_ip_ports                        (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static int  isOverlapAny                           (XcodaEditorGraph* graph,
						    drawComp *q,
						    int col, int row);
static int  isDoubleClick                          (XcodaEditorGraph* graph,
						    XEvent* event);
static int  isArcMovable                           (Arc* arc);
static void delete_arc                             (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static void reassign_arc_geometry                  (Arc* arc,
						    int x, int y);
static void delete_comp                            (XcodaEditorGraph* graph,
						    Widget w,
						    XEvent* event);
static Arc* newArcConnectingComps                  (drawComp* comp0,
						    drawComp* comp1);
static Arc* newArcConnectingPorts                  (ipPort* from,
						    ipPort* to);
static void config_arc_geometry                    (Arc* arc,
						    ipPort* from,
						    ipPort* to);
#else
static void XcodaEditorInitXgc                     ();
static void XcodaEditorInitSwGeometry              ();
static void XcodaEditorInitGraph                   ();
static void scrolled                               ();
static void resize_callback                        ();
static void zoom_scroll_graph                      ();
static void expose_callback                        ();
static void updateIndicatorPositions               ();
static void XcodaEditorBtndownAction               ();
static void XcodaEditorBtnupAction                 ();
static void XcodaEditorEnterWindowAction           ();
static void XcodaEditorSetAddNodeMode              ();
static void XcodaEditorActivateAdd                 ();
static void XcodaEditorDeactivateAdd               ();
static void XcodaEditorHighLightPort               ();
static void XcodaEditorUnselectNode                ();
static void XcodaEditorSelectNode                  ();
static void XcodaEditorSelectArc                   ();
static void XcodaEditorUnselectArc                 ();
static drawComp *newDrawComp                       ();
static drawComp *createDrawComp                    ();
static drawComp *dupDrawComp                       ();
static void     setGraphStartingPosition           ();
static void     setDrawCompGeometry                ();
static void     drawCompTransform                  ();
static void     freeDrawComp                       ();
static drawComp *XcodaEditorClickedNode            ();
static ipPort   *XcodaEditorClickedPort            ();
static Arc      *XcodaEditorClickedArc             ();
static void XcodaEditorHighLightAllConnectedPorts  ();
static void XcodaEditorUnhighLightAllConnectedPorts();
static void StartAddNode                           ();
static void StartAddExistingNode                   ();
static void EndAddNode                             ();
static void EndAddExistingNode                     ();
static void StartMovingNode                        ();
static void StartMovingArc                         ();
static void EndMovingNode                          ();
static void EndMovingArc                           ();
static void StartConnectingPorts                   ();
static void EndConnectingPorts                     ();
static void HandleMotion                           ();
static void HandleDoubleClick                      ();
static void PositionNewNode                        ();
static void PositionNewArc                         ();
static void resize_comp                            ();
static void connect_2cols                          ();
static void connect_2cols_action                   ();
static void redisplay                              ();
static int  select_arc_or_node                     ();
static void select_ip_ports                        ();
static int  isOverlapAny                           ();
static int  isDoubleClick                          ();
static int  isArcMovable                           ();
static void delete_arc                             ();
static void reassign_arc_geometry                  ();
static void delete_comp                            ();
static Arc* newArcConnectingComps                  ();
static Arc* newArcConnectingPorts                  ();
static void config_arc_geometry                    ();
#endif



/* return a from widget containing two ruler 
 * a drawing area and two scroll bar
 */
#if defined (__STDC__)
Widget XcodaEditorDrawingArea (Widget parent)
#else
Widget XcodaEditorDrawingArea (parent)
     Widget parent;
#endif
{
  Widget form, h_sc, v_sc;
  Widget drawing_area, sw;
  Widget top_ruler, left_ruler;
  Widget top_frame, left_frame;
  Widget clipped_win;
  Dimension ruler_width= 30,ruler_height = 30;
  Dimension scb_width = 15, scb_height = 15;
  Dimension sh_thick = 2;
  Arg    args[20];
  int    ac = 0;

  /* initialize all graphics stuff here */

  sw_geometry.bg_pixmap =  XcodaCreatePixmapFromXpm(parent,stone_xpm, 1);
  XtSetArg(args[ac], XmNbackgroundPixmap, sw_geometry.bg_pixmap); ac++;
  XtSetValues(parent, args, ac);

  ac = 0;
  XcodaEditorInitXgc (parent);

  form = XtCreateWidget("drawing_form",xmFormWidgetClass,
			parent, NULL, 0);
  
  XtSetArg(args[ac], XmNshadowThickness, 2); ac++;
  XtSetArg(args[ac], XmNscrollBarDisplayPolicy, XmSTATIC); ac++;
  XtSetArg(args[ac], XmNscrollBarPlacement, XmBOTTOM_RIGHT); ac++;
  XtSetArg(args[ac], XmNscrollingPolicy, XmAPPLICATION_DEFINED); ac++;
  XtSetArg(args[ac], XmNvisualPolicy, XmVARIABLE); ac++;
  sw = XtCreateManagedWidget("scrolled_window",
			     xmScrolledWindowWidgetClass, form,
			     args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNorientation, XmHORIZONTAL); ac++;
  XtSetArg(args[ac], XmNprocessingDirection, XmMAX_ON_RIGHT); ac++;
  XtSetArg(args[ac], XmNshowArrows, TRUE); ac++;
  h_sc = XmCreateScrollBar(sw,"editor_hsc",args, ac);
  XtManageChild(h_sc);
  ac = 0;

  XtSetArg(args[ac], XmNorientation, XmVERTICAL); ac++;
  XtSetArg(args[ac], XmNprocessingDirection, XmMAX_ON_BOTTOM); ac++;
  XtSetArg(args[ac], XmNshowArrows, TRUE); ac++;
  v_sc = XmCreateScrollBar(sw,"editor_hsc",args, ac);
  XtManageChild(v_sc);
  ac = 0;

/* create frame Widget */
  XtSetArg(args[ac], XmNshadowType, XmSHADOW_IN); ac++;
  XtSetArg(args[ac], XmNshadowThickness, 2); ac++;
  top_frame = XtCreateManagedWidget("drawing_top_frame",xmFrameWidgetClass,
				    form, args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNshadowType, XmSHADOW_IN); ac++;
  XtSetArg(args[ac], XmNshadowThickness, 2); ac++;
  left_frame = XtCreateManagedWidget("drawing_left_frame",xmFrameWidgetClass,
				    form, args, ac);
  ac = 0;				    

  XtSetArg(args[ac], XmNwidth,500); ac++;
  XtSetArg(args[ac], XmNheight,500); ac++;
  drawing_area = XtCreateManagedWidget("drawing_area",xmDrawingAreaWidgetClass,
				       sw, args, ac);
  ac = 0;
  add_widget_to_poll (drawing_area);

  XtSetArg(args[ac], XmNborderWidth, 0); ac++;
  XtSetArg(args[ac], XmNbackground, xgc.Gray1); ac++;
  XtSetArg(args[ac], XmNforeground, xgc.White); ac++;
  XtSetArg(args[ac], XcodaNtickerColor, xgc.White); ac++;
  XtSetArg(args[ac], XcodaNindicatorColor, xgc.White); ac++;
  XtSetArg(args[ac], XcodaNnumDivBetweenMajor, 4); ac++;
  XtSetArg(args[ac], XcodaNorientation, XcodaTOP_HORIZONTAL); ac++;
  XtSetArg(args[ac], XcodaNrealTickerDiv, 5); ac++;
  XtSetArg(args[ac], XcodaNworldTickerDiv, 1); ac++;
  top_ruler = XtCreateManagedWidget("top_ruler",xcodaRulerWidgetClass,
				    top_frame, args, ac);
  ac = 0;
  add_widget_to_poll (top_ruler);
				        
  XtSetArg(args[ac], XmNborderWidth, 0); ac++;
  XtSetArg(args[ac], XmNbackground, xgc.Gray1); ac++;
  XtSetArg(args[ac], XmNforeground, xgc.White); ac++;
  XtSetArg(args[ac], XcodaNtickerColor, xgc.White); ac++;
  XtSetArg(args[ac], XcodaNindicatorColor, xgc.White); ac++;
  XtSetArg(args[ac], XcodaNnumDivBetweenMajor, 4); ac++;
  XtSetArg(args[ac], XcodaNorientation, XcodaLEFT_VERTICAL); ac++;
  XtSetArg(args[ac], XcodaNrealTickerDiv, 5); ac++;
  XtSetArg(args[ac], XcodaNworldTickerDiv, 1); ac++;
  left_ruler = XtCreateManagedWidget("left_ruler",xcodaRulerWidgetClass,
				  left_frame, args, ac);
  ac = 0;
  add_widget_to_poll (left_ruler);
  
  /* get clipped window ID */
  XtSetArg (args[ac], XmNworkWindow, &clipped_win); ac++;
  XtGetValues (sw, args, ac);
  ac = 0;
  add_widget_to_poll (clipped_win);

/* set up form widget resources */
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 5); ac++;
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNtopOffset, (int)(ruler_height + 2*sh_thick + scb_width));ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomOffset, (int)(scb_height + 2*sh_thick)); ac++;
  XtSetValues(left_frame, args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNtopOffset,     5); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, (int)(ruler_width+ 2*sh_thick + scb_width)); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, (int)(scb_width + 2*sh_thick)); ac++;
  XtSetValues(top_frame, args, ac); 
  ac = 0;

  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNleftWidget, left_frame); ac++;
  XtSetArg(args[ac], XmNleftOffset, 10); ac++;
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, top_frame); ac++;
  XtSetArg(args[ac], XmNtopOffset, 10); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetValues(sw, args, ac);
  ac = 0;

  XmScrolledWindowSetAreas(sw, h_sc, v_sc, drawing_area);

  XtManageChild(form);

  XcodaEditorInitSwGeometry(h_sc, v_sc, drawing_area,top_ruler, left_ruler);
  XcodaEditorBgPixmap(drawing_area);
  XcodaEditorInitGraph(drawing_area);

/* add edit popup menu */
  XcodaEditorCreatePopupMenu(drawing_area);

  XtAddEventHandler(drawing_area, PointerMotionMask, False,
		    XcodaEditorTrackPointerPosition, (XtPointer)NULL);
  XtAddEventHandler(drawing_area, ButtonPressMask, False,
		    XcodaEditorBtndownAction, (XtPointer)NULL);
  XtAddEventHandler(drawing_area, ButtonReleaseMask, False,
		    XcodaEditorBtnupAction, (XtPointer)NULL);
  XtAddEventHandler(drawing_area,Button2MotionMask, False,
		    XcodaEditorTrackPointerPosition, (XtPointer)NULL);
  XtAddEventHandler(drawing_area, EnterWindowMask, False,
		    XcodaEditorEnterWindowAction, (XtPointer)NULL);

  XtAddCallback(h_sc, XmNvalueChangedCallback, scrolled,
		(XtPointer)XmHORIZONTAL);
  XtAddCallback(h_sc, XmNdragCallback, scrolled,(XtPointer)XmHORIZONTAL);
  XtAddCallback(v_sc, XmNvalueChangedCallback, scrolled,
		(XtPointer)XmVERTICAL);
  XtAddCallback(v_sc, XmNdragCallback, scrolled,(XtPointer)XmVERTICAL);
  XtAddCallback(drawing_area, XmNresizeCallback,resize_callback,
		(XtPointer)NULL);
  XtAddCallback(drawing_area, XmNexposeCallback,expose_callback,
		(XtPointer)NULL);

  XtAddCallback(sw_geometry.up_arrow, XmNactivateCallback, set_zoomscale,
		(XtPointer)0);
  XtAddCallback(sw_geometry.dn_arrow, XmNactivateCallback, set_zoomscale,
		(XtPointer)1);

  XtAddEventHandler(manager.roc_btn, EnterWindowMask, False, XcodaEditorActivateAdd,
		    (XtPointer)ADD_ROC_ACTION);

  XtAddEventHandler(manager.eb_btn, EnterWindowMask, False, XcodaEditorActivateAdd,
		    (XtPointer)ADD_EB_ACTION);

  XtAddEventHandler(manager.er_btn, EnterWindowMask, False, XcodaEditorActivateAdd,
		    (XtPointer)ADD_ER_ACTION);
  XtAddEventHandler(manager.er_btn, LeaveWindowMask, False, XcodaEditorDeactivateAdd,
		    (XtPointer)ADD_ER_ACTION);

  XtAddEventHandler(manager.fi_btn, EnterWindowMask, False, XcodaEditorActivateAdd,
		    (XtPointer)ADD_FI_ACTION);
  XtAddEventHandler(manager.fi_btn, EnterWindowMask, False, XcodaEditorActivateAdd,
		    (XtPointer)ADD_FI_ACTION);

  XtAddEventHandler(manager.cfi_btn, EnterWindowMask, False, XcodaEditorActivateAdd,
		    (XtPointer)ADD_CFI_ACTION);
  XtAddEventHandler(manager.cfi_btn, EnterWindowMask, False, XcodaEditorActivateAdd,
		    (XtPointer)ADD_CFI_ACTION);

  XtAddEventHandler(manager.dbg_btn, EnterWindowMask, False, XcodaEditorActivateAdd,
		    (XtPointer)ADD_DBG_ACTION);
  XtAddEventHandler(manager.dbg_btn, EnterWindowMask, False, XcodaEditorActivateAdd,
		    (XtPointer)ADD_DBG_ACTION);

  XtAddEventHandler(manager.dd_btn, EnterWindowMask, False, XcodaEditorActivateAdd,
		    (XtPointer)ADD_MON_ACTION);
  XtAddEventHandler(manager.none_btn, EnterWindowMask, False, XcodaEditorActivateAdd,
		    (XtPointer)ADD_NONE_ACTION);

  XtAddEventHandler(manager.trig_btn, EnterWindowMask, False, XcodaEditorActivateAdd,
		    (XtPointer)ADD_TRIG_ACTION);

  XtAddEventHandler(manager.roc_btn, LeaveWindowMask, False, XcodaEditorDeactivateAdd,
		    (XtPointer)ADD_ROC_ACTION);

  XtAddEventHandler(manager.eb_btn, LeaveWindowMask, False, XcodaEditorDeactivateAdd,
		    (XtPointer)ADD_EB_ACTION);


  XtAddEventHandler(manager.trig_btn, LeaveWindowMask, False, XcodaEditorDeactivateAdd,
		    (XtPointer)ADD_TRIG_ACTION);

  /* file menus callbacks 
  XtAddCallback(manager.eth_btn, XmNactivateCallback, XcodaEditorExecGraphCmd,
		(XtPointer)IP_CONNECTION);
  XtAddCallback(manager.mvnode_btn, XmNactivateCallback, XcodaEditorExecGraphCmd,
		(XtPointer)MOVE_IP_NODE);
		*/
  XtAddCallback(manager.newdbase, XmNactivateCallback, 
		XcodaEditorNewDbaseCbk,	(XtPointer)0);
  XtAddCallback(manager.opendbase, XmNactivateCallback,
		XcodaEditorOpenDbaseCbk, (XtPointer)0);
  XtAddCallback(manager.removedbase, XmNactivateCallback, 
		XcodaEditorRemoveDbaseCbk, (XtPointer)0);
		
  /* configuration menus callbacks */
  XtAddCallback(manager.newconfig, XmNactivateCallback, 
		XcodaEditorNewConfigCbk,
		(XtPointer)0);   
  XtAddCallback(manager.openconfig, XmNactivateCallback, 
		XcodaEditorOpenConfigCbk,
		(XtPointer)0); 
  XtAddCallback(manager.removeconfig, XmNactivateCallback, 
		XcodaEditorRemoveConfigCbk,
		(XtPointer)0); 
  XtAddCallback(manager.saveconfig, XmNactivateCallback, 
		XcodaEditorSaveDefault,	(XtPointer)NULL); 
  XtAddCallback(manager.saveasconfig, XmNactivateCallback, 
		XcodaEditorSaveConfig,(XtPointer)NULL);
  XtAddCallback(manager.option, XmNactivateCallback,
		XcodaEditorConfigOption, (XtPointer)NULL);
  if (manager.exit_menu)
    XtAddCallback(manager.exit_menu, XmNactivateCallback, XcodaEditorExitMenuCbk,
		  (XtPointer)NULL);

  XtAddCallback(manager.color_menu, XmNactivateCallback, popup_color_selector,
		(XtPointer)NULL);
  XtAddCallback(manager.rmvcomp_menu, XmNactivateCallback, XcodaEditorCleanDbase,
		(XtPointer)NULL);

  XtAddCallback(manager.delete_comp_menu,XmNactivateCallback, XcodaEditorSetDeleteCompMode,
		(XtPointer)DELETE_NODE_ACTION);
  XtAddCallback(manager.delete_arc_menu, XmNactivateCallback, XcodaEditorSetDeleteArcMode,
		(XtPointer)DELETE_ARC_ACTION);
  XtAddCallback(manager.delete_allarc_menu, XmNactivateCallback, XcodaEditorDeleteAllArcs,
		(XtPointer)NULL);
  XtAddCallback(manager.delete_all_menu, XmNactivateCallback, XcodaEditorDeleteAll,
		(XtPointer)NULL);
  XtAddCallback(manager.undo_menu, XmNactivateCallback, XcodaEditorUndoAction,
		(XtPointer)UNDO_ACTION);
  XtAddCallback(manager.redraw_menu, XmNactivateCallback, XcodaEditorRedraw,
		(XtPointer)NULL);
  XtAddCallback(manager.resize_comp_menu, XmNactivateCallback, XcodaEditorResizeComp,
		(XtPointer)RESIZE_COMP_ACTION);
  XtAddCallback(manager.connect2cols_menu, XmNactivateCallback, 
		XcodaEditorConn2Cols,
		(XtPointer)CONN_2COLS_ACTION);

  return form;
}

#if defined (__STDC__)
static void XcodaEditorInitSwGeometry(Widget h_sc, 
				      Widget v_sc, 
				      Widget draw, 
				      Widget top_ruler, 
				      Widget left_ruler)
#else
static void XcodaEditorInitSwGeometry(h_sc, v_sc, draw, top_ruler, left_ruler)
     Widget h_sc, v_sc, draw;
     Widget top_ruler, left_ruler;
#endif
{
  Arg   args[20];
  int   ac = 0;

  /* setup scroll bar resources */
  sw_geometry.cell_ht = 24;
  sw_geometry.cell_wd = 48;
  sw_geometry.zoomscale = sw_geometry.old_zoomscale = 2;
  sw_geometry.zoomxoff = sw_geometry.zoomyoff = 0;

  sw_geometry.max_row = 100;
  sw_geometry.max_col = 100;
  sw_geometry.min_row = 0;
  sw_geometry.min_col = 0;
  sw_geometry.view_wd = 500;
  sw_geometry.view_ht = 500;
  sw_geometry.slider_size_h = sw_geometry.view_wd/
                              (sw_geometry.cell_wd*sw_geometry.zoomscale);
  sw_geometry.slider_size_v = sw_geometry.view_ht/
                              (sw_geometry.cell_ht*sw_geometry.zoomscale);
  sw_geometry.pageinc_h = sw_geometry.slider_size_h - 1;
  sw_geometry.pageinc_v = sw_geometry.slider_size_v - 1;  
  
  XtSetArg(args[ac], XmNmaximum, (int)sw_geometry.max_col); ac++;
  XtSetArg(args[ac], XmNminimum, (int)sw_geometry.min_col); ac++;
  XtSetArg(args[ac], XmNpageIncrement, (int)sw_geometry.pageinc_h); ac++;
  XtSetArg(args[ac], XmNsliderSize, (int)sw_geometry.slider_size_h); ac++;
  XtSetArg(args[ac], XmNincrement, 1); ac++;
  XtSetValues(h_sc, args, ac); 
  ac = 0;
  
  XtSetArg(args[ac], XmNmaximum, (int)sw_geometry.max_row); ac++;
  XtSetArg(args[ac], XmNminimum, (int)sw_geometry.min_row); ac++;
  XtSetArg(args[ac], XmNpageIncrement, (int)sw_geometry.pageinc_v); ac++;
  XtSetArg(args[ac], XmNsliderSize, (int)sw_geometry.slider_size_v); ac++;
  XtSetArg(args[ac], XmNincrement, 1); ac++;
  XtSetValues(v_sc, args, ac); 
  ac = 0;

  sw_geometry.draw_area = draw;
  sw_geometry.top_ruler = top_ruler;
  sw_geometry.left_ruler = left_ruler;
  sw_geometry.h_scb = h_sc;
  sw_geometry.v_scb = v_sc;

  reset_scrollbar_res();
}

/*******************************************************************
 *        void XcodaEditorInitXgc(w)                               *
 * Description:                                                    *
 *   Initialize XGC Data Structure which provides all graphics     *
 *   contexts                                                      *
 ******************************************************************/
#if defined (__STDC__)
void XcodaEditorInitXgc(Widget w)
#else
void XcodaEditorInitXgc(w)
     Widget w;
#endif
{
  XGCValues gcv;
  Arg       args[10];
  Pixel     fg, bg;
  int       ac = 0, status;
  Colormap  cmap;
  XColor    col, unused;
  Pixel     temp;
  XFontStruct *tfont;

  xgc.dpy = XtDisplay(w);
  xgc.root_w = XDefaultRootWindow(xgc.dpy);
  xgc.scr = XDefaultScreenOfDisplay(xgc.dpy);
  xgc.depth = DefaultDepthOfScreen(xgc.scr);

  XtSetArg(args[ac], XmNbackground, &bg); ac++;
  XtSetArg(args[ac], XmNforeground, &fg); ac++;
  XtGetValues(w, args, ac); 
  ac = 0;

  /* initialize background color to the file sepecified value */
  cmap = DefaultColormapOfScreen(xgc.scr);
  status = XcodaEditorInitColor (xgc.dpy, cmap, &fg, &bg);
 
  xgc.fg = fg;
  xgc.bg = bg;

  gcv.foreground = bg;
  gcv.background = bg;
  gcv.fill_style = FillTiled; 
  gcv.tile = sw_geometry.bg_pixmap;
  gcv.function = GXcopy;
  xgc.erase_gc = XCreateGC(xgc.dpy, xgc.root_w,GCFillStyle|GCTile|GCForeground|GCBackground,
			   &gcv);

  gcv.foreground = fg;
  gcv.background = bg;
  gcv.function = GXcopy;
  xgc.r_gc = XCreateGC(xgc.dpy, xgc.root_w,GCFunction|GCForeground|GCBackground,
		      &gcv);

  gcv.foreground = fg;
  gcv.background = fg;
  gcv.function = GXxor;
  xgc.hl_gc = XCreateGC(xgc.dpy, xgc.root_w,GCForeground|GCBackground,
			&gcv);

  gcv.foreground = fg^bg;
  gcv.background = bg;
  gcv.function = GXxor;
  xgc.xor_gc = XCreateGC(xgc.dpy, xgc.root_w, GCFunction|GCForeground|
			 GCBackground,&gcv);
  
  if ((tfont = XLoadQueryFont(xgc.dpy,"-misc-fixed-*-*-*-*-*-80-*-*-*-*-*-*")) 
      == NULL) {
    fprintf (stderr, "Cannot find simplest font on your x server, quit\n");
    exit (1);
  }
  else
    xgc.font[0] = tfont;
    
  xgc.font[1] = xgc.font[0];

  if ((tfont = XLoadQueryFont(xgc.dpy,"-adobe-times-medium-r-*-*-10-*-*-*-*-*-*-*"))
      == NULL) {
    fprintf(stderr, "cannot find font: -adobe-times-medium-r-*-*-10-*-*-*-*-*-*-*\n");
    xgc.font[2] = xgc.font[1];
  }
  else
    xgc.font[2] = tfont;

  if ((tfont = XLoadQueryFont(xgc.dpy,"-adobe-times-medium-r-*-*-11-*-*-*-*-*-*-*"))
      == NULL) {
    fprintf(stderr,"cannot find font: -adobe-times-medium-r-*-*-11-*-*-*-*-*-*-*\n"); 
    xgc.font[3] = xgc.font[1];
  }
  else
    xgc.font[3] = tfont;
    
  if ((tfont = XLoadQueryFont(xgc.dpy,"-adobe-times-medium-r-*-*-12-*-*-*-*-*-*-*"))
      == NULL ){
    fprintf(stderr, "cannot find font:-adobe-times-medium-r-*-*-12-*-*-*-*-*-*-*\n");
    xgc.font[4] = xgc.font[1];
  }
  else
    xgc.font[4] = tfont;

  if ((tfont = XLoadQueryFont(xgc.dpy,"-adobe-times-medium-r-*-*-14-*-*-*-*-*-*-*"))
      == NULL) {
    fprintf(stderr, "cannot find font: -adobe-times-medium-r-*-*-14-*-*-*-*-*-*-*\n");
    xgc.font[5] = xgc.font[1];
  }
  else
    xgc.font[5] = tfont;

  xgc.White = WhitePixelOfScreen(xgc.scr);
  xgc.Black = BlackPixelOfScreen(xgc.scr);
  temp = get_pixel(xgc.dpy, cmap, 65535, 0, 0);
  if(temp)
    xgc.Red = temp;
  else
    xgc.Red = xgc.Black;
  temp = get_pixel(xgc.dpy, cmap, 0,0,32896);
  if(temp)
    xgc.NavyBlue = temp;
  else
    xgc.NavyBlue = xgc.Black;
  xgc.NavyBlue = bg;
  temp = get_pixel(xgc.dpy, cmap, 0, 65535, 65535);
  if(temp)
    xgc.Cyan = temp;
  else
    xgc.Cyan = xgc.White;
  temp = get_pixel(xgc.dpy, cmap, 65535, 65535, 0);
  if(temp)
    xgc.Yellow = temp;
  else
    xgc.Yellow = xgc.White;
  temp = get_pixel(xgc.dpy, cmap, 0, 65535, 0);
  if(temp)
    xgc.Green = temp;
  else
    xgc.Green = xgc.White;
  temp = get_pixel(xgc.dpy, cmap, 45545, 45545, 45545);
  if (temp)
    xgc.Gray0 = temp;
  else
    xgc.Gray0 = xgc.Black;
  temp = get_pixel(xgc.dpy, cmap, 25545, 25545, 25545);
  if (temp)
    xgc.Gray1 = temp;
  else
    xgc.Gray1 = xgc.Black;

  temp = get_pixel(xgc.dpy, cmap, 55296, 34048, 18944);
  if (temp)
    xgc.Paused = temp;
  else
    xgc.Paused = xgc.Black;
  temp = get_pixel(xgc.dpy, cmap, 63488, 47104, 30720);
  if (temp)
    xgc.Ending = temp;
  else
    xgc.Ending = xgc.Black;
}

/***********************************************************************
 *                void XcodaEditorInitGraph(w)                         *
 * Description:                                                        *
 *    Initialize codaGraph Object                                      *
 **********************************************************************/
#if defined (__STDC__)
static void XcodaEditorInitGraph(Widget w)
#else
static void XcodaEditorInitGraph(w)
     Widget w;
#endif
{
  coda_graph.delete_cursor = XCreateFontCursor(xgc.dpy, XC_pirate);
  coda_graph.working_cursor = XCreateFontCursor(xgc.dpy, XC_watch);
  coda_graph.delete_arc_cursor = XCreateFontCursor(xgc.dpy, XC_crosshair);
  coda_graph.finger_cursor = XCreateFontCursor(xgc.dpy, XC_hand2);
  coda_graph.start_x = coda_graph.end_x = 0;
  coda_graph.start_y = coda_graph.end_y = 0;
  coda_graph.modified = 0;
  coda_graph.child_spacing = sw_geometry.cell_ht*sw_geometry.zoomscale;
  coda_graph.sibling_spacing = sw_geometry.cell_wd*sw_geometry.zoomscale*2;
  coda_graph.current_action = NORMAL_ACTION;
  coda_graph.previous_action = NORMAL_ACTION;
  coda_graph.last_button_time = 0;
  coda_graph.double_click_interval = 250;
  coda_graph.comp_list_head = (compList *)malloc(1*sizeof(compList));
  coda_graph.comp_list_tail = (compList *)malloc(1*sizeof(compList));
  coda_graph.comp_list_head->next = coda_graph.comp_list_tail;
  coda_graph.comp_list_head->previous = (compList *)NULL;
  coda_graph.comp_list_tail->next = (compList *)NULL;
  coda_graph.comp_list_tail->previous = coda_graph.comp_list_head;
  coda_graph.arc_list_head = (arcList *)malloc(1*sizeof(arcList));
  coda_graph.arc_list_tail = (arcList *)malloc(1*sizeof(arcList));
  coda_graph.arc_list_head->previous = (arcList *)NULL;
  coda_graph.arc_list_head->next = coda_graph.arc_list_tail;
  coda_graph.arc_list_tail->previous = coda_graph.arc_list_head;
  coda_graph.arc_list_tail->next = (arcList *)NULL;
  coda_graph.add_comp = StartAddNode;
  coda_graph.add_existing_comp = StartAddExistingNode;
  coda_graph.redisplay = redisplay;
  coda_graph.zoom_scroll = zoom_scroll_graph;
  coda_graph.select_comp = select_arc_or_node;
  coda_graph.highlight_ip_ports = select_ip_ports;
  coda_graph.delete_arc = delete_arc;
  coda_graph.delete_comp = delete_comp;
}

/*****************************************************************************************
 *      static void XcodaEditorActivateAdd()                                             *
 * Description:                                                                          *
 *    Only allow click button on the pushbutton to call callbacks                        *
 *    Key activate will not allowed                                                      *
 ****************************************************************************************/
#if defined (__STDC__)
static void XcodaEditorActivateAdd(Widget w, 
				   XtPointer client_data, 
				   XEvent* event)
#else
static void XcodaEditorActivateAdd(w, client_data, event)
     Widget w;
     XtPointer client_data;
     XEvent    *event;
#endif
{
  if (databaseSelected ())
    XtAddCallback(w, XmNactivateCallback,XcodaEditorSetAddNodeMode, (XtPointer)client_data);
}

#if defined (__STDC__)
static void XcodaEditorDeactivateAdd(Widget w, 
				     XtPointer client_data, 
				     XEvent* event)
#else
static void XcodaEditorDeactivateAdd(w, client_data, event)
     Widget w;
     XtPointer client_data;
     XEvent    *event;
#endif
{
  XtRemoveAllCallbacks(w, XmNactivateCallback);
}

#if defined (__STDC__)
void XcodaEditorBgPixmap(Widget w)
#else
void XcodaEditorBgPixmap(w)
     Widget w;
#endif
{
  int     i, j, k;
  Arg     args[20];
  int     ac = 0;
  int     x, y;

  sw_geometry.bg_pixmap =  XcodaCreatePixmapFromXpm(w,stone_xpm, 1);
  XtSetArg(args[ac], XmNbackgroundPixmap, sw_geometry.bg_pixmap); ac++;
  XtSetValues(w, args, ac);
  ac = 0;
  XSetTile(xgc.dpy, xgc.erase_gc, sw_geometry.bg_pixmap);

  return;

#ifdef THIS_CODE_NOT_USED

  if(sw_geometry.bg_pixmap != NULL){
    XFreePixmap(xgc.dpy, sw_geometry.bg_pixmap);
  }
  sw_geometry.bg_pixmap = XCreatePixmap(xgc.dpy, xgc.root_w, 
	         sw_geometry.view_wd, sw_geometry.view_ht, xgc.depth);
  XFillRectangle(xgc.dpy,sw_geometry.bg_pixmap,xgc.erase_gc, 0, 0,
		 sw_geometry.view_wd, sw_geometry.view_ht);

  XSetForeground(xgc.dpy, xgc.r_gc, xgc.fg);
/* draw horizontal grid first */
  for(k=0;k<=sw_geometry.slider_size_v;k++){
    for(i=0;i<sw_geometry.slider_size_h;i++){ /* for each row */
      for(j=0;j<4;j++){ /* 4 dots */
	x = (sw_geometry.cell_wd*sw_geometry.zoomscale/4)*j +
	  i*sw_geometry.cell_wd*sw_geometry.zoomscale;
	y = sw_geometry.cell_ht*sw_geometry.zoomscale*k;
	XDrawPoint(xgc.dpy, sw_geometry.bg_pixmap, xgc.r_gc, x, y);
      }
    }
    if(sw_geometry.slider_size_h*sw_geometry.cell_wd*sw_geometry.zoomscale
       < sw_geometry.view_wd){/* some extra space left */
      j = 0;
      while((x = sw_geometry.slider_size_h*sw_geometry.cell_wd*
	     sw_geometry.zoomscale + 
	     (sw_geometry.cell_wd*sw_geometry.zoomscale/4)*j) 
	    <= sw_geometry.view_wd){
	y = sw_geometry.cell_ht*sw_geometry.zoomscale*k;
	XDrawPoint(xgc.dpy, sw_geometry.bg_pixmap,xgc.r_gc, x, y);
	j++;
      }
    }
  }
/* draw vertical grid */
  for(k=0;k<=sw_geometry.slider_size_h; k++){
    for(i=0;i<sw_geometry.slider_size_v;i++){
      for(j=1;j<4;j++){
	x = (sw_geometry.cell_wd*sw_geometry.zoomscale)*k;
	y = (sw_geometry.cell_ht*sw_geometry.zoomscale/4)*j +
	  i*sw_geometry.cell_ht*sw_geometry.zoomscale;
	XDrawPoint(xgc.dpy, sw_geometry.bg_pixmap, xgc.r_gc, x, y);
      }
    }
    if(sw_geometry.slider_size_v*sw_geometry.cell_ht*sw_geometry.zoomscale
       < sw_geometry.view_ht){ /* some space left */
      j = 0;
      while((y = sw_geometry.slider_size_v*sw_geometry.cell_ht*
	     sw_geometry.zoomscale + 
	     j*(sw_geometry.cell_ht*sw_geometry.zoomscale)/4) 
	    <= sw_geometry.view_ht){
	x = sw_geometry.cell_wd*sw_geometry.zoomscale*k;
	XDrawPoint(xgc.dpy, sw_geometry.bg_pixmap,xgc.r_gc, x, y);
	j++;
      }
    }
  }

  XtSetArg(args[ac], XmNbackgroundPixmap, sw_geometry.bg_pixmap); ac++;
  XtSetValues(w, args, ac);
  ac = 0;
#endif
}

/*************************************************************************
 *       static void XcodaeditorBtndownAction()                          *
 * Description:                                                          *
 *     Button down action routine                                        *
 ************************************************************************/
#if defined (__STDC__)
static void XcodaEditorBtndownAction(Widget w,
				     XtPointer client_data, 
				     XEvent* event)
#else
static void XcodaEditorBtndownAction(w,client_data, event)
     Widget w;
     XtPointer client_data;
     XEvent    *event;
#endif
{
  drawComp *p;
  Arc      *arc;
  ipPort   *port;

  if(event->xbutton.button == Button1){
    switch(coda_graph.current_action){
    case ADD_TRANSACTION_ACTION:
      EndAddNode(&coda_graph, w, event);
      return;
    case ADD_EXISTING_COMP:
      EndAddExistingNode(&coda_graph, w, event);
      return;
    case UNDO_ACTION:
      coda_graph.current_action = NORMAL_ACTION;
      coda_graph.previous_action = NORMAL_ACTION;
      return;
    case NORMAL_ACTION:
      if (!(*coda_graph.select_comp)(&coda_graph, w, event))
	return;
      break;
    case DELETE_ARC_ACTION:
      (*coda_graph.delete_arc)(&coda_graph, w, event);
      return;
    case DELETE_NODE_ACTION:
      (*coda_graph.delete_comp)(&coda_graph, w, event);
      return;
    case RESIZE_COMP_ACTION:
      resize_comp(&coda_graph,w, event);
      return;
    case CONN_2COLS_ACTION:
      connect_2cols (&coda_graph, w, event);
      return;
    default:
      return;
    }
    if((arc = XcodaEditorClickedArc(&coda_graph,event)) != NULL){
      return;
#ifdef THIS_CODE_NOT_USED

      if(!isArcMovable(arc)){
	XBell(xgc.dpy, 0);
	return;
      }
      coda_graph.current_action = MOVE_ARC_ACTION;
      coda_graph.current_arc = arc;
      StartMovingArc(&coda_graph, w, event);
      return;
#endif
    }
    if((port = XcodaEditorClickedPort(&coda_graph, event)) != NULL){
      coda_graph.current_action = CONNECT_IP_PORTS_ACTION;
      coda_graph.from_port = port;
      (*coda_graph.highlight_ip_ports)(&coda_graph, w, event); 
      StartConnectingPorts(&coda_graph, w, event);
      return;
    }

    if((p = XcodaEditorClickedNode(&coda_graph,event)) != NULL){
      coda_graph.current_action = MOVE_NODE_ACTION;
      coda_graph.current_comp = p;
      XDefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area),coda_graph.delete_arc_cursor);
      StartMovingNode(&coda_graph, w, event);
      return;
    }
  }
}

/***********************************************************************
 *          static void XcodaEditorBtnupAction( )                      *
 * Description:                                                        *
 *     Button Up action routine for the drawing area                   *
 **********************************************************************/
#if defined (__STDC__)
static void XcodaEditorBtnupAction(Widget w,
				   XtPointer client_data, 
				   XEvent* event)
#else
static void XcodaEditorBtnupAction(w,client_data, event)
     Widget w;
     XtPointer client_data;
     XEvent    *event;
#endif
{
  if(event->xbutton.button == Button1){
    switch(coda_graph.current_action){
    case CONNECT_IP_PORTS_ACTION:
      coda_graph.current_action = NORMAL_ACTION;
      XUndefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area));
      EndConnectingPorts(&coda_graph, w, event);
      return;
    case MOVE_ARC_ACTION:
      coda_graph.current_action = NORMAL_ACTION;
      /*EndMovingArc(&coda_graph, w, event);*/
      return;
    case HIGHLIGHT_IP_PORTS_ACTION:
      if(event->xbutton.button == Button2)
	(*coda_graph.highlight_ip_ports)(&coda_graph, w, event); 
      return;
    case MOVE_NODE_ACTION:
      coda_graph.current_action = NORMAL_ACTION;
      EndMovingNode(&coda_graph, w, event);
      return;
    case NORMAL_ACTION:
      return;
    default:
      return;
    } 
  }
}

/***********************************************************************
 *       static void XcodaEditorEnterWindowAction()                    *
 * Description:                                                        *
 *    Enter window event handler                                       *
 **********************************************************************/
#if defined (__STDC__)
static void XcodaEditorEnterWindowAction(Widget w,
					 XtPointer client_data, 
					 XEvent* event)
#else
static void XcodaEditorEnterWindowAction(w,client_data, event)
     Widget w;
     XtPointer client_data;
     XEvent    *event;
#endif
{
  switch(coda_graph.current_action){
  case ADD_ROC_ACTION:
  case ADD_TRIG_ACTION:
  case ADD_EB_ACTION:
  case ADD_ER_ACTION:
  case ADD_FI_ACTION:
  case ADD_CFI_ACTION:
  case ADD_DBG_ACTION:
  case ADD_MON_ACTION:
  case ADD_NONE_ACTION:

    /*XGrabPointer (xgc.dpy, XtWindow (w), False,
		  ButtonPressMask|ButtonMotionMask|PointerMotionMask,
		  GrabModeAsync, GrabModeAsync,
		  XtWindow (w), None, CurrentTime);*/
    (*coda_graph.add_comp)(&coda_graph, w, event);
    coda_graph.current_action = ADD_TRANSACTION_ACTION;
    break;
  case ADD_EXISTING_COMP:
    /*XGrabPointer (xgc.dpy, XtWindow (w), False,
		  ButtonPressMask|ButtonMotionMask|PointerMotionMask,
		  GrabModeAsync, GrabModeAsync,
		  XtWindow (w), None, CurrentTime);*/
    (*coda_graph.add_existing_comp)(&coda_graph, w, event);
    break;
  case NORMAL_ACTION:
    break;
  default:
    break;
  }
}

/***********************************************************************
 *       static void XcodaEditorTrackPointerPosition()                 *
 * Description:                                                        *
 *     Action routine corresponding button  motion event               *
 **********************************************************************/
#if defined (__STDC__)
void XcodaEditorTrackPointerPosition(Widget w,
				     XtPointer client_data, 
				     XEvent* event)
#else
void XcodaEditorTrackPointerPosition(w,client_data, event)
     Widget w;
     XtPointer client_data;
     XEvent    *event;
#endif
{
  int      rel_x, rel_y;
  int      col_off, row_off;
  int      old_cols, old_rows;
  int      x, y;
  Arg      args[10];
  int      ac = 0;

  old_cols = xp_pos.cols;
  old_rows = xp_pos.rows;
  xp_pos.x = rel_x = event->xmotion.x;
  xp_pos.y = rel_y = event->xmotion.y;
  
  col_off = rel_x/(sw_geometry.cell_wd*sw_geometry.zoomscale);
  row_off = rel_y/(sw_geometry.cell_ht*sw_geometry.zoomscale);

  xp_pos.cols = col_off + sw_geometry.zoomxoff + 1;
  xp_pos.rows = row_off + sw_geometry.zoomyoff + 1;
  
  if(xp_pos.cols != old_cols){
    XtSetArg(args[ac], XcodaNposition, xp_pos.cols); ac++;
    XtSetValues(sw_geometry.top_ruler, args, ac);
    ac = 0;
  }
  if(xp_pos.rows != old_rows){
    XtSetArg(args[ac], XcodaNposition, xp_pos.rows); ac++;
    XtSetValues(sw_geometry.left_ruler, args, ac);
    ac = 0;
  }
  switch(coda_graph.current_action){
  case ADD_TRANSACTION_ACTION:
  case ADD_EXISTING_COMP:
    PositionNewNode(&coda_graph, w, event);
    break;
  case PERS_MOVE_NODE_ACTION:
  case MOVE_NODE_ACTION:
    PositionNewNode(&coda_graph, w, event);
    break;
  case PERS_CONNECT_IP_PORTS_ACTION:
  case CONNECT_IP_PORTS_ACTION:
    HandleMotion(&coda_graph, w, event);
    break;
  case MOVE_ARC_ACTION:
    PositionNewArc(&coda_graph, w, event);
    break;
  case NORMAL_ACTION:
    break;
  default:
    break;
  }
}

/********************************************************************
 *       void XcodaEditorAddCompNodeMode(w, client_data, cbs)       *
 * Description:                                                     *
 *    Callback routine for add components with predefined type      *
 ********************************************************************/
#if defined (__STDC__)
void XcodaEditorAddCompNodeMode(Widget w, 
				XtPointer client_data, 
				XmAnyCallbackStruct* cbs)
#else
void XcodaEditorAddCompNodeMode(w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmAnyCallbackStruct *cbs;
#endif
{
  if (databaseSelected ()) {  
    /* reset toggle button part of graph commands */
    XcodaEditorResetGraphCmd();
    coda_graph.current_action = (int)client_data;
  }
}

/********************************************************************
 *       static void XcodaEditorSetAddNodeMode(w, client_data, cbs) *
 * Description:                                                     *
 *    Callback routine for component button                         *
 ********************************************************************/
#if defined (__STDC__)
static void XcodaEditorSetAddNodeMode(Widget w, 
				      XtPointer client_data, 
				      XmAnyCallbackStruct* cbs)
#else
static void XcodaEditorSetAddNodeMode(w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmAnyCallbackStruct *cbs;
#endif
{
  /* reset toggle button part of graph commands */
  XcodaEditorResetGraphCmd();
  coda_graph.current_action = (int)client_data;
}

/*******************************************************************
 *       void XcodaEditorSetDeleteCompMode(w,client_data, cbs)     *
 * Description:                                                    *
 *    Callback routine for delete node menu                        *
 ******************************************************************/
#if defined (__STDC__)
void XcodaEditorSetDeleteCompMode(Widget w, 
				  XtPointer client_data,
				  XmAnyCallbackStruct* cbs)
#else
void XcodaEditorSetDeleteCompMode(w,client_data,cbs)
     Widget w;
     XtPointer client_data;
     XmAnyCallbackStruct *cbs;
#endif
{
  /* reset toggle button part of graph commands */
  XcodaEditorResetGraphCmd();
  coda_graph.current_action = DELETE_NODE_ACTION;
  XDefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area), 
		coda_graph.delete_cursor);
}

/*******************************************************************
 *    void XcodaEditorSetDeleteArcMode(w, client_data, cbs)        *
 * Description:                                                    *
 *    Callback routine for delete arc menu                         *
 ******************************************************************/
#if defined (__STDC__)
void XcodaEditorSetDeleteArcMode(Widget w, 
				 XtPointer client_data, 
				 XmAnyCallbackStruct* cbs)
#else
void XcodaEditorSetDeleteArcMode(w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmAnyCallbackStruct *cbs;
#endif
{
  /* reset toggle button part of graph commands */
  XcodaEditorResetGraphCmd();
  coda_graph.current_action = (int)client_data;
  XDefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area),coda_graph.delete_arc_cursor);
}

/******************************************************************
 *     void XcodaEditorUndoAction(w, client_data, cbs)            *
 * Description:                                                   *
 *    callback routine for undo button                            *
 *****************************************************************/
#if defined (__STDC__)
void XcodaEditorUndoAction(Widget w, 
			   XtPointer client_data, 
			   XmAnyCallbackStruct* cbs)
#else
void XcodaEditorUndoAction(w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmAnyCallbackStruct *cbs;
#endif
{
  coda_graph.current_action = (int)client_data;

  switch(coda_graph.previous_action){
  case DELETE_ARC_ACTION:
    delete_arc(&coda_graph, sw_geometry.draw_area, cbs->event);
    break;
  case DELETE_NODE_ACTION:
    delete_comp(&coda_graph, sw_geometry.draw_area, cbs->event);
    break;
  case CONN_2COLS_ACTION:
    connect_2cols_action (&coda_graph, 0, 0);
    break;
  default:
    break;
  }
  coda_graph.previous_action = NORMAL_ACTION;
}
    
/******************************************************************
 *     void XcodaEditorRedraw(w, client_data, cbs)                *
 * Description:                                                   *
 *    refresh everything                                          *
 *****************************************************************/
#if defined (__STDC__)
void XcodaEditorRedraw(Widget w,
		       XtPointer client_data, 
		       XmAnyCallbackStruct* cbs)
#else
void XcodaEditorRedraw(w,client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmAnyCallbackStruct *cbs;
#endif
{
  XClearWindow(xgc.dpy, XtWindow(sw_geometry.draw_area));
  (*coda_graph.redisplay)(&coda_graph, w, cbs->event);
}

/*****************************************************************
 *    void XcodaEditorResizeComp(w, client_data, cbs)            *
 * Description:                                                  *
 *     callback routine for resize component menu                *
 ****************************************************************/
#if defined (__STDC__)
void XcodaEditorResizeComp(Widget w, 
			   XtPointer client_data, 
			   XmAnyCallbackStruct* cbs)
#else
void XcodaEditorResizeComp(w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmAnyCallbackStruct *cbs;
#endif
{
  /* reset toggle button part of graph commands */
  XcodaEditorResetGraphCmd();
  coda_graph.current_action = (int)client_data;
  XDefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area), coda_graph.finger_cursor);
}

/*****************************************************************
 *    void XcodaEditorConn2Cols (w, client_data, cbs)            *
 * Description:                                                  *
 *     callback routine for connect two columns editing menu     *
 ****************************************************************/
#if defined (__STDC__)
void XcodaEditorConn2Cols (Widget w, 
			   XtPointer client_data, 
			   XmAnyCallbackStruct* cbs)
#else
void XcodaEditorConn2Cols (w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmAnyCallbackStruct *cbs;
#endif
{
  /* reset toggle button part of graph commands */
  XcodaEditorResetGraphCmd();

  coda_graph.current_action = (int)client_data;
  XDefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area), 
		coda_graph.finger_cursor);

}

/**************************************************************
 *       void scrolled(w, client_data, cbs)                   *
 * Description:                                               *
 *    scroll bar callbacks                                    *
 *************************************************************/ 
#if defined (__STDC__)
static void scrolled(Widget w, XtPointer client_data, 
		     XmScrollBarCallbackStruct* cbs)
#else
static void scrolled(w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmScrollBarCallbackStruct *cbs;
#endif
{
  int or = (int)client_data;


  if(or == XmVERTICAL){ /* vertical scroll bar */
    sw_geometry.zoomyoff = cbs->value;
  }
  else{
    sw_geometry.zoomxoff = cbs->value;
  }
  updateIndicatorPositions();
  XClearWindow(xgc.dpy, XtWindow(sw_geometry.draw_area));
  (*coda_graph.redisplay)(&coda_graph, w, cbs->event);
}

/***************************************************************
 *     void updateIndicatorPositions()                         *
 * Description:                                                *
 *     update ruler indicate positoins                         *
 **************************************************************/
#if defined (__STDC__)
static void updateIndicatorPositions (void)
#else
static void updateIndicatorPositions()
#endif
{
  Arg args[10];
  int ac = 0;
  int tic_div;

  if(sw_geometry.zoomxoff + sw_geometry.slider_size_h <
     sw_geometry.max_col){
    tic_div = sw_geometry.zoomscale*sw_geometry.cell_wd/4;
    XtSetArg(args[ac], XcodaNrealTickerDiv,tic_div); ac++;
    XtSetArg(args[ac], XcodaNminimum, sw_geometry.zoomxoff); ac++;
    XtSetValues(sw_geometry.top_ruler, args, ac);
    ac = 0;
  }
  else{
    tic_div = sw_geometry.zoomscale*sw_geometry.cell_wd/4;
    XtSetArg(args[ac], XcodaNrealTickerDiv,tic_div); ac++;    
    XtSetArg(args[ac], XcodaNminimum, sw_geometry.max_col - 
	     sw_geometry.slider_size_h); ac++;
    XtSetValues(sw_geometry.top_ruler, args, ac);
    ac = 0;
  }
  
  if(sw_geometry.zoomyoff + sw_geometry.slider_size_v <
     sw_geometry.max_row){
    tic_div = sw_geometry.zoomscale*sw_geometry.cell_ht/4;
    XtSetArg(args[ac], XcodaNrealTickerDiv,tic_div); ac++;
    XtSetArg(args[ac], XcodaNminimum, sw_geometry.zoomyoff); ac++;
    XtSetValues(sw_geometry.left_ruler, args, ac);
    ac = 0;
  }
  else{
    tic_div = sw_geometry.zoomscale*sw_geometry.cell_ht/4;
    XtSetArg(args[ac], XcodaNrealTickerDiv,tic_div); ac++;    
    XtSetArg(args[ac], XcodaNminimum, sw_geometry.max_row - 
	     sw_geometry.slider_size_v); ac++;
    XtSetValues(sw_geometry.left_ruler, args, ac);
    ac = 0;
  }
}

/***************************************************************
 *         void resize_callback()                              *
 * Description:                                                *
 *     Drawing area resize callbacks                           *
 **************************************************************/
#if defined (__STDC__)
static void resize_callback(Widget w,
			    XtPointer client_data, 
			    XmDrawingAreaCallbackStruct* cbs)
#else
static void resize_callback(w,client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmDrawingAreaCallbackStruct *cbs;
#endif
{
  reset_scrollbar_res();
  XcodaEditorBgPixmap(w);
}

/**************************************************************
 *          void expose_callback()                            *
 * Description:                                               *
 *     expose event handle routine                            *
 *************************************************************/
#if defined (__STDC__)
static void expose_callback(Widget w,
			    XtPointer client_data,
			    XmDrawingAreaCallbackStruct* cbs)
#else
     Widget w;
     XtPointer client_data;
     XmDrawingAreaCallbackStruct *cbs;
#endif
{
  (*coda_graph.redisplay)(&coda_graph, w, cbs->event);
}

/**************************************************************
 *         void reset_scrollbar_res()                         *
 * Description:                                               *
 *  Reset scrollbar and indicator resources                   *
 *************************************************************/
#if defined (__STDC__)
void reset_scrollbar_res (void)
#else
void reset_scrollbar_res()
#endif
{
  static Dimension new_wd, new_ht;
  Arg              args[10];
  int              ac = 0;

  sw_geometry.old_view_wd = sw_geometry.view_wd;
  sw_geometry.old_view_ht = sw_geometry.view_ht;
  
  XtSetArg(args[ac], XmNwidth, &new_wd); ac++;
  XtSetArg(args[ac], XmNheight, &new_ht); ac++;
  XtGetValues(sw_geometry.draw_area, args, ac);
  ac = 0;

  sw_geometry.view_wd = new_wd;
  sw_geometry.view_ht = new_ht;
  
  sw_geometry.pix_wd = new_wd/(sw_geometry.cell_wd*sw_geometry.zoomscale)*
    (sw_geometry.cell_wd*sw_geometry.zoomscale) +
      (sw_geometry.cell_wd*sw_geometry.zoomscale);

  sw_geometry.pix_ht = new_ht/(sw_geometry.cell_ht*sw_geometry.zoomscale)*
    (sw_geometry.cell_ht*sw_geometry.zoomscale) +
      (sw_geometry.cell_ht*sw_geometry.zoomscale);

  sw_geometry.slider_size_h = sw_geometry.view_wd/
    (sw_geometry.cell_wd*sw_geometry.zoomscale);
  sw_geometry.slider_size_v = sw_geometry.view_ht/
    (sw_geometry.cell_ht*sw_geometry.zoomscale);    
  
  sw_geometry.pageinc_h = sw_geometry.slider_size_h - 1;
  sw_geometry.pageinc_v = sw_geometry.slider_size_v - 1;

/* taking care the case when drawing area is large enough 
 * to hold more than 100 when scrolled to end positions
 */
  if(sw_geometry.zoomxoff + sw_geometry.slider_size_h >=
     sw_geometry.max_col){
    sw_geometry.zoomxoff = sw_geometry.max_col - sw_geometry.slider_size_h;
    XtSetArg(args[ac], XmNvalue, sw_geometry.zoomxoff); ac++;
    XtSetValues(sw_geometry.h_scb, args, ac);
    ac = 0;
  }

  if(sw_geometry.zoomyoff + sw_geometry.slider_size_v >=
     sw_geometry.max_row){
    sw_geometry.zoomyoff = sw_geometry.max_row - sw_geometry.slider_size_v;
    XtSetArg(args[ac], XmNvalue, sw_geometry.zoomyoff); ac++;
    XtSetValues(sw_geometry.v_scb, args, ac);
    ac = 0;
  }
  
  XtSetArg(args[ac], XmNincrement, 1); ac++;
  XtSetArg(args[ac], XmNsliderSize, (int)sw_geometry.slider_size_h); ac++;
  XtSetArg(args[ac], XmNpageIncrement, (int)sw_geometry.pageinc_h); ac++;
  XtSetArg(args[ac], XmNmaximum,   sw_geometry.max_col); ac++;
  XtSetArg(args[ac], XmNminimum,   sw_geometry.min_col); ac++;
  XtSetValues(sw_geometry.h_scb, args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNincrement, 1); ac++;
  XtSetArg(args[ac], XmNsliderSize, (int)sw_geometry.slider_size_v); ac++;
  XtSetArg(args[ac], XmNpageIncrement, (int)sw_geometry.pageinc_v); ac++;
  XtSetArg(args[ac], XmNmaximum,   sw_geometry.max_row); ac++;
  XtSetArg(args[ac], XmNminimum,   sw_geometry.min_row); ac++;
  XtSetValues(sw_geometry.v_scb, args, ac);
  ac = 0;

  updateIndicatorPositions();
}

/*************************************************************************
 *            void startAddNode()                                        *
 * Description:                                                          *
 *      start add new component to graph drawing area                    *
 *************************************************************************/
#if defined (__STDC__)
static void StartAddNode(XcodaEditorGraph* graph,
			 Widget w,
			 XEvent* event)
#else
static void StartAddNode(graph,w,event)
     Widget           w;
     XcodaEditorGraph *graph;
     XEvent           *event;
#endif
{
  drawComp *q;

  q = createDrawComp (graph, graph->current_action);
  setGraphStartingPosition (graph, event->xbutton.x, event->xbutton.y);
  (*q->draw_rubber_comp)(q, xgc.dpy, XtWindow(sw_geometry.draw_area),
			 graph->start_x,
			 graph->start_y, sw_geometry.cell_wd*sw_geometry.zoomscale,
			 sw_geometry.cell_ht*sw_geometry.zoomscale);
}


/*************************************************************************
 *            void StartAddExistingNode()                                *
 * Description:                                                          *
 *      start add existing component to graph drawing area               *
 *************************************************************************/
#if defined (__STDC__)
static void StartAddExistingNode(XcodaEditorGraph* graph,
				 Widget w,
				 XEvent* event)
#else
static void StartAddExistingNode(graph,w,event)
     Widget           w;
     XcodaEditorGraph *graph;
     XEvent           *event;
#endif
{
  drawComp *q = graph->current_comp;
  int      i;
  ipPort   *port;

  graph->start_x = event->xbutton.x;
  graph->start_y = event->xbutton.y;
  q->selected = 0;
  (*q->draw_rubber_comp)(q, xgc.dpy, XtWindow(sw_geometry.draw_area),
			 graph->start_x,
			 graph->start_y, sw_geometry.cell_wd*sw_geometry.zoomscale,
			 sw_geometry.cell_ht*sw_geometry.zoomscale);
}

/**********************************************************************************
 *             static void StartMovingNode(graph, w, event)                       *
 * Description:                                                                   *
 *      start moving a node                                                       *
 *********************************************************************************/
#if defined (__STDC__)
static void StartMovingNode(XcodaEditorGraph* graph, 
			    Widget w, 
			    XEvent* event)
#else
static void StartMovingNode(graph, w, event)
     XcodaEditorGraph *graph;
     Widget w;
     XEvent *event;
#endif
{
  drawComp *p;
  int      xx, yy;
  int      i;
  ipPort   *port;
  
  p = graph->current_comp;

  /*XGrabPointer (xgc.dpy, XtWindow (sw_geometry.draw_area), False,
		PointerMotionMask |ButtonReleaseMask,
		GrabModeAsync, GrabModeAsync,
		XtWindow (sw_geometry.draw_area), None, CurrentTime);*/
  for(i=0;i< p->num_ports; i++){
    port = &(p->ip_port[i]);
    (*port->erase_name)(port, xgc.dpy, XtWindow(sw_geometry.draw_area));
  }
  (*p->erase)(p, xgc.dpy, XtWindow(sw_geometry.draw_area));
  (*p->erase_name)(p, xgc.dpy, XtWindow(sw_geometry.draw_area));
  xx = (event->xbutton.x)/(sw_geometry.cell_wd*sw_geometry.zoomscale)*
    sw_geometry.cell_wd*sw_geometry.zoomscale;
  yy = (event->xbutton.y)/(sw_geometry.cell_ht*sw_geometry.zoomscale)*
    sw_geometry.cell_ht*sw_geometry.zoomscale;
  graph->start_x = xx;
  graph->start_y = yy;
  
  /*  XWarpPointer(xgc.dpy, XtWindow(sw_geometry.draw_area),XtWindow(sw_geometry.draw_area),
      event->xbutton.x, event->xbutton.y,0,0,xx,yy);*/
  
  (*p->erase)(p,xgc.dpy,XtWindow(sw_geometry.draw_area));

  (*p->draw_rubber_comp)(p, xgc.dpy, XtWindow(sw_geometry.draw_area),
			 graph->start_x,
			 graph->start_y, sw_geometry.cell_wd*sw_geometry.zoomscale,
			 sw_geometry.cell_ht*sw_geometry.zoomscale);
}

/**********************************************************************************
 *             static void StartMovingArc(graph, w, event)                        *
 * Description:                                                                   *
 *      start moving an arc left or right                                         *
 *********************************************************************************/
#if defined (__STDC__)
static void StartMovingArc(XcodaEditorGraph* graph, 
			   Widget w, 
			   XEvent* event)
#else
static void StartMovingArc(graph, w, event)
     XcodaEditorGraph *graph;
     Widget w;
     XEvent *event;
#endif
{
  Arc      *arc, **from_arcs, **to_arcs,*arcs[200];
  ipPort   *from_port, *to_port;
  int      xx, yy, num_to_arcs, num_from_arcs, num_arcs;
  int      i, j, k;
  ipPort   *port;
  
  arc = graph->current_arc;

  XGrabButton(xgc.dpy,AnyButton, AnyModifier,XtWindow(sw_geometry.draw_area),
	      False,Button2MotionMask|ButtonPressMask|ButtonReleaseMask,
	      GrabModeAsync,GrabModeAsync,
	      XtWindow(sw_geometry.draw_area),
	      graph->delete_arc_cursor);
  XDefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area),graph->delete_arc_cursor);

   XClearWindow(xgc.dpy, XtWindow(sw_geometry.draw_area));
  (*graph->redisplay)(graph, w, event);
  (*arc->erase)(arc,xgc.dpy, XtWindow(sw_geometry.draw_area));
  from_port = arc->from;
  to_port = arc->to;

  k = 0;
  from_arcs = XcodaEditorGetFromArcs(graph,from_port,&num_from_arcs);
  for(j=0;j<num_from_arcs;j++)
    arcs[k++] = from_arcs[j];
  to_arcs = XcodaEditorGetToArcs(graph,from_port,&num_to_arcs);
  for(j=0;j<num_to_arcs;j++)
    arcs[k++] = to_arcs[j];
  num_arcs = k;
  for(i=0;i<num_arcs;i++){
    if(arcs[i] != arc)
      (*arcs[i]->draw_original)(arcs[i], xgc.dpy, XtWindow(sw_geometry.draw_area));
  }
  if (num_from_arcs)
    free(from_arcs);
  if (num_to_arcs)
    free(to_arcs);

  k = 0;
  from_arcs = XcodaEditorGetFromArcs(graph,to_port,&num_from_arcs);
  for(j=0;j<num_from_arcs;j++)
    arcs[k++] = from_arcs[j];
  to_arcs = XcodaEditorGetToArcs(graph,to_port,&num_to_arcs);
  for(j=0;j<num_to_arcs;j++)
    arcs[k++] = to_arcs[j];
  num_arcs = k;
  for(i=0;i<num_arcs;i++){
    if(arcs[i] != arc)
      (*arcs[i]->draw_original)(arcs[i], xgc.dpy, XtWindow(sw_geometry.draw_area));
  }
  if (num_from_arcs)
    free(from_arcs);
  if (num_to_arcs)
    free(to_arcs);
  

  graph->start_x = event->xbutton.x;
  graph->start_y = event->xbutton.y;

  (*arc->draw_rubber_arc)(arc, xgc.dpy, XtWindow(sw_geometry.draw_area),
			 graph->start_x,graph->start_y);
}


/**********************************************************************************
 *            static void StartConnectingPorts(graph, w, event)                   *
 * Description:                                                                   *
 *    Middle mouse button pressed on a port to start connecting this port to      *
 *    Middel button up pointed port                                               *
 *********************************************************************************/
#if defined (__STDC__)
static void StartConnectingPorts(XcodaEditorGraph* graph, 
				 Widget w, 
				 XEvent* event)
#else
static void StartConnectingPorts(graph, w, event)
     XcodaEditorGraph *graph;
     Widget           w;
     XEvent           *event;
#endif
{
  
  graph->start_x = graph->end_x = event->xbutton.x;
  graph->start_y = graph->end_y = event->xbutton.y;
  XGrabButton(xgc.dpy,AnyButton, AnyModifier,XtWindow(sw_geometry.draw_area),
	      False,Button2MotionMask|ButtonPressMask|ButtonReleaseMask,
	      GrabModeAsync,GrabModeAsync,
	      XtWindow(sw_geometry.draw_area),
	      graph->delete_arc_cursor);
  XDefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area),graph->delete_arc_cursor);
  XSetForeground(xgc.dpy, xgc.xor_gc, xgc.fg^xgc.bg);
  XSetLineAttributes(xgc.dpy,xgc.xor_gc,1,LineOnOffDash,CapButt,JoinMiter);
}


/**********************************************************************************
 *        static void HandleMotion(graph, w, event)                               *
 * Description:                                                                   *
 *    Middle button motion handler                                                *
 *********************************************************************************/
#if defined (__STDC__)
static void HandleMotion(XcodaEditorGraph* graph, 
			 Widget w, 
			 XEvent* event)
#else
static void HandleMotion(graph, w, event)
     XcodaEditorGraph *graph;
     Widget           w;
     XEvent           *event;
#endif
{
  if(graph->current_action == CONNECT_IP_PORTS_ACTION ||
     graph->current_action == PERS_CONNECT_IP_PORTS_ACTION){
    XDrawLine(xgc.dpy, XtWindow(sw_geometry.draw_area),
	      xgc.xor_gc,
	      graph->start_x, graph->start_y,
	      graph->end_x, graph->end_y);
    graph->end_x = event->xbutton.x;
    graph->end_y = event->xbutton.y;

    XDrawLine(xgc.dpy, XtWindow(sw_geometry.draw_area),
	      xgc.xor_gc,
	      graph->start_x, graph->start_y,
	      graph->end_x, graph->end_y);  
  }
}

/**********************************************************************************
 *        static void HandleDoubleClick(graph, event)                             *
 * Description:                                                                   *
 *    Double Click action routine                                                 *
 *********************************************************************************/
#if defined (__STDC__)
static void HandleDoubleClick(XcodaEditorGraph* graph, 
			      Widget w, 
			      XEvent* event)
#else
static void HandleDoubleClick(graph, w, event)
     XcodaEditorGraph *graph;
     Widget           w;
     XEvent           *event;
#endif
{
  ipPort *port;
  drawComp *comp;

  if((port = XcodaEditorClickedPort(graph, event)) != NULL)
    (*port->set_attributes)(port, w, event);
  else if((comp = XcodaEditorClickedNode(graph, event)) != NULL)
    (*comp->set_attributes)(comp, w, event, 1);
}

/**********************************************************************************
 *            static void PositionNewNode(graph, w, event)                        *
 * Description:                                                                   *
 *     Pointer motion to show movable node                                        *
 *********************************************************************************/
#if defined (__STDC__)
static void PositionNewNode(XcodaEditorGraph* graph, 
			    Widget w, 
			    XEvent* event)
#else
static void PositionNewNode(graph, w, event)
     Widget           w;
     XcodaEditorGraph *graph;
     XEvent           *event;
#endif
{
  drawComp *q;
  int      i, xx, yy;
  ipPort   *port;

  xx = (event->xbutton.x)/(sw_geometry.cell_wd*sw_geometry.zoomscale)*
    sw_geometry.cell_wd*sw_geometry.zoomscale;
  yy = (event->xbutton.y)/(sw_geometry.cell_ht*sw_geometry.zoomscale)*
    sw_geometry.cell_ht*sw_geometry.zoomscale;
  if (xx == graph->start_x && yy == graph->start_y) /* pointer stays in the same cell */
    return;

 /* erase old drawing */
  q = graph->current_comp;
  XSetForeground(xgc.dpy, xgc.r_gc, xgc.bg);
  XFillRectangle(xgc.dpy,XtWindow(sw_geometry.draw_area) , xgc.r_gc,graph->start_x,
			 graph->start_y, sw_geometry.cell_wd*sw_geometry.zoomscale,
			 sw_geometry.cell_ht*sw_geometry.zoomscale);
  {
    drawComp *comp;
    compList *p2;
    
    for(p2 = coda_graph.comp_list_head->next; p2 != coda_graph.comp_list_tail;p2=p2->next){
      comp = p2->draw_comp;
      if (q != comp) {
	if ((graph->start_x == comp->x) && (graph->start_y == comp->y)) {
	  draw_original(comp,
			xgc.dpy,
			XtWindow(sw_geometry.draw_area),
			graph->start_x,
			graph->start_y,
			comp->width,
			comp->height,
			comp->row,
			comp->col);
	}
      } 
    }
  }

  /* update to a new position */
  graph->start_x = xx;
  graph->start_y = yy;

  /* draw again */

  XClearWindow(xgc.dpy, XtWindow(sw_geometry.draw_area));
  (*graph->redisplay)(graph, w, event);

  (*q->draw_rubber_comp)(q,xgc.dpy, XtWindow(sw_geometry.draw_area), graph->start_x,
			 graph->start_y, sw_geometry.cell_wd*sw_geometry.zoomscale,
			 sw_geometry.cell_ht*sw_geometry.zoomscale);
	       
}

/**********************************************************************************
 *            static void PositionNewArc(graph, w, event)                         *
 * Description:                                                                   *
 *     Pointer motion to show movable arc                                         *
 *********************************************************************************/
#if defined (__STDC__)
static void PositionNewArc(XcodaEditorGraph* graph, 
			   Widget w, 
			   XEvent* event)
#else
static void PositionNewArc(graph, w, event)
     Widget           w;
     XcodaEditorGraph *graph;
     XEvent           *event;
#endif
{
  Arc      *arc;
  int      i;

  arc = graph->current_arc;

  (*arc->draw_rubber_arc)(arc, xgc.dpy, XtWindow(sw_geometry.draw_area), graph->start_x,
			 graph->start_y);
  graph->start_x = event->xbutton.x;
  graph->start_y = event->xbutton.y;
  (*arc->draw_rubber_arc)(arc,xgc.dpy, XtWindow(sw_geometry.draw_area), graph->start_x,
			 graph->start_y);
}

/*********************************************************************************
 *          static void EndAddNode(graph,w, event)                               *
 * Description:                                                                  *
 *     Finish porcess of adding new component                                    *
 ********************************************************************************/
#if defined (__STDC__)
static void EndAddNode(XcodaEditorGraph* graph, 
		       Widget w, 
		       XEvent* event)
#else
static void EndAddNode(graph, w, event)
     Widget w;
     XcodaEditorGraph *graph;
     XEvent *event;
#endif
{
  drawComp *q, *qq;
  int  xx, yy;
  Dimension  wd, ht;
  int  abs_col, abs_row;
  int  row, col;
  int  i;
  ipPort *port;

/* ungrab pointer */
  XUngrabPointer (xgc.dpy, CurrentTime); 

  q = graph->current_comp;

  /*erase last rubber drawing */

  XSetForeground(xgc.dpy, xgc.r_gc, xgc.bg);
  XFillRectangle(xgc.dpy,XtWindow(sw_geometry.draw_area) , xgc.r_gc,graph->start_x,
			 graph->start_y, sw_geometry.cell_wd*sw_geometry.zoomscale,
			 sw_geometry.cell_ht*sw_geometry.zoomscale);
  {
    drawComp *comp;
    compList *p2;
    
    for(p2 = coda_graph.comp_list_head->next; p2 != coda_graph.comp_list_tail;p2=p2->next){
      comp = p2->draw_comp;
      if (q != comp) {
	if ((graph->start_x == comp->x) && (graph->start_y == comp->y)) {
	  draw_original(comp,
			xgc.dpy,
			XtWindow(sw_geometry.draw_area),
			graph->start_x,
			graph->start_y,
			comp->width,
			comp->height,
			comp->row,
			comp->col);
	}
      } 
    }
  }

  /*(*q->draw_rubber_comp)(q,xgc.dpy, XtWindow(sw_geometry.draw_area), graph->start_x,
			 graph->start_y, sw_geometry.cell_wd*sw_geometry.zoomscale,
			 sw_geometry.cell_ht*sw_geometry.zoomscale); */
  /* draw this component in regular gc */
  /* top left corner coords */
  xx = (graph->start_x)/(sw_geometry.cell_wd*sw_geometry.zoomscale)*
    (sw_geometry.cell_wd*sw_geometry.zoomscale);
  yy = (graph->start_y)/(sw_geometry.cell_ht*sw_geometry.zoomscale)*
    (sw_geometry.cell_ht*sw_geometry.zoomscale);
  abs_col = (graph->start_x)/(sw_geometry.cell_wd*sw_geometry.zoomscale) + 1;
  abs_row = (graph->start_y)/(sw_geometry.cell_ht*sw_geometry.zoomscale) + 1;
  col = abs_col + sw_geometry.zoomxoff;
  row = abs_row + sw_geometry.zoomyoff;
  wd = sw_geometry.cell_wd*sw_geometry.zoomscale;
  ht = sw_geometry.cell_ht*sw_geometry.zoomscale;

  if(isOverlapAny(graph, q,col, row)){ /* check whther this one stack onto another */
    XBell(xgc.dpy, 0);
    free(q);
    graph->current_action = NORMAL_ACTION;
    return;
  }

/* create new node and add to comp_list */
  qq = newDrawComp (q, xx, yy, col, row, wd, ht);
  insertNodeToCompList(graph, qq);
  /* daq resources are empty */
  free(q);

/*init ip_port lists*/
  for(i=0;i<MAX_NUM_PORTS; i++){
    port = &(qq->ip_port[i]);
    port->index = i;
    if(i == 0){
      port->left_x = xx;
      port->left_y = yy + 3*ht/8.0;
      port->right_x = xx + wd - wd/8.0;
      port->right_y = port->left_y;
      port->row = row;
      port->col = col;
      port->width = wd/8.0;
      port->height = ht/4.0;
    }
    port->left_selected = 0;
    port->right_selected = 0;
    port->ip_addr = (char *)NULL;
    port->comp_ptr = &(qq->comp);
    port->arm = arm_ip_port;
    port->disarm = disarm_ip_port;
    port->input_high_light = high_light_input_port;
    port->output_high_light = high_light_output_port;
    port->set_attributes = popup_ipport_attributes;
    port->erase_name = erase_port_name;
    port->write_name = draw_port_name;
    port->move_name = draw_rubber_name;
  }

/* draw draw_component */
  XClearWindow(xgc.dpy, XtWindow(sw_geometry.draw_area));
  (*graph->redisplay)(graph, w, event);

  (*qq->draw_original)(qq,xgc.dpy, XtWindow(sw_geometry.draw_area), xx, yy, 
		      sw_geometry.cell_wd*sw_geometry.zoomscale, 
		      sw_geometry.cell_ht*sw_geometry.zoomscale,
		      row, col);
  graph->current_action = NORMAL_ACTION;
  (*qq->set_attributes)(qq,sw_geometry.draw_area,event,0);
	       
}

/*********************************************************************************
 *          static void EndAddExistingNode(graph,w, event)                       *
 * Description:                                                                  *
 *     Finish porcess of adding existing component                               *
 ********************************************************************************/
#if defined (__STDC__)
static void EndAddExistingNode(XcodaEditorGraph* graph, 
			       Widget w, 
			       XEvent* event)
#else
static void EndAddExistingNode(graph, w, event)
     Widget w;
     XcodaEditorGraph *graph;
     XEvent *event;
#endif
{
  drawComp *q;
  int  xx, yy;
  int  wd, ht;
  int  abs_col, abs_row;
  int  row, col;
  int  i;
  ipPort *port;

/* ungrab pointer */
  XUngrabPointer (xgc.dpy, CurrentTime);

  q = graph->current_comp;

  /*erase last rubber drawing */
  (*q->draw_rubber_comp)(q,xgc.dpy, XtWindow(sw_geometry.draw_area), graph->start_x,
			 graph->start_y, sw_geometry.cell_wd*sw_geometry.zoomscale,
			 sw_geometry.cell_ht*sw_geometry.zoomscale);

  /* draw this component in regular gc */
  /* top left corner coords */
  xx = (graph->start_x)/(sw_geometry.cell_wd*sw_geometry.zoomscale)*
    (sw_geometry.cell_wd*sw_geometry.zoomscale);
  yy = (graph->start_y)/(sw_geometry.cell_ht*sw_geometry.zoomscale)*
    (sw_geometry.cell_ht*sw_geometry.zoomscale);
  abs_col = (graph->start_x)/(sw_geometry.cell_wd*sw_geometry.zoomscale) + 1;
  abs_row = (graph->start_y)/(sw_geometry.cell_ht*sw_geometry.zoomscale) + 1;
  col = abs_col + sw_geometry.zoomxoff;
  row = abs_row + sw_geometry.zoomyoff;
  wd = sw_geometry.cell_wd*sw_geometry.zoomscale;
  ht = sw_geometry.cell_ht*sw_geometry.zoomscale;

  if(isOverlapAny(graph,q, col, row)){ /* check whther this one stack onto another */
    XBell(xgc.dpy, 0);
    free(q);
    graph->current_action = NORMAL_ACTION;
    graph->current_comp = (drawComp *)NULL;
    return;
  }
  
  q->x = xx;
  q->y = yy;
  q->col = col;
  q->row = row;
  insertNodeToCompList(graph, q);

/*init ip_port lists*/
  for(i=0;i<MAX_NUM_PORTS; i++){
    port = &(q->ip_port[i]);
    port->left_x = q->x;
    port->left_y = q->y + i*q->height + 3*q->height/8.0;
    port->right_x = q->x + 7*q->width/8.0;
    port->right_y = port->left_y;
    port->row = row + i;
    port->col = col;
  }

/* draw draw_component */
  XClearWindow(xgc.dpy, XtWindow(sw_geometry.draw_area));
  (*graph->redisplay)(graph, w, event);	       
  (*q->draw_original)(q,xgc.dpy, XtWindow(sw_geometry.draw_area), xx, yy, 
		      sw_geometry.cell_wd*sw_geometry.zoomscale, 
		      sw_geometry.cell_ht*sw_geometry.zoomscale,
		      row, col);

  graph->current_action = NORMAL_ACTION;
  graph->current_comp = (drawComp *)NULL;
}


/**************************************************************************
 *         static void EndMovingNode(graph, w, event)                     *
 * Description:                                                           *
 *     end moving a node process                                          *
 *************************************************************************/
#if defined (__STDC__)
static void EndMovingNode(XcodaEditorGraph* graph,
			  Widget w, 
			  XEvent* event)
#else
static void EndMovingNode(graph,w, event)
     XcodaEditorGraph *graph;
     Widget           w;
     XEvent           *event;
#endif
{
  int col, row;
  int xx,  yy;
  int wd, ht;
  int abs_col, abs_row;
  drawComp *q;
  Arc **from_arcs,**to_arcs;
  int num_from_arcs,num_to_arcs;
  Arc *arcs[200];
  int num_arcs;
  int i, j, k;
  ipPort *port;

  XUngrabPointer (xgc.dpy, CurrentTime);   
  if (graph->current_action == NORMAL_ACTION)
    XUndefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area));

  q = graph->current_comp;

  /* erase last rubber drawing */
  (*q->erase)(q,xgc.dpy,XtWindow(sw_geometry.draw_area));

  /* top left coords */
  xx = (graph->start_x)/(sw_geometry.cell_wd*sw_geometry.zoomscale)*
    (sw_geometry.cell_wd*sw_geometry.zoomscale);
  yy = (graph->start_y)/(sw_geometry.cell_ht*sw_geometry.zoomscale)*
    (sw_geometry.cell_ht*sw_geometry.zoomscale);
  abs_col = (graph->start_x)/(sw_geometry.cell_wd*sw_geometry.zoomscale) + 1;
  abs_row = (graph->start_y)/(sw_geometry.cell_ht*sw_geometry.zoomscale) + 1;
  col = abs_col + sw_geometry.zoomxoff;
  row = abs_row + sw_geometry.zoomyoff;
  wd = sw_geometry.cell_wd*sw_geometry.zoomscale;
  ht = sw_geometry.cell_ht*sw_geometry.zoomscale;      

  if(isOverlapAny(graph,q, col, row)){ /* check whther this one stack onto another */
    XBell(xgc.dpy, 0);
    /* redraw at old position */
    if(q->selected)
      (*q->high_light)(q,xgc.dpy,XtWindow(sw_geometry.draw_area));
    else
      (*q->unhigh_light)(q,xgc.dpy,XtWindow(sw_geometry.draw_area));
    return;
  }      
  /* first erase old arc, then draw new arc */
  k = 0;
  for(i=0;i<q->num_ports;i++){
    from_arcs = XcodaEditorGetFromArcs(graph,&(q->ip_port[i]),&num_from_arcs);
    for(j=0;j<num_from_arcs;j++)
      arcs[k++] = from_arcs[j];
    to_arcs = XcodaEditorGetToArcs(graph,&(q->ip_port[i]),&num_to_arcs);
    for(j=0;j<num_to_arcs;j++)
      arcs[k++] = to_arcs[j];
  }
  num_arcs = k;
  for(i=0;i<num_arcs;i++){
    (*arcs[i]->erase)(arcs[i], xgc.dpy, XtWindow(sw_geometry.draw_area));
  }
  if (num_from_arcs)
    free(from_arcs);
  if (num_to_arcs)
    free(to_arcs);
  /* update all informations */
  q->x = xx;
  q->y = yy;
  q->width = wd;
  q->height = ht;
  q->row = row;
  q->col = col;
  for(i=0;i<q->num_ports;i++){
    port = &(q->ip_port[i]);
    port->left_x = xx;
    port->left_y = yy + 3*ht/8.0 + i*ht;
    port->right_x = xx + wd - wd/8.0;
    port->right_y = port->left_y;
    port->width = wd/8.0;
    port->height = ht/4.0;
    port->row = row;
    port->col = col;
  }
  for(i=0;i<num_arcs;i++){
    update_arc_geometry(arcs[i],arcs[i]->from, arcs[i]->to);
    if(arcs[i]->selected)
      (*arcs[i]->high_light)(arcs[i],xgc.dpy,XtWindow(sw_geometry.draw_area));
    else
      (*arcs[i]->draw_original)(arcs[i],xgc.dpy,XtWindow(sw_geometry.draw_area));
  }
  if(q->selected)
    (*q->high_light)(q,xgc.dpy,XtWindow(sw_geometry.draw_area));
  else
    (*q->unhigh_light)(q,xgc.dpy,XtWindow(sw_geometry.draw_area));  
  XClearWindow(xgc.dpy, XtWindow(sw_geometry.draw_area));
  (*graph->redisplay)(graph, w, event);
	       
}

/**************************************************************************
 *         void SplitEbana(drawComp *comp)                                *
 * Description:                                                           *
 *     Split ebana component into one eb and one ana                      *
 *************************************************************************/
#if defined (__STDC__)
void SplitEbana (drawComp* comp)
#else
void SplitEbana (comp)
     drawComp *comp;
#endif
{
  int      i;
  drawComp *eb, *ana;
  ipPort   *port, *old_port;
  Arc      *new_arc;


}  

/**************************************************************************
 *         static void EndMovingArc(graph, w, event)                      *
 * Description:                                                           *
 *     end moving an arc process                                          *
 *************************************************************************/
#if defined (__STDC__)
static void EndMovingArc(XcodaEditorGraph* graph,
			 Widget w, 
			 XEvent* event)
#else
static void EndMovingArc(graph,w, event)
     XcodaEditorGraph *graph;
     Widget           w;
     XEvent           *event;
#endif
{
  int xmin, xmax;
  int i;
  Arc *arc;

  XUngrabButton(xgc.dpy,AnyButton,AnyModifier,XtWindow(sw_geometry.draw_area));
  XUndefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area));

  arc = graph->current_arc;
 
  /*erase last rubber drawing */
  (*arc->draw_rubber_arc)(arc, xgc.dpy, XtWindow(sw_geometry.draw_area), graph->start_x,
			 graph->start_y);
  reassign_arc_geometry(arc, graph->start_x, graph->start_y);
  if(arc->selected)
    (*arc->high_light)(arc, xgc.dpy, XtWindow(sw_geometry.draw_area));    
  else
    (*arc->draw_original)(arc, xgc.dpy, XtWindow(sw_geometry.draw_area));
  graph->current_action = NORMAL_ACTION;
  graph->current_arc = (Arc *)NULL;
}


/**********************************************************************
 *      static void EndConnectingPorts(graph, w, event)               *
 * Description:                                                       *
 *     End Connecting ports                                           *
 *********************************************************************/
#if defined (__STDC__)
static void EndConnectingPorts(XcodaEditorGraph* graph, 
			       Widget w, 
			       XEvent* event)
#else
static void EndConnectingPorts(graph, w, event)
     XcodaEditorGraph *graph;
     Widget           w;
     XEvent           *event;
#endif
{
  ipPort *to_port, *from_port;
  drawComp *to_node, *from_node;
  Arc    *ret_arc, *new_arc;

  XUngrabButton(xgc.dpy,AnyButton,AnyModifier,XtWindow(sw_geometry.draw_area));  

  XDrawLine(xgc.dpy, XtWindow(sw_geometry.draw_area),
	    xgc.xor_gc,
	    graph->start_x, graph->start_y,
	    graph->end_x, graph->end_y);
  
  graph->start_x = graph->end_x = 0;
  graph->start_y = graph->end_y = 0;
  
  from_port = graph->from_port;
  to_port = XcodaEditorClickedPort(graph, event);
  if(to_port != NULL && to_port != from_port && 
     to_port->comp_ptr != from_port->comp_ptr && (
     from_port->comp_ptr->type < 11 || from_port->comp_ptr->type == CODA_MON)){
    to_node = findDrawCompFromPort(graph,to_port);
    from_node = findDrawCompFromPort(graph, from_port);
    /*
    if((ret_arc = XcodaEditorGetArcBetweenNodes(graph, to_node, from_node)) 
       == NULL){ */
    new_arc = newArcConnectingPorts (from_port, to_port);

    update_arc_geometry(new_arc, from_port, to_port);

    (*new_arc->draw_original)(new_arc, xgc.dpy, XtWindow(sw_geometry.draw_area));

    if (from_port->comp_ptr->type == CODA_ER && (to_port->comp_ptr->type == CODA_FILE ||to_port->comp_ptr->type == CODA_CODAFILE)) {
      char *config;
      from_port->comp_ptr->code[0] = strdup(to_port->comp_ptr->code[1]);
      config = currentConfigTable();

      if (config)
	insertValToOptionTable (config,"dataFile",to_port->comp_ptr->code[0]);
    }
    insertArcToWholeArcList(graph, new_arc);

    if(from_port->ip_addr == NULL)
      from_port->ip_addr = strsave (from_port->comp_ptr->node_name);
    if(to_port->ip_addr == NULL){/*ports may have no name */
      to_port->ip_addr = strsave (to_port->comp_ptr->node_name);
    }
  } else {
    XBell(xgc.dpy, 0);
  }

  /* This code just showed up in my editor when I was making a bunch of changes.*/
  /* Saving for a while just in case it is useful. Nov 97 RWM. */
  /*   from_port->comp_ptr->code[0] = strdup(to_port->comp_ptr->code[1]); */
  /*   config = currentCBell(xgc.dpy, 0); */
  graph->from_port = (ipPort *)NULL;
}

/**********************************************************************
 *        static void resize_comp(graph, w, event)                    *
 * Description:                                                       *
 *     Use left button to select a component to resize                *
 *********************************************************************/
#if defined (__STDC__)
static void resize_comp(XcodaEditorGraph* graph, 
			Widget w, 
			XEvent* event)
#else
static void resize_comp(graph, w, event)
     XcodaEditorGraph *graph;
     Widget           w;
     XEvent           *event;
#endif
{
  drawComp *p;

  if((p = XcodaEditorClickedNode(graph, event)) == NULL){
    graph->current_action = NORMAL_ACTION;
    XUndefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area));
  }
  else{
    graph->current_action = NORMAL_ACTION;
    XUndefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area));
    popup_resize_selection(graph, p, sw_geometry.draw_area);
  }
}

/**********************************************************************
 *        static void connect_2cols(graph, w, event)                  *
 * Description:                                                       *
 *     Use left button to select two columns to connect them all      *
 *********************************************************************/
#if defined (__STDC__)
static void connect_2cols (XcodaEditorGraph* graph, 
			   Widget w, 
			   XEvent* event)
#else
static void connect_2cols (graph, w, event)
     XcodaEditorGraph *graph;
     Widget           w;
     XEvent           *event;
#endif
{
  static   int firstCol = -1;
  static   int secondCol = -1;

  drawComp *p;

  if((p = XcodaEditorClickedNode(graph, event)) == NULL){
    graph->current_action = NORMAL_ACTION;
    XUndefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area));
    firstCol = -1;
    secondCol = -1;
    XcodaEditorEnableEditing ();
    graph->previous_action = NORMAL_ACTION;
  }
  else{
    if (firstCol == -1) {
      firstCol = p->col;
      XcodaEditorDisableEditing ();
    }
    else {
      secondCol = p->col;
      XUndefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area));
      connect_2cols_action (graph, firstCol, secondCol);
      firstCol = -1;
      secondCol = -1;
      XcodaEditorEnableEditing ();
    }
  }
}

/**********************************************************************
 *        static void connect_2cols_action (graph, first, second)     *
 * Description:                                                       *
 *     connect all components between two columns                     *
 *********************************************************************/
#if defined (__STDC__)
static void connect_2cols_action (XcodaEditorGraph* graph, 
				  int first, int second)
#else
static void connect_2cols_action (graph, first, second)
     XcodaEditorGraph* graph;
     int first;
     int second;
#endif
{
  static   Arc* arcs[200];
  static   int numArcs = 0;
  compList *q, *p;
  int      left, right, i;
  Arc      *arc;

  if (graph->current_action == CONN_2COLS_ACTION) {
    for (i = 0; i < numArcs; i++)
      free (arcs[i]);
    numArcs = 0;
    
    if (first == second) {
      graph->previous_action = NORMAL_ACTION;
      graph->current_action = NORMAL_ACTION;
      return;
    }

    left = first;
    right =second;

    /* find out whether there is at least one input is codadebug/codafile */
    for(q=graph->comp_list_head->next; q != graph->comp_list_tail; q = q->next)
      if(left == q->draw_comp->col) {
	if (q->draw_comp->comp.type >= 11) {
	  graph->previous_action = NORMAL_ACTION;
	  graph->current_action = NORMAL_ACTION;
	  XBell(xgc.dpy, 0);
	  return;
	}
      }
    
    /* go through each pair to connect all */
    i = 0;
    for(q=graph->comp_list_head->next; q != graph->comp_list_tail; q = q->next){
      if(left == q->draw_comp->col) 
	for(p =graph->comp_list_head->next; p != graph->comp_list_tail; 
	    p = p->next) {
	  if (right == p->draw_comp->col) 
	    if ((arc = XcodaEditorGetArcBetweenNodes 
		 (graph, q->draw_comp, p->draw_comp)) == NULL) {
	      arc = newArcConnectingComps (q->draw_comp, p->draw_comp);
	      update_arc_geometry(arc, arc->from, arc->to);
	      insertArcToWholeArcList(graph, arc);
	      /* draw this arc */
	      (*arc->draw_original)(arc, xgc.dpy, 
				    XtWindow(sw_geometry.draw_area));
	      arcs[i++] = arc;
	    }
	}
    }
    numArcs = i;
    graph->current_action = NORMAL_ACTION;
    coda_graph.previous_action = CONN_2COLS_ACTION;
  }
  else { /* undo action */
    for (i = 0; i < numArcs; i++) {
      removeArcFromWholeArcList (graph, arcs[i]);
      (*arcs[i]->erase)(arcs[i], xgc.dpy, XtWindow(sw_geometry.draw_area));
    }
    /* denote all arcs have been deleted */
    numArcs = 0;
  }
}
	    

/**********************************************************************
 *        static int insideExposedRegion(drawComp, XEvent *)          *
 * Description:                                                       *
 *     Check whether a node is inside the exposed region              *
 *     Using simplified CohanSutherLand Alogrithm                     *
 *********************************************************************/
static short BOTTOM = 0x1000;
static short TOP = 0x0100;
static short LEFT = 0x0001;
static short RIGHT = 0x0010;

#if defined (__STDC__)
static short compOutCode(int x, int y, int x0, int y0, int x1, int y1)
#else
static short compOutCode(x, y, x0, y0, x1, y1)
int x, y;
int x0, y0, x1, y1;
#endif
{
  short code = 0x0000;

  if (y > y1)
    code = code | BOTTOM;
  else if (y < y0)
    code = code | TOP;
  if (x > x1)
    code = code | RIGHT;
  else if (x < x0)
    code = code | LEFT;
  
  return code;
}

#if defined (__STDC__)
static int insideExposedRegion(drawComp* p, XEvent* event)
#else
static int insideExposedRegion(p, event)
     drawComp *p;
     XEvent   *event;
#endif
{
  int regx = event->xexpose.x;
  int regy = event->xexpose.y;
  int regw = event->xexpose.width;
  int regh = event->xexpose.height;
  int clippedLine = 0;
  int i;
  short clipCode[5];

  clipCode[0] = compOutCode (p->x, p->y, regx, regy, regx + regw, regy + regh);
  clipCode[1] = compOutCode (p->x + (p->width)*(p->col), p->y, regx, regy, 
			     regx + regw, regy + regh);
  clipCode[2] = compOutCode (p->x + (p->width)*(p->col), p->y + (p->height)*(p->row),
			     regx, regy, regx + regw, regy + regh);
  clipCode[3] = compOutCode (p->x, p->y + (p->height)*(p->row), 
			     regx, regy, regx + regw, regy + regh);
  clipCode[4] = clipCode[0];

  for (i = 0; i < 5; i++){
    if ( clipCode[i] == 0x0000 && clipCode[i+1] == 0x0000)
      clippedLine++;
    else if((clipCode[i] & clipCode[i+1]) == 0)
      clippedLine++;      
  }
  if (clippedLine >= 2)
    return 1;
  else
    return 0;
}

/**********************************************************************
 *        void redisplay(graph, w, event)                             *
 * Description:                                                       *
 *     Redisplay all components  according to linked list             *
 *     of components                                                  *
 *********************************************************************/
#if defined (__STDC__)
static void redisplay(XcodaEditorGraph* graph, 
		      Widget  w, 
		      XEvent* event)
#else
static void redisplay(graph, w, event)
     XcodaEditorGraph *graph;
     Widget           w;
     XEvent           *event;
#endif
{
  drawComp *q = NULL;
  compList *p;
  arcList  *qq = NULL;
  Arc      *qa, *qh = NULL;
  ipPort   *port;
  int      i;

  if (isEmptyGraph (graph))
    return;

/* reset geometry parameter for draw component */
  (*graph->zoom_scroll) (graph, w, event);
  for(p=graph->comp_list_head->next; p != graph->comp_list_tail; p=p->next){
    q = p->draw_comp;
    if(event && event->type == Expose){
      if (insideExposedRegion(q, event)){
	if(q->selected)
	  (*q->high_light)(q,xgc.dpy, XtWindow(sw_geometry.draw_area));
	else
	  (*q->draw_original)(q,xgc.dpy, XtWindow(sw_geometry.draw_area),q->x, q->y,
			      q->width, q->height,
			      q->row, q->col);
      }
    }
    else{
      if(q->selected)
	(*q->high_light)(q,xgc.dpy, XtWindow(sw_geometry.draw_area));
      else{
	(*q->draw_original)(q,xgc.dpy, XtWindow(sw_geometry.draw_area),q->x, q->y,
			    q->width, q->height,
			    q->row, q->col);
      }
    }
  }
  for(qq = graph->arc_list_head->next; qq != graph->arc_list_tail; qq=qq->next){
    qa = qq->arc;
    config_arc_geometry(qa, qa->from, qa->to);    
    if(qa->selected)
      qh = qa;
    else
      (*qa->draw_original)(qa, xgc.dpy, XtWindow(sw_geometry.draw_area));
  }
  if(qh != NULL) /* draw highlighted arc last */
    (*qh->high_light)(qh, xgc.dpy, XtWindow(sw_geometry.draw_area));    
    
}

/****************************************************************************
 *            void zoom_scroll_graph(graph, w, event)                       *
 * Description:                                                             * 
 *    update all components information in the case of zooming+ scrolled    *
 ***************************************************************************/
#if defined (__STDC__)
static void zoom_scroll_graph(XcodaEditorGraph* graph, 
			      Widget w, 
			      XEvent* event)
#else
static void zoom_scroll_graph(graph, w, event)
     XcodaEditorGraph *graph;
     Widget      w;
     XEvent      *event;
#endif
{
  drawComp *q = NULL;
  compList *p;
  int      xx, yy;
  int      wd, ht;
  ipPort   *port;
  int      i;

  if (isEmptyGraph (graph))
    return;

  wd = sw_geometry.cell_wd*sw_geometry.zoomscale;
  ht = sw_geometry.cell_ht*sw_geometry.zoomscale;
  for(p=graph->comp_list_head->next; p != graph->comp_list_tail; p=p->next){
    q = p->draw_comp;
    xx = (q->col - sw_geometry.zoomxoff - 1)*wd;
    yy = (q->row - sw_geometry.zoomyoff - 1)*ht;

    /* take care geometry elements in case of zoom */
    setDrawCompGeometry (q, xx, yy, wd, ht);
  }
}

/*****************************************************************
 *        Arc* newArcConnectingComps (drawComp* comp0, comp1)    *
 * Description:                                                  *
 *     Create a new arc connecting two components                *
 ****************************************************************/
#if defined (__STDC__)
static Arc* newArcConnectingComps (drawComp* comp0, drawComp* comp1)
#else
static Arc* newArcConnectingComps (comp0, comp1)
     drawComp *comp0, *comp1;
#endif
{
  Arc *new_arc;

  new_arc = (Arc *)malloc(sizeof(Arc));
  if(new_arc == NULL){
    fprintf(stderr,"Cannot allocate memory for new Arc.\n");
    exit(1);
  }
  new_arc->type = IP_CONNECTION;
  new_arc->selected = 0;
  new_arc->arrow_type = 0;
  new_arc->from_row = comp0->ip_port[0].row;
  new_arc->from_col = comp0->ip_port[0].col;
  new_arc->to_row = comp1->ip_port[0].row;
  new_arc->to_col = comp1->ip_port[0].col;
  new_arc->from = &(comp0->ip_port[0]);
  new_arc->to = &(comp1->ip_port[0]);
  new_arc->draw_original = draw_original_arc;
  new_arc->high_light = draw_high_light_arc;
  new_arc->erase = erase_arc;
  new_arc->draw_rubber_arc = draw_rubber_arc;

  return new_arc;
}

/*****************************************************************
 *        Arc* newArcConnectingPorts (ipPorts* p0, p1)           *
 * Description:                                                  *
 *     Create a new arc connecting two ports                     *
 ****************************************************************/
#if defined (__STDC__)
static Arc* newArcConnectingPorts (ipPort* from_port, 
				   ipPort* to_port)
#else
static Arc* newArcConnectingPorts (from_port, to_port)
     ipPort* from_port;
     ipPort* to_port;
#endif
{
  Arc* new_arc;

  new_arc = (Arc *)malloc(sizeof(Arc));
  if(new_arc == NULL){
    fprintf(stderr,"Cannot allocate memory for new Arc.\n");
    exit(1);
  }
  new_arc->type = IP_CONNECTION;
  new_arc->selected = 0;
  new_arc->arrow_type = 0;
  new_arc->from_row = from_port->row;
  new_arc->from_col = from_port->col;
  new_arc->to_row = to_port->row;
  new_arc->to_col = to_port->col;
  new_arc->from = from_port;
  new_arc->to = to_port;
  new_arc->draw_original = draw_original_arc;
  new_arc->high_light = draw_high_light_arc;
  new_arc->erase = erase_arc;
  new_arc->draw_rubber_arc = draw_rubber_arc;

  return new_arc;
}

/****************************************************************************
 *       void update_arc_geometry(arc, from, to)                            *
 * Description:                                                             *
 *    Update a particular arc geometry information according to             *
 *    ports it connects                                                     *
 ***************************************************************************/
#if defined (__STDC__)
void update_arc_geometry(Arc* arc, 
			 ipPort* from, 
			 ipPort* to)
#else
void update_arc_geometry(arc, from, to)
     Arc *arc;
     ipPort *from;
     ipPort *to;
#endif
{
  int from_x, from_y;
  int to_x, to_y;
  int style;
  
  int i = 0;
  arcList *p;

  for(p=coda_graph.arc_list_head->next; p!=coda_graph.arc_list_tail;p=p->next){
    if(p->arc->to == to)
      i++;
  }

  arc->arrow_type = 0;
  from_x = from->right_x + from->width;
  from_y = from->right_y + from->height/2.0;
  to_x = to->left_x;
  to_y = to->left_y + to->height/2.0;
  style = 0;
  if (to_x <= from_x) {
    style = 1;
    if (to_y == from_y) 
      style = 2;
  }
  switch (style) {
  case 1:
    arc->edge_point[0].x = from_x;
    arc->edge_point[0].y = from_y;
    arc->edge_point[1].x = from_x + 20;
    arc->edge_point[1].y = from_y;
    arc->edge_point[2].x = from_x + 20;
    arc->edge_point[2].y = from_y + (to_y - from_y)/2.0;
    arc->edge_point[3].x = to_x - 20;
    arc->edge_point[3].y = from_y + (to_y - from_y)/2.0;
    arc->edge_point[4].x = to_x - 20;
    arc->edge_point[4].y = to_y;
    arc->edge_point[5].x = to_x;
    arc->edge_point[5].y = to_y;
    break;
  case 2:
    arc->edge_point[0].x = from_x;
    arc->edge_point[0].y = from_y;
    arc->edge_point[1].x = from_x + 20;
    arc->edge_point[1].y = from_y;
    arc->edge_point[2].x = from_x + 20;
    arc->edge_point[2].y = from_y + from->height + 20;
    arc->edge_point[3].x = to_x - 20;
    arc->edge_point[3].y = from_y + from->height + 20;
    arc->edge_point[4].x = to_x - 20;
    arc->edge_point[4].y = to_y;
    arc->edge_point[5].x = to_x;
    arc->edge_point[5].y = to_y;
    break;
  default:
    arc->edge_point[0].x = from_x;
    arc->edge_point[0].y = from_y;
    arc->edge_point[1].x = from_x + (to_x - from_x)/2.0;
    arc->edge_point[1].y = from_y;
    arc->edge_point[2].x = arc->edge_point[1].x;
    arc->edge_point[2].y = to_y;
    arc->edge_point[3].x = to_x;
    arc->edge_point[3].y = to_y;
    arc->edge_point[4].x = to_x;
    arc->edge_point[4].y = to_y;
    arc->edge_point[5].x = to_x;
    arc->edge_point[5].y = to_y;
    
  }
}

/****************************************************************************
 *       void config_arc_geometry(arc, from, to)                            *
 * Description:                                                             *
 *    Update a particular arc geometry information according to             *
 *    ports it connects under resize and scroll                             *
 ***************************************************************************/
#if defined (__STDC__)
static void config_arc_geometry(Arc* arc, 
				ipPort* from, 
				ipPort* to)
#else
static void config_arc_geometry(arc, from, to)
     Arc *arc;
     ipPort *from;
     ipPort *to;
#endif
{
  int from_x, from_y;
  int to_x, to_y;
  int style;
  
  int i = 0;
  arcList *p;

  for(p=coda_graph.arc_list_head->next; p!=coda_graph.arc_list_tail;p=p->next){
    if(p->arc->to == to)
      i++;
  }

  arc->arrow_type = 0;
  from_x = from->right_x + from->width;
  from_y = from->right_y + from->height/2.0;
  to_x = to->left_x;
  to_y = to->left_y + to->height/2.0;
  style = 0;
  if (to_x <= from_x) {
    style = 1;
    if (to_y == from_y) 
      style = 2;
  }
  switch (style) {
  case 1:
    arc->edge_point[0].x = from_x;
    arc->edge_point[0].y = from_y;
    arc->edge_point[1].x = from_x + 20;
    arc->edge_point[1].y = from_y;
    arc->edge_point[2].x = from_x + 20;
    arc->edge_point[2].y = from_y + (to_y - from_y)/2.0;
    arc->edge_point[3].x = to_x - 20;
    arc->edge_point[3].y = from_y + (to_y - from_y)/2.0;
    arc->edge_point[4].x = to_x - 20;
    arc->edge_point[4].y = to_y;
    arc->edge_point[5].x = to_x;
    arc->edge_point[5].y = to_y;
    break;
  case 2:
    arc->edge_point[0].x = from_x;
    arc->edge_point[0].y = from_y;
    arc->edge_point[1].x = from_x + 20;
    arc->edge_point[1].y = from_y;
    arc->edge_point[2].x = from_x + 20;
    arc->edge_point[2].y = from_y + from->height + 20;
    arc->edge_point[3].x = to_x - 20;
    arc->edge_point[3].y = from_y + from->height + 20;
    arc->edge_point[4].x = to_x - 20;
    arc->edge_point[4].y = to_y;
    arc->edge_point[5].x = to_x;
    arc->edge_point[5].y = to_y;
    break;
  default:
    arc->edge_point[0].x = from_x;
    arc->edge_point[0].y = from_y;
    arc->edge_point[1].x = from_x + (to_x - from_x)/2.0;
    arc->edge_point[1].y = from_y;
    arc->edge_point[2].x = arc->edge_point[1].x;
    arc->edge_point[2].y = to_y;
    arc->edge_point[3].x = to_x;
    arc->edge_point[3].y = to_y;
    arc->edge_point[4].x = to_x;
    arc->edge_point[4].y = to_y;
    arc->edge_point[5].x = to_x;
    arc->edge_point[5].y = to_y;
    
  }
}

/****************************************************************************
 *       void reassign_arc_geometry(graph, arc)                             *
 * Description:                                                             *
 *    reassign edge point upon ending moving an arc                         *
 ***************************************************************************/
#if defined (__STDC__)
static void reassign_arc_geometry(Arc* arc, int x, int y)
#else
static void reassign_arc_geometry(arc, x, y)
     Arc *arc;
     int x, y;
#endif
{
  int xx, yy;

  if(arc->edge_point[0].x >= arc->edge_point[3].x){
    if(x >= arc->edge_point[0].x)
      xx = arc->edge_point[0].x - 10;
    else if(x <= arc->edge_point[3].x)
      xx = arc->edge_point[3].x + 10;
    else
      xx = x;
  }
  else{
    if(x >= arc->edge_point[3].x)
      xx = arc->edge_point[3].x - 10;
    else if(x <= arc->edge_point[0].x)
      xx = arc->edge_point[0].x + 10;
    else
      xx = x;
  }
  arc->edge_point[1].x = xx;
  arc->edge_point[2].x = xx;
}


/****************************************************************************
 *     static Boolean isArcMovable(arc)                                     *
 * Description:                                                             *
 *     Tell whether an arc can be moved from left to right                  *
 ***************************************************************************/
#if defined (__STDC__)
static int isArcMovable(Arc* arc)
#else
static int isArcMovable(arc)
     Arc *arc;
#endif
{
  int y = arc->edge_point[0].y;
  int i;

  for(i=1;i<4;i++){
    if(arc->edge_point[i].y != y)
      return 1;
  }
  return 0;
}


/****************************************************************************
 *            void select_arc_or_node(graph, w, event)                      *
 * Description:                                                             *
 *     select a node or ip_port or an arc by pressing button1 under         *
 *     Normal mode ie. not delete and move etc....                          *
 ****************************************************************************/
#if defined (__STDC__)
static int select_arc_or_node(XcodaEditorGraph* graph, 
			       Widget w, 
			       XEvent* event)
#else
static int select_arc_or_node(graph, w, event)
     XcodaEditorGraph *graph;
     Widget w;
     XEvent *event;
#endif
{
  drawComp  *p, *selected_node;
  Arc       *arc, *selected_arc;
  ipPort    *port;

  if((p = XcodaEditorClickedNode(graph,event)) != NULL){
    selected_node = XcodaEditorGetSelectedNode(graph);
    if(selected_node != NULL)
      XcodaEditorUnselectNode(graph, selected_node);
    selected_arc = XcodaEditorGetSelectedArc(graph);
    if(selected_arc != NULL)
      XcodaEditorUnselectArc(graph, selected_arc);
    XcodaEditorSelectNode(graph, p);
    graph->current_action = NORMAL_ACTION;
    if(isDoubleClick(graph, event)){
      HandleDoubleClick(graph, w, event);
      return 0;
     }
  }
  else if((port = XcodaEditorClickedPort(graph, event)) != NULL){
    XcodaEditorHighLightPort(graph, port);
    graph->current_action = HIGHLIGHT_IP_PORTS_ACTION;
    if(isDoubleClick(graph, event)){
      HandleDoubleClick(graph, w, event);
      return 0;
    }
  }
  else if((arc = XcodaEditorClickedArc(graph, event)) != NULL){
    selected_node = XcodaEditorGetSelectedNode(graph);
    if(selected_node != NULL)
      XcodaEditorUnselectNode(graph, selected_node);
    selected_arc = XcodaEditorGetSelectedArc(graph);
    if(selected_arc != NULL)
      XcodaEditorUnselectArc(graph, selected_arc);
    XcodaEditorSelectArc(graph, arc);
    graph->current_action = NORMAL_ACTION;
    }
  else{
    graph->current_action = NORMAL_ACTION;
  }
  return 1;
}
  

/**********************************************************************
 *       void select_ip_ports(graph, w, event)                        *
 * Description:                                                       *
 *     select a particular ip_port by releasing btn1 inside the port  *
 *********************************************************************/
#if defined (__STDC__)
static void select_ip_ports(XcodaEditorGraph* graph, 
			    Widget w, 
			    XEvent* event)
#else
static void select_ip_ports(graph, w, event)
     XcodaEditorGraph *graph;
     Widget           w;
     XEvent           *event;
#endif
{
  ipPort *port;
  int     xx, yy;
  Arc     *new_arc, *ret_arc;
  
  xx = event->xbutton.x;
  yy = event->xbutton.y;

  if(graph->current_action == HIGHLIGHT_IP_PORTS_ACTION){
    port = graph->from_port;
    if(port->left_selected){
      graph->from_port = (ipPort *)NULL;
      graph->current_action = NORMAL_ACTION;
      (*port->disarm)(port,xgc.dpy, XtWindow(sw_geometry.draw_area));
      XcodaEditorUnhighLightAllConnectedPorts(graph, port);
    }
    else if(port->right_selected){
      graph->from_port = (ipPort *)NULL;
      graph->current_action = NORMAL_ACTION;
      (*port->disarm)(port,xgc.dpy, XtWindow(sw_geometry.draw_area));
      XcodaEditorUnhighLightAllConnectedPorts(graph, port);
    }    
    else
      ;
  }
}


/***************************************************************
 *     Arc *XcodaEditorClickedArc(graph, event)                *
 * Description:                                                *
 *    return arc object when one clicks on it                  *
 **************************************************************/
#if defined (__STDC__)
static Arc *XcodaEditorClickedArc(XcodaEditorGraph* graph, 
				  XEvent* event)
#else
static Arc *XcodaEditorClickedArc(graph, event)
     XcodaEditorGraph *graph;
     XEvent           *event;
#endif
{
  arcList *p;
  Arc     *q;
  int     i;
  XPoint  pp[4];
  int     xx, yy;

  if(graph->arc_list_head->next == graph->arc_list_tail)
    return (Arc *)NULL;

  xx = event->xbutton.x;
  yy = event->xbutton.y;

  for(p = graph->arc_list_head->next; p != graph->arc_list_tail; p = p->next){
    q = p->arc;
    for(i=0;i<4;i++){
      pp[i].x = q->edge_point[i].x;
      pp[i].y = q->edge_point[i].y;
    }
    if(pp[3].x > pp[0].x){
      if(pp[2].y > pp[1].y){
	if(((xx >= pp[0].x && xx <= pp[1].x) && (yy <= pp[0].y + 2 && yy >= pp[0].y - 2)) ||
	   ((xx >= pp[1].x - 2 && xx <= pp[1].x +2) && (yy <= pp[2].y && yy >= pp[1].y)) ||
	   ((xx >= pp[2].x && xx <= pp[3].x) && (yy >= pp[3].y - 2 && yy <= pp[3].y + 2)))
	  return q;
      }
      else if(pp[2].y < pp[1].y){
	if(((xx >= pp[0].x && xx <= pp[1].x) && (yy <= pp[0].y + 2 && yy >= pp[0].y - 2)) ||
	   ((xx >= pp[1].x - 2 && xx <= pp[1].x +2) && (yy <= pp[1].y && yy >= pp[2].y)) ||
	   ((xx >= pp[2].x && xx <= pp[3].x) && (yy >= pp[3].y - 2 && yy <= pp[3].y + 2)))
	  return q;
      }
      else{
	if((xx >= pp[0].x && xx <= pp[3].x) && (yy <= pp[0].y + 2 && yy >= pp[0].y -2))
	  return q;
      }
    }
    else if(pp[3].x < pp[0].x){
      if(pp[2].y > pp[1].y){
	if(((xx >= pp[1].x && xx <= pp[0].x) && (yy <= pp[0].y + 2 && yy >= pp[0].y - 2)) ||
	   ((xx >= pp[1].x - 2 && xx <= pp[1].x +2) && (yy <= pp[2].y && yy >= pp[1].y)) ||
	   ((xx >= pp[3].x && xx <= pp[2].x) && (yy >= pp[3].y - 2 && yy <= pp[3].y + 2)))
	  return q;
      }
      else if(pp[2].y < pp[1].y){
	if(((xx >= pp[1].x && xx <= pp[0].x) && (yy <= pp[0].y + 2 && yy >= pp[0].y - 2)) ||
	   ((xx >= pp[1].x - 2 && xx <= pp[1].x +2) && (yy <= pp[1].y && yy >= pp[2].y)) ||
	   ((xx >= pp[3].x && xx <= pp[2].x) && (yy >= pp[3].y - 2 && yy <= pp[3].y + 2)))
	  return q;
      }
      else{
	if((xx >= pp[3].x && xx <= pp[0].x) && (yy <= pp[0].y + 2 && yy >= pp[0].y -2))
	  return q;
      }
    }      
    else{
      if(pp[2].y > pp[1].y){
	if(((xx >= pp[1].x && xx <= pp[0].x) && (yy <= pp[0].y + 2 && yy >= pp[0].y - 2)) ||
	   ((xx >= pp[1].x - 2 && xx <= pp[1].x +2) && (yy <= pp[2].y && yy >= pp[1].y)) ||
	   ((xx >= pp[2].x && xx <= pp[3].x) && (yy >= pp[3].y - 2 && yy <= pp[3].y + 2)))
	  return q;
      }
      else if(pp[2].y < pp[1].y){
	if(((xx >= pp[1].x && xx <= pp[0].x) && (yy <= pp[0].y + 2 && yy >= pp[0].y - 2)) ||
	   ((xx >= pp[1].x - 2 && xx <= pp[1].x +2) && (yy <= pp[1].y && yy >= pp[2].y)) ||
	   ((xx >= pp[2].x && xx <= pp[3].x) && (yy >= pp[3].y - 2 && yy <= pp[3].y + 2)))
	  return q;
      }
    }
  }
  return (Arc *)NULL;
}

/***************************************************************
 *       int XcodaEditorPortNumber(graph, port)                *
 * Description:                                                *
 *    return port number for a particular port                 *
 **************************************************************/
#if defined (__STDC__)
int XcodaEditorPortNumber(XcodaEditorGraph* graph, ipPort* port)
#else
int XcodaEditorPortNumber(graph, port)
     XcodaEditorGraph *graph;
     ipPort *port;
#endif
{
  compList *p;
  drawComp *q;
  daqComp  *daq, *daq_comp;
  int      i;

  daq = port->comp_ptr;
  for(p = graph->comp_list_head->next; p!= graph->comp_list_tail; p= p->next){
    daq_comp = &(p->draw_comp->comp);
    if(daq_comp == daq)
      break;
  }
  q = p->draw_comp;
  for(i=0;i<q->num_ports; i++){
    if(port == &(q->ip_port[i]))
      break;
  }
  return i;
}

/***************************************************************
 *        ipPort  *XcodaEditorClickedPort(graph, event)        *
 * Description:                                                *
 *    return ipPort port when one press btn1                   *
 **************************************************************/
#if defined (__STDC__)
static ipPort *XcodaEditorClickedPort(XcodaEditorGraph* graph, 
				      XEvent* event)
#else
static ipPort *XcodaEditorClickedPort(graph, event)
     XcodaEditorGraph *graph;
     XEvent           *event;
#endif
{
  compList *q;
  int      xx, yy;
  int      i;
  int      col, row;
  int      wd, ht;
  ipPort   *port;

  if(graph->comp_list_head->next == graph->comp_list_tail)
    return (ipPort *)NULL;

  xx = event->xbutton.x;
  yy = event->xbutton.y;

  wd = sw_geometry.cell_wd*sw_geometry.zoomscale;
  ht = sw_geometry.cell_ht*sw_geometry.zoomscale;

  col = (event->xbutton.x)/wd + sw_geometry.zoomxoff + 1;

  row = (event->xbutton.y)/ht + sw_geometry.zoomyoff + 1;
  
  for(q=graph->comp_list_head->next; q != graph->comp_list_tail; q = q->next){
    if( (row >= q->draw_comp->row && row <= q->draw_comp->row + q->draw_comp->num_row - 1) &&
        (col >= q->draw_comp->col && col <= q->draw_comp->col + q->draw_comp->num_col - 1) ){
      for(i=0;i<q->draw_comp->num_ports;i++){
	port = &(q->draw_comp->ip_port[i]);
	if( (xx >= port->left_x && xx <= port->left_x + port->width) &&
	    (yy >= port->left_y && yy <= port->left_y + port->height)){
	  port->left_selected = 1;
	  return port;	  
	}
	if( (xx >= port->right_x && xx <= port->right_x + port->width) &&
	    (yy >= port->right_y && yy <= port->right_y + port->height)){
	  port->right_selected = 1;
	  return port;	  
	}
      }
      return (ipPort *)NULL;
    }
  }
  return (ipPort *)NULL;
}


/************************************************************************
 *        drawComp* newDrawComp (drawComp* q, int x, int y,             *
 *                               int col, int row, int wd, ht)          *
 * Description:                                                         *
 *      Create a new drawComp with all initial value set                *
 ***********************************************************************/
#if defined (__STDC__)
static drawComp* newDrawComp (drawComp* q, 
			      int x, int y, 
			      int col, int row, 
			      int wd, int ht)
#else
static drawComp* newDrawComp (q, x, y, col, row, wd, ht)
     drawComp* q;
     int x, y;
     int col, row;
     int wd, ht;
#endif
{
  drawComp *qq;

  qq = (drawComp *)malloc(1*sizeof(drawComp));
  qq->draw_original = q->draw_original;
  qq->draw_rubber_comp = q->draw_rubber_comp;
  qq->high_light = draw_high_light;
  qq->unhigh_light = draw_unhigh_light;
  qq->erase = erase_comp;
  qq->write_name = draw_comp_name;
  qq->erase_name = erase_comp_name;
  qq->move_name = move_comp_name;
  qq->write_hostname = draw_host_name;
  qq->erase_hostname = erase_host_name;
  qq->move_hostname = move_host_name;
  qq->set_attributes = popup_comp_attributes;
  qq->comp.type = q->comp.type;
  qq->comp.comp_name = (char *)NULL;
  qq->comp.node_name = (char *)NULL;
  qq->comp.id_num = -1;
  qq->comp.status = 0;
  qq->comp.boot_string = (char *)NULL;
  qq->comp.code[0] = (char *)NULL;
  qq->comp.code[1] = (char *)NULL;
  qq->comp.code[2] = (char *)NULL;
  qq->editable = 1;
  qq->selected = 0;
  qq->num_ports = 1;
  qq->num_row = 1;
  qq->num_col = 1;
  qq->x = x;
  qq->y = y;
  qq->col = col;
  qq->row = row;
  qq->width = wd;
  qq->height = ht;
  qq->scripts = 0;

  return qq;
}

/***************************************************************
 *        drawComp *createDrawComp (graph, type)               *
 * Description:                                                *
 *     Create a new drawComp with all default values           *
 **************************************************************/
#if defined (__STDC__)
static drawComp* createDrawComp (XcodaEditorGraph* graph, int type)
#else
static drawComp* createDrawComp (graph, type)
     XcodaEditorGraph *graph;
     int type;
#endif
{
  drawComp *q;

  q = (drawComp *)malloc(1*sizeof(drawComp));
  q->draw_original = draw_original;
  q->draw_rubber_comp = draw_rubber_comp;
  switch(type){
  case ADD_ROC_ACTION:
    q->comp.type = CODA_ROC;
    break;
  case ADD_TRIG_ACTION:
    q->comp.type = CODA_TRIG;
    break;
  case ADD_EB_ACTION:
    q->comp.type = CODA_EB;
    break;
  case ADD_MON_ACTION:
    q->comp.type = CODA_MON;
    break;
  case ADD_NONE_ACTION:
    q->comp.type = CODA_NONE;
    break;
  case ADD_ER_ACTION:
    q->comp.type = CODA_ER;
    break;
  case ADD_FI_ACTION:
    q->comp.type = CODA_FILE;
    break;
  case ADD_CFI_ACTION:
    q->comp.type = CODA_CODAFILE;
    break;
  case ADD_DBG_ACTION:
    q->comp.type = CODA_DEBUG;
    break;    
  default:
    break;
  }
  graph->start_x = 0;
  graph->start_y = 0;
  graph->current_comp = q;
  q->num_row = 1;
  q->num_col = 1;
  q->num_ports = 0;
  q->selected = 0;
  q->scripts = 0;
  q->write_name = draw_comp_name;
  q->erase_name = erase_comp_name;
  q->move_name = move_comp_name;
  q->write_hostname = draw_host_name;
  q->erase_hostname = erase_host_name;
  q->move_hostname = move_host_name;
  q->comp.comp_name = (char *)NULL;
  q->comp.node_name = (char *)NULL;
  q->comp.status = 0;

  q->comp.code[0] = (char *)0;
  q->comp.code[1] = (char *)0;
  q->comp.code[2] = (char *)0;
  q->comp.boot_string = (char *)0;
  q->comp.id_num = -1;

  return q;
}

/***************************************************************
 *        drawComp *dupDrawComp (int type, drawComp* comp)     *
 * Description:                                                *
 *     Create a new drawComp with type 'type' and default value*
 *     from an existing draw component 'comp'                  *
 **************************************************************/
#if defined (__STDC__)
static drawComp* dupDrawComp (int type, drawComp* comp)
#else
static drawComp* dupDrawComp (type, comp)
     int type;
     drawComp* comp;
#endif
{
  drawComp *ncomp;
  ipPort   *port, *old_port;
  int i = 0;

/* create ncomp node and add to comp_list */
  ncomp = (drawComp *)malloc(1*sizeof(drawComp));
  ncomp->draw_original = comp->draw_original;
  ncomp->draw_rubber_comp = comp->draw_rubber_comp;
  ncomp->high_light = comp->high_light;
  ncomp->unhigh_light = comp->unhigh_light;
  ncomp->erase = comp->erase;
  ncomp->write_name = comp->write_name;
  ncomp->erase_name = comp->erase_name;
  ncomp->move_name = comp->move_name;
  ncomp->write_hostname = comp->write_hostname;
  ncomp->erase_hostname = comp->erase_hostname;
  ncomp->move_hostname = comp->move_hostname;
  ncomp->set_attributes = comp->set_attributes;
  ncomp->comp.type = type;
  ncomp->comp.comp_name = (char *)NULL;
  ncomp->comp.node_name = strsave(comp->comp.node_name);
  ncomp->comp.id_num = comp->comp.id_num;
  if (comp->comp.boot_string != 0)
    ncomp->comp.boot_string = strsave(comp->comp.boot_string);
  else
    ncomp->comp.boot_string = (char *)0;  
  ncomp->comp.status = 0;
  
  ncomp->comp.code[0] = (char *)0;
  ncomp->comp.code[1] = (char *)0;
  ncomp->comp.code[2] = (char *)0;
  ncomp->editable = comp->editable;
  ncomp->selected = comp->selected;
  ncomp->scripts = 0;
  ncomp->num_ports = comp->num_ports;
  ncomp->num_row = comp->num_row;
  ncomp->num_col = comp->num_col;
  ncomp->x = comp->x;
  ncomp->y = comp->y;
  ncomp->col = comp->col;
  ncomp->row = comp->row;
  ncomp->width = comp->width;
  ncomp->height = comp->height;

/*init ip_port lists*/
  for(i = 0;i < ncomp->num_ports; i++){
    port = &(ncomp->ip_port[i]);
    old_port = &(comp->ip_port[i]);
    port->index = i;
    port->left_x = old_port->left_x;
    port->left_y = old_port->left_y;
    port->right_x = old_port->right_x;
    port->right_y = old_port->right_y;
    port->row = old_port->row;
    port->col = old_port->col;
    port->width = old_port->width;
    port->height = old_port->height;
    port->left_selected = old_port->left_selected;
    port->right_selected = old_port->right_selected;
    port->ip_addr = strsave (ncomp->comp.node_name);
    port->comp_ptr = &(ncomp->comp);
    port->arm = old_port->arm;
    port->disarm = old_port->disarm;
    port->input_high_light = old_port->input_high_light;
    port->output_high_light = old_port->output_high_light;
    port->set_attributes = old_port->set_attributes;
    port->erase_name = old_port->erase_name;
    port->write_name = old_port->write_name;
    port->move_name = old_port->move_name;
  }
  return ncomp;
}


/*******************************************************************************
 *        void drawCompTransform (drawComp* comp, int row, int col)            *
 *  Description:                                                               *
 *    drawComp transform by row and column                                     * 
 ******************************************************************************/
#if defined (__STDC__)
static void drawCompTransform (drawComp* comp, 
			       int row, int col)
#else
static void drawCompTransform (comp, row, col)
     drawComp* comp;
     int       row;
     int       col;
#endif
{
  ipPort *port;
  int    i = 0;

  comp->x = comp->x + col*sw_geometry.cell_wd*sw_geometry.zoomscale;
  comp->col = comp->col + col;

  comp->y = comp->y + row*sw_geometry.cell_wd*sw_geometry.zoomscale;
  comp->row = comp->row + row;

  for (i = 0; i < comp->num_ports; i++) {
    port = &(comp->ip_port[i]);
    port->left_x += col*sw_geometry.cell_wd*sw_geometry.zoomscale;
    port->left_y += row*sw_geometry.cell_wd*sw_geometry.zoomscale;
    port->right_x += col*sw_geometry.cell_wd*sw_geometry.zoomscale;
    port->right_y += row*sw_geometry.cell_wd*sw_geometry.zoomscale;
    port->row += row;
    port->col += col;
  }
}


/*******************************************************************************
 *        void setGraphStartingPosition (drawComp* comp, int x, int y)         *
 *  Description:                                                               *
 *    Setup drawComponent starting position                                    *
 ******************************************************************************/
#if defined (__STDC__)
static void setGraphStartingPosition (XcodaEditorGraph* graph,
				      int x, int y)
#else
static void setGraphStartingPosition (graph, x, y)
     XcodaEditorGraph* graph;
     int x, y;
#endif
{
  graph->start_x = x;
  graph->start_y = y;
}

/*******************************************************************************
 *        void setDrawCompGeometry (drawComp *comp,                            *
 *                                  int x, int y, int wd, int ht)              *
 *  Description:                                                               *
 *    Setup drawComponent Geometry Information                                 *
 ******************************************************************************/
#if defined (__STDC__)
static void setDrawCompGeometry (drawComp* comp, 
				 int xx, int yy, 
				 int wd, int ht)
#else
static void setDrawCompGeometry (comp, xx, yy, wd, ht)
     drawComp* comp;
     int xx, yy;
     int wd, ht;
#endif
{
  ipPort* port;
  int     i = 0;

  /* take care geometry elements in case of zoom */
  comp->x = xx;
  comp->y = yy;
  comp->width =  wd;
  comp->height = ht;
  /* end of geometry */
  for(i = 0; i < comp->num_ports; i++){
    port = &(comp->ip_port[i]);
    port->left_x = xx;
    port->left_y = yy + 3*ht/8.0 + i*ht;
    port->right_x = xx + wd - wd/8.0;
    port->right_y = port->left_y;
    port->width = wd/8.0;
    port->height = ht/4.0;
  }
}
  

/*******************************************************************************
 *        void freeDrawComp (drawComp* comp)                                   *
 *  Description:                                                               *
 *    Free draw component                                                      *
 ******************************************************************************/
#if defined (__STDC__)
static void freeDrawComp (drawComp* comp)
#else
static void freeDrawComp (comp)
     drawComp* comp;
#endif
{
  /* free memories for this component */
  free_daqcomp_res (&(comp->comp));
  free_ipPort_res (comp->ip_port, comp->num_ports);
  if (comp->scripts)
    freeCodaScriptList (comp->scripts);
  free (comp);
  comp = (drawComp *)0;  
}

/***************************************************************
 *        drawComp *XcodaEditorClickedNode(graph, event)       *
 * Description:                                                *
 *    return drawComp node when you click on the drawing area  *
 **************************************************************/
#if defined (__STDC__)
static drawComp *XcodaEditorClickedNode(XcodaEditorGraph* graph, 
					XEvent* event)
#else
static drawComp *XcodaEditorClickedNode(graph, event)
     XcodaEditorGraph *graph;
     XEvent           *event;
#endif
{
  compList *q;
  drawComp *sel;
  int row, col;
  int xx, yy;
  int rel_x, rel_y;
  int wd, ht, xdiv, ydiv;
  

  xx = event->xbutton.x;
  yy = event->xbutton.y;
  wd = sw_geometry.cell_wd*sw_geometry.zoomscale;
  ht = sw_geometry.cell_ht*sw_geometry.zoomscale;
  xdiv = wd/4;
  ydiv = ht/4;

  col = (event->xbutton.x)/(sw_geometry.cell_wd*sw_geometry.zoomscale)
    + sw_geometry.zoomxoff + 1;

  row = (event->xbutton.y)/(sw_geometry.cell_ht*sw_geometry.zoomscale)
    + sw_geometry.zoomyoff + 1;
  
  if(graph->comp_list_head->next == graph->comp_list_tail)
    return (drawComp *)NULL;

  for(q=graph->comp_list_head->next; q != graph->comp_list_tail; q = q->next){
    if( (row >= q->draw_comp->row && row <= q->draw_comp->row + q->draw_comp->num_row - 1) &&
        (col >= q->draw_comp->col && col <= q->draw_comp->col + q->draw_comp->num_col - 1) ){
      sel = q->draw_comp;
      rel_x = xx - sel->x;
      rel_y = yy - sel->y;
      if(rel_x >= 0.0  && rel_x <= wd)
	return q->draw_comp;
      else
	return (drawComp *)NULL;
    }
  }
  return (drawComp *)NULL;
}

/***************************************************************************
 *      drawComp *XcodaeditorGetSelectedNode()                             *
 * Description:                                                            *
 *    get selected node inside drawing area                                *
 **************************************************************************/
#if defined (__STDC__)
drawComp *XcodaEditorGetSelectedNode(XcodaEditorGraph* graph)
#else
drawComp *XcodaEditorGetSelectedNode(graph)
     XcodaEditorGraph *graph;
#endif
{
  compList *p;

  if(graph->comp_list_head->next == graph->comp_list_tail)
    return (drawComp *)NULL;

  for(p=graph->comp_list_head->next; p != graph->comp_list_tail; p=p->next){
    if(p->draw_comp->selected){
      graph->selected_comp = p->draw_comp;
      return p->draw_comp;
    }
  }
  return (drawComp *)NULL;
}


/*************************************************************************
 *          Arc *XcodaEditorGetSelectedArc(graph)                        *
 * Description:                                                          *
 *     get selected arc from arc list                                    *
 ************************************************************************/
#if defined (__STDC__)
Arc *XcodaEditorGetSelectedArc(XcodaEditorGraph* graph)
#else
Arc *XcodaEditorGetSelectedArc(graph)
     XcodaEditorGraph *graph;
#endif
{
  arcList *p;
  Arc     *q;

  if(graph->arc_list_head->next == graph->arc_list_tail)
    return (Arc *)NULL;

  for(p = graph->arc_list_head->next; p != graph->arc_list_tail; p = p->next){
    if(p->arc->selected){
      graph->selected_arc = p->arc;
      return p->arc;
    }
  }
  return (Arc *)NULL;
}

/*************************************************************************
 *     static void XcodaEditorUnselectNode(graph, p)                     *
 * Description:                                                          *
 *     unselect a node if it has been selected before                    *
 ************************************************************************/
#if defined (__STDC__)
static void XcodaEditorUnselectNode(XcodaEditorGraph* graph, 
				    drawComp* p)
#else
static void XcodaEditorUnselectNode(graph, p)
     XcodaEditorGraph *graph;
     drawComp         *p;
#endif
{
  p->selected = 0;
  graph->selected_comp = (drawComp *)NULL;
  (*p->unhigh_light)(p, xgc.dpy, XtWindow(sw_geometry.draw_area));
}


/************************************************************************
 *     static void XcodaEditorUnselectArc(graph, arc)                   *
 * Description:                                                         *
 *      unselect a arc if it has benn selected                          *
 ***********************************************************************/
#if defined (__STDC__)
static void XcodaEditorUnselectArc(XcodaEditorGraph* graph, 
				   Arc* arc)
#else
static void XcodaEditorUnselectArc(graph, arc)
     XcodaEditorGraph *graph;
     Arc              *arc;
#endif
{
  arc->selected = 0;
  graph->selected_arc = (Arc *)NULL;
  (*arc->draw_original)(arc, xgc.dpy, XtWindow(sw_geometry.draw_area));
}


/************************************************************************
 *     static void XcodaEditorSelectNode(graph, p)                      *
 * Description:                                                         *
 *     select a node or high light a node when you click on it          *
 ***********************************************************************/
#if defined (__STDC__)
static void XcodaEditorSelectNode(XcodaEditorGraph* graph, 
				  drawComp* p)
#else
static void XcodaEditorSelectNode(graph, p)
     XcodaEditorGraph *graph;
     drawComp *p;
#endif
{
  p->selected = 1;
  graph->selected_comp = p;
  (*p->high_light)(p,xgc.dpy, XtWindow(sw_geometry.draw_area));
}

/***********************************************************************
 *        static void XcodaEditorHighLightPort(graph, port)            *
 * Description:                                                        *
 *     High light selected ip_port                                     *
 **********************************************************************/
#if defined (__STDC__)
static void XcodaEditorHighLightPort(XcodaEditorGraph* graph, 
				     ipPort* port)
#else
static void XcodaEditorHighLightPort(graph, port)
     XcodaEditorGraph *graph;
     ipPort           *port;
#endif
{
  (*port->arm)(port, xgc.dpy, XtWindow(sw_geometry.draw_area));
  graph->from_port = port;
  XcodaEditorHighLightAllConnectedPorts(graph, port);
}

/************************************************************************
 *      static void XcodaEditorHighLightAllConnectedPorts(graph, port)  *
 * Description:                                                         *
 *    High light all ports which are connected to port 'port'           *
 ***********************************************************************/
#if defined (__STDC__)
static void XcodaEditorHighLightAllConnectedPorts(XcodaEditorGraph* graph, 
						  ipPort* port)
#else
static void XcodaEditorHighLightAllConnectedPorts(graph, port)
     XcodaEditorGraph *graph;
     ipPort *port;
#endif
{
  Arc **from_arcs;
  Arc **to_arcs;
  int num_from_arcs, num_to_arcs;
  int i;
  ipPort *p;

  from_arcs = XcodaEditorGetFromArcs(graph,port,&num_from_arcs);
  if(num_from_arcs != 0){
    for(i=0;i<num_from_arcs;i++){
      p = from_arcs[i]->to;
      (*p->output_high_light)(p,xgc.dpy,XtWindow(sw_geometry.draw_area));
    }
  }
  to_arcs = XcodaEditorGetToArcs(graph, port, &num_to_arcs);
  if(num_to_arcs != 0){
    for(i=0;i<num_to_arcs;i++){
      p = to_arcs[i]->from;
      (*p->input_high_light)(p,xgc.dpy,XtWindow(sw_geometry.draw_area));
    }
  }
  if (num_from_arcs)
    free (from_arcs);
  if (num_to_arcs)
    free (to_arcs);
}


/************************************************************************
 *      static void XcodaEditorUnhighLightAllConnectedPorts(graph, port)*
 * Description:                                                         *
 *    unhigh light all ports which are connected to port 'port'         *
 ***********************************************************************/
#if defined (__STDC__)
static void XcodaEditorUnhighLightAllConnectedPorts(XcodaEditorGraph* graph, 
						    ipPort* port)
#else
static void XcodaEditorUnhighLightAllConnectedPorts(graph, port)
     XcodaEditorGraph *graph;
     ipPort *port;
#endif
{
  Arc **from_arcs;
  Arc **to_arcs;
  int num_from_arcs, num_to_arcs;
  int i;
  ipPort *p;

  from_arcs = XcodaEditorGetFromArcs(graph,port,&num_from_arcs);
  if(num_from_arcs != 0){
    for(i=0;i<num_from_arcs;i++){
      p = from_arcs[i]->to;
      (*p->disarm)(p,xgc.dpy,XtWindow(sw_geometry.draw_area));
    }
  }
  to_arcs = XcodaEditorGetToArcs(graph, port, &num_to_arcs);
  if(num_to_arcs != 0){
    for(i=0;i<num_to_arcs;i++){
      p = to_arcs[i]->from;
      (*p->disarm)(p,xgc.dpy,XtWindow(sw_geometry.draw_area));
    }
  }
  if (num_from_arcs)
    free (from_arcs);
  if (num_to_arcs)
    free (to_arcs);
}


/************************************************************************
 *            static void XcodaEditorSelecteArc(graph, arc)             *
 * Description:                                                         *
 *      High light a selected arc                                       *
 ***********************************************************************/
#if defined (__STDC__)
static void XcodaEditorSelectArc(XcodaEditorGraph* graph, 
				 Arc* arc)
#else
static void XcodaEditorSelectArc(graph, arc)
     XcodaEditorGraph *graph;
     Arc              *arc;
#endif
{
  arc->selected = 1;
  graph->selected_arc = arc;
  (*arc->high_light)(arc, xgc.dpy, XtWindow(sw_geometry.draw_area));
}


/*****************************************************************
 *        Arc *XcodaEditorGetArcBetweenPorts()                   *
 * Description:                                                  *
 *     get Arc object between two ports                          *
 ****************************************************************/
#if defined (__STDC__)
Arc *XcodaEditorGetArcBetweenPorts(XcodaEditorGraph* graph, 
				   ipPort* port1, 
				   ipPort* port2)
#else
Arc *XcodaEditorGetArcBetweenPorts(graph, port1, port2)
     XcodaEditorGraph *graph;
     ipPort *port1, *port2;
#endif
{
  arcList  *p;

  if(graph->arc_list_head->next == graph->arc_list_tail)
    return (Arc *)NULL;

  for(p = graph->arc_list_head->next; p != graph->arc_list_tail; p=p->next){
    if((p->arc->from == port1 && p->arc->to == port2) ||
       (p->arc->from == port2 && p->arc->to == port1) )
      return p->arc;      
  }
  return (Arc *)NULL;
}

/*****************************************************************
 *        Arc *XcodaEditorGetArcBetweenNodes()                   *
 * Description:                                                  *
 *     get Arc object between two Nodes                          *
 *     CODA only allow one connection between two nodes          *
 *     At this moment                                            *
 ****************************************************************/
#if defined (__STDC__)
Arc *XcodaEditorGetArcBetweenNodes(XcodaEditorGraph* graph, 
				   drawComp* node1, 
				   drawComp* node2)
#else
Arc *XcodaEditorGetArcBetweenNodes(graph, node1, node2)
     XcodaEditorGraph *graph;
     drawComp         *node1, *node2;
#endif
{
  arcList  *p;
  Arc      *ret_arc;
  int      i, j;

  if(graph->arc_list_head->next == graph->arc_list_tail)
    return (Arc *)NULL;

  for(i=0;i<node1->num_ports;i++){
    for(j=0;j<node2->num_ports;j++){
      ret_arc = XcodaEditorGetArcBetweenPorts(graph, 
	        &(node1->ip_port[i]),&(node2->ip_port[j]));
      if(ret_arc != NULL)
	return ret_arc;
    }
  }
  return (Arc *)NULL;
}

/****************************************************************
 *            Arc **XcodaEditorGetFromArcs(graph, comp, num)    *
 * Description:                                                 *
 *    return Arc list which are arcs from a port                *
 ***************************************************************/
#if defined (__STDC__)
Arc **XcodaEditorGetFromArcs(XcodaEditorGraph* graph, 
			     ipPort* port, 
			     int* num)
#else
Arc **XcodaEditorGetFromArcs(graph, port, num)
     XcodaEditorGraph *graph;
     ipPort           *port;
     int              *num;
#endif
{
  Arc **list;
  int i = 0;
  arcList *p;

  for(p=graph->arc_list_head->next; p!=graph->arc_list_tail;p=p->next){
    if(p->arc->from == port)
      i++;
  }
  if(i != 0){
    list = (Arc **)malloc(i*sizeof(Arc *));
    *num = i;
    i = 0;
    for(p=graph->arc_list_head->next; p!=graph->arc_list_tail;p=p->next){
      if(p->arc->from == port){
	list[i] = p->arc;
	i++;
      }
    }  
  }
  else{
    *num = 0;
    list = (Arc **)NULL;
  }
  return list;
}
      

/****************************************************************
 *     Arc **XcodaEditorGetToArcs(graph, comp, num)             *
 * Description:                                                 *
 *    return Arc list which are arcs to a port                  *
 ***************************************************************/
#if defined (__STDC__)
Arc **XcodaEditorGetToArcs(XcodaEditorGraph* graph, 
			   ipPort* port, 
			   int* num)
#else
Arc **XcodaEditorGetToArcs(graph, port, num)
     XcodaEditorGraph *graph;
     ipPort           *port;
     int              *num;
#endif
{
  Arc **list;
  int i = 0;
  arcList *p;

  for(p=graph->arc_list_head->next; p!=graph->arc_list_tail;p=p->next){
    if(p->arc->to == port)
      i++;
  }
  
  if(i != 0){
    list = (Arc **)malloc(i*sizeof(Arc *));
    *num = i;
    i = 0;
    for(p=graph->arc_list_head->next; p!=graph->arc_list_tail;p=p->next){
      if(p->arc->to == port){
	list[i] = p->arc;
	i++;
      }
    }  
  }
  else{
    *num = 0;
    list = (Arc **)NULL;
  }
  return list;
}

/****************************************************************
 *      char **XcodaEditorCompGetAllInputs(graph, comp, &int)   *
 * Description:                                                 *
 *    get all the names of component which are the inputs       *
 *    of component of component 'comp'                          *
 ***************************************************************/
#if defined (__STDC__)
char **XcodaEditorCompGetAllInputs(XcodaEditorGraph* graph, 
				   drawComp* comp, 
				   int* num_inputs)
#else
char **XcodaEditorCompGetAllInputs(graph, comp, num_inputs)
     XcodaEditorGraph *graph;
     drawComp         *comp;
     int              *num_inputs;
#endif
{
  int    i, j, k, num_to_arcs;
  ipPort *port;
  char   **inputs;
  daqComp *daq;
  Arc    **to_arcs;
  char   temp[2048];
  

  i = 0;
  for(j=0;j<comp->num_ports;j++){
    port = &(comp->ip_port[j]);
    to_arcs = XcodaEditorGetToArcs(graph, port, &num_to_arcs);
    i = i + num_to_arcs;
    if (num_to_arcs)
      free (to_arcs);
  }
  if(i == 0){
    *num_inputs = 0;
    return (char **)NULL;
  }
  *num_inputs = i;
  inputs = (char **)malloc(i*sizeof(char *));
  if(inputs == NULL){
    fprintf(stderr,"cannot allocate memory for inputs\n");
    exit(1);
  }
  
  i = 0;
  for(j=0;j<comp->num_ports;j++){
    port = &(comp->ip_port[j]);
    to_arcs = XcodaEditorGetToArcs(graph, port, &num_to_arcs);
    bzero(temp,sizeof(temp));
    for(k=i;k<i+ num_to_arcs;k++){
      if (to_arcs[k-i]->from->comp_ptr->type == CODA_FILE)
	strcpy(temp,to_arcs[k-i]->from->comp_ptr->comp_name);
      else if (to_arcs[k-i]->from->comp_ptr->type == CODA_CODAFILE)
	strcpy(temp,to_arcs[k-i]->from->comp_ptr->comp_name);		
      else if (to_arcs[k-i]->from->comp_ptr->type == CODA_DEBUG)
	strcpy(temp, "DEBUG");
      else if (to_arcs[k-i]->from->comp_ptr->type == CODA_MON)
	strcpy(temp, "MON");
      else if (to_arcs[k-i]->from->comp_ptr->type == CODA_NONE)
	strcpy(temp, "NONE");
      else {
	strcpy(temp,to_arcs[k-i]->from->comp_ptr->comp_name);
	strcat(temp,":");
	strcat(temp,to_arcs[k-i]->from->ip_addr);
      }
      inputs[k] = strsave(temp);
    }
    i = i + num_to_arcs;
    if (num_to_arcs)
      free (to_arcs);
  }
  return inputs;
}


/****************************************************************
 *      char **XcodaEditorCompGetAlloutputs(graph, comp, &int)  *
 * Description:                                                 *
 *    get all the names of component which are the outputs      *
 *    of component of component 'comp'                          *
 ***************************************************************/
#if defined (__STDC__)
char **XcodaEditorCompGetAllOutputs(XcodaEditorGraph* graph, 
				    drawComp* comp, 
				    int *num_outputs)
#else
char **XcodaEditorCompGetAllOutputs(graph, comp, num_outputs)
     XcodaEditorGraph *graph;
     drawComp         *comp;
     int              *num_outputs;
#endif
{
  int    i, j, k, num_from_arcs;
  ipPort *port;
  char   **outputs;
  daqComp *daq;
  Arc    **from_arcs;
  char   temp[2048];
  

  i = 0;
  for(j=0;j<comp->num_ports;j++){
    port = &(comp->ip_port[j]);
    from_arcs = XcodaEditorGetFromArcs(graph, port, &num_from_arcs);
    i = i + num_from_arcs;
    if (num_from_arcs)
      free (from_arcs);
  }
  if((i == 0)||(comp->comp.type > 11)){
    *num_outputs = 0;
    return (char **)NULL;
  }
  *num_outputs = i;
  outputs = (char **)malloc(i*sizeof(char *));
  if(outputs == NULL){
    fprintf(stderr,"cannot allocate memory for inputs\n");
    exit(1);
  }
  
  i = 0;
  for(j=0;j<comp->num_ports;j++){
    port = &(comp->ip_port[j]);
    from_arcs = XcodaEditorGetFromArcs(graph, port, &num_from_arcs);
    bzero(temp,sizeof(temp));
    for(k=i;k<i+ num_from_arcs;k++){
      if (from_arcs[k-i]->to->comp_ptr->type == CODA_FILE)
	strcpy(temp,from_arcs[k-i]->to->comp_ptr->comp_name);	
      else if (from_arcs[k-i]->to->comp_ptr->type == CODA_CODAFILE)
	strcpy(temp,from_arcs[k-i]->to->comp_ptr->comp_name);	
      else if (from_arcs[k-i]->to->comp_ptr->type == CODA_DEBUG) 
	strcpy(temp,from_arcs[k-i]->to->comp_ptr->comp_name);
      else if (from_arcs[k-i]->to->comp_ptr->type == CODA_MON) 
	strcpy(temp,from_arcs[k-i]->to->comp_ptr->comp_name);
      else if (from_arcs[k-i]->to->comp_ptr->type == CODA_NONE) 
	strcpy(temp,from_arcs[k-i]->to->comp_ptr->comp_name);
      else {
	strcpy(temp,from_arcs[k-i]->to->comp_ptr->comp_name);
	strcat(temp,":");
	strcat(temp,from_arcs[k-i]->to->ip_addr);
      }
      outputs[k] = strsave(temp);
    }
    i = i + num_from_arcs;
    if (num_from_arcs)
      free (from_arcs);
  }
  return outputs;
}

/****************************************************************
 *      int compHasOutputs(comp)                                *
 * Description:                                                 *
 *      Check a component has any outputs                       *
 ***************************************************************/
#if defined (__STDC__)
int compHasOutputs(drawComp* comp)
#else
int compHasOutputs(comp)
     drawComp         *comp;
#endif
{
  char** outputs;
  int    num = 0;
  int    i = 0;

  outputs = XcodaEditorCompGetAllOutputs (&coda_graph, comp, &num);

  if (num > 0) {
    for (i = 0; i < num; i++)
      free (outputs[i]);
    free (outputs);
    return 1;
  }
  return 0;
}


/****************************************************************
 *      static void delete_arc(graph, w, event)                 *
 * Description:                                                 *
 *     Delete a connection between nodes                        *
 ***************************************************************/ 
#if defined (__STDC__)
static void delete_arc(XcodaEditorGraph* graph, 
		       Widget w, 
		       XEvent* event)
#else
static void delete_arc(graph, w, event)
     XcodaEditorGraph *graph;
     Widget           w;
     XEvent           *event;
#endif
{
  static Arc *ret_arc = NULL; /* rememebr it for undo */
  Arc    **from_arcs;
  Arc    **to_arcs;
  int    num_from_arcs;
  int    num_to_arcs;
  int    i;
  
  if(graph->current_action == DELETE_ARC_ACTION){
    if(ret_arc != NULL){  /* it is time to free old arc */
      free(ret_arc);
      ret_arc = (Arc *)NULL;
    }
    ret_arc = XcodaEditorClickedArc(graph,event);
    if(ret_arc == NULL){
      XUndefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area));
      graph->current_action = NORMAL_ACTION;
    }
    else{
      graph->previous_action = DELETE_ARC_ACTION;
      XUndefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area));
      graph->current_action = NORMAL_ACTION;
      if(ret_arc->selected)
	graph->selected_arc = (Arc *)NULL;
      removeArcFromWholeArcList(graph, ret_arc);
      (*ret_arc->erase)(ret_arc, xgc.dpy, XtWindow(sw_geometry.draw_area));
/* I have to redraw all other arcs from and to at this ports */
      from_arcs = XcodaEditorGetFromArcs(graph, ret_arc->from, &num_from_arcs);
      if(num_from_arcs != 0){
	for(i=0;i<num_from_arcs;i++){
	  if(from_arcs[i]->selected)
	    (*from_arcs[i]->high_light)(from_arcs[i],xgc.dpy,
					XtWindow(sw_geometry.draw_area));
	  else
	    (*from_arcs[i]->draw_original)(from_arcs[i],xgc.dpy,
					   XtWindow(sw_geometry.draw_area));
	}
	free (from_arcs);
      }
      to_arcs = XcodaEditorGetToArcs(graph, ret_arc->from, &num_to_arcs);
      if(num_to_arcs != 0){
	for(i=0;i<num_to_arcs;i++){
	  if(to_arcs[i]->selected)
	    (*to_arcs[i]->high_light)(to_arcs[i],xgc.dpy,
				      XtWindow(sw_geometry.draw_area));
	  else
	    (*to_arcs[i]->draw_original)(to_arcs[i],xgc.dpy,
					 XtWindow(sw_geometry.draw_area));
	}
	free (to_arcs);
      }

      from_arcs = XcodaEditorGetFromArcs(graph, ret_arc->to, &num_from_arcs);
      if(num_from_arcs != 0){
	for(i=0;i<num_from_arcs;i++){
	  if(from_arcs[i]->selected)
	    (*from_arcs[i]->high_light)(from_arcs[i],xgc.dpy,
					XtWindow(sw_geometry.draw_area));
	  else
	    (*from_arcs[i]->draw_original)(from_arcs[i],xgc.dpy,
					   XtWindow(sw_geometry.draw_area));
	}
	free (from_arcs);
      }
      to_arcs = XcodaEditorGetToArcs(graph, ret_arc->to, &num_to_arcs);
      if(num_to_arcs != 0){
	for(i=0;i<num_to_arcs;i++){
	  if(to_arcs[i]->selected)
	    (*to_arcs[i]->high_light)(to_arcs[i],xgc.dpy,
				      XtWindow(sw_geometry.draw_area));
	  else
	    (*to_arcs[i]->draw_original)(to_arcs[i],xgc.dpy,
					 XtWindow(sw_geometry.draw_area));
	}
	free (to_arcs);
      }
    }
  }
  else{
    if(ret_arc != NULL){ /* insert ret_arc to list again */
      insertArcToWholeArcList(graph, ret_arc);
      if(ret_arc->selected){
	graph->selected_arc = ret_arc;
	(*ret_arc->high_light)(ret_arc, xgc.dpy, XtWindow(sw_geometry.draw_area));
      }
      else
	(*ret_arc->draw_original)(ret_arc, xgc.dpy, XtWindow(sw_geometry.draw_area));
    }
    /* set arc pointer to 0 to prevent from beging deleted */
    ret_arc = 0;
  }
  XClearWindow(xgc.dpy, XtWindow(sw_geometry.draw_area));
  (*graph->redisplay)(graph, w, event);

}

/***************************************************************
 *            static void delete_comp(graph, w, event)         *
 * Description:                                                *
 *     delete a component under the pirate cursor              *
 *     also does undo here                                     *
 **************************************************************/
#if defined (__STDC__)
static void delete_comp(XcodaEditorGraph* graph, 
			Widget w, 
			XEvent* event)
#else
static void delete_comp(graph, w, event)
     XcodaEditorGraph *graph;
     Widget           w;
     XEvent           *event;
#endif
{
  static drawComp *comp = NULL;
  static Arc *arcs[200];
  static int num_arcs = 0;
  Arc    **from_arcs;
  Arc    **to_arcs;
  int    num_from_arcs;
  int    num_to_arcs;
  int    i,j,k;

  if(graph->current_action == DELETE_NODE_ACTION){
    if(comp != NULL){ /* free old node */
      free_daqcomp_res (&(comp->comp)); /* free daqComp resources */
      free_ipPort_res (comp->ip_port, comp->num_ports);
      free(comp);
      comp = (drawComp *)NULL;
      for(i=0;i<num_arcs;i++)
	free(arcs[i]);
      num_arcs = 0;
    }
    comp = XcodaEditorClickedNode(graph, event);
    if(comp == NULL){
      XUndefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area));
      graph->current_action = NORMAL_ACTION;
    }
    else{
      graph->previous_action = DELETE_NODE_ACTION;
      XUndefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area));
      graph->current_action = NORMAL_ACTION;
      if(comp->selected)
	graph->selected_comp = (drawComp *)NULL;
      removeNodeFromCompList(graph, comp);
      (*comp->erase)(comp,xgc.dpy,XtWindow(sw_geometry.draw_area));
      /* erase all arcs from and to this node as well */
      k = 0;
      for(i=0;i<comp->num_ports;i++){
	from_arcs = XcodaEditorGetFromArcs(graph,&(comp->ip_port[i]),&num_from_arcs);
	for(j=0;j<num_from_arcs;j++)
	  arcs[k++] = from_arcs[j];
	to_arcs = XcodaEditorGetToArcs(graph,&(comp->ip_port[i]),&num_to_arcs);
	for(j=0;j<num_to_arcs;j++)
	  arcs[k++] = to_arcs[j];
      }
      num_arcs = k;
      for(i=0;i<num_arcs;i++){
	if(arcs[i]->selected)
	  graph->selected_arc = (Arc *)NULL;
	removeArcFromWholeArcList(graph, arcs[i]);
	(*arcs[i]->erase)(arcs[i], xgc.dpy, XtWindow(sw_geometry.draw_area));
      }
      if (num_from_arcs)
	free(from_arcs);
      if (num_to_arcs)
	free(to_arcs);
      XClearWindow(xgc.dpy, XtWindow(sw_geometry.draw_area));
      (*graph->redisplay)(graph, w, event);
    }
  }
  else{ /* undo action */
    if(comp != NULL){ /*insert this comp back to list again */
      insertNodeToCompList(graph, comp);
      if(comp->selected){
	graph->selected_comp = comp;
	(*comp->high_light)(comp, xgc.dpy,XtWindow(sw_geometry.draw_area));
      }
      else
	(*comp->unhigh_light)(comp, xgc.dpy,XtWindow(sw_geometry.draw_area));
    }
    for(i=0;i<num_arcs;i++){
      if(arcs[i]->selected){
	graph->selected_arc = arcs[i];
	(*arcs[i]->high_light)(arcs[i],xgc.dpy, 
			       XtWindow(sw_geometry.draw_area));
      }
      else
	(*arcs[i]->draw_original)(arcs[i],xgc.dpy, 
				  XtWindow(sw_geometry.draw_area));
      insertArcToWholeArcList(graph, arcs[i]);
    }
    /* set component pointer to zero since this one cannot be deleted */
    comp = 0;
  }
  XClearWindow(xgc.dpy, XtWindow(sw_geometry.draw_area));
  (*graph->redisplay)(graph, w, event);	       
}
	  
/**************************************************************************
 *         void delete_all_arcs(w, client_data, cbs)                      *
 * description:                                                           *
 * Delete all connections                                                 *
 **************************************************************************/
#if defined (__STDC__)
void delete_all_arcs(Widget w, 
		     XtPointer client_data, 
		     XmAnyCallbackStruct* cbs)
#else
void delete_all_arcs(w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmAnyCallbackStruct *cbs;
#endif
{
  arcList *p;
  arcList *q;

  if(coda_graph.arc_list_head->next == coda_graph.arc_list_tail)
    return;

  p = coda_graph.arc_list_head->next;
  while(p != coda_graph.arc_list_tail){
    q = p->next;
    free(p->arc);
    free(p);
    p = q;
  }
  coda_graph.arc_list_head->next = coda_graph.arc_list_tail;
  coda_graph.arc_list_head->previous = (arcList *)NULL;
  coda_graph.arc_list_tail->next = (arcList *)NULL;
  coda_graph.arc_list_tail->previous = coda_graph.arc_list_head;
  XClearWindow(xgc.dpy, XtWindow(sw_geometry.draw_area));
  (*coda_graph.redisplay)(&coda_graph, sw_geometry.draw_area, cbs->event);
}

/***************************************************************************
 *          void delete_everything(w, client_data, cbs)                    *
 * Description:                                                            *
 *     Delete evenything inside graph                                      *
 **************************************************************************/
#if defined (__STDC__)
void delete_everything(Widget w,
		       XtPointer client_data,
		       XmAnyCallbackStruct* cbs)
#else
void delete_everything(w,client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmAnyCallbackStruct *cbs;
#endif
{
  compList *qq, *pp;
  arcList  *q,*p;
  int      i;


  p = coda_graph.arc_list_head->next;
  while(p != coda_graph.arc_list_tail){
    q = p->next;
    free(p->arc);
    free(p);
    p = q;
  }
  coda_graph.arc_list_head->next = coda_graph.arc_list_tail;
  coda_graph.arc_list_head->previous = (arcList *)NULL;
  coda_graph.arc_list_tail->next = (arcList *)NULL;
  coda_graph.arc_list_tail->previous = coda_graph.arc_list_head;

  pp = coda_graph.comp_list_head->next;
  while(pp != coda_graph.comp_list_tail){
    qq = pp->next;
    for (i = 0; i < pp->draw_comp->num_ports; i++) {
      if ((pp->draw_comp->ip_port[i]).ip_addr)
	free ((pp->draw_comp->ip_port[i]).ip_addr);
    }
    free_daqcomp_res (&(pp->draw_comp->comp));
    free(pp->draw_comp);
    free(pp);
    pp = qq;
  }
  coda_graph.comp_list_head->next = coda_graph.comp_list_tail;
  coda_graph.comp_list_head->previous = (compList *)NULL;
  coda_graph.comp_list_tail->next = (compList *)NULL;
  coda_graph.comp_list_tail->previous = coda_graph.comp_list_head;
  XClearWindow(xgc.dpy, XtWindow(sw_geometry.draw_area));
  if (cbs)
    (*coda_graph.redisplay)(&coda_graph, sw_geometry.draw_area, cbs->event);
  /* reset modified flag for the graph */
  coda_graph.modified = 0;
}

/******************************************************************
 *    drawComp *findDrawCompFromPort()                            *
 * Descritption:                                                  *
 *     find drawComp on which the port sits                       *
 *****************************************************************/
#if defined (__STDC__)
drawComp *findDrawCompFromPort(XcodaEditorGraph* graph, 
			       ipPort* port)
#else
drawComp *findDrawCompFromPort(graph, port)
     XcodaEditorGraph *graph;
     ipPort           *port;
#endif
{
  compList *p;

  for(p=graph->comp_list_head->next; p!=graph->comp_list_tail;p=p->next){
    if (p->draw_comp->comp.comp_name)
      if(strcmp(p->draw_comp->comp.comp_name, port->comp_ptr->comp_name) == 0)
	break;
  }
  return p->draw_comp;
}


/*****************************************************************
 *     void XcodaEditorUpdateEditableFlag(graph)                 *
 * Description:                                                  *
 *     Upon saving a config file, one has to mark all node       *
 *     on the panel as uneditable                                *
 ****************************************************************/
#if defined (__STDC__)
void XcodaEditorUpdateEditableFlag(XcodaEditorGraph* graph)
#else
void XcodaEditorUpdateEditableFlag(graph)
     XcodaEditorGraph *graph;
#endif
{
  compList *p;

  if(graph->comp_list_head->next == graph->comp_list_tail)
    return;

  for(p=graph->comp_list_head->next; p != graph->comp_list_tail; p = p->next){
    if(p->draw_comp->editable)
      p->draw_comp->editable = 0;
  }
}

/***************************Utilities***************************/
#if defined (__STDC__)
void setup_drawcomp_func(drawComp* dc)
#else
void setup_drawcomp_func(dc)
     drawComp *dc;
#endif
{
  dc->draw_original = draw_original;
  dc->draw_rubber_comp = draw_rubber_comp;
  dc->high_light = draw_high_light;
  dc->unhigh_light = draw_unhigh_light;
  dc->erase = erase_comp;
  dc->write_name = draw_comp_name;
  dc->erase_name = erase_comp_name;
  dc->move_name = move_comp_name;
  dc->write_hostname = draw_host_name;
  dc->erase_hostname = erase_host_name;
  dc->move_hostname = move_host_name;
  dc->set_attributes = popup_comp_attributes;
}

#if defined (__STDC__)
void setup_ipport_func(ipPort* port)
#else
void setup_ipport_func(port)
     ipPort *port;
#endif
{
  port->arm = arm_ip_port;
  port->disarm = disarm_ip_port;
  port->input_high_light = high_light_input_port;
  port->output_high_light = high_light_output_port;
  port->set_attributes = popup_ipport_attributes;
  port->erase_name = erase_port_name;
  port->write_name = draw_port_name;
  port->move_name = draw_rubber_name;
}

#if defined (__STDC__)
void setup_arc_func(Arc *arc)
#else
void setup_arc_func(arc)
     Arc *arc;
#endif
{
  arc->draw_original = draw_original_arc;
  arc->high_light = draw_high_light_arc;
  arc->draw_rubber_arc = draw_rubber_arc;
  arc->erase = erase_arc;
}

#if defined (__STDC__)  
static int isOverlapAny(XcodaEditorGraph* graph,
			drawComp *q,
			int col, int row) /* check every one */
#else
static int isOverlapAny(graph, col, row) /* check every one */
     XcodaEditorGraph *graph;
     drawComp *q;
     int col, row;
#endif
{
  compList *p;
  drawComp *qq;

  if(graph->comp_list_head->next != graph->comp_list_tail){
    for(p = graph->comp_list_head->next; p != graph->comp_list_tail; p=p->next){
      qq = p->draw_comp;
      if((qq!=q) && (col >= qq->col && col <= qq->col + qq->num_col - 1) &&
	 (row >= qq->row && row <= qq->row + qq->num_row - 1))
	return(1);
    }
    return (0);
  }
  else
    return (0);
}

#if defined (__STDC__)
int isOneCoverAnother(XcodaEditorGraph* graph, 
		      drawComp* p, 
		      int col, int row)  /* check every one except p */
#else
int isOneCoverAnother(graph, p, col, row)  /* check every one except p */
     XcodaEditorGraph *graph;
     drawComp *p;
     int col, row;
#endif
{
  compList *q;
  drawComp *qq;

  if(graph->comp_list_head->next != graph->comp_list_tail){
    for(q = graph->comp_list_head->next; q != graph->comp_list_tail; q = q->next){
      qq = q->draw_comp;
      if(qq != p){
	if((col >= qq->col && col <= qq->col + qq->num_col - 1) &&
	   (row >= qq->row && row <= qq->row + qq->num_row - 1))
	  return(1);	
      }
    }
    return (0);
  }
  else
    return (0);
}

#if defined (__STDC__)
static int isDoubleClick(XcodaEditorGraph* graph, 
			 XEvent* event)
#else
static int isDoubleClick(graph, event)
     XcodaEditorGraph *graph;
     XEvent           *event;
#endif
{
  if(graph->last_button_time  &&
     (event->xbutton.time - graph->last_button_time) < graph->double_click_interval){
    graph->last_button_time = 0;
    return 1;
  }
  graph->last_button_time = event->xbutton.time;
  return 0;
}

/************double linked list utilities*********************/
#if defined (__STDC__)
void insertNodeToCompList(XcodaEditorGraph* graph, 
			  drawComp* p)
#else
void insertNodeToCompList(graph, p)
     XcodaEditorGraph *graph;
     drawComp *p;
#endif
{
  compList *q, *qq;

  q = (compList *)malloc(1*sizeof(compList));
  if(q == NULL){
    fprintf(stderr,"Cannot allocate memory for insert comp to list\n");
    exit(1);
  }
  q->draw_comp = p;

  qq = graph->comp_list_head->next;
  qq->previous = q;
  q->next = qq;
  q->previous = graph->comp_list_head;
  graph->comp_list_head->next = q;
  /* graph has been modified */
  graph->modified = 1;
}

#if defined (__STDC__)
void removeNodeFromCompList(XcodaEditorGraph* graph, 
			    drawComp* p)
#else
void removeNodeFromCompList(graph, p)
     XcodaEditorGraph *graph;
     drawComp *p;
#endif
{
  compList *q,*qq,*pp;

  for(q=graph->comp_list_head->next;q!=graph->comp_list_tail;q=q->next){
    if(p == q->draw_comp)
      break;
  }
  qq = q->previous;
  pp = q->next;
  qq->next = pp;
  pp->previous = qq;
  /* graph has been modified */
  graph->modified = 1;
}

#if defined (__STDC__)
int isEmptyGraph(XcodaEditorGraph* graph)
#else
int isEmptyGraph(graph)
     XcodaEditorGraph *graph;
#endif
{
  if(graph->comp_list_head->next == graph->comp_list_tail)
    return 1;
  else
    return 0;
}

#if defined (__STDC__)
int isGraphModified(XcodaEditorGraph* graph)
#else
int isGraphModified(graph)
     XcodaEditorGraph *graph;
#endif
{
  return graph->modified;
}


/*******insert an arc to graph arc list***************************/
#if defined (__STDC__)
void insertArcToWholeArcList(XcodaEditorGraph* graph, 
			     Arc* arc)
#else
void insertArcToWholeArcList(graph, arc)
     XcodaEditorGraph *graph;
     Arc              *arc;
#endif
{
  arcList *p,*q,*qq;

  q = (arcList *)malloc(sizeof(arcList));
  if(q == NULL){
    fprintf(stderr,"Cannot allocate memory for arcList in insert arc routine\n");
    exit(1);
  }
  q->arc = arc;

  /*  p = graph->arc_list_head;
  qq = p->next;
  q->previous = p;
  q->next = qq;
  p->next = q;
  qq->previous = q;*/
  /* add to the end of list */
  p = graph->arc_list_tail;
  qq = p->previous;
  q->next = p;
  p->previous = q;
  q->previous = qq;
  qq->next = q;
  /* graph has been modified */
  graph->modified = 1;
}

/*******remove an arc from graph arc list*************************/
#if defined (__STDC__)
void removeArcFromWholeArcList(XcodaEditorGraph* graph, 
			       Arc* arc)
#else
void removeArcFromWholeArcList(graph, arc)
     XcodaEditorGraph *graph;
     Arc              *arc;
#endif
{
  arcList *p,*q,*qq;

  for(p = graph->arc_list_head->next; p != graph->arc_list_tail; p = p->next){
    if(p->arc == arc)
      break;
  }
  q = p->previous;
  qq = p->next;
  q->next = qq;
  qq->previous = q;
  /* graph has been modified */
  graph->modified = 1;
}

/*************Free resources associated with daqComp structure**************/
#if defined (__STDC__)
void free_daqcomp_res (daqComp* comp)
#else
void free_daqcomp_res (comp)
     daqComp *comp;
#endif
{

  if (comp->comp_name){
    free (comp->comp_name);
    comp->comp_name = (char *)0;
  }
  if (comp->node_name){
    free (comp->node_name);
    comp->node_name = (char *)0;
  }
  if (comp->boot_string){
    free (comp->boot_string);
    comp->boot_string = (char *)0;
  }
  if (comp->code[0]){
    free (comp->code[0]);
    comp->code[0] = (char *)0;
  }
  if (comp->code[1]){
    free (comp->code[1]);
    comp->code[1] = (char *)0;
  }
  if (comp->code[2]){
    free (comp->code[2]);
    comp->code[2] = (char *)0;
  }
}

/***********************free ipPorts resources************************/
#if defined (__STDC__)
void free_ipPort_res (ipPort* ports, int num_ports)      
#else
void free_ipPort_res (ports, num_ports)      
     ipPort *ports;
     int    num_ports;
#endif
{
  int i;

  for (i = 0; i < num_ports; i++){
    if (ports[i].ip_addr){
      free (ports[i].ip_addr);
      ports[i].ip_addr = (char *)0;
    }
  }
}

    
