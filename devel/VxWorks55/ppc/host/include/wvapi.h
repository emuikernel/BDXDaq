/* wvapi.h -- WindView 2.0 C++ API definition. */

/* (c) 1997-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01t,31oct01,cpd  Fix SPR#66965: Add getLastError method
01s,19sep01,cpd  Initial Veloce mods.
                 Fix SPR#23837: Add 'visible contexts only' search
                 Fix SPR#22134: Add BSPName for log info display
01r,21apr99,cjtc changed m_sRawLogTmpFile to a char array instead of a string
		 to try to prevent a core dump on Solaris host. It didn't help
		 the core dump, but has been left as a more belt-and-braces
		 fix.
01q,19apr99,cjtc defined new WVEventBase private member m_sRawLogTmpFile
		 to hold the name of the file created in the host's temporary
		 directory for a direct to graph upload. This is used to
		 unlink the file when the WVEventBase destructor is called.
		 Fixes SPR 26830
01p,01jul98,c_s  WindView 2 -> Tornado 2 Merge
01o,09may98,tcy  undef __WIN32__ for HP-UX10
01n,05may98,tcy  unqualify std namespace for HP-UX10
01m,01may98,dbs  improve collection diagnostics
01l,22apr98,dbs  implement analysis-packs
01k,17apr98,dbs  fix WVQuery begin() method args
01j,15apr98,dbs  remove Tcl in WVAnalysis class
01i,15apr98,dbs  implement object-ID
01h,16mar98,dbs  fix DLL-export warnings
01g,09mar98,dbs  improve WVQuery operation
01f,09feb98,dbs  add MP support, remove returns of std::string class
                 due to Win32 DLL export problems.
01e,23jan98,dbs  add C-string to wv_error class instead of std::string
01d,19jan98,dbs  more fixes for Sparcworks compiler
01c,15jan98,c_s  avoid namespaces under SparcWorks
01b,09jan98,dbs  moved to $WIND_BASE/host/include, integrated with
                 old wvapicpp.h and wvapi.h for first preview release
		 of C++ API.
01a,02jul97,dbs  created.
  
*/

////////////////////////////////////////////////////////////////////////
//
// DESCRIPTION
//
// -- WindView C++ API --
//
// This header file wvapi.h defines the classes that make up the
// WindView C++ external programming interface. In order that the
// internal layout of WindView objects is not revealed to clients, the
// classes are intentionally 'thin' and are in fact only proxies for
// internal classes which perform the real work.
//
// The classes CEventBase, CContext and CEvent are the private
// implementation classes, which are hidden from users of the API.
//
////////////////////////////////////////////////////////////////////////

#ifndef WVAPI_H__
#define WVAPI_H__

#include "host.h"			// WRS std host defines

//
// Nasty kludge to get around MSVC++ 5.0 inability to handle the word
// 'list' after it sees it in some std-lib template headers...
//
#define list lst__
#include "tcl.h"			// Tcl API definition
#undef list
#ifdef PARISC_HPUX10
#undef __WIN32__
#endif /* PARISC_HPUX10 */

#include <string>			// std string class
#include <vector>			// std vector class

#pragma warning (disable:4251)		// DLL-export warning

#if defined (WIN32) && !defined (__SUNPRO_CC) && !defined (PARISC_HPUX10)
using namespace std;			// unqualify std namespace
#endif

/////////////////////////////////////////////////////////////////////////
//
// Some useful typedefs, constants, and declarations...
//
/////////////////////////////////////////////////////////////////////////

typedef double		WV_TIME, TIME;
typedef unsigned short	EVENT_TYPE;
typedef unsigned int 	VX_ID;
typedef int 		VX_PRIORITY;

class  CEventBase;
class  CContext;
struct CEvent;

class WVEventBase;
class WVContext;
class WVEvent;
class WVCoords;
class WVAnalysis;

const	int		WV_NIL = 0xFFFFFFFF;
const	WV_TIME		TIME_ZERO = 0.0;
const	WV_TIME		TIME_TOLERANCE = 0.0000001;
const	WV_TIME		TIME_INVALID = -1.0;
const	int		PRI_COMPUTED = 300;
const	int		MAX_INTERRUPT_CONTEXTS = 64;

/////////////////////////////////////////////////////////////////////////
//
// Define the WVAPI qualifier which declares classes as exported (from 
// Win32 DLLs) when inside the API lib code, and as imported when in
// user code. The API lib code modules all declare WV_EXPORTING_
// before including this file, so this symbol must *not* be defined in 
// user code!
//
/////////////////////////////////////////////////////////////////////////

#ifdef	WIN32
#ifdef	WV_EXPORTING_ /* inside WV API code */
#define WVAPI __declspec(dllexport)
#else	/* user code - imports WV DLL */
#define WVAPI __declspec(dllimport)
#endif
#else	/* WIN32 */
#define WVAPI
#endif	/* WIN32 */

/////////////////////////////////////////////////////////////////////////
//
// WindView API error codes - these are used internally and are also
// part of the wv_error class when exceptions are thrown.
//
/////////////////////////////////////////////////////////////////////////

#define WV_OK		(0)
#define WV_ENOMEM	(1)
#define WV_EFILE	(2)
#define WV_ERANGE	(3)
#define WV_EBADHANDLE	(4)
#define WV_EPANIC	(5)
#define WV_EEMPTY	(6)

#define WV_EOF		(-1)

////////////////////////////////////////////////////////////////////////
//
// Define WV exception class 'wv_error' which holds a textual error
// message and an error-code as defined above.
//
////////////////////////////////////////////////////////////////////////

class WVAPI wv_error
    {

    char *		what_;
    int			errCode_;
    unsigned int	index_;

  public:

    // default constructor
    wv_error ();
    
    // constructor with std::string
    wv_error(const string& msg, int e, unsigned idx=0);

    // constructor with const char *
    wv_error(const char *s, int e, unsigned idx=0);

    // copy constructor
    wv_error (const wv_error & e);

    // assignment operation
    wv_error & operator= (const wv_error &);

    // destructor
    ~wv_error ();
    
    const char *what () const { return what_; }
    int		errorCode () const { return errCode_; }
    unsigned	index () const { return index_; }

    };

////////////////////////////////////////////////////////////////////////
//
// -- class WVEvent --
//
// Proxy class representing a single event within the event-base.
//
////////////////////////////////////////////////////////////////////////
class WVAPI WVEvent
    {
    
    friend class WVEventBase;
    
    //
    // PRIVATE member variables - names are deliberately obscure to
    // fend off casual observers...
    //
    UINT32	_typ;
    WV_TIME	_tim;
    UINT32	_nmp;
    UINT32	_nst;
    void*	_peb;
    UINT32	_par[7];

  public:
    // normal constructor - when a WVEvent is created by dereferencing
    // a WVContext iterator (the normal way)
    WVEvent(const CContext* cx, unsigned ev);

    // copy constructor
    WVEvent(const WVEvent& x);

    // default constructor - may also be used to create search-targets
    // when using STL algorithms
    WVEvent(WV_TIME t=TIME_ZERO);
    
    // assignment operation - assign from one WVEvent to another
    WVEvent& operator=(const WVEvent& x);
    
    // get event type
    EVENT_TYPE type() const;

    // get event time-stamp
    WV_TIME timeStamp() const;

    // get event-name ("semTake" etc)
    const char * name() const;

    // get number of parameters
    int numParams() const;

    // get a single parameter
    UINT32 param(unsigned n) const;

    // get array of params
    const UINT32 * params() const;
    
    // retrieve the task-state at the time of this event
    UINT32 state() const;

    // is event a context-switch event? if so, place the context being
    // switched to in the variable pointed to by the first arg...
    BOOL isContextSwitch(WVContext *) const;
    
    // comparison operator - lets WVEvent objects be stored in STL
    // containers safely, and provides ordering in time.
    BOOL operator<(const WVEvent& x) const;

    // equality operator - also assists in STL container usage
    BOOL operator==(const WVEvent& x) const;

    };

////////////////////////////////////////////////////////////////////////
//
// WVCtxIter class
//
// This is declared as a separate class, then typedef'ed to
// WVContext::iterator.
//
// Each iterator keeps a pointer to a CContext object, and a
// position which represents the offset into the context's sequence of
// events. The 'end' value of iterator is 'one past the end' as for
// usual STL case i.e. >= size(), so even if the event-vector grows
// during accumulation of events, this is not compromised.
//
////////////////////////////////////////////////////////////////////////
class WVAPI WVCtxIter
    {

    friend class WVContext;

    friend ostream& operator<<(ostream&,const WVCtxIter&);
    
    const CContext*	pCContext;
    unsigned		pos;

  public:

    WVCtxIter(const CContext *pc=0, unsigned n=0);// def constructor
    WVCtxIter(const WVContext&, unsigned);        // constructor
    WVEvent operator*() const;			  // deref (*iter)
    BOOL operator<(const WVCtxIter& i) const;	  // iter < iter
    WVCtxIter& operator=(const WVCtxIter& i);	  // iter = iter
    WVCtxIter& operator++();			  // ++iter
    WVCtxIter operator++(int);			  // iter++
    WVCtxIter& operator--();			  // --iter
    WVCtxIter operator--(int);			  // iter--
    BOOL operator==(const WVCtxIter& i) const;	  // iter == iter
    BOOL operator!=(const WVCtxIter& i) const;	  // iter != iter
    int operator-(const WVCtxIter& i) const;	  // iter - iter
    WVCtxIter operator+(int n) const;		  // iter + int
    WVCtxIter operator-(int n) const;		  // iter - int
    WVCtxIter& operator+=(int n);		  // iter += int
    WVCtxIter& operator-=(int n);		  // iter -= int
    operator BOOL() const;			  // if (iter) ...
    WVContext context() const;			  // iter.context()
    unsigned index() const { return pos; }	  // iter.index()
    
    };

////////////////////////////////////////////////////////////////////////
//
// WVQuery - abstract base class for queries into WVContext. The
// application should define its own sub-classes of this class, and
// instantiate such a derived object in its own stack-space. Then,
// pass this object into the WVContext method query(), along with the
// start and end times of interest. The query() method will invoke
// operator() (with a WVEvent) for each event between the given
// times. The derived object can then perform whatever analysis it
// likes!
//
////////////////////////////////////////////////////////////////////////
struct WVAPI WVQuery
    {

    unsigned	m_first;		// first index
    unsigned	m_last;			// last index
    unsigned	m_current;		// current index
    
    WVQuery() {}
    virtual ~WVQuery() {}

    virtual void begin (WV_TIME t0, const WVEvent* pPrevEvent) =0;
    virtual BOOL operator() (WV_TIME t, const WVEvent* pEvent=0) =0;
    virtual void end (WV_TIME t1) =0;

    };
	
////////////////////////////////////////////////////////////////////////
//
// -- class WVContext --
//
// Proxy class representing a single task-context within an
// event-base. Provides iterator class for application of STL
// algorithms and array-like operation to retrieve single events.
//
////////////////////////////////////////////////////////////////////////
class WVAPI WVContext
    {

    friend class WVAnalysisPack;
    friend class WVCtxIter;
    friend class WVEventBase;

    //
    // PRIVATE member variables.
    //
    const CContext*	pCContext;

    //
    // PRIVATE Tcl command handler
    //
    static int tclCmdHandler(ClientData, Tcl_Interp*, int, char**);
    
  public:

    // allow usage with STL algorithms etc...
    typedef WVCtxIter iterator;
    
    // default constructor - requires ptr to real CContext object
    WVContext(const CContext *pc=0) : pCContext(pc) {}

    // copy constructor
    WVContext(const WVContext& x);

    // assignment operation
    WVContext& operator=(const WVContext& x);
    
    // destructor
    ~WVContext() {}

    // array-index returns event object
    WVEvent operator[](unsigned n) const;

    // returns iterator
    WVCtxIter operator()(unsigned n) const;
    
    // test validity of object
    BOOL operator!() const;

    // return unique identifier
    int uniqueId() const;

    // return processor upon which this context executed
    int processorNumber() const;
    
    // return number of events in context
    unsigned size() const;

    // return task ID
    VX_ID taskId() const;

    // return task name
    const char * name() const;

    // return task priority
    int priority() const;

    // return CPU type-identifier e.g. SPARC, 68K, etc...
    int cpuId() const;
    
    // equality test and comparison operator - these are required by
    // some implementations of STL to allow storage of WVContext
    // objects in STL containers  
    BOOL operator<(const WVContext& x) const
	    {
	    return priority() < x.priority();
	    }
    
    BOOL operator==(const WVContext& x) const
	    {
	    return pCContext==x.pCContext;
	    }

    // creates Tcl cmd for this context
    int tclCmdCreate(Tcl_Interp *, const string&) const;
    
    // begin() - returns iterator pointing to start of collection
    iterator begin() const { return iterator(pCContext, 0); }

    // end() - returns iterator pointing to beyond end
    iterator end() const { return iterator(pCContext, size()); }

    // returns iterator pointing to first event <= given time, and if
    // multiple events have the same time (that is <= given time),
    // then it returns the rightmost of them.
    iterator findEventByTime(WV_TIME) const;

    // eventBase() - returns ptr to containing WVEventBase...
    WVEventBase* eventBase() const;

    // test if this is an INTERRUPT context
    BOOL isInterrupt() const;

    // test if this is the IDLE context
    BOOL isIdle() const;

    // query() - perform a repeated invocation of the WVQuery object's 
    // operator() for every event between time t0 and t1...
    void query(WVQuery& q, WV_TIME t0, WV_TIME t1) const;
    
    // get variable from VM (name is 'a' to 'z' inclusive)
    int vmGet(char varName) const;
    
    };

////////////////////////////////////////////////////////////////////////
//
// prevContextFromEvent() - for pre-emption and resumption events,
// returns a WVContext object representing the previous context. If
// called for other types of event, then it returns an invalid
// WVContext object, i.e. one whose operator! returns TRUE...
//
////////////////////////////////////////////////////////////////////////
WVContext prevContextFromEvent(const WVEvent&);
    
////////////////////////////////////////////////////////////////////////
//
// WVEBIter class
//
// This is declared as a separate class, then typedef'ed to
// WVEventBase::iterator.
//
// Each iterator keeps a pointer to a CContext object, and a
// position which represents the offset into the context's sequence of
// events. The 'end' value of iterator is 'one past the end' as for
// usual STL case i.e. >= size(), so even if the event-vector grows
// during accumulation of events, this is not compromised.
//
////////////////////////////////////////////////////////////////////////
class WVAPI WVEBIter
    {

    friend class WVEventBase;

    friend ostream& operator<<(ostream&,const WVEBIter&);
    
    const CEventBase*	pEventBase;
    unsigned int	pos;

  public:

    WVEBIter(const CEventBase* =0, unsigned int=0);// def constructor
    WVEBIter(const WVEventBase&, unsigned int);   // constructor
    WVContext operator*() const;		  // deref (*iter)
    BOOL operator<(const WVEBIter& i) const;	  // iter < iter
    WVEBIter& operator=(const WVEBIter& i);	  // iter = iter
    WVEBIter& operator++();			  // ++iter
    WVEBIter operator++(int);			  // iter++
    WVEBIter& operator--();			  // --iter
    WVEBIter operator--(int);			  // iter--
    BOOL operator==(const WVEBIter& i) const;	  // iter == iter
    BOOL operator!=(const WVEBIter& i) const;	  // iter != iter
    int operator-(const WVEBIter& i) const;	  // iter - iter
    WVEBIter operator+(int n) const;		  // iter + int
    WVEBIter operator-(int n) const;		  // iter - int
    WVEBIter& operator+=(int n);		  // iter += int
    WVEBIter& operator-=(int n);		  // iter -= int
    unsigned index() const { return pos; }	  // iter.index()
    
    };

////////////////////////////////////////////////////////////////////////
//
// -- class WVEventBase --
//
// Public class for holding event-base. Provides access to contained
// collection of contexts via array-index operator, and via iterators
// which can be used for scanning the collection and applying standard
// library algorithms. 
//
// Contexts are always stored in the order IDLE, then TASKs, then
// INTERRUPTs and in priority order inside those groups.
//
////////////////////////////////////////////////////////////////////////

class WVAPI WVEventBase
    {

  public:
    typedef BOOL (*WVEBCB)(void*, int, int);

  private:

    //
    // Define 'friend' classes...
    //
    friend class CEventBase;
    friend class WVEBIter;

    //
    // PRIVATE data members.
    //
    CEventBase*	pEventBase;		// ptr to impl class
    string	tclCmd;			// equivalent Tcl name
    string	tmpFileName;		// temp log file name
    char 	m_sRawLogTmpFile [PATH_MAX];	// file name of temp file
    int		fileCount;		// number of files loaded

    //
    // PRIVATE member functions.
    //
    STATUS loadSingleFile(const string&, int, WVEBCB, void*, FILE*);
    static int tclCmdHandler(ClientData, Tcl_Interp*, int, char**);
    
  public:

    ////////////////////////////////////////////////////////////////////
    //
    // struct event_value - a simple structure used to hold the set of
    // time-value pairs for building a computed-context.
    //
    ////////////////////////////////////////////////////////////////////
    struct event_value
	{
	WV_TIME	time;
	int	value;

	BOOL operator < (const event_value& v) const
	    { return time < v.time; }
	};

    ////////////////////////////////////////////////////////////////////
    // Iterator typedefs...
    ////////////////////////////////////////////////////////////////////
    typedef WVEBIter iterator;
    
    ////////////////////////////////////////////////////////////////////
    // constructor - takes an optional pointer to a TCL interpreter.
    ////////////////////////////////////////////////////////////////////
    WVEventBase(Tcl_Interp *pTcl=0);

    ////////////////////////////////////////////////////////////////////
    //
    // load() - file loading interface - takes the file name and loads
    // a single file with that name. Otionally takes callback args,
    // and a FILE-ptr to allow diagnostic dumping of data as it is
    // loaded. 
    //
    ////////////////////////////////////////////////////////////////////
    STATUS load
	(
	const char *	fileName,
	WVEBCB		callback=0,
	void *		cbArg=0,
	FILE *		dumpFile=0
	);

    ////////////////////////////////////////////////////////////////////
    // tclcmdCreate() - creates Tcl cmd for this event base
    ////////////////////////////////////////////////////////////////////
    int tclCmdCreate(Tcl_Interp *, const string&);
    
    ////////////////////////////////////////////////////////////////////
    // Socket data collection interface - to load event-base data from
    // a socket, the user should create a suitable data socket, and
    // ensure it is connected to a live target. This socket's FD
    // should be passed to beginCollection() which will create an
    // instance of the event-collection object(s) within the event
    // base. Whenever the socket is readable (as reported by select())
    // then readEventData() should be called. It will return the
    // number of bytes read from the socket, and will return when
    // there are no more bytes left to read - sometimes it may return
    // zero. If it ever returns a negative number, then the stream
    // appears to have been closed. When the user confirms this via
    // select(), the endCollection() method can be called, to
    // terminate the collection object(s) associated with the FD.
    ////////////////////////////////////////////////////////////////////
    void beginCollection(int socketFd);
    int  readEventData(int socketFd);
    void endCollection(int socketFd);
    int  saveAs(const char *fileName);
    
    ////////////////////////////////////////////////////////////////////
    // destructor - cleans up all underlying data structures.
    ////////////////////////////////////////////////////////////////////
    ~WVEventBase();

    ////////////////////////////////////////////////////////////////////
    // operator!() - indicates validity of event base...
    ////////////////////////////////////////////////////////////////////
    BOOL operator!() const;
    
    ////////////////////////////////////////////////////////////////////
    // thinMode() - indicates whether the event-data was gathered in
    // thin mode (i.e. limited information)...
    ////////////////////////////////////////////////////////////////////
    int thinMode() const;
    
    ////////////////////////////////////////////////////////////////////
    // logMode() - indicates whether the event-data was gathered in
    // thin mode (i.e. limited information)...
    ////////////////////////////////////////////////////////////////////
    int logMode(int index) const;
    
    ////////////////////////////////////////////////////////////////////
    // BSPNameGet() - 
    // 
    ////////////////////////////////////////////////////////////////////
    string BSPNameGet(int index) const;
    
    ////////////////////////////////////////////////////////////////////
    // LogTypeGet() - 
    // 
    ////////////////////////////////////////////////////////////////////
    string LogTypeGet(int index) const;
    
    ////////////////////////////////////////////////////////////////////
    // numEvents() - returns the total number of events processed by
    // the event base so far
    ////////////////////////////////////////////////////////////////////
    int numEvents() const;
    
    ////////////////////////////////////////////////////////////////////
    // size() - returns number of contexts in event-base
    ////////////////////////////////////////////////////////////////////
    unsigned int size() const;

    ////////////////////////////////////////////////////////////////////
    // array-index operation - returns a single context object
    ////////////////////////////////////////////////////////////////////
    WVContext operator[](unsigned int n) const;

    ////////////////////////////////////////////////////////////////////
    // begin() and end() - return iterators pointing to start and
    // one-past-the-end of the collection, respectively...
    ////////////////////////////////////////////////////////////////////
    iterator begin() const;
    iterator end() const;

    ////////////////////////////////////////////////////////////////////
    // setVerbosity() - sets a level of detail to be dumped to stdout
    // whilst loading an event log. Defaults to zero unless this
    // function is called, currently only one level (other than zero)
    // is supported.
    ////////////////////////////////////////////////////////////////////
    void setVerbosity (int);
    
    ////////////////////////////////////////////////////////////////////
    // name() - return event-base name...
    ////////////////////////////////////////////////////////////////////
    const char * name() const;
    
    ////////////////////////////////////////////////////////////////////
    // return name of given event's type...
    ////////////////////////////////////////////////////////////////////
    const char * eventName(const WVEvent&) const;
    
    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    WVContext contextFromUniqueId(int uid) const;
    
    ////////////////////////////////////////////////////////////////////
    // secsPerTick() - returns timebase multiplier
    ////////////////////////////////////////////////////////////////////
    double secsPerTick() const;
    
    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    WV_TIME startTime() const;
    WV_TIME endTime() const;
    
    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    int objectIdParamNum (EVENT_TYPE) const;

    ////////////////////////////////////////////////////////////////////
    // Add a computed-context...
    ////////////////////////////////////////////////////////////////////
    WVContext computedContextAdd
	(
	const string&	    name,	// name to give the ctx
	event_value *	    pValues,	// ptr to array of time/value pairs
	unsigned int	    nValues,	// the number of values
	int		    min,	// scale minimum value
	int		    max		// scale maximum value
	);

    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    WVContext ctxFromName(const string& ctxName) const;

    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    string getLastError () const;

    ////////////////////////////////////////////////////////////////////
    // Functions that provide parts of the Tcl interface...
    ////////////////////////////////////////////////////////////////////
    void printOn(ostream& os, int depth) const;
    const char * eventAsString(const WVEvent&, const char * fmt=0) const;
    const char * eventAsTclString (const WVCtxIter &) const;
    const char * eventApplyTclCmd (const WVCtxIter &, char *) const;
    const char * tclCommandName() const;
    Tcl_Interp *tclInterp() const;

    static const char * handleFromPtr(WVEventBase*);
    static WVEventBase * ptrFromHandle(const string&);
    
    };

////////////////////////////////////////////////////////////////////////
//
// Define WIND STATE BITS constants which mimic the VxWorks kernel
// task-state bits and are used to define the states of contexts in
// the event base.
//
////////////////////////////////////////////////////////////////////////

#define WIND_READY              0x00    /* ready to run */

/*
 * PEND states - a task, when not running, can be in any combinatorial
 * state made up of these bits, so READY simply means not-suspended
 * and not-pended and not-delayed. Hence there are 8 valid combinations.
 */

#define WIND_SUSPEND            0x01    /* explicitly suspended */
#define WIND_PEND               0x02    /* pending on semaphore */
#define WIND_DELAY              0x04    /* task delay (or timeout) */

/*
 * DEAD cannot be combined with any other states.
 */

#define WIND_DEAD               0x08    /* dead task */

/*
 * RUN states - when a task is running it may also be inherited
 * or locked at the same time, so there are 5 possible valid combinations
 */

#define WIND_LOCKED             0x10    /* locked task */
#define WIND_INHERITED          0x20    /* priority inherited task */
#define WIND_RUNNING            0x40    /* running */

#define WIND_ALL_STATES         0x3f    /* to select all mask bits */

////////////////////////////////////////////////////////////////////////
//
// -- class WVCoords --
//
// Represents a search-index into the event base, and can be created
// by application code when searching for particular kinds of
// events. It provides operators ++ and -- to allow navigation over
// the event base, and allows the seach to be locked to particular
// contexts, or particular event-types, or both.
//
// The helper functions anyContext() and anyEvent() provide values
// that can be used to specify wider searches when constructing a
// WVCoords object.
//
// After construction, the object will always point to some valid
// event, i.e. dereferencing using operator* will always yield a
// WVEvent object from within the event-base. However, the ++ and --
// operators return a BOOL indicating whether they moved or not, and
// when they stop returning TRUE then the end-stop has been hit.
//
////////////////////////////////////////////////////////////////////////

class WVAPI WVCoords
    {

    const WVEventBase*	pWVEB;		// ptr to event base 
    unsigned		nCtx;		// index into EB => ctx
    WVCtxIter		iEvent;		// indicates one event
    EVENT_TYPE		eventType;	// events of interest
    WV_TIME		time;		// current time
    int			objectId;	// -1 if don't care
    BOOL		fixCtx;		// only use one context
    BOOL		fixType;	// only use one type
    BOOL		fixObjId;	// only use one object-ID

    void init();			// clean startup conditions
    void copy(const WVCoords&);		// private copy method
    
    BOOL eventMatch(const WVCtxIter&);	// does event under iterator
					// match search criteria?

    BOOL eventFind			// find next event from time
	(
	WVEBIter	i0,		// first ctx
	WVEBIter	i1,		// last ctx
	int		dir		// direction
	);
    
  public:

    enum { LEFT = -1, RIGHT = 1 };
    
    BOOL move(int);			// move fwds = 1, bkwds = -1

    static WVContext  anyContext();	// helper
    static EVENT_TYPE anyEvent();	// helper
    static int	      anyObject ();	// helper

    WVCoords();				// default c'tor

    WVCoords(const WVCoords&);		// copy c'tor
    
    WVCoords				// search c'tor
	(
	const WVEventBase*,		// event base to use
	WV_TIME,			// time to start
	const WVContext&,		// context to begin with
	EVENT_TYPE,			// event type to look for
	BOOL =FALSE,			// lock context
	BOOL =FALSE			// lock event type
	);

    WVCoords				// search c'tor from iterator
	(
	const WVEventBase*,		// event base
	const WVCtxIter&		// ctxt + event
	);

    WVCoords& operator=(const WVCoords&); // assignment
    
    const WVEventBase& eventBase() const // get event base
	{ return *pWVEB; }
    
    WVCtxIter operator*() const;	// deref => iter 

    BOOL operator++(int)		// move to next event
	{ return move(1); }
    
    BOOL operator--(int)		// move to prev event
	{ return move(-1); }

    void contextLock(BOOL);		// stick to context
    void eventTypeLock(BOOL);		// stick to event type
    void objectIdLock (BOOL);		// stick to object with given ID

    void eventTypeSet(EVENT_TYPE);	// choose event type
    void contextSet(const WVContext&);	// choose context
    void objectIdSet (int objid);	// choose object ID

    void timeSet(WV_TIME);		// chnage timestamp
    
    const char * contextName() const;	// name of curr ctx
    WVContext WVCoords::contextGet() const;
    
    static int tclCmdHandler(ClientData, Tcl_Interp*, int, char**);

    };

////////////////////////////////////////////////////////////////////////
//
// If the iterator-classes are to be used with std-lib algorithms,
// then it is necessary to enable the compiler to determine the
// 'distance type' and 'iterator category' for the WV API iterator
// classes. We do this by overloading the equivalent std lib template
// functions which perform this type resolution, and hence allow the
// compiler to deduce the template type parameters for the
// algorithms.
//
////////////////////////////////////////////////////////////////////////
#include <iterator>

#ifdef _MSC_VER
namespace std {
inline ptrdiff_t* _Dist_type (WVCtxIter)
    { return (ptrdiff_t*)0; }
inline ptrdiff_t* _Dist_type (WVEBIter)
    { return (ptrdiff_t*)0; }
inline random_access_iterator_tag _Iter_cat (WVCtxIter)
    { return random_access_iterator_tag (); }    
inline random_access_iterator_tag _Iter_cat (WVEBIter)
    { return random_access_iterator_tag (); }    
};
#endif /* _MSC_VER */

#if defined (__GNUC__) || defined (__SUNPRO_CC)
inline bidirectional_iterator_tag iterator_category(const WVCtxIter&)
    { return bidirectional_iterator_tag(); }
inline bidirectional_iterator_tag iterator_category(const WVEBIter&)
    { return bidirectional_iterator_tag(); }
inline int* distance_type(const WVCtxIter&)
    { return (int*)0; }
inline int* distance_type(const WVEBIter&)
    { return (int*)0; }
#endif

#endif


