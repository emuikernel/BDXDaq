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

#ifndef _StripFallback
#define _StripFallback

#include "StripDefines.h"

#if !defined (STRIP_APP_CLASS)
#define STRIP_APP_CLASS         "StripTool"
#endif

#define SA      STRIP_APP_CLASS

String  fallback_resources[] =
{
#ifdef STRIP_DEFINE_GUICOLORS
  SA "*background: Grey75",
  SA "*foreground: Black",
#endif
  SA "*font: *helvetica-medium-r-*-12*",
  SA "*fontList: *helvetica-medium-r-*-12*",
  SA "*graphPanel.shadowType: XmSHADOW_ETCHED_IN",
  SA "*StripGraph*graphPanel.shadowType: XmSHADOW_IN",
  SA "*StripGraph*graphPanel*shadowThickness: 1",
  SA "*StripGraph*graphPanel.controlsRowColumn*marginHeight: 0",
  SA "*StripGraph*graphPanel.controlsRowColumn*marginWidth: 0",
  SA "*HintShell.background: White",
  SA "*HintShell.foreground: Black",
  SA "*HintShell.cancelWaitPeriod: 200",
  SA "*HintShell.fontSet: *-helvetica-medium-r-normal--12-*",

  SA "*AnnotateDialog_popup.title: Enter Annotation Text",
  SA "*AnnotateDialog*okLabelString: OK",
  SA "*AnnotateDialog*cancelLabelString: Cancel",
  SA "*AnnotateDialog*helpLabelString: Help",

  SA "*AnnotateDialog*editText.topAttachment: XmATTACH_FORM",
  SA "*AnnotateDialog*editText.rightAttachment: XmATTACH_FORM",
  SA "*AnnotateDialog*editText.bottomAttachment: XmATTACH_FORM",
  SA "*AnnotateDialog*editText.leftAttachment: XmATTACH_WIDGET",
  SA "*AnnotateDialog*editText.leftWidget: textLabel",
  SA "*AnnotateDialog*editText.editMode: XmMULTI_LINE_EDIT",
  SA "*AnnotateDialog*editText.rows: 3",
  SA "*AnnotateDialog*editText.columns: 40",

  SA "*AnnotateDialog*textLabel.labelString: Text:",
  SA "*AnnotateDialog*textLabel.rightOffset: 3",
  SA "*AnnotateDialog*textLabel.topAttachment: XmATTACH_FORM",
  SA "*AnnotateDialog*textLabel.leftAttachment: XmATTACH_FORM",

  SA "*canvas*displayText.editable: False",
  SA "*canvas*displayText.rows: 3",
  SA "*canvas*displayText.columns: 40",
  SA "*canvas*displayText.wordWrap: True",

  SA "*PrinterDialog_popup.title: Printer Selector",
  SA "*PrinterDialog*okLabelString: OK",
  SA "*PrinterDialog*cancelLabelString: Cancel",
  SA "*PrinterDialog*nameLabel.labelString: Printer:",
  SA "*PrinterDialog*deviceLabel.labelString: Device:",
  SA "*PrinterDialog*deviceCombo.autoFillIn: False",
  SA "*PrinterDialog*deviceCombo.visibleItemCount: 5",
  SA "*PrinterDialog*deviceCombo.itemCount: 9",
  SA "*PrinterDialog*deviceCombo.items: la100,ljet,ln03,pjet,pjetxl,dj500,dj1200,pp,ps",

  SA "*PrinterDialog*nameTextF.topAttachment: XmATTACH_FORM",
  SA "*PrinterDialog*nameTextF.leftAttachment: XmATTACH_NONE",
  SA "*PrinterDialog*nameTextF.rightAttachment: XmATTACH_FORM",
  SA "*PrinterDialog*nameTextF.bottomAttachment: XmATTACH_NONE",

  SA "*PrinterDialog*deviceCombo.topAttachment: XmATTACH_WIDGET",
  SA "*PrinterDialog*deviceCombo.topWidget: nameTextF",
  SA "*PrinterDialog*deviceCombo.topOffset: 3",
  SA "*PrinterDialog*deviceCombo.leftAttachment: XmATTACH_OPPOSITE_WIDGET",
  SA "*PrinterDialog*deviceCombo.leftWidget: nameTextF",
  SA "*PrinterDialog*deviceCombo.rightAttachment: XmATTACH_OPPOSITE_WIDGET",
  SA "*PrinterDialog*deviceCombo.rightWidget: nameTextF",
  SA "*PrinterDialog*deviceCombo.bottomAttachment: XmATTACH_NONE",

  SA "*PrinterDialog*nameLabel.topAttachment: XmATTACH_OPPOSITE_WIDGET",
  SA "*PrinterDialog*nameLabel.topWidget: nameTextF",
  SA "*PrinterDialog*nameLabel.leftAttachment: XmATTACH_NONE",
  SA "*PrinterDialog*nameLabel.rightAttachment: XmATTACH_WIDGET",
  SA "*PrinterDialog*nameLabel.rightWidget: nameTextF",
  SA "*PrinterDialog*nameLabel.rightOffset: 3",
  SA "*PrinterDialog*nameLabel.bottomAttachment: XmATTACH_OPPOSITE_WIDGET",
  SA "*PrinterDialog*nameLabel.bottomWidget: nameTextF",

  SA "*PrinterDialog*deviceLabel.topAttachment: XmATTACH_OPPOSITE_WIDGET",
  SA "*PrinterDialog*deviceLabel.topWidget: deviceCombo",
  SA "*PrinterDialog*deviceLabel.leftAttachment: XmATTACH_NONE",
  SA "*PrinterDialog*deviceLabel.rightAttachment: XmATTACH_WIDGET",
  SA "*PrinterDialog*deviceLabel.rightWidget: deviceCombo",
  SA "*PrinterDialog*deviceLabel.rightOffset: 3",
  SA "*PrinterDialog*deviceLabel.bottomAttachment: XmATTACH_OPPOSITE_WIDGET",
  SA "*PrinterDialog*deviceLabel.bottomWidget: deviceCombo",
  
  SA "*ColorDialog*colorPaletteDrawingArea.height: 80",
  SA "*ColorDialog*redSlider.titleString: Red",
  SA "*ColorDialog*greenSlider.titleString: Green",
  SA "*ColorDialog*blueSlider.titleString: Blue",
  SA "*ColorDialog*cellStatusLabel*FontList: *-helvetica-medium-o-normal--10-*",
  SA "*ColorDialog*buttonRowColumn.orientation: XmHORIZONTAL",
  SA "*ColorDialog*buttonRowColumn.entryAlignment: XmALIGNMENT_CENTER",
  SA "*ColorDialog*buttonRowColumn.isAligned: True",
  SA "*ColorDialog*buttonRowColumn.packing: XmPACK_COLUMN",

  SA "*StripGraph.title: " STRIPGRAPH_TITLE,
  SA "*StripGraph.iconName: " STRIPGRAPH_ICON_NAME,
  SA "*StripDialog.title: " STRIPDIALOG_TITLE,
  SA "*StripDialog.iconName: " STRIPDIALOG_ICON_NAME,

  SA "*StripDialog*MenuBar*FontList: *helvetica-medium-r-*-12*",

  SA "*StripDialog*connectText.columns: 32",
  SA "*StripDialog*connectLabel.labelString: Plot New Signal: ",
  SA "*StripDialog*connectButton.labelString: Connect",

  SA "*StripDialog*tabs.tabLabels: Curves, Controls",
  SA "*StripDialog*tabs.tabShape: 0",
  SA "*StripDialog*tabs.fontList: *helvetica-medium-o-*-12*",
  SA "*StripDialog*tabs.shadowThickness: 1",
  
  SA "*StripDialog*curvePageForm.shadowThickness: 0",
  SA "*StripDialog*controlsPageForm.shadowThickness: 0",

  SA "*curvePageForm*nameRowLabel.labelString: Name",
  SA "*StripDialog*curvePageForm*colorRowLabel.labelString: Color",
  SA "*StripDialog*curvePageForm*plotRowLabel.labelString: Plot",
  SA "*StripDialog*curvePageForm*scaleRowLabel.labelString: Log10",
  SA "*StripDialog*curvePageForm*precisionRowLabel.labelString: Precision",
  SA "*StripDialog*curvePageForm*minRowLabel.labelString: Min",
  SA "*StripDialog*curvePageForm*maxRowLabel.labelString: Max",
  SA "*StripDialog*curvePageForm*modifyRowLabel.labelString: Modify",
  SA "*StripDialog*curvePageForm*removeRowLabel.labelString: Remove",
  
  SA "*StripDialog*curvePageForm*nameRowLabel.fontList: *helvetica-bold-r-normal--12*",
  SA "*StripDialog*curvePageForm*colorRowLabel.fontList: *helvetica-bold-r-normal--12*",
  SA "*StripDialog*curvePageForm*plotRowLabel.fontList: *helvetica-bold-r-normal--12*",
  SA "*StripDialog*curvePageForm*scaleRowLabel.fontList: *helvetica-bold-r-normal--12*",
  SA "*StripDialog*curvePageForm*precisionRowLabel.fontList: *helvetica-bold-r-normal--12*",
  SA "*StripDialog*curvePageForm*minRowLabel.fontList: *helvetica-bold-r-normal--12*",
  SA "*StripDialog*curvePageForm*maxRowLabel.fontList: *helvetica-bold-r-normal--12*",
  SA "*StripDialog*curvePageForm*modifyRowLabel.fontList: *helvetica-bold-r-normal--12*",
  SA "*StripDialog*curvePageForm*removeRowLabel.fontList: *helvetica-bold-r-normal--12*",


  SA "*StripDialog*curvePageForm*curveSeparator.shadowType: XmSHADOW_ETCHED_IN",
  SA "*StripDialog*curvePageForm*nameLabel.alignment: XmALIGNMENT_BEGINNING",
  SA "*StripDialog*curvePageForm*nameLabel.labelString: Some pretty long string",
  SA "*StripDialog*curvePageForm*colorPushButton.labelString: \\ \\ ",
  SA "*StripDialog*curvePageForm*plotToggle.labelString: ",
  SA "*StripDialog*curvePageForm*scaleToggle.labelString: ",
  SA "*StripDialog*curvePageForm*plotToggle.indicatorSize: 15",
  SA "*StripDialog*curvePageForm*scaleToggle.indicatorSize: 15",
  SA "*StripDialog*curvePageForm*precisionLabel.alignment: XmALIGNMENT_END",
  SA "*StripDialog*curvePageForm*precisionText.columns: 2",
  SA "*StripDialog*curvePageForm*minLabel.alignment: XmALIGNMENT_END",
  SA "*StripDialog*curvePageForm*minText.columns: 10",
  SA "*StripDialog*curvePageForm*maxLabel.alignment: XmALIGNMENT_END",
  SA "*StripDialog*curvePageForm*maxText.columns: 10",
  SA "*StripDialog*curvePageForm*removePushButton.labelString: Remove",
  SA "*StripDialog*optionFrame.title.fontList: *helvetica-bold-r-normal--12*",
  SA "*StripDialog*timeFrame.title.fontList: *helvetica-bold-r-normal--12*",
  
  SA "*StripDialog*appearanceForm*fgColorPushButton.labelString: \\ \\ ",
  SA "*StripDialog*appearanceForm*bgColorPushButton.labelString: \\ \\ ",
  SA "*StripDialog*appearanceForm*gridColorPushButton.labelString: \\ \\ ",
  
  SA "*StripDialog*appearanceForm*option0PushBG.labelString: None",
  SA "*StripDialog*appearanceForm*option1PushBG.labelString: Some",
  SA "*StripDialog*appearanceForm*option2PushBG.labelString: All",
  SA "*StripDialog*appearanceForm*OptionLabel.recomputeSize: False",
  SA "*StripDialog*appearanceForm*OptionLabel.width: 0",
  SA "*StripDialog*appearanceForm*OptionLabel.marginWidth: 0",
  SA "*StripDialog*appearanceForm*OptionLabel.marginLeft: 0",
  SA "*StripDialog*appearanceForm*OptionLabel.marginRight: 0",
  SA "*StripDialog*appearanceForm*xgridOptionMenu.marginWidth: 0",
  SA "*StripDialog*appearanceForm*xgridOptionMenu.marginHeight: 0",
  SA "*StripDialog*appearanceForm*xgridOptionMenu.spacing: 0",
  SA "*StripDialog*appearanceForm*ygridOptionMenu.marginWidth: 0",
  SA "*StripDialog*appearanceForm*ygridOptionMenu.marginHeight: 0",
  SA "*StripDialog*appearanceForm*ygridOptionMenu.spacing: 0",
  
  SA "*StripDialog*timeFrame.shadowType: XmSHADOW_ETCHED_IN",
  SA "*StripDialog*timeFrame.title.labelString: Time Controls",
  SA "*StripDialog*optionFrame.shadowType: XmSHADOW_ETCHED_IN",
  SA "*StripDialog*optionFrame.title.labelString: Graph Options",

  SA "*StripDialog*appMenubar*filePulldown*loadPushB.labelString: Load...",
  SA "*StripDialog*appMenubar*filePulldown*loadPushB.mnemonic: L",
  SA "*StripDialog*appMenubar*filePulldown*saveAsPushB.labelString: Save As...",
  SA "*StripDialog*appMenubar*filePulldown*saveAsPushB.mnemonic: A",
  SA "*StripDialog*appMenubar*filePulldown*savePushB.labelString: Save",
  SA "*StripDialog*appMenubar*filePulldown*savePushB.mnemonic: S",
  SA "*StripDialog*appMenubar*filePulldown*clearPushB.labelString: Clear",
  SA "*StripDialog*appMenubar*filePulldown*clearPushB.mnemonic: C",
  SA "*StripDialog*appMenubar*filePulldown*dismissPushB.labelString: Dismiss Window",
  SA "*StripDialog*appMenubar*filePulldown*dismissPushB.mnemonic: D",
  SA "*StripDialog*appMenubar*filePulldown*exitPushB.labelString: Exit Program",
  SA "*StripDialog*appMenubar*filePulldown*exitPushB.mnemonic: x",
  SA "*StripDialog*appMenubar*windowPulldown*window1PushB.labelString: Graph",
  SA "*StripDialog*appMenubar*windowPulldown*window1PushB.mnemonic: G",
  SA "*StripDialog*appMenubar*helpPulldown*webHelpPushB.labelString: Web Help...",
  SA "*StripDialog*appMenubar*helpPulldown*webHelpPushB.mnemonic: W",
  SA "*StripDialog*appMenubar*helpPulldown*helpOnHelpPushB.labelString: Help on Help...",
  SA "*StripDialog*appMenubar*helpPulldown*helpOnHelpPushB.mnemonic: H",
  SA "*StripDialog*appMenubar*helpPulldown*aboutPushB.labelString: About...",
  SA "*StripDialog*appMenubar*helpPulldown*aboutPushB.mnemonic: A",
 
  SA "*StripGraph.width: 792",    /* 550 Albert */
  SA "*StripGraph.height: 600",   /* 350 Albert */
  
  SA "*StripGraph*yAxis.direction: XjAXIS_LEFT",
  SA "*StripGraph*yAxis.valueType: XjAXIS_REAL",
  SA "*StripGraph*yAxis.font:   *helvetica-bold-r-normal--12*",

  SA "*StripGraph*xAxis.direction: XjAXIS_DOWN",
  SA "*StripGraph*xAxis.valueType: XjAXIS_RELTIME_NUMBERS",
  SA "*StripGraph*xAxis.usePixmap: True",
  SA "*StripGraph*xAxis.font: *helvetica-bold-r-normal--12*",

  SA "*StripGraph*messageLabel.labelString: Out of memory! Try reducing window size.",
  SA "*StripGraph*messageLabel.background: Black",
  SA "*StripGraph*messageLabel.foreground: White",

  SA "*StripGraph*locationLabel.recomputeSize: False",
  SA "*StripGraph*locationLabel.string: (12:34:56, 78.9012345)",
  SA "*StripGraph*locationLabel.alignment: XmALIGNMENT_BEGINNING",
  
  SA "*StripGraph*legend.font: *helvetica-bold-r-normal--12*",
  SA "*StripGraph*legend.usePixmap: True",
  
  SA "*StripGraph*titleLabel.fontList: *helvetica-bold-r-normal--12*",
  SA "*StripGraph*titleLabel.labelString: untitled",  

  0
};

#endif
