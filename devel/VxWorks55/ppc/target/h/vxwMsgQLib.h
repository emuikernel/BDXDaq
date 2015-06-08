// VXWMsgQ/vxwMsgQLib.h - message queue classes

// Copyright 1995-1999 Wind River Systems, Inc.

// modification history
// --------------------
// 01b,23feb99,fle  doc : made it refgen compliant
// 01c,21feb99,jdi  added library section, checked in documentation.
// 01b,03oct95,rhp  documented.
// 01a,15jun95,srh  written.

// DESCRIPTION
// The `VXWMsgQ' class provides message queues, the primary intertask
// communication mechanism within a single CPU.  Message queues allow
// a variable number of messages (varying in length) to be queued in
// first-in-first-out (FIFO) order.  Any task or interrupt service
// routine can send messages to a message queue.  Any task can receive
// messages from a message queue.  Multiple tasks can send to and
// receive from the same message queue.  Full-duplex communication
// between two tasks generally requires two message queues, one for
// each direction.
//
// CREATING AND USING MESSAGE QUEUES
// The message-queue constructor takes parameters to specify the
// maximum number of messages that can be queued to that message queue and
// the maximum length in bytes of each message.  Enough buffer space is
// pre-allocated to accommodate the specified number of messages of
// specified length.
//
// A task or interrupt service routine sends a message to a message
// queue with VXWMsgQ::send().  If no tasks are waiting for messages
// on the message queue, the message is simply added to the buffer of
// messages for that queue.  If any tasks are already waiting to
// receive a message from the message queue, the message is
// immediately delivered to the first waiting task.
//
// A task receives a message from a message queue with VXWMsgQ::receive().
// If any messages are already available in the message queue's buffer,
// the first message is immediately dequeued and returned to the caller.
// If no messages are available, the calling task blocks and joins
// a queue of tasks waiting for messages.  This queue of waiting tasks can
// be ordered either by task priority or FIFO, as specified in an option
// parameter when the queue is created.
//
// TIMEOUTS
// Both VXWMsgQ::send() and VXWMsgQ::receive() take timeout
// parameters.  When sending a message, if no buffer space is
// available to queue the message, the timeout specifies how many
// ticks to wait for space to become available.  When receiving a
// message, the timeout specifies how many ticks to wait if no message
// is immediately available.  The <timeout> parameter can have the
// special values NO_WAIT (0) or WAIT_FOREVER (-1).  NO_WAIT means the
// routine should return immediately; WAIT_FOREVER means the routine
// should never time out.
//
// URGENT MESSAGES
// The VXWMsgQ::send() routine allows the priority of a message to be specified
// as either normal (MSG_PRI_NORMAL) or urgent (MSG_PRI_URGENT).  Normal
// priority messages are added to the tail of the list of queued messages,
// while urgent priority messages are added to the head of the list.
//
// INCLUDE FILES: vxwMsgQLib.h
//
// SEE ALSO: pipeDrv, msgQSmLib,
// .pG "Basic OS"
//
// SECTION: 1C
//

#ifndef vxwMsgQLib_h
#define vxwMsgQLib_h

#include "vxWorks.h"
#include "msgQLib.h"
#include "vxwObject.h"
#include "vxwErr.h"

class VXWMsgQ : virtual public VXWIdObject
    {
  public:

//_ VXWMsgQ Public Constructors

///////////////////////////////////////////////////////////////////////////////
//
// VXWMsgQ::VXWMsgQ - create and initialize a message queue
//
// This constructor creates a message queue capable of holding up to <maxMsgs>
// messages, each up to <maxMsgLen> bytes long.  The queue can be created 
// with the following options specified as <opts>:
// .iP MSG_Q_FIFO 20
// queue pended tasks in FIFO order.
// .iP MSG_Q_PRIORITY
// queue pended tasks in priority order.
//
// RETURNS: N/A.
//
// ERRNO:
// S_memLib_NOT_ENOUGH_MEMORY
//     - unable to allocate memory for message queue and message buffers.
//
// S_intLib_NOT_ISR_CALLABLE
//     - called from an interrupt service routine.
//
// SEE ALSO: vxwSmLib

    VXWMsgQ (int maxMsgs, int maxMsgLen, int opts)
	: msgq_ (msgQCreate (maxMsgs, maxMsgLen, opts))
	{
	if (msgq_ == 0)
	    vxwThrowErrno ();
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWMsgQ::VXWMsgQ - build message-queue object from ID
//
// Use this constructor to manipulate a message queue that was not created
// using C++ interfaces.  The argument <id> is the message-queue
// identifier returned and used by the C interface to the VxWorks
// message queue facility.
// 
// RETURNS: N/A.
// 
// SEE ALSO: msgQLib

    VXWMsgQ (MSG_Q_ID id)
	: msgq_ (id)
	{
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWMsgQ::~VXWMsgQ - delete message queue
//
// This destructor deletes a message queue.  Any task blocked on either
// a VXWMsgQ::send() or VXWMsgQ::receive() is unblocked and receives an error
// from the call with `errno' set to S_objLib_OBJECT_DELETED.  
//
// RETURNS: N/A.
//
// ERRNO:
// S_objLib_OBJ_ID_ERROR
//     - <msgQId> is invalid.
//
// S_intLib_NOT_ISR_CALLABLE
//     - called from an interrupt service routine.

    virtual ~VXWMsgQ ()
	{
	if (msgQDelete (msgq_) != OK)
	    vxwThrowErrno ();
	}

//_ VXWMsgQ Public Member Functions

///////////////////////////////////////////////////////////////////////////////
//
// VXWMsgQ::send - send a message to message queue
// 
// This routine sends the message in <buffer> of length <nBytes> to
// its message queue.  If any tasks are already waiting to receive
// messages on the queue, the message is immediately delivered to the
// first waiting task.  If no task is waiting to receive messages, the
// message is saved in the message queue.
//
// The <timeout> parameter specifies the number of ticks to wait for free
// space if the message queue is full.  The <timeout> parameter can also have 
// the following special values:
// .iP NO_WAIT 22
// return immediately, even if the message has not been sent.  
// .iP WAIT_FOREVER 
// never time out.
// .LP
//
// The <pri> parameter specifies the priority of the message being sent.
// The possible values are:
// .iP MSG_PRI_NORMAL 22
// normal priority; add the message to the tail of the list of queued 
// messages.
// .iP MSG_PRI_URGENT
// urgent priority; add the message to the head of the list of queued messages.
// .LP
//
// USE BY INTERRUPT SERVICE ROUTINES
// This routine can be called by interrupt service routines as well as
// by tasks.  This is one of the primary means of communication
// between an interrupt service routine and a task.  When called from an
// interrupt service routine, <timeout> must be NO_WAIT.
//
// RETURNS: OK or ERROR.
//
// ERRNO:
// S_objLib_OBJ_DELETED
//     - the message queue was deleted while waiting to a send message.
//
// S_objLib_OBJ_UNAVAILABLE
//     - <timeout> is set to NO_WAIT, and the queue is full.
//
// S_objLib_OBJ_TIMEOUT
//     - the queue is full for <timeout> ticks.
//
// S_msgQLib_INVALID_MSG_LENGTH
//     - <nBytes> is larger than the <maxMsgLength> set for the message queue.
//
// S_msgQLib_NON_ZERO_TIMEOUT_AT_INT_LEVEL
//     - called from an ISR, with <timeout> not set to NO_WAIT.

    STATUS send (char * buffer, UINT nBytes, int timeout, int pri)
	{
	return msgQSend (msgq_, buffer, nBytes, timeout, pri);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWMsgQ::receive - receive a message from message queue
//
// This routine receives a message from its message queue.
// The received message is copied into the specified <buffer>, which is
// <nBytes> in length.  If the message is longer than <nBytes>,
// the remainder of the message is discarded (no error indication
// is returned).
//
// The <timeout> parameter specifies the number of ticks to wait for 
// a message to be sent to the queue, if no message is available when
// VXWMsgQ::receive() is called.  The <timeout> parameter can also have 
// the following special values: 
// .iP NO_WAIT 22
// return immediately, even if the message has not been sent.  
// .iP WAIT_FOREVER 
// never time out.
// .LP
//
// WARNING: This routine must not be called by interrupt service routines.
//
// RETURNS:
// The number of bytes copied to <buffer>, or ERROR.
//
// ERRNO:
// S_objLib_OBJ_DELETED
//     - the message queue was deleted while waiting to receive a message.
//
// S_objLib_OBJ_UNAVAILABLE
//     - <timeout> is set to NO_WAIT, and no messages are available.
//
// S_objLib_OBJ_TIMEOUT
//     - no messages were received in <timeout> ticks.
//
// S_msgQLib_INVALID_MSG_LENGTH
//     - <nBytes> is less than 0.

    int receive (char * buffer, UINT nBytes, int timeout)
	{
	return msgQReceive (msgq_, buffer, nBytes, timeout);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWMsgQ::numMsgs - report the number of messages queued
//
// This routine returns the number of messages currently queued to the
// message queue.
//
// RETURNS:
// The number of messages queued, or ERROR.
//
// ERRNO:
// S_objLib_OBJ_ID_ERROR
//     - <msgQId> is invalid.

    int numMsgs () const
	{
	return msgQNumMsgs (msgq_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWMsgQ::info - get information about message queue
//
// This routine gets information about the state and contents of its message
// queue.  The parameter <pInfo> is a pointer to a structure of type MSG_Q_INFO
// defined in msgQLib.h as follows:
//
// .CS
//  typedef struct		/@ MSG_Q_INFO @/
//     {
//     int     numMsgs;		/@ OUT: number of messages queued            @/
//     int     numTasks;	/@ OUT: number of tasks waiting on msg q     @/
//     int     sendTimeouts;	/@ OUT: count of send timeouts               @/
//     int     recvTimeouts;	/@ OUT: count of receive timeouts            @/
//     int     options;		/@ OUT: options with which msg q was created @/
//     int     maxMsgs;		/@ OUT: max messages that can be queued      @/
//     int     maxMsgLength;	/@ OUT: max byte length of each message      @/
//     int     taskIdListMax;	/@ IN: max tasks to fill in taskIdList       @/
//     int *   taskIdList;	/@ PTR: array of task IDs waiting on msg q   @/
//     int     msgListMax;	/@ IN: max msgs to fill in msg lists         @/
//     char ** msgPtrList;	/@ PTR: array of msg ptrs queued to msg q    @/
//     int *   msgLenList;	/@ PTR: array of lengths of msgs             @/
//     } MSG_Q_INFO;
// .CE
//
// If the message queue is empty, there may be tasks blocked on receiving.
// If the message queue is full, there may be tasks blocked on sending.
// This can be determined as follows:
// .iP "" 4
// If <numMsgs> is 0, then <numTasks> indicates the number of tasks blocked
// on receiving.
// .iP
// If <numMsgs> is equal to <maxMsgs>, then <numTasks> is the number of
// tasks blocked on sending.
// .iP
// If <numMsgs> is greater than 0 but less than <maxMsgs>, then <numTasks> 
// will be 0.
// .LP
//
// A list of pointers to the messages queued and their lengths can be
// obtained by setting <msgPtrList> and <msgLenList> to the addresses of
// arrays to receive the respective lists, and setting <msgListMax> to
// the maximum number of elements in those arrays.  If either list pointer
// is NULL, no data is returned for that array.
//
// No more than <msgListMax> message pointers and lengths are returned,
// although <numMsgs> is always returned with the actual number of messages
// queued.
//
// For example, if the caller supplies a <msgPtrList> and <msgLenList>
// with room for 10 messages and sets <msgListMax> to 10, but there are 20
// messages queued, then the pointers and lengths of the first 10 messages in
// the queue are returned in <msgPtrList> and <msgLenList>, but <numMsgs> is
// returned with the value 20.
//
// A list of the task IDs of tasks blocked on the message queue can be obtained
// by setting <taskIdList> to the address of an array to receive the list, and
// setting <taskIdListMax> to the maximum number of elements in that array.
// If <taskIdList> is NULL, then no task IDs are returned.  No more than
// <taskIdListMax> task IDs are returned, although <numTasks> is always
// returned with the actual number of tasks blocked.
//
// For example, if the caller supplies a <taskIdList> with room for 10 task IDs
// and sets <taskIdListMax> to 10, but there are 20 tasks blocked on the
// message queue, then the IDs of the first 10 tasks in the blocked queue
// are returned in <taskIdList>, but <numTasks> is returned with
// the value 20.
//
// Note that the tasks returned in <taskIdList> may be blocked for either send
// or receive.  As noted above this can be determined by examining <numMsgs>.
// The variables <sendTimeouts> and <recvTimeouts> are the counts of
// the number of times VXWMsgQ::send() and VXWMsgQ::receive() (or
// their equivalents in other language bindings) respectively returned
// with a timeout.
//
// The variables <options>, <maxMsgs>, and <maxMsgLength> are the parameters
// with which the message queue was created.
//
// WARNING
// The information returned by this routine is not static and may be
// obsolete by the time it is examined.  In particular, the lists of
// task IDs and/or message pointers may no longer be valid.  However,
// the information is obtained atomically, thus it is an accurate
// snapshot of the state of the message queue at the time of the call.
// This information is generally used for debugging purposes only.
//
// WARNING
// The current implementation of this routine locks out interrupts while
// obtaining the information.  This can compromise the overall interrupt
// latency of the system.  Generally this routine is used for debugging
// purposes only.
//
// RETURNS: OK or ERROR.

    STATUS info (MSG_Q_INFO *pInfo) const
	{
	return msgQInfoGet (msgq_, pInfo);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWMsgQ::show - show information about a message queue
//
// This routine displays the state and optionally the contents of a message
// queue.
//
// A summary of the state of the message queue is displayed as follows:
// .CS
//     Message Queue Id    : 0x3f8c20
//     Task Queuing        : FIFO
//     Message Byte Len    : 150
//     Messages Max        : 50
//     Messages Queued     : 0
//     Receivers Blocked   : 1
//     Send timeouts       : 0
//     Receive timeouts    : 0
// .CE
//
// If <level> is 1, more detailed information is displayed.
// If messages are queued, they are displayed as follows:
// .CS
//     Messages queued:
//       #    address length value
//       1 0x123eb204    4   0x00000001 0x12345678
// .CE
//
// If tasks are blocked on the queue, they are displayed as follows:
// .CS
//     Receivers blocked:
//
//        NAME      TID    PRI DELAY
//     ---------- -------- --- -----
//     tExcTask     3fd678   0   21
// .CE
//
// RETURNS: OK or ERROR.

    STATUS show (int level) const
	{
	return msgQShow (msgq_, level);
	}
  protected:
    VXWMsgQ ()
	{
	}
    VXWMsgQ (const VXWMsgQ & aMsgQ)
	: msgq_ (aMsgQ.msgq_)
	{
	}
    VXWMsgQ & operator = (const VXWMsgQ & aMsgQ)
	{
	msgq_ = aMsgQ.msgq_;
	return *this;
	}
    virtual void * myValue ();
    MSG_Q_ID msgq_;
    };

#endif // ifndef vxwMsgQLib_h
