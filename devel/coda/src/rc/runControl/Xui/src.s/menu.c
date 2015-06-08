#ifndef lint
static char rcsId[]="$Header: /net/mizar/usr/local/source/coda_source/runControl/Xui/menu.c,v 1.3 1999/10/25 17:30:48 abbottd Exp $"; 
#endif
/*****
* menu.c : X11/Motif menu creation/convenience routines
*
* This file Version:	$Revision: 1.3 $
*
* Creation date:		06/15/1994
* Last modification:	$Date: 1999/10/25 17:30:48 $ 
* By:					$Author: abbottd $
* Current State:		$State: Exp $
* 
* Author:				koen
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
* History:
*	iEdit
*	XntHelp
*****/ 
/*****
* Revision Log:
* $Log: menu.c,v $
* Revision 1.3  1999/10/25 17:30:48  abbottd
* Remove Linux WCHAR_T define
*
* Revision 1.2  1998/11/06 20:07:03  timmer
* Linux port wchar_t stuff
*
* Revision 1.1  1998/04/09 17:51:59  heyes
* add
*
* Revision 2.1  1997/02/11 02:23:27  newt
* Migration into eXweb
*
*****/

#include <stdio.h>
#include <string.h>

/* X11 includes */
#include <X11/Xlib.h> 
#include <X11/Xos.h>

/* Motif includes */
#include <Xm/CascadeBG.h> 
#include <Xm/CascadeB.h> 
#include <Xm/MenuShell.h> 
#include <Xm/RowColumn.h> 
#include <Xm/Separator.h>
#include <Xm/SeparatoG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/Xm.h>
#include <stdlib.h>

/* Local includes */
#include "menu.h"   

/*****
* Name: 		MenuBuild
* Return Type: 	Widget
* Description: 	multiple menu type creation routine
* In: 
*	parent:		parent for this menu
*	type:		type of desired menu, see below
*	title:		title for this menu
*	mnemonic:	shortcut key for this menu
*	items:		menu descriptions, see menu.h
* Returns:
*	On success, the created menu. Can exit the application if the menu
*	specification is invalid
* Notes:
*	- This function is a modified version of the buildMenu function
*	  in The Motif Programming Manual, Volume 6 in the O'Reilly X Window
*	  System Series.
*	- The menu types supported are: XmMENU_PULLDOWN, XmMENU_OPTION and
*	  XmMENU_POPUP 
*	- The class of the menu buttons can be:
*	  xmCascadeButtonGadgetClass, xmLabelGadgetClass, xmPushButtonGadgetClass, 
*	  xmSeparatorGadgetClass, xmToggleButtonGadgetClass and all of their Widget 
*	  counterparts. 
*****/
Widget 
MenuBuild(Widget parent, int type, String title, KeySym mnemonic,
	MenuItem *items) 
{ 
	int i, argc = 0;
	Widget menu, cascade, widget; 
	String resource;
	char menu_name[512];
	Arg args[5];

	sprintf(menu_name,"%sMenu", title);

	/* create appropriate RowColum container */
	switch(type)
	{
		case XmMENU_PULLDOWN:
			menu = XmCreatePulldownMenu(parent, menu_name, NULL, 0);
			break;
		case XmMENU_OPTION:
			strcat(menu_name, "_Option");
			menu = XmCreatePulldownMenu(parent, menu_name, NULL, 0);
			break;
		case XmMENU_POPUP:
			strcat(menu_name, "_Popup");
			menu = XmCreatePopupMenu(parent, menu_name, NULL, 0);
			break;
		default:
			return(NULL);
	}

	/* create menu triggerer */
	if(type == XmMENU_PULLDOWN) 
	{ 
        cascade = XtVaCreateManagedWidget(title,
            xmCascadeButtonGadgetClass, parent,
            XmNsubMenuId, menu,
            NULL);
		if(mnemonic != None)
		{
			XtVaSetValues(cascade, 
				XmNmnemonic, mnemonic,
				NULL);
		}
	} 
	else if(type == XmMENU_OPTION) 
	{ 
		argc = 0;
		XtSetArg(args[argc], XmNsubMenuId, menu); argc++;
		cascade = XmCreateOptionMenu(parent, title, args, argc);
	} 

	/* add all menu items */
	for(i = 0; items[i].name != NULL; i++) 
	{ 
		/* check for pull-rights */
		if(items[i].subitems) 
		{
			if(type == XmMENU_OPTION) 
			{
				fprintf(stderr, "MenuBuild:\n"
					"\tsubmenus in option menus are not allowed.\n");
				return(NULL);
			}
			else
				widget = MenuBuild(menu, XmMENU_PULLDOWN, 
					items[i].name, items[i].mnemonic, 
					items[i].subitems); 
		}
		else
			widget = XtCreateManagedWidget(items[i].name, 
				*items[i].class, menu, NULL, 0);

		/* configure the new menu item */
		if(items[i].class != &xmSeparatorGadgetClass &&  
			items[i].class != &xmSeparatorWidgetClass)  
		{ 
			argc = 0;

			/* callback? */
			if(items[i].callback) 
			{
				if(items[i].class == &xmToggleButtonWidgetClass || 
					items[i].class == &xmToggleButtonGadgetClass)
					resource = XmNvalueChangedCallback;
				else
					resource = XmNactivateCallback;

				XtAddCallback(widget, resource,
					items[i].callback, items[i].client_data);
			}

			/* do we have an mnemonic for this item? */
			if(items[i].mnemonic != None) 
			{
				XtSetArg(args[argc], XmNmnemonic, items[i].mnemonic);  
				argc++;
			}

			/* initial state? */
			if(items[i].class == &xmToggleButtonWidgetClass ||
				items[i].class == &xmToggleButtonGadgetClass)
				resource = XmNset;
			else
				resource = XmNsensitive;
				
			XtSetArg(args[argc], resource, items[i].sensitive); argc++;

			/* userData? */
			if(items[i].id != (XtPointer)-1)
			{
				XtSetArg(args[argc], XmNuserData, items[i].id);
				argc++;
			}
			/* set them */
			if(argc)
				XtSetValues(widget, args, argc);
		} 
	} 
	return(type == XmMENU_POPUP ? menu : cascade);
} 

/*****
* Name:			MenuBuildShared
* Return Type: 	Widget
* Description: 	creates a ``floating'' menu suitable for sharing between
*				multiple menubars
* In: 
*	parent:		parent for this menu;
*	type:		type of menu to be created;
*	title:		name of this menu;
*	items:		items for this menu;
* Returns:
*	the created menu.
*****/
Widget
MenuBuildShared(Widget parent, int type, String title, MenuItem *items)
{
	int i, argc = 0;
	Widget menu, widget; 
	String resource;
	char menu_name[512];
	Arg args[5];

	sprintf(menu_name,"%sMenu", title);

	/* create appropriate RowColum container */
	switch(type)
	{
		case XmMENU_PULLDOWN:
			menu = XmCreatePulldownMenu(parent, menu_name, NULL, 0);
			break;
		case XmMENU_OPTION:
			strcat(menu_name, "_Option");
			menu = XmCreatePulldownMenu(parent, menu_name, NULL, 0);
			break;
		case XmMENU_POPUP:
			strcat(menu_name, "_Popup");
			menu = XmCreatePopupMenu(parent, menu_name, NULL, 0);
			break;
		default:
			return(NULL);
	}

	/* add all menu items */
	for(i = 0; items[i].name != NULL; i++) 
	{ 
		/* check for pull-rights */
		if(items[i].subitems) 
		{
			if(type == XmMENU_OPTION) 
			{
				fprintf(stderr, "MenuBuildShared:\n"
					"\tsubmenus in option menus are not allowed.\n");
				return(NULL);
			}
			else
				widget = MenuBuild(menu, XmMENU_PULLDOWN, 
					items[i].name, items[i].mnemonic, 
					items[i].subitems); 
		}
		else
			widget = XtCreateManagedWidget(items[i].name, 
				*items[i].class, menu, NULL, 0);

		/* configure the new menu item */
		if(items[i].class != &xmSeparatorGadgetClass &&  
			items[i].class != &xmSeparatorWidgetClass)  
		{ 
			argc = 0;

			/* callback? */
			if(items[i].callback) 
			{
				if(items[i].class == &xmToggleButtonWidgetClass || 
					items[i].class == &xmToggleButtonGadgetClass)
					resource = XmNvalueChangedCallback;
				else
					resource = XmNactivateCallback;

				XtAddCallback(widget, resource,
					items[i].callback, items[i].client_data);
			}

			/* do we have an mnemonic for this item? */
			if(items[i].mnemonic != None) 
			{
				XtSetArg(args[argc], XmNmnemonic, items[i].mnemonic);  
				argc++;
			}

			/* initial state? */
			if(items[i].class == &xmToggleButtonWidgetClass ||
				items[i].class == &xmToggleButtonGadgetClass)
				resource = XmNset;
			else
				resource = XmNsensitive;
				
			XtSetArg(args[argc], resource, items[i].sensitive); argc++;

			/* userData? */
			if(items[i].id != (XtPointer)-1)
			{
				XtSetArg(args[argc], XmNuserData, items[i].id);
				argc++;
			}
			/* set them */
			if(argc)
				XtSetValues(widget, args, argc);
		} 
	} 
	return(menu);
}

/*****
* Name:			MenuAddShared
* Return Type: 	Widget
* Description: 	creates a menubar entry and adds the given menu to it.
* In: 
*	parent:		parent for this menu
*	type:		type of desired menu, see below
*	title:		title for this menu
*	mnemonic:	shortcut key for this menu
*	items:		menu items;
* Returns:
*	The new menubar entry when a pulldown or optionmenu was requested, NULL
*	otherwise.
*****/
Widget
MenuAddShared(Widget parent, int type, String title, KeySym mnemonic,
	Widget items)
{
	Arg args[2];
	int argc = 0;

	/* create menu triggerer */
	switch(type)
	{
		case XmMENU_PULLDOWN:
			XtSetArg(args[argc], XmNsubMenuId, items); argc++;
			if(mnemonic != None)
			{
				XtSetArg(args[argc], XmNmnemonic, mnemonic);
				argc++;
			}
			return(XmCreateCascadeButtonGadget(parent, title, args, argc));

		case XmMENU_OPTION:
			XtSetArg(args[argc], XmNsubMenuId, items); argc++;
			return(XmCreateOptionMenu(parent, title, args, argc));

		default:
			fprintf(stderr, "MenuAddShared: unknown menu type!\n");
			return(NULL);
	}
}

/**********
***** Common menu convenience routines
**********/

/*****
* Name: 		MenuGetChildren
* Return Type: 	void
* Description:	Get the children of a menu. 
* In: 
*	menu:		menu of which to get the children
*	children:	children of menu. This elemenent is filled upon return.
*	num_children: number of elements in children. This element is filled 
*				upon return.
* Returns:
*	nothing
*****/
void 
MenuGetChildren(Widget menu, WidgetList *children, int *num_children)
{ 
	Widget foo = NULL; 

	*children = NULL; 
	*num_children = 0; 

	if (XtIsSubclass(menu, xmCascadeButtonWidgetClass) || 
		XtIsSubclass(menu, xmCascadeButtonGadgetClass))
	{ 
		XtVaGetValues(menu, XmNsubMenuId, &foo, NULL);  
		XtVaGetValues(foo, 
			XmNchildren, &*children, 
			XmNnumChildren, &*num_children,
			NULL);    
	} 
	else 
		if ((XmIsRowColumn(menu)) == 1)
		{ 
			XtVaGetValues(menu,
				XmNchildren, &*children,
				XmNnumChildren, &*num_children, 
				NULL); 
		} 
		else
		{
			fprintf(stderr, "MenuGetChildren:\n"
				"\tcan't retrieve children: menu is of unknown class.\n");
			return;
		}
} 

/*****
* Name: 		MenuFindButtonById
* Return Type: 	Widget
* Description:	Find a widget identified by Id in a pointer array of widgets
* In: 
*	menu:		list of widgets to search
*	id:			id of widget to find
* Returns:
* 	widget found or NULL on failure.
*****/
Widget
MenuFindButtonById(Widget menu, int id)
{
	register WidgetList child_ptr;
	WidgetList childs;
	int num_childs;
	
	MenuGetChildren(menu, &childs, &num_childs);

	for(child_ptr = childs; child_ptr != NULL; child_ptr++)
	{ 
		if((MenuButtonGetId(*child_ptr)) == id)
			return(*child_ptr);	
	} 
	return(NULL); /* not found */ 
} 

/*****
* Name: 		MenuFindButtonByName
* Return Type: 	Widget
* Description:	Find a named widget in a pointer array of widgets
* In: 
*	wlist:		list of widgets to search
*	name:		name of widget to find
* Returns:
* 	widget found or NULL on failure.
*****/
Widget 
MenuFindButtonByName(WidgetList wlist, String name)
{ 
	WidgetList	child_ptr; 
	Widget		child = NULL; 

	for(child_ptr = wlist; child_ptr != NULL; child_ptr++)
	{ 
		child = *child_ptr; 
		if (strcmp(XtName(child),name) == 0) 
			return(child);  
	} 
	return(NULL);	/* not found */ 
} 

/*****
* Name: 		MenuButtonGetId
* Return Type: 	int
* Description:	This routine finds out what number a menubutton has by
*				examining the XmNuserData field of the widget
* In: 
*	button:		button for which to fetch the userData value
* Returns:
*	value of the XmNuserData resource
*****/
int 
MenuButtonGetId(Widget button)
{   
    int w_num = -1;
    XtVaGetValues(button,
        XmNuserData, &w_num,
        NULL);
    return(w_num);
}       

/*****
* Name: 		MenuIsToggleMenu
* Return Type: 	void
* Description:	Make a menu with togglebuttons act as a radiobox
* In: 
*	menu:		menu of which to set state
* Returns:
*	nothing
*****/
void 
MenuIsToggleMenu(Widget menu)
{ 
	Widget foo = NULL; 

	if (XtIsSubclass(menu, xmCascadeButtonWidgetClass) || 
		XtIsSubclass(menu, xmCascadeButtonGadgetClass))
	{ 
		XtVaGetValues(menu, 
			XmNsubMenuId, &foo, 
			NULL);  
		XtVaSetValues(foo,
			XmNradioBehavior, True,
			XmNradioAlwaysOne, True,
			NULL);
	}
	else
	{
		fprintf(stderr, "MenuToggleSet:\n\tmenu %s is no subclass "
			"of CascadeButton (internal error).\n", XtName(menu));
	}
}

/**********
***** Option menu specific convenience routines
**********/

/*****
* Name: 		MenuOptionSelectItem
* Return Type: 	void
* Description: 	selects the given option menu button;
* In: 
*	menu:		menu of which to select a menu item
*	id:			id of menu item to set
* Returns:
*	nothing
*****/
void
MenuOptionSelectItem(Widget menu, int id)
{
	Widget option_menu = NULL;
	Widget pb;

	/* Get all children and number of items contained */
	XtVaGetValues(menu, 
		XmNsubMenuId, &option_menu,
		NULL);

	/* sanity check */
	if(!option_menu)
	{
		fprintf(stderr, "MenuSelectOption:\n"
			"\tcan't select requested menu: menu has no children.\n");
		return;
	}

	/* get correct menu entry */
	if((pb = MenuFindButtonById(menu, id)) == NULL)
	{
		fprintf(stderr, "MenuSelectOption:\n"
			"\tcan't select requested menu: requested id out of range.\n");
		return;
	}

	/* Select it */
	XtVaSetValues(option_menu,
		XmNmenuHistory, pb,
		NULL);
}

/*****
* Name: 		MenuOptionSetSensitivity
* Return Type: 	void
* Description:	Set the sensitivity state of an item in a option menu
* In: 
*	menu:option menu id
*	id:			id of menu item of which the state is to be set.
*				Zero based index.
*	state:		state of the item.
* Returns:
*	nothing
*****/
void
MenuOptionSetSensitivity(Widget menu, int id, int state)
{
	Widget pb;

	/* get correct menu entry */
	if((pb = MenuFindButtonById(menu, id)) == NULL)
	{
		fprintf(stderr, "MenuOptionSetSensitivity:\n"
			"\tcan't select requested menu: requested id out of range.\n");
		return;
	}
	XtSetSensitive(pb, state);
}

/*****
* Name: 		MenuOptionGetSelected
* Return Type: 	int
* Description:	Return the id of the selected option menu item
* In: 
*	menu:		Option menu of which to return the selected menu item
* Returns:
*	the id (XmNuserData field) of the selected menu item. -1 upon failure.
*****/
int
MenuOptionGetSelected(Widget menu)
{
	int item_no;
	Widget item;

	XtVaGetValues(menu,
		XmNmenuHistory, &item,
		NULL);

	if(!item)
	{
		fprintf(stderr, "MenuOptionGetSelected:\n"
			"\tcan't retrieve selected item in option menu: XtVaGetValues "
			"returned NULL.\n");
		return(-1);
	}
	XtVaGetValues(item,
		XmNuserData, &item_no,
		NULL);
	return(item_no);
}

/**********
***** Toggle menu specific convenience routines
**********/

/*****
* Name: 		MenuToggleSelected
* Return Type: 	Boolean
* Description:	See if a toggle button is checked
* In: 
*	toggle:		toggle button widget to check
* Returns:
*	selection state of toggle
*****/
Boolean
MenuToggleSelected(Widget toggle)
{
	Boolean value = False;
	XtVaGetValues(toggle,
		XmNset, &value,
		NULL);
	return(value);
}

/*****
* Name:			MenuToggleSetState
* Return Type: 	void
* Description: 	sets the selection state of a togglebutton.
* In: 
*	menu:		menu of which an item has to be set;
*	id:			id of menubutton to be set;
*	state:		new toggle state;
* Returns:
*	nothing.
*****/
void
MenuToggleSetState(Widget menu, int id, Boolean state)
{
	Widget toggle;

	/* get correct menu entry */
	if((toggle = MenuFindButtonById(menu, id)) == NULL)
	{
		fprintf(stderr, "MenuToggleSetState:\n"
			"\tcan't select requested menu: requested id out of range.\n");
		return;
	}
	XtVaSetValues(toggle, XmNset, state, NULL);
}

/*****
* Name:			MenuToggleGetState
* Return Type: 	void
* Description: 	gets the selection state of a togglebutton.
* In: 
*	menu:		menu for which an item has to be checked;
*	id:			id of menubutton to be checked;
* Returns:
*	False when it's not selected, True otherwise.
*****/
Boolean
MenuToggleGetState(Widget menu, int id)
{
	Widget toggle;
	Boolean value = False;

	/* get correct menu entry */
	if((toggle = MenuFindButtonById(menu, id)) == NULL)
	{
		fprintf(stderr, "MenuToggleSetState:\n"
			"\tcan't get requested menu: requested id out of range.\n");
		return(False);
	}
	XtVaGetValues(toggle,
		XmNset, &value,
		NULL);
	return(value);
}
