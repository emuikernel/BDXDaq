#ifndef production
static char rcsId[]="$Header: /net/mizar/usr/local/source/coda_source/rc/XmHTML-1.1.4/XmHTML-1.1.4/src/events.c,v 1.1.1.1 1998/08/25 17:48:13 rwm Exp $";
#endif
/*****
* events.c : HTML4.0 event routines
*
* This file Version	$Revision: 1.1.1.1 $
*
* Creation date:		Fri Nov 14 14:48:28 GMT+0100 1997
* Last modification: 	$Date: 1998/08/25 17:48:13 $
* By:					$Author: rwm $
* Current State:		$State: Exp $
*
* Author:				newt
*
* Copyright (C) 1994-1997 by Ripley Software Development 
* All Rights Reserved
*
* This file is part of the XmHTML Widget Library
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library; if not, write to the Free
* Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*****/
/*****
* ChangeLog 
* $Log: events.c,v $
* Revision 1.1.1.1  1998/08/25 17:48:13  rwm
*   Initial XmHTML-1.1.4 with RWM fixes.
*
* Revision 1.2  1998/04/27 06:59:11  newt
* tka stuff
*
* Revision 1.1  1998/04/04 06:27:19  newt
* Initial Revision
*
*****/ 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Local includes */
#include "toolkit.h"
#include XmHTMLPrivateHeader
#include "XmHTMLfuncs.h"

/*** External Function Prototype Declarations ***/

/*** Public Variable Declarations ***/

/*** Private Datatype Declarations ****/

/*** Private Function Prototype Declarations ****/

/*** Private Variable Declarations ***/
String event_names[XmCR_HTML_USEREVENT] = {
	"onload", "onunload", "onsubmit", "onreset", "onfocus", "onblur",
	"onselect", "onchange", "onclick", "ondblclick", "onmousedown",
	"onmouseup", "onmouseover", "onmousemove", "onmouseout",
	"onkeypress", "onkeydown", "onkeyup"
};

static HTEvent*
storeEvent(XmHTMLWidget html, int type, XtPointer data)
{
	int i;

	/*
	* check event array to see if we've already got an event with the
	* same data
	*/
	for(i = 0; i < html->html.nevents; i++)
		if(html->html.events[i].data == data)
			return(&html->html.events[i]);

	/* not yet in event array */
	if(html->html.nevents)
	{
		html->html.events = (HTEvent*)realloc(html->html.events,
								sizeof(HTEvent)*(html->html.nevents+1));
	}
	else	/* no event array yet */
		html->html.events  = (HTEvent*)malloc(sizeof(HTEvent));

	html->html.events[html->html.nevents].type = type;
	html->html.events[html->html.nevents].data = data;
	html->html.nevents++;
	return(&html->html.events[html->html.nevents-1]);
}

static HTEvent*
checkEvent(XmHTMLWidget html, int type, String attributes)
{
	String chPtr;
	XtPointer data;

	if(attributes != NULL &&
		(chPtr = _XmHTMLTagGetValue(attributes, event_names[type])) != NULL)
	{
		if((data = html->html.event_proc((Widget)html, chPtr,
			html->html.client_data)) != NULL)
		{
			free(chPtr);
			return(storeEvent(html, type, data));
		}
		free(chPtr);
	}
	return(NULL);
}


AllEvents*
_XmHTMLCheckCoreEvents(XmHTMLWidget html, String attributes)
{
	AllEvents events;
	AllEvents *events_return = NULL;
	Boolean have_events = False;

	/* don't do a damn thing if we can't process any scripts or events */
	if(!html->html.event_proc || !html->html.event_callback)
		return(NULL);

	/* reset */
	(void)memset(&events, 0, sizeof(AllEvents));

	/* process all possible core events */
	if((events.onClick = checkEvent(html, XmCR_HTML_CLICK,
		attributes)) != NULL)
		have_events = True;
	if((events.onDblClick = checkEvent(html, XmCR_HTML_DOUBLE_CLICK,
		attributes)) != NULL)
		have_events = True;
	if((events.onDblClick = checkEvent(html, XmCR_HTML_MOUSEDOWN,
		attributes)) != NULL)
		have_events = True;
	if((events.onMouseUp = checkEvent(html, XmCR_HTML_MOUSEUP,
		attributes)) != NULL)
		have_events = True;
	if((events.onMouseDown = checkEvent(html, XmCR_HTML_MOUSEOVER,
		attributes)) != NULL)
		have_events = True;
	if((events.onMouseMove = checkEvent(html, XmCR_HTML_MOUSEMOVE,
		attributes)) != NULL)
		have_events = True;
	if((events.onMouseOut = checkEvent(html, XmCR_HTML_MOUSEOUT,
		attributes)) != NULL)
		have_events = True;
	if((events.onKeyPress = checkEvent(html, XmCR_HTML_KEYPRESS,
		attributes)) != NULL)
		have_events = True;
	if((events.onKeyDown = checkEvent(html, XmCR_HTML_KEYDOWN,
		attributes)) != NULL)
		have_events = True;
	if((events.onKeyUp = checkEvent(html, XmCR_HTML_KEYUP,
		attributes)) != NULL)
		have_events = True;

	/* alloc & copy if we found any events */
	if(have_events)
	{
		events_return = (AllEvents*)malloc(sizeof(AllEvents));
		events_return = memcpy(events_return, (const void*)&events,
							sizeof(AllEvents));
	}
	return(events_return);
}

AllEvents*
_XmHTMLCheckFormEvents(XmHTMLWidget html, String attributes)
{
	AllEvents events;
	AllEvents *events_return = NULL;
	Boolean have_events = False;

	/* don't do a damn thing if we can't process any scripts or events */
	if(!html->html.event_proc || !html->html.event_callback)
		return(NULL);

	/* reset */
	(void)memset(&events, 0, sizeof(AllEvents));

	/* check core events */
	if((events_return = _XmHTMLCheckCoreEvents(html, attributes)) != NULL)
		have_events = True;

	/* process all possible form events */
	if((events.onSubmit = checkEvent(html, XmCR_HTML_SUBMIT,
		attributes)) != NULL)
		have_events = True;
	if((events.onReset = checkEvent(html, XmCR_HTML_RESET,
		attributes)) != NULL)
		have_events = True;
	if((events.onFocus = checkEvent(html, XmCR_HTML_FOCUS,
		attributes)) != NULL)
		have_events = True;
	if((events.onBlur = checkEvent(html, XmCR_HTML_BLUR,
		attributes)) != NULL)
		have_events = True;
	if((events.onSelect = checkEvent(html, XmCR_HTML_SELECT,
		attributes)) != NULL)
		have_events = True;
	if((events.onChange = checkEvent(html, XmCR_HTML_CHANGE,
		attributes)) != NULL)
		have_events = True;

	/* alloc & copy if we found any events */
	if(have_events)
	{
		/* no core events found */
		if(!events_return)
		{
			events_return = (AllEvents*)malloc(sizeof(AllEvents));
			events_return = memcpy(events_return, (const void*)&events,
								sizeof(AllEvents));
		}
		else	/* has got core events as well */
		{
			events_return->onSubmit = events.onSubmit;
			events_return->onReset  = events.onReset;
			events_return->onFocus  = events.onFocus;
			events_return->onBlur   = events.onBlur;
			events_return->onSelect = events.onSelect;
			events_return->onChange = events.onChange;
		}
	}
	return(events_return);
}

AllEvents*
_XmHTMLCheckBodyEvents(XmHTMLWidget html, String attributes)
{
	AllEvents events;
	AllEvents *events_return = NULL;
	Boolean have_events = False;

	/* don't do a damn thing if we can't process any scripts or events */
	if(!html->html.event_proc || !html->html.event_callback)
		return(NULL);

	/* reset */
	(void)memset(&events, 0, sizeof(AllEvents));

	/* check core events */
	if((events_return = _XmHTMLCheckCoreEvents(html, attributes)) != NULL)
		have_events = True;

	/* process all possible body events */
	if((events.onLoad = checkEvent(html, XmCR_HTML_LOAD,
		attributes)) != NULL)
		have_events = True;
	if((events.onUnload= checkEvent(html, XmCR_HTML_UNLOAD,
		attributes)) != NULL)
		have_events = True;

	/* alloc & copy if we found any events */
	if(have_events)
	{
		/* no core events found */
		if(!events_return)
		{
			events_return = (AllEvents*)malloc(sizeof(AllEvents));
			events_return = memcpy(events_return, (const void*)&events,
								sizeof(AllEvents));
		}
		else	/* has got core events as well */
		{
			events_return->onLoad   = events.onLoad;
			events_return->onUnload = events.onUnload;
		}
	}
	return(events_return);
}

/*****
* Name:			_XmHTMLProcessEvent
* Return Type: 	void
* Description: 	calls the XmNeventCallback callback resource for the
*				given event.
* In: 
*	html:		XmHTMLWidget id;
*	event:		actual event data;
*	ht_event:	private event data;
* Returns:
*	nothing.
*****/
void
_XmHTMLProcessEvent(XmHTMLWidget html, XEvent *event, HTEvent *ht_event)
{
	XmHTMLEventCallbackStruct cbs;

	cbs.reason    = XmCR_HTML_EVENT;
	cbs.event     = event;
	cbs.type      = ht_event->type;
	cbs.data      = ht_event->data;

	XtCallCallbackList((Widget)html, html->html.event_callback, &cbs);
}

/*****
* Name:			_XmHTMLFreeEventDatabase
* Return Type: 	void
* Description: 	destroys all registered events. This routine is called
*				when the current document is being unloaded.
* In: 
*	old:		current XmHTMLWidget id;
*	html:		new XmHTMLWidget id;
* Returns:
*	nothing.
*****/
void
_XmHTMLFreeEventDatabase(XmHTMLWidget old, XmHTMLWidget html)
{
	int i;
	for(i = 0; i < old->html.nevents; i++)
	{
		XmHTMLEventCallbackStruct cbs;
		cbs.reason = XmCR_HTML_EVENTDESTROY;
		cbs.event  = NULL;
		cbs.type   = old->html.events[i].type;
		cbs.data   = old->html.events[i].data;
		XtCallCallbackList((Widget)old, old->html.event_callback, &cbs);
	}
	if(old->html.events)
		free(old->html.events);
	old->html.events = html->html.events = (HTEvent*)NULL;
	old->html.nevents = html->html.nevents = 0;
}
