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
 
