/* locale.h - locale header file */

/* Copyright 1992-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,17feb99,mrs  Add C++ support for NULL, (SPR #25079).
01e,21jul96,dbt  moved localeconv() definition (SPR #3627).
01d,15oct92,rrr  silenced warnings
01c,22sep92,rrr  added support for c++
01b,12jul92,smb  modified definition of localconv for MIPS cpp.
01a,08jul92,smb  written.
*/

#ifndef __INClocaleh
#define __INClocaleh

#ifdef __cplusplus
extern "C" {
#endif

#include "types/vxANSI.h"

#ifndef NULL
#if defined __GNUG__
#define NULL (__null)
#else
#if !defined(__cplusplus) && 0
#define NULL ((void*)0)
#else
#define NULL (0)
#endif
#endif
#endif

#define LC_ALL		1
#define LC_COLLATE	2
#define LC_CTYPE	3
#define LC_MONETARY	4
#define LC_NUMERIC	5
#define LC_TIME		6

struct lconv
	{
	char *decimal_point;		/* LC_NUMERIC */
	char *thousands_sep;
	char *grouping;
	char *int_curr_symbol;		/* LC_MONETARY */
	char *currency_symbol;
	char *mon_decimal_point;
	char *mon_thousands_sep;
	char *mon_grouping;
	char *positive_sign;
	char *negative_sign;
	char int_frac_digits;
	char frac_digits;
	char p_cs_precedes;
	char p_sep_by_space;
	char n_cs_precedes;
	char n_sep_by_space;
	char p_sign_posn;
	char n_sign_posn;
	};

extern struct lconv __locale;

#if defined(__STDC__) || defined(__cplusplus)

extern char    *setlocale(int __category, const char *___locale);
extern struct lconv *localeconv(void);

#else	/* __STDC__ */

extern char    *setlocale();
extern struct lconv *localeconv();

#endif	/* __STDC__ */

#define 	localeconv()	(&__locale)

#ifdef __cplusplus
}
#endif

#endif /* __INClocaleh */
