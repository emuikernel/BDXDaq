h17568
s 00010/00005/00079
d D 1.4 07/10/18 23:39:41 boiarino 5 4
c *** empty log message ***
e
s 00006/00006/00078
d D 1.3 00/10/26 13:07:14 wolin 4 3
c const
e
s 00021/00005/00063
d D 1.2 96/09/26 15:03:33 wolin 3 1
c Added fortran callable strc2f
e
s 00000/00000/00000
d R 1.2 96/09/11 09:51:29 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libutil/s/strf.c
c Name history : 1 0 s/strf.c
e
s 00068/00000/00000
d D 1.1 96/09/11 09:51:28 wolin 1 0
c String utilities for fortran-to-c interface
e
u
U
f e 0
t
T
I 1
/*
*  strf.c
*
*
*  C functions help move strings between C and Fortran
*
*  ejw, 11-sep-96
*
*/


I 3
#define _POSIX_SOURCE 1
#define __EXTENSIONS__
E 3

I 3

E 3
#include <stdio.h>
I 5
/*
E 5
#include <macros.h>
I 5
*/
#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
E 5

D 5

E 5
/*-----------------------------------------------------------------------*/


/* 
*  duplicates fortran string, removes trailing blanks and inserts ending NULL 
*/

D 3
char *strdupf(fortran_string,len)
     char *fortran_string;
     int len;
E 3
I 3
D 4
char *strdupf(char *fortran_string, int len)
E 3
{
E 4
I 4
D 5
char *strdupf(const char *fortran_string, int len) {
E 5
I 5
char *
strdupf(const char *fortran_string, int len) {
E 5

E 4
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
D 3
*  copies c string (nul terminated) into fortran character variable (blank padded)
E 3
I 3
*  copies c string (null terminated) into fortran character variable (blank padded)
E 3
*/

D 3
void strc2f(char *fortran_string, int maxf, char *cstring)
E 3
I 3
D 4
void strc2f(char *fortran_string, char *cstring, int maxf)
E 3
{
E 4
I 4
D 5
void strc2f(char *fortran_string, const char *cstring, int maxf) {
E 5
I 5
void
strc2f(char *fortran_string, const char *cstring, int maxf) {
E 5

E 4
  int i;
  int len=strlen(cstring);
D 5
  int n=min(maxf,len);
E 5
I 5
  int n=MIN(maxf,len);
E 5

  strncpy(fortran_string,cstring,n);
  
  for(i=n; i<maxf; i++){
    fortran_string[i]=' ';
  }

  return;
}
I 3


/*-----------------------------------------------------------------------*/


/*
*  (fortran callable) copies c string passed into fortran function 
*       into fortran character variable
*/

D 4
void strc2f_(char *fortran_string, char *cstring, int maxf)
{
E 4
I 4
D 5
void strc2f_(char *fortran_string, const char *cstring, int maxf) {
E 5
I 5
void
strc2f_(char *fortran_string, const char *cstring, int maxf) {
E 5

E 4
  strc2f(fortran_string,cstring,maxf);
  return;
}
E 3


/*-----------------------------------------------------------------------*/


E 1
