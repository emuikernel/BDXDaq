#ifdef USE_TK

/* 
 * tkXAccess.c --
 *
 *      This module implements functionality required for sending
 *      X events to windows and do other Xlib stuff.
 *
 * Copyright (c) 1995 by Sven Delmas
 * All rights reserved.
 * See the file COPYRIGHT for the copyright notes.
 *
 */

#ifndef lint
static char *AtFSid = "$Header: /net/mizar/usr/local/source/coda_source/runControl/Xui/tkXAccess.c,v 1.3 1997/06/12 10:44:50 heyes Exp $";
#endif /* not lint */

#include "tkPort.h"
#include "tk.h"
#include "./tkXAccess.h"

/*
  I do not know why, but several window managers require multiple
  reparent events. Right now a value of 25 should be enough.
  */
#define REPARENT_LOOPS 25

/* If this variable is 1, X errors will not stop the */
/* continuation of the xaccess command. Otherwise a */
/* standard X error is created. */
static int continueXAccess;

/* This variable contains the cursor that is used */
/* when interactively picking a color from the screen. */
static Cursor tksteal_cursor = (Cursor) NULL;

/* This struct contains the list of predefined */
/* (supported) window properties for the property */
/* manipulation. */
typedef struct {
  char *name;
  Atom type;
} AtomList;

static AtomList propertyTable[] = {
    {"ARC",             XA_ARC},
    {"ATOM",            XA_ATOM},
    {"BITMAP",          XA_BITMAP},
    {"CARDINAL",        XA_CARDINAL},
    {"COLORMAP",        XA_COLORMAP},
    {"CURSOR",          XA_CURSOR},
    {"DRAWABLE",        XA_DRAWABLE},
    {"FONT",            XA_FONT},
    {"INTEGER",         XA_INTEGER},
    {"PIXMAP",          XA_PIXMAP},
    {"POINT",           XA_POINT},
    {"RECTANGLE",       XA_RECTANGLE},
    {"RGB_COLOR_MAP",   XA_RGB_COLOR_MAP},
    {"STRING",          XA_STRING},
    {"WINDOW",          XA_WINDOW},
    {"VISUALID",        XA_VISUALID},
    {"WM_COMMAND",      XA_WM_COMMAND},
    {"WM_HINTS",        XA_WM_HINTS},
    {"WM_ICON_SIZE",    XA_WM_ICON_SIZE},
    {"WM_SIZE_HINTS",   XA_WM_SIZE_HINTS},
    {"",                0},
};

/*
 * external functions used by TkSteal.
 */

/*
 *----------------------------------------------------------------------
 *
 * LocalXErrorHandler --
 *
 *      This procedure prevents X errors, by replacing the
 *      standard X error handler.
 *
 * Results:
 *      0
 *
 * Side effects:
 *      X errors are ignored.
 *
 *----------------------------------------------------------------------
 */
static int
LocalXErrorHandler(clientData, errorPtr)
     ClientData clientData;
     XErrorEvent *errorPtr;
{
  continueXAccess = 0;
  /*
  fprintf(stderr, "X error: errcode %d request %d minor %d<\n",
          errorPtr->error_code, errorPtr->request_code,
          errorPtr->minor_code);
  */
  return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * TkXAccessMatchModifierState --
 *
 *      This procedure matches a string to the corresponding
 *      modifier state.
 *
 * Results:
 *      The state of the modifier.
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */
static int
TkXAccessMatchModifierState(name)
     char *name;
{
  size_t length = 0;

  length = strlen(name);
  if ((strncmp(name, "Any", length) == 0) &&
      (length >= 2)) {
    return(0);
  } else if ((strncmp(name, "Shift", length) == 0) &&
             (length >= 2)) {
    return(ShiftMask);
  } else if ((strncmp(name, "Lock", length) == 0) &&
             (length >= 2)) {
    return(LockMask);
  } else if ((strncmp(name, "Control", length) == 0) &&
             (length >= 2)) {
    return(ControlMask);
  } else if ((strncmp(name, "Meta", length) == 0) &&
             (length == 4)) {
    return(Mod1Mask);
  } else if ((strncmp(name, "Mod1", length) == 0) &&
             (length >= 4)) {
    return(Mod1Mask);
  } else if ((strncmp(name, "Mod2", length) == 0) &&
             (length >= 4)) {
    return(Mod2Mask);
  } else if ((strncmp(name, "Mod3", length) == 0) &&
             (length >= 4)) {
    return(Mod3Mask);
  } else if ((strncmp(name, "Mod4", length) == 0) &&
             (length >= 4)) {
    return(Mod4Mask);
  } else if ((strncmp(name, "Mod5", length) == 0) &&
             (length >= 4)) {
    return(Mod5Mask);
  } else if ((strncmp(name, "Mod2", length) == 0) &&
             (length >= 4)) {
    return(Mod2Mask);
  } else if ((strncmp(name, "Button1", length) == 0) &&
             (length >= 7)) {
    return(Button1Mask);
  } else if ((strncmp(name, "Button2", length) == 0) &&
             (length >= 7)) {
    return(Button2Mask);
  } else if ((strncmp(name, "Button3", length) == 0) &&
             (length >= 7)) {
    return(Button3Mask);
  } else if ((strncmp(name, "Button4", length) == 0) &&
             (length >= 7)) {
    return(Button4Mask);
  } else if ((strncmp(name, "Button5", length) == 0) &&
             (length >= 7)) {
    return(Button5Mask);
  }
  return -1;
}

/*
 *----------------------------------------------------------------------
 *
 * TkXAccessCmd --
 *
 *      This procedure gives access to various X-lib calls.
 *
 * Results:
 *      Returns a standard Tcl completion code, and leaves an error
 *      message in interp->result if an error occurs.
 *
 * Side effects:
 *      Depends on the performed action.
 *
 *----------------------------------------------------------------------
 */
int
TkXAccessCmd(clientData, interp, argc, argv)
     ClientData clientData;     /* Main window associated with
                                 * interpreter. */
     Tcl_Interp *interp;        /* Current interpreter. */
     int argc;                  /* Number of arguments. */
     char **argv;               /* Argument strings. */
{
  char c;
  char *interval = (char *) NULL;
  char *property_return = (char *) NULL;
  char *win_name = (char *) NULL;
  char *windowname = (char *) NULL;
  char buf[30];
  char error_buf[80];
  
  int actual_format = 0;
  int border_width = 0;
  int counter1 = 0;
  int counter2 = 0;
  int counter3 = 0;
  int counter4 = 0;
  int dataindex = 0;
  int datalen = 0;
  int delete = 0;
  int format = 8;
  int grabResult = 0;
  int height = 0;
  int keysymsPerKeyCode = 0;
  int maxKeyCode = 0;
  int minKeyCode = 0;
  int mode = PropModeReplace;
  int num_elements = 0;
  int override_redirect = True;
  int relative = 0;
  int width = 0;
  int x = -1002;
  int x_ret = -1002;
  int x_root = -1002;
  int x_root_ret = -1002;
  int y = -1002;
  int y_ret = -1002;
  int y_root = -1002;
  int y_root_ret = -1002;

  unsigned int button = 0;
  unsigned int keycode = 0;
  unsigned int mask_ret = 0;
  unsigned int modeModifierMask = 0;
  unsigned int num_child1 = 0;
  unsigned int num_child2 = 0;
  unsigned int num_child3 = 0;
  unsigned int permanentState = 0;
  unsigned int state = 0;
  unsigned int tmpKeycode = 0;

  long interval_value = 0L;
  long long_length = 100000L;
  long long_offset = 0L;
  long retries = 10000L;

  unsigned long bytes_after = 0L;
  unsigned long lcounter1 = 0L;
  unsigned long lcounter2 = 0L;
  unsigned long long_num_elements = 0L;
  
  double interval_steps = 0;
  double x_tmp = 0;
  double y_tmp = 0;

  size_t length = 0;
  size_t paramLength = 0;

  struct timeval time;
  struct timeval startTime;

  Window above = (Window) None;
  Window child_ret = (Window) None;
  Window parent = (Window) None;
  Window parent_ret = (Window) None;
  Window root_ret = (Window) None;
  Window root_window = (Window) None;
  Window window = (Window) None;
  Window stop_window = (Window) None;
  Window *child_list1 = (Window *) None;
  Window *child_list2 = (Window *) None;
  Window *child_list3 = (Window *) None;
  
  Atom actual_type = (Atom) NULL;
  Atom message_type = (Atom) NULL;
  Atom property  = (Atom) NULL;
  Atom req_type = (Atom) XA_STRING;
  Atom type = (Atom) XA_STRING;
  Atom *atomList = (Atom *) NULL;

  KeySym *mainKeyMap;
  KeySym keySym;

  KeyCode *keyCode;
  
  XColor color;

  XWindowAttributes winAttributes;

  XImage *image;

  XModifierKeymap *modifierMap;
  
  XEvent xevent;
  XButtonEvent *xbutton;
  XClientMessageEvent *xclient;
  XConfigureEvent *xconfig;
  XExposeEvent *xexpose;
  XKeyEvent *xkey;
  XFocusInEvent *xfocusin;
  XFocusOutEvent *xfocusout;

  Tk_Window abovetkwin = (Tk_Window) NULL;
  Tk_Window parenttkwin = (Tk_Window) NULL;
  Tk_Window tkrootwin = (Tk_Window) clientData;
  Tk_Window tkwin = (Tk_Window) NULL;

  Tk_ErrorHandler handler1;
  Tk_ErrorHandler handler2;
  Tk_ErrorHandler handler3;
    
  if (argc < 2) {
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, "wrong # args: should be \"",
                     argv[0], " command ?arg arg....?\"",
                     (char *) NULL);
    return TCL_ERROR;
  }

  continueXAccess = 1;
  /* Parse the arguments. */
  while (counter1 < argc) {
    length = strlen(argv[counter1]);
    if ((strncmp(argv[counter1], "-abovewidget", length) == 0) &&
        (counter1 < argc) && (length >= 9)) {
      counter1++;
      if ((abovetkwin = Tk_NameToWindow(interp, argv[counter1],
                                        tkrootwin)) != (Tk_Window) NULL) {
        above = Tk_WindowId(abovetkwin);
      }
    } else if ((strncmp(argv[counter1], "-abovewindowid", length) == 0) &&
               (counter1 < argc) && (length >= 9)) {
      counter1++;
      above = strtoul(argv[counter1], (char **) NULL, 0);
    } else if ((strncmp(argv[counter1], "-borderwidth", length) == 0) &&
               (counter1 < argc) && (length >= 3)) {
      counter1++;
      border_width = atoi(argv[counter1]);
    } else if ((strncmp(argv[counter1], "-button", length) == 0) &&
               (counter1 < argc) && (length >= 4)) {
      counter1++;
      paramLength = strlen(argv[counter1]);
      if ((strncmp(argv[counter1], "1", paramLength) == 0) &&
          (paramLength == 1)) {
        button = Button1;
      } else if ((strncmp(argv[counter1], "2", paramLength) == 0) &&
          (paramLength == 1)) {
        button = Button2;
      } else if ((strncmp(argv[counter1], "3", paramLength) == 0) &&
          (paramLength == 1)) {
        button = Button3;
      } else if ((strncmp(argv[counter1], "4", paramLength) == 0) &&
          (paramLength == 1)) {
        button = Button4;
      } else if ((strncmp(argv[counter1], "5", paramLength) == 0) &&
          (paramLength == 1)) {
        button = Button5;
      }
    } else if ((strncmp(argv[counter1], "-data", length) == 0) &&
               (counter1 < argc) && (length >= 3)) {
      counter1++;
      dataindex = counter1;
      datalen = strlen(argv[dataindex]);
    } else if ((strncmp(argv[counter1], "-delete", length) == 0) &&
               (counter1 < argc) && (length >= 3)) {
      counter1++;
      delete = atoi(argv[counter1]);
    } else if ((strncmp(argv[counter1], "-format", length) == 0) &&
               (counter1 < argc) && (length >= 3)) {
      counter1++;
      format = atoi(argv[counter1]);
    } else if ((strncmp(argv[counter1], "-height", length) == 0) &&
               (counter1 < argc) && (length >= 3)) {
      counter1++;
      height = atoi(argv[counter1]);
    } else if ((strncmp(argv[counter1], "-interval", length) == 0) &&
               (counter1 < argc) && (length >= 3)) {
      counter1++;
      interval = argv[counter1];
    } else if ((strncmp(argv[counter1], "-keycode", length) == 0) &&
               (counter1 < argc) && (length >= 4)) {
      counter1++;
      keycode = XKeysymToKeycode(Tk_Display(tkrootwin),
                                 XStringToKeysym(argv[counter1]));
    } else if ((strncmp(argv[counter1], "-longlength", length) == 0) &&
               (counter1 < argc) && (length >= 6)) {
      counter1++;
      long_length = strtoul(argv[counter1], (char **) NULL, 0);
    } else if ((strncmp(argv[counter1], "-longoffset", length) == 0) &&
               (counter1 < argc) && (length >= 6)) {
      counter1++;
      long_offset = strtoul(argv[counter1], (char **) NULL, 0);
    } else if ((strncmp(argv[counter1], "-messagetype", length) == 0) &&
               (counter1 < argc) && (length >= 12)) {
      counter1++;
      message_type = strtoul(argv[counter1], (char **) NULL, 0);
    } else if ((strncmp(argv[counter1], "-messagetypename", length) == 0) &&
               (counter1 < argc) && (length >= 13)) {
      counter1++;
      message_type = Tk_InternAtom(tkrootwin, argv[counter1]);
    } else if ((strncmp(argv[counter1], "-numelements", length) == 0) &&
               (counter1 < argc) && (length >= 3)) {
      counter1++;
      num_elements = atoi(argv[counter1]);
    } else if ((strncmp(argv[counter1], "-overrideredirect", length) == 0) &&
               (counter1 < argc) && (length >= 3)) {
      counter1++;
      override_redirect = atoi(argv[counter1]);
    } else if ((strncmp(argv[counter1], "-parentwidget", length) == 0) &&
               (counter1 < argc) && (length >= 10)) {
      counter1++;
      if ((parenttkwin = Tk_NameToWindow(interp, argv[counter1],
                                         tkrootwin)) != (Tk_Window) NULL) {
        parent = Tk_WindowId(parenttkwin);
      }
    } else if ((strncmp(argv[counter1], "-parentwindowid", length) == 0) &&
               (counter1 < argc) && (length >= 10)) {
      counter1++;
      parent = strtoul(argv[counter1], (char **) NULL, 0);
    } else if ((strncmp(argv[counter1], "-property", length) == 0) &&
               (counter1 < argc) && (length >= 9)) {
      counter1++;
      property = strtoul(argv[counter1], (char **) NULL, 0);
    } else if ((strncmp(argv[counter1], "-propertyname", length) == 0) &&
               (counter1 < argc) && (length >= 10)) {
      counter1++;
      property = Tk_InternAtom(tkrootwin, argv[counter1]);
    } else if ((strncmp(argv[counter1], "-propmode", length) == 0) &&
               (counter1 < argc) && (length >= 6)) {
      counter1++;
      if (strcmp(argv[counter1], "replace") == 0) {
        mode = PropModeReplace;
      } else if (strcmp(argv[counter1], "prepend") == 0) {
        mode = PropModePrepend;
      } else if (strcmp(argv[counter1], "append") == 0) {
        mode = PropModeAppend;
      }
    } else if ((strncmp(argv[counter1], "-proptype", length) == 0) &&
               (counter1 < argc) && (length >= 6)) {
      counter1++;
      lcounter1 = 0;
      while (1) {
        if (strcmp(propertyTable[lcounter1].name, "") == 0) {
          break;
        }
        if (strcmp(propertyTable[lcounter1].name, argv[counter1]) == 0) {
          type = propertyTable[lcounter1].type;
          req_type = propertyTable[lcounter1].type;
          break;
        }
        lcounter1++;
      }
    } else if ((strncmp(argv[counter1], "-relative", length) == 0) &&
               (counter1 < argc) && (length >= 3)) {
      counter1++;
      relative = atoi(argv[counter1]);
    } else if ((strncmp(argv[counter1], "-retries", length) == 0) &&
               (counter1 < argc) && (length >= 3)) {
      counter1++;
      retries = strtoul(argv[counter1], (char **) NULL, 0);
    } else if ((strncmp(argv[counter1], "-state", length) == 0) &&
               (counter1 < argc) && (length >= 4)) {
      counter1++;
      if (TkXAccessMatchModifierState(argv[counter1]) != -1) {
        state |= TkXAccessMatchModifierState(argv[counter1]);
      }
    } else if ((strncmp(argv[counter1], "-widget", length) == 0) &&
               (counter1 < argc) && (length >= 4)) {
      counter1++;
      if ((tkwin = Tk_NameToWindow(interp, argv[counter1],
                                   tkrootwin)) != (Tk_Window) NULL) {
        window = Tk_WindowId(tkwin);
      }
    } else if ((strncmp(argv[counter1], "-width", length) == 0) &&
               (counter1 < argc) && (length >= 3)) {
      counter1++;
      width = atoi(argv[counter1]);
    } else if ((strncmp(argv[counter1], "-windowid", length) == 0) &&
               (counter1 < argc) && (length >= 4)) {
      counter1++;
      window = strtoul(argv[counter1], (char **) NULL, 0);
    } else if ((strncmp(argv[counter1], "-windowname", length) == 0) &&
               (counter1 < argc) && (length >= 4)) {
      counter1++;
      windowname = argv[counter1];
    } else if ((strncmp(argv[counter1], "-x", length) == 0) &&
               (counter1 < argc) && (length == 2)) {
      counter1++;
      x = atoi(argv[counter1]);
    } else if ((strncmp(argv[counter1], "-xroot", length) == 0) &&
               (counter1 < argc) && (length >= 3)) {
      counter1++;
      x_root = atoi(argv[counter1]);
    } else if ((strncmp(argv[counter1], "-y", length) == 0) &&
               (counter1 < argc) && (length == 2)) {
      counter1++;
      y = atoi(argv[counter1]);
    } else if ((strncmp(argv[counter1], "-yroot", length) == 0) &&
               (counter1 < argc) && (length >= 3)) {
      counter1++;
      y_root = atoi(argv[counter1]);
    }
    counter1++;
  }

  /* Make sure that the possible X errors are */
  /* ignored. At least we don't care about. */
  handler1 = Tk_CreateErrorHandler(Tk_Display(tkrootwin), BadWindow,
                                   -1, -1, LocalXErrorHandler,
                                   (ClientData) NULL);
  handler2 = Tk_CreateErrorHandler(Tk_Display(tkrootwin), BadAccess,
                                   X_GetProperty, -1,
                                   LocalXErrorHandler, (ClientData)
                                   NULL);
  handler3 = Tk_CreateErrorHandler(Tk_Display(tkrootwin), BadAtom,
                                   X_GetProperty, -1,
                                   LocalXErrorHandler, (ClientData)
                                   NULL);

  /* Validate the parameters. */
  length = strlen(argv[1]);
  if (window == (Window) None &&
      !(((strncmp(argv[1], "eventmotion", length) == 0) && length >= 7) ||
        ((strncmp(argv[1], "eventreparent", length) == 0) && length >= 6))) {
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, argv[0], ": no target window specified",
                     (char *) NULL);
    goto error;
  }
  if (format != 8 && format != 16 && format != 32) {
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, argv[0],
                     ": wrong format value (must be 8, 16 or 32)",
                     (char *) NULL);
    goto error;
  }

  /* Retrieve global (often used) informations. */
  if (window != (Window) None) {
    XQueryTree(Tk_Display(tkrootwin), window, &root_ret, &parent_ret,
               &child_list1, &num_child1);
    if (!continueXAccess) {
      sprintf(error_buf, "querying the widget tree for the root window!");
      goto Xerror;
    }
    XQueryPointer(Tk_Display(tkrootwin), window, &root_ret,
                  &child_ret, &x_root_ret, &y_root_ret, &x_ret,
                  &y_ret, &mask_ret);
    if (!continueXAccess) {
      sprintf(error_buf, "querying the current pointer position!");
      goto Xerror;
    }
  } else {
    XQueryTree(Tk_Display(tkrootwin), Tk_WindowId(tkrootwin),
               &root_ret, &parent_ret, &child_list1, &num_child1);
    if (!continueXAccess) {
      sprintf(error_buf, "querying the widget tree for the root window!");
      goto Xerror;
    }
    XQueryPointer(Tk_Display(tkrootwin), Tk_WindowId(tkrootwin),
                  &root_ret, &child_ret, &x_root_ret, &y_root_ret,
                  &x_ret, &y_ret, &mask_ret);
    if (!continueXAccess) {
      sprintf(error_buf, "querying the current pointer position!");
      goto Xerror;
    }
  }
  (void) gettimeofday(&time, (struct timezone *) NULL);
  if (num_elements == 0) {
    num_elements = datalen;
  }
  if (width == 0 && tkwin != (Tk_Window) NULL) {
    width = Tk_Width(tkwin);
  }
  if (height == 0 && tkwin != (Tk_Window) NULL) {
    height = Tk_Height(tkwin);
  }
  if (x == -1002) {
    x = x_ret;
  }
  if (x_root == -1002) {
    x_root = x_root_ret;
  }
  if (y == -1002) {
    y = y_ret;
  }
  if (y_root == -1002) {
    y_root = y_root_ret;
  }

  c = argv[1][0];
  if ((c == 'e') &&
      (strncmp(argv[1], "eventbuttonpress", length) == 0) &&
      (length >= 12)) {
    xevent.type = ButtonPress;
    xbutton = (XButtonPressedEvent*) &xevent;
    xbutton->display = Tk_Display(tkrootwin);
    xbutton->window = window;
    xbutton->root = root_ret;
    xbutton->subwindow = child_ret;
    xbutton->time = (time.tv_sec * 1000) + time.tv_usec;
    xbutton->x = x;
    xbutton->y = y;
    xbutton->x_root = x_root;
    xbutton->y_root = y_root;
    xbutton->state = state;
    xbutton->button = button;
    xbutton->same_screen = True;
    XSendEvent(Tk_Display(tkrootwin), window, True, 0xffffL, &xevent);
    if (!continueXAccess) {
      sprintf(error_buf, "event button press %d", button);
      goto Xerror;
    }
  } else if ((c == 'e') &&
             (strncmp(argv[1], "eventbuttonrelease", length) == 0) &&
             (length >= 12)) {
    xevent.type = ButtonRelease;
    xbutton = (XButtonReleasedEvent*) &xevent;
    xbutton->display = Tk_Display(tkrootwin);
    xbutton->window = window;
    xbutton->root = root_ret;
    xbutton->subwindow = child_ret;
    xbutton->time = (time.tv_sec * 1000) + time.tv_usec;
    xbutton->x = x;
    xbutton->y = y;
    xbutton->x_root = x_root;
    xbutton->y_root = y_root;
    xbutton->state = state;
    xbutton->button = button;
    xbutton->same_screen = True;
    XSendEvent(Tk_Display(tkrootwin), window, True, 0xffffL, &xevent);
    if (!continueXAccess) {
      sprintf(error_buf, "event button release %d", button);
      goto Xerror;
    }
  } else if ((c == 'e') &&
             (strncmp(argv[1], "eventclientmessage", length) == 0) &&
             (length >= 7)) {
    xevent.type = ClientMessage;
    xclient = (XClientMessageEvent*) &xevent;
    xclient->display = Tk_Display(tkrootwin);
    xclient->window = window;
    xclient->message_type = message_type;
    xclient->format = format;
    XSendEvent(Tk_Display(tkrootwin), window, True, 0xffffL, &xevent);
    if (!continueXAccess) {
      sprintf(error_buf, "event client message");
      goto Xerror;
    }
  } else if ((c == 'e') &&
             (strncmp(argv[1], "eventconfigure", length) == 0) &&
             (length >= 7)) {
    xevent.type = ConfigureNotify;
    xconfig = (XConfigureEvent*) &xevent;
    xconfig->display = Tk_Display(tkrootwin);
    xconfig->event = window;
    xconfig->window = window;
    xconfig->x = x;
    xconfig->y = y;
    xconfig->width = width;
    xconfig->height = height;
    xconfig->border_width = border_width;
    xconfig->above = above;
    xconfig->override_redirect = override_redirect;
    XSendEvent(Tk_Display(tkrootwin), window, True, 0xffffL, &xevent);
    if (!continueXAccess) {
      sprintf(error_buf, "event configure notifiy");
      goto Xerror;
    }
  } else if ((c == 'e') &&
             (strncmp(argv[1], "eventexpose", length) == 0) &&
             (length >= 6)) {
    xevent.type = Expose;
    xexpose = (XExposeEvent*) &xevent;
    xexpose->count = 0;
    xexpose->display = Tk_Display(tkrootwin);
    xexpose->window = window;
    xexpose->x = x;
    xexpose->y = y;
    xexpose->width = width;
    xexpose->height = height;
    XSendEvent(Tk_Display(tkrootwin), window, True, 0xffffL, &xevent);
    if (!continueXAccess) {
      sprintf(error_buf, "event window expose");
      goto Xerror;
    }
  } else if ((c == 'e') &&
             (strncmp(argv[1], "eventfocusin", length) == 0) &&
             (length >= 11)) {
    xevent.type = FocusIn;
    xfocusin = (XFocusInEvent*) &xevent;
    xfocusin->display = Tk_Display(tkrootwin);
    xfocusin->window = window;
    xfocusin->mode = NotifyNormal;
    xfocusin->detail = NotifyPointer;
    XSendEvent(Tk_Display(tkrootwin), window, True, 0xffffL, &xevent);
    if (!continueXAccess) {
      sprintf(error_buf, "event window focusin");
      goto Xerror;
    }
  } else if ((c == 'e') &&
             (strncmp(argv[1], "eventfocusout", length) == 0) &&
             (length >= 11)) {
    xevent.type = FocusOut;
    xfocusout = (XFocusOutEvent*) &xevent;
    xfocusout->display = Tk_Display(tkrootwin);
    xfocusout->window = window;
    xfocusout->mode = NotifyNormal;
    xfocusout->detail = NotifyPointer;
    XSendEvent(Tk_Display(tkrootwin), window, True, 0xffffL, &xevent);
    if (!continueXAccess) {
      sprintf(error_buf, "event window focusout");
      goto Xerror;
    }
  } else if ((c == 'e') &&
             (strncmp(argv[1], "eventkeypress", length) == 0) &&
             (length >= 9)) {
    xevent.type = KeyPress;
    xkey = (XKeyPressedEvent*) &xevent;
    xkey->display = Tk_Display(tkrootwin);
    xkey->window = window;
    xkey->root = root_ret;
    xkey->subwindow = child_ret;
    xkey->time = (time.tv_sec * 1000) + time.tv_usec;
    xkey->x = x;
    xkey->y = y;
    xkey->x_root = x_root;
    xkey->y_root = y_root;
    xkey->state = state;
    xkey->keycode = keycode;
    xkey->same_screen = True;
    XSendEvent(Tk_Display(tkrootwin), window, True, 0xffffL, &xevent);
    if (!continueXAccess) {
      sprintf(error_buf, "event key press %d", keycode);
      goto Xerror;
    }
  } else if ((c == 'e') &&
             (strncmp(argv[1], "eventkeyrelease", length) == 0) &&
             (length >= 9)) {
    xevent.type = KeyRelease;
    xkey = (XKeyReleasedEvent*) &xevent;
    xkey->display = Tk_Display(tkrootwin);
    xkey->window = window;
    xkey->root = root_ret;
    xkey->subwindow = child_ret;
    xkey->time = (time.tv_sec * 1000) + time.tv_usec;
    xkey->x = x;
    xkey->y = y;
    xkey->x_root = x_root;
    xkey->y_root = y_root;
    xkey->state = state;
    xkey->keycode = keycode;
    xkey->same_screen = True;
    XSendEvent(Tk_Display(tkrootwin), window, True, 0xffffL, &xevent);
    if (!continueXAccess) {
      sprintf(error_buf, "event key release %d", keycode);
      goto Xerror;
    }
  } else if ((c == 'e') && (strncmp(argv[1], "eventmap", length) == 0) &&
             (length >= 7)) {
    XMapWindow(Tk_Display(tkrootwin), window);
    if (!continueXAccess) {
      sprintf(error_buf, "event window map");
      goto Xerror;
    }
  } else if ((c == 'e') &&
             (strncmp(argv[1], "eventmotion", length) == 0) &&
             (length >= 7)) {
    if (interval != (char *) NULL) {
      interval_steps = atof(interval);
    }
    XQueryPointer(Tk_Display(tkrootwin), Tk_WindowId(tkrootwin),
                  &root_ret, &child_ret, &x_root_ret, &y_root_ret,
                  &x_ret, &y_ret, &mask_ret);
    if (!continueXAccess) {
      sprintf(error_buf, "querying the current pointer position!");
      goto Xerror;
    }
    if (window == (Window) None) {
      if (!relative) {
        x -= x_root_ret;
        y -= y_root_ret;
      }
    }
    if (interval_steps > 0) {
      x_tmp = x_ret;
      y_tmp = y_ret;
      while (x_tmp != x || y_tmp != y) {
        if (x_tmp < x) {
          x_tmp += interval_steps;
          if (x_tmp > x) {
            x_tmp = x;
          }
        } else {
          x_tmp -= interval_steps;
          if (x_tmp < x) {
            x_tmp = x;
          }
        }
        if (y_tmp < y) {
          y_tmp += interval_steps;
          if (y_tmp > y) {
            y_tmp = y;
          }
        } else {
          y_tmp -= interval_steps;
          if (y_tmp < y) {
            y_tmp = y;
          }
        }
        XWarpPointer(Tk_Display(tkrootwin), (Window) None, (Window) window,
                     0, 0, 0, 0, (int) x_tmp, (int) y_tmp);
        if (!continueXAccess) {
          sprintf(error_buf,
                  "setting the pointer position to x:%d y:%d",
                  (int) x_tmp, (int) y_tmp);
          goto Xerror;
        }
        XSync(Tk_Display(tkrootwin), False);
      }
    } else {
      XWarpPointer(Tk_Display(tkrootwin), (Window) None, (Window) window,
                   0, 0, 0, 0, x, y);
      if (!continueXAccess) {
        sprintf(error_buf,
                "setting the pointer position to x:%d y:%d", x, y);
        goto Xerror;
      }
    }
  } else if ((c == 'e') &&
             (strncmp(argv[1], "eventreparent", length) == 0) &&  
             (length >= 6)) {
    if (parent == (Window) None) {
      Tcl_ResetResult(interp);
      Tcl_AppendResult(interp, argv[0], ": no parent window specified",
                       (char *) NULL);
      goto error;
    }
    /* Get the first level of X windows. */
#if defined(XlibSpecificationRelease)
    XFree((Window *) child_list1);
#else
    XFree((char *) child_list1);
#endif
    child_list1 = (Window *) None;
    XSync(Tk_Display(tkrootwin), False);
    XQueryTree(Tk_Display(tkrootwin), Tk_WindowId(tkrootwin),
               &root_window, &parent_ret, &child_list1, &num_child1);
    if (!continueXAccess) {
      sprintf(error_buf, "querying the widget tree for the root window!");
      goto Xerror;
    }
    if (windowname != (char *) NULL && strcmp(windowname, "") != 0) {
      /* Get the second, third and fourth level of X */
      /* windows. Check their names against the name to */
      /* reparent. Do this several times, as it may take */
      /* some time for the embedded application to create */
      /* the window. */
#if defined(XlibSpecificationRelease)
      XFree((Window *) child_list1);
#else
      XFree((char *) child_list1);
#endif
      child_list1 = (Window *) None;
      for (counter4 = 0; counter4 < retries; counter4++) {
        XSync(Tk_Display(tkrootwin), False);
        XQueryTree(Tk_Display(tkrootwin), root_window,
                   &root_ret, &parent_ret, &child_list1, &num_child1);
        if (!continueXAccess) {
          sprintf(error_buf, "querying the widget tree for the root window!");
          goto Xerror;
        }
        for (counter1 = 0; counter1 < num_child1; counter1++) {
          XSync(Tk_Display(tkrootwin), False);
          XFetchName(Tk_Display(tkrootwin), child_list1[counter1],
                     &win_name);
          if (!continueXAccess) {
            sprintf(error_buf, "retrieving window name!");
            goto Xerror;
          }
          if (win_name != (char *) NULL && strcmp(win_name, "") != 0) {
            if (Tcl_StringMatch(windowname, win_name)) {
              window = child_list1[counter1];
              XFree(win_name);
              goto doreparent;
            }
            XFree(win_name);
          }
          XSync(Tk_Display(tkrootwin), False);
          XQueryTree(Tk_Display(tkrootwin), child_list1[counter1],
                     &root_ret, &parent_ret, &child_list2, &num_child2);
          if (!continueXAccess) {
            sprintf(error_buf, "querying the widget tree for the root window!");
            goto Xerror;
          }
          for (counter2 = 0; counter2 < num_child2; counter2++) {
            XSync(Tk_Display(tkrootwin), False);
            XFetchName(Tk_Display(tkrootwin), child_list2[counter2],
                       &win_name);
            if (!continueXAccess) {
              sprintf(error_buf, "retrieving window name!");
              goto Xerror;
            }
            if (win_name != (char *) NULL && strcmp(win_name, "") != 0) {
              if (Tcl_StringMatch(windowname, win_name)) {
                window = child_list2[counter2];
                XFree(win_name);
                goto doreparent;
              }
              XFree(win_name);
            }
            XSync(Tk_Display(tkrootwin), False);
            XQueryTree(Tk_Display(tkrootwin), child_list2[counter2],
                       &root_ret, &parent_ret, &child_list3, &num_child3);
            if (!continueXAccess) {
              sprintf(error_buf, "querying the widget tree for the root window!");
              goto Xerror;
            }
            for (counter3 = 0; counter3 < num_child3; counter3++) {
              XSync(Tk_Display(tkrootwin), False);
              XFetchName(Tk_Display(tkrootwin), child_list3[counter3],
                         &win_name);
              if (!continueXAccess) {
                sprintf(error_buf, "retrieving window name!");
                goto Xerror;
              }
              if (win_name != (char *) NULL && strcmp(win_name, "") != 0) {
                if (Tcl_StringMatch(windowname, win_name)) {
                  window = child_list3[counter3];
                  XFree(win_name);
                  goto doreparent;
                }
                XFree(win_name);
              }
            }
#if defined(XlibSpecificationRelease)
            XFree((Window *) child_list3);
#else
            XFree((char *) child_list3);
#endif
            child_list3 = (Window *) None;
          }
#if defined(XlibSpecificationRelease)
          XFree((Window *) child_list2);
#else
          XFree((char *) child_list2);
#endif
          child_list2 = (Window *) None;
        }
#if defined(XlibSpecificationRelease)
        XFree((Window *) child_list1);
#else
        XFree((char *) child_list1);
#endif
        child_list1 = (Window *) None;
      }
    }

  doreparent:
    if (window != (Window) None) {
      /* We found the window to reparent... now reparent it */
      XSync(Tk_Display(tkrootwin), False);
      XWithdrawWindow(Tk_Display(tkrootwin), window,
                      Tk_ScreenNumber(tkrootwin));
      for (counter1 = 0; counter1 < REPARENT_LOOPS; counter1++) {
        XReparentWindow(Tk_Display(tkrootwin), window, parent, 0, 0);
        XSync(Tk_Display(tkrootwin), False);
      }
      XMapWindow(Tk_Display(tkrootwin), window);
      if (!continueXAccess) {
        XSync(Tk_Display(tkrootwin), False);
        sprintf(error_buf, "event window reparent %ld to %ld", window, parent);
        goto Xerror;
      }
      Tcl_ResetResult(interp);
      sprintf(buf, "%ld", window);
      Tcl_AppendResult(interp, buf, (char *) NULL);
    }
    XSync(Tk_Display(tkrootwin), False);
  } else if ((c == 'e') &&
             (strncmp(argv[1], "eventunmap", length) == 0) &&  
             (length >= 6)) {
    XUnmapWindow(Tk_Display(tkrootwin), window);
    if (!continueXAccess) {
      sprintf(error_buf, "event window unmap");
      goto Xerror;
    }
  } else if ((c == 'p') && (strncmp(argv[1], "pickcolor", length) == 0) &&
      (length >= 3)) {
#if defined(XlibSpecificationRelease)
    XFree((Window *) child_list1);
#else
    XFree((char *) child_list1);
#endif
    child_list1 = (Window *) None;
    if (tksteal_cursor == (Cursor) NULL) {
      tksteal_cursor = XCreateFontCursor(Tk_Display(tkrootwin), XC_target);
    }
    Tcl_ResetResult(interp);
    XQueryTree(Tk_Display(tkrootwin), Tk_WindowId(tkrootwin),
               &root_window, &parent_ret, &child_list1, &num_child1);
    XUngrabPointer(Tk_Display(tkrootwin), CurrentTime);
    grabResult = XGrabPointer(Tk_Display(tkrootwin), window,
                              False, ButtonReleaseMask,
                              GrabModeAsync, GrabModeAsync, None,
                              tksteal_cursor, CurrentTime);
    if (grabResult == GrabSuccess) {
      counter1 = 1;
      while (counter1) {
        XNextEvent(Tk_Display(tkrootwin), &xevent);
        switch (xevent.type) {
        case ButtonRelease:
          window = root_window;
          x = xevent.xbutton.x_root;
          y = xevent.xbutton.y_root;
          XTranslateCoordinates(Tk_Display(tkrootwin), window, window,
                                x, y, &x, &y, &stop_window);
          if (!stop_window)
            stop_window = window;
          XGetWindowAttributes(Tk_Display(tkrootwin), stop_window,
                               &winAttributes);
          if (winAttributes.depth ==
              DefaultDepth(Tk_Display(tkrootwin),
                           Tk_ScreenNumber(tkrootwin))) {
            image = XGetImage(Tk_Display(tkrootwin), root_window,
                              xevent.xbutton.x_root,
                              xevent.xbutton.y_root,
                              1, 1, AllPlanes, ZPixmap);
          } else {
            XTranslateCoordinates(Tk_Display(tkrootwin), root_window,
                                  window, xevent.xbutton.x_root,
                                  xevent.xbutton.y_root,
                                  &x_ret, &y_ret, &stop_window);
            image = XGetImage(Tk_Display(tkrootwin), root_window,
                              x_ret, y_ret, 1, 1, AllPlanes, ZPixmap);
          }
          color.pixel = XGetPixel(image, 0, 0);
          XQueryColor(Tk_Display(tkrootwin), winAttributes.colormap, &color);
          sprintf(buf, "#%04x%04x%04x", color.red, color.green, color.blue);
          Tcl_AppendResult(interp, buf, (char *) NULL);
          XDestroyImage(image);
          counter1 = 0;
          break;
        }
      }
    } else {
      Tcl_ResetResult(interp);
      sprintf(buf, "%d", grabResult);
      Tcl_AppendResult(interp, argv[0], ": unable to grab server (",
                       buf, ")", (char *) NULL);
      goto error;
    }
    XUngrabPointer(Tk_Display(tkrootwin), CurrentTime);
  } else if ((c == 'p') && (strncmp(argv[1], "propchange", length) == 0) &&
      (length >= 5)) {
    if (dataindex == 0) {
      Tcl_ResetResult(interp);
      Tcl_AppendResult(interp, argv[0], ": no data defined",
                       (char *) NULL);
      goto error;
    }
    if (property == (Atom) NULL) {
      Tcl_ResetResult(interp);
      Tcl_AppendResult(interp, argv[0], ": no property specified",
                       (char *) NULL);
      goto error;
    }
    XChangeProperty(Tk_Display(tkrootwin), window, property, type,
                    format, mode, (unsigned char *) argv[dataindex],
                    num_elements);
    if (!continueXAccess) {
      sprintf(error_buf, "event change property");
      goto Xerror;
    }
  } else if ((c == 'p') && (strncmp(argv[1], "propdelete", length) == 0) &&
      (length >= 5)) {
    XDeleteProperty(Tk_Display(tkrootwin), window, property);
  } else if ((c == 'p') && (strncmp(argv[1], "propget", length) == 0) &&
      (length >= 5)) {
    if (property == (Atom) NULL) {
      atomList = XListProperties(Tk_Display(tkrootwin), window,
                                 &num_elements);
      if (!continueXAccess) {
        sprintf(error_buf, "event list properties");
        goto Xerror;
      }
      if (atomList != (Atom *) NULL) {
        Tcl_ResetResult(interp);
        for (counter1 = 0; counter1 < num_elements; counter1++) {
          sprintf(buf, "%ld ", atomList[counter1]);
          Tcl_AppendResult(interp, buf, (char *) NULL);
        }
#if defined(XlibSpecificationRelease)
        XFree((Atom *) atomList);
#else
        XFree((char *) atomList);
#endif
      }
    } else {
      if (XGetWindowProperty(Tk_Display(tkrootwin), window, property,
                             long_offset, long_length, delete, req_type,
                             &actual_type, &actual_format,
                             &long_num_elements, &bytes_after,
                             (unsigned char **) &property_return) != Success) {
        Tcl_ResetResult(interp);
        Tcl_AppendResult(interp, argv[0], ": could not read property",
                         (char *) NULL);
        goto error;
      }
      if (actual_type == None) {
        Tcl_ResetResult(interp);
        Tcl_AppendResult(interp, argv[0], ": could not read property",
                         (char *) NULL);
        goto error;
      }
      if (actual_type != XA_STRING) {
        Tcl_ResetResult(interp);
        Tcl_AppendResult(interp, argv[0], ": can only retrieve XA_STRING type",
                         (char *) NULL);
        goto error;
      }
      if (!continueXAccess) {
        sprintf(error_buf, "event get window property");
        goto Xerror;
      }

      lcounter1 = 0;
      Tcl_ResetResult(interp);
      Tcl_AppendResult(interp,
                       XGetAtomName(Tk_Display(tkrootwin), property),
                       (char *) NULL);
      Tcl_AppendResult(interp, " ", (char *) NULL);
      while (1) {
        if (strcmp(propertyTable[lcounter1].name, "") == 0) {
          Tcl_AppendResult(interp, "UNDEFINED", (char *) NULL);
          break;
        }
        if (propertyTable[lcounter1].type == actual_type) {
          Tcl_AppendResult(interp,
                           propertyTable[lcounter1].name,
                           (char *) NULL);
          break;
        }
        lcounter1++;
      }
      Tcl_AppendResult(interp, " {", (char *) NULL);
      for (lcounter2 = 0; lcounter2 < long_num_elements; lcounter2++) {
        sprintf(buf, "%c", property_return[lcounter2]);
        Tcl_AppendResult(interp, buf, (char *) NULL);
        if (property_return[lcounter2] == '\0' &&
            lcounter2 < long_num_elements-1) {
          Tcl_AppendResult(interp, "} {", (char *) NULL);
        }
      }
      Tcl_AppendResult(interp, "}", (char *) NULL);
      XFree((char *) property_return);
    }
  } else if ((c == 's') && (strncmp(argv[1], "sendstring", length) == 0) &&  
             (length >= 2)) {
    if (dataindex == 0) {
      Tcl_ResetResult(interp);
      Tcl_AppendResult(interp, argv[0], ": no data defined",
                       (char *) NULL);
      goto error;
    }
    if (interval != (char *) NULL) {
      interval_value = atol(interval);
    }
    if ((modifierMap = XGetModifierMapping(Tk_Display(tkrootwin))) == NULL) {
      Tcl_ResetResult(interp);
      Tcl_AppendResult(interp, argv[0], ": unable to get ",
                       "modifier mapping", (char *) NULL);
      goto error;
    }
    /* Thanks go to John Ousterhout who implemented this in */
    /* tkBind.c... so I took it from there instead of reimplementing. */
    keyCode = modifierMap->modifiermap;
    for (counter2 = 0; counter2 < 8 * modifierMap->max_keypermod;
         counter2++, keyCode++) {
      if (*keyCode == 0) {
        continue;
      }
      if (XKeycodeToKeysym(Tk_Display(tkrootwin), *keyCode, 0)
          == XK_Mode_switch) {
        modeModifierMask |= ShiftMask << (counter2/modifierMap->max_keypermod);
      }
    }
    XFreeModifiermap(modifierMap);
    XDisplayKeycodes(Tk_Display(tkrootwin), &minKeyCode, &maxKeyCode);
    if ((mainKeyMap = XGetKeyboardMapping(Tk_Display(tkrootwin),
                                          (KeyCode) minKeyCode,
                                          (maxKeyCode - minKeyCode + 1),
                                          &keysymsPerKeyCode)) == NULL) {
      Tcl_ResetResult(interp);
      Tcl_AppendResult(interp, argv[0], ": unable to get ",
                       "keyboard mapping", (char *) NULL);
      goto error;
    }
    for (counter1 = 0; counter1 < datalen; counter1++) {
      if (argv[dataindex][counter1] == '<') {
        counter1++;
        if (counter1 == datalen) {
          Tcl_ResetResult(interp);
          Tcl_AppendResult(interp, argv[0], ": malformed data defined: \"",
                           argv[dataindex], "\"", (char *) NULL);
          goto error;
        }
        if (argv[dataindex][counter1] != '<') {
          if (argv[dataindex][counter1] == 'P' &&
              argv[dataindex][counter1+1] == 'r' &&
              argv[dataindex][counter1+2] == 'e' &&
              argv[dataindex][counter1+3] == 's' &&
              argv[dataindex][counter1+4] == 's' &&
              argv[dataindex][counter1+5] == '-') {
            counter1 += 6;
            if (counter1 == datalen) {
              Tcl_ResetResult(interp);
              Tcl_AppendResult(interp, argv[0], ": malformed data defined: \"",
                               argv[dataindex], "\"", (char *) NULL);
              goto error;
            }
            lcounter1 = 0;
            while (counter1 < datalen && argv[dataindex][counter1] != '>') {
              buf[lcounter1++] = argv[dataindex][counter1++];
            }
            buf[lcounter1] = '\0';
            if (TkXAccessMatchModifierState(buf) != -1) {
              permanentState |= TkXAccessMatchModifierState(buf);
            }
            continue;
          } else {
            if (argv[dataindex][counter1] == 'R' &&
                argv[dataindex][counter1+1] == 'e' &&
                argv[dataindex][counter1+2] == 'l' &&
                argv[dataindex][counter1+3] == 'e' &&
                argv[dataindex][counter1+4] == 'a' &&
                argv[dataindex][counter1+5] == 's' &&
                argv[dataindex][counter1+6] == 'e' &&
                argv[dataindex][counter1+7] == '-') {
              counter1 += 8;
              if (counter1 == datalen) {
                Tcl_ResetResult(interp);
                Tcl_AppendResult(interp, argv[0],
                                 ": malformed data defined: \"",
                                 argv[dataindex], "\"", (char *) NULL);
                goto error;
              }
              lcounter1 = 0;
              while (counter1 < datalen && argv[dataindex][counter1] != '>') {
                buf[lcounter1++] = argv[dataindex][counter1++];
              }
              buf[lcounter1] = '\0';
              if (TkXAccessMatchModifierState(buf) != -1) {
                permanentState &= ~TkXAccessMatchModifierState(buf);
              }
              continue;
            } else {
              lcounter1 = 0;
              while (counter1 < datalen && argv[dataindex][counter1] != '>') {
                buf[lcounter1++] = argv[dataindex][counter1++];
              }
              buf[lcounter1] = '\0';
              if (TkXAccessMatchModifierState(buf) != -1) {
                state |= TkXAccessMatchModifierState(buf);
                continue;
              }
            }
          }
        } else {
          buf[0] = argv[dataindex][counter1];
          buf[1] = '\0';
        }
      } else {
        buf[0] = argv[dataindex][counter1];
        buf[1] = '\0';
      }
      /* now we map certain special character. Is there another way */
      /* to do this (ok besides using a ascii indexed table) ??? */
      if (strlen(buf) == 1) {
        switch (buf[0]) {
        case '^':
          strcpy(buf, "asciicircum");
          break;
        case '~':
          strcpy(buf, "asciitilde");
          break;
        case ' ':
          strcpy(buf, "space");
          break;
        case '!':
          strcpy(buf, "exclam");
          break;
        case '"':
          strcpy(buf, "quotedbl");
          break;
        case '@':
          strcpy(buf, "at");
          break;
        case '$':
          strcpy(buf, "dollar");
          break;
        case '%':
          strcpy(buf, "percent");
          break;
        case '&':
          strcpy(buf, "ampersand");
          break;
        case '/':
          strcpy(buf, "slash");
          break;
        case '(':
          strcpy(buf, "parenleft");
          break;
        case ')':
          strcpy(buf, "parenright");
          break;
        case '{':
          strcpy(buf, "braceleft");
          break;
        case '}':
          strcpy(buf, "braceright");
          break;
        case '[':
          strcpy(buf, "bracketleft");
          break;
        case ']':
          strcpy(buf, "bracketright");
          break;
        case '=':
          strcpy(buf, "equal");
          break;
        case '?':
          strcpy(buf, "question");
          break;
        case '\\':
          strcpy(buf, "backslash");
          break;
        case '\'':
          strcpy(buf, "apostrophe");
          break;
        case '`':
          strcpy(buf, "grave");
          break;
        case '+':
          strcpy(buf, "plus");
          break;
        case '*':
          strcpy(buf, "asterisk");
          break;
        case '#':
          strcpy(buf, "numbersign");
          break;
        case '|':
          strcpy(buf, "bar");
          break;
        case ',':
          strcpy(buf, "comma");
          break;
        case ';':
          strcpy(buf, "semicolon");
          break;
        case '.':
          strcpy(buf, "period");
          break;
        case ':':
          strcpy(buf, "colon");
          break;
        case '-':
          strcpy(buf, "minus");
          break;
        case '_':
          strcpy(buf, "underscore");
          break;
        case '<':
          strcpy(buf, "less");
          break;
        case '>':
          strcpy(buf, "greater");
          break;
        case '\t':
          strcpy(buf, "Tab");
          break;
        case '\r':
          strcpy(buf, "Return");
          break;
        case '\n':
          strcpy(buf, "Return");
          break;
        case '\b':
          strcpy(buf, "BackSpace");
          break;
        }
      }
      
      keycode = 0;
      tmpKeycode = XKeysymToKeycode(Tk_Display(tkrootwin),
                                    XStringToKeysym(buf));
      keySym = XKeycodeToKeysym(Tk_Display(tkrootwin), tmpKeycode, 0);
      if (XKeysymToString(keySym) != NULL &&
          strcmp(buf, XKeysymToString(keySym)) == 0) {
        keycode = tmpKeycode;
      }
      if (keycode == 0 && keysymsPerKeyCode > 0) {
        keySym = XKeycodeToKeysym(Tk_Display(tkrootwin), tmpKeycode, 1);
        if (XKeysymToString(keySym) != NULL &&
            strcmp(buf, XKeysymToString(keySym)) == 0) {
          keycode = tmpKeycode;
          state |= ShiftMask;
        }
      }
      if (keycode == 0 && keysymsPerKeyCode > 1) {
        keySym = XKeycodeToKeysym(Tk_Display(tkrootwin), tmpKeycode, 2);
        if (XKeysymToString(keySym) != NULL &&
            strcmp(buf, XKeysymToString(keySym)) == 0) {
          keycode = tmpKeycode;
          state |= modeModifierMask;
        }
      }
      if (keycode == 0 && keysymsPerKeyCode > 2) {
        keySym = XKeycodeToKeysym(Tk_Display(tkrootwin), tmpKeycode, 3);
        if (XKeysymToString(keySym) != NULL &&
            strcmp(buf, XKeysymToString(keySym)) == 0) {
          keycode = tmpKeycode;
          state |= ShiftMask;
          state |= modeModifierMask;
        }
      }
      if (keycode != 0) {
        (void) gettimeofday(&time, (struct timezone *) NULL);
        xevent.type = KeyPress;
        xkey = (XKeyPressedEvent*) &xevent;
        xkey->display = Tk_Display(tkrootwin);
        xkey->window = window;
        xkey->root = root_ret;
        xkey->subwindow = child_ret;
        xkey->time = (time.tv_sec * 1000) + time.tv_usec;
        xkey->x = x;
        xkey->y = y;
        xkey->x_root = x_root;
        xkey->y_root = y_root;
        xkey->state = state | permanentState;
        xkey->keycode = keycode;
        xkey->same_screen = True;
        XSendEvent(Tk_Display(tkrootwin), window, True, 0xffffL, &xevent);
        if (!continueXAccess) {
          sprintf(error_buf, "event key press %d", keycode);
          goto Xerror;
        }
        XSync(Tk_Display(tkrootwin), False);
        (void) gettimeofday(&time, (struct timezone *) NULL);
        xevent.type = KeyRelease;
        xkey = (XKeyReleasedEvent*) &xevent;
        xkey->display = Tk_Display(tkrootwin);
        xkey->window = window;
        xkey->root = root_ret;
        xkey->subwindow = child_ret;
        xkey->time = (time.tv_sec * 1000) + time.tv_usec;
        xkey->x = x;
        xkey->y = y;
        xkey->x_root = x_root;
        xkey->y_root = y_root;
        xkey->state = state | permanentState;
        xkey->keycode = keycode;
        xkey->same_screen = True;
        XSendEvent(Tk_Display(tkrootwin), window, True, 0xffffL, &xevent);
        if (!continueXAccess) {
          sprintf(error_buf, "event key release %d", keycode);
          goto Xerror;
        }
        XSync(Tk_Display(tkrootwin), False);
        if (interval_value > 0) {
          (void) gettimeofday(&startTime, (struct timezone *) NULL);
          startTime.tv_sec += interval_value;
          while (1) {
            (void) gettimeofday(&time, (struct timezone *) NULL);
            if ((startTime.tv_sec < time.tv_sec) ||
                ((startTime.tv_sec == time.tv_sec) &&
                 (startTime.tv_usec < time.tv_usec))) {
              break;
            }
          }
        }
      }
      state = 0;
    }
    XFree ((char *) mainKeyMap);
  } else if ((c == 'x') && (strncmp(argv[1], "xwinchilds", length) == 0) &&  
             (length >= 5)) {
    Tcl_ResetResult(interp);
    for (counter1 = 0; counter1 < num_child1; counter1++) {
      if (counter1 == num_child1 - 1) {
        sprintf(buf, "%ld", child_list1[counter1]);
      } else {
        sprintf(buf, "%ld ", child_list1[counter1]);
      }
      Tcl_AppendResult(interp, buf, (char *) NULL);
    }
  } else if ((c == 'x') && (strncmp(argv[1], "xwinname", length) == 0) &&  
             (length >= 5)) {
    Tcl_ResetResult(interp);
    if (!XFetchName(Tk_Display(tkrootwin), window, &win_name)) {
      if (!continueXAccess) {
        sprintf(error_buf, "retrieving window name!");
        goto Xerror;
      }
      Tcl_AppendResult(interp, "", (char *) NULL);
    } else {
      if (!continueXAccess) {
        sprintf(error_buf, "retrieving window name!");
        goto Xerror;
      }
      if (win_name != (char *) NULL && strcmp(win_name, "") != 0) {
        Tcl_AppendResult(interp, win_name, (char *) NULL);
        XFree(win_name);
      }
    }
  } else if ((c == 'x') && (strncmp(argv[1], "xwinparent", length) == 0) &&  
             (length >= 5)) {
    Tcl_ResetResult(interp);
    sprintf(buf, "%ld", parent_ret);
    Tcl_AppendResult(interp, buf, (char *) NULL);
  } else if ((c == 'x') && (strncmp(argv[1], "xwinroot", length) == 0) &&  
             (length >= 5)) {
    Tcl_ResetResult(interp);
    sprintf(buf, "%ld", root_ret);
    Tcl_AppendResult(interp, buf, (char *) NULL);
  } else {
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, argv[0], ": unknown command \"", argv[1], "\"",
                     (char *) NULL);
    goto error;
  }

  XFlush(Tk_Display(tkrootwin));
  if (child_list3 != (Window *) None) {
#if defined(XlibSpecificationRelease)
    XFree((Window *) child_list3);
#else
    XFree((char *) child_list3);
#endif
  }
  if (child_list2 != (Window *) None) {
#if defined(XlibSpecificationRelease)
    XFree((Window *) child_list2);
#else
    XFree((char *) child_list2);
#endif
  }
  if (child_list1 != (Window *) None) {
#if defined(XlibSpecificationRelease)
    XFree((Window *) child_list1);
#else
    XFree((char *) child_list1);
#endif
  }
  Tk_DeleteErrorHandler(handler1);
  Tk_DeleteErrorHandler(handler2);
  Tk_DeleteErrorHandler(handler3);
  return TCL_OK;

 Xerror:
  XFlush(Tk_Display(tkrootwin));
  Tcl_ResetResult(interp);
  Tcl_AppendResult(interp, argv[0], " X protocol error: ",
                   error_buf, (char *) NULL);
  
 error:
  if (property_return != (char *) NULL) {
    XFree((char *) property_return);
  }
  if (atomList != (Atom *) NULL) {
#if defined(XlibSpecificationRelease)
    XFree((Atom *) atomList);
#else
    XFree((char *) atomList);
#endif
  }
  if (child_list3 != (Window *) None) {
#if defined(XlibSpecificationRelease)
    XFree((Window *) child_list3);
#else
    XFree((char *) child_list3);
#endif
  }
  if (child_list2 != (Window *) None) {
#if defined(XlibSpecificationRelease)
    XFree((Window *) child_list2);
#else
    XFree((char *) child_list2);
#endif
  }
  if (child_list1 != (Window *) None) {
#if defined(XlibSpecificationRelease)
    XFree((Window *) child_list1);
#else
    XFree((char *) child_list1);
#endif
  }
  Tk_DeleteErrorHandler(handler1);
  Tk_DeleteErrorHandler(handler2);
  Tk_DeleteErrorHandler(handler3);
  return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * TkSteal_Init --
 *
 *      This procedure initializes the TkSteal package, and
 *      should be called in Tcl_AppInit, or by the load feature.
 *
 * Results:
 *      Always TCL_OK.
 *
 * Side effects:
 *      Adds the new commands to the interpreter.
 *
 *----------------------------------------------------------------------
 */
int
TkSteal_Init(interp)
    Tcl_Interp *interp;         /* Interpreter for application. */
{
  /*
   * Call Tcl_CreateCommand for application-specific commands.
   */
  Tcl_CreateCommand(interp, "xaccess", TkXAccessCmd,
    (ClientData) Tk_MainWindow(interp), (Tcl_CmdDeleteProc *) NULL);

  return TCL_OK;
}

/* eof */

#endif /*USE_TK*/

