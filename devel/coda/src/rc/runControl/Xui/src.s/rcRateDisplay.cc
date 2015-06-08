//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
//       coda@cebaf.gov  Tel: (804) 249-7030     Fax: (804) 249-5800
//-----------------------------------------------------------------------------
//
// Description:
//      RunControl Client Event/Data Rate Display
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRateDisplay.cc,v $
//   Revision 1.9  2000/08/21 18:13:47  abbottd
//   Sun 5.0 C++ compiler fix
//
//   Revision 1.8  2000/01/21 15:15:26  rwm
//   Changed some labels
//
//   Revision 1.7  1999/02/17 18:07:17  rwm
//   New var name so we don't cover the previous one.
//
//   Revision 1.6  1998/11/24 13:57:54  heyes
//   check in for Carl...
//
//   Revision 1.5  1998/11/05 20:12:18  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.4  1998/05/28 17:47:02  heyes
//   new GUI look
//
//   Revision 1.3  1998/04/08 18:31:26  heyes
//   new look and feel GUI
//
//   Revision 1.2  1996/12/04 18:32:32  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#include <stdio.h>
#include <math.h>
#include <Xm/Xm.h>
#include <Xm/PushBG.h>
#include <Xm/LabelG.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/SeparatoG.h>
#include <SciPlot.h>
#include "rcRateDisplay.h"

Widget OutFrame = (Widget) NULL;
Widget OutLabel = (Widget) NULL;
Widget outForm = (Widget) NULL;
Widget fileFr = (Widget) NULL;
Widget fileLabel = (Widget) NULL;
Widget tokenFr = (Widget) NULL;
Widget tokenLabel = (Widget) NULL;
Widget bootFr = (Widget) NULL;
Widget bootLabel = (Widget) NULL;
Widget updFr = (Widget) NULL;
Widget updLabel = (Widget) NULL;
Widget monFr = (Widget) NULL;
Widget monLabel = (Widget) NULL;

rcRateDisplay::rcRateDisplay (Widget parent, rcClientHandler& handler, Widget status, char* name, char* title,
			      int buffersize)
  :XcodaUi (name), parent_ (parent),netHandler_ (handler),statusPanel_ (status), title_(title),etime_ (0), dtime_ (0), size_ (buffersize)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Create rcRateDisplay Class Object\n");
#endif
  evx_ = new double[size_];
  evy_ = new double[size_];
  dx_ = new double[size_];
  dy_ = new double[size_];
  rx_ = new double[size_];
  ry_ = new double[size_];
  lvx_ = new double[size_]; //sergey
  lvy_ = new double[size_]; //sergey

  //etime_ = dtime_ = time(0);

  /*sergey: update rate defined here ???*/
  /* it is overloaded in rcRunDInfoPanel.cc ... */
  updateRate_ = 2;

  for (int i = 0; i < size_; i++)
  {
    evx_[i] = i;
    evy_[i] = 0.0;
    dx_[i] = i;
    dy_[i] = 0.0;
    rx_[i] = i;
    ry_[i] = 0.0;
    lvx_[i] = i; //sergey
    lvy_[i] = 0.0; //sergey
  }
  num_ = 0;

  //sergey
  time0 = time1 = time(0);
  evy_average = 0.0;
  ibin0 = ibin1 = 0;

}

rcRateDisplay::~rcRateDisplay (void)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Delete rcRateDisplay Class Object\n");
#endif
  delete []evx_;
  delete []evy_;
  delete []dx_;
  delete []dy_;
  delete []rx_;
  delete []ry_;
  delete []lvx_; //sergey
  delete []lvy_; //sergey
}

void
rcRateDisplay::init (rcMenuWindow *menW)
{
  Arg arg[30];
  int ac = 0;

  _w = XtCreateManagedWidget ("rateForm", xmFormWidgetClass, parent_, arg, ac);

  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  Widget topForm = XtCreateManagedWidget ( "topform",xmFormWidgetClass,_w, arg, ac );

  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNtopPosition, 49); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNbottomPosition, 51); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  XtSetArg (arg[ac], XmNorientation, XmHORIZONTAL); ac++;

  Widget sep = XtCreateManagedWidget ("sep", xmSeparatorGadgetClass,
				      topForm, arg, ac);


  /* create event rate display */
  ac = 0;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_IN); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 5); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 5); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 49); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNbottomPosition, 49); ac++;
  // plot widget specific
  XtSetArg (arg[ac], XtNshowLegend, FALSE); ac++;
  XtSetArg (arg[ac], XtNxAxisNumbers, FALSE); ac++;
  XtSetArg (arg[ac], XtNshowTitle, FALSE); ac++;
  XtSetArg (arg[ac], XtNdrawMinor, False); ac++;
  XtSetArg (arg[ac], XtNxAutoScale, False); ac++;
  XtSetArg (arg[ac], XtNmonochrome, False); ac++;
  XtSetArg (arg[ac], XtNxLabel, "Time (Sec)"); ac++;
  XtSetArg (arg[ac], XtNyLabel, "Event Rate (Hz)"); ac++;
  evrate_ = XtCreateManagedWidget ("evrateDisplay",
 				   sciplotWidgetClass, topForm,
 				   arg, ac);

   
  /* create data rate display */
  ac = 0;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_IN); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 5); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 51); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 5); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNbottomPosition, 49); ac++;
  // plot widget specific
  XtSetArg (arg[ac], XtNxAxisNumbers, FALSE); ac++;
  XtSetArg (arg[ac], XtNshowLegend, FALSE); ac++;
  XtSetArg (arg[ac], XtNshowTitle, FALSE); ac++;
  XtSetArg (arg[ac], XtNdrawMinor, False); ac++;
  XtSetArg (arg[ac], XtNxAutoScale, False); ac++;
  XtSetArg (arg[ac], XtNmonochrome, False); ac++;
  XtSetArg (arg[ac], XtNxLabel, "Time (Sec)"); ac++;
  XtSetArg (arg[ac], XtNyLabel, "Data Rate (KB/Sec)"); ac++;
  datarate_ = XtCreateManagedWidget ("datarateDisplay",
				     sciplotWidgetClass, topForm,
				     arg, ac);


  /* create event size (ratio) display */
  ac = 0;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_IN); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNtopPosition, 51); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNbottomPosition,100); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 5); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 49); ac++;
  // plot widget specific
  XtSetArg (arg[ac], XtNxAxisNumbers, FALSE); ac++;
  XtSetArg (arg[ac], XtNshowLegend, FALSE); ac++;
  XtSetArg (arg[ac], XtNshowTitle, FALSE); ac++;
  XtSetArg (arg[ac], XtNdrawMinor, False); ac++;
  XtSetArg (arg[ac], XtNxAutoScale, False); ac++;
  XtSetArg (arg[ac], XtNmonochrome, False); ac++;
  XtSetArg (arg[ac], XtNxLabel, "Time (Sec)"); ac++;
  XtSetArg (arg[ac], XtNyLabel, "Event Size (Bytes)"); ac++;
  ratio_ = XtCreateManagedWidget ("ratioDisplay",
 				   sciplotWidgetClass, topForm,
 				   arg, ac);

   
  /* create livetime display */
  ac = 0;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_IN); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNtopPosition, 51); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNbottomPosition,100); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 51); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 5); ac++;
  // plot widget specific
  XtSetArg (arg[ac], XtNxAxisNumbers, FALSE); ac++;
  XtSetArg (arg[ac], XtNshowLegend, FALSE); ac++;
  XtSetArg (arg[ac], XtNshowTitle, FALSE); ac++;
  XtSetArg (arg[ac], XtNdrawMinor, False); ac++;
  XtSetArg (arg[ac], XtNxAutoScale, False); ac++;
  XtSetArg (arg[ac], XtNmonochrome, False); ac++;
  XtSetArg (arg[ac], XtNxLabel, "Time (Sec)"); ac++;
  XtSetArg (arg[ac], XtNyLabel, "Livetime (percent)"); ac++;
  other_ = XtCreateManagedWidget ("otherDisplay",
				     sciplotWidgetClass, topForm,
				     arg, ac);


  /* Create event rate display for main panel */
  ac = 0;
  XtSetArg (arg[ac], XtNlabelFont,(XtFONT_TIMES | 14) ); ac++;
  XtSetArg (arg[ac], XtNxAxisNumbers, FALSE); ac++;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_IN); ac++;
  XtSetArg (arg[ac], XtNshowLegend, FALSE); ac++;
  XtSetArg (arg[ac], XtNshowTitle, FALSE); ac++;
  XtSetArg (arg[ac], XtNdrawMinor, False); ac++;
  XtSetArg (arg[ac], XtNxAutoScale, False); ac++;
  XtSetArg (arg[ac], XtNmonochrome, False); ac++;
  XtSetArg (arg[ac], XtNxLabel, "Time (Sec)"); ac++;
  XtSetArg (arg[ac], XtNyLabel, "Events/Sec"); ac++;
  rates_ = XtCreateManagedWidget ("ratesDisplay",
				     sciplotWidgetClass, statusPanel_,
				     arg, ac);


  Widget _wid = menW->createTabFrame("Options",0);
  Widget children[5];      /* Children to manage */
  Arg al[64];                    /* Arg List */
  XrmValue from_value, to_value; /* For resource conversion */
  XmString xmstrings[16];    /* temporary storage for XmStrings */
  Widget form4 = (Widget)NULL;
  Widget frame9 = (Widget)NULL;
  Widget label7 = (Widget)NULL;
  Display *display = XtDisplay(_wid);

  if (DefaultDepthOfScreen(DefaultScreenOfDisplay(XtDisplay(_wid))) != 1)
  {
    from_value.addr = "yellow";
    from_value.size = strlen( from_value.addr ) + 1;
    to_value.addr = NULL;
    XtConvertAndStore (_wid, XmRString, &from_value, XmRPixel, &to_value);
    if ( to_value.addr )
      {
	XtSetArg(al[ac], XmNborderColor, *(unsigned int *)to_value.addr); ac++;
      }
  }

  ac = 0;
  XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (al[ac], XmNtopOffset, -400); ac++;
  XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (al[ac], XmNbottomOffset, 2); ac++;
  XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (al[ac], XmNleftOffset, 2); ac++;
  XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (al[ac], XmNrightOffset, 2); ac++;
  OutFrame = XmCreateFrame ( _wid, "OutFrame", al, ac );
  ac = 0;
  xmstrings[0] = XmStringCreateLtoR ( "Run Control Options", (XmStringCharSet)XmFONTLIST_DEFAULT_TAG );
  XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
  OutLabel = XmCreateLabel ( OutFrame, "OutLabel", al, ac );
  XmStringFree ( xmstrings [ 0 ] );

  outForm = XtCreateManagedWidget ("outForm", xmFormWidgetClass, OutFrame, arg, ac);
  // File name frame
  ac = 0;
  XtSetArg (al[ac], XmNheight, 60); ac++;
  fileFr = XmCreateFrame ( outForm, "fileFr", al, ac );
  ac = 0;
  xmstrings[0] = XmStringCreateLtoR ( "File name", (XmStringCharSet)XmFONTLIST_DEFAULT_TAG );
  XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
  fileLabel = XmCreateLabel ( fileFr, "fileLabel", al, ac );
  XmStringFree ( xmstrings [ 0 ] );
  ac = 0;
  /*
  tokenFr = XmCreateFrame ( outForm, "tokenFr", al, ac );
  ac = 0;
  xmstrings[0] = XmStringCreateLtoR ( "Token Interval", (XmStringCharSet)XmFONTLIST_DEFAULT_TAG );
  XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
  tokenLabel = XmCreateLabel ( tokenFr, "fileLabel", al, ac );
  XmStringFree ( xmstrings [ 0 ] );
  ac = 0;
  */
  bootFr = XmCreateFrame ( outForm, "bootFr", al, ac );
  ac = 0;
  xmstrings[0] = XmStringCreateLtoR ( "Auto boot control", (XmStringCharSet)XmFONTLIST_DEFAULT_TAG );
  XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
  bootLabel = XmCreateLabel ( bootFr, "bootLabel", al, ac );
  XmStringFree ( xmstrings [ 0 ] );
  ac = 0;
  updFr = XmCreateFrame ( outForm, "updFr", al, ac );
  ac = 0;
  xmstrings[0] = XmStringCreateLtoR ( "Server poll rate", (XmStringCharSet)XmFONTLIST_DEFAULT_TAG );
  XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
  updLabel = XmCreateLabel ( updFr, "updLabel", al, ac );
  XmStringFree ( xmstrings [ 0 ] );
  ac = 0;
  monFr = XmCreateFrame ( outForm, "monFr", al, ac );
  ac = 0;
  xmstrings[0] = XmStringCreateLtoR ( "Component monitoring", (XmStringCharSet)XmFONTLIST_DEFAULT_TAG );
  XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
  monLabel = XmCreateLabel ( monFr, "monLabel", al, ac );
  XmStringFree ( xmstrings [ 0 ] );
  ac = 0;
  XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(al[ac], XmNtopOffset, 1); ac++;
  XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(al[ac], XmNbottomPosition, 13); ac++;
  XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(al[ac], XmNleftOffset, 1); ac++;
  XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(al[ac], XmNrightOffset, 1); ac++;
  XtSetValues ( fileFr,al, ac );
  /*
    ac = 0;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(al[ac], XmNtopOffset, 1); ac++;
    XtSetArg(al[ac], XmNtopWidget, fileFr); ac++;
    XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
    XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNleftOffset, 1); ac++;
    XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNrightOffset, 1); ac++;
    XtSetValues ( tokenFr,al, ac );
  */
  ac = 0;
  XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(al[ac], XmNtopWidget, fileFr); ac++;
  XtSetArg(al[ac], XmNtopOffset, 4); ac++;
  //XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
  //XtSetArg(al[ac], XmNbottomPosition, 50); ac++;
  XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(al[ac], XmNleftOffset, 1); ac++;
  XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(al[ac], XmNrightOffset, 1); ac++;
  XtSetValues ( bootFr,al, ac );
  ac = 0;
  XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(al[ac], XmNtopOffset, 4); ac++;
  XtSetArg(al[ac], XmNtopWidget, bootFr); ac++;
  // XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
  // XtSetArg(al[ac], XmNbottomPosition, 58); ac++;
  XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(al[ac], XmNleftOffset, 1); ac++;
  XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(al[ac], XmNrightOffset, 1); ac++;
  XtSetValues ( updFr,al, ac );
  ac = 0;
  XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(al[ac], XmNtopOffset, 4); ac++;
  XtSetArg(al[ac], XmNtopWidget, updFr); ac++;
  //XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  //XtSetArg(al[ac], XmNbottomOffset, 1); ac++;
  XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(al[ac], XmNleftOffset, 1); ac++;
  XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(al[ac], XmNrightOffset, 1); ac++;
  XtSetValues ( monFr,al, ac );
  ac = 0;
  children[ac++] = fileFr;
  //children[ac++] = tokenFr;
  children[ac++] = bootFr;
  children[ac++] = updFr;
  children[ac++] = monFr;
  XtManageChildren(children, ac);
  ac = 0;
  children[ac++] = fileLabel;
  XtManageChildren(children, ac);
  ac = 0;
  children[ac++] = tokenLabel;
  XtManageChildren(children, ac);
  ac = 0;
  children[ac++] = bootLabel;
  XtManageChildren(children, ac);
  ac = 0;
  children[ac++] = updLabel;
  XtManageChildren(children, ac);
  ac = 0;
  children[ac++] = monLabel;
  XtManageChildren(children, ac);
  ac = 0;
  children[ac++] = OutLabel;
  children[ac++] = outForm;
  XtManageChildren(children, ac);
  ac = 0;
  XtManageChild ( OutFrame);
  
  ac = 0;
  

  /* create plotes */
  evlist_ = SciPlotListCreateFromDouble (evrate_, 0, evx_, evy_, "evRate");
  datalist_ = SciPlotListCreateFromDouble (datarate_, 0, dx_, dy_, "dataRate");
  lvlist_ = SciPlotListCreateFromDouble (other_, 0, lvx_, lvy_, "livetime"); //sergey
  ratiolist_ = SciPlotListCreateFromDouble (ratio_, 0, rx_, ry_, "ratio");
  revlist_ = SciPlotListCreateFromDouble (rates_, 0, evx_, evy_, "evRate");


  /* set X scale */
  SciPlotSetXUserScale (evrate_, 0.0, (float)size_ * updateRate_);
  SciPlotSetXUserScale (datarate_, 0.0, (float)size_ * updateRate_);
  SciPlotSetXUserScale (ratio_, 0.0, (float)size_ * updateRate_);
  SciPlotSetXUserScale (other_, 0.0, (float)size_ * updateRate_); //sergey
  SciPlotSetXUserScale (rates_, 0.0, (float)size_ * updateRate_);

  /* set Y scale (sergey) */
  SciPlotSetYUserScale (evrate_, 0.0, 12000.);
  SciPlotSetYUserScale (datarate_, 0.0, 50000.);
  SciPlotSetYUserScale (ratio_, 0.0, 10000.);
  SciPlotSetYUserScale (other_, 0.0, 110.0);
  SciPlotSetYUserScale (rates_, 0.0, 12000.);


  /* set colors */
  int evc = SciPlotAllocNamedColor (evrate_, "Red");
  int dc  = SciPlotAllocNamedColor (datarate_, "Blue");
  int rc  = SciPlotAllocNamedColor (ratio_, "Green");
  int lvc = SciPlotAllocNamedColor (other_, "Green"); //sergey
  int revc = SciPlotAllocNamedColor (rates_, "Red");


  /* set line style */
  SciPlotListSetStyle (evrate_, evlist_, evc, XtMARKER_CIRCLE, evc, XtLINE_SOLID);
  SciPlotListSetStyle (datarate_, datalist_, dc, XtMARKER_SQUARE, dc, XtLINE_SOLID);
  SciPlotListSetStyle (ratio_, ratiolist_, rc, XtMARKER_SQUARE, rc, XtLINE_SOLID);
  SciPlotListSetStyle (other_, lvlist_, lvc, XtMARKER_SQUARE, lvc, XtLINE_SOLID);  //sergey
  SciPlotListSetStyle (rates_, revlist_, revc, XtMARKER_DOT, evc, XtLINE_SOLID);
}

void
rcRateDisplay::dismissCallback (Widget w, XtPointer data,
				XmAnyCallbackStruct* cbs)
{
  rcRateDisplay* obj = (rcRateDisplay *)data;
}

/* set update rate */
void
rcRateDisplay::setUpdateRate (int data)
{
  Arg arg[30];
  char xLabel[1000];
  int ac = 0;

  printf("setUpdateRate reached, requested update interval is %d sec\n",data);
  updateRate_ = data;

  resetDisplay ();
  ac = 0;
  sprintf(xLabel,"%d Sec. update",updateRate_);
  XtSetArg (arg[ac], XtNxLabel, xLabel); ac++;

  XtSetValues(datarate_, arg, ac);
  XtSetValues(evrate_, arg, ac);
  XtSetValues(ratio_, arg, ac);
  XtSetValues(other_, arg, ac); //sergey
  XtSetValues(rates_, arg, ac);

  SciPlotUpdate(datarate_);
  SciPlotUpdate(evrate_); 
  SciPlotUpdate(ratio_);
  SciPlotUpdate(other_); //sergey
  SciPlotUpdate(rates_);
  
  printf("setUpdateRate finished\n");
}


/* event rate historgam update (for both histograms ?) */
void
rcRateDisplay::addEvRateData (double data)
{
  int jj;
  double evyI[1000];

  printf("rcRateDisplay::addEvRateData: reached, data=%f(%d)\n",
    data,updateRate_);

  if(nume_ < size_)
  {
    int i;
    evy_[nume_] = data;
    
    evyI[nume_] = 0.0;

    for(i=0; i<=nume_; i++) evyI[nume_] += evy_[i];
    evyI[nume_] = evyI[nume_] / (nume_ + 1); 
    printf("rcRateDisplay::addEvRateData: event rate is %f %f\n", evy_[nume_], evyI[nume_]);
    nume_++;
  }
  else
  {
    printf("rcRateDisplay::addEvRateData: ???\n");

    int begin, i, j;
    begin = size_ / 2;
    nume_ = size_/2;
    etime_ ++;
    for(i=0; i<nume_; i++)
    {
      evy_[i] = evy_[i + begin];
      evyI[i] = evyI[i + begin];
    }
    evy_[i] = data;
    evyI[i] = 0.0;

    for(j=i-(size_/2); j<=i; j++) evyI[i] += evy_[j];

    evyI[i] /= (size_/2 + 1);
 
    nume_++;

    // change x coordinates
    for(i=0; i<size_; i++) evx_[i] = (etime_*size_/2 + i) * updateRate_;

    SciPlotSetXUserScale(evrate_, (float)(evx_[1]), 
     			  (float)(evx_[1] + size_ * updateRate_));

    SciPlotSetXUserScale(rates_, (float)(evx_[1]), 
			  (float) (evx_[1] + size_ * updateRate_));
  }







  /* sergey: following call must be made with 'updateRate_' intervals,
	 and during that interval 'evy_' value must be averaged ???!!!*/


  time1 = time(0);
  if(time1 < (time0+updateRate_))
  {
    evy_average += data;
  }
  else
  {
    ibin1 = nume_ - 1;
    evy_average /= (time1-time0);
    printf("update !!!!!!! avg=%f (dev=%u)\n",evy_average,(time1-time0));
    for(jj=ibin0; jj<=ibin1; jj++) evy_[jj] = evy_average;

    time0 = time1;
    evy_average = 0.0;
    ibin0 = nume_ - 1;

    /* sergey: differential hist */
    SciPlotListUpdateFromDouble(evrate_, evlist_, nume_, evx_, evy_);
  }
  

  printf("time0=%u, time1=%u\n",time0,time1);









  /* sergey: integrated hist */
  SciPlotListUpdateFromDouble(rates_,  revlist_, nume_, evx_, evyI);

  if(SciPlotQuickUpdate(evrate_)) SciPlotUpdate(evrate_);
  if(SciPlotQuickUpdate(rates_)) SciPlotUpdate(rates_);

  addRatioData(0.0);
}




/* data rate historgam update */
void
rcRateDisplay::addDataRateData (double data)
{
  int i;

  if (numd_ < size_) {
    dy_[numd_++] = data;
  } else {
    double begin = ceil ((double)size_/2.0);
    numd_ = size_/2;
    dtime_ ++;
    for (i = 0; i < numd_; i++) 
      dy_[i] = dy_[i + (int)begin];
    dy_[i] = data;
    numd_++;
    // change x coordinates
    for (i = 0; i < size_; i++) {
      dx_[i] = (dtime_*size_/2 + i) * updateRate_;
    }
    SciPlotSetXUserScale (datarate_, (float)(dx_[0]),
			  (float)(dx_[size_ - 1]));
  }

  SciPlotListUpdateFromDouble(datarate_, datalist_, numd_, dx_, dy_);

  if (SciPlotQuickUpdate(datarate_)) 
    SciPlotUpdate(datarate_);

  addRatioData(0.0);

}

/*sergey*/
/* livetime historgam update */
void
rcRateDisplay::addLivetimeData (double data)
{
  int i;

  if (numl_ < size_) {
    lvy_[numl_++] = data;
  } else {
    double begin = ceil ((double)size_/2.0);
    numl_ = size_/2;
    dtime_ ++;
    for (i = 0; i < numl_; i++) 
      lvy_[i] = lvy_[i + (int)begin];
    lvy_[i] = data;
    numl_++;
    // change x coordinates
    for (i = 0; i < size_; i++) {
      lvx_[i] = (dtime_*size_/2 + i) * updateRate_;
    }
    SciPlotSetXUserScale (other_, (float)(lvx_[0]),
			  (float)(lvx_[size_ - 1]));
  }

  SciPlotListUpdateFromDouble(other_, lvlist_, numl_, lvx_, lvy_);

  if (SciPlotQuickUpdate(other_)) 
    SciPlotUpdate(other_);

  addRatioData(0.0);

}

/* data size (ratio) historgam update */
void
rcRateDisplay::addRatioData (double data)
{
  int i;

  //printf("rcRateDisplay::addRatioData: %f\n",data);

  if(nume_ != numd_) return;

  if(numr_ < size_)
  {
    ry_[numr_] = 1000.0 * dy_[numr_]/evy_[numr_];
    numr_++;
  }
  else
  {
    double begin = ceil ((double)size_/2.0);
    numr_ = size_/2;
    rtime_ ++;
    for (i = 0; i < numr_; i++) 
      ry_[i] = ry_[i + (int)begin];
    ry_[i] = 1000.0 * dy_[nume_]/evy_[nume_];
    numr_++;
    // change x coordinates
    for(i = 0; i < size_; i++)
    {
      rx_[i] = (rtime_*size_/2 + i) * updateRate_;
    }
    SciPlotSetXUserScale (ratio_, (float)(rx_[0]),
			  (float) (rx_[size_ - 1]));
  }
  SciPlotListUpdateFromDouble(ratio_, ratiolist_, numr_, rx_, ry_);
  if (SciPlotQuickUpdate(ratio_)) 
  {
    SciPlotUpdate(ratio_);
  }
}

void
rcRateDisplay::resetDisplay (void)
{
  int i;
  numd_ = 0;
  numl_ = 0; //sergey
  nume_ = 0;
  numr_ = 0;
  dtime_ = 0;
  etime_ = 0;
  rtime_ = 0;

  for (i = 0; i < size_; i++) {
    evx_[i] = i * updateRate_;
    evy_[i] = 0.0;
    dx_[i] = i * updateRate_;
    dy_[i] = 0.0;
    rx_[i] = i * updateRate_;
    ry_[i] = 0.0;
    lvx_[i] = i * updateRate_;
    lvy_[i] = 0.0;
  }
  num_ = 0;

  SciPlotListUpdateFromDouble(datarate_, datalist_, numd_, dx_, dy_);  
  SciPlotListUpdateFromDouble(evrate_, evlist_, nume_, evx_, evy_);
  SciPlotListUpdateFromDouble(ratio_, ratiolist_, numr_, rx_, ry_);
  SciPlotListUpdateFromDouble(other_, lvlist_, numl_, lvx_, lvy_); //sergey
  SciPlotListUpdateFromDouble(rates_, revlist_, numd_, dx_, dy_);  

  SciPlotSetXUserScale (datarate_, 0.0, (float)size_ * updateRate_);
  SciPlotSetXUserScale (evrate_, 0.0, (float)size_ * updateRate_);
  SciPlotSetXUserScale (ratio_, 0.0, (float)size_ * updateRate_);
  SciPlotSetXUserScale (other_, 0.0, (float)size_ * updateRate_); //sergey
  SciPlotSetXUserScale (rates_, 0.0, (float)size_ * updateRate_);

}

void
rcRateDisplay::cleanDisplay (void)
{
  resetDisplay ();

  SciPlotUpdate(datarate_);
  SciPlotUpdate(evrate_); 
  SciPlotUpdate(ratio_);
  SciPlotUpdate(other_); //sergey
  SciPlotUpdate(rates_);
 
  // add monitor on callbacks for active components
  
  rcClient& client = netHandler_.clientHandler ();

  client.monitorOffCallback (client.exptname (), "components",
				 (rcCallback)&(rcRateDisplay::compCallback),
				 (void *)this,
				 (rcCallback)&(rcRateDisplay::offCallback),
				 (void *)this);

  client.monitorOnCallback (client.exptname (), "components",
			    (rcCallback)&(rcRateDisplay::compCallback),
			    (void *)this);
  
}  

void
rcRateDisplay::manage ()
{

}

void
rcRateDisplay::unmanage ()
{

}

void
rcRateDisplay::offCallback (int status, void* oarg, daqNetData* data)
{
}

void
rcRateDisplay::compCallback (int status, void* oarg, daqNetData* data)
{

}

void
rcRateDisplay::popup (void)
{
  resetDisplay ();
  SciPlotUpdate(datarate_);
  SciPlotUpdate(evrate_); 
  SciPlotUpdate(ratio_);
  SciPlotUpdate(other_); //sergey
  SciPlotUpdate(rates_);
}

void
rcRateDisplay::sendChanged (char *what,int newval)
{
}

void
rcRateDisplay::sendMonitorInfo (daqMonitorStruct* info)
{
}

void
rcRateDisplay::changedValueCallback (Widget w, XtPointer data, XmScaleCallbackStruct *cb)
{
}

void
rcRateDisplay::setCallback (int status, void* arg, daqNetData* )
{

}
