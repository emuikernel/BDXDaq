/* wvanalyze.h - WindView 2.0 Analysis Pack header */

/* Copyright (c) 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,25jun98,tcy  added friend class CAnalysisDlg for HP-UX10
01b,01jul98,c_s  WindView 2 -> Tornado 2 Merge
01a,28apr98,c_s  adjust APACK_FN definition for UNIX

01a,27apr98,dbs  created.
*/

/*
DESCRIPTION - this file must be included into analysis-pack code
after wvapi.h and defines the WVAnalysisPack class. Analysis-pack
modules wishing to use this code must link with apilib.lib to get
access to it.
*/

#ifndef __WVANALYZE_H__
#define __WVANALYZE_H__

#include <string>			// std string class
#include <vector>			// std vector class
#include <algorithm>			// std algorithms

#ifdef _MSC_VER
#pragma warning (disable:4251)		// DLL-export warning
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

typedef const char * (*PFN_INIT) (unsigned int *);
typedef void (*PFN_BEGIN) (const WVEventBase *);
typedef BOOL (*PFN_PERFORM) (WVAnalysisPack *);
typedef BOOL (*PFN_END) ();

class WVAPI WVAnalysisPack
    {

    typedef vector<WVEventBase::event_value> ResultList_t;

    // Dynamically-loaded functions from AP module...
    PFN_INIT		m_pInit;
    PFN_BEGIN		m_pBegin;
    PFN_PERFORM		m_pPerform;
    PFN_END		m_pEnd;

    // Private member variables...
    string		m_resultName;
    string		m_niceName;
    const WVEventBase *	m_pEventBase;
    ResultList_t        m_results;
    vector<WVContext>   m_ctxs;
    vector<WVCtxIter>   m_pos;
    WV_TIME		m_currTime;
    WV_TIME		m_endTime;
    unsigned int	m_flags;
    unsigned int	m_tclResults;
    int			m_min;
    int			m_max;

    void copy (const WVAnalysisPack& x);

  public:

#if PARISC_HPUX10
    friend class CAnalysisDlg;
#endif

    // Constructor - requires a pointer to an Event Base...
    WVAnalysisPack (const WVEventBase *pEB=0);

    // Destructor
    ~WVAnalysisPack ();

    // Copy-constructor
    WVAnalysisPack (const WVAnalysisPack& x);

    // Assignment operator
    WVAnalysisPack& operator= (const WVAnalysisPack& x);

    // Get different names for analysis-pack
    string resultCtxName () const;
    string niceName () const;

    // Load the DLL module
    BOOL analysisPackLoad (const string &dllName);

    // Get the flags from the WVAP enumeration
    unsigned int flags () const;

    // perform () - defines the analysis period and the
    // range of contexts it will cover. Returns TRUE if
    // analysis succeeded, FALSE if not...
    BOOL perform
	(
	WV_TIME			    startTime,
	WV_TIME			    endTime,
	const vector<WVContext>&    ctxs
	);

    // numResults () - indicates number of results available
    unsigned int numResults () const;

    // resultsExport () - exports the results to the designated
    // destination according to the presence or absence of the
    // WVAP_DIRECTEXCEL flag...
    BOOL resultsExport ();


    /*******************************************************/
    /***** UTILITY METHODS AVAILABLE TO ANALYSIS-PACKS *****/
    /*******************************************************/

    // eventBase () - returns ptr to event base
    const WVEventBase *eventBase () const;

    // nextEvent () - gets the next event from the event base, if
    // there is one, and returns TRUE. If no more events are
    // available, return FALSE...
    BOOL nextEvent (WVEvent *pEvent);

    // currentTime () - returns the time of the last event
    // read by the analysis-pack using nextEvent ()...
    WV_TIME currentTime () const;

    // eventValueAdd () - adds an EVENT_VALUE to the computed 
    // context where results of this analysis are stored. This
    // is the default way that analysis-packs generate events.
    // This method will fail if the AP has specified the flag
    // WVAP_DIRECTEXCEL...
    BOOL eventValueAdd (WV_TIME t, int value);

    // resultScaleSet () - sets the scale for view-graph type
    // results
    BOOL resultScaleSet (int min, int max);

    // cellAdd () - if the analysis-pack has specified the
    // WVAP_DIRECTEXCEL flag, then it may call this function
    // at any time to export a cell to Excel. If the flag wasn't 
    // specified then the function will fail...
    BOOL cellAdd (const char *cellName, const char *cellValue);

    };

////////////////////////////////////////////////////////////////////////
//
// Definitions for those people writing analysis packs - functions
// must be exported from the analysis-pack DLL with the linkage
// WV_APACK_FN.
//
////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#define WV_APACK_FN extern "C" __declspec (dllexport)
#else
#define WV_APACK_FN extern "C"
#endif 

////////////////////////////////////////////////////////////////////////
//
// The analysis-pack's begin() function should return a
// combination of these values (OR'ed together) if it
// wants to make use of any of the extended features,
// such as 'direct export to Excel'...
//
////////////////////////////////////////////////////////////////////////

enum WVAP
    {
    WVAP_DIRECTEXCEL  = 0x01, // export to Excel, not ViewGraph
    WVAP_ALLCONTEXTS  = 0x02  // must have all ctxs
    };



#endif // __WVANALYZE_H__
