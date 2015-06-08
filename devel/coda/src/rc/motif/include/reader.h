/*-----------------------------------------------------------------------------
** reader.h	free field reader and string utilities
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

#ifndef _READER_H
#define _READER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

#define MAXFIELD 40
#define STRING_MAX 63

#ifndef TRUE
#define TRUE                            1
#endif

#ifndef FALSE
#define FALSE                           0
#endif

#ifdef	WHERE
#undef	WHERE
#endif

#ifdef	_READER_
#define WHERE
#else
#define WHERE	extern
#endif

WHERE	char	field[MAXFIELD][STRING_MAX+1];
WHERE	float	ffield[MAXFIELD];

#undef WHERE

#if __STDC__ || defined(__cplusplus)
#define P_(s) s
#else
#define P_(s) ()
#endif

/* reader.c */
int sepfield P_((char *str, int count));
void catfield P_((char *str, int count));
void upper P_((char *str));
void tofloat P_((int count));
float getfloat P_((int count));
int checkfloat P_((int loc));
void clearfield P_((void));
int reads P_((int fd, char *buf, int num));
int getline P_((FILE *file, char *loc));

#undef P_

#endif /* _READER_H */
