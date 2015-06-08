/*****
* cache.h : HTML document & image caching routines
*
* This file Version	$Revision: 1.1 $
*
* Creation date:		Fri May 23 14:43:04 GMT+0100 1997
* Last modification: 	$Date: 1998/04/09 17:51:48 $
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
* $Source: /net/mizar/usr/local/source/coda_source/runControl/Xui/cache.h,v $
*****/
/*****
* ChangeLog 
* $Log: cache.h,v $
* Revision 1.1  1998/04/09 17:51:48  heyes
* add
*
* Revision 1.2  1997/08/30 01:47:55  newt
* getObjectFromCache proto changed. Added pruneObjectCache.
*
* Revision 1.1  1997/05/28 02:01:31  newt
* Initial Revision
*
*****/ 

#ifndef _cache_h_
#define _cache_h_

/* get an object from the cache, given it's url */
extern XtPointer getURLObjectFromCache(String url);

/* get an object from the cache, given it's location or reference */
extern XtPointer getObjectFromCache(String file, String url);

/* insert a new entry in the cache */
extern void storeObjectInCache(XtPointer object, String file, String url);

/* replace the given object in the cache with the newly given object */
extern void replaceObjectInCache(XtPointer old_object, XtPointer new_object);

/* decrement reference counter for the given url */
extern void removeURLObjectFromCache(String url);

/* decrement reference counter for the given file */
extern void removeObjectFromCache(String file);

/* clean the cache: remove all objects which are no longer used */
extern void cleanObjectCache(void);

/* prune ObjectCache: remove all objects for which no mappings are present */
extern void pruneObjectCache(void);

/* completely destroy the cache */
extern void destroyObjectCache(void);

/* get the current size and number of objects stored in the cache */
extern void getCacheInfo(int *size, int *nobjects);

/*****
* prune the image cache to the requested size. When small_first is True,
* pruning is started with the smallest objects, otherwise it starts with
* the largest objects. Cache pruning removes cached objects with low
* reference counts.
*****/
#if 0
extern void pruneCache(int size, int small_first);
#endif

/*****
* Proto for the clean_proc in the initCache proto below. This function
* is called when cleanObjectCache want's to destroy a stored object. This
* function is called with the following arguments:
*
* clean_proc(object, client_data)
*	XtPointer object:	object to be destroyed;
*	client_data:		additional data provided by the argument with the same
*						name in initCache. This object is unused internally.
*****/
typedef void (*cleanObjectProc)(XtPointer, XtPointer);

/*****
* Proto for the size_proc in the initCache proto below. This function is
* called whenever a new object is stored in the cache and should return the
* size of the given object. The arguments are the same as above.
*****/
typedef int (*sizeObjectProc)(XtPointer, XtPointer);

/*****
* function to initialize object cache. Arguments: 
* size: maximum cache size (UNUSED)
* clean_proc: function to call when a stored object is to be destroyed 
* clean_data: client_data for the clean_proc.
*****/
extern void initCache(int size, cleanObjectProc clean_proc,
	sizeObjectProc size_proc, XtPointer client_data);

/* Don't add anything after this endif! */
#endif /* _cache_h_ */
