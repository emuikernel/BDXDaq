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

