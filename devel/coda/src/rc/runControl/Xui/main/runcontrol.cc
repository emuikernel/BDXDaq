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
//      Main Part of RunControl
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: runcontrol.cc,v $
//   Revision 1.34  1998/11/09 17:01:42  timmer
//   Linux port
//
//   Revision 1.33  1998/11/05 20:12:23  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.32  1998/09/01 18:48:43  heyes
//   satisfy Randy's lust for command line options
//
//   Revision 1.31  1998/08/25 17:58:52  rwm
//   Drop reference to XmHTML_TopLevel. Don't start cedit & dbedit.
//
//   Revision 1.30  1998/06/18 12:20:44  heyes
//   new GUI ready I think
//
//   Revision 1.29  1998/06/02 19:51:38  heyes
//   fixed rcServer
//
//   Revision 1.28  1998/05/28 17:47:08  heyes
//   new GUI look
//
//   Revision 1.27  1998/04/08 18:31:35  heyes
//   new look and feel GUI
//
//   Revision 1.26  1998/01/23 15:27:36  heyes
//   commit LINUX changes for Carl
//
//   Revision 1.25  1997/10/20 12:45:54  heyes
//   first tag for B
//
//   Revision 1.24  1997/10/15 16:08:33  heyes
//   embed dbedit, ddmon and codaedit
//
//   Revision 1.23  1997/09/05 12:03:55  heyes
//   almost final
//
//   Revision 1.22  1997/08/25 16:00:38  heyes
//   fix some display problems
//
//   Revision 1.21  1997/08/20 18:38:31  heyes
//   fix up for SunOS
//
//   Revision 1.20  1997/08/18 13:36:38  heyes
//   add bg_pixmap1
//
//   Revision 1.19  1997/08/18 13:26:55  heyes
//   pixmap import
//
//   Revision 1.18  1997/08/01 18:38:19  heyes
//   nobody will believe this!
//
//   Revision 1.17  1997/07/30 15:32:29  heyes
//   clean for Solaris
//
//   Revision 1.16  1997/07/30 14:32:55  heyes
//   add more xpm support
//
//   Revision 1.15  1997/07/22 19:39:11  heyes
//   cleaned up lots of things
//
//   Revision 1.14  1997/07/18 16:54:49  heyes
//   new GUI
//
//   Revision 1.13  1997/07/11 14:07:53  heyes
//   almost working
//
//   Revision 1.11  1997/07/09 17:12:26  heyes
//   add rotated.h
//
//   Revision 1.10  1997/07/08 15:00:53  heyes
//   deep trouble
//
//   Revision 1.8  1997/06/16 13:22:55  heyes
//   clear graph
//
//   Revision 1.7  1997/06/16 12:26:53  heyes
//   add dbedit and so on
//
//   Revision 1.6  1997/06/14 17:29:41  heyes
//   new GUI
//
//   Revision 1.5  1997/06/06 18:51:33  heyes
//   new RC
//
//   Revision 1.3  1996/12/04 18:32:35  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.2  1996/10/14 20:13:42  chen
//   add display server messages preference
//
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//

#include <stdio.h>
#ifdef solaris
#include <libgen.h>
#endif
#include <string.h>
#include <assert.h>
#include <XcodaApp.h>
#include <rcClientHandler.h>
#include <rcTopW.h>
#include <rcComdOption.h>
#include <rcDbaseHandler.h>
#include <rcBackButton.h>
#include <rcHReload.h>
#include <rcHHome.h>

#ifdef Linux
#include <dlfcn.h>
#endif

#include <Xm/LabelG.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <Xm/MainW.h>
#include <Balloon.h>
/*#include <debug.h>*/
#include <cedit.h>

#ifdef USE_CREG
#include <codaRegistry.h>
#endif

int root_height;
XtAppContext app_context;
Widget toplevel;

char *userPath = "";
extern "C" void HTMLhelp(Widget w,char *src);
extern "C" int getStartupVisual(Widget shell, Visual **visual, int *depth,
	Colormap *colormap);
#if !defined(Linux) && !defined(Darwin)
extern "C" void bzero(void *,int);
#endif

#ifdef USE_CREG
extern "C" int     codaSendInit _ANSI_ARGS_((Widget w,char *name));
#endif

char *dollar_coda;

static char *fallback_res[] = 
{
  "runcontrol.*.fontList:                        -*-helvetica-medium-r-*-*-14-*-*-*-*-*-*-*",
  "runcontrol.*.menu_bar.*.fontList:             -*-helvetica-medium-r-*-*-14-*-*-*-*-*-*-*",
  "runcontrol*.rcMsgWindow.fontList:             -*-courier-medium-r-normal-*-12-*-*-*-*-*-*-*",
  "runcontrol.*.runInfoPanel*status.*.fontList:  -*-times-medium-i-*-*-14-*-*-*-*-*-*-*",
  "runcontrol*.runcinfo*.time*.fontList:         -*-times-medium-r-*-*-14-*-*-*-*-*-*-*",
  "runcontrol*.runcinfo*.fontList:               -*-times-medium-r-*-*-14-*-*-*-*-*-*-*",
  "runcontrol*.helpAboutDialog*.fontList:        -*-helvetica-medium-r-*-*-14-*-*-*-*-*-*-*",
  "runcontrol.*.dataLimitUnit.fontList:          -*-helvetica-medium-i-*-*-14-*-*-*-*-*-*-*",
  "runcontrol*.helpMsgWindow.fontList:           -*-helvetica-medium-r-*-*-14-*-*-*-*-*-*-*",
  "runcontrol*.connectDialogLabel.fontList:      -*-times-medium-i-*-*-14-*-*-*-*-*-*-*",
  "runcontrol*.compBootDialogLabel.fontList:     -*-times-medium-i-*-*-14-*-*-*-*-*-*-*",
  "runcontrol*.updateDiaLabel.fontList:          -*-times-medium-i-*-*-14-*-*-*-*-*-*-*",
  "runcontrol*.analogDialogLabel.fontList:       -*-times-medium-i-*-*-14-*-*-*-*-*-*-*",
  "runcontrol*.helpTextW.fontList:               -*-helvetica-medium-r-*-*-14-*-*-*-*-*-*-*",
  "runcontrol*.datafilename.fontList:            -*-helvetica-medium-r-*-*-14-*-*-*-*-*-*-*",
  "runcontrol*.OutFrame.*.fontList:              -*-helvetica-medium-r-*-*-12-*-*-*-*-*-*-*",
  "runcontrol*.dialogLabel.fontList:             -*-helvetica-medium-r-*-*-14-*-*-*-*-*-*-*",


  /*"runcontrol*.iEvRateG.foreground:              RoyalBlue4", does not do enything ..*/
  "runcontrol*.iEvRateG.fontList:                -*-times-bold-r-*-*-18-*-*-*-*-*-*-*",
  "runcontrol*.iDataRateG.fontList:              -*-times-bold-r-*-*-18-*-*-*-*-*-*-*",
  "runcontrol*.dEvRateG.fontList:                -*-times-bold-r-*-*-18-*-*-*-*-*-*-*",
  "runcontrol*.dDataRateG.fontList:              -*-times-bold-r-*-*-18-*-*-*-*-*-*-*",


  "runcontrol*.OutFrame.height:              200",
  "runcontrol*OutForm*sensitive:              False",
  "runcontrol*.Xmhelp.width:                      460",
  "runcontrol*.Xmhelp.height:                     550",

  "runcontrol*.*foreground:                      white",
  "runcontrol*.*background:                      gray20",
  "runcontrol*.rcMsgWindow.background:           lightGray",
  "runcontrol*.rcMsgWindow*foreground:           black",
  "runcontrol*.XmToggleButtonGadget.selectColor: yellow",
  "runcontrol*.XmToggleButton.selectColor:       yellow",
  "runcontrol*.connectDialog*.foreground:        white ",
  "runcontrol*.connectDialog*.background:        gray20",
  "runcontrol*.runTypeDialog*.foreground:        white",
  "runcontrol*.runTypeDialog*.background:        gray20",
  "runcontrol*.topShadowColor:                   gray",
  "runcontrol*.bottomShadowColor:                black",
  "runcontrol*.borderColor:                      gray25",
  "runcontrol* runstatusFrame*.borderColor:      blue",
  "runcontrol* runstatusFrame*.borderWidth:      2",
  "runcontrol*.list*shadowThickness:             2",
  "runcontrol*.list.borderWidth:                 4",
  "runcontrol.*.initInfoPanelForm.*.Hbar.*.background: lightGrey",
  "runcontrol.*.initInfoPanelForm.*.Vbar.*.background: lightGrey",
  "runcontrol.*.initInfoPanelForm.*.foreground:        white",
  "runcontrol.*.initInfoPanelForm.*.background:        black",
  
  "runcontrol.*.runInfoPanel.*.runPanelsessStatFrame.foreground: lightGrey",
  "runcontrol.*.runInfoPanel.*.runstatusLabel.fontList:  -*-times-bold-i-*-*-16-*-*-*-*-*-*-*",
  
  "runcontrol.*.runInfoPanel.*.runinfoLabel.foreground: lightGrey",
  "runcontrol.*.runInfoPanel.*.runinfoLabel.fontList:  -*-times-bold-i-*-*-16-*-*-*-*-*-*-*",
  
  "runcontrol.*.runInfoPanel.*.runsprogressLabel.foreground: lightGrey",
  "runcontrol.*.runInfoPanel.*.runsprogressLabel.fontList:  -*-times-bold-i-*-*-16-*-*-*-*-*-*-*",
  
  "runcontrol.*.runInfoPanel.*.datafn.topShadowColor:   red",
  "runcontrol.*.runInfoPanel.*.datafilename.foreground: red",
  "runcontrol.*.runInfoPanel.*.datafilename.background: lightGrey",
  "runcontrol.*.runInfoPanel.*.datafilename.fontList:  -*-times-bold-i-*-*-16-*-*-*-*-*-*-*",
  
  "runcontrol.*.runInfoPanel.*.evnbFrame2.topShadowColor: red",
  "runcontrol.*.runInfoPanel.*.evNumLabel.foreground: red",
  "runcontrol.*.runInfoPanel.*.evNumLabel.fontList:  -*-times-bold-r-*-*-16-*-*-*-*-*-*-*",
  
  "runcontrol.*.runInfoPanel*simpleInfoPanel.foreground:  blue",
  "runcontrol.*.runInfoPanel*limitframe.foreground:       red",
  "runcontrol.*.runInfoPanel*limitframe.topShadowColor:   red",
  "runcontrol.*.runInfoPanel*eventLimitFrame.foreground:  white",
  "runcontrol.*.runInfoPanel*dataLimitFrame.foreground:   white",
  
  "runcontrol.*.runInfoPanel.*.iEvDispFrame.topShadowColor: red",
  
  "runcontrol.*.runInfoPanel*runNumber.*.background:   lightGrey",
  "runcontrol.*.runInfoPanel*runNumber.*.foreground:   black",
  
  "runcontrol.*.runInfoPanel*runNumber.*.background:   lightGrey",
  "runcontrol.*.runInfoPanel*runNumber.*.foreground:   black",

  "runcontrol.*.runInfoPanel*database.*.background:    lightGrey",
  "runcontrol.*.runInfoPanel*exptname.*.background:    lightGrey",
  "runcontrol.*.runInfoPanel*runType.*.background:     lightGrey",
  "runcontrol.*.runInfoPanel*hostname.*.background:    lightGrey",
  "runcontrol.*.runInfoPanel*status.*.background:      lightGrey",
  "runcontrol.*.runInfoPanel*status.*.foreground:      black",
  "runcontrol.*.runInfoPanel*startTimeG.*.background:   lightGrey",
  "runcontrol.*.runInfoPanel*startTimeG.*.foreground:   black",
  "runcontrol.*.runInfoPanel*endTimeG.*.background:     lightGrey",
  "runcontrol.*.runInfoPanel*endTimeG.*.foreground:     black",
  "runcontrol.*.runInfoPanel*eventLimit.*.background:  lightGrey",
  "runcontrol.*.runInfoPanel*eventLimit.*.foreground:  black",
  "runcontrol.*.runInfoPanel*dataLimit.*.background:   lightGrey",
  "runcontrol.*.runInfoPanel*dataLimit.*.foreground:   black",
  "runcontrol.*.runInfoPanel*status.*.foreground:      black",
  "runcontrol.*.runInfoPanel*timeG.*.background:       lightGrey",
  "runcontrol.*.runInfoPanel*timeG.*.foreground:       black",
  "runcontrol.*.runInfoPanel*exptname.*.foreground:    RoyalBlue4",
  
  "runcontrol.*.runInfoPanel*runType.*.foreground:     red",
  "runcontrol.*.runInfoPanel*runType.*.fontList:       -*-times-bold-r-*-*-16-*-*-*-*-*-*-*",
  
  "runcontrol.*.runInfoPanel*exptid.*.foreground:      RoyalBlue4",
  "runcontrol.*.runInfoPanel*hostname.*.foreground:    RoyalBlue4",
  "runcontrol.*.runInfoPanel*database.*.foreground:    RoyalBlue4",
  "runcontrol.*.runInfoPanel*session.*.foreground:     RoyalBlue4",
  "runcontrol.*.runInfoPanel*cinfoSubForm.*.alignment: alignment_center",
  "runcontrol.*.runInfoPanel*eventNumber.*.background: lightGrey",
  "runcontrol.*.runInfoPanel*eventNumber.*.foreground: black",
  
  "runcontrol.*.evrateDisplay.background:     lightGrey",
  
  "runcontrol.*.datarateDisplay.background:     lightGrey",
  
  "runcontrol.*.ratioDisplay.background:     lightGrey",
  
  "runcontrol.*.otherDisplay.background:     lightGrey",
  "runcontrol.*.runInfoPanel*iEvRate.*.background:     lightGrey",
  "runcontrol.*.runInfoPanel*iEvRate.*.background:     lightGrey",
  "runcontrol.*.runInfoPanel*iEvRate.*.foreground:     black",
  "runcontrol.*.runInfoPanel*dEvRate.*.background:     lightGrey",
  "runcontrol.*.runInfoPanel*dEvRate.*.foreground:     black",
  "runcontrol.*.runInfoPanel*iDataRate.*.background:   lightGrey",
  "runcontrol.*.runInfoPanel*iDataRate.*.foreground:   black",
  "runcontrol.*.runInfoPanel*dDataRate.*.background:   lightGrey",
  "runcontrol.*.runInfoPanel*dDataRate.*.foreground:   black",
  "runcontrol.*.runInfoPanel.*background:              gray20",
  "runcontrol.*.runInfoPanel.*foreground:              white",
  "runcontrol.*.runInfoPanel*optionPulldown*foreground:white",
  "runcontrol.*.runInfoPanel*runtype*foreground:       white",
  "runcontrol.*.runInfoPanel*eventNumberG.*.background:lightGrey",
  "runcontrol.*.runInfoPanel*eventNumberG.foreground:  RoyalBlue4",
  "runcontrol.*.runInfoPanel*eventNumberG.fontList:       -*-times-bold-r-*-*-18-*-*-*-*-*-*-*",
  "runcontrol.*.runInfoPanel*eventNumberG.*borderWidth:1",
  "runcontrol.*.runInfoPanel*netstatus.*background:    daykGray",
  "runcontrol.*.scriptTab.tabcolor:                    gray20",
  "runcontrol.*.menu_bar.background:                   gray20",
  "runcontrol.*.menu_bar.*.foreground:                 white",
  "runcontrol.*.XmPushButton*highlightThickness:       0",
  "runcontrol.*.XmPushButtonGadget*highlightThickness: 0",
  "runcontrol.*.XmTextField*highlightThickness:        0",
  "runcontrol.*.XmLabel*highlightThickness:            0",
  "runcontrol.*.XmLabelGadget*highlightThickness:      0",
  "runcontrol.*.XmToggleButtonGadget*highlightThickness: 0  ",
  "runcontrol.*.XmToggleButton*highlightThickness:     0  ",
  "runcontrol.*.XmRowColumn*spacing:                   0",
  "runcontrol*.scale_red*troughColor:                  RoyalBlue4",
  "runcontrol*.scale_green*troughColor:                Green",
  "runcontrol*.scale_blue*troughColor:                 Blue",
  "runcontrol*.highlightThickness:                     0",
  "runcontrol*.XmRowColumn*spacing:                    0",
  "runcontrol*.selectColor:                            RoyalBlue4",
  "runcontrol*.scriptTab.shadowThickness:              2",
  "runcontrol*.scriptTab.tabWidthPercentage:           10",
  "runcontrol*.scriptTab.cornerwidth:                  2",
  "runcontrol*.scriptTab.cornerheight:                 2",
  "runcontrol*.scriptTab.textmargin:                   4",
  "runcontrol*.scriptTab.foreground:                   blue",
  "runcontrol*.scriptTab.tabcolor:                     lightGrey",
  
  "runcontrol*.top_ruler.background:                   lightGrey",
  "runcontrol*.left_ruler.background:                  lightGrey",
  "runcontrol*.top_ruler.foreground:                   White",
  "runcontrol*.left_ruler.foreground:                  White",
  "runcontrol*.top_ruler.tickerColor:                  White",
  "runcontrol*.left_ruler.tickerColor:                 White  ",
  "runcontrol*.edit_popup.*.background:                White",
  "runcontrol*.top_ruler.indicatorColor:               RoyalBlue4",
  "runcontrol*.left_ruler.indicatorColor:              RoyalBlue4",
  
  "runcontrol*icon_sw*background:                      lightGray",
  "runcontrol*icon_sw*foreground:                      black",
  
  "runcontrol*htext*background:                        lightGray",
  "runcontrol*htext*foreground:                        black",
  "runcontrol*anaLogFile.background:                   lightGrey",
  "runcontrol*anaLogFile.foreground:                   black",
  "runcontrol*anaLogFile.fontList:  -*-times-bold-i-*-*-16-*-*-*-*-*-*-*",

  "runcontrol.*.fileFr.topShadowColor:   red",
  "runcontrol.*.bootFr.topShadowColor:   red",
  "runcontrol.*.updFr.topShadowColor:   red",
  "runcontrol.*.monFr.topShadowColor:   red",

  "runcontrol*runInfoPanel*statuspanel.*.background:     lightGrey",
  "runcontrol*runNumber*backgroundPixmap:               XmUNSPECIFIED_PIXMAP",
  "runcontrol*startTime*backgroundPixmap:               XmUNSPECIFIED_PIXMAP",
  "runcontrol*endTime*backgroundPixmap:                 XmUNSPECIFIED_PIXMAP",
  "runcontrol*eventLimit*backgroundPixmap:              XmUNSPECIFIED_PIXMAP",
  "runcontrol*dataLimit*backgroundPixmap:               XmUNSPECIFIED_PIXMAP",
  "runcontrol*status*backgroundPixmap:                  XmUNSPECIFIED_PIXMAP",
  "runcontrol*time*backgroundPixmap:                    XmUNSPECIFIED_PIXMAP",
  "runcontrol*exptname*backgroundPixmap:                XmUNSPECIFIED_PIXMAP",
  "runcontrol*runType*backgroundPixmap:                 XmUNSPECIFIED_PIXMAP",
  "runcontrol*exptid*backgroundPixmap:                  XmUNSPECIFIED_PIXMAP",
  "runcontrol*hostname*backgroundPixmap:                XmUNSPECIFIED_PIXMAP",
  "runcontrol*database*backgroundPixmap:                XmUNSPECIFIED_PIXMAP",
  "runcontrol*session*backgroundPixmap:                 XmUNSPECIFIED_PIXMAP",
  "runcontrol*eventNumber*backgroundPixmap:             XmUNSPECIFIED_PIXMAP",
  "runcontrol*iDataRate*backgroundPixmap:               XmUNSPECIFIED_PIXMAP",
  "runcontrol*dDataRate*backgroundPixmap:               XmUNSPECIFIED_PIXMAP",
  "runcontrol*iEvRate*backgroundPixmap:                 XmUNSPECIFIED_PIXMAP",
  "runcontrol*dEvRate*backgroundPixmap:                 XmUNSPECIFIED_PIXMAP",
  "runcontrol*top_ruler*backgroundPixmap:               XmUNSPECIFIED_PIXMAP",
  "runcontrol*left_ruler*backgroundPixmap:              XmUNSPECIFIED_PIXMAP",
  "runcontrol*icon_sw*backgroundPixmap:                 XmUNSPECIFIED_PIXMAP",
  "runcontrol*htext*backgroundPixmap:                   XmUNSPECIFIED_PIXMAP",
  "runcontrol*initInfoPanelForm*backgroundPixmap:       XmUNSPECIFIED_PIXMAP",
  "runcontrol*eventNumberG*backgroundPixmap:            XmUNSPECIFIED_PIXMAP",
  "runcontrol*rcMsgWindow*backgroundPixmap:             XmUNSPECIFIED_PIXMAP",
  "runcontrol*rcRateDisplay*backgroundPixmap:           XmUNSPECIFIED_PIXMAP",
  "runcontrol*rcRateDisplay*foreground:                 black",
  "runcontrol*icon_sw*backgroundPixmap:                 XmUNSPECIFIED_PIXMAP",
  "runcontrol*icon_box*backgroundPixmap:                XmUNSPECIFIED_PIXMAP",
  "runcontrol*icon*backgroundPixmap:                    XmUNSPECIFIED_PIXMAP",
  
  "runcontrol.*.RbuttonFrame.topShadowColor:   red",
  "runcontrol.*.LbuttonFrame.topShadowColor:   blue",

  "runcontrol*HelpWidget*helpHtml*background:               lightGray",
  "runcontrol*HelpWidget*helpHtml*foreground:               black",
  "runcontrol*HelpWidget*fontList:		     -*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1",
  
  "runcontrol*balloonHelp*background:     yellow",
  "runcontrol*balloonHelp*foreground:     black",
  
  "runcontrol*balloonHelp*fontList:    -*-helvetica-medium-r-*-*-12-*-*-*-*-*-*-*",
  
  "runcontrol*balloonHelp2*background:     yellow",
  "runcontrol*balloonHelp2*foreground:     black",
  
  "runcontrol*balloonHelp2*fontList:    -*-helvetica-medium-r-*-*-12-*-*-*-*-*-*-*",
  
  "runcontrol*HelpWidget*maxImageColors: 0",
  "runcontrol*HelpWidget*file.labelString: File",
  "runcontrol*HelpWidget*fileMenu*open.labelString: Open File...",
  "runcontrol*HelpWidget*fileMenu*open.mnemonic: O",
  "runcontrol*HelpWidget*fileMenu*open.accelerator: Ctrl<Key>O",
  "runcontrol*HelpWidget*fileMenu*open.acceleratorText: Ctrl+O",
  "runcontrol*HelpWidget*fileMenu*saveas.labelString: Save File As...",
  "runcontrol*HelpWidget*fileMenu*saveas.mnemonic: S",
  "runcontrol*HelpWidget*fileMenu*reload.labelString: Reload File",
  "runcontrol*HelpWidget*fileMenu*reload.mnemonic: R",
  "runcontrol*HelpWidget*fileMenu*reload.accelerator: Ctrl<Key>R",
  "runcontrol*HelpWidget*fileMenu*reload.acceleratorText: Ctrl+R",
  "runcontrol*HelpWidget*fileMenu*quit.labelString: Exit",
  "runcontrol*HelpWidget*fileMenu*quit.mnemonic: x",
  "runcontrol*HelpWidget*fileMenu*quit.accelerator: Ctrl<Key>X",
  "runcontrol*HelpWidget*fileMenu*quit.acceleratorText: Ctrl+X",
  "runcontrol*HelpWidget*fileMenu*view.labelString: View",
  "runcontrol*HelpWidget*fileMenu*view.mnemonic: V",
  "runcontrol*HelpWidget*viewMenu*viewInfo.labelString: Document Info",
  "runcontrol*HelpWidget*viewMenu*viewInfo.mnemonic: I",
  "runcontrol*HelpWidget*viewMenu*viewSource.labelString: Document Source",
  "runcontrol*HelpWidget*viewMenu*viewSource.mnemonic: S",
  "runcontrol*HelpWidget*viewMenu*viewFonts.labelString: Font Cache Info",
  "runcontrol*HelpWidget*viewMenu*viewFonts.mnemonic: F",
  "runcontrol*HelpWidget*edit.labelString: Edit",
  "runcontrol*HelpWidget*editMenu*find.labelString: Find...",
  "runcontrol*HelpWidget*editMenu*find.mnemonic: F",
  "runcontrol*HelpWidget*editMenu*findAgain.labelString: Find Again",
  "runcontrol*HelpWidget*editMenu*findAgain.mnemonic: A",
  "runcontrol*HelpWidget*option.labelString: Options",
  "runcontrol*HelpWidget*optionMenu*anchorButtons.labelString: Buttoned Anchors",
  "runcontrol*HelpWidget*optionMenu*anchorButtons.mnemonic: B",
  "runcontrol*HelpWidget*optionMenu*highlightOnEnter.labelString: Highlight Anchors",
  "runcontrol*HelpWidget*optionMenu*highlightOnEnter.mnemonic: H",
  "runcontrol*HelpWidget*optionMenu*imageAnchorTracking.labelString: Track Image Anchors",
  "runcontrol*HelpWidget*optionMenu*imageAnchorTracking.mnemonic: I",
  "runcontrol*HelpWidget*optionMenu*anchorTips.labelString: Anchor tooltips",
  "runcontrol*HelpWidget*optionMenu*anchorTips.mnemonic: t",
  "runcontrol*HelpWidget*optionMenu*enableBodyColors.labelString: Body Colors",
  "runcontrol*HelpWidget*optionMenu*enableBodyColors.mnemonic: C",
  "runcontrol*HelpWidget*optionMenu*enableBodyImages.labelString: Body Image",
  "runcontrol*HelpWidget*optionMenu*enableBodyImages.mnemonic: o",
  "runcontrol*HelpWidget*optionMenu*enableDocumentColors.labelString: Allow Document Colors",
  "runcontrol*HelpWidget*optionMenu*enableDocumentColors.mnemonic: l",
  "runcontrol*HelpWidget*optionMenu*enableDocumentFonts.labelString: Allow Document Fonts",
  "runcontrol*HelpWidget*optionMenu*enableDocumentFonts.mnemonic: F",
  "runcontrol*HelpWidget*optionMenu*enableOutlining.labelString: Text Justification",
  "runcontrol*HelpWidget*optionMenu*enableOutlining.mnemonic: J",
  "runcontrol*HelpWidget*optionMenu*strictHTMLChecking.labelString: Strict HTML Checking ",
  "runcontrol*HelpWidget*optionMenu*strictHTMLChecking.mnemonic: S",
  "runcontrol*HelpWidget*optionMenu*warning.labelString: HTML Warnings",
  "runcontrol*HelpWidget*optionMenu*warning.mnemonic: W",
  "runcontrol*HelpWidget*optionMenu*freezeAnimations.labelString: Freeze Animations",
  "runcontrol*HelpWidget*optionMenu*freezeAnimations.mnemonic: r",
  "runcontrol*HelpWidget*optionMenu*imageEnable.labelString: Enable Image Support",
  "runcontrol*HelpWidget*optionMenu*imageEnable.mnemonic: E",
  "runcontrol*HelpWidget*optionMenu*autoImageLoad.labelString: Autoload Images",
  "runcontrol*HelpWidget*optionMenu*autoImageLoad.mnemonic: u",
  "runcontrol*HelpWidget*optionMenu*save.labelString: Save Options",
  "runcontrol*HelpWidget*optionMenu*save.mnemonic: v",
  "runcontrol*HelpWidget*warningMenu*none.labelString: Disable",
  "runcontrol*HelpWidget*warningMenu*none.mnemonic: D",
  "runcontrol*HelpWidget*warningMenu*all.labelString: Show All Warnings",
  "runcontrol*HelpWidget*warningMenu*all.mnemonic: A",
  "runcontrol*HelpWidget*warningMenu*unknownElement.labelString: Unknown HTML element",
  "runcontrol*HelpWidget*warningMenu*unknownElement.mnemonic: U",
  "runcontrol*HelpWidget*warningMenu*bad.labelString: Badly placed tags",
  "runcontrol*HelpWidget*warningMenu*bad.mnemonic: B",
  "runcontrol*HelpWidget*warningMenu*openBlock.labelString: Bad block aparture",
  "runcontrol*HelpWidget*warningMenu*openBlock.mnemonic: p",
  "runcontrol*HelpWidget*warningMenu*closeBlock.labelString: Bad block closure",
  "runcontrol*HelpWidget*warningMenu*closeBlock.mnemonic: c",
  "runcontrol*HelpWidget*warningMenu*openElement.labelString: Unbalanced Terminators",
  "runcontrol*HelpWidget*warningMenu*openElement.mnemonic: T",
  "runcontrol*HelpWidget*warningMenu*nested.labelString: Improper Nested Tags",
  "runcontrol*HelpWidget*warningMenu*nested.mnemonic: I",
  "runcontrol*HelpWidget*warningMenu*violation.labelString: HTML 3.2 Violations",
  "runcontrol*HelpWidget*warningMenu*violation.mnemonic: V",
  "runcontrol*HelpWidget*window.labelString: Window",
  "runcontrol*HelpWidget*windowMenu*lower.labelString: Lower Window",
  "runcontrol*HelpWidget*windowMenu*lower.mnemonic: L",
  "runcontrol*HelpWidget*windowMenu*raise.labelString: Raise Window",
  "runcontrol*HelpWidget*windowMenu*raise.mnemonic: R",
  "runcontrol*HelpWidget*help.labelString: Help",
  "runcontrol*HelpWidget*helpMenu*about.labelString: About XmHTML",
  "runcontrol*HelpWidget*helpMenu*about.mnemonic: A",
  NULL,
};

extern XcodaApp *theApplication;

int doTk = 0;
Display *MainDisplay;
rcMenuWindow *menW;

void messageHandler(char *message)
{
  
  switch (message[0]) {
  case 't':
    {
      char name[200];
      int pid;
      sscanf(&message[2],"%d %s",&pid,name);
      menW->createTabFrame(name,pid);
    }
    break;
  default:
    printf("unknown message : %s\n",message);
    
  }
}
void warningHandler(char *msg)
{
}

main (int argc, char** argv)
{
  static int once = 0;
  
  dollar_coda = getenv("CODA");
  if (dollar_coda == NULL) {
    printf ("$CODA must be set\n");
    exit(0);
  }
  doTk = 1;

  struct rlimit limits;
  getrlimit(RLIMIT_NOFILE,&limits);
  
  limits.rlim_cur = limits.rlim_max;

  setrlimit(RLIMIT_NOFILE,&limits);

  /*__rsd_selectDebugLevels("1");*/
  
  // set up command option
  
  rcComdOption::initOptionParser (argc, argv);
  rcComdOption* option = rcComdOption::option ();
  
  option->reportMsgOn ();
  option->parseOptions ();
  
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  // setup database connection handler
  (void)rcDbaseHandler::dbaseHandler ();
#endif
  
  XcodaApp* app = new XcodaApp (argv[0], fallback_res);
  // open a X display
  app->open (&argc, argv);
  
  
  // create a network handler which register itself to x window
  // event loop
  app_context = app->appContext ();
  
  rcClientHandler netHandler (app->appContext ());
  
  XtAppSetWarningHandler (app->appContext (), warningHandler);
  
  rcTopW* window  = new rcTopW ("RunControl", netHandler);
  
  app->initialize (&argc, argv);

  { 
    int x,y;
    unsigned int w,h,bw,dp,ac;
    Arg arg[20];
    Window root;
    extern Widget helpBalloon;
    char tmp[200];
    
    menW = window->window_;
    
    if (option->autostart_)
	{
	  menW->bootall_ = 1;
    }

    toplevel = XtParent(XtParent(XtParent(menW->rform_)));
    
    ac = 0;
    XtSetArg(arg[0], XmNpopdownDelay, 6000);
    XtSetArg(arg[1], XmNpopupDelay,   3000);
    helpBalloon = XmCreateBalloon(toplevel, "balloonHelp2", arg, 2);
#ifdef USE_CREG    
    codaSendInit(toplevel,"RUNCONTROL");
#endif
    MainDisplay = XtDisplay(toplevel);
    


	/* sergey: contents of 'help' on right side */
    XGetGeometry(
       XtDisplay(menW->tabChildren_[0]),
       XtWindow(menW->tabChildren_[0]),
       &root,&x,&y,&w,&h,&bw,&dp
    );
    
    {
      char  use_file[1000];
      char  *root = getenv("RC_HELP_ROOT");
      if (root)
      {
	    sprintf(use_file,"%s",root);
      }
      else
      {
	    char *dollar_coda;
	
	    dollar_coda = getenv ("CODA");
	
	    if (dollar_coda == NULL) return 0;
	
	    sprintf(use_file, "%s/common/html/rc/Notice.html", dollar_coda);
      }
      HTMLhelp(menW->tabChildren_[0],use_file);
    }





#ifdef USE_CREG
    CODASetAppName (XtDisplay(menW->tabChildren_[0]),XtWindow(menW->tabChildren_[0]),"RC_HTML_WINDOW");
#endif
    XStoreName(XtDisplay(menW->tabChildren_[0]),XtWindow(menW->tabChildren_[0]),"RC_HTML_WINDOW");
    
    menW->createTabFrame("cedit",0);
    menW->createTabFrame("dbedit",0);

    ac = 0;

    XtSetArg (arg[ac], XmNresizePolicy, XmRESIZE_ANY); ac++;

#ifdef USE_CREG
    codaSendInit(toplevel,"RUNCONTROL");  
    codaRegisterMsgCallback((void *)messageHandler);
#endif    

	if (!option->startWide_)
    {
      XResizeWindow(
              XtDisplay(XtParent(menW->rform_)),
              XtWindow(toplevel),
		      920, /*490*/ /*sergey: initial gui width*/
              1080 /*HeightOfScreen(XtScreen(menW->rform_))*/ /*sergey: initial gui height*/
      );
    }

    {
      char temp2[100],temp3[100];

      if (option->startCedit_)
      {
        if (option->noEdit_)
        {
          sprintf(temp2,
            "(echo \"start cedit\"; sleep 3; %s/codaedit -embed -noedit )&",
            getenv("CODA_BIN"));
        }
        else
        {
          sprintf(temp2,
            "(echo \"start cedit\"; sleep 3; %s/codaedit -embed )&",
            getenv("CODA_BIN"));
        }

        system(temp2);
      }


      if (option->startDbedit_)
      {
        sprintf (temp2,"(echo \"start dbedit\";sleep 4; %s/common/scripts/dbedit -embed )&",getenv("CODA"));
        system(temp2);
      }
    }

  }

  
  while (1)
  {
    if (theApplication != NULL)
      app->execute();
    
    if (theApplication == NULL) {
      return 0;
    }
  }
  

  return 0;
}
