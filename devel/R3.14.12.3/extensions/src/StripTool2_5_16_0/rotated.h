/* ************************************************************************ */


/* Header file for the `xvertext 5.0' routines.

   Copyright (c) 1993 Alan Richardson (mppa3@uk.ac.sussex.syma) */


/* ************************************************************************ */

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

#ifndef _XVERTEXT_INCLUDED_ 
#define _XVERTEXT_INCLUDED_


#define XV_VERSION      5.0
#define XV_COPYRIGHT \
      "xvertext routines Copyright (c) 1993 Alan Richardson"


/* ---------------------------------------------------------------------- */


/* text alignment */

#define NONE             0
#define TLEFT            1
#define TCENTRE          2
#define TRIGHT           3
#define MLEFT            4
#define MCENTRE          5
#define MRIGHT           6
#define BLEFT            7
#define BCENTRE          8
#define BRIGHT           9


#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif
  
float   XRotVersion                     (char*, int);
void    XRotSetMagnification            (float);
void    XRotSetBoundingBoxPad           (int);
int     XRotDrawString                  (Display*, XFontStruct*, float,
                                         Drawable, GC, int, int, char*);
int     XRotDrawImageString             (Display*, XFontStruct*, float,
                                         Drawable, GC, int, int, char*);
int     XRotDrawAlignedString           (Display*, XFontStruct*, float,
                                         Drawable, GC, int, int, char*, int);
int     XRotDrawAlignedImageString      (Display*, XFontStruct*, float,
                                         Drawable, GC, int, int, char*, int);
XPoint *XRotTextExtents                 (Display*, XFontStruct*, float,
                                         int, int, char*, int);
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#ifdef NON_ANSI
extern float   XRotVersion();
extern void    XRotSetMagnification();
extern void    XRotSetBoundingBoxPad();
extern int     XRotDrawString();
extern int     XRotDrawImageString();
extern int     XRotDrawAlignedString();
extern int     XRotDrawAlignedImageString();
extern XPoint *XRotTextExtents();
#endif



#endif /* _XVERTEXT_INCLUDED_ */



