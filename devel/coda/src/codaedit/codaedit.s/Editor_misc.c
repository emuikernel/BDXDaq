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
 *	CODA Editor misc routines
 *	
 * Author:  Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_misc.c,v $
 *   Revision 1.3  1997/11/24 14:50:28  heyes
 *   bombproof strsave
 *
 *   Revision 1.2  1997/09/08 15:19:36  heyes
 *   fix dd icon etc
 *
 *   Revision 1.1.1.2  1996/11/05 17:45:15  chen
 *   coda source
 *
 *	  
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "Editor.h"

#if defined (__STDC__)
char *strsave(char* s)
#else
char *strsave(s)
     char *s;
#endif
{
  char *p;
  if (s == NULL) {
    s = "";
    fprintf(stderr,"NULL string passed to strsave\n");
  }

  p = (char *)malloc(strlen(s)+2);
  if(p == NULL){
    fprintf(stderr,"Cannot allocate memory in strsave.\n");
    exit(1);
  }
  strcpy(p, s);

  return p;
}

/******************************************************************
 *        int isEmptyLine(s)                                      *
 * Description:                                                   *
 *    check whether we have a empty string or not                 *
 *****************************************************************/
#if defined (__STDC__)
int isEmptyLine(char* s)
#else
int isEmptyLine(s)
     char *s;
#endif
{
  char *p;

  p = s;
  while(*p != '\0' && *p != '\n'){
    if(!isspace(*p))
      return 0;
    p++;
  }
  return 1;
}

/*****************************************************************
 *        Related routines for codaScript structure              *
 ****************************************************************/
codaScript*
#if defined (__STDC__)
newCodaScriptList (void)
#else
newCodaScriptList ()
#endif
{
  codaScript* cs = (codaScript *)malloc (sizeof (codaScript));
  if (cs == 0) {
    fprintf (stderr, "Malloc error\n");
    exit (1);
  }
  cs->state = strsave ("dormant");
  cs->script = strsave ("null");
  cs->next = 0;
  return cs;
}

/* create empty new codaScript item     */
codaScript*
#if defined (__STDC__)
newCodaScript (char* state, char* script)
#else
newCodaScript (state, script)
     char* state;
     char* script;
#endif
{
  codaScript* cs = (codaScript *)malloc (sizeof (codaScript));
  if (cs == 0) {
    fprintf (stderr, "Malloc error\n");
    exit (1);
  }
  cs->state = strsave (state);
  cs->script = strsave (script);
  cs->next = 0;
  return cs;
}

/* add a codaScript item to the list pointed by a codaScript */\
void
#if defined (__STDC__)
addCodaScriptToList (codaScript* list, codaScript* item)
#else
addCodaScriptToList (list, item)
     codaScript* list;
     codaScript* item;
#endif
{
  codaScript* p;

  for (p = list; p->next != 0; p = p->next)
    ;
  p->next = item;
}

/* add a codaScript to the list only if there is this one */
int
#if defined (__STDC__)
addScriptToList (codaScript* list, char* state, char* script)
#else
addScriptToList (list, state, script)
     codaScript* list;
     char* state;
     char* script;
#endif
{
  codaScript* p;

  for (p = list; p != 0; p = p->next) {
    if (strcasecmp (p->state, state) == 0 &&
	strcmp (p->script, script) == 0)
      return -1;
  }
  p = newCodaScript (state, script);
  addCodaScriptToList (list, p);
  return 0;
}

/* duplicate a codaScript item */
static codaScript*
#if defined (__STDC__)
duplicateCodaScript (codaScript* item)
#else
duplicateCodaScript (item)
     codaScript* item;
#endif
{
  return newCodaScript (item->state, item->script);
}

/* Duplicate a whole list */
codaScript*
#if defined (__STDC__)
duplicateCodaScriptList (codaScript* list)
#else
duplicateCodaScriptList (list)
     codaScript* list;
#endif
{
  codaScript* newlist;
  codaScript* p;
  codaScript* q;

  if (list == 0)
    return 0;

  newlist = duplicateCodaScript (list);
  for (p = list->next; p != 0; p = p->next) {
    q = duplicateCodaScript (p);
    addCodaScriptToList (newlist, q);
  }
  return newlist;
}
  
  

/* free codaScript item */
void
#if defined (__STDC__)
freeCodaScript (codaScript* item)
#else
freeCodaScript (item)
     codaScript* item;
#endif
{
  free (item->script);
  free (item->state);
  item->next = 0;
}

/* free whole list of scripts */
void
#if defined (__STDC__)
freeCodaScriptList (codaScript* list)
#else
freeCodaScriptList (list)
     codaScript* list;
#endif
{
  codaScript* curr = list;
  codaScript* next = list->next;

  freeCodaScript (curr);
  while (next) {
    curr = next;
    next = curr->next;
    freeCodaScript (curr);
  }
}

