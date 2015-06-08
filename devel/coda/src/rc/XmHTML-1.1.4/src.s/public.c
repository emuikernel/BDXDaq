#ifndef lint
static char rcsId[]="$Header: /net/mizar/usr/local/source/coda_source/rc/XmHTML-1.1.4/XmHTML-1.1.4/src/public.c,v 1.1.1.1 1998/08/25 17:48:15 rwm Exp $";
#endif
/*****
* public.c : XmHTML public routines that do not depend on X Intrinsics
*			 or Motif.
*
* This file Version	$Revision: 1.1.1.1 $
*
* Creation date:		Tue Apr 14 15:36:37 GMT+0100 1998
* Last modification: 	$Date: 1998/08/25 17:48:15 $
* By:					$Author: rwm $
* Current State:		$State: Exp $
*
* Author:				newt
*
* Copyright (C) 1994-1998 by Ripley Software Development 
* All Rights Reserved
*
* This file is part of the XmHTML Widget Library.
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
* $Log: public.c,v $
* Revision 1.1.1.1  1998/08/25 17:48:15  rwm
*   Initial XmHTML-1.1.4 with RWM fixes.
*
* Revision 1.1  1998/04/27 06:54:17  newt
* Initial Revision
*
*****/ 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <X11/IntrinsicP.h>	/* Fast macros */
#include <Xm/XmP.h>			/* Private motif funcs. */
#include <Xm/DrawP.h>
#include <Xm/XmStrDefs.h>	/* For motif XmN macros */

/* Our private header files */
#include "toolkit.h"
#include XmHTMLPrivateHeader
#include "XmHTMLfuncs.h"

/*** External Function Prototype Declarations ***/

/*** Public Variable Declarations ***/

/*** Private Datatype Declarations ****/

/*** Private Function Prototype Declarations ****/

/*** Private Variable Declarations ***/

/*****
* Name: 		XmHTMLAnchorGetId
* Return Type: 	int
* Description: 	returns the internal id of an anchor
* In: 
*	w:			XmHTMLWidget
*	anchor:		anchor to locate
* Returns:
*	the id upon success, -1 if not found.
*****/
int
XmHTMLAnchorGetId(WIDGET(w), String anchor)
{
	XmHTMLObjectTableElement anchor_data = NULL;

	/* sanity check */
	if(!w || !XmIsHTML(w))
	{
		_XmHTMLBadParent(w, "AnchorGetId");
		return(-1);
	}

	if((anchor_data = _XmHTMLGetAnchorByName((XmHTMLWidget)w, anchor)) != NULL)
		return(anchor_data->id);
	else /* not found */
		return(-1);
}

/*****
* Name: 		XmHTMLScrollToAnchorById
* Return Type: 	void
* Description: 	moves the text with the current anchor on top.
* In: 
*	w:			XmHTMLWidget
*	anchor_id:	internal anchor id to scroll to.
* Returns:
*	nothing.
*****/
void
XmHTMLAnchorScrollToId(WIDGET(w), int anchor_id)
{
	HTMLW(html);
	XmHTMLObjectTableElement anchor_data = NULL;	

	/* sanity check */
	if(!w || !XmIsHTML(w) || anchor_id < 0)
	{
		String func = "AnchorScrollToId";
		if(anchor_id < 0)
			_XmHTMLWarning(__WFUNC__(w, func), XMHTML_MSG_21,
				"Invalid id", func);
		else
			_XmHTMLBadParent(w, func);
		return;
	}

	html = (XmHTMLWidget)w;

	/* only scroll when we have a vertical scrollbar */
	/* fix 10/22/97-01, kdh */
	if((anchor_data = _XmHTMLGetAnchorByValue(html, anchor_id)) != NULL &&
		HTML_ATTR(needs_vsb))
	{
		int value;

		_XmHTMLDebug(1, ("XmHTML.c: XmHTMLAnchorScrollToId, "
			"calling _XmHTMLMoveToPos\n"));

		value = anchor_data->y - anchor_data->height;

		/* fix 01/30/97-04, kdh */
		_XmHTMLAdjustVerticalScrollValue(HTML_ATTR(vsb), &value);

		_XmHTMLMoveToPos(HTML_ATTR(vsb), html, value);
	}
}

/*****
* Name: 		XmHTMLAnchorScrollToName
* Return Type: 	void
* Description: 	moves the text with the current anchor on top.
* In: 
*	w:			XmHTMLWidget
*	anchor:		anchor to scroll to.
* Returns:
*	nothing.
*****/
void
XmHTMLAnchorScrollToName(WIDGET(w), String anchor)
{
	HTMLW(html);
	XmHTMLObjectTableElement anchor_data = NULL;

	/* sanity check */
	if(!w || !XmIsHTML(w))
	{
		_XmHTMLBadParent(w, "AnchorScrollToName");
		return;
	}

	html = (XmHTMLWidget)w;

	/* only scroll when we have a vertical scrollbar */
	/* fix 10/22/97-01, kdh */
	if((anchor_data = _XmHTMLGetAnchorByName(html, anchor)) != NULL &&
		HTML_ATTR(needs_vsb))
	{
		int value;

		_XmHTMLDebug(1, ("XmHTML.c: XmHTMLAnchorScrollToName, "
			"calling _XmHTMLMoveToPos\n"));

		value = anchor_data->y - anchor_data->height;

		/* fix 01/30/97-04, kdh */
		_XmHTMLAdjustVerticalScrollValue(HTML_ATTR(vsb), &value);

		_XmHTMLMoveToPos(HTML_ATTR(vsb), html, value);
	}
	return;
}

/*****
* Name: 		XmHTMLTextScrollToLine
* Return Type: 	void
* Description: 	scrolls the widget to the given line number.
* In: 
*	w:			widget to scroll
*	line:		line number to scroll to.
* Returns:
*	nothing.
*****/
void
XmHTMLTextScrollToLine(WIDGET(w), int line)
{
	/* sanity check */
	if(!w || !XmIsHTML(w))
	{
		_XmHTMLBadParent(w, "AnchorScrollToLine");
		return;
	}

	if(line == ((XmHTMLWidget)w)->html.top_line)
		return;

	/* scroll to the requested line */
	_XmHTMLScrollToLine((XmHTMLWidget)w, line);
}

/*****
* Name: 		XmHTMLTextGetSource
* Return Type: 	String
* Description: 	returns a copy of the original, unmodified document.
* In: 
*	w:			XmHTMLWidget in question
* Returns:
*	a *pointer* to the original text, or NULL when w isn't a subclass of XmHTML
*	or there wasn't a current document.
*****/
String
XmHTMLTextGetSource(WIDGET(w))
{
	if(!w || !XmIsHTML(w))
	{
		_XmHTMLBadParent(w, "TextGetSource");
		return(NULL);
	}

	return(((XmHTMLWidget)w)->html.source);
}

/*****
* Name: 		XmHTMLTextGetString
* Return Type: 	String
* Description: 	composes a text buffer consisting of the parser output.
*				This return buffer is not necessarely equal to the original
*				document as the document verification and repair routines
*				are capable of modifying the original rather heavily.
* In: 
*	w:			XmHTMLWidget in question
* Returns:
*	An allocated buffer containing the composed text upon success, NULL on
*	failure.
* Note:
*	The return value from this function must be freed by the caller.
*	Typical use of this function is to set this buffer into the widget when
*	the parser failed to verify the document as it might well be that a next
*	parser pass on the original document does produce a HTML3.2 conforming
*	and verified document.
*****/
String
XmHTMLTextGetString(WIDGET(w))
{
	if(!w || !XmIsHTML(w))
	{
		_XmHTMLBadParent(w, "TextGetString");
		return(NULL);
	}

	return(_XmHTMLTextGetString(((XmHTMLWidget)w)->html.elements));
}

/*****
* Name: 		XmHTMLGetVersion
* Return Type: 	int
* Description: 	returns the version number of XmHTML
* In: 
*	nothing
* Returns:
*	version number of this library.
*****/
int 
XmHTMLGetVersion(void)
{
	return(XmHTMLVersion);
}

/*****
* Name: 		XmHTMLGetTitle
* Return Type: 	String
* Description: 	returns the value of the <title></title> element
* In: 
*	w:			XmHTMLWidget in question
* Returns:
*	value of the title upon success, NULL on failure.
*****/
String 
XmHTMLGetTitle(WIDGET(w))
{
	HTMLW(html);
	XmHTMLObject *tmp;
	static String ret_val;
	String start, end;

	/* sanity check */
	if(!w || !XmIsHTML(w))
	{
		_XmHTMLBadParent(w, "GetTitle");
		return(NULL);
	}

	html = (XmHTMLWidget)w;

	for(tmp = HTML_ATTR(elements);
		tmp != NULL && tmp->id != HT_TITLE && tmp->id != HT_BODY;
		tmp = tmp->next);

	/* sanity check */
	if(!tmp || !tmp->next || tmp->id == HT_BODY)
		return(NULL);

	/* ok, we have reached the title element, pick up the text */
	tmp = tmp->next;

	/* another sanity check */
	if(!tmp->element)
		return(NULL);

	/* skip leading... */
	for(start = tmp->element; *start != '\0' && isspace(*start); start++);

	/* ...and trailing whitespace */
	for(end = &start[strlen(start)-1]; *end != '\0' && isspace(*end);
		end--);

	/* always backs up one to many */
	end++;

	/* sanity */
	if(*start == '\0' || (end - start) <= 0)
		return(NULL);

  	/* duplicate the title */
	ret_val = my_strndup(start, end - start);

	/* expand escape sequences */
	_XmHTMLExpandEscapes(ret_val, HTML_ATTR(bad_html_warnings));

	/* and return to caller */
	return(ret_val);
}

/*****
* Name: 		XmHTMLTextSetString
* Return Type: 	void
* Description: 	sets the given text into the given HTML widget
* In: 
*	w:			XmHTMLWidget in question
*	value:		text to set
* Returns:
*	clears any previous text and sets the new text.
*****/
void
XmHTMLTextSetString(WIDGET(w), String text)
{
	/* sanity check */
	if(!w || !XmIsHTML(w))
	{
		_XmHTMLBadParent(w, "TextSetString");
		return;
	}
	/* test required since strlen(NULL) doesn't exactly work... */
	XmHTMLTextSetStringWithLength(w, text, text ? strlen(text) : 0);
}

/*****
* Name: 		XmHTMLTextSetStringWithLength
* Return Type: 	void
* Description: 	sets the given text into the given HTML widget
* In: 
*	w:			XmHTMLWidget in question
*	value:		text to set. Doesn't have to be NULL terminated
*	len:		size of input string.
* Returns:
*	clears any previous text and sets the new text.
*****/
void
XmHTMLTextSetStringWithLength(WIDGET(w), String text, size_t len)
{
	HTMLW(html);
	Boolean had_hsb, had_vsb;
	ToolkitAbstraction *tka = NULL;

	/* sanity check */
	if(!w || !XmIsHTML(w))
	{
		_XmHTMLBadParent(w, "TextSetStringWithLength");
		return;
	}

	_XmHTMLDebug(1, ("XmHTML.c: XmHTMLTextSetStringWithLength, start\n"));

	html = (XmHTMLWidget)w;
	tka = HTML_ATTR(tka);

	/* almost impossible */
	if(HTML_ATTR(value) == text)
		return;

	/* check if the new value is different from the current source */
	if(text && HTML_ATTR(source) && len &&
		!(strncmp(HTML_ATTR(source), text, len)))
		return;

	/* check the current state of the scrollbars */
	had_hsb = tka->IsManaged(HTML_ATTR(hsb));
	had_vsb = tka->IsManaged(HTML_ATTR(vsb));

	/* First kill any outstanding PLC's */
	_XmHTMLKillPLCCycler(html);

	/* release event database */
	_XmHTMLFreeEventDatabase(html, html);

	/* now destroy any forms */
	_XmHTMLFreeForm(html, HTML_ATTR(form_data));
	HTML_ATTR(form_data) = (XmHTMLFormData*)NULL;

	/* clear the current display area. Prevents color flashing etc. */
	if(HTML_ATTR(gc) != NULL)
	{
		tka->ClearArea(tka->dpy, tka->win, 0, 0,
			CORE_ATTR(width), CORE_ATTR(height), False);
	}

	/* clear current source */
	if(HTML_ATTR(source))
	{
		free(HTML_ATTR(source));
		HTML_ATTR(source) = NULL;
		HTML_ATTR(value)  = NULL;
	}

	/* set new source text */
	if(text && len)
	{
		/* strndup returns a NULL terminated string */
		HTML_ATTR(source) = my_strndup(text, len);
		HTML_ATTR(value) = HTML_ATTR(source);
	}

	/* destroy any existing frames */
	if(HTML_ATTR(nframes))
		_XmHTMLDestroyFrames(html, HTML_ATTR(nframes));

	/* free all non-persistent resources and images */
	_XmHTMLFreeExpendableResources(html, True);

	/* reset some important vars */
	_XmHTMLReset(html, True);

	/*****
	* Create XCC, it's probably been destroyed by now (unless a fixed
	* palette is being used.
	*****/
	_XmHTMLCheckXCC(html);

	/* input is always complete */
	HTML_ATTR(input_complete) = True;

	/* Parse the raw HTML text */
	HTML_ATTR(elements) = _XmHTMLparseHTML(html, HTML_ATTR(elements), 
		HTML_ATTR(source), html);

	/* Trigger link callback */
	if(HTML_ATTR(link_callback))
		_XmHTMLLinkCallback(html);

	/* reset topline */
	HTML_ATTR(top_line) = 0;

	/* check for frames */
	HTML_ATTR(nframes) = _XmHTMLCheckForFrames(html, HTML_ATTR(elements));

	/* set appropriate background color */
	XtVaSetValues(HTML_ATTR(work_area), 
		XmNbackground, HTML_ATTR(body_bg), NULL);

	/* get new values for top, bottom & highlight */
	_XmHTMLRecomputeColors(html);

	/* create frames */
	if(!_XmHTMLCreateFrames(NULL, html))
	{
		HTML_ATTR(frames) = NULL;
		HTML_ATTR(nframes) = 0;
		/* keep current frame setting */
	}

	/* do initial markup */
	_XmHTMLformatObjects(html, html);

	/* check for delayed external imagemaps */
	_XmHTMLCheckImagemaps(html);

	/* compute new screen layout */
	_XmHTMLLayout(html);

	/* and clear the display, causing an Expose event */
	if(HTML_ATTR(gc) != NULL)
		_XmHTMLClearArea(html, 0, 0, CORE_ATTR(width), CORE_ATTR(height));

	/* and start up the PLC cycler */
	HTML_ATTR(plc_suspended) = False;
	_XmHTMLPLCCycler((XtPointer)html, None);
}

/*****
* Name: 		XmHTMLXYToInfo
* Return Type: 	XmHTMLInfoStructure*
* Description: 	Retrieves the contents of an image and/or anchor at the
*				given cursor position.
* In: 
*	w:			XmHTMLWidget id;
*	x:			x-location of pointer, relative to left side of the workArea
*	y:			y-location of pointer, relative to top side of the workArea
* Returns:
*	A filled XmHTMLInfoStructure when the pointer was pressed on an image
*	and/or anchor. NULL if not.
* Note:
*	The return value, nor one of its members may be freed by the caller.
*****/
XmHTMLInfoPtr
XmHTMLXYToInfo(WIDGET(w), int x, int y)
{
	static XmHTMLInfoStructure cbs;
	static XmHTMLImage *image;
	static XmHTMLAnchorCallbackStruct anchor_cbs;
	static XmImageInfo info;
	long line = -1;
	XmHTMLAnchor *anchor;
	XmHTMLWord *anchor_word;
	XmHTMLImage anchor_img;
	HTMLW(html);

	/* sanity check */
	if(!w || !XmIsHTML(w))
	{
		_XmHTMLBadParent(w, "XYToInfo");
		return(NULL);
	}

	html = (XmHTMLWidget)w;

	/* default fields */
	cbs.x      = x - CORE_ATTR(x);
	cbs.y      = y - CORE_ATTR(y);
	cbs.is_map = XmMAP_NONE;
	cbs.image  = NULL;
	cbs.anchor = NULL;
	line = -1;

	/* pick up a possible anchor or imagemap location */
	anchor = NULL;
			
	if((anchor_word = _XmHTMLGetAnchor(html, x, y, &anchor_img)) == NULL)
		anchor = _XmHTMLGetImageAnchor(html, x, y, &anchor_img);

	/* no regular anchor, see if it's an imagemap */
	if(anchor == NULL && anchor_word)
		anchor = anchor_word->owner->anchor;

	/*
	* Final check: if this anchor is a form component it can't be followed 
	* as this is an internal-only anchor.
	*/
	if(anchor && anchor->url_type == ANCHOR_FORM_IMAGE)
		anchor = NULL;

	/* check if we have an anchor */
	if(anchor != NULL)
	{
		/* set to zero */
		(void)memset(&anchor_cbs, 0, sizeof(XmHTMLAnchorCallbackStruct));

		/* initialize callback fields */
		anchor_cbs.reason   = XmCR_ACTIVATE;
		anchor_cbs.event    = NULL;
		anchor_cbs.url_type = anchor->url_type;
		anchor_cbs.line     = anchor->line;
		anchor_cbs.href     = anchor->href;
		anchor_cbs.target   = anchor->target;
		anchor_cbs.rel      = anchor->rel;
		anchor_cbs.rev      = anchor->rev;
		anchor_cbs.title    = anchor->title;
		anchor_cbs.doit     = False;
		anchor_cbs.visited  = anchor->visited;
		cbs.anchor = &anchor_cbs;
		line       = anchor->line;
	}

	/* check if we have an image.*/
	if((image = _XmHTMLOnImage(html, x, y)) != NULL)
	{
		/* set map type */
		cbs.is_map = (image->map_type != XmMAP_NONE);

		if(image->html_image != NULL)
		{
			/* no image info if this image is being loaded progressively */
			if(!ImageInfoProgressive(image->html_image))
			{
				/* use real url but link all other members */
				info.url          = image->url;
				info.data         = image->html_image->data;
				info.clip         = image->html_image->clip;
				info.width        = image->html_image->width;
				info.height       = image->html_image->height;
				info.reds         = image->html_image->reds;
				info.greens       = image->html_image->greens;
				info.blues        = image->html_image->blues;
				info.bg           = image->html_image->bg;
				info.ncolors      = image->html_image->ncolors;
				info.options      = image->html_image->options;
				info.type         = image->html_image->type;
				info.depth        = image->html_image->depth;
				info.colorspace   = image->html_image->colorspace;
				info.transparency = image->html_image->transparency;
				info.swidth       = image->html_image->swidth;
				info.sheight      = image->html_image->sheight;
				info.scolors      = image->html_image->scolors;
				info.alpha        = image->html_image->alpha;
				info.fg_gamma     = image->html_image->fg_gamma;
				info.x            = image->html_image->x;
				info.y            = image->html_image->y;
				info.loop_count   = image->html_image->loop_count;
				info.dispose      = image->html_image->dispose;
				info.timeout      = image->html_image->timeout;
				info.nframes      = image->html_image->nframes;
				info.frame        = image->html_image->frame;
				info.user_data    = image->html_image->user_data;
				/* set it */
				cbs.image = &info;
			}
		}
		else
		{
			/* XmImageInfo has been freed, construct one */
			/* set to zero */
			memset(&info, 0, sizeof(XmImageInfo));
			/* fill in the fields we know */
			info.url     = image->url;
			info.type    = IMAGE_UNKNOWN;
			info.width   = image->swidth;
			info.height  = image->sheight;
			info.swidth  = image->width;
			info.sheight = image->height;
			info.ncolors = image->npixels;
			info.nframes = image->nframes;
			/* set it */
			cbs.image     = &info;
		}
		if(line == -1)
			line = (image->owner ? image->owner->line : -1);
	}
	/* no line number yet, get one */
	if(line == -1)
		cbs.line = _XmHTMLVerticalPosToLine(html, y + HTML_ATTR(scroll_y));
	else
		cbs.line = line;
	return(&cbs);
}

/*****
* Name:			XmHTMLTextGetFormatted
* Return Type: 	String
* Description: 	returns a formatted copy of the current document.
* In: 
*	w:			XmHTMLWidget id;
*	papertype:	type of paper to use (any of the XmHTMLTEXT_PAPERSIZE enums);
*	papersize:	size of paper for custom stuff, or default overrides;
*	type:		type of output wanted, plain, formatted or PS;
*	PSoptions:	options to use when creating postscript output.
* Returns:
*	a string which needs to be freed by the caller.
*****/
String
XmHTMLTextGetFormatted(WIDGET(w), unsigned char papertype,
	XmHTMLPaperSize *paperdef, unsigned char type, unsigned char PSoptions)
{
	HTMLW(html);
	XmHTMLPaperSize *pdef, pbase;
	String ret_val = NULL;

	/* sanity check */
	if(!w || !XmIsHTML(w))
	{
		_XmHTMLBadParent(w, "TextGetFormatted");
		return(NULL);
	}

	/*****
	* Check args: we only allow a papersize of XmHTML_NONE for plain and
	* formatted output. PS requires a papersize.
	*****/
	if(paperdef == NULL && type == XmHTMLTEXT_POSTSCRIPT)
	{
		_XmHTMLWarning(__WFUNC__(w, "XmHTMLTextGetFormatted"), XMHTML_MSG_22);
		return(NULL);
	}
	/* custom papersize requires a paper definition. */
	if(papertype == XmHTMLTEXT_PAPERSIZE_CUSTOM && paperdef == NULL)
	{
		_XmHTMLWarning(__WFUNC__(w, "XmHTMLTextGetFormatted"), XMHTML_MSG_23);
		return(NULL);
	}

	/* widget ptr */
	html = (XmHTMLWidget)w;

	/*****
	* get appropriate papersize definitions if not given.
	*****/
	if(papertype != XmHTMLTEXT_PAPERSIZE_CUSTOM && paperdef == NULL)
	{
		/* formatting routines use point size */
		if(papertype == XmHTMLTEXT_PAPERSIZE_A4)
		{
			pbase.unit_type     = XmHTML_POINT;
			pbase.paper_type    = XmHTMLTEXT_PAPERSIZE_A4;
			pbase.width         = 845;	/* 297mm */
			pbase.height        = 597;	/* 210mm */
			pbase.left_margin   = 57;	/* 20mm  */
			pbase.right_margin  = 57;
			pbase.top_margin    = 57;
			pbase.bottom_margin = 57;
		}
		else 	/* XmHTMLTEXT_PAPERSIZE_LETTER */
		{
			pbase.unit_type     = XmHTML_POINT;
			pbase.paper_type    = XmHTMLTEXT_PAPERSIZE_LETTER;
			pbase.width         = 795;	/* 11in  */
			pbase.height        = 614;	/* 8.5in */
			pbase.left_margin   = 65;	/* 0.9in */
			pbase.right_margin  = 65;
			pbase.top_margin    = 65;
			pbase.bottom_margin = 51;	/* 0.7in */
		}
		/* convert to correct output type */
		pdef = _XmHTMLTextCheckAndConvertPaperDef(html, paperdef, type);
	}
	else	/* check validity of paper definition and convert to correct type */
		pdef = _XmHTMLTextCheckAndConvertPaperDef(html, paperdef, type);

	if(pdef == NULL)
		return(NULL);

	switch(type)
	{
		case XmHTMLTEXT_PLAIN:
			ret_val = _XmHTMLTextGetPlain(html, pdef, HTML_ATTR(formatted),
				NULL, 0);
			break;
		case XmHTMLTEXT_FORMATTED:
			ret_val = _XmHTMLTextGetFormatted(html, pdef, HTML_ATTR(formatted),
				NULL, 0);
			break;
		case XmHTMLTEXT_POSTSCRIPT:
			ret_val = _XmHTMLTextGetPS(html, pdef, HTML_ATTR(formatted),
				NULL, PSoptions);
			break;
		default:
			_XmHTMLWarning(__WFUNC__(w, "XmHTMLTextGetFormatted"),
				XMHTML_MSG_24);
	}
	/* no longer needed */
	free(pdef);

	return(ret_val);
}

/*****
* Name:			XmHTMLGetDocumentInfo
* Return Type: 	XmHTMLDocumentInfo
* Description: 	returns a list of all images and anchors found in the
*				currently displayed page.
* In: 
*	w:			XmHTMLWidget id.
* Returns:
*	a new XmHTMLDocumentInfo structure on success or NULL on failure.
*****/
XmHTMLDocumentInfo*
XmHTMLGetDocumentInfo(WIDGET(w))
{
	HTMLW(html);
	XmHTMLImage *image;
	XmHTMLAnchor *anchor;
	int ssize = 0;
	Byte *chPtr;
	static XmHTMLDocumentInfo *doc_info = NULL;

	/* sanity check */
	if(!w || !XmIsHTML(w))
	{
		_XmHTMLBadParent(w, "GetDocumentInfo");
		return(NULL);
	}
	html = (XmHTMLWidget)w;

	doc_info = (XmHTMLDocumentInfo*)calloc(1, sizeof(XmHTMLDocumentInfo));

	/* background image ? */
	if((image = HTML_ATTR(images)) != NULL && ImageIsBackground(image))
	{
		doc_info->bg_image = strdup(image->url);
		image = image->next;
	}
	if(image)
	{
		/* see how many images we have */
		ssize = 0;
		for(; image != NULL; image = image->next)
			ssize += (strlen(image->url) + 1);
		ssize++;	/* terminating \0 */

		/* allocate */
		doc_info->images = (String)calloc(ssize, sizeof(Byte));

		image = ImageIsBackground(HTML_ATTR(images)) ?
					HTML_ATTR(images->next) : HTML_ATTR(images);

		chPtr = doc_info->images;
		while(image != NULL)
		{
			(void)memcpy(chPtr, image->url, strlen(image->url));
			chPtr += (strlen(image->url) + 1);
			image = image->next;
		}
	}

	/* see how many anchors we have */
	ssize = 0;
	for(anchor = HTML_ATTR(anchor_data); anchor != NULL; anchor = anchor->next)
	{
		if(*anchor->href != '\0')
			ssize += (strlen(anchor->href) + 1);
	}
	ssize++;	/* terminating \0 */

	doc_info->anchors = (Byte*)calloc(ssize, sizeof(Byte));

	chPtr = doc_info->anchors;
	for(anchor = HTML_ATTR(anchor_data); anchor != NULL; anchor = anchor->next)
	{
		if(*anchor->href != '\0')
		{
			(void)memcpy(chPtr, anchor->href, strlen(anchor->href));
			chPtr += (strlen(anchor->href) + 1);
		}
	}
	return(doc_info);
}

/*****
* Name:			XmHTMLFreeDocumentInfo
* Return Type: 	void
* Description: 	frees a XmHTMLDocumentInfo structure.
* In: 
*	doc_info:	structure to be freed.
* Returns:
*	nothing.
*****/
void
XmHTMLFreeDocumentInfo(XmHTMLDocumentInfo *doc_info)
{
	if(doc_info == NULL)
		return;

	if(doc_info->bg_image)
		free(doc_info->bg_image);
	if(doc_info->images)
		free(doc_info->images);
	if(doc_info->anchors)
		free(doc_info->anchors);
	free(doc_info);
}
