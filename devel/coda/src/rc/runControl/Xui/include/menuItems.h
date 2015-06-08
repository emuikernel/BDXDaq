/*****
* menuItems.h : example_2 menu item definitions
*
* This file Version	$Revision: 1.1 $
*
* Creation date:		Sun Dec 14 17:16:11 GMT+0100 1997
* Last modification: 	$Date: 1998/04/09 17:51:49 $
* By:					$Author: heyes $
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
* $Source: /net/mizar/usr/local/source/coda_source/runControl/Xui/menuItems.h,v $
*****/
/*****
* ChangeLog 
* $Log: menuItems.h,v $
* Revision 1.1  1998/04/09 17:51:49  heyes
* add
*
*****/ 

#ifndef _menuItems_h_
#define _menuItems_h_

#include <Xm/CascadeBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>

/****
* Menu defines
*****/

/* File Menu defines */
#define FILE_NEW						0
#define FILE_OPEN						1
#define FILE_OPEN_URL					2
#define FILE_SAVEAS						3
#define FILE_RELOAD						4
#define FILE_VIEW						5
#define FILE_PRINT						6
#define FILE_CLOSE						7
#define FILE_QUIT                                               8

/* File->View defines */
#define VIEW_INFO						0
#define VIEW_SOURCE						1
#define VIEW_FONTS						2

/* Edit Menu defines */
#define EDIT_FIND						0
#define EDIT_FIND_AGAIN					1

/* Option Menu defines */
#define OPTIONS_GENERAL					0
#define OPTIONS_DOCUMENT				1
#define OPTIONS_ANCHOR					2
#define OPTIONS_IMAGES					3
#define OPTIONS_FONTS					4

#define OPTIONS_START					5

#define OPTIONS_ANCHOR_BUTTONS			( OPTIONS_START )
#define OPTIONS_ANCHOR_HIGHLIGHT		( OPTIONS_START + 1 )
#define OPTIONS_ANCHOR_TRACKING			( OPTIONS_START + 2 )
#define OPTIONS_ANCHOR_TIPS				( OPTIONS_START + 3 )

#define OPTIONS_BODY_COLORS				( OPTIONS_START + 4 )
#define OPTIONS_BODY_IMAGES				( OPTIONS_START + 5 )
#define OPTIONS_ALLOW_COLORS			( OPTIONS_START + 6 )
#define OPTIONS_ALLOW_FONTS				( OPTIONS_START + 7 )
#define OPTIONS_JUSTIFY					( OPTIONS_START + 8 )

#define OPTIONS_STRICTHTML				( OPTIONS_START + 9 )
#define OPTIONS_BADHTML					( OPTIONS_START + 10 )

#define OPTIONS_ANIMATION_FREEZE		( OPTIONS_START + 11 )
#define OPTIONS_ENABLE_IMAGES			( OPTIONS_START + 12 )
#define OPTIONS_AUTOLOAD_IMAGES			( OPTIONS_START + 13 )
#define OPTIONS_SAVE					( OPTIONS_START + 14 )

#define OPTIONS_LAST					( OPTIONS_START + 15 )

/* Warning menu defines correspond with XmHTML warning type defines */

/* Window Menu defines */
#define WINDOW_RAISE					0
#define WINDOW_LOWER					1

/* Help Menu defines */
#define HELP_ABOUT						0

/* Any menu items that are commented out aren't supported (yet) */
static MenuItem viewMenu[] = {
	{"viewInfo", &xmPushButtonGadgetClass, None, True, (XtPointer)VIEW_INFO,
		viewCB, NULL, NULL },
	{"viewSource", &xmPushButtonGadgetClass, None, True, (XtPointer)VIEW_SOURCE,
		viewCB, NULL, NULL },
	{"viewFonts", &xmPushButtonGadgetClass, None, True, (XtPointer)VIEW_FONTS,
		viewCB, NULL, NULL },
	{NULL, NULL, None, True, NULL, NULL, NULL, NULL },
};

static MenuItem fileMenu[] = {
#if 0
	{"new", &xmPushButtonGadgetClass, None, False, (XtPointer)FILE_NEW,
		fileCB, NULL, NULL },
	{"_separator_", &xmSeparatorGadgetClass, None, True, NULL,
		NULL, NULL, NULL },
#endif
	{"open", &xmPushButtonGadgetClass, None, True, (XtPointer)FILE_OPEN,
		fileCB, NULL, NULL },
#if 0
	{"openURL", &xmPushButtonGadgetClass, None, False, (XtPointer)FILE_OPEN_URL,
		fileCB, NULL, NULL },
#endif
	{"saveas", &xmPushButtonGadgetClass, None, True, (XtPointer)FILE_SAVEAS,
		fileCB, NULL, NULL },
	{"reload", &xmPushButtonGadgetClass, None, False, (XtPointer)FILE_RELOAD,
		fileCB, NULL, NULL },
	{"_separator_", &xmSeparatorGadgetClass, None, True, NULL,
		NULL, NULL, NULL },
	{"view", &xmCascadeButtonGadgetClass, None, True, (XtPointer)FILE_VIEW,
		fileCB, NULL, viewMenu},
	{"_separator_", &xmSeparatorGadgetClass, None, True, NULL,
		NULL, NULL, NULL },
#if 0
	{"print", &xmPushButtonGadgetClass, None, False, (XtPointer)FILE_PRINT,
		fileCB, NULL, NULL },
	{"_separator_", &xmSeparatorGadgetClass, None, True, NULL,
		NULL, NULL, NULL },
	{"close", &xmPushButtonGadgetClass, None, False, (XtPointer)FILE_CLOSE,
		fileCB, NULL, NULL },
	{"quit", &xmPushButtonGadgetClass, None, True, (XtPointer)FILE_QUIT,
		fileCB, NULL, NULL },
#endif
	{NULL, NULL, None, True, NULL, NULL, NULL, NULL },
}; 

static MenuItem warningMenu[] = {
	{"none", &xmToggleButtonGadgetClass, None, False, (XtPointer)XmHTML_NONE,
		warningCB, NULL, NULL },
	{"all", &xmToggleButtonGadgetClass, None, True, (XtPointer)XmHTML_ALL,
		warningCB, NULL, NULL },
	{"_separator_", &xmSeparatorGadgetClass, None, True, NULL,
		NULL, NULL, NULL },
	{"unknownElement", &xmToggleButtonGadgetClass, None, False,
		(XtPointer)XmHTML_UNKNOWN_ELEMENT, warningCB, NULL, NULL },
	{"bad", &xmToggleButtonGadgetClass, None, False,
		(XtPointer)XmHTML_BAD, warningCB, NULL, NULL },
	{"openBlock", &xmToggleButtonGadgetClass, None, False,
		(XtPointer)XmHTML_OPEN_BLOCK,
		warningCB, NULL, NULL },
	{"closeBlock", &xmToggleButtonGadgetClass, None, False,
		(XtPointer)XmHTML_CLOSE_BLOCK,
		warningCB, NULL, NULL },
	{"openElement", &xmToggleButtonGadgetClass, None, False,
		(XtPointer)XmHTML_OPEN_ELEMENT, warningCB, NULL, NULL },
	{"nested", &xmToggleButtonGadgetClass, None, False,
		(XtPointer)XmHTML_NESTED, warningCB, NULL, NULL },
	{"violation", &xmToggleButtonGadgetClass, None, False,
		(XtPointer)XmHTML_VIOLATION, warningCB, NULL, NULL },
	{NULL, NULL, None, True, NULL, NULL, NULL, NULL },
}; 

static MenuItem optionMenu[] = {
#if 0
	{"general", &xmPushButtonGadgetClass, None, False, 
		(XtPointer)OPTIONS_GENERAL, optionsCB, NULL, NULL },
	{"document", &xmPushButtonGadgetClass, None, False,
		(XtPointer)OPTIONS_DOCUMENT, optionsCB, NULL, NULL },
	{"anchor", &xmPushButtonGadgetClass, None, False,
		(XtPointer)OPTIONS_ANCHOR, optionsCB, NULL, NULL },
	{"images", &xmPushButtonGadgetClass, None, False,
		(XtPointer)OPTIONS_IMAGES, optionsCB, NULL, NULL },
	{"fonts", &xmPushButtonGadgetClass, None, False,
		(XtPointer)OPTIONS_FONTS, optionsCB, NULL, NULL },

	{"_separator_", &xmSeparatorGadgetClass, None, True, NULL,
		NULL, NULL, NULL },
#endif

	/* anchor options */
	{"anchorButtons", &xmToggleButtonGadgetClass, None, True,
		(XtPointer)OPTIONS_ANCHOR_BUTTONS, optionsCB, NULL, NULL },
	{"highlightOnEnter", &xmToggleButtonGadgetClass, None, True,
		(XtPointer)OPTIONS_ANCHOR_HIGHLIGHT, optionsCB, NULL, NULL },
	{"imageAnchorTracking", &xmToggleButtonGadgetClass, None, True,
		(XtPointer)OPTIONS_ANCHOR_TRACKING, optionsCB, NULL, NULL },
	{"anchorTips", &xmToggleButtonGadgetClass, None, True,
		(XtPointer)OPTIONS_ANCHOR_TIPS, optionsCB, NULL, NULL },

	{"_separator_", &xmSeparatorGadgetClass, None, True, NULL,
		NULL, NULL, NULL },

	/* body options */
	{"enableBodyColors", &xmToggleButtonGadgetClass, None, True,
		(XtPointer)OPTIONS_BODY_COLORS, optionsCB, NULL, NULL },
	{"enableBodyImages", &xmToggleButtonGadgetClass, None, True,
		(XtPointer)OPTIONS_BODY_IMAGES, optionsCB, NULL, NULL },
	{"enableDocumentColors", &xmToggleButtonGadgetClass, None, True,
		(XtPointer)OPTIONS_ALLOW_COLORS, optionsCB, NULL, NULL },
	{"enableDocumentFonts", &xmToggleButtonGadgetClass, None, True,
		(XtPointer)OPTIONS_ALLOW_FONTS, optionsCB, NULL, NULL },
	{"enableOutlining", &xmToggleButtonGadgetClass, None, True,
		(XtPointer)OPTIONS_JUSTIFY, optionsCB, NULL, NULL },

	{"_separator_", &xmSeparatorGadgetClass, None, True, NULL,
		NULL, NULL, NULL },

	/* parser options */
	{"strictHTMLChecking", &xmToggleButtonGadgetClass, None, False,
		(XtPointer)OPTIONS_STRICTHTML, optionsCB, NULL, NULL },
	{"warning", &xmCascadeButtonGadgetClass, None, True,
		(XtPointer)OPTIONS_BADHTML, optionsCB, NULL, warningMenu},

	{"_separator_", &xmSeparatorGadgetClass, None, True, NULL,
		NULL, NULL, NULL },

	/* image options */
	{"freezeAnimations", &xmToggleButtonGadgetClass, None, True,
		(XtPointer)OPTIONS_ANIMATION_FREEZE, optionsCB, NULL, NULL },
	{"imageEnable", &xmToggleButtonGadgetClass, None, True,
		(XtPointer)OPTIONS_ENABLE_IMAGES, optionsCB, NULL, NULL },
	{"autoImageLoad", &xmToggleButtonGadgetClass, None, True,
		(XtPointer)OPTIONS_AUTOLOAD_IMAGES, optionsCB, NULL, NULL },

	{"_separator_", &xmSeparatorGadgetClass, None, True, NULL,
		NULL, NULL, NULL },

	{"save", &xmPushButtonGadgetClass, None, False,
		(XtPointer)OPTIONS_SAVE, optionsCB, NULL, NULL },
	{NULL, NULL, None, True, NULL, NULL, NULL, NULL },
};

/* the Edit menu */
static MenuItem editMenu[] = {
	{"find", &xmPushButtonGadgetClass, None, True,
		(XtPointer)EDIT_FIND, editCB, NULL, NULL },
	{"findAgain", &xmPushButtonGadgetClass, None, True,
		(XtPointer)EDIT_FIND_AGAIN, editCB, NULL, NULL },
	{NULL, NULL, None, True, NULL, NULL, NULL, NULL },
};

/* the Window menu */
static MenuItem windowMenu[] = {
	{"raise", &xmPushButtonGadgetClass, None, True,
		(XtPointer)WINDOW_RAISE, windowCB, NULL, NULL },
	{"lower", &xmPushButtonGadgetClass, None, True,
		(XtPointer)WINDOW_LOWER, windowCB, NULL, NULL },
	{NULL, NULL, None, True, NULL, NULL, NULL, NULL },
};

/* the Help menu */
static MenuItem helpMenu[] = {
	{"about", &xmPushButtonGadgetClass, None, True,
		(XtPointer)HELP_ABOUT, helpCB, NULL, NULL },
	{NULL, NULL, None, True, NULL, NULL, NULL, NULL },
};

/* Don't add anything after this endif! */
#endif /* _menuItems_h_ */
