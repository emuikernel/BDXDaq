/* UITcl.h - main header file for the UITcl DLL
 */

/*
modification history
--------------------
01d,25aug98,wmd  added HPUX to XINLINE conditional.
01c,17aug98,j_k  fixed dialog units to pixels and vice-versa conversion.
01b,21jul98,c_s  fix callback type-safety issues
01a,14jul98,c_s  mod hist added and (WV2->T2 merge).
*/

//
#ifndef __INCUITclh
#define __INCUITclh


#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#if defined (__SUNPRO_CC) || defined (PARISC_HPUX10)
#define	XINLINE
#else
#define XINLINE inline 
#endif 

#include "Resource.h"		// main symbols

#define UITclExport __declspec(dllexport)
#ifndef MAINWIN
#define STDNAMESPACE std::
#pragma warning (disable: 4786)
#else   // MAINWIN
#define STDNAMESPACE 
#endif


#define UITCL_INFO      0
#define UITCL_WARNING   1
#define UITCL_ERROR     2

UITclExport void UITclLog(UINT uType, const char *format, va_list ap);

#include "UITclCommandInfo.h"
#include "UITclRoutines.h"
#include "UITclHelper.h"
#include "UITclWndHelper.h"
#include "UITclCtrlHelper.h"

/////////////////////////////////////////////////////////////////////////////
// UITclDll main header file
// See UITcl.cpp for the implementation of this class
//
#endif	// __INCUITclh

/////////////////////////////////////////////////////////////////////////////
