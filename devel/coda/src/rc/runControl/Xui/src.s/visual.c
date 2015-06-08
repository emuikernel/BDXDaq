#ifndef lint
static char rcsId[]="$Header: /net/mizar/usr/local/source/coda_source/runControl/Xui/visual.c,v 1.4 1999/10/25 17:31:43 abbottd Exp $";
#endif
/*****
* visual.c : visual & colormap support code for the XmHTML examples.
*
* This file Version	$Revision: 1.4 $
*
* Creation date:		Thu May  1 00:17:12 GMT+0100 1997
* Last modification: 	$Date: 1999/10/25 17:31:43 $
* By:					$Author: abbottd $
* Current State:		$State: Exp $
*
* Author:				John L. Cwikla
*						X Programmer
*						Wolfram Research Inc.
*						cwikla@wri.com
*
* Copyright (C) 1996 by John C. Cwikla
* Copyright (C) 1997 by Ripley Software Development 
* All Rights Reserved
*
* This file is part of the XmHTML Widget Library.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU [Library] General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU [Library] General Public
* License along with this library; if not, write to the Free
* Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*****/
/*****
* ChangeLog 
* $Log: visual.c,v $
* Revision 1.4  1999/10/25 17:31:43  abbottd
* Remove Linux WCHAR_T define
*
* Revision 1.3  1998/11/06 20:07:04  timmer
* Linux port wchar_t stuff
*
* Revision 1.2  1998/05/28 17:47:10  heyes
* new GUI look
*
* Revision 1.1  1998/04/09 17:52:00  heyes
* add
*
* Revision 1.1  1997/05/28 02:01:34  newt
* Initial Revision
*
*****/ 

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <stdlib.h>

/*** External Function Prototype Declarations ***/

/*** Public Variable Declarations ***/

/*** Private Datatype Declarations ****/
#define XmNvisualID				"visualID"
#define XmNapplicationDepth		"applicationDepth"
#define XmNvisualClass			"visualClass"
#define XmNusePrivateColormap	"usePrivateColormap"

#define XmCVisualID				"VisualID"
#define XmCApplicationDepth		"ApplicationDepth"
#define XmCVisualClass			"VisualClass"
#define XmCUsePrivateColormap	"UsePrivateColormap"

/*** Private Function Prototype Declarations ****/

/*** Private Variable Declarations ***/
typedef struct _OurResourceStruct
{
	int visualID;
	int applicationDepth;
	int visualClass;
	Boolean usePrivateColormap;
}OurResourceStruct, *OurResourcePtr;

OurResourceStruct ourResources;

#define UNDEFINED_DEFAULT -1
#define TheOffset(a) XtOffset(OurResourcePtr, a)

static XtResource AppResources[] =
{
	{
		XmNvisualID,
		XmCVisualID, XtRInt, sizeof(int),
		TheOffset(visualID),
		XtRImmediate, (XtPointer)UNDEFINED_DEFAULT
	},
	{
		XmNapplicationDepth,
		XmCApplicationDepth, XtRInt, sizeof(int),
		TheOffset(applicationDepth),
		XtRImmediate, (XtPointer)UNDEFINED_DEFAULT
	},
	{
		XmNvisualClass,
		XmCVisualClass, XtRInt, sizeof(int),
		TheOffset(visualClass),
		XtRImmediate, (XtPointer)UNDEFINED_DEFAULT
	},
	{
		XmNusePrivateColormap,
		XmCUsePrivateColormap, XtRBoolean, sizeof(Boolean),
		TheOffset(usePrivateColormap),
		XtRImmediate, (XtPointer)FALSE
	},
};

/*****
* Name:			getStartupVisual
* Return Type:	int
* Description:	pick visual and colormap that are in balance.
* In:
*	shell:		widget for which to pick visual and colormap;
*	visual:		visual picked, updated upon return;
*	colormap:	created colormap (default or privated), updated upon return;
*	depth:		required depth.
* Returns:
*	True when args have been updated, false if not.
* Note:
*	This routine comes from an article of John Cwikla that appeared in the
*	X Advisor Journal, ``Beyond the Default Visual'', dated june 1995. Read it
*	to get the full story and why the resources. You can find this article at:
*	http://www.unx.com/DD/advisor/docs/jun95/jun95.cwikla1.shtml
*****/
int
getStartupVisual(Widget shell, Visual **visual, int *depth,
	Colormap *colormap)
{
	Display *display;
	int success = FALSE, screen;
	int theVisualClass;
	XVisualInfo theVisualInfo;
	static Visual *theVisual;
	static Colormap theColormap;
	static int theApplicationDepth;

	display = XtDisplay(shell);

	XtGetApplicationResources(shell, &ourResources, AppResources,
		XtNumber(AppResources), NULL, 0);

	ourResources.usePrivateColormap = 1;
	ourResources.applicationDepth = 16;
	ourResources.visualClass = 1;
	/* if no resources have been set, just return */
	if(ourResources.visualID == UNDEFINED_DEFAULT &&
		ourResources.applicationDepth == UNDEFINED_DEFAULT &&
		ourResources.visualClass == UNDEFINED_DEFAULT &&
		ourResources.usePrivateColormap == FALSE)
	{
		return(FALSE);
	}
	printf("ho ho ho...\n");
	if(ourResources.visualID != UNDEFINED_DEFAULT)
	{
		XVisualInfo vtemp, *vinfos;
		int vitems;
		vtemp.visualid = ourResources.visualID;
		vinfos = XGetVisualInfo(display, VisualIDMask, &vtemp, &vitems);

		if(vinfos != NULL)
		{
			/* Better only be one match! */
			theVisual = vinfos[0].visual;
			theApplicationDepth = vinfos[0].depth;
			theVisualClass = vinfos[0].class;

			XFree(vinfos);
			success = TRUE;
		}
	}
	screen = DefaultScreen(display);

	if(!success)
	{
		/* Step 2 */
		if((ourResources.applicationDepth == UNDEFINED_DEFAULT) &&
			(ourResources.visualClass == UNDEFINED_DEFAULT))
		{
			theVisual = DefaultVisual(display, screen);
			theApplicationDepth = DefaultDepth(display, screen);
			theVisualClass = theVisual->class;
		}
		else
		{
			/* Step 3 */
			if(ourResources.applicationDepth == UNDEFINED_DEFAULT)
				theApplicationDepth = DefaultDepth(display, screen);
			else
				theApplicationDepth = ourResources.applicationDepth;
			if(ourResources.visualClass == UNDEFINED_DEFAULT)
				theVisualClass = DefaultVisual(display, screen)->class;
			else
				theVisualClass = ourResources.visualClass;

			if(XMatchVisualInfo(display, screen, theApplicationDepth, 
				theVisualClass, &theVisualInfo) != 0)
			{
				theVisual = theVisualInfo.visual;
				theApplicationDepth = theVisualInfo.depth;
				theVisualClass = theVisualInfo.class;
			}
			else
			{
				/* Step 4 */
				XVisualInfo visTemplate;
				XVisualInfo *visReturn;
				int numVis, n;

				visReturn = (XVisualInfo *)NULL;
				n = 0;

				/* See if we can find a visual at the depth they ask for. */
				if(ourResources.applicationDepth != UNDEFINED_DEFAULT)
				{
					visTemplate.depth = ourResources.applicationDepth;
					visReturn = XGetVisualInfo(display, VisualDepthMask, 
						&visTemplate, &numVis);
					/*
					* If numVis > 1 you may want to have it pick your favorite
					* visual. This is not necessary since the user still has
					* finer control by setting XtNvisualClass or XtNvisualID.
					*/
					/* Step 5 */
					if(visReturn == (XVisualInfo *)NULL)
					{
						visTemplate.class = theVisualClass;
						visReturn = XGetVisualInfo(display, VisualClassMask, 
							&visTemplate, &numVis);
						if(visReturn != (XVisualInfo *)NULL)
						{
							int i, d = 0;
							for(i = 1; i < numVis; i++)
							{
								if(d < visReturn[i].depth)
								{
									d = visReturn[i].depth;
									n = i;
								}
							}
						}
					}
					/* Step 6 */
					if(visReturn == (XVisualInfo *)NULL)
					{
						theVisual = DefaultVisual(display, screen);
						theApplicationDepth = DefaultDepth(display, screen);
						theVisualClass = theVisual->class;
					}
					else
					{
						theVisual = visReturn[n].visual;
						theApplicationDepth = visReturn[n].depth;
						theVisualClass = visReturn[n].class;
					}
				}
			}
		}
	}
	if((theVisual->visualid == DefaultVisual(display, screen)->visualid) &&
		!ourResources.usePrivateColormap)
		theColormap = DefaultColormap(display, screen);
	else
		theColormap = XCreateColormap(display, RootWindow(display, screen),
			theVisual, AllocNone);

	/* get everything, set return values */
	*visual   = theVisual;
	*depth    = theApplicationDepth;
	*colormap = theColormap;

	return(True);
}
