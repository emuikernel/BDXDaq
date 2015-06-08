
/* codaRegistry.c; flag USE_TK is used to distinguish C version from Tk one */

#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#ifdef USE_TK

/*
#include "tkInt_forCREG.h"
*/
/*
#include "tcl.h"
#include "tk.h"
#include "tkPort.h"
*/

#include "tkInt.h"

#endif

#include "codaRegistry.h"




typedef struct PendingCommand {
    int serial;			/* Serial number expected in result */
    Display *dispPtr;	/* Display being used for communication */
    char *target;		/* Name of interpreter command is being sent to */
    Window commWindow;	/* Target's communication window */
#ifdef USE_TK
  /*sergey: gives error: do we need it ????????????????????????*/
  /*Tk_TimerToken timeout;*/ /* Token for timer handler used to check up on target during long sends. */
  /*Tcl_Interp *interp;*/	   /* Interpreter from which the send was invoked */
#endif
    int code;			/* Tcl return code for command
				 * will be stored here. */
    char *result;		/* String result for command (malloc'ed),
				 * or NULL. */
    char *errorInfo;		/* Information for "errorInfo" variable,
				 * or NULL (malloc'ed). */
    char *errorCode;		/* Information for "errorCode" variable,
				 * or NULL (malloc'ed). */
    int gotResponse;		/* 1 means a response has been received,
				 * 0 means the command is still outstanding. */
    struct PendingCommand *nextPtr;
				/* Next in list of all outstanding
				 * commands.  NULL means end of
				 * list. */
} PendingCommand;



/* 
 * The following structure is used to keep track of the interpreters
 * registered by this process.
 */

typedef struct RegisteredInterp {
    char *name;			/* Interpreter's name (malloc-ed). */
#ifdef USE_TK
    Tcl_Interp *interp;		/* Interpreter associated with name.  NULL
				 * means that the application was unregistered
				 * or deleted while a send was in progress
				 * to it. */
#endif
    Display *dispPtr;		/* Display for the application.  Needed
				 * because we may need to unregister the
				 * interpreter after its main window has
				 * been deleted. */
    struct RegisteredInterp *nextPtr;

				/* Next in list of names associated
				 * with interps in this process.
				 * NULL means end of list. */
} RegisteredInterp;

static RegisteredInterp *registry = NULL;
				/* List of all interpreters
				 * registered by this process. */

#define MAX_PROP_WORDS 100000

/*
 * The following variable can be set while debugging to do things like
 * skip locking the server.
 */

static int sendDebug = 0;
static PendingCommand *pendingCommands = NULL;
				/* List of all commands currently
				 * being waited for. */
/*sergey: not in use ..??
static int tkSendSerial = 0;
*/

/*
 * Maximum size property that can be read at one time by
 * this module:
 */

#define MAX_PROP_WORDS 100000

/*
 * The default X error handler gets saved here, so that it can
 * be invoked if an error occurs that we can't handle.
 */


static int	(*defaultHandler) _ANSI_ARGS_((Display *display,
		    XErrorEvent *eventPtr)) = NULL;


/*
 * Forward references to procedures declared later in this file:
 */

static int	ErrorProc _ANSI_ARGS_((Display *display,
		    XErrorEvent *errEventPtr));

/*
 * Forward declarations for procedures defined later in this file:
 */

#ifndef USE_TK
#define TCL_OK     0
#define TCL_ERROR  1
#endif

#ifdef USE_TK
static int		AppendErrorProc _ANSI_ARGS_((ClientData clientData,
				XErrorEvent *errorPtr));
#endif

static void		AppendPropCarefully _ANSI_ARGS_((Display *display,
			    Window window, Atom property, char *value,
			    int length, PendingCommand *pendingPtr));

#ifdef USE_TK
static void		DeleteProc _ANSI_ARGS_((ClientData clientData));
#endif

static void		RegAddName _ANSI_ARGS_((NameRegistry *regPtr,
			    char *name, Window commWindow));

NameRegistry *	codaRegOpen _ANSI_ARGS_((Display *display, int lock));

static void		codaRegClose _ANSI_ARGS_((NameRegistry *regPtr));

static void		RegDeleteName _ANSI_ARGS_((NameRegistry *regPtr,
			    char *name));

static Window		RegFindName _ANSI_ARGS_((NameRegistry *regPtr,
			    char *name));

#ifdef USE_TK
static void		SendEventProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
#endif

static Bool		SendRestrictProc _ANSI_ARGS_((Display *display,
			    XEvent *eventPtr, char *arg));
static int		ServerSecure _ANSI_ARGS_((Display *dispPtr));
static void		UpdateCommWindow _ANSI_ARGS_((Display *display,Window window,char *name));
static int		ValidateWindowName _ANSI_ARGS_((Display *dispPtr,
			    char *name, Window commWindow, int oldOK));

/*
 *----------------------------------------------------------------------
 *
 * RegOpen --
 *
 *	This procedure loads the name registry for a display into
 *	memory so that it can be manipulated.
 *
 * Results:
 *	The return value is a pointer to the loaded registry.
 *
 * Side effects:
 *	If "lock" is set then the server will be locked.  It is the
 *	caller's responsibility to call RegClose when finished with
 *	the registry, so that we can write back the registry if
 *	neeeded, unlock the server if needed, and free memory.
 *
 *----------------------------------------------------------------------
 */

NameRegistry *
codaRegOpen(display, lock)
    Display *display;		/* Display whose name registry is to be
				 * opened. */
    int lock;			/* Non-zero means lock the window server
				 * when opening the registry, so no-one
				 * else can use the registry until we
				 * close it. */
{
    NameRegistry *regPtr;
    int result, actualFormat;
    unsigned long bytesAfter;
    Atom registryProperty, actualType;
    
    regPtr = (NameRegistry *) malloc(sizeof(NameRegistry));
    regPtr->dispPtr = display;
    regPtr->locked = 0;
    regPtr->modified = 0;
    regPtr->allocedByX = 1;

    if (lock) {
      XGrabServer(display);
      regPtr->locked = 1;
    }
    
    /*
     * Read the registry property.
     */
    registryProperty = XInternAtom(display,"CODARegistry",False);
    
    defaultHandler = XSetErrorHandler(ErrorProc);
    result = XGetWindowProperty(display,
				RootWindow(display, 0),
				registryProperty, 0, 10000,
				False, XA_STRING, &actualType, &actualFormat,
				&regPtr->propLength, &bytesAfter,
				(unsigned char **) &regPtr->property);
    
    XSetErrorHandler(defaultHandler);
    if (actualType == None) {
      regPtr->propLength = 0;
      regPtr->property = NULL;
    } else if ((result != Success) || (actualFormat != 8)
	       || (actualType != XA_STRING)) {
      /*
       * The property is improperly formed;  delete it.
       */
      
      if (regPtr->property != NULL) {
	XFree(regPtr->property);
	regPtr->propLength = 0;
	regPtr->property = NULL;
      }
      XDeleteProperty(display,
		      RootWindow(display, 0),
		      registryProperty);
    }
    
    /*
     * Xlib placed an extra null byte after the end of the property, just
     * to make sure that it is always NULL-terminated.  Be sure to include
     * this byte in our count if it's needed to ensure null termination
     * (note: as of 8/95 I'm no longer sure why this code is needed;  seems
     * like it shouldn't be).
     */
    
    if ((regPtr->propLength > 0)
	&& (regPtr->property[regPtr->propLength-1] != 0)) {
      regPtr->propLength++;
    }

    return regPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * RegFindName --
 *
 *	Given an open name registry, this procedure finds an entry
 *	with a given name, if there is one, and returns information
 *	about that entry.
 *
 * Results:
 *	The return value is the X identifier for the comm window for
 *	the application named "name", or None if there is no such
 *	entry in the registry.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static Window
RegFindName(regPtr, name)
    NameRegistry *regPtr;	/* Pointer to a registry opened with a
				 * previous call to RegOpen. */
    char *name;			/* Name of an application. */
{
    char *p, *entry;
    Window commWindow;

    commWindow = None;
    for (p = regPtr->property; (p-regPtr->property) < regPtr->propLength; ) {
	entry = p;
	while ((*p != 0) && (!isspace((unsigned char)(*p)))) {
	    p++;
	}
	if ((*p != 0) && (strcmp(name, p+1) == 0)) {
	    if (sscanf(entry, "%x", (unsigned int *) &commWindow) == 1) {
		return commWindow;
	    }
	}
	while (*p != 0) {
	    p++;
	}
	p++;
    }
    return None;
}

/*
 *----------------------------------------------------------------------
 *
 * RegDeleteName --
 *
 *	This procedure deletes the entry for a given name from
 *	an open registry.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If there used to be an entry named "name" in the registry,
 *	then it is deleted and the registry is marked as modified
 *	so it will be written back when closed.
 *
 *----------------------------------------------------------------------
 */

static void
RegDeleteName(regPtr, name)
    NameRegistry *regPtr;	/* Pointer to a registry opened with a
				 * previous call to RegOpen. */
    char *name;			/* Name of an application. */
{
    char *p, *entry, *entryName;
    int count;

    for (p = regPtr->property; (p-regPtr->property) < regPtr->propLength; ) {
	entry = p;
	while ((*p != 0) && (!isspace((unsigned char)(*p)))) {
	    p++;
	}
	if (*p != 0) {
	    p++;
	}
	entryName = p;
	while (*p != 0) {
	    p++;
	}
	p++;
	if ((strcmp(name, entryName) == 0)) {
	    /*
	     * Found the matching entry.  Copy everything after it
	     * down on top of it.
	     */

	    count = regPtr->propLength - (p - regPtr->property);
	    if (count > 0)  {
		memmove((void *) entry, (void *) p, (size_t) count);
	    }
	    regPtr->propLength -=  p - entry;
	    regPtr->modified = 1;
	    return;
	}
    }
}

/*
 *----------------------------------------------------------------------
 *
 * RegAddName --
 *
 *	Add a new entry to an open registry.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The open registry is expanded;  it is marked as modified so that
 *	it will be written back when closed.
 *
 *----------------------------------------------------------------------
 */

static void
RegAddName(regPtr, name, commWindow)
    NameRegistry *regPtr;	/* Pointer to a registry opened with a
				 * previous call to RegOpen. */
    char *name;			/* Name of an application.  The caller
				 * must ensure that this name isn't
				 * already registered. */
    Window commWindow;		/* X identifier for comm. window of
				 * application.  */
{
    char id[30];
    char *newProp;
    int idLength, newBytes;
	/*
printf("RegAddName reached\n");
	*/
	/*
printf("RegAddName >%s<\n",name);
	*/
    sprintf(id, "%x ", (unsigned int) commWindow);
    idLength = strlen(id);
    newBytes = idLength + strlen(name) + 1;
    newProp = (char *) malloc((unsigned) (regPtr->propLength + newBytes));
    strcpy(newProp, id);
    strcpy(newProp+idLength, name);
    if (regPtr->property != NULL) {
	memcpy((void *) (newProp + newBytes), (void *) regPtr->property,
		regPtr->propLength);
	if (regPtr->allocedByX) {
	    XFree(regPtr->property);
	} else {
	    free(regPtr->property);
	}
    }
    regPtr->modified = 1;
    regPtr->propLength += newBytes;
    regPtr->property = newProp;
    regPtr->allocedByX = 0;
}

/*
 *----------------------------------------------------------------------
 *
 * RegClose --
 *
 *	This procedure is called to end a series of operations on
 *	a name registry.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The registry is written back if it has been modified, and the
 *	X server is unlocked if it was locked.  Memory for the
 *	registry is freed, so the caller should never use regPtr
 *	again.
 *
 *----------------------------------------------------------------------
 */

static void
codaRegClose(regPtr)
    NameRegistry *regPtr;	/* Pointer to a registry opened with a
				 * previous call to RegOpen. */
{
  Atom registryProperty;
    if (regPtr->modified) {
	if (!regPtr->locked && !sendDebug) {
	  /*panic("The name registry was modified without being locked!");*/
	}
	registryProperty = XInternAtom(regPtr->dispPtr,"CODARegistry",False);
	XChangeProperty(regPtr->dispPtr,
		RootWindow(regPtr->dispPtr, 0),
		registryProperty, XA_STRING, 8,
		PropModeReplace, (unsigned char *) regPtr->property,
		(int) regPtr->propLength);
	
    }

    if (regPtr->locked) {
	XUngrabServer(regPtr->dispPtr);
    }
    XFlush(regPtr->dispPtr);

    if (regPtr->property != NULL) {
	if (regPtr->allocedByX) {
	    XFree(regPtr->property);
	} else {
	    free(regPtr->property);
	}
    }
    free((char *) regPtr);
}

static int
ErrorProc(display, errEventPtr)
    Display *display;			/* Display for which error
					 * occurred. */
    register XErrorEvent *errEventPtr;	/* Information about error. */
{
  return 0;
}
static int
ErrorProc2(display, errEventPtr)
    Display *display;			/* Display for which error
					 * occurred. */
    register XErrorEvent *errEventPtr;	/* Information about error. */
{
  /* Does this do anything useful? RWM bugbug */
  int *i = (int *) 0x98765432;

  *i = 1;
}


/*
 *----------------------------------------------------------------------
 *
 * ValidateWindowName --
 *
 *	This procedure checks to see if an entry in the registry
 *	is still valid.
 *
 * Results:
 *	The return value is 1 if the given commWindow exists and its
 *	name is "name".  Otherwise 0 is returned.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
ValidateWindowName(display, name, commWindow, oldOK)
    Display *display;		/* Display for which to perform the
				 * validation. */
    char *name;			/* The name of an application. */
    Window commWindow;		/* X identifier for the application's
				 * comm. window. */
    int oldOK;			/* Non-zero means that we should consider
				 * an application to be valid even if it
				 * looks like an old-style (pre-4.0) one;
				 * 0 means consider these invalid. */
{
    int result, actualFormat, argc, i;
    unsigned long length, bytesAfter;
    Atom appNameProperty,actualType;
    char *property;

    property = NULL;

    appNameProperty = XInternAtom(display,"CODA_APPLICATION",False);

    /*
     * Ignore X errors when reading the property (e.g., the window
     * might not exist).  If an error occurs, result will be some
     * value other than Success.
     */

    defaultHandler = XSetErrorHandler(ErrorProc);
    
    result = XGetWindowProperty(display, commWindow,
	    appNameProperty, 0, MAX_PROP_WORDS,
	    False, XA_STRING, &actualType, &actualFormat,
	    &length, &bytesAfter, (unsigned char **) &property);
    XSetErrorHandler(defaultHandler);

    if ((result == Success) && (actualType == None)) {
      result = 0;
    } else if ((result == Success) && (actualFormat == 8)
	   && (actualType == XA_STRING)) {
	result = 0;
	if (strcmp(property, name) == 0) {
	  result = 1;
	}
    } else {
      result = 0;
    }
    if (property != NULL) {
	XFree(property);
    }
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * ServerSecure --
 *
 *	Check whether a server is secure enough for us to trust
 *	Tcl scripts arriving via that server.
 *
 * Results:
 *	The return value is 1 if the server is secure, which means
 *	that host-style authentication is turned on but there are
 *	no hosts in the enabled list.  This means that some other
 *	form of authorization (presumably more secure, such as xauth)
 *	is in use.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
ServerSecure(display)
    Display *display;		/* Display to check. */
{
#ifdef TK_NO_SECURITY
    return 1;
#else
    XHostAddress *addrPtr;
    int numHosts, secure;
    Bool enabled;

    secure = 0;
    addrPtr = XListHosts(display, &numHosts, &enabled);
    if (enabled && (numHosts == 0)) {
	secure = 1;
    }
    if (addrPtr != NULL) {
	XFree((char *) addrPtr);
    }
    return secure;
#endif /* TK_NO_SECURITY */
}

/*
 *--------------------------------------------------------------
 *
 * CODASetAppName --
 *
 *	This procedure is called to associate an ASCII name with a Tk
 *	application.  If the application has already been named, the
 *	name replaces the old one.
 *
 * Results:
 *	The return value is the name actually given to the application.
 *	This will normally be the same as name, but if name was already
 *	in use for an application then a name of the form "name #2" will
 *	be chosen,  with a high enough number to make the name unique.
 *
 * Side effects:
 *	Registration info is saved, thereby allowing the "send" command
 *	to be used later to invoke commands in the application.  In
 *	addition, the "send" command is created in the application's
 *	interpreter.  The registration will be removed automatically
 *	if the interpreter is deleted or the "send" command is removed.
 *
 *--------------------------------------------------------------
 */

char *
CODASetAppName(display,window, name)
Display *display;
Window window;		/* Token for any window in the application
			 * to be named:  it is just used to identify
			 * the application and the display.  */
char *name;			/* The name that will be used to
				 * refer to the interpreter in later
				 * "send" commands.  Must be globally
				 * unique. */
{
    Window w;
    NameRegistry *regPtr;
    char *actualName;
    int offset, i;

    /*
     * See if the application is already registered;  if so, remove its
     * current name from the registry.

     */
    regPtr = codaRegOpen(display, 1);

    if (RegFindName(regPtr,name) != None) {
      RegDeleteName(regPtr, name);
    }
    RegAddName(regPtr, name, window);
    codaRegClose(regPtr);
    UpdateCommWindow(display,window,name);

    return name;
}

/*
 *----------------------------------------------------------------------
 *
 * TkGetInterpNames --
 *
 *	This procedure is invoked to fetch a list of all the
 *	interpreter names currently registered for the display
 *	of a particular window.
 *
 * Results:
 *	A standard Tcl return value.  Interp->result will be set
 *	to hold a list of all the interpreter names defined for
 *	tkwin's display.  If an error occurs, then TCL_ERROR
 *	is returned and interp->result will hold an error message.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char *
CODAGetAppNames(Display *display)
{
  Window window;
  char *p, *entry, *entryName;
  NameRegistry *regPtr;
  Window commWindow;
  int count;

  char *names = NULL;
    /*
     * Read the registry property, then scan through all of its entries.
     * Validate each entry to be sure that its application still exists.

     */
    regPtr = codaRegOpen(display, 1);

    for (p = regPtr->property; (p-regPtr->property) < regPtr->propLength; ) {
      entry = p;
      if (sscanf(p,  "%x",(unsigned int *) &commWindow) != 1) {
	commWindow =  None;
      }
      while ((*p != 0) && (!isspace((unsigned char)(*p)))) {
	p++;
      }
      if (*p != 0) {
	p++;
      }
      entryName = p;
      while (*p != 0) {
	p++;
      }
      p++;
      if (ValidateWindowName(display, entryName, commWindow, 1)) {
	/*
	 * The application still exists; add its name to the result.
	 */
	if (names == NULL) {
	  names = (char *) malloc(strlen(entryName)+1);
	  strcpy(names,entryName);
	} else {
	  names = (char *) realloc(names,strlen(names)+strlen(entryName)+2);
	  strcat(names," ");
	  strcat(names,entryName);
	}
      } else {
	/*
	 * This name is bogus (perhaps the application died without
	 * cleaning up its entry in the registry?).  Delete the name.
	 */
	
	count = regPtr->propLength - (p - regPtr->property);
	if (count > 0)  {
	  memmove((void *) entry, (void *) p, (size_t) count);
	}
	regPtr->propLength -= p - entry;
	regPtr->modified = 1;
	p = entry;
      }
    }
    codaRegClose(regPtr);
    return names;
}

int
CODAGetAppWindow(Display *display,char *name)
{
  Window window;
  char *p, *entry, *entryName;
  NameRegistry *regPtr;
  Window commWindow;
  int count;

  /*
   * Read the registry property, then scan through all of its entries.
   * Validate each entry to be sure that its application still exists.
   
   */
  regPtr = codaRegOpen(display, 1);
  
  for (p = regPtr->property; (p-regPtr->property) < regPtr->propLength; ) {
    entry = p;
    if (sscanf(p,  "%x",(unsigned int *) &commWindow) != 1) {
      commWindow =  None;
    }
    while ((*p != 0) && (!isspace((unsigned char)(*p)))) {
      p++;
    }
    if (*p != 0) {
      p++;
    }
    entryName = p;
    while (*p != 0) {
      p++;
    }
    p++;
    if (strcmp(entryName,name) == 0) {
      if (ValidateWindowName(display, entryName, commWindow, 1)) {
	codaRegClose(regPtr);
	
	return commWindow; 
      } else {
	/*
	 * This name is bogus (perhaps the application died without
	 * cleaning up its entry in the registry?).  Delete the name.
	 */
	
	count = regPtr->propLength - (p - regPtr->property);
	if (count > 0)  {
	  memmove((void *) entry, (void *) p, (size_t) count);
	}
	regPtr->propLength -= p - entry;
	regPtr->modified = 1;
	p = entry;
      }
    }
  }
  codaRegClose(regPtr);
  return 0;
}

/*
 *--------------------------------------------------------------
 *
 * AppendPropCarefully --
 *
 *	Append a given property to a given window, but set up
 *	an X error handler so that if the append fails this
 *	procedure can return an error code rather than having
 *	Xlib panic.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The given property on the given window is appended to.
 *	If this operation fails and if pendingPtr is non-NULL,
 *	then the pending operation is marked as complete with
 *	an error.
 *
 *--------------------------------------------------------------
 */

static void
AppendPropCarefully(display, window, property, value, length, pendingPtr)
    Display *display;		/* Display on which to operate. */
    Window window;		/* Window whose property is to
				 * be modified. */
    Atom property;		/* Name of property. */
    char *value;		/* Characters to append to property. */
    int length;			/* Number of bytes to append. */
    PendingCommand *pendingPtr;	/* Pending command to mark complete
				 * if an error occurs during the
				 * property op.  NULL means just
				 * ignore the error. */
{
  defaultHandler = XSetErrorHandler(ErrorProc);
  XChangeProperty(display, window, property, XA_STRING, 8,
		  PropModeAppend, (unsigned char *) value, length);
  XSync(display,0);
  XSetErrorHandler(defaultHandler);
}


/*
 *----------------------------------------------------------------------
 *
 * UpdateCommWindow --
 *
 *	This procedure updates the list of application names stored
 *	on our commWindow.  It is typically called when interpreters
 *	are registered and unregistered.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The TK_APPLICATION property on the comm window is updated.
 *
 *----------------------------------------------------------------------
 */

static void
UpdateCommWindow(Display *display,Window window,char *name)
{
  Atom appNameProperty;
  defaultHandler = XSetErrorHandler(ErrorProc);
  appNameProperty = XInternAtom(display,"CODA_APPLICATION",False);
  XChangeProperty(display, window,
		  appNameProperty, XA_STRING, 8, PropModeReplace,
		  (unsigned char *) name,
		  strlen(name));
  XSetErrorHandler(defaultHandler);
}






/********************************************************************/
/********************************************************************/

static int codaSendSerial = 0;



/*
 *--------------------------------------------------------------
 *
 * Tk_SendCmd --
 *
 *	This procedure is invoked to process the "send" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *--------------------------------------------------------------
 */

int
coda_Send(Display *display,char *destName,char *cmd)
{
    Window commWindow;
    int result, c, async, i, firstArg;
    size_t length;
    NameRegistry *regPtr;

    Atom commProperty = XInternAtom(display,"COMM",False);
    defaultHandler = XSetErrorHandler(ErrorProc);

    /*
     * Bind the interpreter name to a communication window.
     */
    regPtr = codaRegOpen(display, 1);
    commWindow = RegFindName(regPtr, destName);
    if (ValidateWindowName(display, destName, commWindow, 1) == 0) {
      /*
       * This name is bogus (perhaps the application died without
       * cleaning up its entry in the registry?).  Delete the name.
       */
      RegDeleteName(regPtr,destName);
      commWindow == None;
    }

    codaRegClose(regPtr);
    if (commWindow == None) {
      return TCL_ERROR;
    } 

    (void) AppendPropCarefully(display, commWindow,
			       commProperty, cmd,
			       strlen(cmd) + 1,
			       (PendingCommand *) NULL);
    return TCL_OK;
}


#ifndef USE_TK

static void 
resizeHandler(Widget w,Window target,XEvent *eventPtr)
{
  XWindowChanges wc;

  wc.x = 0;
  wc.y = 0;
  wc.width  = eventPtr->xconfigure.width;
  wc.height = eventPtr->xconfigure.height;
  wc.border_width = 0;
  wc.sibling = None;
  wc.stack_mode = Above;
  XConfigureWindow(XtDisplay(w),target, CWWidth | CWHeight, &wc);
}

static void 
exposeHandler(Widget w,Window target,XEvent *eventPtr)
{
  XWindowChanges wc;

  wc.x = 0;
  wc.y = 0;
  wc.width  = eventPtr->xexpose.width;
  wc.height = eventPtr->xexpose.height;
  wc.border_width = 0;
  wc.sibling = None;
  wc.stack_mode = Above;
  XConfigureWindow(XtDisplay(w),target, CWWidth | CWHeight, &wc);
}

typedef void (*MSG_FUNCPTR) (char *);
static void (*messageCallback)(char *message) = NULL;

void
codaRegisterMsgCallback(void *callback)
{
  messageCallback = (MSG_FUNCPTR) callback;
}

static void 
motifHandler(Widget w,XtPointer p,XEvent *eventPtr)
{
  Atom commProperty = XInternAtom(XtDisplay(w),"COMM",False);
  char *propInfo;
  int result, actualFormat;
  unsigned long numItems, bytesAfter;
  Atom actualType;

  if ((eventPtr->xproperty.atom != commProperty)
      || (eventPtr->xproperty.state != PropertyNewValue)) {
    return;
  }
  
    /*
     * Read the comm property and delete it.
     */

    propInfo = NULL;
    {
      result = XGetWindowProperty(XtDisplay(w),
				  XtWindow(w),
				  commProperty, 0, MAX_PROP_WORDS, True,
				  XA_STRING, &actualType, &actualFormat,
				  &numItems, &bytesAfter, (unsigned char **) &propInfo);
    }
    if (propInfo) {
      if (propInfo[0] == 'r') {
	int x,y,wid,hit,bw,d;
	Window root;
	Widget target,parent;
	int counter1;
	XWindowChanges wc;

	sscanf(&propInfo[2],"%x %x",&target,&parent);
	XGetGeometry(XtDisplay(w),parent,&root,&x,&y,&wid,&hit,&bw,&d);
	wc.x = 0;
        wc.y = 0;
        wc.width  = wid;
        wc.height = hit;
        wc.border_width = 0;
        wc.sibling = None;
        wc.stack_mode = Above;
	XConfigureWindow(XtDisplay(w),target, CWWidth | CWHeight, &wc);

	XWithdrawWindow(XtDisplay(w), target,0);
	for (counter1 = 0; counter1 < 25; counter1++) {
	  XReparentWindow(XtDisplay(w),target,parent,0,0);
	  XSync(XtDisplay(w), False);
	}
	   
	XMapWindow(XtDisplay(w), target);  
	XtAddEventHandler(XtParent(XtWindowToWidget(XtDisplay(w),parent)),StructureNotifyMask, False, resizeHandler, target);
	/*XtAddEventHandler(XtWindowToWidget(XtDisplay(w),parent),ExposureMask, False, exposeHandler, target);*/
      } else if (messageCallback) 
	(*messageCallback)(propInfo);
      XFree(propInfo);
    }
}

int
codaSendInit(Widget w, char *name)
{
    XSetWindowAttributes atts;

    XtAddEventHandler(w, PropertyChangeMask, False,
		      motifHandler, (XtPointer)NULL);

    CODASetAppName(XtDisplay(w),XtWindow(w),name);

    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * SendEventProc --
 *
 *	This procedure is invoked automatically by the toolkit
 *	event manager when a property changes on the communication
 *	window.  This procedure reads the property and handles
 *	command requests and responses.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If there are command requests in the property, they
 *	are executed.  If there are responses in the property,
 *	their information is saved for the (ostensibly waiting)
 *	"send" commands. The property is deleted.
 *
 *--------------------------------------------------------------
 */
/*
    ClientData clientData - Display information
    XEvent *eventPtr      - Information about event
*/
#ifdef DONOTKNOW
static void
SendEventProc(ClientData clientData, XEvent *eventPtr)
{
  Tk_Window *window = (Tk_Window *) clientData;
  char *propInfo;
  register char *p;
  int result, actualFormat;
  unsigned long numItems, bytesAfter;
  Atom actualType;
  Atom commProperty = XInternAtom(Tk_Display(window),"COMM",False);

    if ((eventPtr->xproperty.atom != commProperty)
	    || (eventPtr->xproperty.state != PropertyNewValue)) {
	return;
    }

    /*
     * Read the comm property and delete it.
     */

    propInfo = NULL;
    {
      result = XGetWindowProperty(Tk_Display(window),
				  Tk_WindowId(window),
				  commProperty, 0, MAX_PROP_WORDS, True,
				  XA_STRING, &actualType, &actualFormat,
				  &numItems, &bytesAfter, (unsigned char **) &propInfo);
    }
    if (propInfo) {
      XFree(propInfo);
    }

}
#endif


#else /*USE_TK*/


/*
 *--------------------------------------------------------------
 *
 * Tk_SendCmd --
 *
 *	This procedure is invoked to process the "send" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *--------------------------------------------------------------
 */

int
coda_SendCmd(clientData, interp, argc, argv)
    ClientData clientData;		/* Information about sender (only
					 * dispPtr field is used). */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    TkWindow *winPtr;
    Window commWindow;
    PendingCommand pending;
    register RegisteredInterp *riPtr;
    char *destName, buffer[30];
    int result, c, async, i, firstArg;
    size_t length;
    Bool (*prevRestrictProc)();
    char *prevArg;
    TkDisplay *dispPtr;
    NameRegistry *regPtr;
    Tcl_DString request;

    /*
     * Process options, if any.
     */

    async = 0;
    winPtr = (TkWindow *) Tk_MainWindow(interp);
    if (winPtr == NULL) {
	return TCL_ERROR;
    }
    for (i = 1; i < (argc-1); ) {
	if (argv[i][0] != '-') {
	    break;
	}
	c = argv[i][1];
	length = strlen(argv[i]);
	if ((c == 'a') && (strncmp(argv[i], "-async", length) == 0)) {
	    async = 1;
	    i++;
	} else if ((c == 'd') && (strncmp(argv[i], "-displayof",
		length) == 0)) {
	    winPtr = (TkWindow *) Tk_NameToWindow(interp, argv[i+1],
		    (Tk_Window) winPtr);
	    if (winPtr == NULL) {
		return TCL_ERROR;
	    }
	    i += 2;
	} else if (strcmp(argv[i], "--") == 0) {
	    i++;
	    break;
	} else {
	    Tcl_AppendResult(interp, "bad option \"", argv[i],
		    "\": must be -async, -displayof, or --", (char *) NULL);
	    return TCL_ERROR;
	}
    }

    if (argc < (i+2)) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" ?options? interpName arg ?arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    destName = argv[i];
    firstArg = i+1;

    /*
     * Bind the interpreter name to a communication window.
     */
    regPtr = codaRegOpen(Tk_Display(winPtr), 1);
    commWindow = RegFindName(regPtr, destName);
    codaRegClose(regPtr);
    if (commWindow == None) {
      Tcl_AppendResult(interp, "no application named \"",
		       destName, "\"", (char *) NULL);
      return TCL_ERROR;
    }

    /*
     * Send the command to the target interpreter by appending it to the
     * comm window in the communication window.
     */

    codaSendSerial++;
    Tcl_DStringInit(&request);
    Tcl_DStringAppend(&request, "testing ", 7);
    Tcl_DStringAppend(&request, destName, -1);
    {
      Atom commProperty = XInternAtom(Tk_Display(winPtr),"COMM",False);
      (void) AppendPropCarefully(Tk_Display(winPtr), commWindow,
				 commProperty, Tcl_DStringValue(&request),
				 Tcl_DStringLength(&request) + 1,
				 (PendingCommand *) NULL);
    } 
    Tcl_DStringFree(&request);
    return TCL_OK;
}

int
codaSendInit(interp, window, name)
    Tcl_Interp *interp;		/* Interpreter to use for error reporting
				 * (no errors are ever returned, but the
				 * interpreter is needed anyway). */
    Tk_Window *window;		/* Display to initialize. */
    char * name;
{
    XSetWindowAttributes atts;

    Tk_CreateEventHandler(window, PropertyChangeMask,
	    SendEventProc, (ClientData) window);

    /*XtAddEventHandler(Tk_Window(mainWindow), PropertyChangeMask, False,
		      TestHandler, (XtPointer)dispPtr);*/

    CODASetAppName(Tk_Display(window),Tk_WindowId(window),name);

    /*
     * Get atoms used as property names.

    dispPtr->commProperty = Tk_InternAtom(dispPtr->commTkwin, "Comm");
    dispPtr->registryProperty = Tk_InternAtom(dispPtr->commTkwin,
	    "InterpRegistry");
    dispPtr->appNameProperty = Tk_InternAtom(dispPtr->commTkwin,
	    "TK_APPLICATION");
     */
    Tcl_CreateCommand(interp, "coda_send", coda_SendCmd, (ClientData) NULL,
		      NULL);

    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * SendEventProc --
 *
 *	This procedure is invoked automatically by the toolkit
 *	event manager when a property changes on the communication
 *	window.  This procedure reads the property and handles
 *	command requests and responses.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If there are command requests in the property, they
 *	are executed.  If there are responses in the property,
 *	their information is saved for the (ostensibly waiting)
 *	"send" commands. The property is deleted.
 *
 *--------------------------------------------------------------
 */

static void
SendEventProc(clientData, eventPtr)
    ClientData clientData;	/* Display information. */	
    XEvent *eventPtr;		/* Information about event. */
{
    Tk_Window *window = (Tk_Window *) clientData;
    char *propInfo;
    register char *p;
    int result, actualFormat;
    unsigned long numItems, bytesAfter;
    Atom actualType;
    Atom commProperty = XInternAtom(Tk_Display(window),"COMM",False);

    if ((eventPtr->xproperty.atom != commProperty)
	    || (eventPtr->xproperty.state != PropertyNewValue)) {
	return;
    }

    /*
     * Read the comm property and delete it.
     */

    propInfo = NULL;
    {
      result = XGetWindowProperty(Tk_Display(window),
				  Tk_WindowId(window),
				  commProperty, 0, MAX_PROP_WORDS, True,
				  XA_STRING, &actualType, &actualFormat,
				  &numItems, &bytesAfter, (unsigned char **) &propInfo);
    }
    if (propInfo) {
      XFree(propInfo);
    }

}

#endif /*USE_TK*/

