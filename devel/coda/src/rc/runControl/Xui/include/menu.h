/*****
* menu.h : menu protos, structures & defines
*
* This file Version	$Revision: 1.1 $
*
* Creation date:		Sun Nov  9 16:21:52 GMT+0100 1997
* Last modification: 	$Date: 1998/04/09 17:51:49 $
* By:					$Author: heyes $
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
* $Source: /net/mizar/usr/local/source/coda_source/runControl/Xui/menu.h,v $
*****/
/*****
* ChangeLog 
* $Log: menu.h,v $
* Revision 1.1  1998/04/09 17:51:49  heyes
* add
*
*****/ 

#ifndef _menu_h_
#define _menu_h_

/*****
* Definition of the menu items.
* The id field in this structure is stored as XmNuserData for the created
* menu item. Setting it to -1 will make MenuBuild ignore it.
*****/ 
typedef struct _MenuItem{
	char *name;						/* menu item name		*/
	WidgetClass *class;				/* menu item class		*/
	KeySym mnemonic;				/* menu item mnemonic	*/
	Bool sensitive; 				/* initial menu state	*/
	XtPointer id;					/* menu button id		*/
	XtCallbackProc callback;		/* menu callback		*/
	XtPointer client_data; 			/* menu callback data	*/
	struct _MenuItem *subitems;		/* pullright menu items	*/
}MenuItem; 

/* build an entire menu, including the menubar entry */
extern Widget MenuBuild(Widget parent, int type, String title, 
	KeySym mnemonic, MenuItem *items);

/* build an entire ``floating'' menu (no menubar entry) */
extern Widget MenuBuildShared(Widget parent, int type, String title,
	MenuItem *items);

/* create a menubar entry and add the ``floating'' menu to it */
extern Widget MenuAddShared(Widget parent, int type, String title,
	KeySym mnemonic, Widget items);

/*****
* common menu convenience routines
*****/

/* return the children of the given menu */
extern void MenuGetChildren(Widget menu, WidgetList *children,
	int *num_children);

/* locate a menu button by it's id */
extern Widget MenuFindButtonById(Widget menu, int id);

/* locate a menu button by it's name */
extern Widget MenuFindButtonByName(WidgetList wlist, String name);

/* return the id of a menu button (value of XmNuserData resource) */
extern int MenuButtonGetId(Widget button);

/* make a menu with toggleButtons act as a radiobox */
extern void MenuIsToggleMenu(Widget menu);

/*****
* Option Menu specific routines
*****/

/* select the given OptionMenu menu item */
extern void MenuOptionSelectItem(Widget menu, int id);

/* set the state of a Toggle menu item in an OptionMenu */
extern void MenuOptionSetSensitivity(Widget menu, int id, int state);

/* return the selected item in an OptionMenu */
extern int MenuOptionGetSelected(Widget menu);

/*****
* Toggle Menu specific routines
*****/

/* return the selection state of a Toggle menu item */
extern Boolean MenuToggleSelected(Widget toggle);

/* get the state of a Toggle menu item */
extern Boolean MenuToggleGetState(Widget menu, int id);

/* set the state of a Toggle menu item */
extern void MenuToggleSetState(Widget menu, int id, Boolean state);

/* Don't add anything after this endif! */
#endif /* _menu_h_ */
