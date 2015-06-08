/*-----------------------------------------------------------------------------
** reader.c	free field reader and string utilities
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

#include <stdlib.h>
#include <ctype.h>
#define _READER_
#include "reader.h"
#include "strutil.h"

#define PARSE_SEPARATORS	",= \t\n"

int
sepfield(char *str, int count)
{
  int len, val, j;
  char *ptr, *end;
  char tmpval[1024], *tmp;

  strcpy(tmpval, str);
  ptr = strchr(tmpval, '#');
  end = tmpval + strlen(tmpval);
  if (ptr)
    *ptr = (char) 0;
  ptr = strparse(tmpval, PARSE_SEPARATORS);
  while (ptr) {
    strncpy(field[count], ptr, STRING_MAX - 1);
    field[count][STRING_MAX - 1] = (char) 0;
    count++;
    ptr = strparse((char *) NULL, PARSE_SEPARATORS);
  }
#ifdef DEBUG
  printf("sepfield: %d\t", count);
  for (j = 0; j < count; j++)
    printf("->%s<-", field[j]);
  printf("\n");
#endif
  return count;
}

void
catfield(char *str, int count)
{
  int i;

  if (count > 0) {
    strcpy(str, field[0]);
    for (i = 1; i < count; i++) {
      strcat(str, " ");
      strcat(str, field[i]);
    }
  }
  else
    *str = '\0';
}

void
upper(char *str)
{
  int i;
  char *dest;

  dest = str;
  for (i = strlen(str); i > 0; i--) {
    if (*str != ' ')
      *dest++ = toupper(*str);
    str++;
  }
  *dest = (char) 0;
}

void
tofloat(int count)
{
  int i;
  float val;

  for (i = 0; i < count; i++) {
    val = (float) atof(field[i]);
    ffield[i] = val;
  }
  for (i = count; i < MAXFIELD; i++)
    ffield[i] = 0.0;
#ifdef DEBUG
  if (1) {
    int j;

    printf("tofloat: (%d)", count);
    for (j = 0; j < count; j++)
      printf("->%f<-", ffield[j]);
    printf("\n");
  }
#endif
}

float
getfloat(int count)
{
  int i;

  ffield[count] = (float) atof(field[count]);
  return ffield[count];
}

#define isfloat(c) (isdigit(c) || c=='.' || c=='-')
#define isfloatany(c) (isdigit(c) || c=='.' || c=='e' || c=='E' || c=='-' || c=='+')

int
checkfloat(int loc)
{
  char *c, *start;
  int len;

  c = field[loc];
  if (*c && (isfloat(*c))) {
    c++;
    while (*c) {
      if (!isfloatany(*c))
	return 0;
      c++;
    }
  }
  else {
    return 0;
  }
#ifdef DEBUG
  printf("%s is a float\n", field[loc]);
#endif
  return 1;
}

void
clearfield()
{
  int i;

  for (i = 0; i < MAXFIELD; i++) {
    ffield[i] = 0.0;
    strcpy(field[i], "");
  }
}

int
reads(int fd, char *buf, int num)
{
  int count;

  count = 0;
  while (count < num) {
    if (read(fd, buf, 1) == 1) {
      count++;
      if (*buf == '\n')
	break;
      buf++;
    }
    else if (count > 0)
      break;
    else {
      count = -1;
      break;
    }
  }
  *buf = '\0';
  return count;
}

/*
 * ** Read a line (ended by \n) from the specified file and call sepfield() to
 * ** break the line up into fields.
 */
int
getline(FILE * file, char *loc)
{
  char command[64], *remaining, c;
  int count, i, more, loop;
  static char inputline[1024], storedline[2048], *ptr = NULL;

  do {
    strcpy(storedline, "");
    do {
      more = FALSE;
      ptr = fgets((char *) &inputline, 1000, file);
      if (ptr == (char *) NULL) {
	if (strlen(storedline) == 0)
	  return -1;
	more = FALSE;
      }
      else {
	i = strlen(inputline) - 1;
	loop = TRUE;
	while ((i >= 0) && loop) {
	  c = inputline[i];
#ifdef DEBUG
	  printf("inputline[%d]=%c\n", i, c);
#endif
	  if ((c == '/') || (c == '\\')) {
	    more = TRUE;
	    loop = FALSE;
	    inputline[i] = '\n';
	    inputline[i + 1] = '\0';
	  }
	  else if ((c != ' ') && (c != '\t') && (c != '\n'))
	    loop = FALSE;
	  i--;
	}
	strcat(storedline, inputline);
      }
    } while (more);
    count = sepfield(storedline, 0);
  } while ((count == 0) && ptr);
  loc = ptr;
  return count;
}
