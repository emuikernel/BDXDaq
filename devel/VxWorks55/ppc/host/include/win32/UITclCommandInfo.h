/* UITclCommandInfo.h - CUITclCommandInfo class interface */

/* Copyright 1984-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,21jul98,c_s  fix callback type-safety issues
01a,08oct97,j_k written.
*/

/*
DESCRIPTION

This file documents the class interface for CUITclCommandInfo.

*/

#ifndef __UITclCommandInfo_H__
#define __UITclCommandInfo_H__

#pragma warning(disable: 4275)  // deriving exported class from non-exported
#pragma warning(disable: 4251)  // using non-exported as public in exported

/////////////////////////////////////////////////////////////////////////////
// CUITclCommandInfo

typedef struct Tcl_Interp Tcl_Interp;

#include "UITclRoutines.h"

class CUITclHelper;

extern UITclExport void CopyOptions(CServiceOptions* pMapDest, CServiceOptions* pMapSrc);

class UITclExport CUITclCommmadInfo {
public:
	CUITclCommmadInfo() { }
	CUITclCommmadInfo(Tcl_Interp* pTclInterp)
	{
		m_pTclInterp = pTclInterp;
	}
	~CUITclCommmadInfo()
	{
		// deallocate any allocated memory in 'argsList'
		POSITION pos = m_cMap.GetStartPosition();
		while (pos)
		{
			UITclOptnType otKey;
			void* value;
			m_cMap.GetNextAssoc(pos, (void*&)otKey, (void*&)value);
			if (value != NULL) free (value);
		}
	}

	Tcl_Interp*& TclInterpreter()
	{ return m_pTclInterp; }

	CServiceOptions& Arguments()
	{ return m_cMap; }

	CPtrArray& Values()
	{ return m_aValues; }

	char* ValueItem(int nIndex)
	{
		if (nIndex < ValueItems())
			return ((char*)m_aValues.GetAt(nIndex));
		return NULL;
	}

	int ValueItems()
	{ return m_aValues.GetSize(); }
	
	int& ElementsCount()
	{ return m_iElems; }

	char**& Data()
	{ return m_ppData; }

	CString& ReturnValue()
	{ return m_csReturnVal; }
	
	void SetArgsAt(void* key, void* data, size_t size)
	{
		void* mem = NULL;
		if (size) {
			mem = malloc(size);
			memcpy(mem, data, size);
		}
		m_cMap.SetAt(key, (void*&)mem);
	}
	BOOL LookupArgs(void* key, void*& rValue ) const
	{ return(m_cMap.Lookup(key, rValue)); }
	void SetValsAt(int nIndex, void* newElement)
	{ m_aValues.SetAt(nIndex, newElement); }
	void SetValsSize(int newSize)
	{ m_aValues.SetSize(newSize); }

protected:
	CServiceOptions		m_cMap;
	Tcl_Interp*			m_pTclInterp;
	CPtrArray			m_aValues;

	CString				m_csReturnVal;
	int					m_iElems;
	char**				m_ppData;
};


typedef int (CUITclHelper::*ServiceCallbackProc) (CUITclCommmadInfo * pCmdInfo);
typedef int (*SvcCbProcWrapper) (CUITclHelper *, CUITclCommmadInfo *);


#endif	// __UITclCommandInfo_H__
/////////////////////////////////////////////////////////////////////////////
