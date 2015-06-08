h42401
s 00000/00000/00000
d R 1.2 96/09/11 09:51:53 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libutil/s/getaddr.c
c Name history : 1 0 s/getaddr.c
e
s 00018/00000/00000
d D 1.1 96/09/11 09:51:52 wolin 1 0
c Gets address of arg for fortran-to-c interface
e
u
U
f e 0
t
T
I 1
/*
*  Fortran callable function returns (by ref) address of argument (passed by ref)
*
*  for interfacing C to Fortran
*
*  ejw, 11-sep-96
*
*/

int *getaddr_(int *var){

  return (int*) var;

}

/*-----------------------------------------------------------------------*/


E 1
