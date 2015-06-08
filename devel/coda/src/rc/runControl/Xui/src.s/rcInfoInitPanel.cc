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
//      Implementation of RunControl Initial Panel
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcInfoInitPanel.cc,v $
//   Revision 1.8  1998/05/28 17:46:57  heyes
//   new GUI look
//
//   Revision 1.7  1998/04/08 18:31:19  heyes
//   new look and feel GUI
//
//   Revision 1.6  1997/07/30 14:32:51  heyes
//   add more xpm support
//
//   Revision 1.5  1997/07/22 19:38:57  heyes
//   cleaned up lots of things
//
//   Revision 1.4  1997/07/18 16:54:44  heyes
//   new GUI
//
//   Revision 1.3  1997/06/06 18:51:24  heyes
//   new RC
//
//   Revision 1.2  1997/05/26 12:27:44  heyes
//   embed tk in RC GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#include <stdio.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/Frame.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <rcComdOption.h>
#include <rcAudioOutput.h>
#include "rcInfoInitPanel.h"
#include "XcodaXpm.h"
#include <XmHTML.h>

double rcInfoInitPanel::xmin = 0.0;
double rcInfoInitPanel::ymin = 0.0;
double rcInfoInitPanel::xmax = 100.0;
double rcInfoInitPanel::ymax = 100.0;

double rcInfoInitPanel::vxmin = 0.0;
double rcInfoInitPanel::vymin = 0.0;
double rcInfoInitPanel::vxmax = 1.0;
double rcInfoInitPanel::vymax = 1.0;

static char   path[1024];

static XmImageInfo*
loadImage(Widget w, String url)
{
	char filename[1024];
	XmImageInfo *image = NULL;
	int i;

	if (path[0]) {
	  sprintf(filename,"%s/%s",path,url);
	} else {
	  sprintf(filename,"./%s",url);
	}
	image = XmHTMLImageDefaultProc(w, filename, NULL, 0);
	return(image);
}

static void
anchorCB(Widget widget, XtPointer client_data, 
	XmHTMLAnchorCallbackStruct *cbs)
{
	cbs->doit = True;
	cbs->visited = True;
}

static String
loadFile(String filename)
{
	FILE *file;
	int size;
	static String content;

	/* open the given file */
	if((file = fopen(filename, "r")) == NULL)
	{
		perror(filename);
		return(NULL);
	}

	int ix;
	for (ix=1023;ix>=0;ix--)
    {
	  path[ix] = 0;
	}
	sprintf(path,"%s",filename);

	for (ix=1023;ix>=0;ix--)
    {
	  if (path[ix] == '/')
	    break;
	}

	if (ix >=0) path[ix] = 0;

	/* see how large this file is */
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	rewind(file);

	/* allocate a buffer large enough to contain the entire file */
	if((content = (char *) ::malloc(size)) == NULL)
	{
		fprintf(stderr, "malloc failed for %i bytes\n", size);
		exit(EXIT_FAILURE);
	}

	/* now read the contents of this file */
	if((fread(content, 1, size, file)) != size)
		printf("Warning: did not read entire file!\n");

	fclose(file);
	return(content);
}

rcInfoInitPanel::rcInfoInitPanel (Widget parent, char* name,
				  Dimension width, Dimension height)
:armed_ (0), autoTimerId_ (0), rotate_ (0), counter_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rcInfoInitPanel Class Object\n");
#endif
  // check with the global option class to find out rotation flag
  rcComdOption* option = rcComdOption::option ();
  //if (option->showAnimation ())
  //  rotate_ = 1;
  
  Arg arg[20];
  int ac = 0;

  // get application context
  appContext_ = XtWidgetToApplicationContext (parent);

  /* sergey: not here ..?? */
  // Create Frame Widget
  XtSetArg (arg[ac], XmNwidth, width); ac++;
  XtSetArg (arg[ac], XmNheight, height); ac++;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_IN); ac++;
  frame_ = XtCreateWidget ("initInfoPanelForm", xmFrameWidgetClass,
  			   parent, arg, ac);

  {
    char	  help_file[1000];
    
    extern char *dollar_coda;
    String content;

    sprintf(help_file,"%s/common/html/rc/Start.html",dollar_coda);

	/* create the HTML widget using the default resources */
    Widget html = XtVaCreateManagedWidget("html",
					  xmHTMLWidgetClass, frame_,
					  XmNimageProc, loadImage,
					  XmNmarginWidth, 20,
					  XmNmarginHeight, 20,
					  XmNwidth, 600,
					  XmNheight, 500,
					  NULL);

	content = loadFile(help_file);

	if(content == NULL)
		XmHTMLTextSetString(html, "<html><body>Could not read given "
			"file</body></html>");
	else
	{
		XmHTMLTextSetString(html, content);
		free(content);
	}
	
	/* add a simple anchor callback so XmHTML can jump to local anchors */
	XtAddCallback(html, XmNactivateCallback, 
		(XtCallbackProc)anchorCB, NULL);


  }
}

rcInfoInitPanel::~rcInfoInitPanel (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete rcInfoInitPanel Class Object\n");
#endif
}

const Widget
rcInfoInitPanel::baseWidget (void)
{
  return frame_;
}

void
rcInfoInitPanel::manage (void)
{
  XtManageChild (frame_);

  //start ();
}

void
rcInfoInitPanel::unmanage (void)
{
  //stop ();
  XtUnmanageChild (frame_);
}

double
rcInfoInitPanel::createRotatingImages (double ybeginning)
{
  return 0.0;
}

void
rcInfoInitPanel::start (void)
{
  return;
}

void
rcInfoInitPanel::timerCallback (XtPointer clientData, XtIntervalId *)
{
}

void
rcInfoInitPanel::stop (void)
{
  return;
}

void
rcInfoInitPanel::disableRotatingLog (void)
{
}


    
