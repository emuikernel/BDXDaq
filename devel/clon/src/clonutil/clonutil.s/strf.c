/*
*  strf.c
*
*
*  C functions help move strings between C and Fortran
*
*  ejw, 11-sep-96
*
*/


#define _POSIX_SOURCE 1
#define __EXTENSIONS__


#include <stdio.h>
/*
#include <macros.h>
*/
#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )

/*-----------------------------------------------------------------------*/


/* 
*  duplicates fortran string, removes trailing blanks and inserts ending NULL 
*/

char *
strdupf(const char *fortran_string, int len) {

  char *s;
  int len_occ;

  /* find occupied length, not including trailing blanks */
  for(len_occ=len;(len_occ>0)&&(fortran_string[len_occ-1]==' ');len_occ--) ;

  s=(char *)malloc(len_occ+1);
  strncpy(s,fortran_string,(size_t)len_occ);
  s[len_occ]='\0';
  
  return(s);

}


/*-----------------------------------------------------------------------*/


/*
*  copies c string (null terminated) into fortran character variable (blank padded)
*/

void
strc2f(char *fortran_string, const char *cstring, int maxf) {

  int i;
  int len=strlen(cstring);
  int n=MIN(maxf,len);

  strncpy(fortran_string,cstring,n);
  
  for(i=n; i<maxf; i++){
    fortran_string[i]=' ';
  }

  return;
}


/*-----------------------------------------------------------------------*/


/*
*  (fortran callable) copies c string passed into fortran function 
*       into fortran character variable
*/

void
strc2f_(char *fortran_string, const char *cstring, int maxf) {

  strc2f(fortran_string,cstring,maxf);
  return;
}


/*-----------------------------------------------------------------------*/


