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
 *	CODA Editor graph drawing area header file
 *	
 * Author:  Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_graph.h,v $
 *   Revision 1.7  1997/09/08 17:27:16  heyes
 *   fixed memory free bugs (I think!)
 *
 *   Revision 1.6  1997/09/08 15:19:32  heyes
 *   fix dd icon etc
 *
 *   Revision 1.5  1997/08/01 18:37:51  heyes
 *   fixed moving bug
 *
 *   Revision 1.4  1997/07/22 12:16:44  heyes
 *   need to debug on solaris
 *
 *   Revision 1.3  1997/07/08 15:30:42  heyes
 *   add stuff
 *
 *   Revision 1.2  1997/06/20 17:00:12  heyes
 *   clean up GUI!
 *
 *   Revision 1.1.1.2  1996/11/05 17:45:11  chen
 *   coda source
 *
 *	  
 */
#ifndef _EDITOR_GRAPH_H
#define _EDITOR_GRAPH_H

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include "Editor.h"

#define IP_CONNECTION 300
#define MOVE_IP_NODE  301

#define NORMAL_ACTION                 100
#define ADD_TRIG_ACTION               101
#define ADD_ROC_ACTION                102
#define ADD_EB_ACTION                 103
#define ADD_MON_ACTION                104
#define ADD_FI_ACTION                 105
#define ADD_EXISTING_COMP             106
#define ADD_ER_ACTION                 107
#define ADD_FI2_ACTION                108
#define ADD_CFI_ACTION                109
#define ADD_DBG_ACTION                110
#define ADD_NONE_ACTION               111
/* you may put more new component type here */

#define DELETE_NODE_ACTION            120
#define DELETE_ARC_ACTION             121
#define HIGHLIGHT_IP_PORTS_ACTION     122
#define PRE_CONNECT_IP_PORTS_ACTION   123
#define PERS_CONNECT_IP_PORTS_ACTION  124
#define CONNECT_IP_PORTS_ACTION       125
#define UNDO_ACTION                   126
#define PRE_MOVE_NODE_ACTION          127
#define PERS_MOVE_NODE_ACTION         128
#define MOVE_NODE_ACTION              129
#define MOVE_ARC_ACTION               130
#define RESIZE_COMP_ACTION            131
#define CONN_2COLS_ACTION             132
#define ADD_TRANSACTION_ACTION        133

typedef struct _geometry{
  unsigned short cell_wd, cell_ht;     /* grid_size          */
  unsigned short slider_size_h;
  unsigned short slider_size_v;        /* scrollbar resource */
  int   max_row, min_row;              /* scroll bar max min row */
  int   max_col, min_col;
  unsigned short pageinc_h, pageinc_v; /* scroll bar page increment */
  int   sw_xoff, sw_yoff;              /* scrolled window offset */
  /* above are direct related to scrolled bar */
  int   zoomxoff, zoomyoff;            /* x, y offset from origin */
  unsigned short view_wd, view_ht;     /* view area size           */
  unsigned short old_view_wd, old_view_ht;      /* previous view size       */
  unsigned short pix_wd, pix_ht;                /* backup pixmap size       */
  unsigned short zoomscale;                     /* zoom scale current       */
  unsigned short old_zoomscale;
  Pixmap bg_pixmap;                    /* background pixmap        */
  Widget draw_area;                    /* darwing area             */
  Widget top_ruler;                    /* ruler widgets            */
  Widget left_ruler;
  Widget h_scb,v_scb;                  /* scroll bar               */
  Widget up_arrow, dn_arrow;           /* scale arrow buttons      */
}SwGeometry;

typedef struct _buttonPosition{
  int   rows, cols;                    /* pointer position in row coulmn*/
  int   x, y;                          /* screen coord                  */
}XpointerPosition;

typedef struct _xgc{
  GC      r_gc;
  GC      xor_gc;
  GC      erase_gc;
  GC      hl_gc;
  Display *dpy;
  Window  root_w;
  Screen  *scr;
  unsigned int depth;
  XFontStruct *font[6];
  Pixel   bg, fg;
  Pixel   White, Black, Yellow, Red;
  Pixel   Cyan,  Green, NavyBlue;
  Pixel   Gray0, Gray1;
  Pixel   Paused, Ending;
}XGC;

typedef struct _ip_port{
  daqComp  *comp_ptr;
  char     *ip_addr;
  int      index;  /* port number from top to bottom , start from 0 */
  int      left_x, left_y;
  int      right_x, right_y;
  int      row, col;
  unsigned short width, height;
  int      left_selected;
  int      right_selected;
#if defined (__STDC__)
  void     (*arm)(struct _ip_port* port, Display* dpy, Drawable win);
  void     (*input_high_light)(struct _ip_port* port, Display* dpy, Drawable win);
  void     (*output_high_light)(struct _ip_port* port, Display* dpy, Drawable win);
  void     (*disarm)(struct _ip_port* port, Display* dpy, Drawable win);
  void     (*set_attributes)(struct _ip_port* port, Widget w, XEvent* event);
  void     (*write_name)(struct _ip_port* port, Display* dpy, Drawable win);
  void     (*erase_name)(struct _ip_port* port, Display* dpy, Drawable win);
  void     (*move_name)(struct _ip_port* port, Display* dpy, Drawable win,
			int x, int y, Dimension wd, Dimension ht);
#else
  void     (*arm)();
  void     (*input_high_light)();
  void     (*output_high_light)();
  void     (*disarm)();
  void     (*set_attributes)();
  void     (*write_name)();
  void     (*erase_name)();
  void     (*move_name)();
#endif
}ipPort;

typedef struct _arc{
  int      type;       /* type of connection */
  int      arrow_type; /* arrow type         */
  XPoint   edge_point[8]; /* x, y and r, c coords */
  int      selected;
  int      from_row, from_col;
  int      to_row, to_col;
  ipPort   *from;
  ipPort   *to;
#if defined (__STDC__)
  void     (*draw_original)(struct _arc* arc, Display* dpy, Drawable win);
  void     (*draw_rubber_arc)(struct _arc* arc, Display* dpy, Drawable win,
			      int x, int y);	
  void     (*high_light)(struct _arc* arc, Display* dpy, Drawable win);
  void     (*erase)(struct _arc* arc, Display* dpy, Drawable win);
#else
  void     (*draw_original)();
  void     (*draw_rubber_arc)();
  void     (*high_light)();
  void     (*erase)();
#endif
}Arc;

typedef struct _arc_list{
  Arc      *arc;
  struct _arc_list *previous;
  struct _arc_list *next;
}arcList;


typedef struct _editor_drawcomp{
  daqComp     comp;
  ipPort      ip_port[MAX_NUM_PORTS]; /* ip ports                */
  int         num_ports;            /* num of ip port            */
  int         editable;             /* 0, no, 1, yes             */
  int         row,col;              /* top left row col number   */
  int         x, y;                 /* top left screen coord     */
  unsigned short  width, height;        /* size of comp geometry     */
  int         num_row, num_col;     /* number of row occupied etc */
  int         selected;             /* selected, 0 no            */
  codaScript* scripts;              /* run time user scripts     */
#if defined (__STDC__)
  void        (*draw_original)(struct _editor_drawcomp* comp, 
    Display* dpy,
    Drawable win,
    int x, int y,
    Dimension wd, Dimension ht,
    int r, int c);   /* draw original comp        */
  void        (*draw_rubber_comp)(struct _editor_drawcomp* comp,
    Display* dpy,
    Drawable win,
    int x, int y,
    Dimension wd,
    Dimension ht);/* draw rubber component     */
  void        (*erase)(struct _editor_drawcomp* comp,
    Display* dpy,
    Drawable win);           /* erase component           */
  void        (*high_light)(struct _editor_drawcomp* comp,
    Display* dpy,
    Drawable win);      /* selection of comp         */
  void        (*unhigh_light)(struct _editor_drawcomp* comp,
    Display* dpy,
    Drawable win);    /* unselect of comp          */
  void        (*write_name)(struct _editor_drawcomp* comp,
    Display* dpy,
    Drawable win);      /* draw comp name            */
  void        (*erase_name)(struct _editor_drawcomp* comp,
    Display* dpy,
    Drawable win);      /* erase comp name           */
  void        (*move_name)(struct _editor_drawcomp* comp,
    Display* dpy,
    Drawable win,
    int x, int y,
    Dimension wd,
    Dimension ht);       /* draw movable name         */
  void        (*write_hostname)(struct _editor_drawcomp* comp,
    Display* dpy,
    Drawable win);  /* draw host name            */
  void        (*erase_hostname)(struct _editor_drawcomp* comp,
    Display* dpy,
    Drawable win);  /* erase host name           */
  void        (*move_hostname)(struct _editor_drawcomp* comp,
    Display* dpy,
    Drawable win,
    int x, int y,
    Dimension wd, Dimension ht);   /* move hostname             */
  void        (*set_attributes)(struct _editor_drawcomp* comp,
    Widget base,
    XEvent* event,
    int     type);/* setup attributes dialog   */
#else
  void        (*draw_original)();   /* draw original comp        */
  void        (*draw_rubber_comp)();/* draw rubber component     */
  void        (*erase)();           /* erase component           */
  void        (*resize)();          /* resize drawing            */
  void        (*high_light)();      /* selection of comp         */
  void        (*unhigh_light)();    /* unselect of comp          */
  void        (*write_name)();      /* draw comp name            */
  void        (*erase_name)();      /* erase comp name           */
  void        (*move_name)();       /* draw movable name         */
  void        (*write_hostname)();  /* draw host name            */
  void        (*erase_hostname)();  /* erase host name           */
  void        (*move_hostname)();   /* move hostname             */
  void        (*set_attributes)();  /* setup attributes dialog   */
#endif
}drawComp;

typedef struct _comp_list{
  drawComp *draw_comp;
  struct _comp_list *next;
  struct _comp_list *previous;
}compList;

/* graph manager */
typedef struct _editor_graph {
  int      current_action;
  int      previous_action;
  Cursor   delete_cursor;        /* cursor type for delete    */
  Cursor   delete_arc_cursor;   
  Cursor   working_cursor;
  Cursor   finger_cursor;
  Time     last_button_time;
  int      double_click_interval;
  int      start_x, start_y;    /* rubber band drawing */
  int      end_x, end_y;       
  unsigned short    child_spacing;
  unsigned short    sibling_spacing;
  int      modified;
  compList *comp_list_head;
  compList *comp_list_tail;
  arcList  *arc_list_head;
  arcList  *arc_list_tail;
  drawComp *selected_comp;
  drawComp *current_comp;      /* current component    */
  Arc      *selected_arc;      /* selected arc         */
  Arc      *current_arc;       /* current_arc          */
  ipPort   *from_port;         /* first cliked port    */
#if defined (__STDC__)
  int     (*select_comp)(struct _editor_graph* graph,
    Widget w, XEvent* event);   /* left btn1 click      */
  void     (*highlight_ip_ports)(struct _editor_graph* graph,
    Widget w, XEvent* event);
                               /* high light all connected ports */
  void     (*redisplay)(struct _editor_graph* graph,
    Widget w, XEvent* event);     /* expose event         */
  void     (*zoom_scroll)(struct _editor_graph* graph,
    Widget w, XEvent* event);   /* zoom + scrolled event*/
  void     (*add_comp)(struct _editor_graph* graph,
    Widget w, XEvent* event);      /* add component        */
  void     (*add_existing_comp)(struct _editor_graph* graph,
    Widget w, XEvent* event); /* add an existing comp */
  void     (*delete_comp)(struct _editor_graph* graph,
    Widget w, XEvent* event);   /* delete compoent      */
  void     (*delete_arc)(struct _editor_graph* graph,
    Widget w, XEvent* event);    /* delete an arc        */
#else
  void     (*select_comp)();   /* left btn1 click      */
  void     (*highlight_ip_ports)();
                               /* high light all connected ports */
  void     (*redisplay)();     /* expose event         */
  void     (*zoom_scroll)();   /* zoom + scrolled event*/
  void     (*add_comp)();      /* add component        */
  void     (*add_existing_comp)(); /* add an existing comp */
  void     (*delete_comp)();   /* delete compoent      */
  void     (*delete_arc)();    /* delete an arc        */
#endif
}XcodaEditorGraph;
  
extern SwGeometry        sw_geometry;
extern XpointerPosition  xp_pos;
extern XGC               xgc;
extern XcodaEditorGraph  coda_graph;

/* functions usable for other functions */
#if defined (__STDC__)
extern void reset_scrollbar_res             (void);
extern void XcodaEditorTrackPointerPosition (Widget w,
					     XtPointer clientData,
					     XEvent* event);
extern void XcodaEditorUpdateEditableFlag   (XcodaEditorGraph* graph);
extern Widget XcodaEditorDrawingArea        (Widget parent);
extern void XcodaEditorResizeComp           (Widget w,
					     XtPointer clientData,
					     XmAnyCallbackStruct* cbs);
extern void XcodaEditorConn2Cols            (Widget w,
					     XtPointer clientData,
					     XmAnyCallbackStruct* cbs);
extern void XcodaEditorBgPixmap             (Widget w);
extern void XcodaEditorAddCompNodeMode      (Widget w, XtPointer data,
					     XmAnyCallbackStruct* cbs);
extern void XcodaEditorSetDeleteArcMode     (Widget w,
					     XtPointer clientData,
					     XmAnyCallbackStruct* cbs);
extern void XcodaEditorSetDeleteCompMode    (Widget w,
					     XtPointer clientData,
					     XmAnyCallbackStruct* cbs);
extern void XcodaEditorRedraw               (Widget w,
					     XtPointer clientData,
					     XmAnyCallbackStruct* cbs);
extern void XcodaEditorUndoAction           (Widget w,
					     XtPointer clientData,
					     XmAnyCallbackStruct* cbs);
extern void delete_all_arcs                 (Widget w,
					     XtPointer clientData,
					     XmAnyCallbackStruct* cbs);
extern void delete_everything               (Widget w,
					     XtPointer clientData,
					     XmAnyCallbackStruct* cb);
extern void removeArcFromWholeArcList       (XcodaEditorGraph* graph,
					     Arc *arc);
extern void setup_drawcomp_func             (drawComp* comp);
extern void setup_ipport_func               (ipPort* port);
extern void insertNodeToCompList            (XcodaEditorGraph* graph,
					     drawComp* comp);
extern void removeNodeFromCompList          (XcodaEditorGraph* graph,
					     drawComp* comp);
extern void insertArcToWholeArcList         (XcodaEditorGraph* graph,
					     Arc *arc);
extern void free_daqcomp_res                (daqComp* comp);
extern void free_ipPort_res                 (ipPort* port, int num_ports);
extern Arc  **XcodaEditorGetFromArcs        (XcodaEditorGraph* graph,
					     ipPort* port,
					     int* num);
extern Arc  **XcodaEditorGetToArcs          (XcodaEditorGraph* graph,
					     ipPort* port,
					     int* num);
extern Arc  *XcodaEditorGetSelectedArc      (XcodaEditorGraph* graph);
extern Arc  *XcodaEditorGetArcBetweenPorts  (XcodaEditorGraph* graph,
					     ipPort* port0,
					     ipPort* port1);
extern Arc  *XcodaEditorGetArcBetweenNodes  (XcodaEditorGraph* graph,
					     drawComp* node0,
					     drawComp* node1);
extern char **XcodaEditorCompGetAllInputs   (XcodaEditorGraph* graph,
					     drawComp* comp,
					     int *num_inputs);
extern int  compHasOutputs                  (drawComp* comp);
extern char **XcodaEditorCompGetAllOutputs  (XcodaEditorGraph* graph,
					     drawComp* comp,
					     int *num_outputs);
extern int  isOneCoverAnother               (XcodaEditorGraph* graph,
					     drawComp* comp,
					     int col,
					     int row);
extern void update_arc_geometry             (Arc* arc,
					     ipPort* from,
					     ipPort* to);
extern int  isEmptyGraph                    (XcodaEditorGraph* graph);
extern drawComp *XcodaEditorGetSelectedNode (XcodaEditorGraph* graph);
extern drawComp *findDrawCompFromPort       (XcodaEditorGraph* graph,
					     ipPort* port);
#else
extern void reset_scrollbar_res             ();
extern void XcodaEditorTrackPointerPosition ();
extern void XcodaEditorUpdateEditableFlag   ();
extern Widget XcodaEditorDrawingArea        ();
extern void XcodaEditorResizeComp           ();
extern void XcodaEditorConn2Cols            ();
extern void XcodaEditorBgPixmap             ();
extern void XcodaEditorAddCompNodeMode      ();
extern void XcodaEditorSetDeleteArcMode     ();
extern void XcodaEditorSetDeleteCompMode    ();
extern void XcodaEditorRedraw               ();
extern void XcodaEditorUndoAction           ();
extern void delete_all_arcs                 ();
extern void delete_everything               ();
extern void removeArcFromWholeArcList       ();
extern void setup_drawcomp_func             ();
extern void setup_ipport_func               ();
extern void insertNodeToCompList            ();
extern void removeNodeFromCompList          ();
extern void insertArcToWholeArcList         ();
extern void free_daqcomp_res                ();
extern void free_ipPort_res                 ();
extern Arc  **XcodaEditorGetFromArcs        ();
extern Arc  **XcodaEditorGetToArcs          ();
extern Arc  *XcodaEditorGetSelectedArc      ();
extern Arc  *XcodaEditorGetArcBetweenPorts  ();
extern char **XcodaEditorCompGetAllInputs   ();
extern int  compHasOutputs                  ();
extern char **XcodaEditorCompGetAllOutputs  ();
extern int  isOneCoverAnother               ();
extern void update_arc_geometry             ();
extern int  isEmptyGraph                    ();
extern drawComp *XcodaEditorGetSelectedNode ();
extern drawComp *findDrawCompFromPort       ();
#endif


#endif
  
