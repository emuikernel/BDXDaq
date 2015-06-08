h05643
s 00141/00005/00877
d D 1.3 03/04/17 16:55:51 boiarino 4 3
c *** empty log message ***
e
s 00040/00016/00842
d D 1.2 02/01/29 15:55:55 boiarino 3 1
c update some cernlib functions by 'C' interface
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 19:12:19 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/util.s/utcern.c
e
s 00858/00000/00000
d D 1.1 01/11/19 19:12:18 boiarino 1 0
c date and time created 01/11/19 19:12:18 by boiarino
e
u
U
f e 0
t
T
I 1

/* utcern.c - library to resolve cern etc references */


#include <stdio.h>
#include <string.h>
#include <math.h>

#include "dclib.h"

/* some CERNLIB functions */


#define MOD(x1,x2) ( (x1) - (int)((x1)/(x2)) * (x2) )


/*
     TO FIT A STRAIGHT LINE    Y=A*X+B    TO L POINTS WITH ERROR E
     SEE MENZEL , FORMULAS OF PHYSICS P.116
     IF KEY=0 POINTS WITH Y=0. ARE IGNORED
     L IS NO. OF POINTS
*/

void
lfit_(float *x, float *y, int *l, int *key, float *a, float *b, float *e)
{
  lfit(x, y, *l, *key, a, b, e);
}

void
lfit(float *x, float *y, int l, int key, float *a, float *b, float *e)
{
  int j;
  float count,sumx,sumy,sumxx,sumyy,sumxy,xmed,ymed,scartx,scarty;

  /* calculate sums */

  if(l-2 >= 0)
  {
    count=0.;
    sumx=0.;
    sumy=0.;
    sumxy=0.;
    sumxx=0.;
    sumyy=0.;
    for(j=0; j<l; j++)
    {
      if(y[j] == 0. && key == 0) continue;
      sumx  += x[j];
      sumy  += y[j];
      count += 1.0;
    }
    if(count <= 1.) {*a=0.; *b=0.; *e=0.; return;}
    ymed = sumy / count;
    xmed = sumx / count;
    for(j=0; j<l; j++)
    {
      if(y[j] == 0. && key == 0) continue;
      scartx = x[j] - xmed;
      scarty = y[j] - ymed;
      sumxy += scartx*scarty;
      sumxx += scartx*scartx;
      sumyy += scarty*scarty;
    }

    /* fit parameters */
    if(sumxx == 0.) {*a=0.; *b=0.; *e=0.; return;}
    *a = sumxy / sumxx;
    *b = ymed - (*a)*xmed;
	if(count < 3.) {*e=0.; return;}
    *e = (sumyy - sumxy*(*a))/(count-2.0);
  }
  else		/* isufficient points */
  {
    *a=0.;
    *b=0.;
    *e=0.;
  }

  return;
}


/*
     TO PERFORM A WEIGHTED STRAIGHT LINE FIT

     FOR FORMULAE USED SEE MENZEL, FORMULAS OF PHYSICS P.116

     FIT IS OF Y=AX+B , WITH S**2 ESTIMATOR E. WEIGHTS ARE IN W.
          (weights = 1/sigma**2 - S.B.)
     IF KEY=0, POINTS WITH Y=0 ARE IGNORED
     L IS NO. OF POINTS
*/

void
lfitw_(float *x, float *y, float *w, int *l, int *key, float *a, float *b, float *e)
{
  lfitw(x, y, w, *l, *key, a, b, e);
}

void
lfitw(float *x, float *y, float *w, int l, int key, float *a, float *b, float *e)
{
  int j,icnt;
  float w2,w2x,w2y,w2xy,w2x2,w2y2,ww,wwf,wwfi,tmp;

  /* calculate sums */

  if(l > 1)
  {
    w2   = 0.;
    w2x  = 0.;
    w2y  = 0.;
    w2xy = 0.;
    w2x2 = 0.;
    w2y2 = 0.;
    icnt = 0;
    for(j=0; j<l; j++)
    {
      if(y[j] == 0. && key == 0) continue;
      ww   = w[j]*w[j];
      w2   = ww + w2;
      wwf  = ww*x[j];
      w2x  = wwf + w2x;
      w2x2 = wwf*x[j] + w2x2;
      w2xy = wwf*y[j] + w2xy;
      wwfi = ww*y[j];
      w2y  = wwfi + w2y;
      w2y2 = wwfi*y[j] + w2y2;
      icnt ++;
	}

    /* fit parameters */
    *a = (w2xy-w2x*w2y/w2)/(w2x2-w2x*w2x/w2);
    *b = (w2y-(*a)*w2x)/w2;
    if(icnt <= 2)
    {
      *e=0.;
      printf("lfitw ERROR: icnt=%d l=%d y=",icnt,l);
      for(j=0; j<l; j++) printf(" %f",y[j]);
      printf("\n");
      return;
    }
    tmp = w2xy-w2x*w2y/w2;
    *e = (w2y2-w2y*w2y/w2-tmp*tmp/(w2x2-w2x*w2x/w2))/(icnt-2);
  }
  else  /* isufficient points */
  {
    *a = 0.;
    *b = 0.;
    *e = 0.;
  }
  return;
}


/*
      SUBROUTINE VSUB (A,B,X,N)
      DIMENSION A(*),B(*),X(*)
      IF (N.LE.0)  RETURN
      DO 9 I= 1,N
    9 X(I)= A(I) - B(I)
      RETURN
      END
*/
void vsub_(float *a, float *b, float *x, int *n)
{
I 3
  vsub(a, b, x, *n);
}

void vsub(float *a, float *b, float *x, int n)
{
E 3
  int i;
D 3
  for(i=0; i<(*n); i++) x[i] = a[i] - b[i];
E 3
I 3
  for(i=0; i<n; i++) x[i] = a[i] - b[i];
E 3
  return;
}

/*
      SUBROUTINE UCOPY (A,B,N)
      INTEGER   A(*),B(*)
      IF (N.EQ.0) RETURN
      DO 21 I=1,N
   21 B(I)=A(I)
      RETURN
      END
*/
void ucopy_(int *a, int *b, int *n)
{
I 3
  ucopy(a, b, *n);
}

void ucopy(int *a, int *b, int n)
{
E 3
  int i;
D 3
  for(i=0; i<(*n); i++) b[i] = a[i];
E 3
I 3
  for(i=0; i<n; i++) b[i] = a[i];
E 3
  return;
}

/*
      INTEGER FUNCTION IBSET(IZW,IZP)
      INTEGER IZW,IZP
      IBSET =  OR (IZW,LSHIFT(1,IZP)) ! could be LSHFT for some platforms
*/
int ibset_(int *izw, int *izp)
{
  int tmp;
  tmp = 1;
  return( (*izw) | (tmp << (*izp)) );
}

/*
      LOGICAL FUNCTION BTEST(IZW,IZP)
      INTEGER IZW,IZP
      BTEST = AND (IZW,LSHIFT(1,IZP)) .NE. 0
*/
int btest_(int *izw, int *izp)
{
  int tmp;
  tmp = 1;
  return( (*izw) & (tmp << (*izp)) );
}

/*
      SUBROUTINE CROSS(X,Y,Z)
      REAL X(3),Y(3),Z(3)
      Z(1)=X(2)*Y(3)-X(3)*Y(2)
      Z(2)=X(3)*Y(1)-X(1)*Y(3)
      Z(3)=X(1)*Y(2)-X(2)*Y(1)
*/
void cross_(float x[3], float y[3], float z[3])
{
I 3
  cross(x, y, z);
}

void cross(float x[3], float y[3], float z[3])
{
E 3
  z[0] = x[1] * y[2] - x[2] * y[1];
  z[1] = x[2] * y[0] - x[0] * y[2];
  z[2] = x[0] * y[1] - x[1] * y[0];
  return;
}

/*
      REAL FUNCTION VMOD(A,N)
      INTEGER N,I
      REAL A(*),XX
      XX = 0.
      IF(N.LE.0) GOTO 100
      DO I = 1,N
        XX = XX + A(I)*A(I)
      ENDDO
      IF(XX.LE.0.) GOTO 100
      VMOD = SQRT (XX)
      RETURN
  100 VMOD = 0.
*/
float vmod_(float *a, int *n)
{
D 3
  int i;
  float xx;
E 3
I 3
  return(vmod(a, *n));
}
E 3

D 3
  xx = 0.0;
  if(*n <= 0) return(0.);
  for(i=0; i < *n; i++) xx = xx + a[i] * a[i];
E 3
I 3
float vmod(float *a, int n)
{
  int i;
  float xx = 0.0;
  for(i=0; i<n; i++) xx = xx + a[i] * a[i];
E 3
  if(xx <= 0.0) return(0.);
  return( sqrt(xx) );
}

/*
      SUBROUTINE VUNIT(A,B,N)
      INTEGER N,I
      REAL A(*),B(*),XX,YY
      IF(N.LE.0) RETURN
      XX= 0.
      DO I = 1,N
        XX = XX + A(I)*A(I)
      ENDDO
      IF(XX.LE.0.) RETURN
      YY = 1. / SQRT(XX)
      DO I = 1,N
        B(I) = A(I) * YY
      ENDDO
*/
void vunit_(float *a, float *b, int *n)
{
I 3
  vunit(a, b, *n);
}

void vunit(float *a, float *b, int n)
{
E 3
  int i;
  float xx, yy;

D 3
  if(*n <= 0) return;
E 3
I 3
  if(n <= 0) return;
E 3
  xx = 0.;
D 3
  for(i=0; i < *n; i++) xx = xx + a[i] * a[i];
E 3
I 3
  for(i=0; i<n; i++) xx = xx + a[i] * a[i];
E 3
  if(xx <= 0.) return;
  yy = 1. / sqrt(xx);
D 3
  for(i=0; i < *n; i++) b[i] = a[i] * yy;
E 3
I 3
  for(i=0; i<n; i++) b[i] = a[i] * yy;
E 3
}

/*
      REAL FUNCTION VDOT(X,Y,N)
      INTEGER N,I
      REAL X(*),Y(*),XX
      IF(N.LE.0) THEN
        VDOT = 0.
        RETURN
      ENDIF
      XX= 0.
      DO I = 1,N
        XX = XX + X(I)*Y(I)
      ENDDO
      VDOT = XX
*/
float vdot_(float *x, float *y, int *n)
{
D 3
  int i;
  float xx;
E 3
I 3
  return(vdot(x, y, *n));
}
E 3

D 3
  if(*n <= 0) return(0.);
  xx = 0.;
  for(i=0; i < *n; i++) xx = xx + x[i] * y[i];
  return( xx );
E 3
I 3
float vdot(float *x, float *y, int n)
{
  int i;
  float xx = 0.;
  for(i=0; i<n; i++) xx = xx + x[i] * y[i];
  return(xx);
E 3
}

/*
      INTEGER FUNCTION IUCOMP(ITEXT,IVECT,N)
      INTEGER IVECT(9)
      IF(N.EQ.0) THEN
        J = 0
        IUCOMP = J
        RETURN
      ENDIF
      DO J = 1,N
        IF(ITEXT.EQ.IVECT(J)) THEN
          IUCOMP = J
          RETURN
        ENDIF
      ENDDO
      J = 0
      IUCOMP = J
*/
int iucomp_(void *text, void *vect, int *n, int len)
{
  int j, *itext, *ivect;

  if(*n == 0) return(0);
  itext = (int *) text;
  ivect = (int *) vect;
  for(j=1; j<=*n; j++)
  {
    /*printf("iucomp: %4.4s --- %4.4s\n",itext,&ivect[j-1]);*/
    if(*itext == ivect[j-1]) return(j);
  }
  return(0);
}

/* Flat Random Numbers (from Numerical Recipes:"RAN2")
      REAL FUNCTION SRAN(IDUM)
      SAVE
      INTEGER IDUM
      INTEGER M, IA, IC
      REAL RM
      PARAMETER (M=714025, IA=1366, IC=150889, RM=1.4005112E-6)
      INTEGER J, IY, IFF, IR(97)
      DATA IFF /0/
      IF(IDUM.LT.0 .OR. IFF.EQ.0) THEN
        IFF = 1
        IDUM = MOD(IC-IDUM,M)
        DO 11 J=1,97
          IDUM = MOD(IA*IDUM+IC,M)
          IR(J) = IDUM
 11     CONTINUE
        IDUM = MOD(IA*IDUM+IC,M)
        IY = IDUM
      ENDIF
      J = 1 + (97*IY)/M
      IF(J.GT.97 .OR. J.LT.1) PAUSE
      IY   = IR(J)
      SRAN = IY*RM
      IDUM = MOD(IA*IDUM+IC,M)
      IR(J)= IDUM
*/
float sran__(int *idum)
{
  static int m = 714025, ia = 1366, ic = 150889;
  static float rm = 1.4005112E-6;
  static int j, iy, ir[97];
  static int iff = 0;
  float tmp;

  if(*idum < 0 || iff == 0)
  {
    iff = 1;
    *idum = MOD(ic-(*idum),m);
    for(j=1; j<=97; j++)
    {
      *idum = MOD(ia*(*idum)+ic,m);
      ir[j-1] = *idum;
    }
    *idum = MOD(ia*(*idum)+ic,m);
    iy = *idum;
  }
  j = 1 + (97*iy) / m;
  if(j > 97 || j < 1) printf("sran: ERROR !!!\n");
  iy   = ir[j-1];
  tmp = (float)iy * rm;
  *idum = MOD(ia*(*idum)+ic,m);
  ir[j-1]= *idum;

  return(tmp);
}


#if defined(CERNLIB_QMDOS)
#include "dosgs/timel.c"
#elif defined(CERNLIB_QMIRTD)
#include "irtdgs/timel.c"
#else
/*>    ROUTINE TIMEL
  CERN PROGLIB# Z007    TIMEST          .VERSION KERNFOR  4.39  940228
  ORIG. 01/03/85  FCA, mod 03/11/93 GF
*/
#include <sys/types.h>
#include <time.h>
#include <sys/times.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/resource.h>

#ifndef CLOCKS_PER_SEC
#define  CLOCKS_PER_SEC CLK_TCK
#endif


/*#ifndef RLIMIT_CPU*/
#ifdef HPUX
#define RLIMIT_CPU 0    /* For HP-UX... */
#endif
/*#ifndef RLIM_INFINITY*/
#ifdef HPUX
#define RLIM_INFINITY 0x7fffffff    /* For HP-UX... */
#endif

#if defined(CERNLIB_QSYSBSD)||defined(CERNLIB_QMVMI)||defined(CERNLIB_QMVAOS)
#define HZ 60.;
#endif

#ifndef HZ
#ifdef __GNUC__
#define HZ 1
#else
#define HZ 1./CLOCKS_PER_SEC
#endif
#endif

struct tms tps;
static float timlim;
static time_t timstart, timlast;
static int tml_init = 1;
float deftim = 999.;

/* Serguei
#if defined(CERNLIB_QX_SC)
Serguei */
#define timest timest_
#define timex  timex_
#define timed  timed_
#define timel  timel_
/* Serguei
#endif
Serguei */
#if defined(CERNLIB_QXCAPT)
#define timest TIMEST
#define timex  TIMEX
#define timed  TIMED
#define timel  TIMEL
#endif

                   /*  local routine called by timst, and time_init */
static void time_st(timl)
float timl;
{
    times(&tps);
    timlim = timl;
    timstart =  tps.tms_utime+tps.tms_cutime+tps.tms_stime+tps.tms_cstime;
    timlast  = timstart;
    tml_init = 0;
    return;
}
                   /*  local routine to start by default  */
static void time_init()
{
	struct rlimit rlimit;
	float  maxtime;

	maxtime=deftim;

	if (getrlimit(RLIMIT_CPU, &rlimit)==0) {
		if ( rlimit.rlim_cur != RLIM_INFINITY )
		   maxtime = (float) rlimit.rlim_cur;
	}	

	time_st(maxtime);
	return;
}

void timest(timl)
float *timl;
{
 struct rlimit rlimit;
 float  maxtime;

 if (tml_init != 0) {

/*  get maximum time allowed by system, and do not allow more */
    maxtime = *timl;
    if (getrlimit(RLIMIT_CPU, &rlimit)==0) {
           maxtime = (float) rlimit.rlim_cur;
           maxtime = ( maxtime > *timl ) ? *timl : maxtime;
    }
    time_st(maxtime);
 }
 return;
}
void timex(tx)
/*
C
  CERN PROGLIB# Z007    TIMEX           .VERSION KERNFOR  4.39  940228
C
*/
float *tx;
{
   time_t timnow;
   if (tml_init) {
       time_init();
       *tx = 0.;
   }
   else {
       times(&tps);
       timnow = tps.tms_utime+tps.tms_cutime+tps.tms_stime+tps.tms_cstime;
       *tx = (float) (timnow - timstart) / HZ;
   }
   return;
}

void timed(td)
/*
C
  CERN PROGLIB# Z007    TIMED           .VERSION KERNFOR  4.39  940228
C
*/
float *td;
{
   time_t timnow;
   if (tml_init) {
       time_init();
       *td = timlim;
   }
   else {
       times(&tps);
       timnow = tps.tms_utime+tps.tms_cutime+tps.tms_stime+tps.tms_cstime;
       *td = (float) (timnow - timlast) / HZ;
       timlast = timnow;
   }
   return;
}

void timel(tl)
/*
C
  CERN PROGLIB# Z007    TIMEL           .VERSION KERNFOR  4.39  940228
C
*/
float *tl;
{
   time_t timnow;
   if (tml_init) {
       time_init();
       *tl = timlim;
   }
   else {
       times(&tps);
       timnow = tps.tms_utime+tps.tms_cutime+tps.tms_stime+tps.tms_cstime;
       *tl = timlim - (float) (timnow - timstart) / HZ;
   }
   return;
}
#ifdef __GNUC__
#undef time_t
#endif
/*> END <----------------------------------------------------------*/
#endif




#include <signal.h>

int
signalf_(int *signum, int *funct, int *flag)
{
  int  signo, istat;
  int  handler;
  void *oldhand;

  signo = *signum;

  if (*flag < 0)          handler = (int)funct;
  else if (*flag == 0)  handler = (int)SIG_DFL;
        else if (*flag == 1)  handler = (int)SIG_IGN;
        else                  handler = *flag;

      oldhand = signal(signo,handler);
      istat   = (int)oldhand;
#ifndef __GNUC__
      if (oldhand == SIG_ERR)  istat = -1;
#endif
      return istat;
}



D 4
void geteni_(char *fname, char *ftext, int *lgtext, int *lgname)
E 4
I 4
void
geteni_(char *fname, char *ftext, int *lgtext, int *lgname)
E 4
{
  char *ptname, *fchtak();
  char *pttext, *getenv();
  int  fchput();

  pttext = NULL;
  ptname = fchtak(fname,*lgname);
  if (ptname == NULL)          goto out;
  pttext = getenv (ptname);
  free(ptname);

out:
  *lgtext = fchput (pttext,ftext,*lgtext);
  return;
}


D 4
char *fchtak(char *ftext, int lgtext)
E 4
I 4
char *
fchtak(char *ftext, int lgtext)
E 4
{
  char *malloc();
  char *ptalc, *ptuse;
  char *utext;
  int  nalc;
  int  ntx, jcol;

  nalc  = lgtext + 8;
  ptalc = malloc (nalc);
  if (ptalc == NULL)     goto exit;
  utext = ftext;

  ptuse = ptalc;
  ntx   = lgtext;
  for (jcol = 0; jcol < ntx; jcol++)  *ptuse++ = *utext++;

  *ptuse = '\0';
exit:
  return  ptalc;
}

D 4
int fchput(char *pttext, char *ftext, int lgtext)
E 4
I 4
int
fchput(char *pttext, char *ftext, int lgtext)
E 4
{
  char *utext;
  int  limit, jcol;
  int  nhave;

  limit = lgtext;
  jcol  = 0;
  utext = ftext;
  if (pttext == NULL)          goto out;

/*--      copy the text to the caller   */
  for (jcol = 0; jcol < limit; jcol++)
  {
    if (*pttext == '\0')  break;
    *utext++ = *pttext++;
  }

out:
  nhave = jcol;
  for (; jcol < limit; jcol++) *utext++ = ' ';
  return nhave;
}


float
iw2rw_(float *iw)
{
  return(*iw);
}





/*
   parint.c - parabol interpolation

         Input parameters:

   x    - argument, in which function value must be found
   a[n] - array arguments
   f[n] - array function
   n    - array dimension

         Returned value:

   r - function value for argument x
*/

#include <stdio.h>

#define ABS(x) ((x) < 0 ? -(x) : (x))

D 4
float parint(float, float *, float *, int);

E 4
void
parint_(float *x, float *a, float *f, int *n, float *r)
{
  *r = parint(*x, a, f, *n);
  return;
}

float
parint(float x, float *a, float *f, int n)
{
  int k1,k2,k3;
  float r,b1,b2,b3,b4,b5,b6,c;

  printf("x=%f a[1]=%f f[1]=%f n=%d\n",x,a[1],f[1],n);

  if(x < a[0]) goto a11;
  if(x > a[n-1]) goto a4;
  k1 = 1;
  k2 = n;
a2:
  k3 = k2 - k1;
  if(k3 <= 1) goto a6;
  k3 = k1 + k3/2;

  if(a[k3-1] < x)       goto a7;
  else if(a[k3-1] == x) goto a8;
  else                  goto a9;

a7:
  k1 = k3;
  goto a2;
a9:
  k2 = k3;
  goto a2;
a8:
  r = f[k3-1];
  return(r);

a3:
  b1 = a[k1-1];
  b2 = a[k1+1-1];
  b3 = a[k1+2-1];
  b4 = f[k1-1];
  b5 = f[k1+1-1];
  b6 = f[k1+2-1];
  r = b4*((x-b2)*(x-b3))/((b1-b2)*(b1-b3))+
      b5*((x-b1)*(x-b3))/((b2-b1)*(b2-b3))+
      b6*((x-b1)*(x-b2))/((b3-b1)*(b3-b2));
  return(r);

a6:
  if(k2 != n) goto a3;
  k1 = n - 2;
  goto a3;
a4:
  c=ABS(x-a[n-1]);
  if(c < 0.1E-7) goto a5;
  k1 = n - 2;
a13:
  printf("parint: X is out of the interval , X=f7.3\n",x);
  return(0.0);

a5:
  r=f[n-1];
  return(r);

a11:
  c=ABS(x-a[0]);
  if(c < 0.1E-7) goto a12;
  k1 = 1;
  goto a13;
a12:
  r = f[0];
  return(r);
}



#define RADDEG 57.2957795130823209
#define DEGRAD  0.0174532925199432958
#define PI      3.14159265358979322702

/* 1<*isec<6
 0.<*theta_<PI/2 
 -PI<*phi_<PI in radians in a Detector Coordinate System
 */
float
massage_(float *p, int *isec, float *theta_, float *phi_)
{
  float pel,theta,phi,thetad,phid,phioff,tmp;

  pel = *p;
  theta = *theta_;
  phi = *phi_;
  /*phi = (*phi_) - (PI/3.)*((*isec)-1);*/
  thetad = theta * RADDEG;
  phid = phi * RADDEG;
  phioff = 0.06;
printf("massage: p=%f sec=%d thetad=%f phid=%f\n",*p,*isec,thetad,phid);

  tmp = cos(theta);
  if(tmp > 0.96)
  {
    tmp -= 0.96;
    pel *= (1.-tmp*tmp*tmp*900.);
  }

  tmp = phi + phioff;
  switch(*isec)
  {
    case 1:
      if(thetad < 25.)
        pel *= (1.+tmp*0.006+tmp*tmp*0.06);
      pel *= (1.+0.0025);
      break;

    case 2:
      if(thetad < 25.)
        pel *= (1.+(tmp-PI/3.)*0.0+(tmp-PI/3.)*(tmp-PI/3.)*0.05);
      pel *= (1.+0.0036);
      break;

    case 3:
      if(thetad < 25.)
        pel *= (1.-(tmp-PI*2./3.)*0.05+(tmp-PI*2./3.)*(tmp-PI*2./3.)*0.06);
      pel *= (1.+0.0000);
      break;

    case 4:
      if(thetad < 25.)
      {
        if(phi > 0 && phi < PI)
          pel *= (1.-(tmp-PI)*0.05+(tmp-PI)*(tmp-PI)*0.02);
        if(phi > -PI && phi < 0.)
          pel *= (1.-(tmp+PI)*0.02+(tmp+PI)*(tmp+PI)*0.02);
      }
      pel *= (1.+0.0020);
      break;

    case 5:
      if(thetad < 25.)
        pel *= (1.-(tmp+PI*2./3.)*0.0175+(tmp+PI*2./3.)*(tmp+PI*2./3.)*0.10);
      pel *= (1.+0.0000);
      break;

    case 6:
      if(thetad < 25.)
        pel *= (1.-(tmp+PI/3.)*0.035+(tmp+PI/3.)*(tmp+PI/3.)*0.06);
      pel *= (1.+0.0000);
      break;

    default:
      printf("massage: ERROR: wrong sector # %d\n",*isec);
  }

  return(pel);
}
I 4



/* from Cole
c-------------------------------------------
      SUBROUTINE boost(BETA,P1,E1,P2,E2)    
C----------------------------------------------------------------------
C-
C-   Purpose : Makes Lorentz Boost (Lab <--> CM)
C-
C-   Inputs  : P1(3),E1 is four vector (Px,Py,Pz,E) 
C-             BETA is CM velocity
C-                  If BETA < 0. Boost from Lab to center mass system.
C-                  If BETA > 0. Boost from center mass to Lab system.
C- 
C-   Outputs : P2(3),E2
C----------------------------------------------------------------------
      IMPLICIT NONE
      real BETA, P1(3), E1, P2(3), E2
      real GAMMA
      P2(1) = P1(1)
      P2(2) = P1(2)
      IF(BETA**2.GT.1E-20) THEN    
        GAMMA 	= 1.0 / SQRT(1.0 - BETA**2)  
        P2(3) 	= GAMMA*(P1(3) + E1*BETA)    
        E2 	= GAMMA*(E1 + P1(3)*BETA) 
      ENDIF
      RETURN    
      END 
*/

void
boost_(float *bet, float p1[3], float *e1, float p2[3], float *e2)
{
  float gamma, beta, en;

  beta = *bet;
  en = *e1;
  p2[0] = p1[0];
  p2[1] = p1[1];
printf("boost: beta=%f %f\n",beta,beta*beta);
  if(beta*beta > 1.e-20)
  {
    gamma = 1.0 / sqrt(1.0 - beta*beta);  
    p2[2] = gamma * (p1[2] + en*beta);
    *e2   = gamma * (en + p1[2]*beta);
printf("boost: %f %f %f\n",gamma,p2[2],*e2);
  }

  return;
}

/*
      subroutine rotate(idir,the,phi,p1,p2)    
      implicit none
      integer idir
      real the, phi, p1(3), p2(3)
      real cth,sth,cph,sph
      integer j
      real rot(3,3),pv(3)  
      
      cth = cos(the)
      sth = sqrt(1.-cth**2)
      cph = cos(phi)
      sph = sin(phi)
      
      IF(the**2+phi**2 .GT. 1E-20) THEN   
        ROT(1,1) =  cth*cph 
        ROT(1,2) = -sph 
        ROT(1,3) =  -sth*cph  
        ROT(2,1) =  cth*sph  
        ROT(2,2) =  cph  
        ROT(2,3) =  -sth*sph 
        ROT(3,1) = sth  
        ROT(3,2) =  0. 
        ROT(3,3) = cth
        
        DO j = 1,3    
          pv(j) = p1(j)
        ENDDO    
        DO j = 1,3    
          IF(idir.GE.0) THEN
            p2(j) = ROT(j,1)*pv(1) + ROT(j,2)*pv(2) + ROT(j,3)*pv(3)
          ELSE
            p2(j) = ROT(1,j)*pv(1) + ROT(2,j)*pv(2) + ROT(3,j)*pv(3)
          ENDIF 
        ENDDO
      ENDIF

      RETURN    
      END   
*/

void
rotate_(int *idir_, float *the_, float *phi_, float p1[3], float p2[3])
{
  float idir, the, phi;
  float cth, sth, cph, sph, rot[3][3], pv[3];
  int j;

  idir = *idir_;
  the = *the_;
  phi = *phi_;
  cth = cos(the);
  sth = sqrt(1.-cth*cth);
  cph = cos(phi);
  sph = sin(phi);
      
  if(the*the+phi*phi > 1.e-20)
  {
    rot[0][0] = cth*cph;
    rot[1][0] = -sph;
    rot[2][0] = -sth*cph;
    rot[0][1] = cth*sph;
    rot[1][1] = cph;
    rot[2][1] = -sth*sph;
    rot[0][2] = sth;
    rot[1][2] = 0.;
    rot[2][2] = cth;

    for(j=0; j<3; j++) pv[j] = p1[j];
    for(j=0; j<3; j++)
    {
      if(idir >=0)
        p2[j] = rot[0][j]*pv[0] + rot[1][j]*pv[1] + rot[2][j]*pv[2];
      else
        p2[j] = rot[j][0]*pv[0] + rot[j][1]*pv[1] + rot[j][2]*pv[2]; 
    }
  }

  return;
}



E 4

E 1
