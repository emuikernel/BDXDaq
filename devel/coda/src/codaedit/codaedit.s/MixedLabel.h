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
 *	CODA Mixed label and Pixmap Widget
 *	
 * Author:  Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: MixedLabel.h,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:08  heyes
 *   Imported sources
 *
 *	  
 */
#ifndef _MixedLabel_h
#define _MixedLabel_h


/* Mixed Lable is derived from core, so no need to include
   the super class public header file */
/* New resources */
#define XcodaNtopLabel          "topLabel"
#define XcodaNbottomLabel       "bottomLabel"
#define XcodaNnormalPixmap      "normalPixmap"
#define XcodaNhighLightPixmap   "highLightPixmap"
#define XcodaNresize            "resize"
#define XcodaNfont              "font"
#define XcodaNforeground        "foreground"
#define XcodaNsetHighLight      "setHighLight"
#define XcodaNspace             "space"
#define XcodaNactivateCallback  "activateCallback"

#define XcodaCTopLabel          "TopLabel"
#define XcodaCBottomLabel       "BottomLabel"
#define XcodaCNormalPixmap      "NormalPixmap"
#define XcodaCHighLightPixmap   "HighLightPixmap"
#define XcodaCResize            "Resize"
#define XcodaCAlignment         "Alignment"
#define XcodaCFont              "Font"
#define XcodaCForeground        "Foreground"
#define XcodaCSetHighLight      "SetHighLight"
#define XcodaCSpace             "Space"
#define XcodaCActivateCallback  "ActivateCallback"

/* extern reference to the class record pointer */
extern WidgetClass xcodaMixedLabelWidgetClass;

/* type definition for mixed label widget */
typedef struct _XcodaMixedLabelClassRec * XcodaMixedLabelWidgetClass;
typedef struct _XcodaMixedLabelRec      * XcodaMixedLabelWidget;

#define XcodaIsMixedLabel(w) XtIsSubclass(w,xcodaMixedLabelWidgetClass)

extern Boolean XcodaMixedLabelIsHighLighted();
extern void    XcodaMixedLabelSetHighLightOn();
extern void    XcodaMixedLabelSetHighLightOff(); 

#endif /*MixedLabel.h*/
