/*  gen7120.h
 * 
 *   SP, 10-Sep-2002
 *
 */ 

#ifndef _GEN7120_H
#define _GEN7120_H

typedef struct gen7120
{
  int dt1;
  int dt2;
  int ddt;
  int rate;
  int N;
  int R[9];
  int TDAC;
  int QDAC;
  int TRange;
  int QRange;
  int Q;
  int genRunning;
} GEN7120;


/* Function declaration */
IMPORT	STATUS	pTickConnect (FUNCPTR routine, int arg);
IMPORT	STATUS	pTickEnable  (void);
IMPORT	STATUS	pTickDisable (void);
IMPORT	UINT32	pTickPeriod  (int pval);

/* Library of functions */
void gen7120clearInh    (int c, int *q);
int  gen7120readCSR     (int c, int n, int *q, int *x);
void gen7120set16DAC    (int c, int n, int data, int *q);
void gen7120set8DAC     (int c, int n, int data, int *q);
int  gen7120switchstat  (int c, int n, int prn);
int  gen7120checkinputs (int c, int n, int prn);
void gen7120inthandler  (int arg);

int  gen7120_start (int c, int n, int dt1, int dt2, int ddt, int rate, int N);
void gen7120_stop  (int c, int n);
int  gen7120_stat  (int c, int n, int prn);
int  gen7120_Tset  (int c, int n, int T);
int  gen7120_Qset  (int c, int n, int Q);
void gen7120_getpos ();

void gen7120initglob   ();
void gen7120setglobVar (int c, int n, char *ch, int val, int i);
int  gen7120getglobVar (int c, int n, char *ch, int i);
void gen7120prnglobVar (int c, int n);


#endif  /* _GEN7120_H */

