// VXWWd/vxwWdLib.h - watchdog timer class

// Copyright 1995-1999 Wind River Systems, Inc.

// modification history
// --------------------
// 01c,08mar99,jdi  doc: fixed cross-reference.
// 01b,23feb99,fle  doc : made it refgen compliant
// 01c,21feb99,jdi  added library section, checked in documentation.
// 01b,29sep95,rhp  adjusted comments for new mangen-C++ conventions
// 01a,15jun95,srh  written.

// DESCRIPTION
// This library provides a general watchdog timer facility.  Any task may
// create a watchdog timer and use it to run a specified routine in
// the context of the system-clock ISR, after a specified delay.
//
// Once a timer has been created, it can be started with
// VXWWd::start().  The VXWWd::start() routine specifies what routine to run, a
// parameter for that routine, and the amount of time (in ticks) before
// the routine is to be called.  (The timeout value is in ticks as
// determined by the system clock; see sysClkRateSet() for more
// information.)  After the specified delay ticks have elapsed (unless
// VXWWd::cancel() is called first to cancel the timer) the timeout routine is
// invoked with the parameter specified in the VXWWd::start() call.  The
// timeout routine is invoked whether the task which started the watchdog
// is running, suspended, or deleted.
//
// The timeout routine executes only once per VXWWd::start() invocation; there
// is no need to cancel a timer with VXWWd::cancel() after it has expired, or
// in the expiration callback itself.
//
// Note that the timeout routine is invoked at interrupt level, rather than
// in the context of the task.  Thus, there are restrictions on what the
// routine may do.  Watchdog routines are constrained to the same rules
// as interrupt service routines.  For example, they may not take semaphores,
// issue other calls that may block, or use I/O system routines like printf().
//
// EXAMPLE
// In the fragment below, if maybeSlowRoutine() takes more than 60
// ticks, logMsg() will be called with the string as a parameter,
// causing the message to be printed on the console.  Normally, of
// course, more significant corrective action would be taken.
// .CS
//     VXWWd *pWd = new VXWWd;
//     pWd->start (60, logMsg, "Help, I've timed out!");
//     maybeSlowRoutine ();     /@ user-supplied routine @/
//     delete pWd;
// .CE
//
// INCLUDE FILES: vxwWdLib.h
//
// SEE ALSO: wdLib, logLib,
// .pG "Basic OS",
// .pG "C++ Development"
//
// SECTION: 1C
//

#ifndef vxwWdLib_h
#define vxwWdLib_h

#include "vxWorks.h"
#include "wdLib.h"
#include "vxwObject.h"
#include "vxwErr.h"


class VXWWd : virtual public VXWIdObject
    {
  public:

//_ VXWWd Public Constructors

////////////////////////////////////////////////////////////////////////////////
//
// VXWWd::VXWWd - construct a watchdog timer
//
// This routine creates a watchdog timer.
//
// RETURNS: N/A
//
// SEE ALSO: VXWWd::~VXWWd()

    VXWWd ()
	: wd_ (wdCreate ())
	{
	if (wd_ == 0)
	    vxwThrowErrno ();
	}

////////////////////////////////////////////////////////////////////////////////
//
// VXWWd::VXWWd - construct a watchdog timer
//
// This routine creates a watchdog timer from an existing WDOG_ID.
//
// RETURNS: N/A
//
// SEE ALSO: VXWWd::~VXWWd()

    VXWWd (WDOG_ID aWdId)
	: wd_ (aWdId)
	{
	}

////////////////////////////////////////////////////////////////////////////////
//
// VXWWd::~VXWWd - destroy a watchdog timer
//
// This routine destroys a watchdog timer.  The watchdog will be removed
// from the timer queue if it has been started.
//
// RETURNS: N/A
//
// SEE ALSO: VXWWd::VXWWd()

    ~VXWWd ()
	{
	if (wdDelete (wd_) != OK)
	    vxwThrowErrno ();
	}

//_ VXWWd Public Member Functions

////////////////////////////////////////////////////////////////////////////////
//
// VXWWd::cancel - cancel a currently counting watchdog
//
// This routine cancels a currently running watchdog timer by
// zeroing its delay count.  Watchdog timers may be canceled from interrupt
// level.
//
// RETURNS: OK, or ERROR if the watchdog timer cannot be canceled.
//
// SEE ALSO: VXWWd::start()
//

    STATUS cancel ()
	{
	return wdCancel (wd_);
	}

    STATUS show () const
	{
	return wdShow (wd_);
	}

////////////////////////////////////////////////////////////////////////////////
//
// VXWWd::start - start a watchdog timer
// 
// This routine adds a watchdog timer to the system tick queue.  The
// specified watchdog routine will be called from interrupt level after
// the specified number of ticks has elapsed.  Watchdog timers may be
// started from interrupt level.  
// 
// To replace either the timeout <delay> or the routine to be executed,
// call VXWWd::start() again; only the most recent
// VXWWd::start() on a given watchdog ID has any effect.  (If your
// application requires multiple watchdog routines, use VXWWd::VXWWd() to
// generate separate a watchdog for each.)  To cancel a watchdog
// timer before the specified tick count is reached, call VXWWd::cancel().
// 
// Watchdog timers execute only once, but some applications require
// periodically executing timers.  To achieve this effect, the timer
// routine itself must call VXWWd::start() to restart the timer on each
// invocation.
// 
// WARNING: The watchdog routine runs in the context of the
// system-clock ISR; thus, it is subject to all ISR restrictions.
// 
// RETURNS: OK, or ERROR if the watchdog timer cannot be started.
//
// SEE ALSO: VXWWd::cancel()
//

    STATUS start (int delay, FUNCPTR pRoutine, int parameter)
	{
	return wdStart (wd_, delay, pRoutine, parameter);
	}

  protected:

////////////////////////////////////////////////////////////////////////////////
//
// VXWWd::VXWWd (const VXWWd &) - copy constructor
//
// RETURNS: N/A
//
// NOMANUAL
//

    VXWWd (const VXWWd & aWd)
	: wd_ (aWd.wd_)
	{
	}

////////////////////////////////////////////////////////////////////////////////
//
// VXWWd::operator=(const VXWWd &) - assignment operator
//
// RETURNS: reference to <this>
//
// NOMANUAL
//

    VXWWd & operator = (const VXWWd & aWd)
	{
	wd_ = aWd.wd_;
	return *this;
	}
    virtual void * myValue ();
    WDOG_ID wd_;
    };

#endif /* ifndef vxwWdLib_h */
