/*-----------------------------------------------------------------------------
** strutil.c	String utilities and reusable string allocator
**
** Copyright (c) 1996 Robert W. McMullen
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

#include "strutil.h"

/*
 * strrstr.c -- find last occurence of string in another string
 */

char
strclast(const char *str)
{
  size_t len;

  if (str) {
    len = strlen(str);
    if (len > 0)
      return *(str + len - 1);
  }
  return '\0';
}

char *
strlast(const char *str)
{
  size_t len;

  if (str) {
    len = strlen(str);
    if (len > 0)
      return ((char *)str + len - 1);
  }
  return NULL;
}

char *
strrstr(const char *str, const char *pat)
{
  size_t len, patlen;
  const char *p;

  if (str && pat) {
    len = strlen(str);
    patlen = strlen(pat);

    if (patlen > len)
      return NULL;
    for (p = str + (len - patlen); p > str; --p)
      if (*p == *pat && strncmp(p, pat, patlen) == 0)
	return (char *) p;
  }
  return NULL;
}

char *
strrcmp(const char *str, const char *pat)
{
  size_t len, patlen;
  const char *p;

  if (str && pat) {
    len = strlen(str);
    patlen = strlen(pat);

    if (patlen > len)
      return NULL;
    p = str + (len - patlen);
    if (*p == *pat && strncmp(p, pat, patlen) == 0)
      return (char *) p;
  }
  return NULL;
}

char *
strparse(char *str, char *primary)
{
  static int start, len;
  static char *save;
  static char quotes[] = "'\"";
  int found, skip, i, end, loc;
  int plen, qlen;
  char c, *begin;

  if (primary) {
    if (str) {
      save = str;
      len = strlen(str);
      start = 0;
    }
    if (start >= len)
      return NULL;
    plen = strlen(primary);
    qlen = strlen(quotes);

/* find first valid character: */
    skip = TRUE;
    while ((start < len) && (skip)) {
      c = save[start];
      skip = FALSE;
      for (i = 0; i < plen; i++) {
	if (c == primary[i]) {
	  skip = TRUE;
	  break;
	}
      }
      if (skip)
	start++;
    }
    if (start >= len)
      return NULL;

    begin = &save[start];

/* OK, are pointing to first delimeter */

    if (*begin == '\"') {
      begin++;
      start++;
      while (start < len) {
	c = save[start];
	if (c == '\"') {
	  save[start] = '\0';
	  start++;
	  break;
	}
	else if (c == '\0') {
	  break;
	}
	start++;
      }
    }
    else {
      skip = FALSE;
      while ((start < len) && (!skip)) {
	c = save[start];
	skip = FALSE;
	for (i = 0; i < plen; i++)
	  if (c == primary[i]) {
	    skip = TRUE;
	    break;
	  }
	if (skip) {
	  save[start] = '\0';
	  start++;
	  break;
	}
	else
	  start++;
      }
    }
    return begin;
  }
  return NULL;
}

STRINGSTORAGE *
strallocinit()
{
  STRINGSTORAGE *s;

  s = (STRINGSTORAGE *) malloc(sizeof(STRINGSTORAGE) + STRCHUNKSIZE);
  s->storage = ((char *) s) + sizeof(STRINGSTORAGE);
  s->free = s->storage;
  s->last = s->storage + STRCHUNKSIZE - 1;
  s->next = NULL;
  return s;
}

char *
stralloc(STRINGSTORAGE * ss1, int size)
{
  STRINGSTORAGE *s, *slast;
  char *storage;
  int alloced_size;

  s = ss1;
  while (s) {
    if (s->free + size + 1 < s->last) {
      storage = s->free;
      s->free += size + 1;
      return storage;
    }
    slast = s;
    s = s->next;
  }

  if (size > STRCHUNKSIZE)
    alloced_size = size + STRCHUNKSIZE + 1;
  else
    alloced_size = STRCHUNKSIZE;

  s = (STRINGSTORAGE *) malloc(sizeof(STRINGSTORAGE) + alloced_size);
  slast->next = s;
  s->storage = ((char *) s) + sizeof(STRINGSTORAGE);
  s->free = s->storage + size + 1;
  s->last = s->storage + alloced_size - 1;
  s->next = NULL;

  storage = s->storage;
  printf("stralloc:  new %d bytes\n", sizeof(STRINGSTORAGE) + alloced_size);
  return storage;
}

void
strfreeall(STRINGSTORAGE * ss1)
{
  STRINGSTORAGE *s;

  s = ss1;
  printf("stralloced: ");
  while (s) {
    printf("%d ", (int) (s->free - s->storage));
    s->free = s->storage;
    s = s->next;
  }
  printf("\n");
}
