/*-----------------------------------------------------------------------------
 * Copyright (c) 1991,1992 Southeastern Universities Research Association,
 *                         Continuous Electron Beam Accelerator Facility
 *
 * This software was developed under a United States Government license
 * described in the NOTICE file included as part of this distribution.
 *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
 * Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
 *-----------------------------------------------------------------------------
 * 
 * Description:
 *	CODA mixed label widget private header file
 *	
 * Author:  Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: MixedLabelP.h,v $
 *   Revision 1.1  1997/07/30 14:33:33  heyes
 *   add more xpm support
 *
 *   Revision 1.1.1.1  1996/08/21 19:36:08  heyes
 *   Imported sources
 *
 *	  
 */
#ifndef _MIXED_LABEL_P_H
#define _MIXED_LABEL_P_H

#include "MixedLabel.h"

/* Mixed label is derived from core , so no need to include the
 * super class provate header file, no internal types need to
 * be defined */

/* new fields for the mixed label widget */
typedef struct {
  /* resources */
  Pixel       foreground;
  XFontStruct *font;
  char        *top_label;
  char        *bottom_label;
  Pixmap      n_pixmap;
  Pixmap      h_pixmap;
  Boolean     resize;
  Boolean     set;
  Dimension   space;
  XtCallbackList callback;
  /* internal */
  GC          gc;
  GC          inverted_gc;
  GC          current_gc;
  Boolean     inverted;
  Dimension   desired_width;
  Dimension   desired_height;
  Dimension   top_label_width;
  Dimension   top_label_height;
  Dimension   btm_label_width;
  Dimension   btm_label_height;
  Dimension   n_pixmap_width;
  Dimension   n_pixmap_height;
  Dimension   h_pixmap_width;
  Dimension   h_pixmap_height;
  Cardinal    depth;
  Position    top_label_x;
  Position    top_label_y;
  Position    btm_label_x;
  Position    btm_label_y;
  Position    n_pixmap_x;
  Position    n_pixmap_y;
  Position    h_pixmap_x;
  Position    h_pixmap_y;
}XcodaMixedLabelPart;  
  
/* define full instance record */
typedef struct _XcodaMixedLabelRec{
  CorePart            core;
  XcodaMixedLabelPart mixedLabel;
}XcodaMixedLabelRec;

/* define class part */
typedef struct _XcodaMixedLabelClassPart{
  XtPointer           extension;
}XcodaMixedLabelClassPart;

/* define whole class part */
typedef struct _XcodaMixedLabelClassRec{
  CoreClassPart       core_class;
  XcodaMixedLabelClassPart mixedLabel_class;
}XcodaMixedLabelClassRec;

extern XcodaMixedLabelClassRec xcodaMixedLabelClassRec;

#endif /* MixedLabelP.h */

