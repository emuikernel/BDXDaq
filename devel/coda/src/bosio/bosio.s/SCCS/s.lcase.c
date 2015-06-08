h00214
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/lcase.c
e
s 00030/00000/00000
d D 1.1 00/08/10 11:10:14 boiarino 1 0
c date and time created 00/08/10 11:10:14 by boiarino
e
u
U
f e 0
t
T
I 1
/*DECK ID>, LCASE. */
 
#include <ctype.h>
#if defined(APOLLO) || defined(ALLIANT)
/* on APOLLO and ALLIANT 'tolower' is a macro, not a function,
   and its behaviour is undefined if the argument is not in
   upper case, hence...                                       */
#ifndef TOLOWER
#define TOLOWER(x) isupper(x) ? tolower(x) : x
#endif
#else
#ifndef TOLOWER
#define TOLOWER(x) tolower(x)
#endif
#endif
 
 
/* convert a string to lower case */
 
int lcase(charptr)
char *charptr;
 
{   char *ptr;
 
    ptr = charptr;
    --ptr;
    while(*++ptr != '\0') *ptr = TOLOWER(*ptr);
    return(0);
}
 
E 1
