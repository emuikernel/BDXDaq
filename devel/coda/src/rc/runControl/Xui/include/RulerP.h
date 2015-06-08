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
 * private header for Ruler widget
 *	
 * Author:  Jie Chen and Chip Watson, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: RulerP.h,v $
 *   Revision 1.1  1997/07/30 14:33:34  heyes
 *   add more xpm support
 *
 *   Revision 1.1.1.1  1996/08/21 19:36:08  heyes
 *   Imported sources
 *
 */
#ifndef RULERP_H
#define RULERP_H

#define MAXSEGMENTS 1000  /*max segments for the ruler*/

typedef struct _XcodaRulerClassPart{
  int      ignore;
}XcodaRulerClassPart;

typedef struct _XcodaRulerClassRec{
  CoreClassPart     core_class;
  XcodaRulerClassPart  ruler_class;
}XcodaRulerClassRec;

extern XcodaRulerClassRec xcodaRulerClassRec;

typedef struct _XcodaRulerPart{
  Pixel         indicatorColor;  /*color of the indicator*/
  Pixel         tickerColor;      /*color of the tic_markers*/
  int           orientation;      /*orientation of the ruler*/
  int           minimum;
  int           realTickerDiv;        /*distance between tickers in pixel*/
  int           worldTickerDiv;       /*distance between tickers in world*/
  int           numDivBetweenMajor; /*num of div bettwen major tickers*/
/*internal strucure*/
  XFontStruct   *font;
  char          label[MAXSEGMENTS][20];
  Cardinal      label_num;
  XPoint        label_xy[MAXSEGMENTS];
/*others*/
  int           maximum;
  int           numTicker;        /*number of tic_markers*/
  int           realNumTicker;    /*real number of ticker, above use can sel*/
  Dimension     marker_length;    /*marker length*/
  Dimension     marker_width;     /*marker top width*/
  int           position;         /*marker position relative to world coord*/
  Position      marker_x;
  Position      marker_y;         /*x,y coords for marker tip*/
  GC            marker_GC;
  GC            tic_GC;
  GC            xor_GC;
  XSegment      segments[MAXSEGMENTS];
  XtCallbackList valueChanged;   /*callback list*/
}XcodaRulerPart;

typedef struct _XcodaRulerRec{
  CorePart        core;
  XcodaRulerPart     ruler;
}XcodaRulerRec;

#endif RULERP_H
  
