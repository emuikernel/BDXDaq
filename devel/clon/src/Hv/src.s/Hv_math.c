/*
-----------------------------------------------------------------------
File:     Hv_math.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

/* This file contains various mathematical functions of general interest */

#include "Hv.h"	/* contains all necessary include files */

static float sqrarg;

Boolean   Hv_nrerror = False;

static float maxarg1,maxarg2;
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ? (maxarg1) : (maxarg2))

static int iminarg1, iminarg2;
#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ? (iminarg1) : (iminarg2))

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
#define SQR(a) (sqrarg=(a),sqrarg*sqrarg)

#define NR_END 1
#define FREE_ARG char*


/* ------- local prototypes ---------*/


static void spline(float x[],
                   float y[],
                   int n,
                   float yp1,
                   float ypn,
                   float y2[]);

static void splint(float xa[],
                   float ya[],
                   float y2a[],
                   int n,
                   float x,
                   float *y);

static float ran3(int *idum);

static void nrerror(char error_text[]);

static float *vector(long nl,
                     long nh);

static int *ivector(long nl,
                    long nh);

static float **matrix(long nrl,
                      long nrh,
                      long ncl,
                      long nch);

static void free_vector(float *v,
                        long nl,
                        long nh);

static void free_ivector(int *v,
                         long nl,
                         long nh);

static void free_matrix(float **m,
                        long nrl,
                        long nrh,
                        long ncl,
                        long nch);

static float gammq(float a,
                   float x);

static void gcf(float *gammcf,
                float a,
                float x,
                float *gln);

static void gser(float *gamser,
                 float a,
                 float x,
                 float *gln);

static void fit(float x[],
                float y[],
                int ndata,
                float sig[],
                int mwt,
                float *a,
                float *b,
                float *siga,
                float *sigb,
                float *chi2,
                float *q);

static void lfit(float x[],
                 float y[],
                 float sig[],
                 int ndat,
                 float a[],
                 int ia[],
                 int ma,
                 float **covar,
                 float *chisq,
                 void (*funcs)(float, float*, int));

static void covsrt(float **covar,
                   int ma,
                   int ia[],
                   int mfit);

static void gaussj(float **a,
                   int n,
                   float **b,
                   int m);

static void svdfit(float x[],
                   float y[],
                   float sig[],
                   int ndata,
                   float a[],
                   int ma,
                   float **u,
                   float **v,
                   float w[],
                   float *chisq,
                   void (*funcs)(float, float*, int));

static void svbksb(float **u,
                   float w[],
                   float **v,
                   int m,
                   int n,
                   float b[],
                   float x[]);

static void svdcmp(float **a,
                   int m,
                   int n,
                   float w[],
                   float **v);

static float gammln(float xx);

static void polint(float xa[],
                   float ya[],
                   int n,
                   float x,
                   float *y,
                   float *dy);

static void mrqmin(float x[],
                   float y[],
                   float sig[],
                   int ndata,
                   float a[],
                   int ia[],
                   int ma,
                   float **covar,
                   float **alpha,
                   float *chisq,
                   void (*funcs)(float x, float a[], float *y, float dyda[], int na),
                   float *alamda);

static void mrqcof(float x[],
                   float y[],
                   float sig[],
                   int ndata,
                   float a[],
                   int ia[],
                   int ma,
                   float **alpha,
                   float beta[],
                   float *chisq,
                   void (*funcs)(float x, float a[], float *y, float dyda[], int na));

static float pythag(float a,
                    float b);

static void indexx(int    n,
		   float *arr,
		   int   *indx);

/*------------- Hv_Distance ---------*/

float Hv_Distance(Hv_FPoint f1,
		  Hv_FPoint f2) {

  float dx, dy;

  dx = f2.h - f1.h;
  dy = f2.v - f1.v;
  return (float)(sqrt(dx*dx + dy*dy));
}


/* ----------- Hv_fMin ---------------*/

float Hv_fMin(float i,
	      float j)

{
  if (i < j)
    return(i);
  else
    return(j);
}

/* ----------- Hv_fMax ---------------*/

float Hv_fMax(float i,
	      float j)

{
  if (i > j)
    return(i);
  else
    return(j);
}

/* ----------- Hv_sMin ---------------*/

short  Hv_sMin(short i,
	       short j)

{
  if (i < j)
    return(i);
  else
    return(j);
}

/* ----------- Hv_sMax ---------------*/

short  Hv_sMax(short i,
	       short j)

{
  if (i > j)
    return(i);
  else
    return(j);
}

/* ----------- Hv_iMin ---------------*/

int  Hv_iMin(int i,
	     int j)

{
  if (i < j)
    return(i);
  else
    return(j);
}

/* ----------- Hv_iMax ---------------*/

int  Hv_iMax(int i,
	     int j)

{
  if (i > j)
    return(i);
  else
    return(j);
}

/* --------- Hv_nint ----------- */

int  Hv_nint(float f)

/* returns the next nearest integer */

{
  int  i;

  if (f < 0.0)
    i = (int)(f - 0.5);
  else
    i = (int)(f + 0.5);

  return(i);
}

/* -------- Hv_InRange ----------- */

#define  MAXPIXVAL  16000.0

short  Hv_InRange(float v)

/* makes sure we stay in range when converting to pixels */

{
  if (((float)fabs(v)) < MAXPIXVAL)
    return (short)(Hv_nint(v));
  
  if (v > 0.0)
    return (short)(MAXPIXVAL);
  else
    return -(short)(MAXPIXVAL);
}

#undef MAXPIXVAL

/* --------- Hv_WorldToLocal -------------- */

void Hv_WorldToLocal(Hv_View     View,
		     float     fx,
		     float     fy,
		     short     *x,
		     short     *y)

/* converts world (real) coordinates fx and fy into local
   (pixel) corrdinates x and y for the given coordinate systems */

{
  float       xx, yy, rx, ry;
  short       xoff, yoff;
  Hv_FRect    *world;
  Hv_Rect     *local;

  local = View->local;
  world = View->world;

  xoff = local->left;
  yoff = local->top;

  rx = ((float)local->width)/world->width;
  ry = ((float)local->height)/world->height;

  xx = xoff + rx*(fx - world->xmin);
  yy = yoff + ry*(world->ymax - fy);

  *x = Hv_InRange(xx);
  *y = Hv_InRange(yy);
}


/* --------- Hv_LocalToWorld -------------- */

void Hv_LocalToWorld(Hv_View     View,
		     float     *fx,
		     float     *fy,
		     short     x,
		     short     y)

/* converts local (pixel) coordinates x and y into world
   (real) coordinates fx and fy for the given coordinate systems */


{
  float       rx, ry;
  short       xoff, yoff;
  Hv_FRect    *world;
  Hv_Rect     *local;

  local = View->local;
  world = View->world;

  xoff = local->left;
  yoff = local->top;

  rx = world->width/((float)local->width);
  ry = world->height/((float)local->height);

  *fx = world->xmin + rx*(x - xoff);
  *fy = world->ymax + ry*(yoff - y);
}


#define MBIG 1000000000
#define MSEED 161803398
#define MZ 0
#define FAC (1.0/MBIG)


/*------- Hv_RandomInt -------*/

int Hv_RandomInt(int   *seed,
		 int   maxval)

/* generate a uniform pseudo-random number from 0..maxval
   with given seed and using generator specified by randgen*/

{
  float   ranval;

  ranval = ran3(seed);
  ranval *= (maxval+1);
  return ((int)ranval);
}

/*============== NUMERICAL RECIPE ROUTINES ============*/

/*-------- Hv wrappers to NR routines ----------*/

void Hv_IndexSort(int     n,
		  float  *arr,
		  int    *indx)

{
  int i;

  indexx(n, arr-1, indx-1);

/* change the indices in the index array back to C indices */

  for (i = 0; i < n; i++) 
    indx[i] -= 1;
}


/*-------- Hv_Spline -------*/

void Hv_Spline(float x[],
	       float y[],
	       int n,
	       float yp1,
	       float ypn,
	       float y2[])

/* this will call the NR routine spline (below), with
   the offset to handle the fact that NR routines expect
   1--N arrays */

/* REQUIRES X IN ASCENDING ORDER!!!!!*/


{
  spline(x-1, y-1, n, yp1, ypn, y2-1);
}


/*-------- Hv_Splint -------*/

void Hv_Splint(float xa[],
               float ya[],
               float y2a[],
               int n,
               float x,
               float *y)

/* this will call the NR routine splint (below), with
   the offset to handle the fact that NR routines expect
   1--N arrays */

/* REQUIRES X IN ASCENDING ORDER!!!!!*/

{
  splint(xa-1, ya-1, y2a-1, n, x, y);
}

/*----- Hv_Polint ----------*/

void Hv_Polint(float xa[],
               float ya[],
               int n,
               float x,
               float *y,
               float *dy)

{
  polint(xa-1, ya-1, n, x, y, dy);
}

/* ------ Hv_Fit -------*/

void Hv_Fit(float x[],
	    float y[],
	    int ndata,
	    float sig[],
	    int mwt,
	    float *a,
	    float *b,
	    float *siga,
	    float *sigb,
	    float *chi2,
	    float *q)

/* note: set mwt = 0 of sig unavailable */

{
  fit(x-1,y-1,ndata,sig-1,mwt,a,b,siga,sigb,chi2,q);
}

/* ------ Hv_LFit -------*/

void Hv_LFit(float x[],
	     float y[],
	     float sig[],
	     int ndata,
	     float a[],
	     int ma,
	     float *chisq,
	     void (*funcs)(float, float*, int))

{
  int i, *ia;
  float  **covar;

  ia = ivector(1, ma);
  covar = matrix(1, ma, 1, ma);

  for (i = 1; i <= ma; i++)
    ia[i] = 1;

  lfit(x-1, y-1, sig-1, ndata, a-1, ia, ma, covar, chisq, funcs);

  free_matrix(covar, 1, ma, 1, ma);
  free_ivector(ia, 1, ma);
}

/* ------ Hv_SVDFit -----*/

void Hv_SVDFit(float x[],
	       float y[],
	       float sig[],
	       int ndata,
	       float a[],
	       int ma,
	       float *chisq,
	       void (*funcs)(float, float*, int))

{
  float  **u, **v, *w;

  u = matrix(1, ndata, 1, ma);
  v = matrix(1, ma, 1, ma);
  w = vector(1, ma);
  svdfit(x-1,y-1,sig-1,ndata,a-1,ma,u,v,w,chisq,funcs);

  free_matrix(u, 1, ndata, 1, ma);
  free_matrix(v, 1, ma, 1, ma);
  free_vector(w, 1, ma);
}

/*------ Hv_MRQMin -------*/

void Hv_MRQMin(float x[],
	       float y[],
	       float sig[],
	       int ndata,
	       float a[],
	       int ma,
	       float *chisq,
	       void (*funcs)(float x, float a[], float *y, float dyda[], int na))

{
  int    i;
  int    *ia;
  float  **covar, **alpha;
  float  alamda = -1.0;
  float  oldchisq;
  int    nccnt = 0;

  ia = ivector(1, ma);
  for (i = 1; i <= ma; i++)
    ia[i] = 1;

  covar = matrix(1, ma, 1, ma);
  alpha = matrix(1, ma, 1, ma);

  mrqmin(x-1,y-1,sig-1,ndata,a-1,ia, ma, covar, alpha, chisq, funcs, &alamda);
  oldchisq = (float)(-1.0e30);

  for (i = 0; i < 60; i++) {
    mrqmin(x-1,y-1,sig-1,ndata,a-1,ia, ma, covar, alpha, chisq, funcs, &alamda);

    if ((*chisq < 1.0e-6) || (*chisq > 1.0e5))
      break;

    if ((fabs((*chisq) - oldchisq)/(*chisq)) < 1.0e-5) {
      nccnt++;
      
      if (nccnt > 4)
	break;
    }
    else
      nccnt = 0;
    
    oldchisq = *chisq;
  }

/*
  alamda = 0.0;
  mrqmin(x-1,Ay-1,sig-1,ndata,a-1,ia, ma, covar, alpha, chisq, funcs, &alamda);
*/
  free_ivector(ia, 1, ma);
  free_matrix(covar, 1, ma, 1, ma);
  free_matrix(alpha, 1, ma, 1, ma);


}

/*************************************************************************
 All routines from here to EOF:
   cf "Numerical Recipes in C" 2nd ed.,  by Press, Teukolsky, Vettering,
   and Flannery, Cambridge, 1992

 (C) Copr. 1986-95 Numerical Recipes Software. 

**************************************************************************/

/*-------- NR: spline -------*/

static void spline(float x[],
                   float y[],
                   int n,
                   float yp1,
                   float ypn,
                   float y2[])


{
	int i,k;
	float p,qn,sig,un,*u;

	u=vector(1,n-1);
	if (yp1 > 0.99e30)
		y2[1]=u[1]=0.0;
	else {
		y2[1] = -0.5;
		u[1]=(float)((3.0/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1));
	}
	for (i=2;i<=n-1;i++) {
		sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
		p=(float)(sig*y2[i-1]+2.0);
		y2[i]=(float)((sig-1.0)/p);
		u[i]=(y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
		u[i]=(float)((6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p);
	}
	if (ypn > 0.99e30)
		qn=un=0.0;
	else {
		qn=0.5;
		un=(float)((3.0/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1])));
	}
	y2[n]=(float)((un-qn*u[n-1])/(qn*y2[n-1]+1.0));
	for (k=n-1;k>=1;k--)
		y2[k]=y2[k]*y2[k+1]+u[k];
	free_vector(u,1,n-1);
}

/*------- NR: splint --------*/

static void splint(float xa[],
                   float ya[],
                   float y2a[],
                   int n,
                   float x,
                   float *y)

{
	int klo,khi,k;
	float h,b,a;

	klo=1;
	khi=n;
	while (khi-klo > 1) {
		k=(khi+klo) >> 1;
		if (xa[k] > x) khi=k;
		else klo=k;
	}
	h=xa[khi]-xa[klo];
	if (h == 0.0) {
	  nrerror("Bad XA input to routine SPLINT");
	  return;
	}

	a=(xa[khi]-x)/h;
	b=(x-xa[klo])/h;
	*y=(float)(a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0);
}

/*----- NR: ran3 --------*/

static float ran3(int *idum)

/* Numerical recipe routine for a pseudo random number [0..1.0]
   cf "Numerical Recipes in C" 2nd ed.,  by Press, Teukolsky, Vettering,
   and Flannery, Cambridge, 1992*/

{
	static int inext,inextp;
	static long ma[56];
	static int iff=0;
	long mj,mk;
	int i,ii,k;

	if (*idum < 0 || iff == 0) {
		iff=1;
		mj=MSEED-(*idum < 0 ? -*idum : *idum);
		mj %= MBIG;
		ma[55]=mj;
		mk=1;
		for (i=1;i<=54;i++) {
			ii=(21*i) % 55;
			ma[ii]=mk;
			mk=mj-mk;
			if (mk < MZ) mk += MBIG;
			mj=ma[ii];
		}
		for (k=1;k<=4;k++)
			for (i=1;i<=55;i++) {
				ma[i] -= ma[1+(i+30) % 55];
				if (ma[i] < MZ) ma[i] += MBIG;
			}
		inext=0;
		inextp=31;
		*idum=1;
	}
	if (++inext == 56) inext=1;
	if (++inextp == 56) inextp=1;
	mj=ma[inext]-ma[inextp];
	if (mj < MZ) mj += MBIG;
	ma[inext]=mj;
	return (float)(mj*FAC);
}

/*----- NR: nrerror --------*/

static void nrerror(char error_text[])

{

/* only print one warning */

  if (Hv_nrerror)
    return;

  Hv_SysBeep();
  Hv_Println("Numerical Recipes error: %s", error_text);
  Hv_nrerror = True;

}


/*----- NR: vector --------*/

static float *vector(long nl,
                     long nh)


/* allocate a float vector with subscript range v[nl..nh] */
{
	float *v;

	v=(float *)malloc((unsigned int) ((nh-nl+1+NR_END)*sizeof(float)));
	if (!v) nrerror("allocation failure in vector()");
	return v-nl+NR_END;
}

/*----- NR: ivector --------*/

static int *ivector(long nl,
                    long nh)

/* allocate an int vector with subscript range v[nl..nh] */
{
	int *v;

	v=(int *)malloc((unsigned int) ((nh-nl+1+NR_END)*sizeof(int)));
	if (!v) nrerror("allocation failure in ivector()");
	return v-nl+NR_END;
}


/*----- NR: matrix --------*/

static float **matrix(long nrl,
                      long nrh,
                      long ncl,
                      long nch)


/* allocate a float matrix with subscript range m[nrl..nrh][ncl..nch] */
{
	long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
	float **m;

	/* allocate pointers to rows */
	m=(float **) malloc((unsigned int)((nrow+NR_END)*sizeof(float*)));
	if (!m) nrerror("allocation failure 1 in matrix()");
	m += NR_END;
	m -= nrl;

	/* allocate rows and set pointers to them */
	m[nrl]=(float *) malloc((unsigned int)((nrow*ncol+NR_END)*sizeof(float)));
	if (!m[nrl]) nrerror("allocation failure 2 in matrix()");
	m[nrl] += NR_END;
	m[nrl] -= ncl;

	for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

	/* return pointer to array of pointers to rows */
	return m;
}



/*----- NR: free_vector --------*/

static void free_vector(float *v,
                        long nl,
                        long nh)

/* free a float vector allocated with vector() */
{
	free((FREE_ARG) (v+nl-NR_END));
}

/*----- NR: free_ivector --------*/

static void free_ivector(int *v,
                         long nl,
                         long nh)

/* free an int vector allocated with ivector() */
{
	free((FREE_ARG) (v+nl-NR_END));
}


/*----- NR: free_matrix --------*/

static void free_matrix(float **m,
                        long nrl,
                        long nrh,
                        long ncl,
                        long nch)

/* free a float matrix allocated by matrix() */
{
	free((FREE_ARG) (m[nrl]+ncl-NR_END));
	free((FREE_ARG) (m+nrl-NR_END));
}



/*----- NR: gammaq --------*/

static float gammq(float a,
                   float x)

{
	float gamser,gammcf,gln;

	if (x < 0.0 || a <= 0.0) nrerror("Invalid arguments in routine gammq");
	if (x < (a+1.0)) {
		gser(&gamser,a,x,&gln);
		return (float)(1.0-gamser);
	} else {
		gcf(&gammcf,a,x,&gln);
		return gammcf;
	}
}


#define ITMAX 100
#define EPS 3.0e-7
#define FPMIN 1.0e-30

/*----- NR: gcf -------- */

static void gcf(float *gammcf,
                float a,
                float x,
                float *gln)

{
	int i;
	float an,b,c,d,del,h;

	*gln=gammln(a);
	b=(float)(x+1.0-a);
	c=(float)(1.0/FPMIN);
	d=(float)(1.0/b);
	h=d;
	for (i=1;i<=ITMAX;i++) {
		an = -i*(i-a);
		b += 2.0;
		d=an*d+b;
		if (fabs(d) < FPMIN)
			d=(float)FPMIN;
		c=b+an/c;
		if (fabs(c) < FPMIN)
			c=(float)FPMIN;
		d=(float)(1.0/d);
		del=d*c;
		h *= del;
		if (fabs(del-1.0) < EPS) break;
	}
	if (i > ITMAX) nrerror("a too large, ITMAX too small in gcf");
	*gammcf=(float)(exp(-x+a*log(x)-(*gln))*h);
}

/*------ NR: gser --------*/

static void gser(float *gamser,
                 float a,
                 float x,
                 float *gln)

{
	int n;
	float sum,del,ap;

	*gln=gammln(a);
	if (x <= 0.0) {
		if (x < 0.0) nrerror("x less than 0 in routine gser");
		*gamser=0.0;
		return;
	} else {
		ap=a;
		del=sum=(float)(1.0/a);
		for (n=1;n<=ITMAX;n++) {
			++ap;
			del *= x/ap;
			sum += del;
			if (fabs(del) < fabs(sum)*EPS) {
				*gamser=(float)(sum*exp(-x+a*log(x)-(*gln)));
				return;
			}
		}
		nrerror("a too large, ITMAX too small in routine gser");
		return;
	}
}

/*----- NR: fit -------*/

static void fit(float x[],
                float y[],
                int ndata,
                float sig[],
                int mwt,
                float *a,
                float *b,
                float *siga,
                float *sigb,
                float *chi2,
                float *q)

{
	int i;
	float wt,t,sxoss,sx=0.0,sy=0.0,st2=0.0,ss,sigdat;

	*b=0.0;
	if (mwt) {
		ss=0.0;
		for (i=1;i<=ndata;i++) {
			wt=(float)(1.0/SQR(sig[i]));
			ss += wt;
			sx += x[i]*wt;
			sy += y[i]*wt;
		}
	} else {
		for (i=1;i<=ndata;i++) {
			sx += x[i];
			sy += y[i];
		}
		ss=(float)(ndata);
	}
	sxoss=sx/ss;
	if (mwt) {
		for (i=1;i<=ndata;i++) {
			t=(x[i]-sxoss)/sig[i];
			st2 += t*t;
			*b += t*y[i]/sig[i];
		}
	} else {
		for (i=1;i<=ndata;i++) {
			t=x[i]-sxoss;
			st2 += t*t;
			*b += t*y[i];
		}
	}
	*b /= st2;
	*a=(sy-sx*(*b))/ss;
	*siga=(float)sqrt((1.0+sx*sx/(ss*st2))/ss);
	*sigb=(float)sqrt(1.0/st2);
	*chi2=0.0;
	if (mwt == 0) {
		for (i=1;i<=ndata;i++)
			*chi2 += SQR(y[i]-(*a)-(*b)*x[i]);
		*q=1.0;
		sigdat=(float)sqrt((*chi2)/(ndata-2));
		*siga *= sigdat;
		*sigb *= sigdat;
	} else {
		for (i=1;i<=ndata;i++)
			*chi2 += SQR((y[i]-(*a)-(*b)*x[i])/sig[i]);
		*q=gammq((float)(0.5*(ndata-2)),(float)(0.5*(*chi2)));
	}
}


/*-------NR: lfit --------*/

static void lfit(float x[],
                 float y[],
                 float sig[],
                 int ndat,
                 float a[],
                 int ia[],
                 int ma,
                 float **covar,
                 float *chisq,
                 void (*funcs)(float, float*, int))

{
	int i,j,k,l,m,mfit=0;
	float ym,wt,sum,sig2i,**beta,*afunc;

	beta=matrix(1,ma,1,1);
	afunc=vector(1,ma);
	for (j=1;j<=ma;j++)
		if (ia[j]) mfit++;
	if (mfit == 0) nrerror("lfit: no parameters to be fitted");
	for (j=1;j<=mfit;j++) {
		for (k=1;k<=mfit;k++) covar[j][k]=0.0;
		beta[j][1]=0.0;
	}
	for (i=1;i<=ndat;i++) {
		(*funcs)(x[i],afunc,ma);
		ym=y[i];
		if (mfit < ma) {
			for (j=1;j<=ma;j++)
				if (!ia[j]) ym -= a[j]*afunc[j];
		}
		sig2i=(float)(1.0/SQR(sig[i]));
		for (j=0,l=1;l<=ma;l++) {
			if (ia[l]) {
				wt=afunc[l]*sig2i;
				for (j++,k=0,m=1;m<=l;m++)
					if (ia[m]) covar[j][++k] += wt*afunc[m];
				beta[j][1] += ym*wt;
			}
		}
	}
	for (j=2;j<=mfit;j++)
		for (k=1;k<j;k++)
			covar[k][j]=covar[j][k];
	gaussj(covar,mfit,beta,1);
	for (j=0,l=1;l<=ma;l++)
		if (ia[l]) a[l]=beta[++j][1];
	*chisq=0.0;
	for (i=1;i<=ndat;i++) {
		(*funcs)(x[i],afunc,ma);
		for (sum=0.0,j=1;j<=ma;j++) sum += a[j]*afunc[j];
		*chisq += SQR((y[i]-sum)/sig[i]);
	}
	covsrt(covar,ma,ia,mfit);
	free_vector(afunc,1,ma);
	free_matrix(beta,1,ma,1,1);
}

#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}

/*------- NR: covsrt --------*/

static void covsrt(float **covar,
                   int ma,
                   int ia[],
                   int mfit)

{
	int i,j,k;
	float swap;

	for (i=mfit+1;i<=ma;i++)
		for (j=1;j<=i;j++) covar[i][j]=covar[j][i]=0.0;
	k=mfit;
	for (j=ma;j>=1;j--) {
		if (ia[j]) {
			for (i=1;i<=ma;i++) SWAP(covar[i][k],covar[i][j])
			for (i=1;i<=ma;i++) SWAP(covar[k][i],covar[j][i])
			k--;
		}
	}
}
#undef SWAP

/*------- NR: gaussj --------*/

#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}

static void gaussj(float **a,
                   int n,
                   float **b,
                   int m)

{
	int *indxc,*indxr,*ipiv;
	int i,icol,irow,j,k,l,ll;
	float big,dum,pivinv,temp;

	indxc=ivector(1,n);
	indxr=ivector(1,n);
	ipiv=ivector(1,n);
	for (j=1;j<=n;j++) ipiv[j]=0;
	for (i=1;i<=n;i++) {
		big=0.0;
		for (j=1;j<=n;j++)
			if (ipiv[j] != 1)
				for (k=1;k<=n;k++) {
					if (ipiv[k] == 0) {
						if (fabs(a[j][k]) >= big) {
							big=(float)fabs(a[j][k]);
							irow=j;
							icol=k;
						}
					} else if (ipiv[k] > 1) nrerror("gaussj: Singular Matrix-1");
				}
		++(ipiv[icol]);
		if (irow != icol) {
			for (l=1;l<=n;l++) SWAP(a[irow][l],a[icol][l])
			for (l=1;l<=m;l++) SWAP(b[irow][l],b[icol][l])
		}
		indxr[i]=irow;
		indxc[i]=icol;
		if (a[icol][icol] == 0.0) nrerror("gaussj: Singular Matrix-2");
		pivinv=(float)(1.0/a[icol][icol]);
		a[icol][icol]=1.0;
		for (l=1;l<=n;l++) a[icol][l] *= pivinv;
		for (l=1;l<=m;l++) b[icol][l] *= pivinv;
		for (ll=1;ll<=n;ll++)
			if (ll != icol) {
				dum=a[ll][icol];
				a[ll][icol]=0.0;
				for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
				for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
			}
	}
	for (l=n;l>=1;l--) {
		if (indxr[l] != indxc[l])
			for (k=1;k<=n;k++)
				SWAP(a[k][indxr[l]],a[k][indxc[l]]);
	}
	free_ivector(ipiv,1,n);
	free_ivector(indxr,1,n);
	free_ivector(indxc,1,n);
}


#define TOL 1.0e-5
#undef SWAP

/*------ NR: svdfit ---------*/

static void svdfit(float x[],
                   float y[],
                   float sig[],
                   int ndata,
                   float a[],
                   int ma,
                   float **u,
                   float **v,
                   float w[],
                   float *chisq,
                   void (*funcs)(float, float*, int))

{
	int j,i;
	float wmax,tmp,thresh,sum,*b,*afunc;

	b=vector(1,ndata);
	afunc=vector(1,ma);
	for (i=1;i<=ndata;i++) {
		(*funcs)(x[i],afunc,ma);
		tmp=(float)(1.0/sig[i]);
		for (j=1;j<=ma;j++) u[i][j]=afunc[j]*tmp;
		b[i]=y[i]*tmp;
	}
	svdcmp(u,ndata,ma,w,v);
	wmax=0.0;
	for (j=1;j<=ma;j++)
		if (w[j] > wmax) wmax=w[j];
	thresh=(float)(TOL*wmax);
	for (j=1;j<=ma;j++)
		if (w[j] < thresh) w[j]=0.0;
	svbksb(u,w,v,ndata,ma,b,a);
	*chisq=0.0;
	for (i=1;i<=ndata;i++) {
		(*funcs)(x[i],afunc,ma);
		for (sum=0.0,j=1;j<=ma;j++) sum += a[j]*afunc[j];
		*chisq += (tmp=(y[i]-sum)/sig[i],tmp*tmp);
	}
	free_vector(afunc,1,ma);
	free_vector(b,1,ndata);
}

/*------- NR: svbksb ------*/

static void svbksb(float **u,
                   float w[],
                   float **v,
                   int m,
                   int n,
                   float b[],
                   float x[])
{
	int jj,j,i;
	float s,*tmp;

	tmp=vector(1,n);
	for (j=1;j<=n;j++) {
		s=0.0;
		if (w[j]) {
			for (i=1;i<=m;i++) s += u[i][j]*b[i];
			s /= w[j];
		}
		tmp[j]=s;
	}
	for (j=1;j<=n;j++) {
		s=0.0;
		for (jj=1;jj<=n;jj++) s += v[j][jj]*tmp[jj];
		x[j]=s;
	}
	free_vector(tmp,1,n);
}


/*--------- NR: svdcmp --------*/

static void svdcmp(float **a,
                   int m,
                   int n,
                   float w[],
                   float **v)

{
	int flag,i,its,j,jj,k,l,nm;
	float anorm,c,f,g,h,s,scale,x,y,z,*rv1;

	rv1=vector(1,n);
	g=scale=anorm=0.0;
	for (i=1;i<=n;i++) {
		l=i+1;
		rv1[i]=scale*g;
		g=s=scale=0.0;
		if (i <= m) {
			for (k=i;k<=m;k++) scale += (float)fabs(a[k][i]);
			if (scale) {
				for (k=i;k<=m;k++) {
					a[k][i] /= scale;
					s += a[k][i]*a[k][i];
				}
				f=a[i][i];
				g = (float)(-SIGN(sqrt(s),f));
				h=f*g-s;
				a[i][i]=f-g;
				for (j=l;j<=n;j++) {
					for (s=0.0,k=i;k<=m;k++) s += a[k][i]*a[k][j];
					f=s/h;
					for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
				}
				for (k=i;k<=m;k++) a[k][i] *= scale;
			}
		}
		w[i]=scale *g;
		g=s=scale=0.0;
		if (i <= m && i != n) {
			for (k=l;k<=n;k++) scale += (float)fabs(a[i][k]);
			if (scale) {
				for (k=l;k<=n;k++) {
					a[i][k] /= scale;
					s += a[i][k]*a[i][k];
				}
				f=a[i][l];
				g = (float)(-SIGN(sqrt(s),f));
				h=f*g-s;
				a[i][l]=f-g;
				for (k=l;k<=n;k++) rv1[k]=a[i][k]/h;
				for (j=l;j<=m;j++) {
					for (s=0.0,k=l;k<=n;k++) s += a[j][k]*a[i][k];
					for (k=l;k<=n;k++) a[j][k] += s*rv1[k];
				}
				for (k=l;k<=n;k++) a[i][k] *= scale;
			}
		}
		anorm=(float)(FMAX(anorm,(float)(fabs(w[i])+fabs(rv1[i]))));
	}
	for (i=n;i>=1;i--) {
		if (i < n) {
			if (g) {
				for (j=l;j<=n;j++)
					v[j][i]=(a[i][j]/a[i][l])/g;
				for (j=l;j<=n;j++) {
					for (s=0.0,k=l;k<=n;k++) s += a[i][k]*v[k][j];
					for (k=l;k<=n;k++) v[k][j] += s*v[k][i];
				}
			}
			for (j=l;j<=n;j++) v[i][j]=v[j][i]=0.0;
		}
		v[i][i]=1.0;
		g=rv1[i];
		l=i;
	}
	for (i=IMIN(m,n);i>=1;i--) {
		l=i+1;
		g=w[i];
		for (j=l;j<=n;j++) a[i][j]=0.0;
		if (g) {
			g=(float)(1.0/g);
			for (j=l;j<=n;j++) {
				for (s=0.0,k=l;k<=m;k++) s += a[k][i]*a[k][j];
				f=(s/a[i][i])*g;
				for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
			}
			for (j=i;j<=m;j++) a[j][i] *= g;
		} else for (j=i;j<=m;j++) a[j][i]=0.0;
		++a[i][i];
	}
	for (k=n;k>=1;k--) {
		for (its=1;its<=30;its++) {
			flag=1;
			for (l=k;l>=1;l--) {
				nm=l-1;
				if ((float)(fabs(rv1[l])+anorm) == anorm) {
					flag=0;
					break;
				}
				if ((float)(fabs(w[nm])+anorm) == anorm) break;
			}
			if (flag) {
				c=0.0;
				s=1.0;
				for (i=l;i<=k;i++) {
					f=s*rv1[i];
					rv1[i]=c*rv1[i];
					if ((float)(fabs(f)+anorm) == anorm) break;
					g=w[i];
					h=pythag(f,g);
					w[i]=h;
					h=(float)(1.0/h);
					c=g*h;
					s = -f*h;
					for (j=1;j<=m;j++) {
						y=a[j][nm];
						z=a[j][i];
						a[j][nm]=y*c+z*s;
						a[j][i]=z*c-y*s;
					}
				}
			}
			z=w[k];
			if (l == k) {
				if (z < 0.0) {
					w[k] = -z;
					for (j=1;j<=n;j++) v[j][k] = -v[j][k];
				}
				break;
			}
			if (its == 30) nrerror("no convergence in 30 svdcmp iterations");
			x=w[l];
			nm=k-1;
			y=w[nm];
			g=rv1[nm];
			h=rv1[k];
			f=(float)(((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y));
			g=pythag(f,1.0);
			f=(float)(((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x);
			c=s=1.0;
			for (j=l;j<=nm;j++) {
				i=j+1;
				g=rv1[i];
				y=w[i];
				h=s*g;
				g=c*g;
				z=pythag(f,h);
				rv1[j]=z;
				c=f/z;
				s=h/z;
				f=x*c+g*s;
				g = g*c-x*s;
				h=y*s;
				y *= c;
				for (jj=1;jj<=n;jj++) {
					x=v[jj][j];
					z=v[jj][i];
					v[jj][j]=x*c+z*s;
					v[jj][i]=z*c-x*s;
				}
				z=pythag(f,h);
				w[j]=z;
				if (z) {
					z=(float)(1.0/z);
					c=f*z;
					s=h*z;
				}
				f=c*g+s*y;
				x=c*y-s*g;
				for (jj=1;jj<=m;jj++) {
					y=a[jj][j];
					z=a[jj][i];
					a[jj][j]=y*c+z*s;
					a[jj][i]=z*c-y*s;
				}
			}
			rv1[l]=0.0;
			rv1[k]=f;
			w[k]=x;
		}
	}
	free_vector(rv1,1,n);
}

/*------- NR: gammln -----*/

static float gammln(float xx)
{
	double x,y,tmp,ser;
	static double cof[6]={76.18009172947146,-86.50532032941677,
		24.01409824083091,-1.231739572450155,
		0.1208650973866179e-2,-0.5395239384953e-5};
	int j;

	y=x=xx;
	tmp=x+5.5;
	tmp -= (x+0.5)*log(tmp);
	ser=1.000000000190015;
	for (j=0;j<=5;j++) ser += cof[j]/++y;
	return (float)(-tmp+log(2.5066282746310005*ser/x));
}


/*------ NR: polint -----*/

static void polint(float xa[],
                   float ya[],
                   int n,
                   float x,
                   float *y,
                   float *dy)

{
	int i,m,ns=1;
	float den,dif,dift,ho,hp,w;
	float *c,*d;

	dif=(float)fabs(x-xa[1]);
	c=vector(1,n);
	d=vector(1,n);
	for (i=1;i<=n;i++) {
		if ( (dift=(float)fabs(x-xa[i])) < dif) {
			ns=i;
			dif=dift;
		}
		c[i]=ya[i];
		d[i]=ya[i];
	}
	*y=ya[ns--];
	for (m=1;m<n;m++) {
		for (i=1;i<=n-m;i++) {
			ho=xa[i]-x;
			hp=xa[i+m]-x;
			w=c[i+1]-d[i];
			if ( (den=ho-hp) == 0.0) nrerror("Error in routine polint");
			den=w/den;
			d[i]=hp*den;
			c[i]=ho*den;
		}
		*y += (*dy=(2*ns < (n-m) ? c[ns+1] : d[ns--]));
	}
	free_vector(d,1,n);
	free_vector(c,1,n);
}

/*------- NR: mrqmin -------*/

static void mrqmin(float x[],
                   float y[],
                   float sig[],
                   int ndata,
                   float a[],
                   int ia[],
                   int ma,
                   float **covar,
                   float **alpha,
                   float *chisq,
                   void (*funcs)(float x, float a[], float *y, float dyda[], int na),
                   float *alamda)

{
	int j,k,l;
	static int mfit;
	static float ochisq,*atry,*beta,*da,**oneda;

	if (*alamda < 0.0) {
		atry=vector(1,ma);
		beta=vector(1,ma);
		da=vector(1,ma);
		for (mfit=0,j=1;j<=ma;j++)
			if (ia[j]) mfit++;
		oneda=matrix(1,mfit,1,1);
		*alamda=(float)(0.001);
		mrqcof(x,y,sig,ndata,a,ia,ma,alpha,beta,chisq,funcs);
		ochisq=(*chisq);
		for (j=1;j<=ma;j++) atry[j]=a[j];
	}
	for (j=1;j<=mfit;j++) {
		for (k=1;k<=mfit;k++) covar[j][k]=alpha[j][k];
		covar[j][j]=(float)(alpha[j][j]*(1.0+(*alamda)));
		oneda[j][1]=beta[j];
	}
	gaussj(covar,mfit,oneda,1);
	for (j=1;j<=mfit;j++) da[j]=oneda[j][1];
	if (*alamda == 0.0) {
		covsrt(covar,ma,ia,mfit);
		free_matrix(oneda,1,mfit,1,1);
		free_vector(da,1,ma);
		free_vector(beta,1,ma);
		free_vector(atry,1,ma);
		return;
	}
	for (j=0,l=1;l<=ma;l++)
		if (ia[l]) atry[l]=a[l]+da[++j];
	mrqcof(x,y,sig,ndata,atry,ia,ma,covar,da,chisq,funcs);
	if (*chisq < ochisq) {
		*alamda *= (float)(0.1);
		ochisq=(*chisq);
		for (j=1;j<=mfit;j++) {
			for (k=1;k<=mfit;k++) alpha[j][k]=covar[j][k];
			beta[j]=da[j];
		}
		for (l=1;l<=ma;l++) a[l]=atry[l];
	} else {
		*alamda *= 10.0;
		*chisq=ochisq;
	}
}

/*------ NR: mrqcof ------*/

static void mrqcof(float x[],
                   float y[],
                   float sig[],
                   int ndata,
                   float a[],
                   int ia[],
                   int ma,
                   float **alpha,
                   float beta[],
                   float *chisq,
                   void (*funcs)(float x, float a[], float *y, float dyda[], int na))
{
	int i,j,k,l,m,mfit=0;
	float ymod,wt,sig2i,dy,*dyda;

	dyda=vector(1,ma);
	for (j=1;j<=ma;j++)
		if (ia[j]) mfit++;
	for (j=1;j<=mfit;j++) {
		for (k=1;k<=j;k++) alpha[j][k]=0.0;
		beta[j]=0.0;
	}
	*chisq=0.0;
	for (i=1;i<=ndata;i++) {
		(*funcs)(x[i],a,&ymod,dyda,ma);
		sig2i=(float)(1.0/(sig[i]*sig[i]));
		dy=y[i]-ymod;
		for (j=0,l=1;l<=ma;l++) {
			if (ia[l]) {
				wt=dyda[l]*sig2i;
				for (j++,k=0,m=1;m<=l;m++)
					if (ia[m]) alpha[j][++k] += wt*dyda[m];
				beta[j] += dy*wt;
			}
		}
		*chisq += dy*dy*sig2i;
	}
	for (j=2;j<=mfit;j++)
		for (k=1;k<j;k++) alpha[k][j]=alpha[j][k];
	free_vector(dyda,1,ma);
}

/*----- pythag ------*/

static float pythag(float a,
		    float b)
{
	float absa,absb;
	absa=(float)fabs(a);
	absb=(float)fabs(b);
	if (absa > absb) return (float)(absa*sqrt(1.0+SQR(absb/absa)));
	else return (float)((absb == 0.0 ? 0.0 : absb*sqrt(1.0+SQR(absa/absb))));
}

/*------ indexx --------*/

#define SWAP(a,b) itemp=(a);(a)=(b);(b)=itemp;
#define M 7
#define NSTACK 50

static void indexx(int    n,
		   float *arr,
		   int   *indx)
{
  int i,indxt,ir=n,itemp,j,k,l=1;
  int jstack=0,*istack;
  float a;
  
  istack=ivector(1,NSTACK);
  for (j=1;j<=n;j++) indx[j]=j;
  for (;;) {
    if (ir-l < M) {
      for (j=l+1;j<=ir;j++) {
	indxt=indx[j];
	a=arr[indxt];
	for (i=j-1;i>=1;i--) {
	  if (arr[indx[i]] <= a) break;
	  indx[i+1]=indx[i];
	}
	indx[i+1]=indxt;
      }
      if (jstack == 0) break;
      ir=istack[jstack--];
      l=istack[jstack--];
    } else {
      k=(l+ir) >> 1;
      SWAP(indx[k],indx[l+1]);
      if (arr[indx[l+1]] > arr[indx[ir]]) {
	SWAP(indx[l+1],indx[ir])
      }
      if (arr[indx[l]] > arr[indx[ir]]) {
	SWAP(indx[l],indx[ir])
      }
      if (arr[indx[l+1]] > arr[indx[l]]) {
	SWAP(indx[l+1],indx[l])
      }
      i=l+1;
      j=ir;
      indxt=indx[l];
      a=arr[indxt];
      for (;;) {
	do i++; while (arr[indx[i]] < a);
	do j--; while (arr[indx[j]] > a);
	if (j < i) break;
	SWAP(indx[i],indx[j])
      }
      indx[l]=indx[j];
      indx[j]=indxt;
      jstack += 2;
      if (jstack > NSTACK) nrerror("NSTACK too small in indexx.");
      if (ir-i+1 >= j-l) {
	istack[jstack]=ir;
	istack[jstack-1]=i;
	ir=j-1;
      } else {
	istack[jstack]=j-1;
	istack[jstack-1]=l;
	l=i;
      }
    }
  }
  free_ivector(istack,1,NSTACK);
}
#undef M
#undef NSTACK
#undef SWAP

#undef SIGN
#undef FMAX
#undef IMIN
#undef TOL
#undef SQR
#undef ITMAX
#undef EPS
#undef MBIG
#undef MSEED
#undef MZ
#undef FAC
#undef NR_END
#undef FREE_ARG
#undef FPMIN
