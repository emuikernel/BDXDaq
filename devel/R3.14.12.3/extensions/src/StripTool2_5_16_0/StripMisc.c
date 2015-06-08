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

#include "StripMisc.h"
#include "StripDefines.h"

#include <Xm/Xm.h>
#include <Xm/MessageB.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
/* This is not an ANSI function and may not be included from stdlib.h.
   Include it here explicitly */
extern char *ecvt(double, int, int *, int *);
#endif

#define MAX_PRE         32
#define MAX_LEN         64
#define EXP_DIGITS      3       /* digits in exp numerical portion */
#define EXP_LEN         (2+EXP_DIGITS)  /* length of exponent string */

/* Function prototypes */
static void Question_cb(Widget w, XtPointer client, XtPointer call);

#if 0
static char             *StripVersion   = "2.4.3";
#endif
unsigned char   Strip_x_error_code = Success;

float   vertical_pixels_per_mm;
float   horizontal_pixels_per_mm;
char    stripHelpPath[STRIP_PATH_MAX];

/*
 * StripMisc_init
 *
 *      Initializes global constants.
 */
void    StripMisc_init  (Display *display, int screen)
{
  horizontal_pixels_per_mm = 
    ((float)DisplayWidth(display, screen) /
     (float)DisplayWidthMM(display, screen));

  vertical_pixels_per_mm = 
    ((float)DisplayHeight(display, screen) /
     (float)DisplayHeightMM(display, screen));
}


#if 0
/* strip_version
 */
char    *strip_name  (void)
{
#if defined (USE_CDEV)
  return "StripTool (CDEV)";
#else
  return "StripTool (EPICS)";
#endif
}


/* strip_version
 */
char    *strip_version  (void)
{
  return StripVersion;
}


/* strip_lib_version
 */
char    *strip_lib_version  (void)
{
  static char buf[1024];

#if defined (USE_CDEV)
# include <cdevVersion.h>
  sprintf (buf, "CA: %s, CDEV: %s", ca_version(), CDEV_VERSION_STRING);
#else
  sprintf (buf, "CA: %s", ca_version());
#endif
  return buf;
}
#endif

/*
 * load_fonts
 */
void    get_current_time        (struct timeval *tv)
{
  gettimeofday (tv, 0);
}


char    *time2str (struct timeval *t)
{
  static char buf[512];

  sprintf (buf, "sec:\t %lu\nusec:\t %ld", t->tv_sec, (long)t->tv_usec);
  return buf;
}


/*
 * window_isviewable
 */
int     window_isviewable       (Display *d, Window w)
{
  XWindowAttributes     xwa;

  if (!w) return 0;

  XGetWindowAttributes (d, w, &xwa);
  return (xwa.map_state == IsViewable);
}


/*
 * window_ismapped
 */
int     window_ismapped         (Display *d, Window w)
{
  XWindowAttributes     xwa;

  if (!w) return 0;

  XGetWindowAttributes (d, w, &xwa);
  return (xwa.map_state != IsUnmapped);
}


/*
 * window_isiconic
 */
int     window_isiconic         (Display *d, Window w)
{
  unsigned long *state = NULL;
  unsigned long nitems;
  unsigned long bytesAfter;
  Atom wmStateAtom, actualTypeAtom;
  int actualFormat;
  int status;
  int iconic = 0;

  /* KE: this method violates the ICCCM guidelines, but checking
     XmNiconic or XmNiconWindow doesn't work */

  /* intern the WM_STATE atom, create it if it doesn't exist */
  wmStateAtom = XInternAtom (d, "WM_STATE", False);

  /* get the window state */
  status = XGetWindowProperty (d, w, wmStateAtom, 0l, 1, False, wmStateAtom,
    &actualTypeAtom, &actualFormat, &nitems, &bytesAfter,
    (unsigned char **)&state);
  if (state) {
    if (status == Success && actualTypeAtom == wmStateAtom && nitems == 1)
    {
	/* possibile states are IconicState, NormalState, WithdrawnState */
	if(*state == IconicState) iconic = 1;
    }
    XFree((void *)state);
  }
  
  return iconic;
}


/*
 * window_map
 */
void    window_map              (Display *d, Window w)
{
  XMapRaised (d, w);
}


/*
 * window_unmap
 */
void    window_unmap    (Display *d, Window w)
{
  XUnmapWindow (d, w);
}


void    sec2hms (unsigned sec, int *h, int *m, int *s)
{
  *s = sec;
  *h = *s / 3600;
  *s %= 3600;
  *m = *s / 60;
  *s %= 60;
}


char    *dbl2str        (double d, int p, char buf[], int n)
{
#if 0

  /* 
   * <sign>     1 char
   * <e>        1 char
   * <sign>     1 char
   * <exponent> 1-3 chars
   *
   * Also, if the field width is less than the precision
   * (significant digits), then the latter will have to
   * be decreased in order to avoid over-running the buffer.
   *
   * In order to determine the appropriate precision, if it must
   * be decreased, we need to know whether or not the converted
   * number will include an exponent, and if so, how many digits
   * the exponent will include.
   */
#define STR_SIZE        1023
  static char           str[STR_SIZE+1];
  
  sprintf (str, "% #*.*g", n, p);
  strncpy (buf, str, n);
  buf[n] = 0;
  
#else
  int   decpt, sign;
  char  tmp[MAX_LEN+1];
  char  e_str[EXP_LEN+1];
  char  e_cnt;
  int   i = 0, j = 0;

  memset (e_str, EXP_LEN+1, 1);
  e_str[0] = 'e';

  strcpy (tmp, ecvt (d, n, &decpt, &sign));
  buf[i++] = sign? '-' : ' ';
                   
  e_str[1] = (decpt > 0? '+' : '-');
  
  if (decpt > 0)                /* magnitude >= 1? */
  {
    if (p > 0)          /* print some digits after decimal point */
    {
      if (decpt+p > n-2)        /* need scientific notation */
      {
        int2str (decpt-1, &e_str[2], 2);
        for (e_cnt = 0; e_str[(int)e_cnt]; e_cnt++);
        if (e_cnt+2 > n) goto no_room;
        buf[i++] = tmp[j++];
        if (i < n-e_cnt-1)
        {
          buf[i++] = '.';
          while (i < n-EXP_LEN) buf[i++] = tmp[j++];
        }
        strcpy (&buf[i], e_str);
      }
      else                      /* print out d+p digits */
      {
        for (; decpt > 0; decpt--) buf[i++] = tmp[j++];
        buf[i++] = '.';
        for (; p > 0; p--) buf[i++] = tmp[j++];
        buf[i++] = '\0';
      }
    }
    else                        /* not interested in digits after decimal */
    {
      if (decpt > n-1)          /* need scientific notation */
      {
        
      }
      else while (i < decpt+1) buf[i++] = tmp[j++];
      buf[i++] = 0;
    }
  }
  else                          /* magnitude < 1*/
  {
    if (p > 0)          /* */
    {
      if (p+decpt > 0)          /* print some digits out */
      {
        if (p-decpt > n-3)      /* need scientific notation */
        {
          int2str (-(decpt-1), &e_str[2], 2);
          for (e_cnt = 0; e_str[(int)e_cnt]; e_cnt++);
          if (e_cnt+2 > n) goto no_room;
          buf[i++] = tmp[j++];
          if (i < n-e_cnt-1)
          {
            buf[i++] = '.';
            while (i < n-e_cnt) buf[i++] = tmp[j++];
          }
          strcpy (&buf[i], e_str);
        }
        else            /* print 0.(-decpt zeroes)(p+decpt digits) */
        {
          buf[i++] = '0';
          buf[i++] = '.';
          p += decpt;
          for (; decpt < 0; decpt++) buf[i++] = '0';
          for (; p > 0; p--) buf[i++] = tmp[j++];
          buf[i++] = '\0';
        }
      }
      else                      /* number too small --effectively zero */
      {
        buf[i++] = '0';
        buf[i++] = '.';
        for (; (i < n) && (p > 0); p--) buf[i++] = '0';
        buf[i++] = '\0';
      }
    }
    else                        /* effectively zero */
    {
      buf[i++] = '0';
      buf[i++] = '\0';
    }
  }
  
  return buf;

  no_room:
  for (i = 0; i < n; i++) buf[i] = '#';
  buf[i++] = '\0';
  return buf;
#endif
}


char    *int2str        (int x, char buf[], int n)
{
  static char   digits[10] = 
  {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

  char  tmp[MAX_LEN];
  int   i, j;

  /* convert digits to characters (reverse order) */
  for (i = 0; x != 0; i++)
  {
    tmp[i] = digits[x%10];
    x /= 10;
  }

  /* pad front of buffer with zeros */
  for (j = 0; j < n-i; j++) buf[j] = digits[0];

  /* fill in rest of buffer with number string */
  for (i -= 1; i >= 0; i--) buf[j++] = tmp[i];
  buf[j++] = '\0';
  return buf;
}


/* ====== MessageBox stuff ====== */
static void     MessageBox_cb           (Widget, XtPointer, XtPointer);

typedef enum
{
  MSGBOX_OK = 0,
  MSGBOX_MAP
}
MsgBoxEvent;

/*
 * MessageBox_popup
 */
void    MessageBox_popup        (Widget         parent,
                                 Widget         *message_box,
                                 int            type,
                                 char           *title,
                                 char           *btn_txt,
                                 char           *format,
                                 ...)
{
  va_list       args;
  XmString      xstr_title;
  XmString      xstr_msg;
  XmString      xstr_btn;
  char          buf[2048];

  /* make string */
  va_start (args, format);
  vsprintf (buf, format, args);
  va_end (args);

  /* initialize widget if necessary */
  if (*message_box != (Widget)0)
    if (parent != XtParent (*message_box))
    {
      XtDestroyWidget (*message_box);
      *message_box = (Widget)0;
    }
  if (*message_box == (Widget)0)
    *message_box = XmCreateMessageDialog (parent, "Oops", NULL, 0);
  
#if 0
  xstr_msg = XmStringCreateLocalized (buf);
#else
  /* Use XmStringGetLtoR because it handles multiple lines */
   xstr_msg = XmStringCreateLtoR(buf, XmFONTLIST_DEFAULT_TAG);
#endif  
  xstr_btn = XmStringCreateLocalized (btn_txt);
  xstr_title = XmStringCreateLocalized (title);

  XtVaSetValues
    (*message_box,
#ifndef WIN32
     XmNdialogType,             type,
#endif
     XmNdefaultButtonType,      XmDIALOG_OK_BUTTON,
     XmNnoResize,               True,
     XmNdefaultPosition,        False,
     XmNdialogTitle,            xstr_title,
     XmNmessageString,          xstr_msg,
     XmNokLabelString,          xstr_btn,
     NULL);

  XtUnmanageChild
    (XmMessageBoxGetChild (*message_box, XmDIALOG_CANCEL_BUTTON));
  XtUnmanageChild
    (XmMessageBoxGetChild (*message_box, XmDIALOG_HELP_BUTTON));

  XtAddCallback
    (*message_box, XmNokCallback, MessageBox_cb, (XtPointer)MSGBOX_OK);
  XtAddCallback
    (*message_box, XmNmapCallback, MessageBox_cb, (XtPointer)MSGBOX_MAP);

  XmStringFree (xstr_msg);
  XmStringFree (xstr_btn);
  XmStringFree (xstr_title);

  XtManageChild (*message_box);
  XtPopup (XtParent (*message_box), XtGrabNone);
}

/*
 * MessageBox_cb
 */
static void     MessageBox_cb   (Widget w, XtPointer client, XtPointer BOGUS(1))
{
  MsgBoxEvent           event = (MsgBoxEvent)client;
  Window                root, child;
  int                   root_x, root_y;
  int                   win_x, win_y;
  unsigned              mask;
  int                   screen;
  Display               *display;
  XWindowAttributes     xwa;
  Dimension             width, height;

  switch (event)
  {
      case MSGBOX_MAP:

        /* find out where the pointer is */
        display = XtDisplay (w);
        screen = DefaultScreen (display);
        XQueryPointer
          (XtDisplay (w), RootWindow (display, screen), 
           &root, &child,
           &root_x, &root_y,
           &win_x, &win_y,
           &mask);

        if (child != (Window)0)
          XGetWindowAttributes (display, child, &xwa);
        else XGetWindowAttributes (display, root, &xwa);

        XtVaGetValues (w, XmNwidth, &width, XmNheight, &height, NULL);

        /* place the dialog box in the center of the window in which the
         * pointer currently resides */
        XtVaSetValues
          (w,
           XmNx,                        xwa.x + ((xwa.width - (int)width)/2),
           XmNy,                        xwa.y + ((xwa.height - (int)height)/2),
           NULL);
      
        break;


      case MSGBOX_OK:

        break;
  }
}

/*
 * Question_popup
 */
int Question_popup(XtAppContext app, Widget wparent, char * question)
  /* Based on O'Reilly */
  /* Returns 1 for yes, 0 for no, -1 for cancel */
{
  static Widget wdialog = (Widget)0;
  XmString message,yes,no,cancel;
  static int answer;
  
  /* Create the dialog if not created */
  if (!wdialog) {
    yes = XmStringCreateLocalized("Yes");
    no = XmStringCreateLocalized("No");
    cancel = XmStringCreateLocalized("Cancel");
#ifndef WIN32
    wdialog = XmCreateQuestionDialog(wparent, "yesNo", NULL, 0);
#else
    wdialog = XmCreateMessageDialog(wparent, "yesNo", NULL, 0);
#endif
    XtVaSetValues(wdialog,
	XmNokLabelString,     yes,
	XmNcancelLabelString, no,
	XmNhelpLabelString,   cancel,
	XmNdialogStyle,       XmDIALOG_FULL_APPLICATION_MODAL,
	NULL);
    XtAddCallback(wdialog, XmNokCallback, Question_cb, &answer);
    XtAddCallback(wdialog, XmNcancelCallback, Question_cb, &answer);
    XtAddCallback(wdialog, XmNhelpCallback, Question_cb, &answer);
    XmStringFree(yes);
    XmStringFree(no);
    XmStringFree(cancel);

    /* Set the title of the dialog shell. (This is what the user sees.)
	 Otherwise it is named yesNo_popup.  */
    XtVaSetValues(XtParent(wdialog),
	XmNtitle,             "Question",
	NULL);
  }

  /* Add the question */
  message = XmStringCreateLocalized(question);
  XtVaSetValues(wdialog,
    XmNmessageString,     message,
    NULL);
  XmStringFree(message);
  XtManageChild(wdialog);
  XtPopup(XtParent(wdialog), XtGrabNone);

  /* Loop until the user decides what to do */
  answer = -2;
  while (answer == -2 || XtAppPending(app))
    XtAppProcessEvent(app, XtIMAll);
  XtPopdown(XtParent(wdialog));
  XSync(XtDisplay(wparent), 0);
  XmUpdateDisplay(wparent);
  return answer;
}

/*
 * Question_cb
 */
static void Question_cb(Widget w, XtPointer clientdata, XtPointer calldata)
{
  int *answer = (int *)clientdata;
  XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)calldata;
  
  switch (cbs->reason) {
  case XmCR_OK:
    *answer = 1;
    break;
  case XmCR_CANCEL:
    *answer = 0;
    break;
  case XmCR_HELP:
    *answer = -1;
    break;
  default:
    break;
  }
}


char    *basename       (char *path)
{
  char  *p = 0;

  if (path)
  {
    /* find end of string */
    for (p = path; *p; p++);

    /* move backwards, looking for '/' character, or
     * beginning of string --whichever comes first. */
    while ((p > path) && (*p != '/')) p--;
    if (*p == '/') p++;
  }

  return p;
}

/* Albert */
extern Widget history_topShell;
void History_MessageBox_popup(char *title,char *btn_txt,char *str)
{
          MessageBox_popup
            (history_topShell,
             (Widget *)XmCreateMessageDialog(history_topShell,"Oops",NULL,0), 
#ifndef WIN32
             XmDIALOG_WARNING,title,btn_txt,str);
#else
             XmDIALOG_MESSAGE,title,btn_txt,str);
#endif
}



/* General purpose output routine
 * Works with both UNIX and WIN32
 * Uses sprintf to avoid problem with lprintf not handling %f, etc.
 *   (Exceed 5 only) */
void print(const char *fmt, ...)
{
  va_list vargs;
  static char lstring[1024];  /* DANGER: Fixed buffer size */
  
  va_start(vargs,fmt);
  vsprintf(lstring,fmt,vargs);
  va_end(vargs);
  
  if(lstring[0] != '\0') {
#ifdef WIN32
    lprintf("%s",lstring);
#else
    printf("%s",lstring);
#endif
  }
}

/* Gets current time and puts it in a static array
 * The calling program should copy it to a safe place
 *   e.g. strcpy(savetime,timestamp()); */
char *timeStamp(void)
{
  static char timeStampStr[16];
  time_t now;
  struct tm *tblock;
  
  time(&now);
  tblock=localtime(&now);
  strftime(timeStampStr,20,"%b %d %H:%M:%S",tblock);
  
  return timeStampStr;
}

/* Function to convert / to \ to avoid inconsistencies in WIN32 */
void convertDirDelimiterToWIN32(char *pathName)
{
    char *ptr;
    
    ptr=strchr(pathName,'/');
    while(ptr) {
	*ptr='\\';
	ptr=strchr(ptr,'/');
    }
}

/* **************************** Emacs Editing Sequences ***************** */
/* Local Variables: */
/* tab-width: 6 */
/* c-basic-offset: 2 */
/* c-comment-only-line-offset: 0 */
/* c-indent-comments-syntactically-p: t */
/* c-label-minimum-indentation: 1 */
/* c-file-offsets: ((substatement-open . 0) (label . 2) */
/* (brace-entry-open . 0) (label .2) (arglist-intro . +) */
/* (arglist-cont-nonempty . c-lineup-arglist) ) */
/* End: */
