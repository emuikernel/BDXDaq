/*
   prlib.c - CLAS detector pattern recognition

   Author:      Sergey Boyarinov

   Created:     May  1, 1998
   Last update: Sep 17, 2000

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "prlib.h"
#include "sglib.h"

#include "uttime.h"
#include "dclib.h"   /* just to get min and max wire number in GETDCDATA */
#include "sdageom.h" /* just to get min and max wire number in GETDCDATA */


#undef DEBUG


#define RADDEG 57.2957795130823209

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))


static float vk[7] = {VECT1, VECT2, VECT3, VECT4, VECT5, VECT6, VECT7};

/************************************************/
/********** FILE STRUCTURE PROCEDURES ***********/
/************************************************/

#define NFMT 15

#define OPENFILE(filnam) \
    strcpy(str,"OPEN INPUT UNIT=57 FILE=\""); \
    strcat(str,filnam); \
    strcat(str,"\""); \
    fparm_(str,strlen(str)); \
    frname_("INPUT",5); \
    for(i=0; i<strlen(kname); i++) kname[i] = ' '; \
    frkey_(kname,&numra,&numrb,&icl,&ierr,strlen(kname))

#define READFILE \
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),NFMT); \
    frdat_(&max_lnk_local,link1,&ncol); \
    printf(" %s %8d %6d %6d %6d    %s\n",hname,numdb,ncol,nrow,max_lnk_local,format); fflush(stdout); \
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),NFMT); \
    frdat_(&max_lnk_local,link2,&ncol); \
    printf(" %s %8d %6d %6d %6d    %s\n",hname,numdb,ncol,nrow,max_lnk_local,format); fflush(stdout); \
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),NFMT); \
    frdat_(&max_lnk_local,buf,&ncol); \
    printf(" %s %8d %6d %6d %6d    %s\n",hname,numdb,ncol,nrow,max_lnk_local,format); fflush(stdout); \
    for(j=0; j<max_lnk_local; j++) \
      for(l=0; l<2; l++) \
        nlink3[l][j] = buf2[j*2+l]; \
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),NFMT); \
    tmp = max_lnk*6*(MAXNPAT+1)/4; \
    frdat_(&tmp,buf3,&ncol); \
    printf(" %s %8d %6d %6d %6d    %s\n",hname,numdb,ncol,nrow,tmp,format); fflush(stdout); \
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),NFMT); \
    /*printf("1: >%s< >%s< ncol=%d nrow=%d\n",hname,format,ncol,nrow); fflush(stdout);*/ \
    nwd = ncol * nrow; \
    frdat_(&tmp,link4,&nwd); \
    printf(" %s %8d %6d %6d %6d    %s\n",hname,numdb,ncol,nrow,tmp,format); fflush(stdout)

#define CLOSEFILE \
    fparm_("CLOSE UNIT=57",13); \
    printf("dictionary file closed.\n"); fflush(stdout)



/************************************************/
/*********** ROAD FINDING PROCEDURES ************/
/************************************************/

#define MOM(imom) P0*(float)pow((double)(1.+DP),(double)(imom))
#define IMOM(mom) (int)(log((double)(((float)(mom))/P0))/log((double)(1.+DP))+0.1)

float
getmom_(int *imom)
{
  return(MOM(*imom));
}

int
getimom_(float *mom)
{
  return(IMOM(*mom));
}


static DCstereo *alist[NLISTAX]; /* pointers to the list of stereo partners */
static DCaxial  *slist[NLISTST]; /* pointers to the list of axial partners */
static int max[3] = {-100,-100,-100}, min[3] = {100,100,100};





/* <<<<<compress_uncompress>>>>> 

             A X I A L      S U P E R L A Y E R S
                t[2]                t[1]            t[0]
               SL5-SL3             SL3-SL2           SL2
________________________________________________________________
         |                 |                 |                 |
         | x x x x x x x x | 0 0 x x x x x x | x x x x x x x x |
_________|_________________|_________________|_________________|

            S T E R E O      S U P E R L A Y E R S
                t[2]                t[1]            t[0]
               SL6-SL4             SL4-SL1           SL1
________________________________________________________________
         |                 |                 |                 |
         | x x x x x x x x | 0 x x x x x x x | 0 x x x x x x x |
_________|_________________|_________________|_________________|

*/


#define T0AX 0
#define T1AX 4
#define T2AX 65
#define T0ST 0
#define T1ST 4
#define T2ST 75

static int
GETAXADR(int tmp)
{
  unsigned char *t;
  unsigned int t0, t1, t2;

  t = (unsigned char *) &tmp;
  t0 = (t[0]-1);
  t1 = (t[1]-t[0]+T1AX-1);
  t2 = (t[2]-t[1]+T2AX-1);

  /*
if(t0 < 0 || t0 > 255) return(0);
if(t1 < 0 || t1 > 63)  return(0);
if(t2 < 0 || t2 > 255) return(0);
  */

  /*
  if(t0 < 0 || t0 > 255)
  {
    printf("prlib.c: GETAXADR ERROR: t0 out of range: t0 = %d\n",t0); fflush(stdout);
  }
  if(t1 < 0 || t1 > 63)
  {
    printf("prlib.c: GETAXADR ERROR: t1 out of range: t1 = %d\n",t1);
    printf("-------> t[1]=%u   t[0]=%u\n",t[1],t[0]); fflush(stdout);
  }
  if(t2 < 0 || t2 > 255)
  {
    printf("prlib.c: GETAXADR ERROR: t2 out of range: t2 = %d\n",t2);
    printf("-------> t[2]=%u   t[1]=%u\n",t[2],t[1]); fflush(stdout);
  }
  */

  return( (t0 + (t1 << 8) + (t2 << 14)) & 0x3fffff );
}

#define GETAXADR1(byte0, byte1, byte2) \
  ( ((byte0-1) + ((byte1-byte0+T1AX-1) << 8) + ((byte2-byte1+T2AX-1) << 14)) & 0x3fffff )

static int
GETAXPATH(int adr)
{
  unsigned char t0, t1, t2;

  t0 = (unsigned char)( (adr     &0xff)+1);
  t1 = (unsigned char)(((adr>>8) &0x3f)+1-T1AX+t0);
  t2 = (unsigned char)(((adr>>14)&0xff)+1-T2AX+t1);
#ifdef Linux
  return( t0+(t1<<8)+(t2<<16) );
#else
  return( (t0<<24)+(t1<<16)+(t2<<8) );
#endif
}




static int
GETSTADR(int tmp)
{
  unsigned char *t;
  int t0, t1, t2;

  t = (unsigned char *) &tmp;
  t0 = (t[0]     +T0ST-1);
  t1 = (t[1]-t[0]+T1ST-1);
  t2 = (t[2]-t[1]+T2ST-1);

  /*
if(t0 < 0 || t0 > 127) return(0);
if(t1 < 0 || t1 > 127) return(0);
if(t2 < 0 || t2 > 255) return(0);
  */

  /*
  if(t0 < 0 || t0 > 127) printf("prlib.c: GETSTADR ERROR: t0 out of range: t0 = %d\n",t0); fflush(stdout);
  if(t1 < 0 || t1 > 127)
  {
    printf("prlib.c: GETSTADR ERROR: t1 out of range: t1 = %d\n",t1);
    printf("-------> t[1]=%u   t[0]=%u\n",t[1],t[0]); fflush(stdout);
  }
  if(t2 < 0 || t2 > 255)
  {
    printf("prlib.c: GETSTADR ERROR: t2 out of range: t2 = %d\n",t2);
    printf("-------> t[2]=%u   t[1]=%u\n",t[2],t[1]); fflush(stdout);
  }
  */

  return ( (t0 + (t1 << 7) + (t2 << 14)) & 0x3fffff );
}

#define GETSTADR1(byte0, byte1, byte2) \
  ( ((byte0+T0ST-1) + ((byte1-byte0+T1ST-1) << 7) + ((byte2-byte1+T2ST-1) << 14)) & 0x3fffff )

static int
GETSTPATH(int adr)
{
  unsigned char t0, t1, t2;

  t0 = (unsigned char)( (adr     &0x7f)+1-T0ST);
  t1 = (unsigned char)(((adr>>7) &0x7f)+1-T1ST+t0);
  t2 = (unsigned char)(((adr>>14)&0xff)+1-T2ST+t1);
#ifdef Linux
  return( t0+(t1<<8)+(t2<<16) );
#else
  return( (t0<<24)+(t1<<16)+(t2<<8) );
#endif
}




/* <<<<<end_of_compress_uncompress>>>>> */



#define NALLOC 5

/* update dictionary, using slist[] only */

int
prupdatelink(int atest, int stest, float vect1[6], float mom, int ntof, int ecdigi2[3], int nw[6][6], float *p)
{
  DCroad *ptr;
  int iv, adr, k, is, pin, charge;
  char *b1, *b2;
  unsigned char *cha, *chs;
  static int nev = 0;

cha = (unsigned char *)&atest;
chs = (unsigned char *)&stest;
/*
printf("prupdatelink: axial -> %3d %3d %3d stereo -> %3d %3d %3d\n",
                       cha[0],cha[1],cha[2],chs[0],chs[1],chs[2]);
*/
/*
  if((cha[0] >= 8 && cha[0] <= 9 || cha[0]==5)
  && cha[1] >= 39 && cha[1] <= 40
  && cha[2] >= 130 && cha[2] <= 140)
     printf("==> %d %d %d - %d %d %d\n",cha[0],cha[1],cha[2],chs[0],chs[1],chs[2]);
*/


#ifndef Linux
  atest = (atest>>8)&0xffffff;
#endif

  nev ++;
  if(!(nev%100000))
  {
    printf("prlib.c: %d roads are generated.\n",nev);
    fflush(stdout);
  }

  for(iv=0; iv<6; iv++) vect1[iv] = vect1[iv] * vk[iv];
  pin = mom * vk[6]; /* mom*VECT7 */
  charge = 1;
  if(pin < 0.) charge = -1;
  pin = ABS(pin);

  adr = GETSTADR(stest);

  if(slist[adr])    /* stereo found - update stereo entry */
  {
    ptr = (DCroad *) slist[adr]->road;
    for(k=0; k<slist[adr]->nroads; k++)
    {
      if(ptr[k].path == atest)
      {		
        /* update existing stereo entry */
#ifdef SIM
		/*
printf("update: %5d, %8x %8x\n",uuu++,atest,stest);
printf("  old: p,x,y,z,dx,dy,dz=%d %d %d %d %d %d %d\n",
  ptr[k].psim,ptr[k].x,ptr[k].y,ptr[k].z,ptr[k].dx,ptr[k].dy,ptr[k].dz);
printf("  new: p,x,y,z,dx,dy,dz=%d %f %f %f %f %f %f\n",
  pin,vect1[0],vect1[1],vect1[2],vect1[3],vect1[4],vect1[5]);
		*/
        ptr[k].psim = (int)( (float)(ptr[k].psim*ptr[k].np + pin)
                    /(float)(ptr[k].np+1) );
        ptr[k].x  = ((float)ptr[k].x  * (float)ptr[k].np + vect1[0]) / (float)(ptr[k].np+1);
        ptr[k].y  = ((float)ptr[k].y  * (float)ptr[k].np + vect1[1]) / (float)(ptr[k].np+1);
        ptr[k].z  = ((float)ptr[k].z  * (float)ptr[k].np + vect1[2]) / (float)(ptr[k].np+1);
        ptr[k].dx = ((float)ptr[k].dx * (float)ptr[k].np + vect1[3]) / (float)(ptr[k].np+1);
        ptr[k].dy = ((float)ptr[k].dy * (float)ptr[k].np + vect1[4]) / (float)(ptr[k].np+1);
        ptr[k].dz = ((float)ptr[k].dz * (float)ptr[k].np + vect1[5]) / (float)(ptr[k].np+1);

        ptr[k].ntof = ((float)ptr[k].ntof * (float)ptr[k].np + (float)ntof) / (float)(ptr[k].np+1);
        ptr[k].nu = ((float)ptr[k].nu * (float)ptr[k].np + (float)ecdigi2[0]) / (float)(ptr[k].np+1);
        ptr[k].nv = ((float)ptr[k].nv * (float)ptr[k].np + (float)ecdigi2[1]) / (float)(ptr[k].np+1);
        ptr[k].nw = ((float)ptr[k].nw * (float)ptr[k].np + (float)ecdigi2[2]) / (float)(ptr[k].np+1);
        ptr[k].np++;

        *p = (float)ptr[k].psim / vk[6];
#endif

        return(1);
      }
    }

    /* partner not found - add new axial entry */
    if((slist[adr]->nroads % NALLOC) == 0) /* increase space */
    {
      ptr = (DCroad *) realloc( slist[adr]->road, (slist[adr]->nroads + NALLOC) * sizeof(DCroad) );
      if(!ptr)
      {
        printf("prlib.c: ERROR in REALLOC - exit.\n");
        exit(1);
      }
      else
      {
        slist[adr]->road = ptr;
      }
    }
    k = slist[adr]->nroads;
    slist[adr]->road[k].path = atest;
#ifdef SIM

	/*
printf("new axial entry: %8x %8x\n",atest,stest);
printf("  new: p,x,y,z,dx,dy,dz=%d %f %f %f %f %f %f\n",
  pin,vect1[0],vect1[1],vect1[2],vect1[3],vect1[4],vect1[5]);
	*/

    slist[adr]->road[k].psim = pin;
    slist[adr]->road[k].x  = vect1[0];
    slist[adr]->road[k].y  = vect1[1];
    slist[adr]->road[k].z  = vect1[2];
    slist[adr]->road[k].dx = vect1[3];
    slist[adr]->road[k].dy = vect1[4];
    slist[adr]->road[k].dz = vect1[5];

    slist[adr]->road[k].charge = charge;
    slist[adr]->road[k].ntof = ntof;
    slist[adr]->road[k].nu = ecdigi2[0];
    slist[adr]->road[k].nv = ecdigi2[1];
    slist[adr]->road[k].nw = ecdigi2[2];
    slist[adr]->road[k].np = 1;
    for(is=0; is<6; is++) slist[adr]->road[k].npat[is] = 0;
#endif
    slist[adr]->nroads ++;

    *p = (float)pin / vk[6];

    return(1);
  }
  else    /* stereo not found - create new stereo entry */
  {
    slist[adr] = (DCaxial *) malloc(sizeof(DCaxial));
    if(!slist[adr]) {printf("prlib: ERROR in malloc\n"); exit(1);}
    slist[adr]->nroads = 1;
    slist[adr]->road = (DCroad *) malloc(NALLOC * sizeof(DCroad));

    slist[adr]->road[0].path = atest;
#ifdef SIM
	/*
printf("new stereo entry: %8x %8x\n",atest,stest);
printf("  new: p,x,y,z,dx,dy,dz=%d %f %f %f %f %f %f\n",
  pin,vect1[0],vect1[1],vect1[2],vect1[3],vect1[4],vect1[5]);
	*/
    slist[adr]->road[0].psim = pin;
    slist[adr]->road[0].x  = vect1[0];
    slist[adr]->road[0].y  = vect1[1];
    slist[adr]->road[0].z  = vect1[2];
    slist[adr]->road[0].dx = vect1[3];
    slist[adr]->road[0].dy = vect1[4];
    slist[adr]->road[0].dz = vect1[5];

    slist[adr]->road[0].charge = charge;
    slist[adr]->road[0].ntof = ntof;
    slist[adr]->road[0].nu = ecdigi2[0];
    slist[adr]->road[0].nv = ecdigi2[1];
    slist[adr]->road[0].nw = ecdigi2[2];
    slist[adr]->road[0].np = 1;
    for(is=0; is<6; is++) slist[adr]->road[0].npat[is] = 0;
#endif

    *p = (float)pin / vk[6];

    return(1);
  }

}



/* look for 3 axial (ax_st = 0) or 3 stereo (ax_st = 1) combinations,
trying to find not only exact match in dictionary but similar roads
as well; 'ir1', 'ir2' and 'ir3' are deltas for search */

int
getadr1(int ax_st, unsigned char ch0, unsigned char ch1, unsigned char ch2)
{
  if(ax_st==0) return(GETAXADR1(ch0,ch1,ch2));
  else if(ax_st==1) return(GETSTADR1(ch0,ch1,ch2));
  else
  {
    printf("getadr1: ERROR: illegal ax_st=%d (must be 0 or 1) - exit\n",ax_st);
    exit(0);
  }
}


int
prroad1(int ax_st, DCstereo *list[NLISTAX],
        unsigned char *pch0, unsigned char *pch1, unsigned char *pch2,
        int ir0, int ir1, int ir2)
{
  unsigned char ch0, ch1, ch2, i, j, k;
  int adr;

  if(ax_st==0)
  {
#ifdef DEBUG
    printf("prroad1: checking AXIAL ..\n");
#endif
  }
  else
  {
#ifdef DEBUG
    printf("prroad1: checking STEREO ..\n");
#endif
  }

  ch0 = *pch0;
  ch1 = *pch1;
  ch2 = *pch2;
  if(ch0>127||ch1>191||ch2>191)
  {
#ifdef DEBUG
    printf("prroad1: one of wires out of range - not found\n");
#endif
    return(0);
  }
  adr = getadr1(ax_st,ch0,ch1,ch2);
  if(!list[adr])
  {
#ifdef DEBUG
    printf("prroad1: notfound: %d %d %d ...looking around...\n",ch0,ch1,ch2);
#endif

    for(i=ch0-ir0; i<=ch0+ir0; i++)
	{
      for(j=ch1-ir1; j<=ch1+ir1; j++)
	  {
        for(k=ch2-ir2; k<=ch2+ir2; k++)
	    {
          adr=getadr1(ax_st,i,j,k);
          if(list[adr])
          {
            ch0=i;
            ch1=j;
            ch2=k;
#ifdef DEBUG
            printf("prroad1: found corrected: %d %d %d\n",ch0,ch1,ch2);
#endif
            goto vihod;
          }
#ifdef DEBUG
          else
		  {
            /*printf("prroad1: tried %d %d %d not there ..\n",i,j,k)*/;
          }
#endif
		}
	  }
	}
#ifdef DEBUG
    printf("prroad1: notfound: %d %d %d !!!\n",ch0,ch1,ch2);
#endif
    return(0);
  }
#ifdef DEBUG
  else
  {
    printf("prroad1: found original: %d %d %d\n",ch0,ch1,ch2);
  }
#endif

vihod:

  *pch0 = ch0;
  *pch1 = ch1;
  *pch2 = ch2;

  return(adr);
}


#define RECORD_TRACK \
      ntrack = *ntrk; \
      if(ntrack < (NTRACK-1)) \
      { \
        for(is=0; is<6; is++) \
        { \
          if(axst[is]) \
          { \
            for(il=0; il<NLAY; il++) \
            { \
              itmp = sgm->rg[ireg[is]].ax[icl[ireg[is]]].la[il].nhit; \
              for(ih=0; ih<itmp; ih++) \
              { \
                  track[ntrack].cluster[is].layer[il].iw[ih] \
                    = sgm->rg[ireg[is]].ax[icl[ireg[is]]].la[il].iw[ih]+1; \
                  track[ntrack].cluster[is].layer[il].tdc[ih] \
                    = sgm->rg[ireg[is]].ax[icl[ireg[is]]].la[il].tdc[ih]; \
              } \
              track[ntrack].cluster[is].layer[il].nhit = itmp; \
            } \
          } \
          else \
          { \
            for(il=0; il<NLAY; il++) \
            { \
              itmp = sgm->rg[ireg[is]].st[icl[ireg[is]]].la[il].nhit; \
              for(ih=0; ih<itmp; ih++) \
              { \
                  track[ntrack].cluster[is].layer[il].iw[ih] \
                    = sgm->rg[ireg[is]].st[icl[ireg[is]]].la[il].iw[ih]+1; \
                  track[ntrack].cluster[is].layer[il].tdc[ih] \
                    = sgm->rg[ireg[is]].st[icl[ireg[is]]].la[il].tdc[ih]; \
              } \
              track[ntrack].cluster[is].layer[il].nhit = itmp; \
            } \
          } \
        } \
        track[ntrack].sector  = sector; \
        track[ntrack].p       = MOM(ptr[k].p); \
        track[ntrack].charge  = ptr[k].charge; \
        track[ntrack].ntof    = ptr[k].ntof; \
        track[ntrack].nu      = ptr[k].nu; \
        track[ntrack].nv      = ptr[k].nv; \
        track[ntrack].nw      = ptr[k].nw; \
        track[ntrack].vect[0] = (float)ptr[k].x / vk[0]; \
        track[ntrack].vect[1] = (float)ptr[k].y / vk[1]; \
        track[ntrack].vect[2] = (float)ptr[k].z / vk[2]; \
        track[ntrack].vect[3] = (float)ptr[k].dx / vk[3]; \
        track[ntrack].vect[4] = (float)ptr[k].dy / vk[4]; \
        track[ntrack].vect[5] = (float)ptr[k].dz / vk[5]; \
        track[ntrack].ntrk = 1; \
        /* \
        printf("prroad: trk# %d (sec=%d p=%6.3f charge=%d ntof=%d uvw=%d %d %d)\n", \
		 ntrack,track[ntrack].sector,track[ntrack].p, \
                 track[ntrack].charge,track[ntrack].ntof, \
                 track[ntrack].nu/2,track[ntrack].nv/2,track[ntrack].nw/2); \
        fflush(stdout); \
        */ \
        *ntrk = ++ntrack;  \
      } \
      return(1)




int
prroad(unsigned char cha0, unsigned char cha1, unsigned char cha2,
       unsigned char chs0, unsigned char chs1, unsigned char chs2,
       PRSEC *sgm, int icl[3], int sector, int *ntrk, PRTRACK *track)
{
  DCroad *ptr;
  int adr, k, is, ntrack, path, itmp, il, ih;
  unsigned char *ch;
  int ireg[6] = {0,0,1,1,2,2};
  int axst[6] = {1,0,1,0,1,0};

  /*sometimes have 255 - if nothing was found (search for '254' in sgutil.c)*/
  /*
  if(cha0>127) cha0=127;
  if(cha1>191) cha1=191;
  if(cha2>191) cha2=191;
  if(chs0>127) chs0=127;
  if(chs1>191) chs1=191;
  if(chs2>191) chs2=191;
  */

#ifdef DEBUG
  printf("++++++++++ sector=%d, AX= %d %d %d, ST= %d %d %d ++++++++++++++++\n",
    sector,cha0,cha1,cha2,chs0,chs1,chs2);
#endif

  /* try 3SL stereo */
  adr = prroad1(1,slist,&chs0,&chs1,&chs2,2,2,2);
  if(adr==0)
  {
    goto tryaxial; /* no 3SL stereo - goto tryaxial */
  }

  /********************************************/
  /* take stereo and check all axial partners */

  /* all 3 superlayers are required */
  ptr = (DCroad *) slist[adr]->road;
  for(k=0; k<slist[adr]->nroads; k++)
  {
    path = ptr[k].path;
#ifndef Linux
    path = path<<8;
#endif
    ch = (unsigned char *)&path;
#ifdef DEBUG
    printf("prroad: trying axial from dictionary (3SL): %3d %3d %3d\n",
      ch[0],ch[1],ch[2]);
    fflush(stdout);
#endif
    if( (ABS(ch[0]-cha0)<6) && (ABS(ch[1]-cha1)<6) && (ABS(ch[2]-cha2)<6) )
    {
#ifdef DEBUG
      printf("prroad: ax %3d %3d %3d st %3d %3d %3d\n",
        cha0,cha1,cha2,chs0,chs1,chs2);fflush(stdout);
      printf("prroad: have something ...\n");fflush(stdout);
#endif
      RECORD_TRACK;
    }
  }

  /* 2 superlayers are required */
  ptr = (DCroad *) slist[adr]->road;
  for(k=0; k<slist[adr]->nroads; k++)
  {
    path = ptr[k].path;
#ifndef Linux
    path = path<<8;
#endif
    ch = (unsigned char *)&path;
#ifdef DEBUG
    printf("prroad: trying axial from dictionary (2SL): %3d %3d %3d\n",
      ch[0],ch[1],ch[2]);
    fflush(stdout);
#endif
    if( (ABS(ch[0]-cha0)<6) && (ABS(ch[1]-cha1)<6) )
    {
      cha2 = ch[2];
#ifdef DEBUG
      printf("prroad: 2SL ax %3d %3d (%3d) st %3d %3d %3d\n",
        cha0,cha1,cha2,chs0,chs1,chs2);fflush(stdout);
      printf("prroad: have something ...\n");fflush(stdout);
#endif
      RECORD_TRACK;
    }
    else if( (ABS(ch[0]-cha0)<6) && (ABS(ch[2]-cha2)<6) )
	{
      cha1 = ch[1];
#ifdef DEBUG
      printf("prroad: 2SL ax %3d (%3d) %3d st %3d %3d %3d\n",
        cha0,cha1,cha2,chs0,chs1,chs2);fflush(stdout);
      printf("prroad: have something ...\n");fflush(stdout);
#endif
      RECORD_TRACK;
	}
    else if( (ABS(ch[1]-cha1)<6) && (ABS(ch[2]-cha2)<6) )
	{
      cha0 = ch[0];
#ifdef DEBUG
      printf("prroad: 2SL ax (%3d) %3d %3d st %3d %3d %3d\n",
        cha0,cha1,cha2,chs0,chs1,chs2);fflush(stdout);
      printf("prroad: have something ...\n");fflush(stdout);
#endif
      RECORD_TRACK;
	}
  }


  /* did not find 3SLstereo+3SL(2SL)axial, will try axial */


tryaxial:
  /**************************************************************/
  /* if found nothing, take axial and check all stereo partners */

  adr = prroad1(0,alist,&cha0,&cha1,&cha2,2,2,2);
  if(adr==0)
  {
    return(0); /* if no axial then no track - return */
  }

  /* all 3 superlayers are required */
  ptr = (DCroad *) alist[adr]->road;
  for(k=0; k<alist[adr]->nroads; k++)
  {
    path = ptr[k].path;
#ifndef Linux
    path = path<<8;
#endif
    ch = (unsigned char *)&path;
#ifdef DEBUG
    printf("prroad: trying stereo from dictionary (3SL): %3d %3d %3d\n",
      ch[0],ch[1],ch[2]);
    fflush(stdout);
#endif
    if( (ABS(ch[0]-chs0)<6) && (ABS(ch[1]-chs1)<6) && (ABS(ch[2]-chs2)<6) )
    {
#ifdef DEBUG
      printf("prroad: ax %3d %3d %3d st %3d %3d %3d\n",
        cha0,cha1,cha2,chs0,chs1,chs2);fflush(stdout);
      printf("prroad: have something ...\n");fflush(stdout);
#endif
      RECORD_TRACK;
    }
  }


  /* 2 superlayers are required */
  ptr = (DCroad *) alist[adr]->road;
  for(k=0; k<alist[adr]->nroads; k++)
  {
    path = ptr[k].path;
#ifndef Linux
    path = path<<8;
#endif
    ch = (unsigned char *)&path;
#ifdef DEBUG
    printf("prroad: trying stereo from dictionary (2SL): %3d %3d %3d\n",
      ch[0],ch[1],ch[2]);
    fflush(stdout);
#endif
    if( (ABS(ch[0]-chs0)<6) && (ABS(ch[1]-chs1)<6) )
    {
      chs2 = ch[2];
#ifdef DEBUG
      printf("prroad: 2SL ax %3d %3d %3d st %3d %3d (%3d)\n",
        cha0,cha1,cha2,chs0,chs1,chs2);fflush(stdout);
      printf("prroad: have something ...\n");fflush(stdout);
#endif
      RECORD_TRACK;
    }
    else if( (ABS(ch[0]-chs0)<6) && (ABS(ch[2]-chs2)<6) )
	{
      chs1 = ch[1];
#ifdef DEBUG
      printf("prroad: 2SL ax %3d (%3d) %3d st %3d (%3d) %3d\n",
        cha0,cha1,cha2,chs0,chs1,chs2);fflush(stdout);
      printf("prroad: have something ...\n");fflush(stdout);
#endif
      RECORD_TRACK;
	}
    else if( (ABS(ch[1]-chs1)<6) && (ABS(ch[2]-chs2)<6) )
	{
      chs0 = ch[0];
#ifdef DEBUG
      printf("prroad: 2SL ax (%3d) %3d %3d st (%3d) %3d %3d\n",
        cha0,cha1,cha2,chs0,chs1,chs2);fflush(stdout);
      printf("prroad: have something ...\n");fflush(stdout);
#endif
      RECORD_TRACK;
	}
  }




  return(0);
}


int
prlib(int *jw, PRTRACK *track)
{
  int i, isec, found, ntrack, icl0, icl1, icl2, ncl[3], icl[3];
  unsigned char ird[3][2][nclmx];

  /* sector-based segment finding structure */
  int nsgm;
  PRSEC sgm;

/*************************/
/* ????????????????????? */
/*************************/


  etNformat(jw,"ECPI","3I,6F");
  etNformat(jw,"DC1","I,F");
  etNformat(jw,"PATH","F");
  /*
  etNformat(jw,"HBLA","F");
  etNformat(jw,"HDPL","F");
  etNformat(jw,"TBLA","F");
  etNformat(jw,"TDPL","F");
  etNformat(jw,"HBTR","F");
  etNformat(jw,"TBTR","F");
  */

/*************************/
/* ????????????????????? */
/*************************/

#ifdef DEBUG
  printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
#endif

  ntrack = 0;
  for(isec=1; isec<=6; isec++)
  {
#ifdef DEBUG
    printf("++++++++ SECTOR %d ++++++++++++++++++++++++\n",isec);
#endif
    /* 1-cleaning only, 2-segm.find. only, 3-cleaning and segm.find. */
    /*start_timer();*/
    sglib(jw, isec, 2, &nsgm, &sgm); /* search for segments */
    /*stop_timer(1000);*/
    if(nsgm == 0)
    {
#ifdef DEBUG
      printf("NO SEGMENTS\n");
#endif
      continue;
	}
    sgprint(&sgm);
    sgroad(&sgm,ncl,ird);

#ifdef DEBUG
    printf("GOTO PRROAD: %d %d %d\n",ncl[0],ncl[1],ncl[2]);
#endif
    for(icl0 = 0; icl0 < ncl[0]; icl0++)
    {
      for(icl1 = 0; icl1 < ncl[1]; icl1++)
      {
        for(icl2 = 0; icl2 < ncl[2]; icl2++)
        {
          icl[0] = icl0;
          icl[1] = icl1;
          icl[2] = icl2;
		  /*
ird[0][0][icl0]+=7;
ird[0][1][icl0]+=7;
		  */
          found = prroad(ird[0][0][icl0],ird[1][0][icl1],ird[2][0][icl2],
                         ird[0][1][icl0],ird[1][1][icl1],ird[2][1][icl2],
                         &sgm,icl,isec,&ntrack,track);
/*
#ifdef DEBUG
printf("isec=%d, road: %3d %3d %3d %3d %3d %3d, found=%d\n",isec,
ird[0][0][icl0],ird[1][0][icl1],ird[2][0][icl2],
ird[0][1][icl0],ird[1][1][icl1],ird[2][1][icl2],found);
#endif
*/
        }
      }
    }
  }

/*#ifdef DEBUG*/
  /* print EVNT bank if exist (for cooked files only */
  {
    int i, j, ind, nrow, ncol, ind1, nrow1, ncol1, dcstat, sctr, isec, *iptr;
    int sctr_sec, sctr_ptr, ind2, ncol2, nrow2, ind3, nrow3, ncol3;
    int itr_sec, sec, itr, iw[34];
    float *fptr, phi, theta;

    ind2 = etNlink(jw,"HBTR",0);
	/*printf("================= ind2=%d\n",ind2);*/

    if((ind = etNlink(jw,"EVNT",0)) > 0)
    {
      nrow = etNrow(jw,ind);
      ncol = etNcol(jw,ind);
      /*printf("\nEVNT: nrow=%d, ncol=%d\n",nrow,ncol);*/
      for(i=0; i<nrow; i++)
      {
        iptr = (int *)&jw[ind+i*ncol];
        fptr = (float *)&jw[ind+i*ncol];
        dcstat = iptr[11];
if(iptr[0]!=11) goto donotprint;
        printf("[%5d] p=%f m=%f charge=%d beta=%f\n",
          iptr[0],fptr[1],fptr[2],iptr[3],fptr[4]);
        printf("   cos=%f,%f,%f coord=%f,%f,%f\n",
          fptr[5],fptr[6],fptr[7],fptr[8],fptr[9],fptr[10]);

        phi = atan2(fptr[6],fptr[5]) * RADDEG;
        if(phi < 0.) phi += 360.0;
        isec = (phi + 30.)/60. + 1.;
        if(isec > 6) isec -= 6;

        theta = acos(fptr[7])*RADDEG;
        printf("   sector=%d, phi=%f, theta=%f (degrees)\n",isec,phi,theta);

        printf("dcstat=%d\n",dcstat);
        if(dcstat > 0)
        {
          if((ind1 = etNlink(jw,"DCPB",0)) > 0)
          {
            nrow1 = etNrow(jw,ind1);
            ncol1 = etNcol(jw,ind1);
            sctr = jw[ind1+(dcstat-1)*ncol1];
            sctr_sec = sctr / 100;
            sctr_ptr = sctr - sctr_sec * 100;
            printf("\nDCPB: nrow=%d, ncol=%d, sctr=%d (%d %d)\n",
              nrow1,ncol1,sctr,sctr_sec,sctr_ptr);

			printf("sctr_ptr=%d\n",sctr_ptr);
            if(sctr_ptr > 0)
			{
			  printf("sctr_ptr=%d\n",sctr_ptr);
              ind2 = etNlink(jw,"HBTR",0);
	      /*printf("ind2=%d\n",ind2);*/
              if(ind2 > 0)
              {
                nrow2 = etNrow(jw,ind2);
                ncol2 = etNcol(jw,ind2);
                itr_sec = jw[ind2+(sctr_ptr-1)*ncol2+8];
                sec = itr_sec / 100;
                itr = itr_sec - sec * 100;
                /*printf("\nHBTR: nrow=%d, ncol=%d, itr_sec=%d (%d %d)\n",
                  nrow2,ncol2,itr_sec,sec,itr);*/

                if(itr > 0 && sec > 0 && sec < 7)
			    {
                  if((ind3 = etNlink(jw,"HBLA",sec)) > 0)
                  {
                    nrow3 = etNrow(jw,ind3);
                    ncol3 = etNcol(jw,ind3);
                    /*printf("ncol1=%d nrow3=%d\n",ncol3,nrow3);*/
                    for(j=0; j<34; j++)
                    {
                      iw[j] = jw[ind3 + (itr-1)*ncol3*34 + 10 + j*16];
                      /*printf("iw[%2d] = %d\n",j,iw[j]);*/
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
donotprint:
    ;
  }
  /*
  printf("============================================================\n");
  printf("============================================================\n");
  printf("============================================================\n");
  printf("============================================================\n");
  printf("============================================================\n");
  */
/*#endif*/


  for(i=1; i<=6; i++)
  {
    etNdrop(jw,"DC1 ",i);
    etNdrop(jw,"HBLA",i);
    etNdrop(jw,"HDPL",i);
    etNdrop(jw,"TBLA",i);
    etNdrop(jw,"TDPL",i);
  }
  i=0;
  etNdrop(jw,"HBTR",i);
  etNdrop(jw,"TBTR",i);
  etNdrop(jw,"PATH",i);
  etNdrop(jw,"ECPI",i);

  return(ntrack);
}



















/*
 prbos.c - filling output BOS bank

  input:  iw, ntrk, trk

CHECK bcopy(), DOING IN ONE MOVE, BUT IF ENDS IN THE MIDDLE OF WORD ? WHAT ABOUT FORMAT ???

*/

int
prbos(int *jw, int *ntrk, PRTRACK *trk)
{
  PRTRACK *trkout;
  int nr, ncol, ind, i;

  nr = 0;
  ncol = (sizeof(PRTRACK)+3)/4;
  ind = etNcreate(jw,"PATH",nr,ncol,*ntrk);
  /*printf("prbos: nr=%d ncol=%d nrow=%d -> ind=%d\n",nr,ncol,*ntrk,ind);*/
  if(ind <= 0) {/*printf("prbos: cannot create PATH bank: %d %d -> return\n",ncol,*ntrk);*/ return(-1);}
  trkout = (PRTRACK *)&jw[ind];
  bcopy((char *)trk, (char *)trkout, (*ntrk)*sizeof(PRTRACK));
  /*printf("prbos: creates PATH bank\n");*/

  return(ind);
}


/************************************************/
/*********** INITIALIZATION PROCEDURE ***********/
/************************************************/


void
PRINTDIFF(int axial, int stereo)
{
  char *b1, *b2;

  b1 = (char *)&axial;
  b2 = (char *)&stereo;
  /*
  printf("dif= %3d %3d %3d - %3d %3d %3d = %3d %3d %3d\n",
    b1[0],b1[1],b1[2],b2[0],b2[1],b2[2],b1[0]-b2[0],b1[1]-b2[1],b1[2]-b2[2]);
  */
  if(max[0] < (b1[0]-b2[0])) max[0] = b1[0]-b2[0];
  if(max[1] < (b1[1]-b2[1])) max[1] = b1[1]-b2[1];
  if(max[2] < (b1[2]-b2[2])) max[2] = b1[2]-b2[2];
  if(min[0] > (b1[0]-b2[0])) min[0] = b1[0]-b2[0];
  if(min[1] > (b1[1]-b2[1])) min[1] = b1[1]-b2[1];
  if(min[2] > (b1[2]-b2[2])) min[2] = b1[2]-b2[2];
  /*
  printf("min= %3d %3d %3d   ",min[0],min[1],min[2]);
  printf("max= %3d %3d %3d\n",max[0],max[1],max[2]);
  */
  return;
}







/***********************************************
 ***********************************************
   lanal1 - in use
   lanal2 & lanal5 - not in use 
 ***********************************************
************************************************/

void
prinit(char *filename, int lanal1, int lanal2, int lanal5)
{
  static int max_lnk, max_lnk_local;
  static int max_npat, npat, ipat[6], take;
  static int i, j, k, l, iv, nf, ind, nroads, nrw, numdb, ncol, nrow, nwd, nch;
  static int numra, numrb, icl, ierr, adr, adr1, tmp, *lptr;
  static char str[1000], *knam;
  static char utimes[25];
  int *buf, *buf0, md1, md2, md3, is, in;
  short *buf2;
  char *buf3;
  unsigned char *ntest1, *ntest2, *ch;
  char kname[9], hname[9], format[NFMT+1];
  int dtyp, utime;
  int lnk[NS2];
  int *link1, *link2;
  int *link4;
  short *nlink3[2];
  static LINKsegm *nlink4;

  /* new segment finding initialization */
  sginit();

  /* lanal1 = 0   old algorithm (no update) */
  /* lanal1 = 1   Write only (templet generating) (1-st time one generates new templets) */
  /* lanal1 = 2   Both Read & Write (read and write updated templets) */
  /* lanal1 = 3   new algorithm (no update) */

  /* create first segment "by hands" */
  if(lanal1 == 1)
  {
    for(i=0; i<NLISTST; i++) slist[i] = 0; /* will use it in prupdatelink, should be clean !!! */
    for(i=0; i<NLISTST; i++) slist[i] = 0; /* will use it in prupdatelink, should be clean !!! */
  }

  /* lanal1 = 0 or 2 or 3 is Read-Only or Read-Write (Update) or new segment algorithm */

  if(lanal1 == 0 || lanal1 == 2 || lanal1 == 3)
  {
    int tmp;

    /* cleanup */
    for(i=0; i<NLISTAX; i++) alist[i] = 0;
    for(i=0; i<NLISTST; i++) slist[i] = 0;


    /*******************************************************/
    /* first pass - calculates memory size for every stack */
    /*******************************************************/

    OPENFILE(filename);

    /* read first record */
    for(i=0; i<strlen(hname); i++) hname[i] = ' ';
    for(i=0; i<strlen(format); i++) format[i] = ' ';
    format[NFMT] = '\0';
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),NFMT);
    buf0 = (int *) malloc(ncol*sizeof(int));
    frdat_(&md1,buf0,&ncol);
    max_npat = (md1-1-2-NS2)/6;
    printf(" %s %8d %6d %6d %6d    %s ---> max_npat=%d\n",
                      hname,numdb,ncol,nrow,md1,format,max_npat);
    max_lnk = buf0[ncol-1];
    printf("max_lnk=%d max_npat=%d\n",max_lnk,max_npat); fflush(stdout);
    k = 0;



    /*************************************************************************/
    /* readout old segment info - to be compatible with old dictionary files */
    k++;
    for(j=0; j<max_npat; j++) for(i=0; i<6; i++) k++;
    /*************************************************************************/


    dtyp = buf0[k++];
    utime = buf0[k++];
    for(i=0; i<NS2; i++) lnk[i] = buf0[k++];


    /* allocate temporary buffers */

    link1 = (int *) malloc(max_lnk*sizeof(int));
    if(!link1) {printf("prlib: ERROR: malloc problem (link1)\n"); fflush(stdout);}
    link2 = (int *) malloc(max_lnk*sizeof(int));
    if(!link2) {printf("prlib: ERROR: malloc problem (link2)\n"); fflush(stdout);}
    nlink3[0] = (short *) malloc(max_lnk*sizeof(short));
    if(!nlink3[0]) {printf("prlib: ERROR: malloc problem (nlink3[0])\n"); fflush(stdout);}
    nlink3[1] = (short *) malloc(max_lnk*sizeof(short));
    if(!nlink3[1]) {printf("prlib: ERROR: malloc problem (nlink3[1])\n"); fflush(stdout);}
    link4 = (int *) malloc(max_lnk*NVEC*sizeof(int));
    if(!link4) {printf("prlib: ERROR: malloc problem (link4)\n"); fflush(stdout);}
    buf = (int *) malloc(max_lnk*sizeof(int));
    buf2 = (short *) buf;
    nlink4 = (LINKsegm *) malloc(max_lnk*sizeof(LINKsegm));
    if(!nlink4) {printf("prlib: ERROR: malloc problem (nlink4)\n"); fflush(stdout);}
    for(i=0; i<strlen(hname); i++) hname[i] = ' ';
    for(i=0; i<strlen(format); i++) format[i] = ' ';
    buf3 = (char *) malloc(max_lnk*6*(MAXNPAT+1)*sizeof(char));
    if(!buf3) {printf("prlib: ERROR: malloc problem (buf3)\n",max_lnk); fflush(stdout);}


    /* fill alist[] and slist[] temporary with bytes# should be allocated */

    for(i=0; i<NS2; i++)
    {
      READFILE;

      for(j=0; j<lnk[i]; j++)
      {
        adr = GETAXADR(link1[j]);
        if(adr >= NLISTAX) printf("prlib: ERROR: adr1=%d %x\n",adr,adr);
        alist[adr] ++;

        adr = GETSTADR(link2[j]);
        if(adr >= NLISTST) printf("prlib: ERROR: adr2=%d %x\n",adr,adr);
        slist[adr] ++;
      }
    }

    CLOSEFILE;

    /***********************************/
    /* second pass - memory allocation */
    /***********************************/

    for(i=0; i<NLISTAX; i++)
    {
      if(alist[i])
      {
        tmp = ((int)alist[i])/sizeof(DCstereo); /* how many stereo partners */
        alist[i] = (DCstereo *) malloc(sizeof(DCstereo));
        alist[i]->nroads = 0; /* cleanup length word */
        alist[i]->min[0] = alist[i]->min[1] = alist[i]->min[2] = 200;
        alist[i]->max[0] = alist[i]->max[1] = alist[i]->max[2] = 0;
        alist[i]->road = (DCroad *) malloc(tmp * sizeof(DCroad));
      }
    }

    for(i=0; i<NLISTST; i++)
    {
      if(slist[i])
      {
        tmp = ((int)slist[i])/sizeof(DCaxial); /* how many stereo partners */
        slist[i] = (DCaxial *) malloc(sizeof(DCaxial));
        slist[i]->nroads = 0; /* cleanup length word */
        slist[i]->min[0] = slist[i]->min[1] = slist[i]->min[2] = 200;
        slist[i]->max[0] = slist[i]->max[1] = slist[i]->max[2] = 0;
        slist[i]->road = (DCroad *) malloc(tmp * sizeof(DCroad));
      }
    }


    /*******************************/
    /* third pass - memory filling */
    /*******************************/

    OPENFILE(filename);

    /* read first record */

    for(i=0; i<strlen(hname); i++) hname[i] = ' ';
    for(i=0; i<strlen(format); i++) format[i] = ' ';
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),NFMT);
    frdat_(&md1,buf0,&ncol);


    /* loop over dictionary file */

    for(i=0; i<NS2; i++)
    {
      READFILE;

      /* copy buf3 to nlink4 */

      k = 0;
      for(j=0; j<lnk[i]; j++)
      {
        for(is=0; is<6; is++)
        {
          nlink4[j].npat[is] = buf3[k++];
          for(in=0; in<nlink4[j].npat[is]; in++) nlink4[j].ipat[is][in] = buf3[k++];
        }
      }

      /* fill road structures */

      for(j=0; j<lnk[i]; j++)
      {


        /************ fill alist[] **************/

        adr =  GETAXADR(link1[j]);
        if(adr >= NLISTAX) printf("prlib: ERROR: adr1=%d %x\n",adr,adr);
        k = alist[adr]->nroads ++;
#ifdef Linux
        alist[adr]->road[k].path = link2[j]; /* stereo pattern */
#else
        alist[adr]->road[k].path = (link2[j]>>8)&0xffffff;
#endif
        ch = (unsigned char *)&link2[j];

		/*
        {
          unsigned char *ch1, *ch2;
          ch1 = (unsigned char *)&link1[j];
          ch2 = (unsigned char *)&link2[j];

          if(ch1[0]==14&&ch1[1]==12&ch1[2]==3)
          {
            printf("===> (%3d %3d %3d) (%3d %3d %3d)\n",
              ch1[0],ch1[1],ch1[2],ch2[0],ch2[1],ch2[2]);
          }
        }
		*/


/* apply corrections !!! TEMPORARY !!! */
#define C0 0
#define C1 10
#define C2 5
        if(alist[adr]->max[0] < (ch[0]+C0)) alist[adr]->max[0] = ch[0]+C0;
        if(alist[adr]->max[1] < (ch[1]+C1)) alist[adr]->max[1] = ch[1]+C1;
        if(alist[adr]->max[2] < (ch[2]+C2)) alist[adr]->max[2] = ch[2]+C2;
        if(alist[adr]->min[0] > MAX(0,(ch[0]-C0))) alist[adr]->min[0] = MAX(0,(ch[0]-C0));
        if(alist[adr]->min[1] > MAX(0,(ch[1]-C1))) alist[adr]->min[1] = MAX(0,(ch[1]-C1));
        if(alist[adr]->min[2] > MAX(0,(ch[2]-C2))) alist[adr]->min[2] = MAX(0,(ch[2]-C2));



		/*copied from slist[] fillup below - is it right ???*/
        alist[adr]->road[k].p = IMOM(nlink3[1][j]/VECT7);
        alist[adr]->road[k].charge = link4[j*NVEC+10];
        alist[adr]->road[k].ntof   = link4[j*NVEC+6];
        alist[adr]->road[k].nu     = link4[j*NVEC+7];
        alist[adr]->road[k].nv     = link4[j*NVEC+8];
        alist[adr]->road[k].nw     = link4[j*NVEC+9];
        alist[adr]->road[k].x      = link4[j*NVEC+0];
        alist[adr]->road[k].y      = link4[j*NVEC+1];
        alist[adr]->road[k].z      = link4[j*NVEC+2];
        alist[adr]->road[k].dx     = link4[j*NVEC+3];
        alist[adr]->road[k].dy     = link4[j*NVEC+4];
        alist[adr]->road[k].dz     = link4[j*NVEC+5];





        /************ fill slist[] **************/


        adr =  GETSTADR(link2[j]);
        if(adr >= NLISTST) printf("prlib: ERROR: adr2=%d %x\n",adr,adr);
        k = slist[adr]->nroads ++;
#ifdef Linux
        slist[adr]->road[k].path = link1[j]; /* axial pattern */
#else
        slist[adr]->road[k].path = (link1[j]>>8)&0xffffff;
#endif
        ch = (unsigned char *)&link1[j];

/* apply corrections !!! TEMPORARY !!! */
#define D0 0
#define D1 0
#define D2 0
        if(slist[adr]->max[0] < (ch[0]+D0)) slist[adr]->max[0] = ch[0]+D0;
        if(slist[adr]->max[1] < (ch[1]+D1)) slist[adr]->max[1] = ch[1]+D1;
        if(slist[adr]->max[2] < (ch[2]+D2)) slist[adr]->max[2] = ch[2]+D2;
        if(slist[adr]->min[0] > MAX(0,(ch[0]-D0))) slist[adr]->min[0] = MAX(0,(ch[0]-D0));
        if(slist[adr]->min[1] > MAX(0,(ch[1]-D1))) slist[adr]->min[1] = MAX(0,(ch[1]-D1));
        if(slist[adr]->min[2] > MAX(0,(ch[2]-D2))) slist[adr]->min[2] = MAX(0,(ch[2]-D2));




        slist[adr]->road[k].p = IMOM(nlink3[1][j]/VECT7);/* momentum MeV/c -> (0-255) */
        /* slist[adr]->road[k].np = nlink3[0][j]; how many momenta - DO WE NEED IT HERE ??? */
        slist[adr]->road[k].charge = link4[j*NVEC+10];
        slist[adr]->road[k].ntof   = link4[j*NVEC+6];
        slist[adr]->road[k].nu     = link4[j*NVEC+7];
        slist[adr]->road[k].nv     = link4[j*NVEC+8];
        slist[adr]->road[k].nw     = link4[j*NVEC+9];
        slist[adr]->road[k].x      = link4[j*NVEC+0];
        slist[adr]->road[k].y      = link4[j*NVEC+1];
        slist[adr]->road[k].z      = link4[j*NVEC+2];
        slist[adr]->road[k].dx     = link4[j*NVEC+3];
        slist[adr]->road[k].dy     = link4[j*NVEC+4];
        slist[adr]->road[k].dz     = link4[j*NVEC+5];




      }
    }

    printf("prlib: New dictionary are created.\n");
    nroads = 0;
    for(j=0; j<NS2; j++) nroads = nroads + lnk[j];
    printf("\n  Nroads =%8d\n",nroads);
    for(j=0; j<NS2; j++) {printf("%7d",lnk[j]); if(!((j+1)%10))printf("\n");}
    printf("\n");
    free(buf3);
    free(nlink4);
    free(buf);
    free(link4);
    free(nlink3[1]);
    free(nlink3[0]);
    free(link2);
    free(link1);
    free(buf0);
    CLOSEFILE;
  }

  return;
}

/************************************************/
/********* DICTIONARY WRITING PROCEDURE *********/
/******** called for road generating only *******/
/************************************************/

void
prwrite(char *filename)
{
  int is, in, ister;
  static int max_lnk;
  static int max_npat;
  static int iv, i, j, k, l, nroads, ierr, ind, md1, numra, numrb, icl, adr, spath;
  static int numdb, ncol, ncol1, nrow;
  static char *knam, *hnam, *format, str[100];
  static int utime;
  static char utimes[25];
  static int *buf;
  static short *buf2;
  static char *buf3;
  static char *b1, *b2;
  static int   lnk[NS2];
  static int   *link1;
  static int   *link2;
  static short **nlink3;
#ifdef SIM
  static LINKsegm *nlink4;
  static short **vect;
  static int *bufv;
#endif

  int tmp1, tmp2, tmp[NS2];

    /* find max_npat */
    max_npat = MAX_NPAT;

    /* find max_lnk */
    max_lnk = 0;
    for(i=0; i<NS2; i++) tmp[i] = 0;
    for(adr=0; adr<NLISTST; adr++)
    {
      if(slist[adr])
      {
        spath = GETSTPATH(adr);
/*
{
  char *ch;
  ch = (char *)&spath;
printf("adr=%08x   spath=%08x (%3d %3d %3d)\n",adr,spath,ch[0],ch[1],ch[2]);
}
*/

#ifdef Linux
        i = spath & 0x000000ff;
#else
        i = (spath & 0xff000000)>>24;
#endif
        i--; /* road numbers starts from 1, so make i = i - 1 if we want index from 0 !!! */
        for(k=0; k<slist[adr]->nroads; k++) tmp[i] ++;
      }
    }
    for(i=0; i<NS2; i++) max_lnk = MAX(max_lnk,tmp[i]);
    /*max_lnk = (max_lnk/1000)*1000 + 1000;*/ /* rounding to high 1000 */


    /* allocate temporary buffers */

    link1 = (int *) malloc(max_lnk*sizeof(int));
    if(!link1) printf("prlib: ERROR: malloc problem  8: max_lnk=%d\n",max_lnk); fflush(stdout);

    link2 = (int *) malloc(max_lnk*sizeof(int));
    if(!link2) printf("prlib: ERROR: malloc problem  9: max_lnk=%d\n",max_lnk); fflush(stdout);

    nlink3 = (short **) malloc(max_lnk*sizeof(short *));
    if(!nlink3) printf("prlib: ERROR: malloc problem 10: max_lnk=%d\n",max_lnk); fflush(stdout);
    for(j=0; j<max_lnk; j++)
    {
      nlink3[j] = (short *) malloc(2*sizeof(short));
      if(!nlink3[j]) printf("prlib: ERROR: malloc problem 11\n"); fflush(stdout);
    }
#ifdef SIM
    nlink4 = (LINKsegm *) malloc(max_lnk*sizeof(LINKsegm));
    if(!nlink4) printf("prlib: ERROR: malloc problem 12\n"); fflush(stdout);
    vect = (short **) malloc(max_lnk*sizeof(short *));
    if(!vect) printf("prlib: ERROR: malloc problem 13\n"); fflush(stdout);
    for(j=0; j<max_lnk; j++)
    {
      vect[j] = (short *) malloc(NVEC*sizeof(short));
      if(!vect[j]) printf("prlib: ERROR: malloc problem 14\n"); fflush(stdout);
    }
#endif


    /* open file */

    strcpy(str,"OPEN OUTPUT UNIT=56 FILE=\"");
    strcat(str,filename);
    strcat(str,"\" RECL=32760 WRITE NEW");
    fparm_(str,strlen(str));
    fwname_("OUTPUT",6);
    knam = "LTORM   ";
    numra = numrb = icl = 0;
    fwkey_(knam,&numra,&numrb,&icl,strlen(knam));

    /* write first bank */

    hnam = "DSEG";
    format = "I";
    numdb = 1;
    ncol = 1 + 6 * max_npat + 2 + NS2 + 1;
    nrow = 1;
    fwhdr_(hnam,&numdb,&ncol,&nrow,format,strlen(hnam),strlen(format));
    buf = (int *) malloc(ncol*sizeof(int));
    if(!buf) { printf("prlib: ERROR: can not allocate buf (ncol=%d)\n",ncol); exit(1); }
    k = 0;


	/*****************************/
	/* temporary - fake segments */
    buf[k++] = 0;
    for(j=0; j<max_npat; j++)
      for(i=0; i<6; i++)
        buf[k++] = 0;
	/*****************************/



    /*if(strncmp(knam,"LTORM",5) == 0)*/ buf[k++] = 3;
    getunixtime_(&buf[k++]); /* unix time */
    for(i=0; i<NS2; i++) buf[k++] = tmp[i];
    buf[k++] = max_lnk;
    fwdat_(&ncol,buf);
    free(buf);
    printf(" %s %8d %6d %6d %6d    %s\n",hnam,numdb,ncol,nrow,ncol,format); fflush(stdout);

    /* update lnk,link1,link2,nlink3;
       everything are sorted against stereo 1 (ntest2(1)) !!! */

    max[0] = max[1] = max[2] = -100;
    min[0] = min[1] = min[2] =  100;

    /* cleanup road counter amd allocate temporary buffers */

    for(i=0; i<NS2; i++) lnk[i] = 0;

    buf = (int *) malloc(max_lnk*sizeof(int));
    if(!buf) { printf("prlib: ERROR: can not allocate buf (max_lnk=%d)\n",max_lnk); fflush(stdout); exit(1); }
    buf2 = (short *) buf;
#ifdef SIM
    buf3 = (char *) malloc(max_lnk*6*(MAXNPAT+1)*sizeof(char));
    if(!buf3) { printf("prlib: ERROR: can not allocate buf3 (max_lnk=%d)\n",max_lnk); fflush(stdout); exit(1); }
    bufv = (int *) malloc(max_lnk*NVEC*sizeof(int));
    if(!bufv) { printf("prlib: ERROR: can not allocate bufv (max_lnk=%d)\n",max_lnk); fflush(stdout); exit(1); }
#endif

    for(ister=0; ister<NS2; ister++)
    {
      /* cleanup temporary buffers */

      for(j=0; j<max_lnk; j++)
      {
        link1[j] = 0;
        link2[j] = 0;
        nlink3[j][0] = 0;
        nlink3[j][1] = 0;
#ifdef SIM
        for(is=0; is<6; is++)
        {
          nlink4[j].npat[is] = 0;
          for(in=0; in<MAXNPAT; in++) nlink4[j].ipat[is][in] = 0;
        }
#endif
      }

      for(adr=0; adr<NLISTST; adr++)
      {
        if(slist[adr])
        {
          spath = GETSTPATH(adr);
#ifdef Linux
          i = spath & 0x000000ff;
#else
          i = (spath & 0xff000000)>>24;
#endif
          i--; /* road numbers starts from 1, so make i = i - 1 if we want index from 0 !!! */
          if(i == ister)
          {
            for(k=0; k<slist[adr]->nroads; k++)
            {
              link2[lnk[i]] = spath;                    /* stereo pattern */
              link1[lnk[i]] = slist[adr]->road[k].path;  /* axial pattern */
#ifndef Linux
              link1[lnk[i]] = link1[lnk[i]] << 8;
#endif

              b1 = (char *)&link1[lnk[i]];
              b2 = (char *)&link2[lnk[i]];
              /*
              printf("dif= %3d %3d %3d - %3d %3d %3d = %3d %3d %3d\n",
              b1[0],b1[1],b1[2],b2[0],b2[1],b2[2],b1[0]-b2[0],b1[1]-b2[1],b1[2]-b2[2]);
              */
              if(max[0] < (b1[0]-b2[0])) max[0] = b1[0]-b2[0];
              if(max[1] < (b1[1]-b2[1])) max[1] = b1[1]-b2[1];
              if(max[2] < (b1[2]-b2[2])) max[2] = b1[2]-b2[2];
              if(min[0] > (b1[0]-b2[0])) min[0] = b1[0]-b2[0];
              if(min[1] > (b1[1]-b2[1])) min[1] = b1[1]-b2[1];
              if(min[2] > (b1[2]-b2[2])) min[2] = b1[2]-b2[2];




#ifdef SIM
              nlink3[lnk[i]][0] = slist[adr]->road[k].np;
              /*nlink3[lnk[i]][1] = IMOM((float)slist[adr]->road[k].u.s.p/VECT7);*/
              nlink3[lnk[i]][1] = slist[adr]->road[k].psim;

              for(is=0; is<6; is++)
              {
                nlink4[lnk[i]].npat[is] = slist[adr]->road[k].npat[is];
                for(in=0; in<nlink4[lnk[i]].npat[is]; in++)
                  nlink4[lnk[i]].ipat[is][in] = slist[adr]->road[k].ipat[is][in];
              }
              vect[lnk[i]][0]  = slist[adr]->road[k].x;
              vect[lnk[i]][1]  = slist[adr]->road[k].y;
              vect[lnk[i]][2]  = slist[adr]->road[k].z;
              vect[lnk[i]][3]  = slist[adr]->road[k].dx;
              vect[lnk[i]][4]  = slist[adr]->road[k].dy;
              vect[lnk[i]][5]  = slist[adr]->road[k].dz;
              vect[lnk[i]][6]  = slist[adr]->road[k].ntof;
              vect[lnk[i]][7]  = slist[adr]->road[k].nu;
              vect[lnk[i]][8]  = slist[adr]->road[k].nv;
              vect[lnk[i]][9]  = slist[adr]->road[k].nw;
              vect[lnk[i]][10] = slist[adr]->road[k].charge;
#endif
              lnk[i] ++;
            }
          }
        }
      }

      /* write file */

      hnam = "DAXI";
      format = "B08";
      ncol = lnk[ister];
      nrow = 4;
      for(j=0; j<ncol; j++)
      {
        buf[j] = link1[j];
      }
      numdb = ister;
      fwhdr_(hnam,&numdb,&ncol,&nrow,format,strlen(hnam),strlen(format));
      fwdat_(&ncol,buf);
      printf(" %s %8d %6d %6d %6d    %s\n",hnam,numdb,ncol,nrow,max_lnk,format); fflush(stdout);

      hnam = "DSTE";
      for(j=0; j<ncol; j++)
      {
        buf[j] = link2[j];
      }
      numdb = ister;
      fwhdr_(hnam,&numdb,&ncol,&nrow,format,strlen(hnam),strlen(format));
      fwdat_(&ncol,buf);
      printf(" %s %8d %6d %6d %6d    %s\n",hnam,numdb,ncol,nrow,max_lnk,format); fflush(stdout);

      hnam = "DMOM";
      format = "B16";
      nrow = 2;
      for(j=0; j<ncol; j++)
      {
        for(l=0; l<2; l++)
        {
          buf2[j*2+l] = nlink3[j][l];
        }
      }
      numdb = ister;
      fwhdr_(hnam,&numdb,&ncol,&nrow,format,strlen(hnam),strlen(format));
      fwdat_(&ncol,buf);
      printf(" %s %8d %6d %6d %6d    %s\n",hnam,numdb,ncol,nrow,max_lnk,format); fflush(stdout);

#ifdef SIM
      hnam = "DSLP";
      format = "B08";
      ncol1 = ncol;
      nrow = 1;
      ncol = 0;
      for(j=0; j<ncol1; j++)
      {
        for(is=0; is<6; is++)
        {
	  /*printf("npat=%d -> ",nlink4[j].npat[is]);*/
          buf3[ncol++] = nlink4[j].npat[is];
          for(in=0; in<nlink4[j].npat[is]; in++)
          {
	    /*printf("%d ",nlink4[j].ipat[is][in]);*/
            buf3[ncol++] = nlink4[j].ipat[is][in];
          }
	  /*printf("\n");*/
        }
      }
      numdb = ister;
      fwhdr_(hnam,&numdb,&ncol,&nrow,format,strlen(hnam),strlen(format));
      ncol = (ncol + 3)/4; /* the number of words */
      fwdat_(&ncol,buf3);
      printf(" %s %8d %6d %6d %6d    %s\n",hnam,numdb,ncol,nrow,max_lnk,format); fflush(stdout);



      hnam = "DVEC";
      format = "I";
      ncol = ncol1;
      nrow = NVEC;
      for(j=0; j<ncol; j++)
      {
        for(l=0; l<nrow; l++)
        {
          bufv[j*nrow+l] = vect[j][l];
        }
      }
      numdb = ister;
      fwhdr_(hnam,&numdb,&ncol,&nrow,format,strlen(hnam),strlen(format));
      iv = ncol*nrow;
      fwdat_(&iv,bufv);
      printf(" %s %8d %6d %6d %6d    %s\n",hnam,numdb,ncol,nrow,max_lnk,format); fflush(stdout);

#endif

    } /* ister */

    free(buf);
#ifdef SIM
    free(bufv);
#endif
    free(buf3);

    fflush(stdout);
    printf("min=%3d %3d %3d   max=%3d %3d %3d\n",
            min[0],min[1],min[2],max[0],max[1],max[2]);
    fflush(stdout);


    fwend_(&ierr);
    if(ierr != 0) printf(" *** error *** \n");
    fweod_();

    fparm_("CLOSE UNIT=56",13);

    nroads = 0;
    for(j=1; j<=NS2; j++) nroads = nroads + lnk[j-1];
    printf("\n  Nroads =%8d\n",nroads);

    for(j=0; j<NS2; j++) {printf("%7d",lnk[j]); if(!((j+1)%10))printf("\n");}

    printf("\n"); fflush(stdout);


  return;
}




/*
    dictionary file format
    ----------------------

    name=DSEG format=I ncol=1+6*max_npat+2+NS2+1 nrow=1
    ---------------------------------------------------
    buf[0]                          = npat - former segments
    .............                     dictionary
    buf[max_npat*6]                   - obsolete
    buf[next]                       = 3 - magnet field type
    buf[next]                       = unix time
    buf[next..]                     = lnk[NS2] - the number of entries
    buf[last]                       = max_lnk - max entry number




    name=DAXI format=B08 ncol=lnk[] nrow=4
    buf[0..ncol] - axial patterns




    name=DSTE format=B08 ncol=lnk[] nrow=4
    buf[0..ncol] - stereo patterns




    name=DMOM format=B16 ncol=lnk[] nrow=2
    buf[0..ncol] - momentun & #entries




    name=DSLP format=B08
    ncol1 = ncol;
    nrow = 1;
    ncol = 0;
    for(j=0; j<ncol1; j++)
    {
      for(is=0; is<6; is++)
      {
        buf3[ncol++] = nlink4[j].npat[is];
        for(in=0; in<nlink4[j].npat[is]; in++)
        {
          buf3[ncol++] = nlink4[j].ipat[is][in];
        }
      }
    }




    name=DVEC format=I ncol=lnk[] nrow=NVEC
    buf[0] - 
    ................
    buf[NVEC] - 


*/













/******************************************************/
/* FOLLOWING FUNCTIONS - FOR CREATING DICTIONARY ONLY */

typedef struct segmdict
{
  int   ipat[6];
} SEGMdict;

static int segments_npat = 1;
static SEGMdict segments[MAX_NPAT];

int nwhit[NFOUND], nwall[NFOUND][NLAY], nsall[NFOUND][NLAY];
int numsegm[NFOUND], numsegm1[NFOUND];




/*
    is - superlayer# (0-5)
    iw3 - wire# in layer 3 (0-191 or less)
    ifound - index in nwhit[] and nwall[][NLAY] (from 0)
 */
void
prsegmlist_(int *iw3, short idtime[192][6], int *ifound, int *min_hit_seg)
{
  int isg, isum, la, iw, nw[6], skip, i, its;

  if(*ifound >= NFOUND) return;
  for(isg=0; isg<segments_npat; isg++)
  {
    isum = 0;
    for(la=0; la<NLAY; la++)
    {
     iw = (*iw3) + segments[isg].ipat[la];
     if(iw < 1) iw = 1;
     if(iw > 192) iw = 192;
      nw[la] = -ABS(iw);
      if(idtime[iw-1][la] > NO_HIT)
      {
        nw[la] = iw;
        isum++;
      }
    }
    if(isum < (*min_hit_seg)) continue;
    if(nw[1] < 0 && nw[2] < 0 && nw[3] < 0) continue;
    skip = 0;
    for(i=0; i<*ifound; i++)
    {
      its = 0;
      for(la=0; la<NLAY; la++)
      {
        if(nw[la]==nwall[i][la] && nsall[i][la]>0) its++;
      }
      if(its == isum && isum <= nwhit[i])
      {
        skip = 1;
        break;
      }
    }
    if(skip) continue;
    if(*ifound < NFOUND)
    {
      numsegm[*ifound]=isg;
      /*printf("segm(%d) -> ",isg);*/
      for(la=0; la<NLAY; la++)
      {
        nwall[*ifound][la] = ABS(nw[la]);
        nsall[*ifound][la] = 0;
        if(nw[la] > 0) nsall[*ifound][la] = 1;
	/*printf("%4d",nw[la]);*/
      }
      /*printf("\n");*/
      nwhit[*ifound] = isum;
     (*ifound)++;
    }
    else
    {
      return;
    }
  }

}


void
cleansegmlist_(int *max_hit_seg, int *min_hit_seg, short idtime[192][6],
     int *ifoundold, int *ifound, int *nsgm, int segm[nsgmx][12])
{
  int i,ii,ih,j,its,la,k,nwflag[NFOUND];

ii=0;

  for(i=*ifoundold; i<*ifound; i++) nwflag[i] = 0;
  for(ih = *max_hit_seg; ih >= *min_hit_seg; ih--)
  {
    if(ih == *max_hit_seg) /* all "max_hit_seg" considered as  good */
    {
      for(i=*ifoundold; i<*ifound; i++) if(nwhit[i] == ih) {nwflag[i] = 1; numsegm1[ii++] = numsegm[i];}
    }
    else
    {
      for(i=*ifoundold; i<*ifound; i++)
      {
        if(nwhit[i] == ih)
        {
          for(j=*ifoundold; j<*ifound; j++)
          {
            if(nwflag[j] > 0)
            {
              its = 0;
              for(la=0; la<NLAY; la++)
                if(nsall[i][la] > 0 && nwall[i][la] == nwall[j][la]) its++;
              if(its >= ih) goto a11; /* if we have segment better or the same - skip */
            }
          }
          nwflag[i] = 1; numsegm1[ii++] = numsegm[i];
        }
a11:
;
      }
    }

    /* fill segm[][] with good segments */
    for(i=*ifoundold; i<*ifound; i++)
    {
      if(nwflag[i] > 0 && nwhit[i] == ih)
      {
        if(*nsgm >= nsgmx) goto a10;
        k = 0;
        for(la=0; la<NLAY; la++)
        {
          if(nsall[i][la] > 0)
            segm[*nsgm][k++] = nwall[i][la];
          else
            segm[*nsgm][k++] = -nwall[i][la];
          segm[*nsgm][k++] = idtime[nwall[i][la]-1][la];
        }
        (*nsgm)++;
      }
a10:
;
    }
  }

/*
printf("numsegm-> ");
for(i=0; i<ii; i++) printf(" %d",numsegm1[i]);
printf("\n");
*/

  return;
}


void
prupdatesegm_(int nw[6])
{
  int la;

  if(ABS(nw[0]-nw[5]) > 14)
  {
    printf("prupdatesegm_ ERROR: ABS(nw[0]-nw[5]) = %d\n",ABS(nw[0]-nw[5]));
    return;
  }
  if(segments_npat >= MAX_NPAT)
  {
    printf("prupdatesegm_ ERROR: segments_npat = %d\n",segments_npat);
    return;
  }
  for(la=0; la<6; la++) segments[segments_npat].ipat[la] = nw[la] - nw[2];
  segments_npat ++;
  printf("prupdatesegm: [%3d] %3d %3d %3d %3d %3d %3d\n",
    segments_npat,nw[0],nw[1],nw[2],nw[3],nw[4],nw[5]);
  
  return;
}





