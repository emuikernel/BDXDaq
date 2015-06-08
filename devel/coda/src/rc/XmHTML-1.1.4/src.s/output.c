#ifndef production
static char rcsId[]="$Header: /net/mizar/usr/local/source/coda_source/rc/XmHTML-1.1.4/XmHTML-1.1.4/src/output.c,v 1.1.1.1 1998/08/25 17:48:11 rwm Exp $";
#endif
/*****
* output.c : XmHTML Text output routines.
*
* This file Version	$Revision: 1.1.1.1 $
*
* Creation date:		Sun Sep  7 18:53:00 GMT+0100 1997
* Last modification: 	$Date: 1998/08/25 17:48:11 $
* By:					$Author: rwm $
* Current State:		$State: Exp $
*
* Author:				newt
*
* Copyright (C) 1994-1997 by Ripley Software Development 
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
* $Log: output.c,v $
* Revision 1.1.1.1  1998/08/25 17:48:11  rwm
*   Initial XmHTML-1.1.4 with RWM fixes.
*
* Revision 1.3  1998/04/27 07:01:33  newt
* tka stuff
*
* Revision 1.2  1998/04/04 06:28:16  newt
* XmHTML Beta 1.1.3
*
* Revision 1.1  1997/10/23 00:23:19  newt
* Initial Revision
*
*****/ 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Local includes */
#include "toolkit.h"
#include XmHTMLPrivateHeader
#include "XmHTMLfuncs.h"


/*** External Function Prototype Declarations ***/

/*** Public Variable Declarations ***/

/*** Private Datatype Declarations ****/

/*** Private Function Prototype Declarations ****/

/*** Private Variable Declarations ***/

/*****
* Name:			_XmHTMLTextCheckAndConvertPaperDef
* Return Type:	XmHTMLPaperSize
* Description:	verifies the given paper definition and converts it's type
*				to XmHTML_POINT.
* In:
*	html:		XmHTMLWidget id;
*	pin:		papersize to be checked & converted;
* Returns:
*	a ptr to the checked & converted paperdefinition.
*	Must be freed by caller.
*****/
XmHTMLPaperSize*
_XmHTMLTextCheckAndConvertPaperDef(XmHTMLWidget html, XmHTMLPaperSize *pdef,
	Byte type)
{
	static XmHTMLPaperSize *pout;
	float multiplier = 1.;

	/* verify margins */
	if(pdef->left_margin + pdef->right_margin >= pdef->width)
	{
		_XmHTMLWarning(__WFUNC__(html, "_XmHTMLTextCheckAndConvertPaperDef"),
			XMHTML_MSG_86, "horizontal", "width");
		return(NULL);
	}

	if(pdef->top_margin + pdef->bottom_margin >= pdef->height)
	{
		_XmHTMLWarning(__WFUNC__(html, "_XmHTMLTextCheckAndConvertPaperDef"),
			XMHTML_MSG_86, "vertical", "height");
		return(NULL);
	}

	/* copy in to out */
	pout = (XmHTMLPaperSize*)malloc(sizeof(XmHTMLPaperSize));

	memcpy((void*)pout, (const void*)pdef, sizeof(XmHTMLPaperSize));

	/* postscript does everything in points */
	if(type == XmHTMLTEXT_POSTSCRIPT)
	{
		switch(pdef->unit_type)
		{
			case XmHTML_CENTIMETER:		/* 1cm = 28.45pt */
				multiplier = 28.45;
				break;
			case XmHTML_MILLIMETER:		/* 1mm = 2.845 pt */
				multiplier = 2.845;
				break;
			case XmHTML_INCH:			/* 1in = 72.27 pt */
				multiplier = 72.27;
				break;
			case XmHTML_PICA:			/* 1pc = 12pt */
				multiplier = 12.;
				break;
			case XmHTML_CHAR:			/* 1char = 10.5625pt */
				multiplier = 10.5625;
				break;
			case XmHTML_POINT:			/* no conversion required */
				return(pout);
			default:					/* bad spec */
				_XmHTMLWarning(__WFUNC__(html,
					"_XmHTMLTextCheckAndConvertPaperDef"), XMHTML_MSG_87);
				free(pout);
				return(NULL);
		}
	}
	else	/* plain text does everything in chars */
	{
		switch(pdef->unit_type)
		{
			case XmHTML_CENTIMETER:		/* 1cm = 2.6934911 char */
				multiplier = 2.6934911;
				break;
			case XmHTML_MILLIMETER:		/* 1mm = 0.26934911 char */
				multiplier = 0.26934911;
				break;
			case XmHTML_INCH:			/* 1in = 6.8421302 char */
				multiplier = 6.8421302;
				break;
			case XmHTML_PICA:			/* 1pc = 1.1360947 char */
				multiplier = 1.1360947;
				break;
			case XmHTML_POINT:			/* 1pt = 0.094674556 char */
				multiplier = 0.094674556;
				break;
			case XmHTML_CHAR:			/* no conversion required */
				return(pout);
			default:					/* bad spec */
				_XmHTMLWarning(__WFUNC__(html,
					"_XmHTMLTextCheckAndConvertPaperDef"), XMHTML_MSG_87);
				free(pout);
				return(NULL);
		}
	}
	pout->unit_type     = XmHTML_POINT;
	pout->width         *= multiplier;
	pout->height        *= multiplier;
	pout->left_margin   *= multiplier;
	pout->right_margin  *= multiplier;
	pout->top_margin    *= multiplier;
	pout->bottom_margin *= multiplier;

	return(pout);
}

/*****
* Name:			_XmHTMLTextGetPlain
* Return Type: 	String
* Description: 	converts text between start & end into a plain ASCII document.
* In: 
*	html:		XmHTMLWidget id;
*	pdef:		papersize definition. unittype must be XmHTML_CHAR;
*	start:		start object;
*	end:		end object;
*	options:	unused;
* Returns:
*	a String with plain ASCII content. Must be freed by caller.
*****/
String
_XmHTMLTextGetPlain(XmHTMLWidget html, XmHTMLPaperSize *pdef, 
	XmHTMLObjectTableElement start, XmHTMLObjectTableElement end,
	Byte options)
{
	int x_pos, y_pos;
	XmHTMLObjectTable *elePtr;
	XmHTMLWord *words;
	String text = NULL, chPtr;
	int nchars = 0, n_words;
	int i, j, k;

	if(pdef->unit_type != XmHTML_CHAR)
	{
		_XmHTMLWarning(__WFUNC__(html, "_XmHTMLTextGetPlain"),
			XMHTML_MSG_88, "CHAR");
		return(NULL);
	}

	/* default start points */
	x_pos = pdef->left_margin;
	y_pos = pdef->top_margin;

	/*****
	* Count how many words we have and compose a text buffer containing
	* all text. We ignore words of type IMG and FORM.
	*****/
	for(elePtr = start; elePtr != end; elePtr = elePtr->next)
	{
		if(elePtr->object_type == OBJ_TEXT)
		{
			int word_len = 0;

			n_words = elePtr->n_words;
			words = elePtr->words;
			for(i = 0; i < n_words; i++)
			{
				if(words[i].type == OBJ_TEXT ||
					words[i].type == OBJ_BLOCK)
				{
					if(words[i].type == OBJ_BLOCK)
						word_len = 1;
					else
					{
						if(!(words[i].spacing & TEXT_SPACE_TRAIL) && 
							i+1 < n_words &&
							!(words[i+1].spacing & TEXT_SPACE_LEAD))
						{
							int k = i+1;
							word_len = words[i].len;
							while(k < n_words)
							{
								if(!(words[k].spacing & TEXT_SPACE_LEAD))
									word_len += words[k].len;
								/*****
								* see if this word has a trailing space and
								* the next a leading
								*****/
								if(!(words[k].spacing & TEXT_SPACE_TRAIL)
									 && k+1 < n_words &&
									!(words[k+1].spacing & TEXT_SPACE_LEAD))
									k++;
								else
									break;
							}
						}
						else
							word_len = words[i].len;
					}
					nchars += word_len + 1;	/* and a space */
				}
				else
				{
					/* images & form elements are considered a space */
					nchars += 1;
				}
			}
		}
		else /* non-text objects are converted to newlines */
			nchars += 1;
	}
	nchars++;	/* for terminator */

#ifdef DEBUG
	_XmHTMLDebug(18, ("text buffer creation, will use %ibytes\n.", nchars));
#endif

	if((text = (String)malloc(nchars * sizeof(char))) == NULL)
		return(NULL);

	/* okay, got it. Now fill the searchable table */
	j = 0;
	chPtr = text;

	for(elePtr = start; elePtr != end; elePtr = elePtr->next)
	{
		if(elePtr->object_type == OBJ_TEXT)
		{
			n_words = elePtr->n_words;
			words = elePtr->words;
			for(i = 0; i < n_words; i++)
			{
				if(words[i].type == OBJ_TEXT ||
					words[i].type == OBJ_BLOCK)
				{
					if(words[i].type == OBJ_BLOCK)
						*chPtr++ = '\n';
					else
					{
						if(!(words[i].spacing & TEXT_SPACE_TRAIL) && 
							i+1 < n_words &&
							!(words[i+1].spacing & TEXT_SPACE_LEAD))
						{
							k = i+1;
							while(k < n_words)
							{
								if(!(words[k].spacing & TEXT_SPACE_LEAD))
								{
									/* copy word */
									memcpy(chPtr, words[k].word,
										words[k].len);
									chPtr += words[k].len;
								}
								if(!(words[k].spacing & TEXT_SPACE_TRAIL)
									 && k+1 < n_words &&
									!(words[k+1].spacing & TEXT_SPACE_LEAD))
									k++;
								else
									break;
							}
						}
						else
						{
							/* copy word */
							memcpy(chPtr, words[i].word, words[i].len);
							chPtr += words[i].len;
						}
						/* add a space */
						*chPtr++ = ' ';
					}
				}
				else
				{
					/* images & form elements are considered as a space */
					*chPtr++ = ' ';
				}
			}
		}
		else /* non-text objects are converted to newlines */
			*chPtr++ = '\n';
	}
	/* terminate text */
	*chPtr++ = '\0';
	return(text);
}

/*****
* Name:			_XmHTMLTextGetFormatted
* Return Type: 	String
* Description: 	converts text between start & end into a somewhat formatted
*				ASCII document.
* In: 
*	html:		XmHTMLWidget id;
*	pdef:		papersize definition. unittype must be XmHTML_CHAR;
*	start:		start object;
*	end:		end object;
*	options:	unused;
* Returns:
*	a String with formatted ASCII content. Must be freed by caller.
*****/
String
_XmHTMLTextGetFormatted(XmHTMLWidget html, XmHTMLPaperSize *pdef, 
	XmHTMLObjectTableElement start, XmHTMLObjectTableElement end,
	Byte options)
{
	if(pdef->unit_type != XmHTML_CHAR)
	{
		_XmHTMLWarning(__WFUNC__(html, "_XmHTMLTextGetFormatted"),
			XMHTML_MSG_88, "CHAR");
		return(NULL);
	}
	return(NULL);
}

/*****
* Name:			_XmHTMLTextGetPS
* Return Type: 	String
* Description: 	converts text between start & end into a postscript document
* In: 
*	html:		XmHTMLWidget id;
*	pdef:		papersize definition. unittype must be XmHTML_POINT;
*	start:		start object;
*	end:		end object;
*	options:	postscript options (font to use, header, footer cmds);
* Returns:
*	a String with postscript commands. Must be freed by caller.
*****/
String
_XmHTMLTextGetPS(XmHTMLWidget html, XmHTMLPaperSize *pdef, 
	XmHTMLObjectTableElement start, XmHTMLObjectTableElement end,
	Byte options)
{
	if(pdef->unit_type != XmHTML_POINT)
	{
		_XmHTMLWarning(__WFUNC__(html, "_XmHTMLTextGetPS"),
			XMHTML_MSG_88, "POINT");
		return(NULL);
	}
	return(NULL);
}




