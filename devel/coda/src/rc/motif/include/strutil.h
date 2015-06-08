/*-----------------------------------------------------------------------------
** strutil.h	A reusable string allocator
**
** Private header file
**
** Copyright (c) 1995 Robert W. McMullen
**
** Permission to use, copy, modify, distribute, and sell this software and its
** documentation for any purpose is hereby granted without fee, provided that
** the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  The author makes no representations about the suitability
** of this software for any purpose.  It is provided "as is" without express
** or implied warranty.
**
** THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
** ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL
** THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
** ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
** SOFTWARE.
*/

#include <string.h>

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

typedef struct _STRINGSTORAGE {
	struct _STRINGSTORAGE *next;
	char	*storage;
	char	*free,*last;
} STRINGSTORAGE;

#ifndef MALLOCCHUNKSIZE
#define MALLOCCHUNKSIZE 4096
#endif

#ifndef STRCHUNKSIZE
#define STRCHUNKSIZE (MALLOCCHUNKSIZE-sizeof(STRINGSTORAGE))
#endif
#ifndef INITSTRCHUNKSIZE
#define INITSTRCHUNKSIZE 4096
#endif

#if __STDC__ || defined(__cplusplus)
#define P_(s) s
#else
#define P_(s) ()
#endif

/* strutil.c */
char strclast P_((const char *str));
char *strlast P_((const char *str));
char *strrstr P_((const char *str, const char *pat));
char *strrcmp P_((const char *str, const char *pat));
char *strparse P_((char *str, char *primary));
STRINGSTORAGE *strallocinit P_((void));
char *stralloc P_((STRINGSTORAGE *ss1, int size));
void strfreeall P_((STRINGSTORAGE *ss1));

#undef P_
