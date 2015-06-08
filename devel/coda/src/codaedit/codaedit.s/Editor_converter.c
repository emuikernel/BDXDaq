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
 *	CODA Editor file to graph, graph to file converter routines
 *	
 * Author:  Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_converter.c,v $
 *   Revision 1.13  1998/07/22 19:31:02  rwm
 *   Increased string length to 64, clean up.
 *
 *   Revision 1.12  1998/06/18 12:28:25  heyes
 *   tidy up a lot of thingscd ../cedit
 *
 *   Revision 1.11  1997/12/03 16:56:07  heyes
 *   increase array bounds to pervent core dumps in graph etc
 *
 *   Revision 1.10  1997/09/08 15:19:12  heyes
 *   fix dd icon etc
 *
 *   Revision 1.9  1997/08/01 18:37:38  heyes
 *   fixed moving bug
 *
 *   Revision 1.8  1997/07/30 15:16:51  heyes
 *   add more xpm support
 *
 *   Revision 1.7  1997/07/22 12:16:27  heyes
 *   need to debug on solaris
 *
 *   Revision 1.6  1997/07/09 13:03:31  heyes
 *   back to normal
 *
 *   Revision 1.5  1997/07/08 15:30:29  heyes
 *   add stuff
 *
 *   Revision 1.4  1997/06/20 16:59:49  heyes
 *   clean up GUI!
 *
 *   Revision 1.3  1996/11/27 15:05:06  chen
 *   change configuration and remove RCS from icon list
 *
 *   Revision 1.2  1996/11/13 14:55:41  chen
 *   allow different readout list in different configuration
 *
 *   Revision 1.1.1.2  1996/11/05 17:45:05  chen
 *   coda source
 *
 *	  
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

#include "Editor.h"
#include "Editor_database.h"
#include "Editor_graph.h"
#include "Editor_misc.h"

/* global file and debugger number */
int outputFileNum = 0;
int codaFileNum = 0;
int debuggerNum = 0;
int ddNum = 0;

/*********************************************************************************
 *      static ipPort *findInputFromPort(graph, input, error)                    *
 * Description:                                                                  *
 *     Find from port coresponding to this input id                              *
 *     This routine must be called after draw components has been constructed    *
 *     error code 0 means success                                                *
 ********************************************************************************/
#if defined (__STDC__)
static ipPort *findInputFromPort(XcodaEditorGraph* graph, 
				 IoId* input, 
				 int*  error)
#else
static ipPort *findInputFromPort(graph, input, error)
     XcodaEditorGraph *graph;
     IoId             *input;
     int              *error;
#endif
{
  compList *p;
  drawComp *tdc;
  int      i;
  ipPort   *from_port;
  
  for(p = graph->comp_list_head->next; p != graph->comp_list_tail; p = p->next){
    tdc = p->draw_comp;
    if(strcmp(tdc->comp.comp_name, input->comp_name) == 0)
      break;
  }      
  if(p == graph->comp_list_tail){
    *error = 1;
    return (ipPort *)NULL;
  }
  for(i=0;i<tdc->num_ports;i++){
    if(strcmp(input->port_name, tdc->ip_port[i].ip_addr)==0)
      break;
  }
  if(i >= tdc->num_ports){
    *error = 1;
    return (ipPort *)NULL;
  }
  from_port = &(tdc->ip_port[i]);
  *error = 0;
  return from_port;
}

/*********************************************************************************
 *   static ipPort *findInputToPort(graph, from_port, to_comp cinfo, num_cinfos) *
 * Description:                                                                  *
 *    Find out To port corresponding to this from port and to 'comp'             *
 *    This tourine must be called after findInputFromPort()                      *
 *    which constructs from_port                                                 *
 ********************************************************************************/
#if defined (__STDC__)
static ipPort *findInputToPort(XcodaEditorGraph* graph, 
			       IoId*             output,
			       int *error)
#else
static ipPort *findInputToPort(graph, output, error)
{
     XcodaEditorGraph *graph;
     IoId*            output,
     int              *error;
#endif
{
  compList *p;
  drawComp *tdc;
  int      i;
  ipPort   *to_port;
  
  for(p = graph->comp_list_head->next; p != graph->comp_list_tail; p = p->next){
    tdc = p->draw_comp;
    if(strcmp(tdc->comp.comp_name, output->comp_name) == 0)
      break;
  }      
  if(p == graph->comp_list_tail){
    *error = 1;
    return (ipPort *)NULL;
  }
  for(i=0;i<tdc->num_ports;i++){
    if(strcmp(output->port_name, tdc->ip_port[i].ip_addr)==0)
      break;
  }
  if(i >= tdc->num_ports){
    *error = 1;
    return (ipPort *)NULL;
  }
  to_port = &(tdc->ip_port[i]);
  *error = 0;
  return to_port;
}

/*********************************************************************************
 *      static  void *createArcsFromCinfos(graph, cinfo, num_cinfos)             *
 * Description:                                                                  *
 *      Create Arc Objects from configuration components                         *
 *********************************************************************************/
#if defined (__STDC__)
static void createArcsFromCinfos(XcodaEditorGraph* graph, 
				 ConfigInfo** cinfo, 
				 int num_cinfos, 
				 int* error)
#else
static void createArcsFromCinfos(graph, cinfo, num_cinfos, error)
     XcodaEditorGraph *graph;
     ConfigInfo       **cinfo;
     int              num_cinfos, *error;
#endif
{
  Arc      *arc, *ret_arc;
  ipPort   *to_port,*from_port;
  drawComp *dc, *tdc;
  compList *p;
  int      i, j, k, m;
  IoId     *io;

  
  if(graph->comp_list_head->next == graph->comp_list_tail)
    return;
  for(i = 0;i < num_cinfos; i++){
    for(p = graph->comp_list_head->next; p != graph->comp_list_tail; p = p->next){
      dc = p->draw_comp;
      if(strcmp(dc->comp.comp_name, cinfo[i]->comp_name) == 0) 
	break;
    }
    if( p == graph->comp_list_tail){
      *error = 1;
      return;
    }
    for(j = 0;j < cinfo[i]->num_inputs;j++){ /* for each inputs */
      from_port = findInputFromPort (graph, cinfo[i]->inputs[j], error);
      if(*error != 0)
	return;

      /* find right config info corresponding to the from port */
      for(k = 0; k < num_cinfos; k++){
	if(strcmp(from_port->comp_ptr->comp_name,cinfo[k]->comp_name) == 0)
	  break;
      }
      if(k >= num_cinfos){
	*error = 1;
	return;
      }

      for (m = 0; m < cinfo[k]->num_outputs; m++) {
	to_port   = findInputToPort(graph, cinfo[k]->outputs[m], error);
	if(*error != 0)
	  return;
	if((ret_arc = XcodaEditorGetArcBetweenPorts(graph, to_port, from_port)) == NULL){
	  arc = (Arc *)malloc(sizeof(Arc));
	  if(arc == NULL){
	    fprintf(stderr, "Cannot allocate memory for new Arc.\n");
	    exit(1);
	  }
	  arc->type = IP_CONNECTION;
	  arc->selected = 0;
	  arc->from_row = from_port->row;
	  arc->from_col = from_port->col;
	  arc->to_row = to_port->row;
	  arc->to_col = to_port->col;
	  arc->from = from_port;
	  arc->to = to_port;
	  setup_arc_func(arc);
	  update_arc_geometry(arc, from_port, to_port);
	  insertArcToWholeArcList(graph, arc);
	}
      }
    }
  }
}

/*********************************************************************************
 *      static void findDefaultDrawCompPosition(graph, dc, row, col)             *
 * Description:                                                                  *
 *    Find out default draw component position on the panel                      *
 *    Simple Algorithm: TRIG Will be in the first column                         *
 *                      ROC will be in 3rd column                                *
 *                      EB  will be in 5th column                                *
 *                      ANA will be in 7th column                                *
 *    Each class objects will be separated by 2 rows                             *
 *********************************************************************************/
#if defined (__STDC__)
static void findDefaultDrawCompPosition(XcodaEditorGraph* graph, 
					drawComp* dc, 
					int* row, 
					int* col)
#else
static void findDefaultDrawCompPosition(graph, dc, row, col)
     XcodaEditorGraph *graph;
     drawComp         *dc;
     int              *row, *col;
#endif
{
  compList *p;
  drawComp *draw_comp;
  int      max_row = 1;

  if(graph->comp_list_head->next == graph->comp_list_tail){
    if(dc->comp.type == CODA_TRIG){
      *row = 3;
      *col = 1;
    }
    else if(dc->comp.type == CODA_ROC){
      *row = 3;
      *col = 3;
    }
    else if(dc->comp.type == CODA_EB){
      *row = 3;
      *col = 5;
    }
    else{
      *row = 3;
      *col = 10;
    }
  }
  else{
    for(p = graph->comp_list_head->next; p != graph->comp_list_tail; p = p->next){
      if(p->draw_comp->comp.type == dc->comp.type){
	if(p->draw_comp->row > max_row)
	  max_row = p->draw_comp->row + p->draw_comp->num_row;
      }
    }
    *row = max_row + 2;
    if(dc->comp.type == CODA_TRIG)
      *col = 1;
    else if(dc->comp.type == CODA_ROC)
      *col = 3;
    else if(dc->comp.type == CODA_EB)
      *col = 5;
    else
      *col = 10;
  }
}

/**********************************************************************************
 *      static drawComp *createDrawCompFromRcnetComp(graph, rc)                   *
 * Description:                                                                   *
 *      Create graph draw component from rcNetwork component                      *
 *********************************************************************************/
#if defined (__STDC__)
static drawComp *createDrawCompFromRcnetComp(XcodaEditorGraph* graph, 
					     rcNetComp* rc, 
					     ConfigInfo* cinfo)
#else
static drawComp *createDrawCompFromRcnetComp(graph, rc, cinfo)
     XcodaEditorGraph *graph;
     rcNetComp        *rc;
     ConfigInfo       *cinfo;     
#endif
{
  drawComp *p;
  daqComp  *daq;
  int      i;
  int      xx, yy;
  int      wd, ht;
  int      row, col, abs_col, abs_row;
  ipPort   *port;
  
  p = (drawComp *)malloc(sizeof(drawComp));
  if(p == NULL){
    fprintf(stderr,"Cannot allocate memory for drawComp object!!!\n");
    exit(1);
  }

  setup_drawcomp_func(p); /* setup functions for this draw comp */

  daq = &(rc->daq);
  daq->status = 0;
  p->comp.comp_name = strsave(daq->comp_name);
  if ( p->comp.type == CODA_NONE) {
    p->comp.node_name = strsave("none");
  } else {
    p->comp.node_name = strsave(daq->node_name);
  }
  p->comp.id_num = daq->id_num;
  if (daq->boot_string != 0)
    p->comp.boot_string = strsave(daq->boot_string);
  else
    p->comp.boot_string = (char *)0;
  p->comp.type = daq->type;
  p->selected = 0;
  /* if cinfo->scrips == 0, the following return 0 */
  p->scripts = duplicateCodaScriptList (cinfo->scripts);
  if(daq->code[0] != NULL)
    p->comp.code[0] = strsave(daq->code[0]);
  else
    p->comp.code[0] = (char *)NULL;

  if(daq->code[1] != NULL)
    p->comp.code[1] = strsave(daq->code[1]);
  else
    p->comp.code[1] = (char *)NULL;

  if(daq->code[2] != NULL)
    p->comp.code[2] = strsave(daq->code[2]);
  else
    p->comp.code[2] = (char *)NULL;
  
  p->editable = 0;
  if (cinfo->row == -1 && cinfo->col == -1)
    findDefaultDrawCompPosition(graph, p, &row, &col);
  else{
    row = cinfo->row;
    col = cinfo->col;
  }
  p->row = row;
  p->col = col;
  abs_row = row - sw_geometry.zoomyoff;
  abs_col = col - sw_geometry.zoomxoff;
  p->x = (abs_col-1)*(sw_geometry.cell_wd*sw_geometry.zoomscale);
  p->y = (abs_row-1)*(sw_geometry.cell_ht*sw_geometry.zoomscale);
  p->width = sw_geometry.cell_wd*sw_geometry.zoomscale;
  p->height = sw_geometry.cell_ht*sw_geometry.zoomscale;
  p->num_row = rc->num_ports;
  p->num_col = 1;
  
  p->num_ports = rc->num_ports;
  for(i=0;i<MAX_NUM_PORTS;i++){
    port = &(p->ip_port[i]);
    port->index = i;
    port->left_selected = 0;
    port->right_selected = 0;
    port->comp_ptr = &(p->comp);
    if(i < p->num_ports)
      port->ip_addr = strsave(rc->port_name[i]);
    else
      port->ip_addr = (char *)NULL;
    port->left_x = p->x;
    port->left_y = p->y + i*p->height + 3*p->height/8.0;
    port->right_x = p->x + p->width*7/8.0;
    port->right_y = port->left_y;
    port->row = p->row + i;
    port->col = p->col;
    port->width = p->width/8.0;
    port->height = p->height/4.0;
    setup_ipport_func(port);
  }
  return p;
}

/**********************************************************************************
 *      static drawComp *createExtraDrawCompFromCinfo(graph, cinfo, type)         *
 * Description:                                                                   *
 *      Create extra graph draw component (debug file) from config info           *
 *********************************************************************************/
#if defined (__STDC__)
static drawComp *createExtraDrawCompFromCinfo(XcodaEditorGraph* graph, 
					      ConfigInfo* cinfo,
					      int type)
#else
static drawComp *createExtraDrawCompFromCinfo(graph, cinfo, type)
     XcodaEditorGraph *graph;
     ConfigInfo       *cinfo;     
     int              type;
#endif
{
  drawComp *p;
  daqComp  *daq;
  int      i;
  int      xx, yy;
  int      wd, ht;
  int      row, col, abs_col, abs_row;
  ipPort   *port;
  char     hname[80];

  if (gethostname (hname, sizeof (hname)) != 0)
    strcpy (hname, "unknown");
  
  p = (drawComp *)malloc(sizeof(drawComp));
  if(p == NULL){
    fprintf(stderr,"Cannot allocate memory for drawComp object!!!\n");
    exit(1);
  }
  setup_drawcomp_func(p); /* setup functions for this draw comp */

  p->comp.comp_name = strsave(cinfo->comp_name);
  if ( p->comp.type == CODA_NONE) {
    p->comp.node_name = strsave("none");
  } else {
    p->comp.node_name = strsave(hname);
  }
  p->comp.id_num = 0;
  p->comp.boot_string = strsave("N/A");
  p->comp.type = type;
  p->comp.status = 0;
  p->selected = 0;

  /* if cinfo->scrips == 0, the following return 0 */
  p->scripts = duplicateCodaScriptList (cinfo->scripts);

  if (cinfo->code[0]) {
    char *config = currentConfigTable();
    if (config)
      insertValToOptionTable (config,"dataFile",cinfo->code[0]);
    p->comp.code[0] = strsave(cinfo->code[0]);
  } else
    p->comp.code[0] = (char *)NULL;

  if (cinfo->code[1])
    p->comp.code[1] = strsave(cinfo->code[1]);
  else
    p->comp.code[1] = (char *)NULL;

  if (cinfo->code[2])
    p->comp.code[2] = strsave(cinfo->code[2]);
  else
    p->comp.code[2] = (char *)NULL;
  
  p->editable = 0;
  if (cinfo->row == -1 && cinfo->col == -1)
    findDefaultDrawCompPosition(graph, p, &row, &col);
  else{
    row = cinfo->row;
    col = cinfo->col;
  }
  p->row = row;
  p->col = col;
  abs_row = row - sw_geometry.zoomyoff;
  abs_col = col - sw_geometry.zoomxoff;
  p->x = (abs_col-1)*(sw_geometry.cell_wd*sw_geometry.zoomscale);
  p->y = (abs_row-1)*(sw_geometry.cell_ht*sw_geometry.zoomscale);
  p->width = sw_geometry.cell_wd*sw_geometry.zoomscale;
  p->height = sw_geometry.cell_ht*sw_geometry.zoomscale;
  p->num_row = 1;
  p->num_col = 1;
  
  p->num_ports = 1;
  for(i=0;i<MAX_NUM_PORTS;i++){
    port = &(p->ip_port[i]);
    port->index = i;
    port->left_selected = 0;
    port->right_selected = 0;
    port->comp_ptr = &(p->comp);
    if(i < p->num_ports)
      port->ip_addr = strsave(hname);
    else
      port->ip_addr = (char *)NULL;
    port->left_x = p->x;
    port->left_y = p->y + i*p->height + 3*p->height/8.0;
    port->right_x = p->x + 7*p->width/8.0;
    port->right_y = port->left_y;
    port->row = p->row + i;
    port->col = p->col;
    port->width = p->width/8.0;
    port->height = p->height/4.0;
    setup_ipport_func(port);
  }
  return p;
}

/*******************ConfigInfo related routines******************/

/*********************************************************************
 *          IoId *newIoId (void)                                     *
 * Description:                                                      *
 *      Create new IoId structure                                    *
 ********************************************************************/
#if defined (__STDC__)
IoId *newIoId (void)
#else
IoId *newIoId ()
#endif
{
  IoId *newId = (IoId *)malloc (sizeof (IoId));
  if (newId == NULL){
    fprintf (stderr, "Cannot allocated IoId structure \n");
    exit (1);
  }
  newId->comp_name = 0;
  newId->port_name = 0;
  return newId;
}

#if defined (__STDC__)
void freeIoId (IoId* id)
#else
void freeIoId (id)
     IoId* id;
#endif
{
  if (id->comp_name)
    free (id->comp_name);
  if (id->port_name)
    free (id->port_name);
}

#if defined (__STDC__)
static void updateIoIdInfo (IoId* id, char* buffer)
#else
static void updateIoIdInfo (id, buffer)
     IoId* id;
     char* buffer;  /* buffer in the form name:host */
#endif
{
  char *p, *q;
  char name[64], node[64];
  char hname[80];
  int  num = 0;

  if (gethostname (hname, sizeof (hname)) != 0)
    strcpy (hname, "unknown");

  p = buffer;
  q = name;
  if (strchr (buffer, ':') != 0) {
    while (*p != ':') {
      *q = *p;
      q++; p++;
    }
    *q = '\0';
    p++;
  
    strncpy (node, p, sizeof (node));
    
    id->comp_name = strsave (name);
    id->port_name = strsave (node);
  }
  else {  /* FILE and DEBUG cases */
    id->comp_name = strsave (buffer);
    id->port_name = strsave (hname);
    if (strstr (buffer, "coda") != 0) { /* coda file */
      p = buffer;
      /* buffer must be in the form coda_file_3 */
      p += (strlen ("coda") + 1); 
      num = atoi (p);
      if (num > codaFileNum)
	codaFileNum = num + 1;
    }
    else {
      if (strstr (buffer, "file") != 0) {  /* binary file situation */
	p = buffer;
	p += (strlen ("file") + 1);    /* buffer must be in the form file_3 */
	num = atoi (p);
	if (num > outputFileNum)
	  outputFileNum = num + 1;
      }
      if (strstr (buffer, "debugger") != 0) { /* coda debug case */
	p = buffer;
	p += (strlen ("debugger") + 1);
	num = atoi (p);
	if (num > debuggerNum)
	  debuggerNum = num + 1;
      }
    }
  }
}

/*******************************************************************
 *          ConfigInfo *newConfigInfo (void)                       *
 * Description:                                                    *
 *      Create a new ConfigInfo structure                          *
 ******************************************************************/
#if defined (__STDC__)
ConfigInfo *newConfigInfo (void)
#else
ConfigInfo *newConfigInfo ()
#endif
{
  ConfigInfo *cinfo = (ConfigInfo *)malloc (sizeof (ConfigInfo));
  if (cinfo == NULL) {
    fprintf (stderr, "Cannot allocate memory for ConfigInfo \n");
    exit (1);
  }
  cinfo->comp_name = 0;
  cinfo->inputs = 0;
  cinfo->num_inputs = 0;
  cinfo->outputs = 0;
  cinfo->num_outputs = 0;
  cinfo->code[0] = 0;
  cinfo->code[1] = 0;
  cinfo->code[2] = 0;
  cinfo->scripts = 0;
  cinfo->row = 0;
  cinfo->col = 0;

  return cinfo;
}

#if defined (__STDC__)
void setConfigInfoName (ConfigInfo* cinfo, char* name) 
#else
void setConfigInfoName (cinfo, name)
     ConfigInfo* cinfo;
     char* name;
#endif
{
  cinfo->comp_name = strsave (name);
}

#if defined (__STDC__)
void setConfigInfoCode (ConfigInfo* cinfo, char* code)
#else
void setConfigInfoCode (cinfo, code)
     ConfigInfo* cinfo;
     char* code;
#endif
{
  char c0[128], c1[128], c2[128];
  char *p = code;
  char *p0 = c0;
  char *p1 = c1;
  char* p2 = c2;

  /* initialize the code pointers */
  cinfo->code[0] = 0;
  cinfo->code[1] = 0;
  cinfo->code[2] = 0;

  if (p != 0) {
    /* get first code */
    while (*p != '{' && *p != '\0')
      p++;
    if (*p == '\0') 
      return;

    p++;
    while (*p != '}') {
      *p0 = *p;
      p0++; p++;
    }
    *p0 = '\0';
    if (!isEmptyLine (c0))
      cinfo->code[0] = strsave (c0);
    
    p++;
    /* get second code */
    while (*p != '{' && *p != '\0')
      p++;
    if (*p == '\0')
      return;

    p++;
    while (*p != '}') {
      *p1 = *p;
      p1++; p++;
    }
    *p1 = '\0';
    if (!isEmptyLine (c1))
      cinfo->code[1] = strsave (c1);

    p++;
    /* get third code */
    while (*p != '{' && *p != '\0')
      p++;
    if (*p == '\0')
      return;

    p++;
    while (*p != '}') {
      *p2 = *p;
      p2++; p++;
    }
    *p2 = '\0';
    p++;
    if (!isEmptyLine (c2))
      cinfo->code[2] = strsave (c2);
  }
}

#if defined (__STDC__)
void setConfigInfoInputs (ConfigInfo* cinfo, char* buffer)
#else
void setConfigInfoInputs (cinfo, buffer)
     ConfigInfo* cinfo;
     char* buffer;
#endif
{

#define MAX_LEN_TOKEN 256

  char *p_buf, *p_token;
  char token[MAX_NUM_IOS][MAX_LEN_TOKEN];  
  int  max_len_token = MAX_LEN_TOKEN;
  int  token_count;

  int  num, i;

  if (!isEmptyLine (buffer) ) {
    p_buf = buffer;
    i = 0;
    token_count = 0;
    /* skip white spaces first */
    while (isspace (*p_buf)) {
      p_buf++;
    }

    while (*p_buf != '\0') {
      p_token = token[i];
      while ( !isspace (*p_buf) && 
	      *p_buf != '\0' ) {
	if (token_count < max_len_token) {
	  /* Copy a good character. */
	  *p_token = *p_buf;
	  p_buf++;
	  p_token++;
	  token_count++;
	} else {
	  /* This should never happen. */
	  fprintf(stderr, "String too long in cedit: line %d, file %s\n",
		  __LINE__, __FILE__);
	  fprintf(stderr, "Recompile cedit with MAX_LEN_TOKEN doubled.\n");
	}
      }

      *p_token = '\0';
      token_count = 0;
      i++;
      if (i >= MAX_NUM_IOS) {
	  fprintf(stderr, "Too many components in cedit: line %d, file %s\n",
		  __LINE__, __FILE__);
	  fprintf(stderr, "Recompile cedit with MAX_NUM_IOS increased.\n");
      }

      /* skip white space more */
      if (*p_buf != '\0')
	while (isspace (*p_buf))
	  p_buf++;
    }
    cinfo->num_inputs = i;

    if (i > 0) {
      cinfo->inputs = (IoId **)malloc (i*sizeof (IoId *));
    }
    for (i = 0; i < cinfo->num_inputs; i++) {
      cinfo->inputs[i] = newIoId ();
      updateIoIdInfo (cinfo->inputs[i], token[i]);
    }
  }
}

#if defined (__STDC__)
void setConfigInfoOutputs (ConfigInfo* cinfo, char* buffer)
#else
void setConfigInfoOutputs (cinfo, buffer)
     ConfigInfo* cinfo;
     char* buffer;
#endif
{
  char *p, *q;
  char token[MAX_NUM_IOS][MAX_LEN_TOKEN];  
  int  num, i;
  int  max_len_token = MAX_LEN_TOKEN;
  int  token_count;

  if (!isEmptyLine (buffer) ) {
    p = buffer;

    /* skip white spaces first */
    while (isspace (*p))
      p++;

    i = 0;
    token_count = 0;

    while (*p != '\0') {
      q = token[i];
      while (!isspace (*p) && *p != '\0') {
	if (token_count < max_len_token) {
	  *q = *p;
	  q++;
	  p++;
	  token_count++;
	} else {
	  /* This should never happen. */
	  fprintf(stderr, "String too long in cedit: line %d, file %s\n",
		  __LINE__, __FILE__);
	  fprintf(stderr, "Recompile cedit with MAX_LEN_TOKEN doubled.\n");
	}
      }
      *q = '\0';
      i++;
      token_count = 0;
      /* skip white space more */
      if (*p != '\0')
	while (isspace (*p))
	  p++;
    }
    cinfo->num_outputs = i;
    if (i > 0) 
      cinfo->outputs = (IoId **)malloc (i*sizeof (IoId *));
    for (i = 0; i < cinfo->num_outputs; i++) {
      cinfo->outputs[i] = newIoId ();
      updateIoIdInfo (cinfo->outputs[i], token[i]);
    }
  }
}

#if defined (__STDC__)
void setConfigInfoPosition (ConfigInfo* cinfo, int row, int col)
#else
void setConfigInfoPosition (cinfo, row, col)
     ConfigInfo* cinfo;
     int row, col;
#endif
{
  cinfo->row = row;
  cinfo->col = col;
}

#if defined (__STDC__)
void addScriptToConfigInfo (ConfigInfo* cinfo, char* state, char* script)
#else
void addScriptToConfigInfo (cinfo, state, script)
     ConfigInfo* cinfo;
     char*       state;
     char*       script;
#endif
{
  codaScript* p;

  if (cinfo->scripts == 0) 
    cinfo->scripts = newCodaScript (state, script);
  else 
    addScriptToList (cinfo->scripts, state, script);
}

#if defined (__STDC__)
int matchConfigInfo (ConfigInfo* cinfo, char* name)
#else
int matchConfigInfo (cinfo, name)
     ConfigInfo* cinfo;
     char* name;
#endif
{
  if (strcmp (cinfo->comp_name, name) == 0)
    return 1;
  return 0;
}
  

#if defined (__STDC__)
void freeConfigInfo (ConfigInfo* cinfo)
#else
void freeConfigInfo (cinfo)
     ConfigInfo* cinfo;
#endif
{
  int i;
  
  if (cinfo->comp_name)
    free (cinfo->comp_name);

  if (cinfo->inputs) {
    for (i = 0; i < cinfo->num_inputs; i++)
      freeIoId (cinfo->inputs[i]);
    
    free (cinfo->inputs);
  }
  if (cinfo->outputs) {
    for (i = 0; i < cinfo->num_outputs; i++)
      freeIoId (cinfo->outputs[i]);
    free (cinfo->outputs);
  }

  if (cinfo->code[0])
    free (cinfo->code[0]);
  if (cinfo->code[1])
    free (cinfo->code[1]);
  if (cinfo->code[2])
    free (cinfo->code[2]);

  if (cinfo->scripts)
    freeCodaScriptList (cinfo->scripts);

  free (cinfo);
}

/************************************************************************
 *        rcNetComp* newRcNetComp (void)                                *
 * Description:                                                         *
 *      Create a new rcNetComp with all initial value set               *
 ***********************************************************************/
#if defined (__STDC__)
rcNetComp* newRcNetComp (void)
#else
rcNetComp* newRcNetComp ()
#endif
{
  rcNetComp* comp = (rcNetComp *)malloc (sizeof(rcNetComp));
  
  comp->daq.type = CODA_UNKNOWN;
  comp->daq.comp_name = 0;
  comp->daq.node_name = 0;
  comp->daq.id_num = -1;
  comp->daq.boot_string = 0;
  comp->daq.status = 0;
  comp->daq.code[0] = 0;
  comp->daq.code[1] = 0;
  comp->daq.code[2] = 0;

  comp->port_name = (char **)malloc(MAX_NUM_PORTS*sizeof(char *));
  comp->num_ports  = 0;

  return comp;
}


/************************************************************************
 *        void  freeRcNetComp (rcNetComp* comp)                         *
 * Description:                                                         *
 *      Free a new rcNetComp with all initial value set                 *
 ***********************************************************************/
#if defined (__STDC__)
void freeRcNetComp (rcNetComp* comp)
#else
void freeRcNetComp (comp)
     rcNetComp* comp;
#endif
{
  int i = 0;

  free_daqcomp_res (&(comp->daq));
  if (comp->num_ports > 0) {
    for (i = 0; i < comp->num_ports; i++)
      free (comp->port_name[i]);
  }
  free (comp->port_name);
  comp->num_ports = 0;
  free (comp);
  comp = 0;
}

/************************************************************************
 *        void  setRcNetComp (rcNetComp* comp, char* name, int id,      *
                              char* cmd, char* type, char* host)        *
 * Description:                                                         *
 *      setup a rcNetComp with all initial value set                    *
 ***********************************************************************/
#if defined (__STDC__)
void setRcNetComp (rcNetComp* comp, 
		   char* name, 
		   int id, 
		   char* cmd, 
		   char* comp_type, 
		   char* host)
#else
void setRcNetComp (comp, name, id, cmd, comp_type, host)
     rcNetComp* comp;
     char*      name;
     int        id;
     char*      cmd;
     char*      comp_type;
     char*      host;
#endif
{
  comp->daq.comp_name = strsave (name);
  if (comp->daq.type == CODA_NONE) {
    comp->daq.node_name = strsave("none");
  } else {
    comp->daq.node_name = strsave (host);
  }
  comp->daq.id_num = id;
  if (cmd != 0)
    comp->daq.boot_string = strsave (cmd);
  else
    comp->daq.boot_string = (char *)0;
  if(strcasecmp(comp_type, "ROC") == 0)
    comp->daq.type = CODA_ROC;
  else if(strcasecmp(comp_type, "EB") == 0)
    comp->daq.type = CODA_EB;
  else if(strcasecmp(comp_type, "TS") == 0)
    comp->daq.type = CODA_TRIG;    
  else if(strcasecmp(comp_type, "RCS") == 0)
    comp->daq.type = CODA_RCS;
  else if(strcasecmp(comp_type,"ER") == 0)
    comp->daq.type = CODA_ER;
  else if(strcasecmp(comp_type,"UC") == 0)
    comp->daq.type = CODA_UC;
  else if(strcasecmp(comp_type,"UT") == 0)
    comp->daq.type = CODA_UT;
  else if(strcasecmp(comp_type,"LOG") == 0)
    comp->daq.type = CODA_LOG;
  else if(strcasecmp(comp_type,"SC") == 0)
    comp->daq.type = CODA_SC;
  else if(strcasecmp(comp_type,"FILE") == 0)
    comp->daq.type = CODA_FILE;
  else if(strcasecmp(comp_type, "CODAFILE") == 0)
    comp->daq.type = CODA_CODAFILE;
  else if(strcasecmp(comp_type,"DEBUG") == 0)
    comp->daq.type = CODA_DEBUG;
  else if(strcasecmp(comp_type,"MON") == 0)
    comp->daq.type = CODA_MON;
  else if(strcasecmp(comp_type,"NONE") == 0)
    comp->daq.type = CODA_NONE;
  else
    comp->daq.type = CODA_UNKNOWN;
  comp->daq.status = 0;

  /* set initial port name */
  comp->num_ports = 1;
  comp->port_name[0] = strsave (host);
}

/*******************Dynamic create/destruc a double array********/
#if defined (__STDC__)
static drawComp ***meshCreate(int m, int n)
#else
static drawComp ***meshCreate(m, n)
int m, n;  /* m is row number, n is column number */
#endif
{
  int      i, j;
  drawComp ***mesh = 0;

  mesh = (drawComp ***)malloc (m*sizeof(drawComp **));
  if (mesh == NULL){
    fprintf(stderr,"Cannot allocate memory for mesh\n");
    exit (1);
  }
  for (i = 0; i < m; i++){
    mesh[i] = (drawComp **)malloc(n*sizeof(drawComp *));
    if (mesh[i] == 0){
      fprintf(stderr,"Cannot allocate memory for mesh\n");
      exit (1);
    }
    for (j = 0; j < n; j++)
      mesh[i][j] = 0;
  }
  return mesh;
}

#if defined (__STDC__)
static void meshDestroy (drawComp*** mesh, 
			 int m, int n)
#else
static void meshDestroy (mesh, m, n)
drawComp ***mesh;
int m, n;
#endif
{
  int i, j;

  for (i = 0; i < m; i++)
    free (mesh[i]);
  free (mesh);
}

/***********************************************************
 *    void findNextComp (type, mesh, maxcol, maxrow)       *
 * Description:                                            *
 *    Find out the very next component than has the same   *
 *    type as the 'type'                                   *
 *    return 0: not found:                                 *
 ***********************************************************/
#if defined (__STDC__)
static char* findNextComp (int type, 
			   drawComp*** mesh, 
			   int m, 
			   int n, 
			   int maxcol, 
			   int maxrow)
#else
static char* findNextComp (type, mesh, m, n, maxcol, maxrow)
     int       type;
     drawComp*** mesh;
     int m, n;          /* current col, row number */
     int maxcol, maxrow;
#endif
{
  int i, j;
  drawComp* comp;
  daqComp*  daq;

  /* find the same type of component from the next row */
  for (i = m; i < maxcol; i++) {
    for (j = n + 1; j < maxrow; j++) {
      if (mesh[j][i] != 0){      
	comp = mesh[j][i];
	daq = &(comp->comp);
	/* component must have outputs to be in the list */
	if (daq->type == type && compHasOutputs (comp))
	  return daq->comp_name;
      }
    }
  }
  return 0;
}


/***********************************************************
 *    int firstComp (type, mesh, maxcol, maxrow)           *
 * Description:                                            *
 *    find out wheather this component is the first one    *
 *    return 1: yes, return 0: no                          *
 ***********************************************************/
#if defined (__STDC__)
static int firstComp (int type, 
		      drawComp*** mesh, 
		      int m, 
		      int n)
#else
static int firstComp (type, mesh, m, n)
     int       type;
     drawComp*** mesh;
     int m, n;          /* current col, row number */
#endif
{
  int i, j;
  drawComp* comp;
  daqComp*  daq;

  /* find the same type of component from the next row */
  for (i = 0; i <= m; i++) {
    for (j = 0; j < n; j++) {
      if (mesh[j][i] != 0){      
	comp = mesh[j][i];
	daq = &(comp->comp);
	if (daq->type == type)
	  return 0;
      }
    }
  }
  return 1;
}

/***********************************************************
 *    void XcodaEditorWriteToConfig()                      *
 * Description:                                            *
 *    Write all infomation to configuration file           *
 ***********************************************************/
void
XcodaEditorWriteToConfig(char* config_name, XcodaEditorGraph* graph)
{
  ipPort   *port;
  daqComp  *daq;
  drawComp *comp;
  compList *p;
  char     inputStr[2048], outputStr[2048], code[2048];
  char     c[3][500];
  char     **inputs, **outputs;
  char     *nextComp;
  int      i, j, m, n, num_inputs, num_outputs;
  int      fComp;
  int      maxrow, maxcol;
  drawComp ***mesh;
  int      err = 0;
  char     temp[1024];

  if (isEmptyGraph (graph)) return;


  /* remove old position and configuration table */

  removeConfigTable(config_name);


  /* create new tables */

  if (createPositionTable(config_name) < 0)
  {
    sprintf (code, "Cannot create position table %s_pos\n", config_name);
    pop_error_message (code, sw_geometry.draw_area);
    return;
  }

  if (createConfigTable(config_name) < 0)
  {
    sprintf (code, "Cannot create config table %s_config\n", config_name);
    pop_error_message (code, sw_geometry.draw_area);
    return;
  }
 
  sprintf (temp, "%s_option", config_name);
  if (!isTableCreated (temp)) createOptionTable (config_name);
  
  createScriptTable (config_name);
  


  /* sort all these components according their x and y coor     */
  /* Reason: the component sequesnce should go like top->bottom */
  /*         left->right, this way, it is easy to assign        */
  /*         to a perticular component                          */
  maxrow = maxcol = 0;
  for(p = graph->comp_list_head->next; p != graph->comp_list_tail;p=p->next)
  {
    comp = p->draw_comp;
    if (comp->row > maxrow) maxrow = comp->row;
    if (comp->col > maxcol) maxcol = comp->col;
  }
  mesh = meshCreate (maxrow, maxcol);
  
  for(p = graph->comp_list_head->next; p != graph->comp_list_tail;p=p->next)
  {
    comp = p->draw_comp;
    mesh[comp->row - 1][comp->col - 1] = comp;
  }

  /* write the configuration information to the database */
  for (m = 0; m < maxcol; m++)
  {
    for (n = 0; n < maxrow ; n++)
    {
      if (mesh[n][m] != 0)
      {
        comp = mesh[n][m];
        daq = &(comp->comp);

        /* update position table first */
        err = insertValToPosTable (config_name, daq->comp_name, n+1,m+1);

        /* update script table */
        err = insertValToScriptTable (config_name, daq->comp_name,
				                         comp->scripts);

	    /* update configuration table */
	    inputs = XcodaEditorCompGetAllInputs(graph, comp, &num_inputs);
	    if(num_inputs != 0)
        {
	      strcpy (inputStr, inputs[0]);
	      for(i = 1; i < num_inputs; i++)
          {
	        strcat (inputStr, " ");
	        strcat (inputStr,inputs[i]);
	      }
	    }
	    else
	      strcpy (inputStr, "");
	
	    outputs = XcodaEditorCompGetAllOutputs(graph, comp, &num_outputs);
	    if(num_outputs != 0)
        {
	      if (outputs[0]) strcpy (outputStr, outputs[0]);
	      for(i = 1; i < num_outputs; i++)
          {
	        strcat (outputStr, " ");
	        if (outputs[i]) strcat (outputStr,outputs[i]);
	      }
	    }
	    else
	      strcpy (outputStr, "");

	    if (daq->code[0]) 
	      sprintf (c[0], "{%s}", daq->code[0]);
	    else
	      sprintf (c[0], "");

	    if (daq->code[1]) 
	      sprintf (c[1], "{%s}", daq->code[1]);
	    else
	      sprintf (c[1], "");

	    if (daq->code[2]) 
	      sprintf (c[2], "{%s}", daq->code[2]);
	    else
	      sprintf (c[2], "");
	    sprintf (code, "%s %s %s", c[0], c[1], c[2]);

	    /* find next the same type of component on the list */
	    nextComp = findNextComp (daq->type, mesh, m, n, maxcol, maxrow);
	    /* find whether this component is the first one */
	    fComp = firstComp (daq->type, mesh, m, n);
	    /* update the configuration table */
	    err = insertValToConfigTable (config_name, daq->comp_name, code, 
				      inputStr, outputStr,
				      nextComp, fComp);
	
	    /* free memories */
	    if (num_inputs)
        {
	      for (i = 0; i < num_inputs; i++)
	        free (inputs[i]);
	      free (inputs);
	    }
	    if (num_outputs)
        {
	      for (i = 0; i < num_outputs; i++)
	        free (outputs[i]);
	      free (outputs);
	    }
      }
    }
  }
  meshDestroy (mesh, maxrow, maxcol);

  /* save option information */
  XcodaEditorSaveConfigOption (config_name);

  if (err == -1) {
    pop_error_message ("Insert new values to database failed", 
		       sw_geometry.draw_area);
    return;
  }
  /* reset graph modified flag */
  graph->modified = 0;
}
    

/********************************************************
 *   void XcodaEditorWriteToRcnet()                     *
 * Description:                                         *
 *    updating rcNetwork file                           *
 *******************************************************/
#if defined (__STDC__)
void XcodaEditorInsertAllComps (XcodaEditorGraph* graph)
#else
void XcodaEditorInsertAllComps (graph)
     XcodaEditorGraph *graph;
#endif
{
  daqComp *daq;
  compList *p;

  /* insert all other regular components */
  for(p=graph->comp_list_head->next; p != graph->comp_list_tail; p = p->next){
    daq = &(p->draw_comp->comp); 
    if (daq->type != CODA_DEBUG && daq->type != CODA_FILE && 
	daq->type != CODA_CODAFILE) {
      if (isDaqCompInProcTable (daq->comp_name)) 
	updateDaqCompToProcTable   (daq);
      else
	insertDaqCompToProcTable   (daq);
    }
  }
}
  
/*****************************************************************
 *     int getConfigurationInfo(char* config, configInfo** info, *
 *                              int *numCinfos)                  *
 * Description:                                                  *
 *     get configuration information from a config file          *
 *     return 0: success, return -1: failure                     *
 *     callers have full control of memory of info               *
 ****************************************************************/
#if defined (__STDC__)
int getConfigurationInfo (char* config, ConfigInfo** cinfo, int* num)
#else
int getConfigurationInfo (config, cinfo, num)
     char* config;
     ConfigInfo** cinfo;
     int* num;
#endif
{
  return retrieveConfigInfoFromDbase (config, cinfo, num);
}

/**********************************************************************
 *      static void updateCompAuxInfo(cinfo, num_cinfo, comp)         *
 * Description:                                                       *
 *     Fill out information about component's code, user_string       *
 *     and port names                                                 *
 *********************************************************************/
#if defined (__STDC__)
static void updateCompAuxInfo(ConfigInfo** cinfo, 
			      int num_cinfo, 
			      rcNetComp* comp)
#else
static void updateCompAuxInfo(cinfo, num_cinfo, comp)
     ConfigInfo **cinfo;
     int        num_cinfo;
     rcNetComp  *comp;
#endif
{
  int        i, j, k;
  daqComp    *daq = &(comp->daq);
  IoId       *io;
  daq->status = 0;
  /* update code and user_string first */
  for(i=0;i<num_cinfo;i++){
    if(strcmp(daq->comp_name, cinfo[i]->comp_name)==0){
      if(daq->code[0] != NULL)
	free(daq->code[0]);
      if(cinfo[i]->code[0] != NULL){
	daq->code[0] = strsave(cinfo[i]->code[0]);
      }

      if(daq->code[1] != NULL)
	free(daq->code[1]);
      if(cinfo[i]->code[1] != NULL){
	daq->code[1] = strsave(cinfo[i]->code[1]);
      }

      if(daq->code[2] != NULL)
	free(daq->code[2]);
      if(cinfo[i]->code[2] != NULL){
	daq->code[2] = strsave(cinfo[i]->code[2]);
      }
      break;
    }
  }
  /* update port name and num_port here */
  for(i = 0; i < num_cinfo; i++){
    for(j = 0; j < cinfo[i]->num_inputs; j++){
      io = cinfo[i]->inputs[j];
      if(strcmp(daq->comp_name,io->comp_name) == 0){
	for(k=0;k<comp->num_ports;k++){
	  if(strcmp(comp->port_name[k],io->port_name) == 0)
	    break;
	}
	if(k >= comp->num_ports){
	  comp->port_name[k] = strsave(io->port_name);
	  comp->num_ports = comp->num_ports + 1;
	}
      }
    }
    for(j=0;j<cinfo[i]->num_outputs;j++){
      io = cinfo[i]->outputs[j];
      if(strcmp(daq->comp_name,io->comp_name) == 0){
	for(k=0;k<comp->num_ports;k++){
	  if(strcmp(comp->port_name[k],io->port_name) == 0)
	    break;
	}
	if (k >= comp->num_ports) {
	  comp->port_name[k] = strsave(io->port_name);
	  comp->num_ports = comp->num_ports + 1;
	}
      }
    }
  }
}    

/************************************************************************
 *           int constructRcnetComps(rcNetComp** comp, int* num)        *
 * Description:                                                         *
 *     construct network component from database                        *
 *     Callers have full control of comp[i] if return success           *
 *     return 0: success. return -1: error                              *
 ***********************************************************************/
#if defined (__STDC__)
int constructRcnetComps(rcNetComp** comps, int* num)
#else
int constructRcnetComps(comps, num)
     rcNetComp** comps;
     int*        num;
#endif
{
  char* configs[EDITOR_MAX_CONFIGS];
  int  numConfigFiles = 0;
  int  i, j;
  ConfigInfo* cinfo[MAX_NUM_COMPS];
  int  num_cinfo;

  if (createRcNetCompsFromDbase (comps, num) < 0) 
    return -1;


  /*  if (listAllConfigs (configs, &numConfigFiles) < 0)
    return -1;

  for (i = 0; i < numConfigFiles; i++) {
    if (getConfigurationInfo(configs[i], cinfo, &num_cinfo) == -1) {
      for(j = 0;j < *num; j++)
	free(comps[j]);
      comps = (rcNetComp **)NULL;
      *num = 0;
      return -1;
    }
    for(j = 0; j< *num; j++)
      updateCompAuxInfo(cinfo, num_cinfo, comps[j]);

    for(j=0;j<num_cinfo;j++)
      freeConfigInfo(cinfo[j]);
  }*/
  return 0;
}


/************************************************************************
 *           int constructRcnetCompsWithConfig (char* configname,       *
 *                                   rcNetComp** comp, int* num)        *
 * Description:                                                         *
 *     construct network component from database with configname        *
 *     Callers have full control of comp[i] if return success           *
 *     return 0: success. return -1: error                              *
 ***********************************************************************/
#if defined (__STDC__)
int constructRcnetCompsWithConfig (char* config, 
				   rcNetComp** comps, int* num,
				   ConfigInfo** cinfos,
				   int* num_cinfo)
#else
int constructRcnetComps(config, comps, num, cinfos, num_cinfo)
     rcNetComp** comps;
     int*        num;
     ConfigInfo** cinfos;
     int*         num_cinfo;
#endif
{
  int  i, j;

  if (createRcNetCompsFromDbase (comps, num) < 0) 
    return -1;

  if (getConfigurationInfo(config, cinfos, num_cinfo) == -1) {
    for(j = 0;j < *num; j++)
      free(comps[j]);
    comps = (rcNetComp **)NULL;
    *num = 0;
    cinfos = (ConfigInfo **)NULL;
    *num_cinfo = 0;
    return -1;
  }
  for(j = 0; j< *num; j++) 
    updateCompAuxInfo(cinfos, *num_cinfo, comps[j]);

  return 0;
}

  
/****************************************************************************
 *     void XcodaEditorConstructGraphFromConfig()                           *
 * Description:                                                             *
 *     Construct coda graph object from configuration file                  *
 *     This routine must be called after constructRcnetComps()              *
 *     and getConfigurationInfo()                                           *
 ***************************************************************************/
#if defined (__STDC__)
void XcodaEditorConstructGraphFromConfig(XcodaEditorGraph* graph, 
					 rcNetComp** daq_list, 
					 int num_daqs, 
					 ConfigInfo** cinfo, 
					 int num_cinfos)
#else
void XcodaEditorConstructGraphFromConfig(graph, daq_list, num_daqs, 
					 cinfo, num_cinfos)
     XcodaEditorGraph *graph;
     rcNetComp        **daq_list;
     int              num_daqs;
     ConfigInfo       **cinfo;
     int              num_cinfos;
#endif
{
  rcNetComp *rc;
  drawComp  *dc;
  daqComp   *daq;
  ipPort    *port;
  Arc       *arc;
  int       i, j, error;
  
  for(i=0;i<num_cinfos;i++){
    for(j=0;j<num_daqs;j++){
      if(strcmp(cinfo[i]->comp_name, daq_list[j]->daq.comp_name) == 0)
	break;
    }
    if (j < num_daqs) {
      rc = daq_list[j];
      dc = createDrawCompFromRcnetComp(graph,rc, cinfo[i]);
    }
    else {
      if (strstr (cinfo[i]->comp_name, "coda") != 0) 
	dc = createExtraDrawCompFromCinfo (graph, cinfo[i], CODA_CODAFILE);
      else 
	dc = createExtraDrawCompFromCinfo (graph, cinfo[i], CODA_FILE);
    }
    insertNodeToCompList(graph, dc);
  }
  createArcsFromCinfos(graph,cinfo,num_cinfos, &error);
  if(error != 0)
    pop_error_message("Error:Inconsistent database, can't create graph.", 
		      sw_geometry.draw_area);
  /* reset modified flag */
  graph->modified = 0;
}


/*************************************************************************
 *              int compNotUsed(comp_name)                               *
 * Description:                                                          *
 *     Check whether a component with name 'comp_name' used in the       *
 *     rcDatabase or not                                                 *
 *     return 1: yes, 0: used                                            *
 ************************************************************************/
#if defined (__STDC__)
int compNotUsed (char *comp_name)
#else
int compNotUsed (comp_name)
     char *comp_name;
#endif
{
  int i;

  if (compInConfigTables (comp_name))
    return 0;

  removeDaqCompFromProcTable (comp_name);
  return 1;
}

