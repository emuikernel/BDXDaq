h53030
s 00000/00000/00000
d R 1.2 01/11/19 19:00:40 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/trlib.h
e
s 00024/00000/00000
d D 1.1 01/11/19 19:00:39 boiarino 1 0
c date and time created 01/11/19 19:00:39 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef _TRLIB_

#ifdef	__cplusplus
extern "C" {
#endif

/* trlib.h - header file for trlib package */


void sda_init_();
void sda_anal_(int *, int *, int *,int *,int *);
void sda_last_(int *);
void sda_cleananal_();
void prlib_(int *, int *);
void sda_path2anal_(int *, int *, int *);
void sda_trak_(int *, int *, int *, float *, float *);

#ifdef	__cplusplus
}
#endif

#define _TRLIB_
#endif

E 1
