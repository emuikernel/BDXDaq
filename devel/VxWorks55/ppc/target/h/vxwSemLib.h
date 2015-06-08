// VXWSem/vxwSemLib.h - semaphore classes

// Copyright 1995-1999 Wind River Systems, Inc.

// modification history
// --------------------
// 01d,08mar99,jdi  doc: fixed wrong cross-references.
// 01c,21feb99,jdi  added library section, checked in documentation.
// 01b,02oct95,rhp  documented.
// 01a,15jun95,srh  written.

// DESCRIPTION
// Semaphores are the basis for synchronization and mutual exclusion in
// VxWorks.  They are powerful in their simplicity and form the foundation
// for numerous VxWorks facilities.
//
// Different semaphore types serve different needs, and while the
// behavior of the types differs, their basic interface is the same.
// The VXWSem class provides semaphore routines common to all VxWorks
// semaphore types.  For all types, the two basic operations are
// VXWSem::take() and VXWSem::give(), the acquisition or relinquishing
// of a semaphore.
//
// Semaphore creation and initialization is handled by the following classes,
// which inherit the basic operations from `VXWSem':
//
//     `VXWBSem'  - binary semaphores
//     `VXWCSem'  - counting semaphores
//     `VXWMSem'  - mutual exclusion semaphores
//
// Two additional semaphore classes provide semaphores that operate
// over shared memory (with the optional product VxMP).  These classes
// also inherit from `VXWSmNameLib'; they are described in vxwSmLib.
// The following are the class names for these shared-memory
// semaphores:
//
//     `VXWSmBSem'  - shared-memory binary semaphores
//     `VXWSmCSem'  - shared-memory counting semaphores
//
// Binary semaphores offer the greatest speed and the broadest applicability.
//
// The `VXWSem' class provides all other semaphore operations, including
// routines for semaphore control, deletion, and information.
//
// SEMAPHORE CONTROL
// The VXWSem::take() call acquires a specified semaphore, blocking the calling
// task or making the semaphore unavailable.  All semaphore types support a
// timeout on the VXWSem::take() operation.  The timeout is specified as the
// number of ticks to remain blocked on the semaphore.  Timeouts of
// WAIT_FOREVER and NO_WAIT codify common timeouts.  If a VXWSem::take() times
// out, it returns ERROR.  Refer to the library of the specific semaphore
// type for the exact behavior of this operation.
//
// The VXWSem::give() call relinquishes a specified semaphore,
// unblocking a pended task or making the semaphore available.  Refer
// to the library of the specific semaphore type for the exact
// behavior of this operation.
//
// The VXWSem::flush() call may be used to atomically unblock all
// tasks pended on a semaphore queue; that is, it unblocks all tasks
// before any are allowed to run.  It may be thought of as a
// broadcast operation in synchronization applications.  The state of
// the semaphore is unchanged by the use of VXWSem::flush(); it is not
// analogous to VXWSem::give().
//
// SEMAPHORE DELETION
// The VXWSem::~VXWSem() destructor terminates a semaphore and
// deallocates any associated memory.  The deletion of a semaphore
// unblocks tasks pended on that semaphore; the routines which were
// pended return ERROR.  Take care when deleting semaphores,
// particularly those used for mutual exclusion, to avoid deleting a
// semaphore out from under a task that already has taken (owns) that
// semaphore.  Applications should adopt the protocol of only deleting
// semaphores that the deleting task has successfully taken.
//
// SEMAPHORE INFORMATION
// The VXWSem::info() call is a useful debugging aid, reporting all
// tasks blocked on a specified semaphore.  It provides a snapshot of
// the queue at the time of the call, but because semaphores are
// dynamic, the information may be out of date by the time it is
// available.  As with the current state of the semaphore, use of the
// queue of pended tasks should be restricted to debugging uses only.
//
// INCLUDE FILES: vxwSemLib.h
//
// SEE ALSO: VXWTask, vxwSmLib,
// .pG "Basic OS"
//
// SECTION: 1C
//


#ifndef vxwSemLib_h
#define vxwSemLib_h

#include "vxWorks.h"
#include "semLib.h"
#include "vxwObject.h"
#include "vxwErr.h"
#include "private/semLibP.h"

class VXWSem : virtual public VXWIdObject
    {
  public:

//_ VXWSem Public Constructors

///////////////////////////////////////////////////////////////////////////////
//
// VXWSem::VXWSem - build semaphore object from semaphore ID
// 
// Use this constructor to manipulate a semaphore that was not created
// using C++ interfaces.  The argument <id> is the semaphore
// identifier returned and used by the C interface to the VxWorks
// semaphore facility.
// 
// RETURNS: N/A
// 
// SEE ALSO: semLib

    VXWSem (SEM_ID id)
	: sem_ (id)
	{
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWSem::~VXWSem - delete a semaphore
//
// This destructor terminates and deallocates any memory associated with a
// specified semaphore.  Any pended tasks unblock and return ERROR.
//
// WARNING
// Take care when deleting semaphores, particularly those used for
// mutual exclusion, to avoid deleting a semaphore out from under a
// task that already has taken (owns) that semaphore.  Applications
// should adopt the protocol of only deleting semaphores that the
// deleting task has successfully taken.
//
// RETURNS: N/A
// 
// SEE ALSO
// .pG "Basic OS"

    virtual ~VXWSem ()
	{
	if (semDelete (sem_) != OK)
	    vxwThrowErrno ();
	}

//_ VXWSem Public Member Functions

///////////////////////////////////////////////////////////////////////////////
//
// VXWSem::give - give a semaphore
//
// This routine performs the give operation on a specified semaphore.
// Depending on the type of semaphore, the state of the semaphore and of the
// pending tasks may be affected.  The behavior of VXWSem::give() is
// discussed fully in the constructor description for the specific semaphore 
// type being used.
//
// RETURNS: OK.
//
// SEE ALSO
// VXWCSem::VXWCsem(), VXWBSem::VXWBsem(), VXWMSem::VXWMsem(),
// .pG "Basic OS"

    STATUS give ()
	{
	return semGive (sem_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWSem::take - take a semaphore
//
// This routine performs the take operation on a specified semaphore.
// Depending on the type of semaphore, the state of the semaphore and
// the calling task may be affected.  The behavior of VXWSem::take() is
// discussed fully in the constructor description for the specific semaphore 
// type being used.
//
// A timeout in ticks may be specified.  If a task times out, VXWSem::take()
// returns ERROR.  Timeouts of WAIT_FOREVER and NO_WAIT indicate to wait
// indefinitely or not to wait at all.
//
// When VXWSem::take() returns due to timeout, it sets the errno to
// S_objLib_OBJ_TIMEOUT (defined in objLib.h).
// 
// The VXWSem::take() routine must not be called from interrupt
// service routines.
//
// RETURNS: OK, or ERROR if the task timed out.
// 
// SEE ALSO
// VXWCSem::VXWCsem(), VXWBSem::VXWBsem(), VXWMSem::VXWMsem(),
// .pG "Basic OS"

    STATUS take (int timeout)
	{
	return semTake (sem_, timeout);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWSem::flush - unblock every task pended on a semaphore
//
// This routine atomically unblocks all tasks pended on a specified
// semaphore; that is, all tasks are unblocked before any is allowed
// to run.  The state of the underlying semaphore is unchanged.  All
// pended tasks will enter the ready queue before having a chance to
// execute.
//
// The flush operation is useful as a means of broadcast in
// synchronization applications.  Its use is illegal for mutual-exclusion
// semaphores created with VXWMSem::VXWMSem().
//
// RETURNS: OK, or ERROR if the operation is not supported.
//
// SEE ALSO
// VXWCSem::VXWCsem(), VXWBSem::VXWBsem(), VXWMSem::VXWMsem(),
// .pG "Basic OS"

    STATUS flush ()
	{
	return semFlush (sem_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWSem::id - reveal underlying semaphore ID
//
// This routine returns the semaphore ID corresponding to a semaphore
// object.  The semaphore ID is used by the C interface to VxWorks
// semaphores.
//
// RETURNS: Semaphore ID.
//
// SEE ALSO: semLib

    SEM_ID id () const
	{
	return sem_;
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWSem::info - get a list of task IDs that are blocked on a semaphore
//
// This routine reports the tasks blocked on a specified semaphore.
// Up to <maxTasks> task IDs are copied to the array specified by <idList>.
// The array is unordered.
//
// WARNING:
// There is no guarantee that all listed tasks are still valid or that new
// tasks have not been blocked by the time VXWSem::info() returns.
//
// RETURNS: The number of blocked tasks placed in <idList>.

    STATUS info (int idList[], int maxTasks) const
	{
	return semInfo (sem_, idList, maxTasks);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWSem::show - show information about a semaphore
//
// This routine displays (on standard output) the state and optionally
// the pended tasks of a semaphore.
//
// A summary of the state of the semaphore is displayed as follows:
// .CS
//     Semaphore Id        : 0x585f2
//     Semaphore Type      : BINARY
//     Task Queuing        : PRIORITY
//     Pended Tasks        : 1
//     State               : EMPTY {Count if COUNTING, Owner if MUTEX}
// .CE
//
// If <level> is 1, more detailed information is displayed.
// If tasks are blocked on the queue, they are displayed in the order
// in which they will unblock, as follows:
// .CS
//        NAME      TID    PRI DELAY
//     ---------- -------- --- -----
//     tExcTask    3fd678   0    21
//     tLogTask    3f8ac0   0   611
// .CE
//
// RETURNS: OK or ERROR.

    STATUS show (int level) const
	{
	return semShow (sem_, level);
	}

  protected:
    VXWSem ()
	{
	}
    VXWSem & operator = (const VXWSem &)
	{
	return *this;
	}
    VXWSem (const VXWSem &)
 	{
 	}
    virtual void * myValue ();
    SEM_ID	sem_;
    };

class VXWCSem : public VXWSem
    {
  public:
//_ VXWCSem Public Constructors

///////////////////////////////////////////////////////////////////////////////
//
// VXWCSem::VXWCSem - create and initialize a counting semaphore
//
// This routine allocates and initializes a counting semaphore.  The
// semaphore is initialized to the specified initial count.
//
// The <opts> parameter specifies the queuing style for blocked tasks.
// Tasks may be queued on a priority basis or a first-in-first-out basis.
// These options are SEM_Q_PRIORITY and SEM_Q_FIFO, respectively.
//
// A counting semaphore may be viewed as a cell in memory whose contents
// keep track of a count.  When a task takes a counting semaphore, using
// VXWSem::take(), subsequent action depends on the state of the count:
// .IP (1) 4
// If the count is non-zero, it is decremented and the calling task
// continues executing.
// .IP (2)
// If the count is zero, the task is blocked, pending the availability
// of the semaphore.  If a timeout is specified and the timeout expires, the
// pended task is removed from the queue of pended tasks and enters the
// ready state with an ERROR status.  A pended task is ineligible for CPU
// allocation.  Any number of tasks may be pended simultaneously on the same
// counting semaphore.
// .LP
// When a task gives a semaphore, using VXWSem::give(), the next
// available task in the pend queue is unblocked.  If no task is
// pending on this semaphore, the semaphore count is incremented.
// Note that if a semaphore is given, and a task is unblocked that is
// of higher priority than the task that called VXWSem::give(), the
// unblocked task preempts the calling task. 
// 
// A VXWSem::flush() on a counting semaphore atomically unblocks all pended
// tasks in the semaphore queue.  Thus, all tasks are made ready before any
// task actually executes.  The count of the semaphore remains unchanged.
// 
// INTERRUPT USAGE
// Counting semaphores may be given but not taken from interrupt level.
// 
// CAVEATS
// There is no mechanism to give back or reclaim semaphores automatically when
// tasks are suspended or deleted.  Such a mechanism, though desirable, is not
// currently feasible.  Without explicit knowledge of the state of the guarded
// resource or region, reckless automatic reclamation of a semaphore could
// leave the resource in a partial state.  Thus, if a task ceases execution
// unexpectedly, as with a bus error, currently owned semaphores are not 
// given back, effectively leaving a resource permanently unavailable.  The
// mutual-exclusion semaphores provided by `VXWMSem' offer protection from
// unexpected task deletion.
// 
// RETURNS: N/A

    VXWCSem (int opts, int count)
	: VXWSem (semCCreate (opts, count))
	{
	if (sem_ == 0)
	    vxwThrowErrno ();
	}
  protected:
    VXWCSem ()
	{
	}
    VXWCSem (const VXWCSem &)
	{
	}
    VXWCSem & operator = (const VXWCSem &)
	{
	return *this;
	}
    };

class VXWBSem : public VXWSem
    {
  public:
//_ VXWBSem Public Constructors

///////////////////////////////////////////////////////////////////////////////
//
// VXWBSem::VXWBSem - create and initialize a binary semaphore
// 
// This routine allocates and initializes a binary semaphore.  The
// semaphore is initialized to the state <iState>:  either SEM_FULL
// (1) or SEM_EMPTY (0).
//
// The <opts> parameter specifies the queuing style for blocked tasks.
// Tasks can be queued on a priority basis or a first-in-first-out basis.
// These options are SEM_Q_PRIORITY and SEM_Q_FIFO, respectively.
//
// Binary semaphores are the most versatile, efficient, and conceptually
// simple type of semaphore.  They can be used to: (1) control mutually
// exclusive access to shared devices or data structures, or (2) synchronize
// multiple tasks, or task-level and interrupt-level processes.  Binary
// semaphores form the foundation of numerous VxWorks facilities.
// 
// A binary semaphore can be viewed as a cell in memory whose contents
// are in one of two states, full or empty.  When a task takes a
// binary semaphore, using VXWSem::take(), subsequent action depends
// on the state of the semaphore:
// .IP (1) 4
// If the semaphore is full, the semaphore is made empty, and the calling task
// continues executing.
// .IP (2)
// If the semaphore is empty, the task is blocked, pending the
// availability of the semaphore.  If a timeout is specified and the timeout
// expires, the pended task is removed from the queue of pended tasks
// and enters the ready state with an ERROR status.  A pended task
// is ineligible for CPU allocation.  Any number of tasks may be pended
// simultaneously on the same binary semaphore.
// .LP
// When a task gives a binary semaphore, using VXWSem::give(), the
// next available task in the pend queue is unblocked.  If no task is
// pending on this semaphore, the semaphore becomes full.  Note that
// if a semaphore is given, and a task is unblocked that is of higher
// priority than the task that called VXWSem::give(), the unblocked
// task preempts the calling task.
// 
// MUTUAL EXCLUSION
// To use a binary semaphore as a means of mutual exclusion, first create it
// with an initial state of full.
// 
// Then guard a critical section or resource by taking the semaphore with
// VXWSem::take(), and exit the section or release the resource by giving
// the semaphore with VXWSem::give().
// 
// While there is no restriction on the same semaphore being given,
// taken, or flushed by multiple tasks, it is important to ensure the
// proper functionality of the mutual-exclusion construct.  While
// there is no danger in any number of processes taking a semaphore,
// the giving of a semaphore should be more carefully controlled.  If
// a semaphore is given by a task that did not take it, mutual
// exclusion could be lost.
// 
// SYNCHRONIZATION
// To use a binary semaphore as a means of synchronization, create it
// with an initial state of empty.  A task blocks by taking a
// semaphore at a synchronization point, and it remains blocked until
// the semaphore is given by another task or interrupt service
// routine.
// 
// Synchronization with interrupt service routines is a particularly
// common need.  Binary semaphores can be given, but not taken, from
// interrupt level.  Thus, a task can block at a synchronization point
// with VXWSem::take(), and an interrupt service routine can unblock
// that task with VXWSem::give().
// 
// A semFlush() on a binary semaphore atomically unblocks all pended
// tasks in the semaphore queue; that is, all tasks are unblocked at once,
// before any actually execute.
// 
// CAVEATS
// There is no mechanism to give back or reclaim semaphores automatically when
// tasks are suspended or deleted.  Such a mechanism, though desirable, is not
// currently feasible.  Without explicit knowledge of the state of the guarded
// resource or region, reckless automatic reclamation of a semaphore could
// leave the resource in a partial state.  Thus, if a task ceases execution
// unexpectedly, as with a bus error, currently owned semaphores will not be
// given back, effectively leaving a resource permanently unavailable.  The
// mutual-exclusion semaphores provided by `VXWMSem' offer protection from
// unexpected task deletion.
// 
// RETURNS: N/A

    VXWBSem (int opts, SEM_B_STATE iState)
	: VXWSem (semBCreate (opts, iState))
	{
	if (sem_ == 0)
	    vxwThrowErrno ();
	}
  protected:
    VXWBSem ()
	{
	}
    VXWBSem (const VXWBSem &)
	{
	}
    VXWBSem & operator = (const VXWBSem &)
	{
	return *this;
	}
    };

class VXWMSem : public VXWSem
    {
  public:
//_ VXWMSem Public Constructors

///////////////////////////////////////////////////////////////////////////////
//
// VXWMSem::VXWMSem - create and initialize a mutual-exclusion semaphore
//
// This routine allocates and initializes a mutual-exclusion semaphore.  The
// semaphore state is initialized to full.
//
// Semaphore options include the following:
// .iP SEM_Q_PRIORITY 8
// Queue pended tasks on the basis of their priority.
// .iP SEM_Q_FIFO
// Queue pended tasks on a first-in-first-out basis.
// .iP SEM_DELETE_SAFE
// Protect a task that owns the semaphore from unexpected deletion.
// This option enables an implicit taskSafe() for each VXWSem::take(),
// and an implicit taskUnsafe() for each VXWSem::give().
// .iP SEM_INVERSION_SAFE
// Protect the system from priority inversion.  With this option, the
// task owning the semaphore executes at the highest priority of the
// tasks pended on the semaphore, if that is higher than its current
// priority.  This option must be accompanied by the SEM_Q_PRIORITY
// queuing mode.
// .LP
// 
// Mutual-exclusion semaphores offer convenient options suited for
// situations that require mutually exclusive access to resources.
// Typical applications include sharing devices and protecting data
// structures.  Mutual-exclusion semaphores are used by many
// higher-level VxWorks facilities.
// 
// The mutual-exclusion semaphore is a specialized version of the
// binary semaphore, designed to address issues inherent in mutual
// exclusion, such as recursive access to resources, priority
// inversion, and deletion safety.  The fundamental behavior of the
// mutual-exclusion semaphore is identical to the binary semaphore as
// described for VXWBSem::VXWBSem(), except for the following
// restrictions:
// 
// .iP
// It can only be used for mutual exclusion.
// .iP
// It can only be given by the task that took it.
// .iP
// It may not be taken or given from interrupt level.
// .iP
// The VXWSem::flush() operation is illegal.
// .LP
// 
// These last two operations have no meaning in mutual-exclusion situations.
// 
// RECURSIVE RESOURCE ACCESS
// 
// A special feature of the mutual-exclusion semaphore is that it may
// be taken "recursively;" that is, it can be taken more than once by
// the task that owns it before finally being released.  Recursion is
// useful for a set of routines that need mutually exclusive access to
// a resource, but may need to call each other.
// 
// Recursion is possible because the system keeps track of which task
// currently owns a mutual-exclusion semaphore.  Before being
// released, a mutual-exclusion semaphore taken recursively must be
// given the same number of times it has been taken; this is tracked
// by means of a count which increments with each VXWSem::take()
// and decrements with each VXWSem::give().
// 
// PRIORITY-INVERSION SAFETY
// If the option SEM_INVERSION_SAFE is selected, the library adopts a
// priority-inheritance protocol to resolve potential occurrences of
// "priority inversion," a problem stemming from the use semaphores for
// mutual exclusion.  Priority inversion arises when a higher-priority task
// is forced to wait an indefinite period of time for the completion of a
// lower-priority task.
// 
// Consider the following scenario: T1, T2, and T3 are tasks of high,
// medium, and low priority, respectively.  T3 has acquired some
// resource by taking its associated semaphore.  When T1 preempts T3
// and contends for the resource by taking the same semaphore, it
// becomes blocked.  If we could be assured that T1 would be blocked
// no longer than the time it normally takes T3 to finish with the
// resource, the situation would not be problematic.  However, the
// low-priority task is vulnerable to preemption by medium-priority
// tasks; a preempting task, T2, could inhibit T3 from relinquishing
// the resource.  This condition could persist, blocking T1 for an
// indefinite period of time.
// 
// The priority-inheritance protocol solves the problem of priority
// inversion by elevating the priority of T3 to the priority of T1
// during the time T1 is blocked on T3.  This protects T3, and
// indirectly T1, from preemption by T2.  Stated more generally, the
// priority-inheritance protocol assures that a task which owns a
// resource executes at the priority of the highest priority task
// blocked on that resource.  When execution is complete, the task
// gives up the resource and returns to its normal, or standard,
// priority.  Hence, the "inheriting" task is protected from
// preemption by any intermediate-priority tasks.
// 
// The priority-inheritance protocol also takes into consideration a task's
// ownership of more than one mutual-exclusion semaphore at a time.  Such a
// task will execute at the priority of the highest priority task blocked on
// any of the resources it owns.  The task returns to its normal priority
// only after relinquishing all of its mutual-exclusion semaphores that have
// the inversion-safety option enabled.
// 
// SEMAPHORE DELETION
// The VXWSem::~VXWSem() destructor terminates a semaphore and deallocates any
// associated memory.  The deletion of a semaphore unblocks tasks pended
// on that semaphore; the routines which were pended return ERROR.  Take
// special care when deleting mutual-exclusion semaphores to avoid
// deleting a semaphore out from under a task that already owns (has
// taken) that semaphore.  Applications should adopt the protocol of only
// deleting semaphores that the deleting task owns.
// 
// TASK-DELETION SAFETY
// If the option SEM_DELETE_SAFE is selected, the task owning the semaphore
// is protected from deletion as long as it owns the semaphore.  This
// solves another problem endemic to mutual exclusion.  Deleting a task
// executing in a critical region can be catastrophic.  The resource could be
// left in a corrupted state and the semaphore guarding the resource would be
// unavailable, effectively shutting off all access to the resource.
// 
// As discussed in taskLib, the primitives taskSafe() and taskUnsafe()
// offer one solution, but as this type of protection goes hand in
// hand with mutual exclusion, the mutual-exclusion semaphore provides
// the option SEM_DELETE_SAFE, which enables an implicit taskSafe()
// with each VXWSem::take(), and a taskUnsafe() with each
// VXWSem::give().  This convenience is also more efficient, as the
// resulting code requires fewer entrances to the kernel.
// 
// CAVEATS
// There is no mechanism to give back or reclaim semaphores automatically when
// tasks are suspended or deleted.  Such a mechanism, though desirable, is not
// currently feasible.  Without explicit knowledge of the state of the guarded
// resource or region, reckless automatic reclamation of a semaphore could
// leave the resource in a partial state.  Thus if a task ceases execution
// unexpectedly, as with a bus error, currently owned semaphores will not be
// given back, effectively leaving a resource permanently unavailable.  The
// SEM_DELETE_SAFE option partially protects an application, to the extent
// that unexpected deletions will be deferred until the resource is released.
// 
// RETURNS: N/A
//
// SEE ALSO: taskSafe(), taskUnsafe()

    VXWMSem (int opts)
	: VXWSem (semMCreate (opts))
	{
	if (sem_ == 0)
	    vxwThrowErrno ();
	}

//_ VXWMSem Public Member Functions

//////////////////////////////////////////////////////////////////////////////
//
// VXWMSem::giveForce - give a mutual-exclusion semaphore without restrictions
//
// This routine gives a mutual-exclusion semaphore, regardless of semaphore
// ownership.  It is intended as a debugging aid only.
//
// The routine is particularly useful when a task dies while holding some
// mutual-exclusion semaphore, because the semaphore can be resurrected.  The
// routine gives the semaphore to the next task in the pend queue, or makes
// the semaphore full if no tasks are pending.  In effect, execution 
// continues as if the task owning the semaphore had actually given the
// semaphore.
//
// CAVEATS
// Use this routine should only as a debugging aid, when the condition of
// the semaphore is known.
//
// RETURNS: OK.
//
// SEE ALSO: VXWSem::give()

    STATUS giveForce ()
	{
	return semMGiveForce (sem_);
	}
  protected:
    VXWMSem ()
	{
	}
    VXWMSem (const VXWMSem &)
	{
	}
    VXWMSem & operator = (const VXWMSem &)
	{
	return *this;
	}
    };

#endif /* ifndef vxwSemLib_h */
