// VXWTask/vxwTaskLib.h - task class

// Copyright 1995-1999 Wind River Systems, Inc. 

// modification history
// --------------------
// 01c,08mar99,jdi  doc: fixed wrong cross-references.
// 01b,23feb99,fle  doc : made it refgen compliant
// 01c,21feb99,jdi  added library section, checked in documentation.
// 01b,29sep95,rhp  documented.
// 01a,15jun95,srh  written.

// DESCRIPTION
// This library provides the interface to the VxWorks task management
// facilities.  This class library provides task control services,
// programmatic access to task information and debugging features, and
// higher-level task information display routines.
//
// TASK CREATION
// Tasks are created with the constructor VXWTask::VXWTask().  Task
// creation consists of the following:  allocation of memory for the stack
// and task control block (WIND_TCB), initialization of the WIND_TCB, and
// activation of the WIND_TCB.  Special needs may require the use of the
// lower-level method VXWTask::activate().
//
// Tasks in VxWorks execute in the most privileged state of the underlying
// architecture.  In a shared address space, processor privilege offers no
// protection advantages and actually hinders performance.
//
// There is no limit to the number of tasks created in VxWorks, as long as
// sufficient memory is available to satisfy allocation requirements.
//
// TASK DELETION
// If a task exits its "main" routine, specified during task creation,
// the kernel implicitly calls exit() to delete the task.  Tasks can
// be deleted with the exit() routine, or explicitly with the <delete>
// operator, which arranges to call the class destructor
// VXWTask::~VXWTask().
//
// Task deletion must be handled with extreme care, due to the inherent
// difficulties of resource reclamation.  Deleting a task that owns a
// critical resource can cripple the system, since the resource may no longer
// be available.  Simply returning a resource to an available state is not a
// viable solution, since the system can make no assumption as to the state
// of a particular resource at the time a task is deleted.
//
// A task can protect itself from deletion by taking a
// mutual-exclusion semaphore created with the SEM_DELETE_SAFE option
// (see vxwSemLib for more information).  Many VxWorks system resources
// are protected in this manner, and application designers may wish to
// consider this facility where dynamic task deletion is a
// possibility.
//
// The sigLib facility may also be used to allow a task to
// execute clean-up code before actually expiring.
//
// TASK CONTROL
// The following methods control task state: VXWTask::resume(),
// VXWTask::suspend(), VXWTask::restart(), VXWTask::priority(),
// and VXWTask::registers().
//
// TASK SCHEDULING
// VxWorks schedules tasks on the basis of priority.  Tasks may have
// priorities ranging from 0, the highest priority, to 255, the lowest
// priority.  The priority of a task in VxWorks is dynamic, and an existing
// task's priority can be changed or examined using VXWTask:priority().
//
// INCLUDE FILES: taskLib.h
//
// SEE ALSO: taskLib, taskHookLib, VXWSem,
// kernelLib,
// .pG "Basic OS"
//
// SECTION: 1C
//

#ifndef vxwTaskLib_h
#define vxwTaskLib_h

#include "vxWorks.h"
#include "envLib.h"
#include "errnoLib.h"
#include "sigLib.h"
#include "taskArchLib.h"
#include "taskLib.h"
#include "taskVarLib.h"
#include "vxwObject.h"
#include "vxwErr.h"

class VXWTask : virtual public VXWIdObject
    {
  public:
//_ VXWTask Public Constructors

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::VXWTask - initialize a task object
//
// This constructor creates a task object from the task ID of an existing task.
// Because of the VxWorks convention that a task ID of 0 refers to the calling
// task, this constructor can be used to derive a task object for the
// calling task, as follows:
// .CS
// myTask = VXWTask (0);
// .CE
//
// RETURNS: N/A
//
// SEE ALSO: taskLib, VXWTask::~VXWTask(), sp()

    VXWTask (int tid)
	: tid_ (tid), managePrivateEnv_ (FALSE)
	{
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::VXWTask - create and spawn a task
//
// This constructor creates and activates a new task with a specified priority
// and options.
//
// A task may be assigned a name as a debugging aid.  This name appears
// in displays generated by various system information facilities such as
// i().  The name may be of arbitrary length and content, but the current
// VxWorks convention is to limit task names to ten characters and prefix
// them with a "t".  If <name> is specified as NULL, an ASCII name is
// assigned to the task of the form "t<n>" where <n> is an integer which
// increments as new tasks are spawned.
//
// The only resource allocated to a spawned task is a stack of a specified
// size <stackSize>, which is allocated from the system memory partition.
// Stack size should be an even integer.  A task control block (TCB) is
// carved from the stack, as well as any memory required by the task name.
// The remaining memory is the task's stack and every byte is filled with the
// value 0xEE for the checkStack() facility.  See the manual entry for
// checkStack() for stack-size checking aids.
//
// The entry address <entryPt> is the address of the "main" routine of the task.
// The routine is called after the C environment is set up.
// The specified routine is called with the ten arguments provided.
// Should the specified main routine return, a call to exit() is
// made automatically.
//
// Note that ten (and only ten) arguments must be passed for the
// spawned function.
//
// Bits in the options argument may be set to run with the following modes:
// .iP VX_FP_TASK 22
// execute with floating-point coprocessor support.
// .iP VX_PRIVATE_ENV
// include private environment support.
// .iP VX_NO_STACK_FILL
// do not fill the stack for use by checkstack().
// .iP VX_UNBREAKABLE
// do not allow breakpoint debugging.
// .LP
// See the definitions in taskLib.h.
//
// RETURNS: N/A
//
// SEE ALSO: VXWTask::~VXWTask(), VXWTask::activate(), sp(),
// .pG "Basic OS"

    VXWTask (char       * name,
	     int	  priority,
	     int	  options,
	     int	  stackSize,
	     FUNCPTR	  entryPoint,
	     int	  arg1=0,
	     int	  arg2=0,
	     int	  arg3=0,
	     int	  arg4=0,
	     int	  arg5=0,
	     int	  arg6=0,
	     int	  arg7=0,
	     int	  arg8=0,
	     int	  arg9=0,
	     int	  arg10=0)
	: tid_ (taskSpawn (name,
			   priority,
			   options,
			   stackSize,
			   entryPoint,
			   arg1,
			   arg2,
			   arg3,
			   arg4,
			   arg5,
			   arg6,
			   arg7,
			   arg8,
			   arg9,
			   arg10)), managePrivateEnv_ (FALSE)
	{
	if (tid_ == ERROR)
	    vxwThrowErrno ();
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::VXWTask - initialize a task with a specified stack
//
// This constructor initializes user-specified regions of memory for a task
// stack and control block instead of allocating them from memory.
// This constructor uses the specified pointers to the WIND_TCB
// and stack as the components of the task.  This allows, for example, the
// initialization of a static WIND_TCB variable.  It also allows for special
// stack positioning as a debugging aid.
//
// As in other constructors, a task may be given a name.  If no name is
// specified, this constructor creates a task without a name (rather 
// than assigning a default name).  
//
// Other arguments are the same as in the previous constructor.  This
// constructor does not activate the task.  This must be done by calling
// VXWTask::activate().
//
// Normally, tasks should be started using the previous constructor rather
// than this one,
// except when additional control is required for task memory allocation or
// a separate task activation is desired.
//
// RETURNS: OK, or ERROR if the task cannot be initialized.
//
// SEE ALSO: VXWTask::activate()

    VXWTask (WIND_TCB	* pTcb,
	     char       * name,
	     int	  priority,
	     int	  options,
	     char	* pStackBase,
	     int	  stackSize,
	     FUNCPTR	  entryPoint,
	     int	  arg1=0,
	     int	  arg2=0,
	     int	  arg3=0,
	     int	  arg4=0,
	     int	  arg5=0,
	     int	  arg6=0,
	     int	  arg7=0,
	     int	  arg8=0,
	     int	  arg9=0,
	     int	  arg10=0)
	: tid_ (-1), managePrivateEnv_ (FALSE)
	{
	if (taskInit (pTcb,
		      name,
		      priority,
		      options,
		      pStackBase,
		      stackSize,
		      entryPoint,
		      arg1,
		      arg2,
		      arg3,
		      arg4,
		      arg5,
		      arg6,
		      arg7,
		      arg8,
		      arg9,
		      arg10) != OK)
	    vxwThrowErrno ();
	else
	    tid_ = int (pTcb);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::~VXWTask - delete a task
//
// This destructor causes the task to cease to exist and deallocates the
// stack and WIND_TCB memory resources.  Upon deletion, all routines specified
// by taskDeleteHookAdd() are called in the context of the deleting task.
//
// RETURNS: N/A
//
// SEE ALSO: excLib, taskDeleteHookAdd(), VXWTask::VXWTask(),
// .pG "Basic OS"

    virtual ~VXWTask ()
	{
	if (managePrivateEnv_ && envPrivateDestroy (tid_) != OK)
	    vxwThrowErrno ();
	if (taskDelete (tid_) != OK)
	    vxwThrowErrno ();
	}

//_ VXWTask Public Member Functions

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::activate - activate a task
//
// This routine activates tasks created by the form of the constructor 
// that does not automatically activate a task.  Without activation, a
// task is ineligible for CPU allocation by the scheduler.
// 
// RETURNS: OK, or ERROR if the task cannot be activated.
//
// SEE ALSO: VXWTask::VXWTask()

    STATUS activate ()
	{
	return taskActivate (tid_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::deleteForce - delete a task without restriction
//
// This routine deletes a task even if the task is protected from deletion.  
// It is similar to VXWTask::~VXWTask().  Upon deletion, all routines
// specified by taskDeleteHookAdd() are called in the context of the
// deleting task.
//
// CAVEATS
// This routine is intended as a debugging aid, and is generally inappropriate
// for applications.  Disregarding a task's deletion protection could leave the
// the system in an unstable state or lead to system deadlock.
//
// The system does not protect against simultaneous VXWTask:deleteForce() calls.
// Such a situation could leave the system in an unstable state.
//
// RETURNS: OK, or ERROR if the task cannot be deleted.
//
// SEE ALSO: taskDeleteHookAdd(), VXWTask::~VXWTask()

    STATUS deleteForce ()
	{
	return taskDeleteForce (tid_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::envCreate - create a private environment
//
// This routine creates a private set of environment variables for a specified
// task, if the environment variable task create hook is not installed.
//
// RETURNS: OK, or ERROR if memory is insufficient.
//
// SEE ALSO: envLib

    STATUS envCreate (int envSource)
	{
	STATUS rval = OK;
	if (envPrivateCreate (tid_, envSource) != OK)
	    rval = ERROR;
	managePrivateEnv_ = TRUE;
	return rval;
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::errNo - retrieve error status value
//
// This routine gets the error status for the task.
//
// RETURNS:
// The error status value contained in `errno'.
//

    int errNo () const
	{
	return errnoOfTaskGet (tid_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::errNo - set error status value 
//
// This routine sets the error status value for its task.
//
// RETURNS: OK.

    STATUS errNo (int errorValue)
	{
	return errnoOfTaskSet (tid_, errorValue);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::id - reveal task ID 
//
// This routine reveals the task ID for its task. The task ID is necessary
// to call C routines that affect or inquire on a task.
//
// RETURNS: task ID
//
// SEE ALSO: taskLib

    int id () const
	{
	return tid_;
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::info - get information about a task
//
// This routine fills in a specified task descriptor (TASK_DESC) for its
// task.  The information in the task descriptor is, for the most
// part, a copy of information kept in the task control block (WIND_TCB).
// The TASK_DESC structure is useful for common information and avoids
// dealing directly with the unwieldy WIND_TCB.
//
// NOTE
// Examination of WIND_TCBs should be restricted to debugging aids.
//
// RETURNS: OK

    STATUS info (TASK_DESC *pTaskDesc) const
	{
	return taskInfoGet (tid_, pTaskDesc);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::isReady - check if task is ready to run
//
// This routine tests the status field of its task to determine
// whether the task is ready to run.
//
// RETURNS: TRUE if the task is ready, otherwise FALSE.

    BOOL isReady () const
	{
	return taskIsReady (tid_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::isSuspended - check if task is suspended
//
// This routine tests the status field of its task to determine
// whether the task is suspended.
//
// RETURNS: TRUE if the task is suspended, otherwise FALSE.

    BOOL isSuspended () const
	{
	return taskIsSuspended (tid_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::kill - send a signal to task
//
// This routine sends a signal <signo> to its task.
//
// RETURNS: OK (0), or ERROR (-1) if the signal number is invalid.
//
// ERRNO: EINVAL

    int kill (int signo)
	{
	return ::kill (tid_, signo);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::name - get the name associated with a task ID
//
// This routine returns a pointer to the name of its task, if
// it has a name; otherwise it returns NULL.
//
// RETURNS: A pointer to the task name, or NULL.

    char * name () const
	{
	return taskName (tid_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::options - examine task options
//
// This routine gets the current execution options of its task.
// The option bits returned indicate the following modes:
// .iP VX_FP_TASK 22
// execute with floating-point coprocessor support.
// .iP VX_PRIVATE_ENV
// include private environment support (see envLib).
// .iP VX_NO_STACK_FILL
// do not fill the stack for use by checkstack().
// .iP VX_UNBREAKABLE
// do not allow breakpoint debugging.
// .LP
// For definitions, see taskLib.h.
//
// RETURNS: OK.

    STATUS options (int *pOptions) const
	{
	return taskOptionsGet (tid_, pOptions);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::options - change task options
//
// This routine changes the execution options of its task.
// The only option that can be changed after a task has been created is:
// .iP VX_UNBREAKABLE 21
// do not allow breakpoint debugging.
// .LP
// For definitions, see taskLib.h.
//
// RETURNS: OK.

    STATUS options (int mask, int newOptions)
	{
	return taskOptionsSet (tid_, mask, newOptions);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::priority - examine the priority of task
//
// This routine reports the current priority of its task.
// The current priority is copied to the integer pointed to by <pPriority>.
//
// RETURNS: OK.

    STATUS priority (int *pPriority) const
	{
	return taskPriorityGet (tid_, pPriority);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::priority - change the priority of a task
//
// This routine changes its task's priority to a specified priority.
// Priorities range from 0, the highest priority, to 255, the lowest priority.
//
// RETURNS: OK.

    STATUS priority (int newPriority)
	{
	return taskPrioritySet (tid_, newPriority);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::registers - set a task's registers
//
// This routine loads a specified register set <pRegs> into the
// task's TCB.
//
// NOTE
// This routine only works well if the task is known not to be in the ready
// state.  Suspending the task before changing the register set is
// recommended.
//
// RETURNS: OK.
//
// SEE ALSO: VXWTask::suspend()

    STATUS registers (const REG_SET *pRegs)
	{
	return taskRegsSet (tid_, (REG_SET *)(pRegs));
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::registers - get task registers from the TCB
//
// This routine gathers task information kept in the TCB.  It copies the
// contents of the task's registers to the register structure <pRegs>.
//
// NOTE
// This routine only works well if the task is known to be in a stable,
// non-executing state.  Self-examination, for instance, is not advisable,
// as results are unpredictable.
//
// RETURNS: OK.
//
// SEE ALSO: VXWTask::suspend()

    STATUS registers (REG_SET *pRegs) const
	{
	return taskRegsGet (tid_, pRegs);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::restart - restart task
//
// This routine "restarts" its task.  The task is first terminated, and then
// reinitialized with the same ID, priority, options, original entry point,
// stack size, and parameters it had when it was terminated.  Self-restarting
// of a calling task is performed by the exception task.  
//
// NOTE
// If the task has modified any of its start-up parameters, the restarted
// task will start with the changed values.
//
// RETURNS: OK, or ERROR if the task could not be restarted.

    STATUS restart ()
	{
	return taskRestart (tid_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::resume - resume task
//
// This routine resumes its task.  Suspension is cleared, and
// the task operates in the remaining state.
//
// RETURNS: OK, or ERROR if the task cannot be resumed.

    STATUS resume ()
	{
	return taskResume (tid_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::show - display the contents of task registers
//
// This routine displays the register contents of its task
// on standard output.
//
// EXAMPLE: The following shell command line displays the register of a task
// `vxwT28':
// .CS 4
// -> vxwT28.show ()
// .CE
//
// The example prints on standard output a display like the following
// (68000 family):
// .CS
// d0     =        0   d1     =        0    d2    =    578fe    d3     =        1
// d4     =   3e84e1   d5     =   3e8568    d6    =        0    d7     = ffffffff
// a0     =        0   a1     =        0    a2    =    4f06c    a3     =    578d0
// a4     =   3fffc4   a5     =        0    fp    =   3e844c    sp     =   3e842c
// sr     =     3000   pc     =    4f0f2
// .CE
//
// RETURNS: N/A

    void show () const
	{
	taskRegsShow (tid_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::show - display task information from TCBs
//
// This routine displays the contents of its task's task control block (TCB).
// If <level> is 1, it also displays task options
// and registers.  If <level> is 2, it displays all tasks.
//
// The TCB display contains the following fields:
//
// .TS
// tab(|);
// lf3 lf3
// l l .
// Field  | Meaning
// _
// NAME   | Task name
// ENTRY  | Symbol name or address where task began execution
// TID    | Task ID
// PRI    | Priority
// STATUS | Task status, as formatted by taskStatusString()
// PC     | Program counter
// SP     | Stack pointer
// ERRNO  | Most recent error code for this task
// DELAY  | If task is delayed, number of clock ticks remaining in delay (0 otherwise)
// .TE
//
// EXAMPLE:
// The following example shows the TCB contents for a task named `t28':
// .CS
// 
//     NAME        ENTRY    TID    PRI  STATUS      PC       SP    ERRNO  DELAY
//   ---------- --------- -------- --- --------- -------- -------- ------ -----
//   t28        _appStart 20efcac   1 READY      201dc90  20ef980      0     0
// 
//   stack: base 0x20efcac  end 0x20ed59c  size 9532   high 1452   margin 8080
// 
//   options: 0x1e
//   VX_UNBREAKABLE      VX_DEALLOC_STACK    VX_FP_TASK         VX_STDIO
// 
// 
//   D0 =       0   D4 =       0   A0 =       0   A4 =        0
//   D1 =       0   D5 =       0   A1 =       0   A5 =  203a084   SR =     3000
//   D2 =       0   D6 =       0   A2 =       0   A6 =  20ef9a0   PC =  2038614
//   D3 =       0   D7 =       0   A3 =       0   A7 =  20ef980
// .CE
//
// RETURNS: N/A
//
// SEE ALSO:
// VXWTaskstatusString(),
// .tG "The Tornado Shell"

    STATUS show (int level) const
	{
	return taskShow (tid_, level);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::sigqueue - send a queued signal to task
// 
// The routine sigqueue() sends to its task the signal specified by <signo> with
// the signal-parameter value specified by <value>.
//
// RETURNS: OK (0), or ERROR (-1) if the signal number is invalid,
// or if there are no queued-signal buffers available.
//
// ERRNO: EINVAL EAGAIN

    int sigqueue (int signo, const union sigval value)
	{
	return ::sigqueue (tid_, signo, value);
	}

#if   (CPU_FAMILY == MC680X0)
///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::SRSet - set the task status register (MC680x0, MIPS, i386/i486)
//
// SYNOPSIS (I80X86)
// .CS
// STATUS SRSet
//     (
//     UINT sr
//     )
// .CE
//
// SYNOPSIS (MIPS)
// .CS
// STATUS SRSet
//     (
//     UINT32 sr
//     )
// .CE
//
// This routine sets the status register of a task that is not running;
// that is, you must not call this->SRSet().
// Debugging facilities use this routine to set the trace bit in the 
// status register of a task that is being single-stepped.
//
// RETURNS: OK.

    STATUS SRSet (UINT16 sr)
#elif (CPU_FAMILY == I80X86)
    STATUS SRSet (UINT sr)
#elif (CPU_FAMILY == MIPS)
    STATUS SRSet (UINT32 sr)
#endif
#if (CPU_FAMILY == MC680X0) || (CPU_FAMILY == I80X86) || (CPU_FAMILY == MIPS)
	{
	return taskSRSet (tid_, sr);
	}
#endif



///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::statusString - get task status as a string
//
// This routine deciphers the WIND task status word in the TCB for its
// task, and copies the appropriate string to <pString>.
// 
// The formatted string is one of the following:
//
// .TS
// tab(|);
// lf3 lf3
// l l .
// String   | Meaning
// _
// READY    | Task is not waiting for any resource other than the CPU.
// PEND     | Task is blocked due to the unavailability of some resource.
// DELAY    | Task is asleep for some duration.
// SUSPEND  | Task is unavailable for execution (but not suspended, delayed, or pended).
// DELAY+S  | Task is both delayed and suspended.
// PEND+S   | Task is both pended and suspended.
// PEND+T   | Task is pended with a timeout.
// PEND+S+T | Task is pended with a timeout, and also suspended.
// \&...+I  | Task has inherited priority (+I may be appended to any string above).
// DEAD     | Task no longer exists.
// .TE
//
// RETURNS: OK.

    STATUS statusString (char *pString) const
	{
	return taskStatusString (tid_, pString);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::suspend - suspend task
//
// This routine suspends its task.  Suspension is additive:  thus, tasks
// can be delayed and suspended, or pended and suspended.  Suspended, delayed
// tasks whose delays expire remain suspended.  Likewise, suspended,
// pended tasks that unblock remain suspended only.
//
// Care should be taken with asynchronous use of this facility.  The 
// task is suspended regardless of its current state.  The task could, for
// instance, have mutual exclusion to some system resource, such as the network
// or system memory partition.  If suspended during such a time, the facilities
// engaged are unavailable, and the situation often ends in deadlock.
//
// This routine is the basis of the debugging and exception handling packages.
// However, as a synchronization mechanism, this facility should be rejected 
// in favor of the more general semaphore facility.
//
// RETURNS: OK, or ERROR if the task cannot be suspended.

    STATUS suspend ()
	{
	return taskSuspend (tid_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::tcb - get the task control block
//
// This routine returns a pointer to the task control block (WIND_TCB) for
// its task.  Although all task state information is contained in the
// TCB, users must not modify it directly.  To change registers, for instance,
// use VXWTask::registers().
//
// RETURNS: A pointer to a WIND_TCB.

    WIND_TCB * tcb () const
	{
	return taskTcb (tid_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::varAdd - add a task variable to task
//
// This routine adds a specified variable <pVar> (4-byte memory location) to
// its task's context.  After calling this routine, the variable is
// private to the task.  The task can access and modify the variable, but
// the modifications are not visible to other tasks, and other tasks'
// modifications to that variable do not affect the value seen by the
// task.  This is accomplished by saving and restoring the variable's initial
// value each time a task switch occurs to or from the calling task.
//
// This facility can be used when a routine is to be spawned repeatedly as
// several independent tasks.  Although each task has its own stack,
// and thus separate stack variables, they all share the same static and
// global variables.  To make a variable \f2not\fP shareable, the routine can
// call VXWTask::varAdd() to make a separate copy of the variable for each
// task, but all at the same physical address.
//
// Note that task variables increase the task switch time to and from the
// tasks that own them.  Therefore, it is desirable to limit the number of
// task variables that a task uses.  One efficient way to use task variables 
// is to have a single task variable that is a pointer to a dynamically 
// allocated structure containing the task's private data.
//
// EXAMPLE:
// Assume that three identical tasks are spawned with a main routine called
// \f2operator()\f1.  All three use the structure OP_GLOBAL for all variables
// that are specific to a particular incarnation of the task.  The following
// code fragment shows how this is set up:
//
// .CS
// OP_GLOBAL *opGlobal;  // ptr to operator task's global variables
// VXWTask   me;         // task object for self
//
// void operator
//     (
//     int opNum         // number of this operator task 
//     )
//     {
//     me = VXWTask (0); // task object for running task
//     if (me.varAdd ((int *)&opGlobal) != OK)
//         {
//         printErr ("operator%d: can't VXWTask::varAdd opGlobal\en", opNum);
//         me.suspend ();
//         }
//
//     if ((opGlobal = (OP_GLOBAL *) malloc (sizeof (OP_GLOBAL))) == NULL)
//         {
//         printErr ("operator%d: can't malloc opGlobal\en", opNum);
//         me.suspend ();
//         }
//     ...
//     }
// .CE
//
// RETURNS:
// OK, or ERROR if memory is insufficient for the task variable descriptor.
//
// SEE ALSO: VXWTask::varDelete(), VXWTask::varGet(), VXWTask::varSet()

    STATUS varAdd (int * pVar)
	{
	return taskVarAdd (tid_, pVar);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::varDelete - remove a task variable from task
//
// This routine removes a specified task variable, <pVar>, from its
// task's context.  The private value of that variable is lost.
//
// RETURNS
// OK, or
// ERROR if the task variable does not exist for the task.
//
// SEE ALSO: VXWTask::varAdd(), VXWTask::varGet(), VXWTask:varSet()

    STATUS varDelete (int * pVar)
	{
	return taskVarDelete (tid_, pVar);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::varGet - get the value of a task variable
//
// This routine returns the private value of a task variable for its
// task.  The task is usually not the calling task,
// which can get its private value by directly accessing the variable.
// This routine is provided primarily for debugging purposes.
//
// RETURNS:
// The private value of the task variable, or
// ERROR if the task does not own the task variable.
//
// SEE ALSO: VXWTask::varAdd(), VXWTask::varDelete(), VXWTask::varSet()

    int varGet (int * pVar) const
	{
	return taskVarGet (tid_, pVar);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::varInfo - get a list of task variables
//
// This routine provides the calling task with a list of all of the task
// variables of its task.  The unsorted array of task variables is
// copied to <varList>.
//
// CAVEATS
// Kernel rescheduling is disabled while task variables are
// looked up.  
//
// There is no guarantee that all the task variables are still
// valid or that new task variables have not been created by the time this
// routine returns.
//
// RETURNS: The number of task variables in the list.

    int varInfo (TASK_VAR varList[], int maxVars) const
	{
	return taskVarInfo (tid_, varList, maxVars);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWTask::varSet - set the value of a task variable
//
// This routine sets the private value of the task variable for a specified
// task.  The specified task is usually not the calling task, which can set
// its private value by directly modifying the variable.  This routine is
// provided primarily for debugging purposes.
//
// RETURNS:
// OK, or ERROR if the task does not own the task variable.
//
// SEE ALSO: VXWTask::varAdd(), VXWTask::varDelete(), VXWTask::varGet()

    STATUS varSet (int * pVar, int value)
	{
	return taskVarSet (tid_, pVar, value);
	}

  protected:
    VXWTask ()
	{
	}
    VXWTask (const VXWTask &)
	{
	}
    VXWTask & operator = (const VXWTask &)
	{
	return *this;
	}
    virtual void * myValue ();

    int		tid_;
    BOOL	managePrivateEnv_;
    };

#endif /* ifndef vxwTaskLib_h */