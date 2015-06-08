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
 * public header for Ruler Widget
 *	
 * Author:  Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Ruler.h,v $
 *   Revision 1.2  1998/02/03 15:19:48  rwm
 *   Casts and stuff to get rid of compiler warnings.
 *
 *   Revision 1.1.1.1  1996/08/21 19:36:08  heyes
 *   Imported sources
 *
 */
#ifndef RULER_H
#define RULER_H
extern WidgetClass xcodaRulerWidgetClass;

typedef struct _XcodaRulerClassRec * XcodaRulerWidgetClass;
typedef struct _XcodaRulerRec      * XcodaRulerWidget;

#define XcodaTOP_HORIZONTAL            0  /*resources for orientation*/
#define XcodaLEFT_VERTICAL             1  /* of the ruler */
#define XcodaBOTTOM_HORIZONTAL         2
#define XcodaRIGHT_VERTICAL            3
/*define reource string for the ruler widget*/
#define XcodaNvalueChangedCallback "valueChangedCallback"
#define XcodaNorientation          "orientation"
#define XcodaNminimum              "minimum"
#define XcodaNindicatorColor       "indicatorColor"
#define XcodaNposition             "position"
#define XcodaNtickerColor          "tickerColor"
#define XcodaNnumDivBetweenMajor   "numDivBetweenMajor"  
#define XcodaNrealTickerDiv        "realTickerDiv"  
#define XcodaNworldTickerDiv       "worldTickerDiv"

#define XtCValueChangedCallback "ValueChangedCallback"
#ifndef XtCOrientation
#define XtCOrientation "Orientation"
#endif
#define XtCRealTickerDiv        "RealTickerDiv"
#define XtCWorldTickerDiv       "WorldTickerDiv"
#define XtCNumDivBetweenMajor   "NumDivBetweenMajor"
#define XtCMin                  "Min"
#define XtCPos                  "Pos"
#define Xcoda_SELEC                 TED    1

typedef struct{
  int    reason;
  XEvent *event;
  int    position;
}XcodaRulerCallbackStruct;

#define Xcoda_SELECTED 1
#endif RULER_H
