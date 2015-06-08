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
 *	CODA Editor syntax checker
 *	
 * Author: Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_syntax_checker.c,v $
 *   Revision 1.3  1998/06/18 12:28:32  heyes
 *   tidy up a lot of thingscd ../cedit
 *
 *   Revision 1.2  1997/06/20 17:00:21  heyes
 *   clean up GUI!
 *
 *   Revision 1.1.1.2  1996/11/05 17:45:17  chen
 *   coda source
 *
 *	  
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include "Editor_graph.h"
#include "Editor_icon_box.h"

/**************************************************************
 *            int compNameOk (char *name)                     *
 * Description:                                               *
 *    Check whether there is a duplicated component name      *
 *    return > 0, for ok, return o for false                  *
 *************************************************************/
#if defined (__STDC__)
int compNameOk (drawComp* comp, char* name)
#else
int compNameOk (comp, name)
     drawComp *comp;
     char *name;
#endif
{
  compList *p;
  XcodaEditorGraph *g = &coda_graph;

  for (p = g->comp_list_head->next; p != g->comp_list_tail; p = p->next){
      if (p->draw_comp != comp && p->draw_comp->comp.comp_name){
	if (strcasecmp (p->draw_comp->comp.comp_name, name) == 0)
	  return 0;
      }
    }
  return (nameOkWithIconList (name));
}

/*************************************************************
 *            int compHostOk (char *host)                    *
 * Description:                                              *
 *     Check input host name is indeed on the network        *
 *     return 1: ok, return 0: false                         *
 ************************************************************/
#if defined (__STDC__)
int compHostOk (drawComp* comp, char* host)
#else
int compHostOk (comp, host)
     drawComp *comp;
     char     *host;
#endif
{
  struct hostent *ret;
  unsigned long inaddr;

  return 1;
}


/*************************************************************
 *           int compIdOk (drawComp *comp, int id)           *
 * Description:                                              *
 *     Check whether there is duplication of this id number  *
 *     return 1: ok, return 0: false                         *
 ************************************************************/
#if defined (__STDC__)
int compIdOk (drawComp* comp, int id)
#else
int compIdOk (comp, id)
     drawComp *comp;
     int      id;
#endif
{
  compList *p;
  XcodaEditorGraph *g = &coda_graph;

  if (comp->comp.type > CODA_ER)
    return 1;

  for (p = g->comp_list_head->next; p != g->comp_list_tail; p = p->next){
    if (p->draw_comp != comp){
      if (comp->comp.type != CODA_EBANA){
	if (p->draw_comp->comp.type == comp->comp.type &&
	    p->draw_comp->comp.id_num == id)
	  return 0;
      }
      else{
	if ((p->draw_comp->comp.type == CODA_EB) &&
	    p->draw_comp->comp.id_num == id)
	  return 0;
      }
    }
  }
  return (idOkWithIconList (comp->comp.comp_name, id, comp->comp.type));  
}
