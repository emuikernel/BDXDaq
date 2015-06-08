/*************************************************************************\
* Copyright (c) 1994-2004 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 1997-2003 Southeastern Universities Research Association,
* as Operator of Thomas Jefferson National Accelerator Facility.
* Copyright (c) 1997-2002 Deutches Elektronen-Synchrotron in der Helmholtz-
* Gemelnschaft (DESY).
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* Strip Tool Version information */

/* All version information should be changed here.  CVS commits should
 *  be tagged as StripTool-i-j-k-l if the version is i.j.k.l.  Commit
 *  information should also be put in StripTool.notes before applying
 *  the tag.  The idea is that the version, the notes, and the tag
 *  should be consistent.  The version information appears in
 *  Help|About along with the Build Date.  The Build Date is obtained
 *  from __DATE__ and __TIME__ in helpmenu_cb() in StripDialog.c, so
 *  StripDialog.c must be recompiled to update the Build Date.  It is
 *  not updated otherwise, even if other files are recompiled or the
 *  project is relinked.  A clean and rebuild makes it consistent and
 *  is a good idea, anyway, as all the dependencies are not in
 *  Makefile.Host.  Note that CVS does not care about the build date,
 *  so the build date doesn't have to be consistent with the tag.  */

#define STRIPTOOL_VERSION      2
#define STRIPTOOL_REVISION     5
#define STRIPTOOL_MODIFICATION 16
#define STRIPTOOL_UPDATE_LEVEL 0
#define STRIPTOOL_VERSION_STRING "Strip Tool Version 2.5.16.0"
