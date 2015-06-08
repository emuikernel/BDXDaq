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
 *	CODA Editor icon box
 *	
 * Author:  Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_icon_box.c,v $
 *   Revision 1.5  1998/07/13 17:53:55  heyes
 *   remove buttons when env var set...
 *
 *   Revision 1.4  1998/06/18 12:28:31  heyes
 *   tidy up a lot of thingscd ../cedit
 *
 *   Revision 1.3  1997/09/08 15:19:34  heyes
 *   fix dd icon etc
 *
 *   Revision 1.2  1997/06/20 17:00:14  heyes
 *   clean up GUI!
 *
 *   Revision 1.1.1.2  1996/11/05 17:45:12  chen
 *   coda source
 *
 *	  
 */
#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/ScrolledW.h>

#include "MixedLabel.h"

#include "Editor_pixmap.h"
#include "Editor_layout.h"
#include "Editor_graph.h"
#include "Editor_misc.h"
#include "Editor_graph_cmd.h"

/* return row column widget */

typedef struct _icon_comp{
  daqComp daq;
  char    **port_name;
  int     num_ports;
  Widget  w;
  struct _icon_comp *next;
  struct _icon_comp *previous;
}iconCompList;

static iconCompList *icon_head, *icon_tail;

#if defined (__STDC__)
static drawComp* newDrawCompFromIcon (iconCompList* icon)
#else
static drawComp* newDrawCompFromIcon (icon)
     iconCompList* icon;
#endif
{
  drawComp* comp;
  ipPort* port;
  int     i = 0;

  comp = (drawComp *)malloc(sizeof(drawComp));
  if(comp == NULL){
    fprintf(stderr,"cannot allocate memory for existing component!\n");
    exit(1);
  }
  comp->comp.type = icon->daq.type;
  comp->comp.comp_name = strsave(icon->daq.comp_name);
  comp->comp.node_name = strsave(icon->daq.node_name);
  comp->comp.id_num = icon->daq.id_num;
  if (icon->daq.boot_string != 0) 
    comp->comp.boot_string = strsave(icon->daq.boot_string);
  else
    comp->comp.boot_string = 0;    
  if(icon->daq.code[0] != NULL)
    comp->comp.code[0] = strsave(icon->daq.code[0]);
  else
    comp->comp.code[0] = (char *)NULL;
  if(icon->daq.code[1] != NULL)
    comp->comp.code[1] = strsave(icon->daq.code[1]);
  else
    comp->comp.code[1] = (char *)NULL;
  if(icon->daq.code[2] != NULL)
    comp->comp.code[2] = strsave(icon->daq.code[2]);
  else
    comp->comp.code[2] = (char *)NULL;

  comp->selected = 0;
  comp->scripts = 0;
  comp->editable = 0;
  comp->width = sw_geometry.cell_wd*sw_geometry.zoomscale;
  comp->height = sw_geometry.cell_ht*sw_geometry.zoomscale;
  comp->num_row = icon->num_ports;
  comp->num_col = 1;
  comp->x = comp->y = 0;
  setup_drawcomp_func(comp);

  comp->num_ports = icon->num_ports;
  for(i = 0;i < MAX_NUM_PORTS;i++){
    port = &(comp->ip_port[i]);
    port->index = i;
    if(i < comp->num_ports)
      port->ip_addr = strsave(icon->port_name[i]);
    else
      port->ip_addr = (char *)NULL;
    port->left_selected = 0;
    port->right_selected = 0;
    port->comp_ptr = &(comp->comp);
    port->width = comp->width/8.0;
    port->height = comp->height/4.0;

    port->left_x = port->right_x = 0;
    port->left_y = port->right_y = 0;
    port->row = port->col = 0;
    setup_ipport_func(port);
  }
  return comp;
}


/************************************************************************
 *     static iconCompList *createIconInfoFromRcnetComp(comp)           *
 * Description:                                                         *
 *     create icon information from rcnetwork component 'comp'          *
 *     except X widget                                                  *
 ***********************************************************************/
#if defined (__STDC__)
static iconCompList *createIconInfoFromRcnetComp(rcNetComp* comp)
#else
static iconCompList *createIconInfoFromRcnetComp(comp)
     rcNetComp *comp;
#endif
{
  daqComp      *daq;
  iconCompList *p;
  int          i;

  p = (iconCompList *)malloc(sizeof(iconCompList));
  if(p == NULL){
    fprintf(stderr,"cannot allocate memory for icon!\n");
    exit(1);
  }
  daq = &(comp->daq);
  p->daq.comp_name = strsave(daq->comp_name);
  p->daq.node_name = strsave(daq->node_name);
  if (daq->boot_string != 0)
    p->daq.boot_string = strsave(daq->boot_string);
  else
    p->daq.boot_string = 0;    

  if(daq->code[0] != NULL)
    p->daq.code[0] = strsave(daq->code[0]);
  else
    p->daq.code[0] = (char *)NULL;

  if(daq->code[1] != NULL)
    p->daq.code[1] = strsave(daq->code[1]);
  else
    p->daq.code[1] = (char *)NULL;

  if(daq->code[2] != NULL)
    p->daq.code[2] = strsave(daq->code[2]);
  else
    p->daq.code[2] = (char *)NULL;

  p->daq.id_num = daq->id_num;
  p->daq.type = daq->type;
  p->num_ports = comp->num_ports;
  if(p->num_ports != 0){
    p->port_name = (char **)malloc((p->num_ports)*sizeof(char *));
    if(p->port_name == NULL){
      fprintf(stderr,"cannot allocate space for port_names!\n");
      exit(1);
    }
  }
  for(i=0;i<p->num_ports;i++)
    p->port_name[i] = strsave(comp->port_name[i]);
  
  return p;
}
  
  
/************************************************************************
 *    static iconCompList *createIconInfoFromDrawComp(comp)             *
 * Description:                                                         *
 *     Create icon information from draw component 'comp'               *
 *     except X widget                                                  *
 ***********************************************************************/
#if defined (__STDC__)
static iconCompList *createIconInfoFromDrawComp(drawComp* comp)
#else
static iconCompList *createIconInfoFromDrawComp(comp)
     drawComp *comp;
#endif
{
  daqComp  *daq;
  ipPort   *port;
  iconCompList *p;
  int       i;

  p = (iconCompList *)malloc(sizeof(iconCompList));
  if(p == NULL){
    fprintf(stderr,"Cannot allocate memory for icon information!\n");
    exit(1);
  }
  daq = &(comp->comp);
  p->daq.comp_name = strsave(daq->comp_name);
  p->daq.node_name = strsave(daq->node_name);
  if (daq->boot_string != 0)
    p->daq.boot_string = strsave(daq->boot_string);
  else
    p->daq.boot_string = 0;

  if(daq->code[0] != NULL)
    p->daq.code[0] = strsave(daq->code[0]);
  else
    p->daq.code[0] = (char *)NULL;

  if(daq->code[1] != NULL)
    p->daq.code[1] = strsave(daq->code[1]);
  else
    p->daq.code[1] = (char *)NULL;

  if(daq->code[2] != NULL)
    p->daq.code[2] = strsave(daq->code[2]);
  else
    p->daq.code[2] = (char *)NULL;

  p->daq.id_num = daq->id_num;
  p->daq.type = daq->type;
  p->num_ports = comp->num_ports;
  if(p->num_ports != 0){
    p->port_name = (char **)malloc((p->num_ports)*sizeof(char *));
    for(i=0;i<p->num_ports;i++){
      port = &(comp->ip_port[i]);
      p->port_name[i] = strsave(port->ip_addr);
    }
  }
  return p;
}

/***************************************************************************
 *          static void updateIconInfoFromDrawComp(p, daq)                 *
 * Description:                                                            *
 *     update icon info from daq component                                 *
 **************************************************************************/
#if defined (__STDC__)
static void updateIconInfoFromDrawComp(iconCompList* p, 
				       drawComp* comp)
#else
static void updateIconInfoFromDrawComp(p, comp)
     iconCompList *p;
     drawComp     *comp;
#endif
{
  daqComp *daq;
  ipPort  *port;
  Arg     args[10];
  int     ac = 0, i;

  daq = &(comp->comp);
  if(p->daq.comp_name != NULL)
    free(p->daq.comp_name);
  p->daq.comp_name = strsave(daq->comp_name);
  if(p->daq.node_name != NULL)
    free(p->daq.node_name);
  p->daq.node_name = strsave(daq->node_name);
  if(p->daq.boot_string != NULL) {
    free(p->daq.boot_string);
    p->daq.boot_string = 0;
  }
  if (daq->boot_string != 0)
    p->daq.boot_string = strsave(daq->boot_string);

  if(p->daq.code[0] != NULL)
    free(p->daq.code[0]);
  if (daq->code[0])
    p->daq.code[0] = strsave(daq->code[0]);
  else
    p->daq.code[0] = 0;

  if(p->daq.code[1] != NULL)
    free(p->daq.code[1]);
  if (daq->code[1])
    p->daq.code[1] = strsave(daq->code[1]);
  else
    p->daq.code[1] = 0;


  if(p->daq.code[2] != NULL)
    free(p->daq.code[2]);
  if (daq->code[2])
    p->daq.code[2] = strsave(daq->code[2]);
  else
    p->daq.code[2] = 0;


  p->daq.id_num = daq->id_num;
  p->daq.type = daq->type;

  if(p->num_ports != 0){
    for(i=0;i<p->num_ports;i++)
      free(p->port_name[i]);
    free(p->port_name); 
  }
  p->num_ports = comp->num_ports;
  if(p->num_ports != 0){
    p->port_name = (char **)malloc((p->num_ports)*sizeof(char *));
    for(i=0;i<p->num_ports;i++){
      port = &(comp->ip_port[i]);
      p->port_name[i] = strsave (port->ip_addr);
    }
  }
  XtSetArg(args[ac], XcodaNtopLabel, p->daq.node_name); ac++;
  XtSetArg(args[ac], XcodaNbottomLabel, p->daq.comp_name); ac++;
  XtSetValues(p->w, args, ac);
  ac = 0;
}

/******************************************************************
 *    static void insertExistingCompToGraph( )                    *
 * Description:                                                   *
 *      Try to insert an existing component into the graph        *
 *****************************************************************/
#if defined (__STDC__)
static void insertExistingCompToGraph(Widget w, 
				      iconCompList* icon, 
				      XmAnyCallbackStruct* cbs)
#else
static void insertExistingCompToGraph(w, icon, cbs)
     Widget w;
     iconCompList *icon;
     XmAnyCallbackStruct *cbs;
#endif
{
  compList *p;
  drawComp *q;
  ipPort   *port;
  int      i;
  
  /* reset toggle button part of graph commands */
  XcodaEditorResetGraphCmd();

  if(coda_graph.comp_list_head->next != coda_graph.comp_list_tail){
    for(p = coda_graph.comp_list_head->next; p != coda_graph.comp_list_tail; p=p->next){
      if(strcmp(icon->daq.comp_name, p->draw_comp->comp.comp_name) == 0)
	break;
    }
    if(p != coda_graph.comp_list_tail){ /* match a component, cannot add */
      XBell(xgc.dpy, 0);
      return;
    }
  }
  q = newDrawCompFromIcon (icon);
  coda_graph.current_action = ADD_EXISTING_COMP;
  coda_graph.current_comp = q;
}  


static Widget sc;

#if defined (__STDC__)
Widget XcodaEditorIconBox(Widget parent)
#else
Widget XcodaEditorIconBox(parent)
     Widget parent;
#endif
{
  Widget rcl;
  Arg    args[20];
  int    ac;
  Widget hsc, vsc;

  ac = 0;
  XtSetArg(args[ac], XmNscrollingPolicy, XmAUTOMATIC); ac++;
  XtSetArg(args[ac], XmNscrollBarDisplayPolicy, XmSTATIC); ac++;
  /*XtSetArg(args[ac], XmNwidth, 75); ac++;*/
  XtSetArg(args[ac], XmNheight,105); ac++;
  sc = XtCreateManagedWidget("icon_sw", xmScrolledWindowWidgetClass,
			     parent, args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNorientation, XmVERTICAL); ac++;
  /* XtSetArg(args[ac], XmNpacking, XmPACK_COLUMN); ac++;*/
  XtSetArg(args[ac], XmNnumColumns, 1); ac++;

  /* XtSetArg(args[ac], XmNwidth, 70); ac++;
  XtSetArg(args[ac], XmNheight,100); ac++;*/

  XtSetArg(args[ac], XmNtraversalOn, FALSE); ac++;
  XtSetArg(args[ac], XmNspacing, 5); ac++;
  rcl = XtCreateManagedWidget("icon_box",xmRowColumnWidgetClass,
			      sc, args, ac);
  ac = 0;
  
  XtSetArg(args[ac], XmNverticalScrollBar, &vsc); ac++;
  XtGetValues(sc, args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNhorizontalScrollBar, &hsc); ac++;
  XtGetValues(sc, args, ac); 
  ac = 0;
  XtSetArg(args[ac], XmNtraversalOn, False); ac++;
  XtSetValues(hsc, args, ac);
  ac = 0;

  XtUnmanageChild(hsc);
  manager.icon_holder = rcl;

/* setup icon component list */
  icon_head = (iconCompList *)malloc(1*sizeof(iconCompList));
  if(icon_head == NULL){
    fprintf(stderr,"Cannot allocate memory for icon list \n");
    exit(1);
  }
  icon_tail = (iconCompList *)malloc(1*sizeof(iconCompList));
  if(icon_tail == NULL){
    fprintf(stderr,"Cannot allocate memory for icon list \n");
    exit(1);
  }
  icon_head->next = icon_tail;
  icon_head->previous = (iconCompList *)NULL;
  icon_tail->next = (iconCompList *)NULL;
  icon_tail->previous = icon_head;
  
  return rcl;
}


/******************************************************************
 *    void XcodaEditorCreateIconsFromRcnet(list, num_comps)       *
 * Description:                                                   *
 *      Construct all icons corresponding rcnetwork components    *
 *****************************************************************/
#if defined (__STDC__)
void XcodaEditorCreateIconsFromRcnet(rcNetComp** list, 
				     int num_comps)
#else
void XcodaEditorCreateIconsFromRcnet(list, num_comps)
     rcNetComp **list;
     int       num_comps;
#endif
{
  iconCompList *p, *q, *ne, *pr;
  Arg          args[10];
  int          ac = 0, i, j;
  daqComp      *daq;
  XmString t;
  for(i=0;i<num_comps;i++){
    if (list[i]->daq.type != CODA_LOG){
      q = createIconInfoFromRcnetComp(list[i]);
      XtSetArg(args[ac], XcodaNtopLabel, q->daq.node_name); ac++;
      XtSetArg(args[ac], XcodaNbottomLabel, q->daq.comp_name); ac++;
      if(q->daq.type == CODA_ROC){
	XtSetArg(args[ac], XcodaNnormalPixmap, btn_pixmaps.roc); ac++;
	XtSetArg(args[ac], XcodaNhighLightPixmap, btn_pixmaps.hl_roc); ac++;
      }
      else if(q->daq.type == CODA_EB){
	XtSetArg(args[ac], XcodaNnormalPixmap, btn_pixmaps.eb); ac++;
	XtSetArg(args[ac], XcodaNhighLightPixmap, btn_pixmaps.hl_eb); ac++;
      }
      else if(q->daq.type == CODA_TRIG){
	XtSetArg(args[ac], XcodaNnormalPixmap, btn_pixmaps.trig); ac++;
	XtSetArg(args[ac], XcodaNhighLightPixmap, btn_pixmaps.trig); ac++;
      }
      else{
	XtSetArg(args[ac], XcodaNnormalPixmap, btn_pixmaps.roc); ac++;
	XtSetArg(args[ac], XcodaNhighLightPixmap, btn_pixmaps.hl_roc); ac++;
      }
      t = XmStringCreateSimple(q->daq.comp_name);
      XtSetArg (args[ac], XmNlabelString, t); ac++;

      q->w = XtCreateManagedWidget("icon", xmPushButtonWidgetClass,
				   manager.icon_holder, args, ac);
      ac = 0;
      XmStringFree (t);

      XtAddCallback(q->w, XmNactivateCallback, 
		    insertExistingCompToGraph, q);

      ne = icon_head->next;
      q->next = ne;
      ne->previous = q;
      icon_head->next = q;
      q->previous = icon_head;
    }
  }
}

/******************************************************************
 *    void XcodaEditorInsertCompToIcons(graph)                    *
 * Description:                                                   *
 *      Insert current Component to Icon box                      *
 *      upon saving a whole graph                                 *
 ******************************************************************/
#if defined (__STDC__)
void XcodaEditorInsertCompToIcons(XcodaEditorGraph* graph)
#else
void XcodaEditorInsertCompToIcons(graph)
     XcodaEditorGraph *graph;
#endif
{
  iconCompList *p, *q, *ne, *pr;
  drawComp     *comp;
  ipPort       *port;
  compList     *pp;
  Arg          args[10];
  int          ac = 0;
  XmString t;

  if (isEmptyGraph (graph))
    return;

  for(pp = graph->comp_list_head->next; pp != graph->comp_list_tail; 
      pp = pp->next){
    comp = pp->draw_comp;
    
    if (comp->comp.type < 11) {

      if(icon_head->next != icon_tail){
	for(p=icon_head->next;p != icon_tail; p=p->next){
	  if(strcmp(comp->comp.comp_name, p->daq.comp_name) == 0) /* found existing one */
	    break;
	}
	if(p != icon_tail) /*found existing one */
	  updateIconInfoFromDrawComp(p, comp);
	else{
	  q = createIconInfoFromDrawComp(comp);

	  XtSetArg(args[ac], XcodaNtopLabel, q->daq.node_name); ac++;
	  XtSetArg(args[ac], XcodaNbottomLabel, q->daq.comp_name); ac++;
	  if(q->daq.type == CODA_ROC){
	    XtSetArg(args[ac], XcodaNnormalPixmap, btn_pixmaps.roc); ac++;
	    XtSetArg(args[ac], XcodaNhighLightPixmap, btn_pixmaps.hl_roc); ac++;
	  }
	  else if(q->daq.type == CODA_EB){
	    XtSetArg(args[ac], XcodaNnormalPixmap, btn_pixmaps.eb); ac++;
	    XtSetArg(args[ac], XcodaNhighLightPixmap, btn_pixmaps.hl_eb); ac++;
	  }
	  else if(q->daq.type == CODA_TRIG){
	    XtSetArg(args[ac], XcodaNnormalPixmap, btn_pixmaps.trig); ac++;
	    XtSetArg(args[ac], XcodaNhighLightPixmap, btn_pixmaps.trig); ac++;
	  }
	  else{
	    XtSetArg(args[ac], XcodaNnormalPixmap, btn_pixmaps.roc); ac++;
	    XtSetArg(args[ac], XcodaNhighLightPixmap, btn_pixmaps.hl_roc); ac++;
	  }

	  t = XmStringCreateSimple(q->daq.comp_name);
	  XtSetArg (args[ac], XmNlabelString, t); ac++;

	  q->w = XtCreateManagedWidget("icon", xmPushButtonWidgetClass,
				       manager.icon_holder, args, ac);
	  ac = 0;
	  XtAddCallback(q->w, XmNactivateCallback, insertExistingCompToGraph, q);
	  
	  XmStringFree (t);
	  ne = icon_head->next;
	  q->next = ne;
	  ne->previous = q;
	  icon_head->next = q;
	  q->previous = icon_head;
	}
      }
      else{
	q = createIconInfoFromDrawComp(comp);
	XtSetArg(args[ac], XcodaNtopLabel, q->daq.node_name); ac++;
	XtSetArg(args[ac], XcodaNbottomLabel, q->daq.comp_name); ac++;
	if(q->daq.type == CODA_ROC){
	  XtSetArg(args[ac], XcodaNnormalPixmap, btn_pixmaps.roc); ac++;
	  XtSetArg(args[ac], XcodaNhighLightPixmap, btn_pixmaps.hl_roc); ac++;
	}
	else if(q->daq.type == CODA_EB){
	  XtSetArg(args[ac], XcodaNnormalPixmap, btn_pixmaps.eb); ac++;
	  XtSetArg(args[ac], XcodaNhighLightPixmap, btn_pixmaps.hl_eb); ac++;
	}
	else if(q->daq.type == CODA_TRIG){
	  XtSetArg(args[ac], XcodaNnormalPixmap, btn_pixmaps.trig); ac++;
	  XtSetArg(args[ac], XcodaNhighLightPixmap, btn_pixmaps.trig); ac++;
	}
	else{
	  XtSetArg(args[ac], XcodaNnormalPixmap, btn_pixmaps.roc); ac++;
	  XtSetArg(args[ac], XcodaNhighLightPixmap, btn_pixmaps.hl_roc); ac++;
	}

	t = XmStringCreateSimple(q->daq.comp_name);
	XtSetArg (args[ac], XmNlabelString, t); ac++;

	q->w = XtCreateManagedWidget("icon", xmPushButtonWidgetClass,
				     manager.icon_holder, args, ac);
	XmStringFree (t);

	ac = 0;
	XtAddCallback(q->w, XmNactivateCallback, insertExistingCompToGraph, q);

	ne = icon_head->next;
	q->next = ne;
	ne->previous = q;
	icon_head->next = q;
	q->previous = icon_head;
      }
    }
  }
}


/**************************************************************
 *   void delete_everyicons( )                                *
 * Description:                                               *
 *      remove every icons from icon box                      *
 *************************************************************/
#if defined (__STDC__)
void delete_everyicons(void)
#else
void delete_everyicons()
#endif
{
  iconCompList *p, *q;
  int          i;

  if(icon_head->next != icon_tail){
    p = icon_head->next;
    while(p != icon_tail){
      q = p->next;
      XtDestroyWidget(p->w);
      for(i=0;i<p->num_ports;i++)
	free(p->port_name[i]);
      if (p->num_ports)
	free(p->port_name);
      free(p->daq.comp_name);
      free(p->daq.node_name);
      if (p->daq.boot_string)
	free(p->daq.boot_string);
      if (p->daq.code[0])
	free (p->daq.code[0]);
      if (p->daq.code[1])
	free (p->daq.code[1]);
      if (p->daq.code[2])
	free (p->daq.code[2]);
      free(p);
      p = q;
    }
    icon_head->next = icon_tail;
    icon_head->previous = (iconCompList *)NULL;
    icon_tail->next = (iconCompList *)NULL;
    icon_tail->previous = icon_head;
  }
}

/********************External checking utilities***************/

/***************************************************************
 *        int isEmptyIconList()                                *
 * Description:                                                *
 *     Check whether icon list is empty or not                 *
 *     return 1: Yes empty, 0: not empty                       *
 **************************************************************/
#if defined (__STDC__)
int isEmptyIconList(void)
#else
int isEmptyIconList()
#endif
{
  if(icon_head->next != icon_tail)
    return 0;
  else 
    return 1;
}
  
/***************************************************************
 *        int nameOkWithIconList(char *name)                   *
 * Description:                                                *
 *    Check whether there is a name clash between name and     *
 *    a name inside icon list                                  *
 *    return 1: yes, 0: no                                     *
 **************************************************************/
#if defined (__STDC__)
int nameOkWithIconList (char* name)
#else
int nameOkWithIconList (name)
     char *name;
#endif
{
  iconCompList *p;

  if (isEmptyIconList())
    return 1;
  else{
    for (p = icon_head->next; p != icon_tail; p = p->next)
      if (strcasecmp (name, p->daq.comp_name) == 0)
	return 0;
  }
  return 1;
}

/***************************************************************
 *        int idOkWithIconList(name, id, type)                 *
 * Description:                                                *
 *    Check whether there is a id clash between id and         *
 *    a id inside icon list                                    *
 *    return 1: yes, 0: no                                     *
 **************************************************************/
#if defined (__STDC__)
int idOkWithIconList (char* name, int id, int type)
#else
int idOkWithIconList (name, id, type)
     char *name;
     int  id, type;
#endif
{
  iconCompList *p;

  

  if (isEmptyIconList())
    return 1;
  else{
    if (type == CODA_EBANA){ /* name should be NULL */
      for (p = icon_head->next; p != icon_tail; p = p->next){
	  if ((p->daq.type == CODA_EB) &&
	      p->daq.id_num == id)
	    return 0;
	}
    }
    else{
      for (p = icon_head->next; p != icon_tail; p = p->next)
	if (strcmp(name, p->daq.comp_name) != 0){
	  if (p->daq.type == type && p->daq.id_num == id)
	    return 0;
	}
    }
  }
  return 1;  
}

/***************************************************************
 *        void updateInfoToIconList(drawComp *comp)            *
 * Description:                                                *
 *    Update all new information (except comp_name) to icon    *
 *    List                                                     *
 **************************************************************/
#if defined (__STDC__)
void updateInfoToIconList(drawComp* comp)
#else
void updateInfoToIconList(comp)
     drawComp *comp;
#endif
{
  iconCompList *p;
  Arg args[5];
  int ac = 0;

  if (isEmptyIconList())
    return;
  else{
    for (p = icon_head->next; p != icon_tail; p = p->next)
      if (strcmp(comp->comp.comp_name, p->daq.comp_name) == 0)
	break;
    if (p != icon_tail){
      free (p->daq.node_name);
      if (p->daq.boot_string != 0) {
	free (p->daq.boot_string);
	p->daq.boot_string = 0;
      }

      if (p->daq.code[0])
	free (p->daq.code[0]);
      if (p->daq.code[1])
	free (p->daq.code[1]);
      if (p->daq.code[2])
	free (p->daq.code[2]);
	
      p->daq.node_name = strsave (comp->comp.node_name);
      p->daq.id_num = comp->comp.id_num;
      if (comp->comp.boot_string != 0)
	p->daq.boot_string = strsave (comp->comp.boot_string);
      else
	p->daq.boot_string = 0;

      if (comp->comp.code[0])
	p->daq.code[0] = strsave (comp->comp.code[0]);
      else
	p->daq.code[0] = (char *)0;

      if (comp->comp.code[1])
	p->daq.code[1] = strsave (comp->comp.code[1]);
      else
	p->daq.code[1] = (char *)0;

      if (comp->comp.code[2])
	p->daq.code[2] = strsave (comp->comp.code[2]);
      else
	p->daq.code[2] = (char *)0;

      /* finally shows up on the widget */
      XtSetArg(args[ac], XcodaNtopLabel, p->daq.node_name); ac++;
      XtSetArg(args[ac], XcodaNbottomLabel, p->daq.comp_name); ac++;      
      XtSetValues (p->w, args, ac);
      ac = 0;
    }
  }
}

/***********************************************************************
 *            void compressIconList(void)                              *
 * Description:                                                        *
 *     Clean out unused icons from the list                            *
 **********************************************************************/
#if defined (__STDC__)
void compressIconList(void)
#else
void compressIconList()
#endif
{
  iconCompList *p, *prev, *next;
  Arg args[10];
  int ac = 0, i;

  if (isEmptyIconList())
    return;  

  for (p = icon_head->next; p != icon_tail; p = p->next){
    if (compNotUsed(p->daq.comp_name)){
      prev = p->previous;
      next = p->next;
      prev->next = next;
      next->previous = prev;
      /* free all old memory */
      XtDestroyWidget(p->w);
      for(i=0;i<p->num_ports;i++)
	free(p->port_name[i]);
      free(p->port_name);
      free(p->daq.comp_name);
      free(p->daq.node_name);
      if (p->daq.boot_string)
	free(p->daq.boot_string);

      if (p->daq.code[0])
	free (p->daq.code[0]);

      if (p->daq.code[1])
	free (p->daq.code[1]);

      if (p->daq.code[2])
	free (p->daq.code[2]);

      free(p);
    }
  }
}
