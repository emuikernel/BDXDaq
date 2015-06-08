#ifndef lint
static char rcsId[]="$Header: /net/mizar/usr/local/source/coda_source/runControl/Xui/cache.c,v 1.3 1999/10/25 17:29:56 abbottd Exp $";
#endif
/*****
* cache.c : Generic caching routines.
*
* This file Version	$Revision: 1.3 $
*
* Creation date:		Fri May 23 14:42:10 GMT+0100 1997
* Last modification: 	$Date: 1999/10/25 17:29:56 $
* By:					$Author: abbottd $
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
* $Log: cache.c,v $
* Revision 1.3  1999/10/25 17:29:56  abbottd
* Remove Linux WCHAR_T define
*
* Revision 1.2  1998/11/06 20:07:02  timmer
* Linux port wchar_t stuff
*
* Revision 1.1  1998/04/09 17:51:58  heyes
* add
*
* Revision 1.3  1997/10/23 00:28:36  newt
* XmHTML Beta 1.1.0 release
*
* Revision 1.2  1997/08/30 01:47:16  newt
* getObjectFromCache now takes two args: file and url. Fixed a few potential
* misalignment bugs when an url mapping goes bad.
*
* Revision 1.1  1997/05/28 02:01:28  newt
* Initial Revision
*
*****/ 

#include <stdio.h>
#include <string.h>

#include <XmHTML.h>
#include <stdlib.h>
#include "cache.h"

/*** External Function Prototype Declarations ***/

/*** Public Variable Declarations ***/

/*** Private Datatype Declarations ****/
/* cache object */
typedef struct _CacheObject{
	XtPointer object;		/* object data */
	String file;			/* location of object on local file system */
	int ref_count;			/* reference count for this entry */
	int map_count;			/* no of mappings to this entry */
	int size;				/* size of this object */
	struct _CacheObject *prev; /* previous entry */
	struct _CacheObject *next; /* next entry */
}CacheObject;

/* mapping between object urls and the object cache */
typedef struct _objectMapping{
	String url;					/* url referencing this object */
	int ref_count;				/* reference counter for this mapping */
	CacheObject *cache_entry;	/* ptr. to cache entry */
	struct _objectMapping *prev;	/* previous entry */
	struct _objectMapping *next;	/* next entry */
}ObjectMapping;

/*** Private Function Prototype Declarations ****/
static void addObjectMapping(CacheObject *cache_entry, String url);
static void discardCacheEntry(CacheObject *cache_entry);
static void discardMapEntry(ObjectMapping *cache_entry);

static ObjectMapping *newMap(ObjectMapping *map, ObjectMapping *prev_map,
	CacheObject *cache_entry, String url);
static CacheObject *newEntry(CacheObject *entry, CacheObject *prev_entry,
	XtPointer object, String file, String url);

/*** Private Variable Declarations ***/
/* initial cache size in bytes: 512 kb */
#define DEFAULT_CACHE_SIZE	524288

static int cacheSize = DEFAULT_CACHE_SIZE;
static int currentCacheSize = 0;			/* used size of the cache */

static CacheObject *objectCache;		/* actual object cache */
static ObjectMapping *objectMapping;	/* url->file mapping */

static cleanObjectProc cleanProc = NULL;	/* clean-up function */
static sizeObjectProc sizeProc   = NULL;	/* object size function */
static XtPointer clientData;				/* client_data for external funcs */

/******
*** Private functions
******/
/*****
* Name: 		newMap
* Return Type: 	ObjectMapping*
* Description:	add a new entry to the chain of url->cache_entry mappings
* In: 
*	map:		parent map;
*	prev_map:	previous map;
*	cache_e..:	cache entry referenced;
*	url:		url used to reference cache_entry
* Returns:
*	a newly created map entry.
*****/
static ObjectMapping*
newMap(ObjectMapping *map, ObjectMapping *prev_map, CacheObject *cache_entry,
	String url)
{
	if(map == NULL)
	{
		map = (ObjectMapping*)malloc(sizeof(ObjectMapping));
		map->url         = strdup(url);
		map->cache_entry = cache_entry;
		map->next        = NULL;
		map->prev        = prev_map;
		map->ref_count   = 1;
		cache_entry->map_count++;
	}
	else
		map->next = newMap(map->next, map, cache_entry, url);
	return(map);
}

/*****
* Name: 		newEntry
* Return Type: 	CacheObject*
* Description:	add a new entry to the cache
* In: 
*	entry:		parent entry;
*	prev_en..:	previous entry;
*	object:		data to be stored;
*	file:		location on the local file system;
*	url:		url used to reference this object;
* Returns:
*
*****/
static CacheObject*
newEntry(CacheObject *entry, CacheObject *prev_entry, XtPointer object,
	String file, String url)
{
	if(entry == NULL)
	{
		/* entry does not yet exists, add one */
		entry = (CacheObject*)malloc(sizeof(CacheObject));
		entry->object    = object;
		entry->file      = strdup(file);
		entry->next      = NULL;
		entry->prev      = prev_entry;
		entry->ref_count = 1;
		entry->map_count = 0;
		entry->size      = sizeof(CacheObject) + strlen(file);
		if(sizeProc)
			entry->size += sizeProc(object, clientData);

		currentCacheSize += entry->size;

		/* and add a url mapping for this one as well */
		addObjectMapping(entry, url);
	}
	else
		entry->next = newEntry(entry->next, entry, object, file, url);
	return(entry);
}

/*****
* Name: 		addObjectMapping
* Return Type: 	void
* Description:	add a mapping between the given cache item and url
* In: 
*	cache_e..:	cache entry referenced;
*	url:		url used to reference cache_entry
* Returns:
*	nothing, but updates the overall objectMappings.
*****/
static void
addObjectMapping(CacheObject *cache_entry, String url)
{
	ObjectMapping *tmp;

	/* see if this mapping has already been added */
	for(tmp = objectMapping; tmp != NULL && strcmp(tmp->url, url);
		tmp = tmp->next);

	/* entry already exists, return */
	if(tmp)
	{
		tmp->ref_count++;
		cache_entry->map_count++;
	}
	else
		objectMapping = newMap(objectMapping, NULL, cache_entry, url);
}

/*****
* Name: 		discardCacheEntry
* Return Type: 	void
* Description:	Remove the given object cache entry from the object cache.
*				Does not delete the entry but removes it from the linked list.
* In: 
*	cache_e..:	entry to be removed;
* Returns:
*	nothing.
*****/
static void
discardCacheEntry(CacheObject *cache_entry)
{
	CacheObject *tmp;

	/* special case if cache_entry is the first entry in the object cache */
	if(objectCache == cache_entry)
	{
		objectCache = objectCache->next;

		/* can happen if this is the last item in the list */
		if(objectCache != NULL)
			objectCache->prev = NULL;	/* invalidate previous entry */

		/* update cache size */
		currentCacheSize -= cache_entry->size;

		return;
	}

	for(tmp = objectCache; tmp != NULL; tmp = tmp->next)
	{
		if(tmp == cache_entry)
		{
			CacheObject *next, *prev;

			/* update cache size */
			currentCacheSize -= tmp->size;

			/* unhook this entry */
			next = tmp->next;	/* ptr to next element */
			prev = tmp->prev;	/* ptr to previous element */
			if(tmp->prev)
				tmp->prev->next = next;
			if(tmp->next)
				tmp->next->prev = prev;
			return;
		}
	}
}

/*****
* Name: 		discardMapEntry
* Return Type: 	void
* Description:	same as discardCacheEntry but for the mappings
* In: 
*	map_e..:	mapping to be removed;
* Returns:
*	nothing.
*****/
static void
discardMapEntry(ObjectMapping *map_entry)
{
	ObjectMapping *tmp;

	/* special case if map_entry is the first entry in the object cache */
	if(objectMapping == map_entry)
	{
		objectMapping = objectMapping->next;
		/* can happen if this is the last item in the list */
		if(objectMapping != NULL)
			objectMapping->prev = NULL;	/* invalidate previous entry */
		return;
	}

	for(tmp = objectMapping; tmp != NULL; tmp = tmp->next)
	{
		if(tmp == map_entry)
		{
			ObjectMapping *prev, *next;
			/* unhook this entry */
			next = tmp->next;	/* ptr to next element */
			prev = tmp->prev;	/* ptr to previous element */
			if(tmp->prev)
				tmp->prev->next = next;
			if(tmp->next)
				tmp->next->prev = prev;
			return;
		}
	}
}

/******
*** Public functions
******/

/*****
* Name: 		getURLObjectFromCache
* Return Type: 	XtPointer
* Description:	get an object from the objectCache, given it's url
* In: 
*	url:		url for which to retrieve a cached object.
* Returns:
*	the cached object if it exists or NULL of it doesn't.
*****/
XtPointer
getURLObjectFromCache(String url)
{
	ObjectMapping *tmp;

	if(url == NULL)
		return(NULL);

	for(tmp = objectMapping; tmp != NULL && strcmp(tmp->url, url);
		tmp = tmp->next);
	if(tmp)
		return(tmp->cache_entry->object);
	return(NULL);
}

/*****
* Name: 		getObjectFromCache
* Return Type: 	XtPointer
* Description:	get an object from the objectcache, given it's location
* In: 
*	file:		file name of a cached object.
* Returns:
*	the cached object if it exists or NULL of it doesn't.
*****/
XtPointer
getObjectFromCache(String file, String url)
{
	CacheObject *tmp;

	/* no filename given, try matching with url */
	if(file == NULL)
		return(getURLObjectFromCache(url));

	/* filename given, try with filename */
	for(tmp = objectCache; tmp != NULL && strcmp(tmp->file, file);
		tmp = tmp->next);

	if(tmp)
		return(tmp->object);
	return(NULL);
}

/*****
* Name: 		storeObjectInCache
* Return Type: 	void
* Description:	insert a new entry in the object cache
* In: 
*	object:		object to be cached;
*	file:		location on local file system; primary object identifier;
*	url:		url referencing object to be cached; secondary object
*				indentifier.
* Returns:
*	nothing.
*****/
void
storeObjectInCache(XtPointer object, String file, String url)
{
	CacheObject *tmp;

	/* check if this file is already stored */
	for(tmp = objectCache; tmp != NULL && strcmp(tmp->file, file);
		tmp = tmp->next);

	/* entry already exists, check if we need to update the mapping */
	if(tmp != NULL)
	{
		/* increment reference counter for this object */
		tmp->ref_count++;

		/* add a new mapping for this object as well */
		addObjectMapping(tmp, url);
	}
	else /* add a new cache item */
		objectCache = newEntry(objectCache, NULL, object, file, url);
}

/*****
* Name: 		replaceObjectInCache
* Return Type: 	void
* Description:	replace an entry in the object cache with the newly given data
* In: 
*	old_object:	object to be replaced;
*	new_object: new data to be stored at the old object's location.
* Returns:
*	nothing.
*****/
void
replaceObjectInCache(XtPointer old_object, XtPointer new_object)
{
	CacheObject *tmp;

	/* locate original object and replace with new object data */
	for(tmp = objectCache; tmp != NULL && tmp->object != old_object;
		tmp = tmp->next);

	/* found it, replace with new data */
	if(tmp)
	{
		/* update cache size */
		currentCacheSize -= tmp->size;

		/* store new object */
		tmp->object = new_object;

		/* update object */
		if(sizeProc)
		{
			tmp->size = sizeProc(new_object, clientData);
			currentCacheSize += tmp->size;
		}
	}
	else
		fprintf(stderr, "Warning:\n    cache replacement: could not locate "
			"given object in object cache.\n\n");
}

/*****
* Name: 		removeURLObjectFromCache
* Return Type: 	void
* Description:	removes the mapping for the given url.
* In: 
*	url:		url to be invalidated.
* Returns:
*	nothing.
* Note:
*	This routine does not actually delete a mapping but decrements the 
*	reference counter for the given url and the cache object it's pointing to.
*****/
void
removeURLObjectFromCache(String url)
{
	ObjectMapping *tmp;
	for(tmp = objectMapping; tmp != NULL && strcmp(tmp->url, url);
		tmp = tmp->next);

	if(tmp)
	{
		tmp->ref_count--;
		tmp->cache_entry->map_count--;
	}
	else
		fprintf(stderr, "Warning:\n    cache URL removal: missing object "
			"entry in object cache.\n    (object: %s)\n\n", url);
}

/*****
* Name: 		removeObjectFromCache
* Return Type: 	void
* Description:	decrement reference counter for the given file
* In: 
*	file:		object to be removed
* Returns:
*	nothing.
*****/
void
removeObjectFromCache(String file)
{
	CacheObject *tmp;

	for(tmp = objectCache; tmp != NULL && strcmp(tmp->file, file);
		tmp = tmp->next);

	if(tmp)
		tmp->ref_count--;
	else
		fprintf(stderr, "Warning:\n    cache removal: missing object entry in "
			"object cache:\n    %s\n\n", file);
}

/*****
* Name: 		cleanObjectCache
* Return Type:	void
* Description:	removes unused objects (zero reference count) from the object
*				cache
* In: 
*	nothing
* Returns:
*	nothing
*****/
void
cleanObjectCache(void)
{
	CacheObject *tmp;
	ObjectMapping *tmp_map;

	if(cleanProc == NULL)
	{
		fprintf(stderr, "Warning:\n    cache cleaning: no clean_proc "
			"defined!\n    The cache list will be updated but stored objects "
			"are NOT freed.\n\n");
	}

	/*
	* Cross check object mapping reference counts against cache reference
	* counts before really freeing an entry.
	*/
	for(tmp_map = objectMapping; tmp_map != NULL; tmp_map = tmp_map->next)
	{
		/* object reference count is stronger than object mapping count */
		if(tmp_map->cache_entry->ref_count <= 0 && tmp_map->ref_count > 0)
		{
			/* objects with 0 reference count are always kicked out */
			fprintf(stderr, "Warning:\n    cache object mapping\n    From: %s"
				"\n    To  : %s\n    has a positive mapping count (%i) while "
				"object reference count is zero.\n\n",
				tmp_map->url, tmp_map->cache_entry->file, tmp_map->ref_count);
			tmp_map->ref_count = 0;
		}

		if(tmp_map->ref_count <= 0)
		{
			if(tmp_map->cache_entry->ref_count != 0)
			{
				fprintf(stderr, "Error:\n    cache cleaning: reference count "
					"for cached object\n    %s is non-zero (%i).\n\n",
					tmp_map->cache_entry->file,
					tmp_map->cache_entry->ref_count);
				tmp_map->ref_count = tmp_map->cache_entry->ref_count;
			}
			else
			{
				ObjectMapping *save;

				save = (tmp_map->prev ? tmp_map->prev : tmp_map->next);

				/* discard this mapping */
				discardMapEntry(tmp_map);
				/* and free it */
				free(tmp_map->url);
				free(tmp_map);
				tmp_map = save;

				/* sanity if last item in list is removed */
				if(tmp_map == NULL)
					break;
			}
		}
	}

	for(tmp = objectCache; tmp != NULL; tmp = tmp->next)
	{
		/* zero reference count, remove it */
		if(tmp->ref_count <= 0)
		{
			CacheObject *save;

			save = (tmp->prev ? tmp->prev : tmp->next);

			/* discard this cache entry from the objectCache */
			discardCacheEntry(tmp);

			/* call clean function to release cached data */
			if(cleanProc)
				cleanProc(tmp->object, clientData);

			tmp->ref_count = 0;
			tmp->map_count = 0;

			/* and wipe the entry */
			free(tmp->file);
			free(tmp);
			tmp = save;
			/* sanity if last item in list is removed */
			if(tmp == NULL)
				break;
		}
	}
}

/*****
* Name: 		destroyObjectCache
* Return Type:	void
* Description:	wipes the object cache completely clean.
*				cache
* In: 
*	nothing
* Returns:
*	nothing
*****/
void
destroyObjectCache(void)
{
	CacheObject *tmp;
	ObjectMapping *tmp_map;

	if(cleanProc == NULL)
	{
		fprintf(stderr, "Warning:\n    cache destruction: no clean_proc "
			"defined!\n    The cache list will be updated but stored objects "
			"are NOT freed.\n\n");
	}

	/*
	* Cross check object mapping reference counts against cache reference
	* counts before really freeing an entry.
	*/
	while(objectMapping != NULL)
	{
		tmp_map = objectMapping->next;
		free(objectMapping->url);
		free(objectMapping);
		objectMapping = tmp_map;
	}

	while(objectCache != NULL)
	{
		tmp = objectCache->next;
	
		/* call clean function to release cached data */
		if(cleanProc)
			cleanProc(objectCache->object, clientData);

		/* and wipe the entry */
		free(objectCache->file);
		free(objectCache);
		objectCache = tmp;
	}
}

/*****
* Name:			pruneObjectCache
* Return Type: 	void
* Description: 	removes all mappings with a reference count of zero and
*				all objects which are no longer being referred.
* In: 
*	nothing;
* Returns:
*	nothing;
*****/
void
pruneObjectCache(void)
{
	CacheObject *tmp;
	ObjectMapping *tmp_map;

	if(cleanProc == NULL)
	{
		fprintf(stderr, "Warning:\n    object cache pruning: no clean_proc "
			"defined!\n    The cache list will be updated but stored objects "
			"are NOT freed.\n\n");
	}

	/* remove all unused mappings */
	for(tmp_map = objectMapping; tmp_map != NULL; tmp_map = tmp_map->next)
	{
		if(tmp_map->ref_count <= 0)
		{
			ObjectMapping *save;

			save = (tmp_map->prev ? tmp_map->prev : tmp_map->next);

			/* discard this mapping */
			discardMapEntry(tmp_map);
			/* and free it */
			free(tmp_map->url);
			free(tmp_map);
			tmp_map = save;

			/* sanity if last item in list is removed */
			if(tmp_map == NULL)
				break;
		}
	}

	/* remove all unused objects */
	for(tmp = objectCache; tmp != NULL; tmp = tmp->next)
	{
		/* zero reference count, remove it */
		if(tmp->map_count == 0)
		{
			CacheObject *save;

			save = (tmp->prev ? tmp->prev : tmp->next);

			/* discard this cache entry from the objectCache */
			discardCacheEntry(tmp);

			/* call clean function to release cached data */
			if(cleanProc)
				cleanProc(tmp->object, clientData);

			tmp->ref_count = 0;
			tmp->map_count = 0;

			/* and wipe the entry */
			free(tmp->file);
			free(tmp);
			tmp = save;
			/* sanity if last item in list is removed */
			if(tmp == NULL)
				break;
		}
	}
}

/*****
* Name:			destroyCache
* Return Type:	void
* Description:	completely obliterates the cache
* In: 
*	nothing
* Returns:
*	nothing
*****/
void
destroyCache(void)
{
	CacheObject *tmp;
	ObjectMapping *tmp_map;

	if(cleanProc == NULL)
	{
		fprintf(stderr, "Warning:\n    cache destruction: no clean_proc "
			"defined!\n    The cache list will be destroyed but stored "
			"objects are NOT freed.\n\n");
	}

	/* first clean out the cache mappings */
	while(objectMapping != NULL)
	{
		tmp_map = objectMapping->next;
		free(objectMapping->url);
		free(objectMapping);
		objectMapping = tmp_map;
	}
	
	/* now clean out the cache itself */
	while(objectCache != NULL)
	{
		tmp = objectCache->next;

		/* call clean function to release cached data */
		if(cleanProc)
			cleanProc(tmp->object, clientData);

		/* and wipe the entry */
		free(tmp->file);
		free(tmp);

		objectCache = tmp;
	}

	/* reset everything to default */
	objectMapping = (ObjectMapping*)NULL;
	objectCache   = (CacheObject*)NULL;
	cleanProc     = (cleanObjectProc)NULL;
	sizeProc      = (sizeObjectProc)NULL;
	clientData    = (XtPointer)NULL;
	cacheSize     = DEFAULT_CACHE_SIZE;
	currentCacheSize = 0;
}

/*****
* Name: 		getCacheInfo
* Return Type: 	void
* Description: 	returns cache usage info
* In: 
*	*size:		occupied size of image cache;
*	*nobjects:	number of objects stored in cache.
* Returns:
*	nothing but the arguments are updated.
*****/
void
getCacheInfo(int *size, int *nobjects)
{
	CacheObject *tmp;
	int i = 0;

	for(tmp = objectCache; tmp != NULL; tmp = tmp->next, i++);

	*nobjects = i;
	*size = currentCacheSize;
}

/*****
* Name:			initCache
* Return Type:	void
* Description:	initialize the object caching routines.
* In: 
*	size:		maximum cache size;
*	clean_proc:	procedure to call for actual object destruction;
*	size_proc:	procedure to call for determining the size of a cached object;
*	client_d..:	data to be used as the second arg. when calling the above to
*				functions. This arg is unused internally.
* Returns:
*	nothing
* Note:
*	cacheSize is unused for now
*****/
void
initCache(int size, cleanObjectProc clean_proc, sizeObjectProc size_proc, 
	XtPointer client_data)
{
	/* uses default cache size if size is not set */
	if(size)
		cacheSize = size;
	currentCacheSize = 0;
	cleanProc  = clean_proc;
	sizeProc   = size_proc;
	clientData = client_data;
}

/*****
* Special debugging functions:
* _listCachedObjects(void)
*	lists all entries in the object cache
* _listObjectMappings(void)
*	lists all url->file mappings in the objectMapping.
*****/
#ifdef DEBUG
void
_listCachedObjects(void)
{
	CacheObject *tmp;
	int i = 0;

	printf("Object id : <file location> Reference Count\n");

	for(tmp = objectCache; tmp != NULL; tmp = tmp->next, i++)
		printf("%i: %s, %i\n", i, tmp->file, tmp->ref_count);

	printf("Total objects cached: %i\n", i);
}

void
_listObjectMappings(void)
{
	ObjectMapping *tmp;
	int i = 0;

	printf("Mapping id : <url + url ref_count> : <file location + "
		"file ref_count>\n");
	for(tmp = objectMapping; tmp != NULL; tmp = tmp->next, i++)
		printf("%i:\n\tFrom : %s (%i)\n\tTo   : %s (%i)\n", i, tmp->url,
			tmp->ref_count, tmp->cache_entry->file,
			tmp->cache_entry->ref_count);
	printf("Total mappings: %i\n", i);
}
#endif /* DEBUG */
