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
//      RunControl Token Interval Dialog
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcTokenIDialog.cc,v $
//   Revision 1.2  1996/12/04 18:32:34  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)

#include <Xm/Scale.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/Form.h>

#include "rcTokenIButton.h"
#include "rcTokenIDialog.h"

rcTokenIDialog::rcTokenIDialog (rcTokenIButton* button,
				char* name,
				char* title,
				rcClientHandler& handler)
:XcodaIntPromptDialog (button->dialogBaseWidget (), name, title), 
 netHandler_ (handler), button_ (button)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcTokenIDialog Class Object\n");
#endif
  // empty
}

rcTokenIDialog::~rcTokenIDialog (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcTokenIDialog Class Object\n");
#endif
}

void
rcTokenIDialog::setTokenInterval (int sec)
{
  char temp[40];

  sprintf (temp, "%d", sec);
  setText (temp);
}

void
rcTokenIDialog::ok (void)
{
  int val = outputResult ();
  button_->sendTokenInterval (val);
  popdown ();
}

void
rcTokenIDialog::cancel (void)
{
  popdown ();
  //reset value
  setTokenInterval (button_->tokenInterval ());
}

int
rcTokenIDialog::checkSyntax (void)
{
  int value = 0;
  int st;

  if((st = sscanf(_result,"%d",&value)) < 1) 
    return 0;
  else {
    if (value >= 0)
      return 1;
    else
      return 0;
  }
} 

#endif
