/* ui.h - generic window-system independent UserInterface implementation */

/* Copyright 1993 Wind River Systems, Inc. */

/* modification history
-----------------------
01l,18sep98,c_s  fix const problem

01k,12nov95,c_s  added busySpinMax, busySpinCount members
01j,06nov95,c_s  added busyAnimUsec member to allow adjustment of busy 
                   animation rate.
01i,02nov95,c_s  added busy animation support; removed notice(Un)Post decls.
01h,08aug95,c_s  added handleFlags and tclSetVar methods; x{malloc...} 
                   routines.
01g,05may95,c_s  added tclVarEval method.
01f,02mar95,c_s  shutdown now take 2 parameters; TOOL_BAR uses named list.
01e,22mar95,c_s  reformatted; removed some useless material, changed 
                   noticePost prototype.
01d,12mar95,c_s  removed everything to do with license management, improved
		   shutdown handling.
01c,10nov94,c_s  changed some prototypes, added TOOL_BAR structure.
01b,21oct94,c_s  removed some local #defines and now #inlclude host.h.
01a,21apr94,c_s  written.
*/

#ifndef __INCuih
#define __INCuih

#include <tcl.h>
#include <iostream.h>
#include <sys/time.h>

#include "host.h"
#include "namelist.h"

//
// Shutdown callback codes.  The UserInterface library will call the
// shutdown callback (established with a method of UserInterface)
// with one of these values.
//


#define SHUTDOWN_USERSIGNAL		0	// SIGINT or SIGTERM was rec'd
#define SHUTDOWN_FATALSIGNAL		1	// some fatal signal was rec'd
#define SHUTDOWN_WINDOWMANAGER		2	// The WM closed the main win
#define SHUTDOWN_USERACTION		3	// (for app implementor use)
#define SHUTDOWN_SUBPROCESS		4	// a connected child exited
#define SHUTDOWN_VM_EXHAUSTED		5	// ran out of heap memory

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif /* !min */

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif /* !max */

class UserInterface
    {
    public:				// PUBLIC

    UserInterface ();				// base constructor

    void appClass				// set appClass
	(
	char *		appClass
	);

    const char *		appClass (void)		// get appClass
        {return _appClass ? _appClass : "Default";}

    virtual const char * appName (void)		// get appName
        {return appClass ();}

    void 		shutdown		// shutdown UI
        (
	int		reason,			// reason code (SHUTDOWN_*)
	int		exitcode = 0		// exit code
	);

    void		shutdownCallback 	// establish user shutdown CB
        (
	void 		(*shutdownCB) (int)	// callback
	);
    
    void 		vmExhausted (void);	// call this if no more mem

    virtual void 	setup (void);		// overridden by subclasses
    virtual void 	build (void);		// create app main window
    virtual void 	launch (void) = 0;	// enter window sys event loop

    virtual const char * handleFlags		// take care of cmd line
        (
	char *		exeName,		// what to type to start tool
        char *		toolDescrip,		// tool description
	int		returnOneArg,		// return 1st nonflag arg
	char *          extraArgName		// name of arg returned
	)
        { return ""; } 
						       
    virtual void 	tclInit (void);		// create/init Tcl interpreter

    void 		tclReadStartupFiles (void);	// read Tcl startups

    int			tclEval 		// evaluate Tcl expression
        (
	char *		cmd			// cmd to evaluate
	);

    int			tclFmtEval		// sprintf-style Tcl eval
        (
	int		buflen,			// max length of expr
	char *		fmt,			// sprintf() format
	...					// sprintf args
	);

    int			tclVarEval		// varargs-style Tcl eval
	(
	...					// strings, then 0
	);

    char *		tclSetVar		// set Tcl variable
	(
	char *		varName,		// name of var to set
	char *		varValue,		// value of var
	int		flags = TCL_GLOBAL_ONLY	// SetVar flags
	);

    int			tclEvalFile		// source in Tcl file
        (
	char *		filename		// file of Tcl commands
	);

    void		tclCreateCommand 	// extend Tcl interpreter
        (
	char *		name,			// Tcl name
	Tcl_CmdProc	proc,			// C implementation
	ClientData	clientData = 0,		// hook data passed to C
	Tcl_CmdDeleteProc deleteProc = 0	// proc delete hook
	);

    char *		tclResult (void);	// result of last Tcl eval

    virtual void	tclErrorHook 		// call this when Tcl errors
        (
	void 		(*tclErrorFunc)		// function
	    (
	    char *	result			// Tcl result
	    )
	);

    virtual char *	appTclPath (void);	// path to Tcl files

    Tcl_Interp *	tclInterp (void)	// get Tcl interpreter
        {return _tclInterp;}  

    char *		strdup			// safe strdup
        (
        char *		s
        );

    void		markTclBusy (void)
        { Tcl_AsyncMark (hBusy); };

    virtual void	busyAnimate		// perform busy animation
	(
	int		fromTimer		// !0 if from timer
	);

    int			busyAnimationOk;	// set when busy anim. safe
    int			busySpinMax;		// max times to intr. select
    int			busySpinCount;		// # of times intr'd so far
    virtual void	asyncAnimationStart (void);
    virtual void	asyncAnimationEnd (void);

    protected:				// PROTECTED

    void 		(*shutdownCallback (void)) (int)
        {return _shutdownCB;}

    void 		shutdownMessagePrint	// print shutdown message
        (
	int		reason
	);

    void 		(*_shutdownCB) (int);	// shutdown CB funcptr
    
    void *		_whandle;		// toplevel window handle

    struct timeval	busyInterval;		// interval for busy animation
    struct itimerval	itzero;			// value to cancel timers
    int			busyAnimUsec;		// usecs between updates

    private:				// PRIVATE

    void 		tclCheck (void);	// make sure Tcl is up
    void 		tclError (void);	// invoke Tcl error handler
    void 		defaultTclErrorFunc (void);	// dflt Tcl err hdlr
    void 		(*_tclErrorHook)	// user Tcl err hdlr
        (
	char *		result
	);

    char *		_appClass;		// app class name
    Tcl_Interp *	_tclInterp;		// UI Tcl interpreter

    Tcl_AsyncHandler	hBusy;			// Tcl busy async handler
    struct itimerval	itval;			// time remaining to next anim.
    };

struct TOOL_BAR
    {
    TOOL_BAR ()					// constructor
	{
	parent = 0;
	}

    NamedList		tbarItems;		// list of toolbar items
    void *		parent;			// win-sys specific; 0=default
    };

extern TOOL_BAR *pMainToolBar;
    
extern STATUS toolBarItemCreate (TOOL_BAR *pTbar, 
				 const char *name, 
				 const char *type, 
				 const char *binding,
				 const char *dragSource, 
				 const char *dropTarget);

extern STATUS toolBarItemDelete (TOOL_BAR *pTbar, char *name);

extern void appMain (void);

extern char *xstrdup (const char *);		// "safe" strdup
extern void *xmalloc (int size);		// "safe" malloc
extern void *xrealloc (void *, int size);	// "safe" realloc

extern void uiBusyFunc (void *, int);		// func to record busy state

#endif /* __INCuIh */
