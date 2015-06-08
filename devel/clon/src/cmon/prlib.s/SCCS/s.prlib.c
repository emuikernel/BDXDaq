h10276
s 00454/00143/01562
d D 1.12 07/10/21 23:46:05 boiarino 13 12
c *** empty log message ***
e
s 00332/00015/01373
d D 1.11 03/12/16 22:17:54 boiarino 12 11
c .
e
s 00002/00000/01386
d D 1.10 03/04/17 16:50:27 boiarino 11 10
c *** empty log message ***
e
s 00003/00001/01383
d D 1.9 02/05/09 14:38:30 boiarino 10 9
c comment out some etNformat's - move it to tr_export.c
c 
e
s 00002/00002/01382
d D 1.8 02/05/08 18:46:27 boiarino 9 8
c comment out timer
c 
e
s 00000/00000/01384
d D 1.7 02/04/30 15:45:39 boiarino 8 7
c minor
c 
e
s 00011/00008/01373
d D 1.6 02/04/30 14:49:46 boiarino 7 6
c fix format readout in frhdr()
c 
e
s 00204/01355/01177
d D 1.5 02/04/30 14:22:12 boiarino 6 5
c new version based on new segment finding
c 
e
s 00000/00006/02532
d D 1.4 01/11/29 11:35:53 boiarino 5 4
c minor
c 
e
s 00000/00000/02538
d D 1.3 01/11/29 08:48:59 boiarino 4 3
c nothing
c 
e
s 00000/00000/02538
d D 1.2 01/11/28 23:00:56 boiarino 3 1
c nothing
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 18:57:06 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/prlib.s/prlib.c
e
s 02538/00000/00000
d D 1.1 01/11/19 18:57:05 boiarino 1 0
c date and time created 01/11/19 18:57:05 by boiarino
e
u
U
f e 0
t
T
I 1
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
I 11
#include <time.h>
E 11

D 6
#include "sglib.h"
E 6
#include "prlib.h"
I 6
#include "sglib.h"
E 6

I 11
#include "uttime.h"
E 11
#include "dclib.h"   /* just to get min and max wire number in GETDCDATA */
#include "sdageom.h" /* just to get min and max wire number in GETDCDATA */


I 12
D 13
/*#define DEBUG 1*/
E 13
I 13
#undef DEBUG
E 13

I 13

E 13
#define RADDEG 57.2957795130823209

E 12
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

D 6
static int segments_npat;
static int linksegments_npat;
static SEGMdict *segments, *segments_realloc;
static SEGMdict *linksegments;
static int grlookup[MAX_NPAT];
E 6

static float vk[7] = {VECT1, VECT2, VECT3, VECT4, VECT5, VECT6, VECT7};

/************************************************/
/********** FILE STRUCTURE PROCEDURES ***********/
/************************************************/

I 7
#define NFMT 15

E 7
#define OPENFILE(filnam) \
    strcpy(str,"OPEN INPUT UNIT=57 FILE=\""); \
    strcat(str,filnam); \
    strcat(str,"\""); \
    fparm_(str,strlen(str)); \
    frname_("INPUT",5); \
    for(i=0; i<strlen(kname); i++) kname[i] = ' '; \
    frkey_(kname,&numra,&numrb,&icl,&ierr,strlen(kname))

#define READFILE \
D 7
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),strlen(format)); \
E 7
I 7
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),NFMT); \
E 7
    frdat_(&max_lnk_local,link1,&ncol); \
    printf(" %s %8d %6d %6d %6d    %s\n",hname,numdb,ncol,nrow,max_lnk_local,format); fflush(stdout); \
D 7
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),strlen(format)); \
E 7
I 7
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),NFMT); \
E 7
    frdat_(&max_lnk_local,link2,&ncol); \
    printf(" %s %8d %6d %6d %6d    %s\n",hname,numdb,ncol,nrow,max_lnk_local,format); fflush(stdout); \
D 7
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),strlen(format)); \
E 7
I 7
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),NFMT); \
E 7
    frdat_(&max_lnk_local,buf,&ncol); \
    printf(" %s %8d %6d %6d %6d    %s\n",hname,numdb,ncol,nrow,max_lnk_local,format); fflush(stdout); \
    for(j=0; j<max_lnk_local; j++) \
      for(l=0; l<2; l++) \
        nlink3[l][j] = buf2[j*2+l]; \
D 7
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),strlen(format)); \
E 7
I 7
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),NFMT); \
E 7
    tmp = max_lnk*6*(MAXNPAT+1)/4; \
    frdat_(&tmp,buf3,&ncol); \
    printf(" %s %8d %6d %6d %6d    %s\n",hname,numdb,ncol,nrow,tmp,format); fflush(stdout); \
D 7
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),strlen(format)); \
E 7
I 7
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),NFMT); \
E 7
    /*printf("1: >%s< >%s< ncol=%d nrow=%d\n",hname,format,ncol,nrow); fflush(stdout);*/ \
    nwd = ncol * nrow; \
    frdat_(&tmp,link4,&nwd); \
    printf(" %s %8d %6d %6d %6d    %s\n",hname,numdb,ncol,nrow,tmp,format); fflush(stdout)

#define CLOSEFILE \
    fparm_("CLOSE UNIT=57",13); \
    printf("dictionary file closed.\n"); fflush(stdout)


D 6
/************************************************/
/********** SEGMENT FINDING PROCEDURES **********/
/************************************************/
E 6

D 6
static int NWL[6] = { 130, 142, 192, 192, 192, 192 }; /* the number of wires */

static int nshift = 0;
#define NGROUP 60 /* SDELAT' DINAMICHESKI - PO FAKTU ngroup !!! */
#define NSGM  0x3fff
static unsigned char nsegm[NGROUP][NSGM];
static DCsegm *segm[NGROUP][NSGM];
static int ngroup;
static int offset[NGROUP][6];
static int nwires[NGROUP][6];

int nwhit[NFOUND], nwall[NFOUND][NLAY], nsall[NFOUND][NLAY];
int numsegm[NFOUND], numsegm1[NFOUND]; /* just to print out */

static int grshift = 0;
int grsegm[NFOUND];

#define GETOFFSET(n) \
    { \
      int i_m,j_m; \
      if(n>=NGROUP) \
      { \
        printf("Max groups number %1d, you are trying %1d\n",NGROUP,n+1); \
      } \
      else \
      { \
        printf("\n\n"); \
        for(i_m=5; i_m>=0; i_m--) \
        { \
          nwires[n][i_m] = 0; \
          offset[n][i_m] = 0; \
          for(j_m=15; j_m>=0; j_m--) \
          { \
            if(a[n][i_m][j_m]) nwires[n][i_m] ++;  \
            if(a[n][i_m][j_m] && !a[n][i_m][j_m-1]) offset[n][i_m] = j_m-iw3;  \
            if(a[n][i_m][j_m]) printf("x"); \
            else printf("."); \
          } \
          printf("  nwires=%d   offset=%d\n",nwires[n][i_m],offset[n][i_m]); \
        } \
        n++; \
      } \
    }


/* 2 FOLLOWING FUNCTIONS - FOR CREATING DICTIONARY ONLY */

/*
    is - superlayer# (0-5)
    iw3 - wire# in layer 3 (0-191 or less)
    ifound - index in nwhit[] and nwall[][NLAY] (from 0)
 */
void
prsegmlist(int iw3, short idtime[192][6], int *ifound, int ngr, int min_hit_seg)
{
  int isg, isum, la, iw, nw[6], skip, i, its;

  if(*ifound >= NFOUND) return;
  for(isg=0; isg<segments_npat; isg++)
  {
    if(ngr >= 0 && ngr != grsegm[isg]) continue;
    isum = 0;
    for(la=0; la<NLAY; la++)
    {
     iw = iw3 + segments[isg].ipat[la];
     if(iw < 1) iw = 1;
     if(iw > 192) iw = 192; /* should check NWL[is] ... think about boundary affect here ... */
      nw[la] = -ABS(iw);
      if(idtime[iw-1][la] > NO_HIT)
      {
        nw[la] = iw;
        isum++;
      }
    }
    if(isum < min_hit_seg) continue;
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




static void
getIdtime(int OFFSET[6], int NW[6], int adr, int iw3, int idtim[16][6])
{
  int i, la, iw, bit;

  bzero((char *)idtim, 96*sizeof(int));
  bit = 1;

  for(la=0; la<NLAY; la++)
  {
    for(iw=(iw3+1)+OFFSET[la]; iw<(iw3+1)+OFFSET[la]+NW[la]; iw++)
    {
      if(adr & bit) idtim[iw-1][la] = 1;
      bit = bit<<1;
    }
  }

  /*
  if(adr == 0x1494)
  {
    printf("\nidtim: iw3=%d (adr=%08x)\n",iw3,adr);
    for(la=NLAY-1; la>=0; la--)
    {
      for(iw=(iw3+4); iw>(iw3-4); iw--) printf("%7d",idtim[iw][la]);
      printf("\n");
    }
    printf("end idtim\n");
  }
  */

  return;
}



void
prupdatesegm(int nw[6])
{
  int la;

  if(ABS(nw[0]-nw[5]) > 14) return;
  if(segments_npat >= MAX_NPAT) return;
  for(la=0; la<6; la++) segments[segments_npat].ipat[la] = nw[la] - nw[2];
  segments_npat ++;
  /*
printf("prupdatesegm: %3d %3d %3d %3d %3d %3d\n",nw[0],nw[1],nw[2],nw[3],nw[4],nw[5]);
  */
  return;
}


E 6
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
D 6
static int maxax[6] = {-200,-200,-200,-200,-200,-200};
static int minax[6] = { 200, 200, 200, 200, 200, 200};
static int maxst[6] = {-200,-200,-200,-200,-200,-200};
static int minst[6] = { 200, 200, 200, 200, 200, 200};
E 6



D 6
/* compress_uncompress 
E 6

I 6

/* <<<<<compress_uncompress>>>>> 

E 6
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


D 6
/*
high momentum:
minax:   3  -4 -24   0  -5  -5
maxax: 140  60  30  19   7   8
minst:   2  -4 -26 -19  -7  -8
maxst: 128  74  29   0   5   5
low momentum:
minax:   3   4 -58   0 -15 -17
maxax: 141  65  95  20  10  25
minst:   2   4 -68 -20 -10 -25
maxst: 129  78  95   0  15  17
*/

E 6
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




D 6
/* end_of_compress_uncompress */
E 6
I 6
/* <<<<<end_of_compress_uncompress>>>>> */
E 6



D 6

int
praxial(unsigned char ntest0, unsigned char ntest1, unsigned char ntest2)
{
  int adr;
  /*printf("iroad(axial) =%3d %3d %3d\n",ntest0, ntest1, ntest2);*/

  adr = GETAXADR1(ntest0,ntest1,ntest2);
  /*printf("adr=%08x\n",adr);*/
  if(alist[adr]) {/*printf("axial-yes\n");*/ return(adr);}
  /*printf("axial-no\n");*/
  return(0);
}

int
prstereo(unsigned char ntest0, unsigned char ntest1, unsigned char ntest2)
{
  /*printf("iroad(stereo)=%3d %3d %3d\n",ntest0, ntest1, ntest2);*/

  if(slist[GETSTADR1(ntest0,ntest1,ntest2)]) {/*printf("stereo-yes\n");*/ return(1);}
  /*printf("stereo-no\n");*/
  return(0);
}


#ifdef Linux
#define M21 0x0000ffff
#define M31 0x00ff00ff
#define M32 0x00ffff00
#else
#define M21 0xffff0000
#define M31 0xff00ff00
#define M32 0x00ffff00
#endif

#define TWOFROMTHREE(a,b) (((a&M21)==(b&M21)||(a&M31)==(b&M31)||(a&M32)==(b&M32)) ? 1:0)

/* that function will be called if 3stereo exist */
/* so we will looks for 3axial3stereo or 2axial3stereo here */

#ifdef SL5

int
praxialscan(int atest, int stest, int *p)
{
  DCroad *ptr;
  int adr, k;

  adr = GETAXADR(atest);
  if(!alist[adr]) /* 3axial does not exist - looks for 2axial */
  {
    adr = GETSTADR(stest); /* slist[adr]!=0 at that point, should be tested outside !!! */
    ptr = (DCroad *) slist[adr]->road;
    for(k=0; k<slist[adr]->nroads; k++)
    {
      if(TWOFROMTHREE((ptr[k].path),atest))
      {
        *p  = MOM(ptr[k].p);
        return(2); /* found 5 superlayers */
      }
    }
    return(0); /* not found */
  }
  else /* at that point axial and stereo both exist, looks for partner */
  {
    unsigned char *ch;
    ch = (unsigned char *)&stest;
printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ard = %d stest=%08x (%3d %3d %3d)\n",adr,stest,ch[2],ch[1],ch[0]);
    ptr = (DCroad *) alist[adr]->road;
    for(k=0; k<alist[adr]->nroads; k++)
    {
      ch = (unsigned char *)&ptr[k];
      printf("ptr[%d].path=%08x (%3d %3d %3d)\n",k,ptr[k].path,ch[2],ch[1],ch[0]);
/* !!!!!!!!!!!!!!!!!! ifndef Linux: .path !!!!!!!!!!!!!!!!!!!! */
      if(ptr[k].path == stest)
      {
        *p  = MOM(ptr[k].p);
        return(1); /* found */
      }
    }
    /* !!! ??? try 2from3 here ??? !!! */
  }

  return(0); /* not found */
}


/* that function will be called if 3axial exist AND 3stereo not exist */
/* so we will looks for 3axial2stereo here */


int
prstereoscan(int atest, int stest, int *p)
{
  DCroad *ptr;
  int adr, k;

  adr = GETAXADR(atest);
  ptr = (DCroad *) alist[adr]->road;
  for(k=0; k<alist[adr]->nroads; k++)
  {
    if(TWOFROMTHREE((ptr[k].path),stest))
    {
      *p  = MOM(ptr[k].p);
      return(2); /* found 5 superlayers */
    }
  }

  return(0); /* not found */
}

#endif


#define MINMAX \
if(maxax[0] <           nw[1][2] ) maxax[0] = nw[1][2]; \
if(maxax[1] < (nw[2][2]-nw[1][2])) maxax[1] = nw[2][2]-nw[1][2]; \
if(maxax[2] < (nw[4][2]-nw[2][2])) maxax[2] = nw[4][2]-nw[2][2]; \
if(maxax[3] < (nw[1][2]-nw[0][2])) maxax[3] = nw[1][2]-nw[0][2]; \
if(maxax[4] < (nw[2][2]-nw[3][2])) maxax[4] = nw[2][2]-nw[3][2]; \
if(maxax[5] < (nw[4][2]-nw[5][2])) maxax[5] = nw[4][2]-nw[5][2]; \
if(minax[0] >           nw[1][2] ) minax[0] = nw[1][2]; \
if(minax[1] > (nw[2][2]-nw[1][2])) minax[1] = nw[2][2]-nw[1][2]; \
if(minax[2] > (nw[4][2]-nw[2][2])) minax[2] = nw[4][2]-nw[2][2]; \
if(minax[3] > (nw[1][2]-nw[0][2])) minax[3] = nw[1][2]-nw[0][2]; \
if(minax[4] > (nw[2][2]-nw[3][2])) minax[4] = nw[2][2]-nw[3][2]; \
if(minax[5] > (nw[4][2]-nw[5][2])) minax[5] = nw[4][2]-nw[5][2]; \
if(maxst[0] <           nw[0][2] ) maxst[0] = nw[0][2]; \
if(maxst[1] < (nw[3][2]-nw[0][2])) maxst[1] = nw[3][2]-nw[0][2]; \
if(maxst[2] < (nw[5][2]-nw[3][2])) maxst[2] = nw[5][2]-nw[3][2]; \
if(maxst[3] < (nw[0][2]-nw[1][2])) maxst[3] = nw[0][2]-nw[1][2]; \
if(maxst[4] < (nw[3][2]-nw[2][2])) maxst[4] = nw[3][2]-nw[2][2]; \
if(maxst[5] < (nw[5][2]-nw[4][2])) maxst[5] = nw[5][2]-nw[4][2]; \
if(minst[0] >           nw[0][2] ) minst[0] = nw[0][2]; \
if(minst[1] > (nw[3][2]-nw[0][2])) minst[1] = nw[3][2]-nw[0][2]; \
if(minst[2] > (nw[5][2]-nw[3][2])) minst[2] = nw[5][2]-nw[3][2]; \
if(minst[3] > (nw[0][2]-nw[1][2])) minst[3] = nw[0][2]-nw[1][2]; \
if(minst[4] > (nw[3][2]-nw[2][2])) minst[4] = nw[3][2]-nw[2][2]; \
if(minst[5] > (nw[5][2]-nw[4][2])) minst[5] = nw[5][2]-nw[4][2]



#define UPDATELINKSEGM \
{ \
  int la,i,is,skip,skip1; \
  for(is=0; is<6; is++) \
  { \
    /*printf("super=%d  ",is+1); \
    printf("nw=%3d %3d %3d %3d %3d %3d\n",nw[is][0],nw[is][1], \
    nw[is][2],nw[is][3],nw[is][4],nw[is][5]);*/ \
    if(linksegments_npat < MAX_NPAT) \
    { \
      skip=-1; \
      for(i=0; i<linksegments_npat; i++) \
      { \
        if((nw[is][0]-nw[is][2]) == linksegments[i].ipat[0] && \
           (nw[is][1]-nw[is][2]) == linksegments[i].ipat[1] && \
           (nw[is][3]-nw[is][2]) == linksegments[i].ipat[3] && \
           (nw[is][4]-nw[is][2]) == linksegments[i].ipat[4] && \
           (nw[is][5]-nw[is][2]) == linksegments[i].ipat[5]) skip=i; \
      } \
      if(skip<0) \
      { \
        for(la=0; la<6; la++) \
          linksegments[linksegments_npat].ipat[la]=nw[is][la]-nw[is][2]; \
        skip=linksegments_npat ++; \
      } \
      /* update dictionary */ \
      if(ptr[k].npat[is]<MAXNPAT) \
      { \
        skip1=-1; \
        for(i=0; i<ptr[k].npat[is]; i++) \
        { \
          if(ptr[k].ipat[is][i] == skip) skip1=1; \
        } \
        if(skip1<0) \
        { \
	  ptr[k].ipat[is][ptr[k].npat[is]] = skip; \
          ptr[k].npat[is] ++; \
if(maxnpat < ptr[k].npat[is]) maxnpat = ptr[k].npat[is]; \
	  /*printf("maxnpat=%d\n",maxnpat); \
	  printf("ptr[k].npat[%1d]=%d\n",is,ptr[k].npat[is]); \
	  for(i=0; i<ptr[k].npat[is]; i++) printf(" %d",ptr[k].ipat[is][i]); \
          printf("\n");*/ \
        } \
      } \
    } \
  } \
}





E 6
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
D 6
  static int maxnpat=0;
  /*
static int uuu=0;
  */
E 6

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
D 13
      {
	/*
	printf("update no. %5d, %8x %8x\n",uuu++,atest,stest);
D 6
        printf("  old: p,x,y,z,dx,dy,dz=%d %d %d %d %d %d %d\n",ptr[k].u.s.p,ptr[k].x,ptr[k].y,ptr[k].z,ptr[k].dx,ptr[k].dy,ptr[k].dz);
E 6
I 6
        printf("  old: p,x,y,z,dx,dy,dz=%d %d %d %d %d %d %d\n",ptr[k].psim,ptr[k].x,ptr[k].y,ptr[k].z,ptr[k].dx,ptr[k].dy,ptr[k].dz);
E 6
        printf("  new: p,x,y,z,dx,dy,dz=%d %f %f %f %f %f %f\n",pin,vect1[0],vect1[1],vect1[2],vect1[3],vect1[4],vect1[5]);
	*/
E 13
I 13
      {		
E 13
        /* update existing stereo entry */
D 6
        ptr[k].u.s.p = (int)( (float)(ptr[k].u.s.p*ptr[k].u.s.np + pin)/(float)(ptr[k].u.s.np+1) );
E 6
#ifdef SIM
I 13
		/*
printf("update: %5d, %8x %8x\n",uuu++,atest,stest);
printf("  old: p,x,y,z,dx,dy,dz=%d %d %d %d %d %d %d\n",
  ptr[k].psim,ptr[k].x,ptr[k].y,ptr[k].z,ptr[k].dx,ptr[k].dy,ptr[k].dz);
printf("  new: p,x,y,z,dx,dy,dz=%d %f %f %f %f %f %f\n",
  pin,vect1[0],vect1[1],vect1[2],vect1[3],vect1[4],vect1[5]);
		*/
E 13
D 6
        ptr[k].x  = ((float)ptr[k].x  * (float)ptr[k].u.s.np + vect1[0]) / (float)(ptr[k].u.s.np+1);
        ptr[k].y  = ((float)ptr[k].y  * (float)ptr[k].u.s.np + vect1[1]) / (float)(ptr[k].u.s.np+1);
        ptr[k].z  = ((float)ptr[k].z  * (float)ptr[k].u.s.np + vect1[2]) / (float)(ptr[k].u.s.np+1);
        ptr[k].dx = ((float)ptr[k].dx * (float)ptr[k].u.s.np + vect1[3]) / (float)(ptr[k].u.s.np+1);
        ptr[k].dy = ((float)ptr[k].dy * (float)ptr[k].u.s.np + vect1[4]) / (float)(ptr[k].u.s.np+1);
        ptr[k].dz = ((float)ptr[k].dz * (float)ptr[k].u.s.np + vect1[5]) / (float)(ptr[k].u.s.np+1);
E 6
I 6
        ptr[k].psim = (int)( (float)(ptr[k].psim*ptr[k].np + pin)
                    /(float)(ptr[k].np+1) );
        ptr[k].x  = ((float)ptr[k].x  * (float)ptr[k].np + vect1[0]) / (float)(ptr[k].np+1);
        ptr[k].y  = ((float)ptr[k].y  * (float)ptr[k].np + vect1[1]) / (float)(ptr[k].np+1);
        ptr[k].z  = ((float)ptr[k].z  * (float)ptr[k].np + vect1[2]) / (float)(ptr[k].np+1);
        ptr[k].dx = ((float)ptr[k].dx * (float)ptr[k].np + vect1[3]) / (float)(ptr[k].np+1);
        ptr[k].dy = ((float)ptr[k].dy * (float)ptr[k].np + vect1[4]) / (float)(ptr[k].np+1);
        ptr[k].dz = ((float)ptr[k].dz * (float)ptr[k].np + vect1[5]) / (float)(ptr[k].np+1);
E 6

D 6
        ptr[k].ntof = ((float)ptr[k].ntof * (float)ptr[k].u.s.np + (float)ntof) / (float)(ptr[k].u.s.np+1);
        ptr[k].nu = ((float)ptr[k].nu * (float)ptr[k].u.s.np + (float)ecdigi2[0]) / (float)(ptr[k].u.s.np+1);
        ptr[k].nv = ((float)ptr[k].nv * (float)ptr[k].u.s.np + (float)ecdigi2[1]) / (float)(ptr[k].u.s.np+1);
        ptr[k].nw = ((float)ptr[k].nw * (float)ptr[k].u.s.np + (float)ecdigi2[2]) / (float)(ptr[k].u.s.np+1);
#endif
        ptr[k].u.s.np++;
E 6
I 6
        ptr[k].ntof = ((float)ptr[k].ntof * (float)ptr[k].np + (float)ntof) / (float)(ptr[k].np+1);
        ptr[k].nu = ((float)ptr[k].nu * (float)ptr[k].np + (float)ecdigi2[0]) / (float)(ptr[k].np+1);
        ptr[k].nv = ((float)ptr[k].nv * (float)ptr[k].np + (float)ecdigi2[1]) / (float)(ptr[k].np+1);
        ptr[k].nw = ((float)ptr[k].nw * (float)ptr[k].np + (float)ecdigi2[2]) / (float)(ptr[k].np+1);
        ptr[k].np++;
E 6

D 6
        *p = (float)ptr[k].u.s.p / vk[6];

        MINMAX;
#ifdef SIM
        UPDATELINKSEGM;
E 6
I 6
        *p = (float)ptr[k].psim / vk[6];
E 6
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
D 6
    slist[adr]->road[k].u.s.p = pin;
E 6
#ifdef SIM
I 13

	/*
printf("new axial entry: %8x %8x\n",atest,stest);
printf("  new: p,x,y,z,dx,dy,dz=%d %f %f %f %f %f %f\n",
  pin,vect1[0],vect1[1],vect1[2],vect1[3],vect1[4],vect1[5]);
	*/

E 13
I 6
    slist[adr]->road[k].psim = pin;
E 6
    slist[adr]->road[k].x  = vect1[0];
    slist[adr]->road[k].y  = vect1[1];
    slist[adr]->road[k].z  = vect1[2];
    slist[adr]->road[k].dx = vect1[3];
    slist[adr]->road[k].dy = vect1[4];
    slist[adr]->road[k].dz = vect1[5];

I 6
    slist[adr]->road[k].charge = charge;
E 6
    slist[adr]->road[k].ntof = ntof;
D 6
    slist[adr]->road[k].u.r.charge = charge;
E 6
    slist[adr]->road[k].nu = ecdigi2[0];
    slist[adr]->road[k].nv = ecdigi2[1];
    slist[adr]->road[k].nw = ecdigi2[2];
D 6
#endif
    slist[adr]->road[k].u.s.np = 1;
#ifdef SIM
E 6
I 6
    slist[adr]->road[k].np = 1;
E 6
    for(is=0; is<6; is++) slist[adr]->road[k].npat[is] = 0;
#endif
    slist[adr]->nroads ++;

    *p = (float)pin / vk[6];

D 6
    MINMAX;
#ifdef SIM
    ptr = (DCroad *) slist[adr]->road;
    UPDATELINKSEGM;
#endif

E 6
    return(1);
  }
  else    /* stereo not found - create new stereo entry */
  {
    slist[adr] = (DCaxial *) malloc(sizeof(DCaxial));
    if(!slist[adr]) {printf("prlib: ERROR in malloc\n"); exit(1);}
    slist[adr]->nroads = 1;
    slist[adr]->road = (DCroad *) malloc(NALLOC * sizeof(DCroad));

    slist[adr]->road[0].path = atest;
D 6
    slist[adr]->road[0].u.s.p = pin;
E 6
#ifdef SIM
I 13
	/*
printf("new stereo entry: %8x %8x\n",atest,stest);
printf("  new: p,x,y,z,dx,dy,dz=%d %f %f %f %f %f %f\n",
  pin,vect1[0],vect1[1],vect1[2],vect1[3],vect1[4],vect1[5]);
	*/
E 13
I 6
    slist[adr]->road[0].psim = pin;
E 6
    slist[adr]->road[0].x  = vect1[0];
    slist[adr]->road[0].y  = vect1[1];
    slist[adr]->road[0].z  = vect1[2];
    slist[adr]->road[0].dx = vect1[3];
    slist[adr]->road[0].dy = vect1[4];
    slist[adr]->road[0].dz = vect1[5];

I 6
    slist[adr]->road[0].charge = charge;
E 6
    slist[adr]->road[0].ntof = ntof;
D 6
    slist[adr]->road[0].u.r.charge = charge;
E 6
    slist[adr]->road[0].nu = ecdigi2[0];
    slist[adr]->road[0].nv = ecdigi2[1];
    slist[adr]->road[0].nw = ecdigi2[2];
D 6
#endif
    slist[adr]->road[0].u.s.np = 1;
#ifdef SIM
E 6
I 6
    slist[adr]->road[0].np = 1;
E 6
    for(is=0; is<6; is++) slist[adr]->road[0].npat[is] = 0;
#endif

    *p = (float)pin / vk[6];

D 6
    MINMAX;
#ifdef SIM
    k=0;
    ptr = (DCroad *) slist[adr]->road;
    UPDATELINKSEGM;
#endif

E 6
    return(1);
  }

}



I 13
/* look for 3 axial (ax_st = 0) or 3 stereo (ax_st = 1) combinations,
trying to find not only exact match in dictionary but similar roads
as well; 'ir1', 'ir2' and 'ir3' are deltas for search */
E 13

I 13
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
E 13

I 13

E 13
D 6

static int
isegmsort(PRSEGMENT *a, PRSEGMENT *b)
{
  if (a->nw > b->nw) return (1);
  if (a->nw < b->nw) return (-1);

  return (0);
}

#define LISTOFHITS(ntrack_) \
  ifound = nsegment_tmp; \
  for(la=0; la<NLAY; la++) \
  { \
    int k; \
    k = 0; \
    for(j=0; j<ifound; j++) \
    { \
      int iwire, new=1; \
      iwire = segment_tmp[j].iw[la]; \
      if(iwire <= 0) continue; \
      for(i=0; i<k; i++) \
      { \
        if(track[ntrack_].cluster[is].layer[la].iw[i] == iwire) new=0; \
      } \
      if(new) \
      { \
        track[ntrack_].cluster[is].layer[la].iw[k] = iwire; \
        track[ntrack_].cluster[is].layer[la].tdc[k] = segment_tmp[j].tdc[la]; \
        k++; \
        if(k >= NHL) break; \
      } \
    } \
    track[ntrack_].cluster[is].layer[la].nhit = k; \
/*printf("prlib==> is=%1d la=%1d nhit=%1d\n",is,la,k); fflush(stdout);*/ \
  }

#define CREATECLUSTER(ntrack_,iw3_) \
  /* create clusters */ \
  nsegment_old = nsegment_tmp; \
  adrsgm = 0; \
  bit = 1; \
  /*printf("\nidtime: iw3[%1d]=%d\n",is,iw3_); fflush(stdout); \
  for(la=NLAY-1; la>=0; la--) \
  { \
    for(iw=(iw3_+4); iw>(iw3_-4); iw--) printf("%7d",idtime[is][iw-1][la]); fflush(stdout); \
    printf("\n"); fflush(stdout); \
  } \
  printf("end idtime\n"); fflush(stdout);*/ \
  for(la=0; la<NLAY; la++) \
  { \
    for(iw=iw3_+offset[ngr][la]; iw<iw3_+offset[ngr][la]+nwires[ngr][la]; iw++) \
    { \
      if(iw>=1 && iw<=NWL[is] && ABS(idtime[is][iw-1][la]) > NO_HIT) adrsgm = adrsgm | bit; \
      bit = bit<<1; \
    } \
  } \
  /*printf("nsegm[%d][0x%08x]=%d\n",ngr,adrsgm,nsegm[ngr][adrsgm]); fflush(stdout);*/ \
  for(i=0; i<nsegm[ngr][adrsgm]; i++) \
  { \
    DCsegm *sgmptr; \
    PRSEGMENT *ptr; \
    int shift; \
    if(nsegment_tmp >= MAX_NPAT) break; \
    sgmptr = &segm[ngr][adrsgm][i]; \
    shift = (iw3_-1) - nshift; \
    ptr = &segment_tmp[nsegment_old + i]; \
    ptr->nw = 6-sgmptr->nwhit; \
    ptr->iw[0] = sgmptr->iw0 + shift; \
    if(!sgmptr->sign0) ptr->iw[0] = - ptr->iw[0]; \
    ptr->iw[1] = sgmptr->iw1 + shift; \
    if(!sgmptr->sign1) ptr->iw[1] = - ptr->iw[1]; \
    ptr->iw[2] = sgmptr->iw2 + shift; \
    if(!sgmptr->sign2) ptr->iw[2] = - ptr->iw[2]; \
    ptr->iw[3] = sgmptr->iw3 + shift; \
    if(!sgmptr->sign3) ptr->iw[3] = - ptr->iw[3]; \
    ptr->iw[4] = sgmptr->iw4 + shift; \
    if(!sgmptr->sign4) ptr->iw[4] = - ptr->iw[4]; \
    ptr->iw[5] = sgmptr->iw5 + shift; \
    if(!sgmptr->sign5) ptr->iw[5] = - ptr->iw[5]; \
    for(la=0; la<6; la++) ptr->tdc[la] = idtime[is][ptr->iw[la]-1][la]; \
    /*printf("\n ssegm %d:",i);  fflush(stdout); \
    printf(" %d - %4d(%5d) %4d(%5d) %4d(%5d) %4d(%5d) %4d(%5d) %4d(%5d)\n", \
           ptr->nw,ptr->iw[0],ptr->tdc[0],ptr->iw[1],ptr->tdc[1], \
           ptr->iw[2],ptr->tdc[2],ptr->iw[3],ptr->tdc[3], \
           ptr->iw[4],ptr->tdc[4],ptr->iw[5],ptr->tdc[5]); fflush(stdout);*/ \
    nsegment_tmp ++; \
    /*printf("track[%1d].cluster[%1d].nsegment_tmp=%d\n",ntrack_,is,nsegment_tmp); fflush(stdout);*/ \
  }








E 6
int
I 13
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
E 13
D 6
prscan(unsigned char cha0, unsigned char cha1, unsigned char cha2,
E 6
I 6
prroad(unsigned char cha0, unsigned char cha1, unsigned char cha2,
E 6
       unsigned char chs0, unsigned char chs1, unsigned char chs2,
D 6
       int sector, short idtime[6][192][6], int *ntrk, PRTRACK *track, SECTRK *sectrk)
E 6
I 6
       PRSEC *sgm, int icl[3], int sector, int *ntrk, PRTRACK *track)
E 6
{
  DCroad *ptr;
D 6
  PRSEGMENT *sgmptr;
  PRCLUSTER *clstptr;
  int adr, adrsgm, i, iv, j, la, k, is, in, ncoin, icoin, icoins[6], nsegment_old, flag;
  int ntrack, itr, take, ifound, path, tmp;
E 6
I 6
  int adr, k, is, ntrack, path, itmp, il, ih;
E 6
  unsigned char *ch;
I 6
  int ireg[6] = {0,0,1,1,2,2};
  int axst[6] = {1,0,1,0,1,0};
E 6

D 6
  int       nsegment_tmp;
  PRSEGMENT segment_tmp[MAX_NPAT];
E 6
I 6
D 13
  adr = GETAXADR1(cha0,cha1,cha2);
  if(!alist[adr]) return(0);
E 13
I 13
  /*sometimes have 255 - if nothing was found (search for '254' in sgutil.c)*/
  /*
  if(cha0>127) cha0=127;
  if(cha1>191) cha1=191;
  if(cha2>191) cha2=191;
  if(chs0>127) chs0=127;
  if(chs1>191) chs1=191;
  if(chs2>191) chs2=191;
  */
E 13
E 6

D 13
  adr = GETSTADR1(chs0,chs1,chs2);
  if(!slist[adr])
E 13
I 13
#ifdef DEBUG
  printf("++++++++++ sector=%d, AX= %d %d %d, ST= %d %d %d ++++++++++++++++\n",
    sector,cha0,cha1,cha2,chs0,chs1,chs2);
#endif

  /* try 3SL stereo */
  adr = prroad1(1,slist,&chs0,&chs1,&chs2,2,2,2);
  if(adr==0)
E 13
  {
D 13
    return(0);
E 13
I 13
    goto tryaxial; /* no 3SL stereo - goto tryaxial */
E 13
  }
D 13
  else
  {
D 6
    /*
printf("prscan: axial %3d %3d %3d  stereo %3d %3d %3d\n",cha0,cha1,cha2,chs0,chs1,chs2);
    */
    if(cha0 < slist[adr]->min[0]) return(0);
    if(cha0 > slist[adr]->max[0]) return(0);
    if(cha1 < slist[adr]->min[1]) return(0);
    if(cha1 > slist[adr]->max[1]) return(0);
    if(cha2 < slist[adr]->min[2]) return(0);
    if(cha2 > slist[adr]->max[2]) return(0);
    /*
printf("prscan: 1\n");
    */
E 6
I 6
    if(cha0 < slist[adr]->min[0]-1) return(0);
    if(cha0 > slist[adr]->max[0]+1) return(0);
    if(cha1 < slist[adr]->min[1]-1) return(0);
    if(cha1 > slist[adr]->max[1]+1) return(0);
    if(cha2 < slist[adr]->min[2]-1) return(0);
    if(cha2 > slist[adr]->max[2]+1) return(0);
E 13

D 13
	/*printf("prroad: proceed through boundary check\n");*/
E 13
I 13
  /********************************************/
  /* take stereo and check all axial partners */
E 13

E 6
D 13
    ptr = (DCroad *) slist[adr]->road;
    for(k=0; k<slist[adr]->nroads; k++)
E 13
I 13
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
E 13
    {
I 13
#ifdef DEBUG
      printf("prroad: ax %3d %3d %3d st %3d %3d %3d\n",
        cha0,cha1,cha2,chs0,chs1,chs2);fflush(stdout);
      printf("prroad: have something ...\n");fflush(stdout);
#endif
      RECORD_TRACK;
    }
  }
E 13

D 13
      path = ptr[k].path;
E 13
I 13
  /* 2 superlayers are required */
  ptr = (DCroad *) slist[adr]->road;
  for(k=0; k<slist[adr]->nroads; k++)
  {
    path = ptr[k].path;
E 13
#ifndef Linux
D 13
      path = path<<8;
E 13
I 13
    path = path<<8;
E 13
#endif
D 13
      ch = (unsigned char *)&path;
D 6

      /*
printf("prscan: axial from dictionary: %3d %3d %3d\n",ch[0],ch[1],ch[2]);
      */
      if(ch[0] == cha0 && ch[1] == cha1 && ch[2] == cha2)
E 6
I 6
	  /*
printf("prroad: axial from dictionary: %3d %3d %3d\n",ch[0],ch[1],ch[2]);fflush(stdout);
	  */
      if((ch[0]==cha0||ch[0]==cha0-1||ch[0]==cha0+1) &&
         (ch[1]==cha1||ch[1]==cha1-1||ch[1]==cha1+1) &&
         (ch[2]==cha2||ch[2]==cha2-1||ch[2]==cha2+1))
E 6
      {
D 6
	/*
printf("prscan: FOUND axial %3d %3d %3d  stereo %3d %3d %3d\n",cha0,cha1,cha2,chs0,chs1,chs2);
	*/
        /* is it old track ? */
E 6
I 6
/*
printf("prroad: ax %3d %3d %3d st %3d %3d %3d\n",
cha0,cha1,cha2,chs0,chs1,chs2);fflush(stdout);
*/
		/*
printf("prroad: have something ...\n");fflush(stdout);
		*/
E 6
        ntrack = *ntrk;
D 6
        for(itr = sectrk->ntrack_firstinsector; itr<ntrack; itr++)
E 6
I 6
        if(ntrack < (NTRACK-1))
E 6
        {
D 6
          /* exactly the same track ? */

          if(cha0 < track[itr].cluster[1].iwmin ||
             cha0 > track[itr].cluster[1].iwmax) goto a10;
          if(cha1 < track[itr].cluster[2].iwmin ||
             cha1 > track[itr].cluster[2].iwmax) goto a10;
          if(cha2 < track[itr].cluster[4].iwmin ||
             cha2 > track[itr].cluster[4].iwmax) goto a10;
          if(chs0 < track[itr].cluster[0].iwmin ||
             chs0 > track[itr].cluster[0].iwmax) goto a10;
          if(chs1 < track[itr].cluster[3].iwmin ||
             chs1 > track[itr].cluster[3].iwmax) goto a10;
          if(chs2 < track[itr].cluster[5].iwmin ||
             chs2 > track[itr].cluster[5].iwmax) goto a10;
          return(0);
a10:
          ;
        }
        /*printf("prscan: ncoin=%d -> have something ...\n",ncoin); fflush(stdout);*/


        for(is=0; is<6; is++)
        {
          int min, max, tmp, bit, iw;
          unsigned int ngr;

          /* extract ngr */

          if(is==0)      {ngr = ptr[k].u.r.gr0; min = max = chs0;}
          else if(is==1) {ngr = ptr[k].u.r.gr1; min = max = cha0;}
          else if(is==2) {ngr = ptr[k].u.r.gr2; min = max = cha1;}
          else if(is==3) {ngr = ptr[k].u.r.gr3; min = max = chs1;}
          else if(is==4) {ngr = ptr[k].u.r.gr4; min = max = cha2;}
          else if(is==5) {ngr = ptr[k].u.r.gr5; min = max = chs2;}
          /*printf(" is=%d ngr=%d\n",is,ngr);   fflush(stdout);*/

          flag = 0;
          min++;




          nsegment_tmp = 0;
          do
E 6
I 6
          for(is=0; is<6; is++)
E 6
          {
D 6
            min--;
            CREATECLUSTER((ntrack),(min));
            if(nsegm[ngr][adrsgm] > 0) flag = 1;
            else min++;
          } while(nsegm[ngr][adrsgm] > 0);
          do
          {
            max++;
            CREATECLUSTER((ntrack),(max));
            if(nsegm[ngr][adrsgm] > 0) flag = 1;
            else max--;
          } while(nsegm[ngr][adrsgm] > 0);
          if(!flag) return(0);
          /*qsort((void *)segment_tmp, nsegment_tmp, sizeof(PRSEGMENT),
            (int (*) (const void *, const void *))isegmsort);*/
          LISTOFHITS((ntrack));
E 6
I 6
            if(axst[is])
            {
              for(il=0; il<NLAY; il++)
              {
                itmp = sgm->rg[ireg[is]].ax[icl[ireg[is]]].la[il].nhit;
                for(ih=0; ih<itmp; ih++)
                {
                    track[ntrack].cluster[is].layer[il].iw[ih]
                      = sgm->rg[ireg[is]].ax[icl[ireg[is]]].la[il].iw[ih]+1;
                    track[ntrack].cluster[is].layer[il].tdc[ih]
                      = sgm->rg[ireg[is]].ax[icl[ireg[is]]].la[il].tdc[ih];
                }
                track[ntrack].cluster[is].layer[il].nhit = itmp;
              }
            }
            else
            {
              for(il=0; il<NLAY; il++)
              {
                itmp = sgm->rg[ireg[is]].st[icl[ireg[is]]].la[il].nhit;
                for(ih=0; ih<itmp; ih++)
                {
                    track[ntrack].cluster[is].layer[il].iw[ih]
                      = sgm->rg[ireg[is]].st[icl[ireg[is]]].la[il].iw[ih]+1;
                    track[ntrack].cluster[is].layer[il].tdc[ih]
                      = sgm->rg[ireg[is]].st[icl[ireg[is]]].la[il].tdc[ih];
                }
                track[ntrack].cluster[is].layer[il].nhit = itmp;
              }
            }
          }
E 13
I 13
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
E 13
E 6

D 6





          clstptr = &track[ntrack].cluster[is];
          clstptr->iwmin = min - 1;
          clstptr->iwmax = max + 1;
        }

        if(ntrack < (NTRACK-1))
        {
          track[ntrack].sector = sector;
          track[ntrack].p = MOM(ptr[k].p);
          track[ntrack].charge = ptr[k].u.r.charge;

          track[ntrack].ntof = ptr[k].ntof;
          track[ntrack].nu = ptr[k].nu;
          track[ntrack].nv = ptr[k].nv;
          track[ntrack].nw = ptr[k].nw;
E 6
I 6
D 13
          track[ntrack].sector  = sector;
          track[ntrack].p       = MOM(ptr[k].p);
          track[ntrack].charge  = ptr[k].charge;
          track[ntrack].ntof    = ptr[k].ntof;
          track[ntrack].nu      = ptr[k].nu;
          track[ntrack].nv      = ptr[k].nv;
          track[ntrack].nw      = ptr[k].nw;
E 6
          track[ntrack].vect[0] = (float)ptr[k].x / vk[0];
          track[ntrack].vect[1] = (float)ptr[k].y / vk[1];
          track[ntrack].vect[2] = (float)ptr[k].z / vk[2];
          track[ntrack].vect[3] = (float)ptr[k].dx / vk[3];
          track[ntrack].vect[4] = (float)ptr[k].dy / vk[4];
          track[ntrack].vect[5] = (float)ptr[k].dz / vk[5];
E 13

D 13
          track[ntrack].ntrk = 1;
D 6

          ntrack++;
          *ntrk=ntrack; 
          /*printf("prscan: NTRACK=%d\n",ntrack);  fflush(stdout);*/
E 6
I 6
          *ntrk = ++ntrack; 
E 6
        }
D 6
        /*printf("prscan: ntrack=%d\n",ntrack);*/
E 6
I 6
		/*
printf("prroad: ntrack=%d\n",ntrack);fflush(stdout);
		*/
E 6
        return(1);
E 13
I 13
  /* did not find 3SLstereo+3SL(2SL)axial, will try axial */
E 13

D 13
      }
E 13

I 13
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
E 13
    }
I 13
  }
E 13

I 13

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
E 13
  }

D 13
  /*printf("not found in prscan\n");  fflush(stdout);*/
E 13

I 13


E 13
  return(0);
}


D 6


/*
 prbos.c - filling output BOS bank

  input:  iw, ntrk, trk

CHECK bcopy(), DOING IN ONE MOVE, BUT IF ENDS IN THE MIDDLE OF WORD ? WHAT ABOUT FORMAT ???

*/

E 6
int
D 6
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

  return(ind);
}



int
E 6
prlib(int *jw, PRTRACK *track)
{
D 6
  /* DC parameters */
  const int NLinSL[6] = {4, 6, 6, 6, 6, 6};
  int dcndig[36], dcdigi[36][192][2];
  int i, j, k, n, sec, isec, is, itdc, il, ih, ind, ind1, nd, ifail;
  int icl, isg, found, adr, tmp, ntrack, level3[6], secbit[7];
  int icl1, icl2, icl3, icl4, icl5, icl6;
  int ihl1, ihl2, ihl3, ihl4, ihl5, ihl6;
  int nhit_in_sl,ifound,iw,isum,ilmin,la,min_hit_seg;
  int iw3, ncl[6], ncl_[6];
  short *i16;
  unsigned char ird[6][nclmx];
  short idtim[6][192][6];
  SECTRK sectrk;
E 6
I 6
  int i, isec, found, ntrack, icl0, icl1, icl2, ncl[3], icl[3];
  unsigned char ird[3][2][nclmx];
E 6

D 6
  /* cleanup after previous event */
  ntrack = 0;
E 6
I 6
  /* sector-based segment finding structure */
  int nsgm;
  PRSEC sgm;
E 6

D 6
  for(i=0; i<6; i++)
  {
    level3[i] = 1;
    secbit[i] = 1<<i;
  }
  if((ind1 = etNlink(jw,"TGBI",0)) > 0)
  {
    tmp = jw[ind1+4] & 0x3f;
    for (i=0; i<6; i++)
    {
	  if (tmp & secbit[i])
      {
        ;
      }
      else
	  {
        level3[i] = 0;
	  }
    }
    /*printf("==> %08x -> %1d %1d %1d %1d %1d %1d\n",tmp,
    level3[5],level3[4],level3[3],level3[2],level3[1],level3[0]);*/
  }





E 6
/*************************/
/* ????????????????????? */
/*************************/

D 12
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
E 12

D 12

E 12
  etNformat(jw,"ECPI","3I,6F");
  etNformat(jw,"DC1","I,F");
I 10
  etNformat(jw,"PATH","F");
  /*
E 10
I 6
  etNformat(jw,"HBLA","F");
  etNformat(jw,"HDPL","F");
  etNformat(jw,"TBLA","F");
  etNformat(jw,"TDPL","F");
  etNformat(jw,"HBTR","F");
  etNformat(jw,"TBTR","F");
E 6
D 5
  etNformat(jw,"HBLA","F");
  etNformat(jw,"HDPL","F");
  etNformat(jw,"TBLA","F");
  etNformat(jw,"TDPL","F");
  etNformat(jw,"HBTR","F");
  etNformat(jw,"TBTR","F");
E 5
D 10
  etNformat(jw,"PATH","F");
E 10
I 10
  */
E 10

/*************************/
/* ????????????????????? */
/*************************/

I 13
#ifdef DEBUG
  printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
#endif

E 13
D 6
  if(1)
E 6
I 6
  ntrack = 0;
  for(isec=1; isec<=6; isec++)
E 6
  {
I 13
#ifdef DEBUG
    printf("++++++++ SECTOR %d ++++++++++++++++++++++++\n",isec);
#endif
E 13
D 6
    for(isec=1; isec<=6; isec++)
E 6
I 6
    /* 1-cleaning only, 2-segm.find. only, 3-cleaning and segm.find. */
D 9
start_timer();
E 9
I 9
    /*start_timer();*/
E 9
D 12
    sglib(jw, isec, 3, &nsgm, &sgm); /* search for segments */
E 12
I 12
    sglib(jw, isec, 2, &nsgm, &sgm); /* search for segments */
E 12
D 9
stop_timer(1000);
E 9
I 9
    /*stop_timer(1000);*/
E 9
D 13
    if(nsgm == 0) continue;
E 13
I 13
    if(nsgm == 0)
    {
#ifdef DEBUG
      printf("NO SEGMENTS\n");
#endif
      continue;
	}
E 13
    sgprint(&sgm);
    sgroad(&sgm,ncl,ird);

I 13
#ifdef DEBUG
    printf("GOTO PRROAD: %d %d %d\n",ncl[0],ncl[1],ncl[2]);
#endif
E 13
    for(icl0 = 0; icl0 < ncl[0]; icl0++)
E 6
    {
D 6
      sglib(jw, isec, &ifail); /* noise reduction from Dave Haddle */
      if(ifail == 0) continue;
E 6
I 6
      for(icl1 = 0; icl1 < ncl[1]; icl1++)
      {
        for(icl2 = 0; icl2 < ncl[2]; icl2++)
        {
          icl[0] = icl0;
          icl[1] = icl1;
          icl[2] = icl2;
I 12
		  /*
ird[0][0][icl0]+=7;
ird[0][1][icl0]+=7;
		  */
E 12
          found = prroad(ird[0][0][icl0],ird[1][0][icl1],ird[2][0][icl2],
                         ird[0][1][icl0],ird[1][1][icl1],ird[2][1][icl2],
                         &sgm,icl,isec,&ntrack,track);
I 13
/*
E 13
I 12
#ifdef DEBUG
printf("isec=%d, road: %3d %3d %3d %3d %3d %3d, found=%d\n",isec,
ird[0][0][icl0],ird[1][0][icl1],ird[2][0][icl2],
ird[0][1][icl0],ird[1][1][icl1],ird[2][1][icl2],found);
#endif
I 13
*/
E 13
E 12
        }
      }
    }
  }
E 6

I 12
D 13
#ifdef DEBUG
E 13
I 13
/*#ifdef DEBUG*/
E 13
  /* print EVNT bank if exist (for cooked files only */
  {
    int i, j, ind, nrow, ncol, ind1, nrow1, ncol1, dcstat, sctr, isec, *iptr;
    int sctr_sec, sctr_ptr, ind2, ncol2, nrow2, ind3, nrow3, ncol3;
    int itr_sec, sec, itr, iw[34];
    float *fptr, phi, theta;

    ind2 = etNlink(jw,"HBTR",0);
D 13
	printf("================= ind2=%d\n",ind2);
E 13
I 13
	/*printf("================= ind2=%d\n",ind2);*/
E 13

    if((ind = etNlink(jw,"EVNT",0)) > 0)
    {
      nrow = etNrow(jw,ind);
      ncol = etNcol(jw,ind);
D 13
      printf("\nEVNT: nrow=%d, ncol=%d\n",nrow,ncol);
E 13
I 13
      /*printf("\nEVNT: nrow=%d, ncol=%d\n",nrow,ncol);*/
E 13
      for(i=0; i<nrow; i++)
      {
        iptr = (int *)&jw[ind+i*ncol];
        fptr = (float *)&jw[ind+i*ncol];
        dcstat = iptr[11];
I 13
if(iptr[0]!=11) goto donotprint;
E 13
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

D 13
			  printf("sctr_ptr=%d\n",sctr_ptr);
E 13
I 13
			printf("sctr_ptr=%d\n",sctr_ptr);
E 13
            if(sctr_ptr > 0)
			{
			  printf("sctr_ptr=%d\n",sctr_ptr);
              ind2 = etNlink(jw,"HBTR",0);
D 13
			  printf("ind2=%d\n",ind2);
E 13
I 13
	      /*printf("ind2=%d\n",ind2);*/
E 13
              if(ind2 > 0)
              {
                nrow2 = etNrow(jw,ind2);
                ncol2 = etNcol(jw,ind2);
                itr_sec = jw[ind2+(sctr_ptr-1)*ncol2+8];
                sec = itr_sec / 100;
                itr = itr_sec - sec * 100;
D 13
                printf("\nHBTR: nrow=%d, ncol=%d, itr_sec=%d (%d %d)\n",
                  nrow2,ncol2,itr_sec,sec,itr);
E 13
I 13
                /*printf("\nHBTR: nrow=%d, ncol=%d, itr_sec=%d (%d %d)\n",
                  nrow2,ncol2,itr_sec,sec,itr);*/
E 13

                if(itr > 0 && sec > 0 && sec < 7)
			    {
                  if((ind3 = etNlink(jw,"HBLA",sec)) > 0)
                  {
                    nrow3 = etNrow(jw,ind3);
                    ncol3 = etNcol(jw,ind3);
D 13
                    printf("ncol1=%d nrow3=%d\n",ncol3,nrow3);
E 13
I 13
                    /*printf("ncol1=%d nrow3=%d\n",ncol3,nrow3);*/
E 13
                    for(j=0; j<34; j++)
D 13
					{
E 13
I 13
                    {
E 13
                      iw[j] = jw[ind3 + (itr-1)*ncol3*34 + 10 + j*16];
D 13
                      printf("iw[%2d] = %d\n",j,iw[j]);
				    }

			      }
			    }

		      }
			}


E 13
I 13
                      /*printf("iw[%2d] = %d\n",j,iw[j]);*/
                    }
                  }
                }
              }
            }
E 13
          }
D 13

		}


E 13
I 13
        }
E 13
      }
D 13
      printf("\n");
E 13
    }
I 13
donotprint:
    ;
E 13
  }
D 13
#endif
E 13
I 13
  /*
  printf("============================================================\n");
  printf("============================================================\n");
  printf("============================================================\n");
  printf("============================================================\n");
  printf("============================================================\n");
  */
/*#endif*/
E 13


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

E 12
D 6
      /*if(!level3[isec-1]) continue;*/
E 6
I 6
  return(ntrack);
}
E 6

D 6
      GETDCDATA;
E 6

D 6
      if(nd <= 0) continue;
      if(nd > 200) continue; /* check 'dcndig[]' instead of 'nd' !!! */
E 6

D 6
      sectrk.ntrack_firstinsector = ntrack;
      for(is=0; is<6; is++)
      {
        ncl_[is] = 0;
        ncl[is] = 0;
        min_hit_seg = MINHITSEGM;
        memset((char *)&idtim[is][0][0],NO_HIT,NWL[5]*NLAY*2);
        nhit_in_sl = 0;
E 6

D 6
        ilmin = is*NLAY;
        for(la = 0; la<NLAY; la++)
        {
          il = ilmin + la;
          if(dcndig[il] == 0) continue;
          nhit_in_sl ++;
          for(iw3 = 0; iw3<dcndig[il]; iw3++)
          {
            iw = dcdigi[il][iw3][0]-1;
            idtim[is][iw][la] = dcdigi[il][iw3][1];
          }
        }
E 6

D 6
        if(nhit_in_sl < min_hit_seg) goto exit;
        for(iw3=0; iw3<NWL[is]; iw3++)
        {
          if(idtim[is][iw3][0]==NO_HIT&&idtim[is][iw3][1]==NO_HIT &&
             idtim[is][iw3][2]==NO_HIT&&idtim[is][iw3][3]==NO_HIT &&
             idtim[is][iw3][4]==NO_HIT&&idtim[is][iw3][5]==NO_HIT) continue;
E 6

D 6
          tmp = 0;
          for(i=0; i<6; i++) if(idtim[is][iw3][i]>NO_HIT) tmp += 1;
          for(i=0; i<6; i++) if(idtim[is][iw3+1][i]>NO_HIT) tmp += 1;
          if(tmp < 2) continue;
	  
          ncl_[is] = 1;
          ird[is][ncl[is]] = iw3+1;
          ncl[is] ++;
          if(ncl[is] >= nclmx) {/*printf("prlink6: exceed nclmx\n");*/ break;}
        }
        if(ncl_[is] == 0) goto exit;
      }
E 6

D 6
      for(icl2 = 0; icl2 < ncl[1]; icl2++)
      {
        for(icl3 = 0; icl3 < ncl[2]; icl3++)
        {
          for(icl5 = 0; icl5 < ncl[4]; icl5++)
          {
/*
printf("prlib: axial -> %3d %3d %3d\n",ird[1][icl2],ird[2][icl3],ird[4][icl5]);
*/
            if((adr=praxial(ird[1][icl2],ird[2][icl3],ird[4][icl5])) != 0)
            {
E 6

D 6
              for(icl1 = 0; icl1 < ncl[0]; icl1++)
              {
								
                if(ird[0][icl1] > alist[adr]->max[0]) continue;
                if(ird[0][icl1] < alist[adr]->min[0]) continue;
				
                for(icl4 = 0; icl4 < ncl[3]; icl4++)
                {
				  
                  if(ird[3][icl4] > alist[adr]->max[1]) continue;
                  if(ird[3][icl4] < alist[adr]->min[1]) continue;
				  
                  for(icl6 = 0; icl6 < ncl[5]; icl6++)
                  {
					
                    if(ird[5][icl6] > alist[adr]->max[2]) continue;
                    if(ird[5][icl6] < alist[adr]->min[2]) continue;
					
		    /*
printf("prlib: stereo-> %3d %3d %3d\n",ird[0][icl1],ird[3][icl4],ird[5][icl6]);
		    */
E 6

D 6
                    found = prscan(ird[1][icl2],ird[2][icl3],ird[4][icl5],
                                   ird[0][icl1],ird[3][icl4],ird[5][icl6],
                                   isec,idtim,&ntrack,track,&sectrk);
		    /*
if(found) printf("prlib: stereo-> %3d %3d %3d\n",ird[0][icl1],ird[3][icl4],ird[5][icl6]);
		    */
E 6
I 6










E 6
/*
D 6
if(found) if(ABS(ird[5][icl6]) < alist[adr]->min[2])
{
  printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  printf("\n%d < %d\n",ABS(ird[5][icl6]),alist[adr]->min[2]);
  printf("prlib: axial -> %3d %3d %3d\n",ird[1][icl2],ird[2][icl3],ird[4][icl5]);
  printf("prlib: stereo-> %3d %3d %3d\n",ird[0][icl1],ird[3][icl4],ird[5][icl6]);
  printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  exit(0);
}
*/
                  }
                }
              }
E 6
I 6
 prbos.c - filling output BOS bank
E 6

D 6
            }
E 6
I 6
  input:  iw, ntrk, trk
E 6

D 6
          }
        }
      }
E 6
I 6
CHECK bcopy(), DOING IN ONE MOVE, BUT IF ENDS IN THE MIDDLE OF WORD ? WHAT ABOUT FORMAT ???
E 6

D 6
exit:
      ;
    }
  }
E 6
I 6
*/
E 6

D 6
  return(ntrack);
}
E 6
I 6
int
prbos(int *jw, int *ntrk, PRTRACK *trk)
{
  PRTRACK *trkout;
  int nr, ncol, ind, i;
E 6

I 6
  nr = 0;
  ncol = (sizeof(PRTRACK)+3)/4;
  ind = etNcreate(jw,"PATH",nr,ncol,*ntrk);
  /*printf("prbos: nr=%d ncol=%d nrow=%d -> ind=%d\n",nr,ncol,*ntrk,ind);*/
  if(ind <= 0) {/*printf("prbos: cannot create PATH bank: %d %d -> return\n",ncol,*ntrk);*/ return(-1);}
  trkout = (PRTRACK *)&jw[ind];
  bcopy((char *)trk, (char *)trkout, (*ntrk)*sizeof(PRTRACK));
E 6

I 6
  return(ind);
}
E 6

I 6

E 6
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

D 6
int
GETDIFF(int axial, int stereo)
{
  char *b1, *b2;
  int i, diff[3];
E 6

D 6
  b1 = (char *)&axial;
  b2 = (char *)&stereo;
E 6

D 6
  for(i=0; i<3; i++) diff[i] = (b1[i] - b2[i]) - min[i];
  /*
  printf("dif= %2d %2d %2d - %2d %2d %2d = %2d %2d %2d (%2d %2d %2d)\n",
    b1[0],b1[1],b1[2],b2[0],b2[1],b2[2],b1[0]-b2[0],b1[1]-b2[1],b1[2]-b2[2],
    diff[0],diff[1],diff[2]);
  */
  return((diff[0] + (diff[1]<<4) + (diff[2]<<7))&0x3ff);
}
E 6

D 6
#define GETNGR \
  /*printf("is=%d  npat=%d  segms -> ",is,nlink4[j].npat[is]);*/ \
  max = -99; \
  min = 100; \
  for(in=0; in<nlink4[j].npat[is]; in++) \
  { \
    tmp = grsegm[nlink4[j].ipat[is][in]]; \
    /*printf(" %d(%d)",nlink4[j].ipat[is][in],tmp);*/ \
    if(max < tmp) max = tmp; \
    if(min > tmp) min = tmp; \
  } \
  if((max-min) <= 1) \
  { \
    ngr[is] = min; \
  } \
  else \
  { \
    /*printf(" (WARNING:max-min=%1d)",max-min);*/ \
    ngr[is] = (max+min)/2; \
  } \
  /*printf(" ngr[%1d]->%d\n",is,ngr[is]);*/
E 6


D 6
static int
ipatsort(DCsegm *a, DCsegm *b)
{
  if (a->nwhit > b->nwhit) return (1);
  if (a->nwhit < b->nwhit) return (-1);
E 6

D 6
  return (0);
}
E 6
I 6
/***********************************************
 ***********************************************
   lanal1 - in use
   lanal2 & lanal5 - not in use 
 ***********************************************
************************************************/
E 6

D 6

/*

lanal1 - in use
lanal2 & lanal5 - not in use 

*/

E 6
void
prinit(char *filename, int lanal1, int lanal2, int lanal5)
{
  static int max_lnk, max_lnk_local;
  static int max_npat, npat, ipat[6], take;
  static int i, j, k, l, iv, nf, ind, nroads, nrw, numdb, ncol, nrow, nwd, nch;
  static int numra, numrb, icl, ierr, adr, adr1, tmp, *lptr;
  static char str[100], *knam;
  static char utimes[25];
  int *buf, *buf0, md1, md2, md3, is, in;
  short *buf2;
  char *buf3;
  unsigned char *ntest1, *ntest2, *ch;
D 7
  char *kname="        ", *hname="        ", *format="               ";
E 7
I 7
  char *kname="        ", *hname="        ", format[NFMT+1];
E 7
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
D 6
    segments = (SEGMdict *) malloc(MAX_NPAT*sizeof(SEGMdict));
    segments_npat = 1;
    for(j=0; j<6; j++) segments[0].ipat[j] = 0;
E 6
    for(i=0; i<NLISTST; i++) slist[i] = 0; /* will use it in prupdatelink, should be clean !!! */
D 6

    linksegments = (SEGMdict *) malloc(MAX_NPAT*sizeof(SEGMdict));
    linksegments_npat = 1;
    for(j=0; j<6; j++) linksegments[0].ipat[j] = 0;
E 6
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
D 7
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),strlen(format));
E 7
I 7
    format[NFMT] = '\0';
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),NFMT);
E 7
    buf0 = (int *) malloc(ncol*sizeof(int));
    frdat_(&md1,buf0,&ncol);
    max_npat = (md1-1-2-NS2)/6;
    printf(" %s %8d %6d %6d %6d    %s ---> max_npat=%d\n",
                      hname,numdb,ncol,nrow,md1,format,max_npat);
    max_lnk = buf0[ncol-1];
    printf("max_lnk=%d max_npat=%d\n",max_lnk,max_npat); fflush(stdout);
    k = 0;

D 6
    segments_npat = buf0[k++];
    segments = (SEGMdict *) malloc(segments_npat*sizeof(SEGMdict));
    for(j=0; j<segments_npat; j++)
    {
      for(i=0; i<6; i++) segments[j].ipat[i] = buf0[k++];
      if(segments[j].ipat[0]-segments[j].ipat[5] > grshift) grshift = segments[j].ipat[0]-segments[j].ipat[5];
    }
    printf("grshift=%d\n",grshift);
    for(j=segments_npat; j<max_npat; j++) for(i=0; i<6; i++) k++;
    for(j=0; j<segments_npat; j++)
    {
      grsegm[j] = segments[j].ipat[5]-segments[j].ipat[0]+grshift;
      printf("============ grsegm[%d]=%d\n",j,grsegm[j]);
    }
    dtyp = buf0[k++];
    utime = buf0[k++];
    for(i=0; i<NS2; i++) lnk[i] = buf0[k++];
E 6


I 6
    /*************************************************************************/
    /* readout old segment info - to be compatible with old dictionary files */
    k++;
    for(j=0; j<max_npat; j++) for(i=0; i<6; i++) k++;
    /*************************************************************************/
E 6

D 6
    /**********************/
    /* SEGMENT DICTIONARY */
    /**********************/
E 6

I 6
    dtyp = buf0[k++];
    utime = buf0[k++];
    for(i=0; i<NS2; i++) lnk[i] = buf0[k++];
E 6

D 6
    {int i; for(i=0; i<25; i++) utimes[i] = ' '; utimes[24] = '\0';}
    getasciitime_(&utime,utimes,strlen(utimes));
    if(dtyp == 1)      knam = "LTORU   ";
    else if(dtyp == 2) knam = "LMINT   ";
    else if(dtyp == 3) knam = "LTOPM   ";
    else if(dtyp == 4) knam = "LUNIF   ";
    else if(dtyp == 5) knam = "LNONE   ";
E 6

D 6
    printf(" Read 'prlink.bos' file for %8.8s created on  %24.24s\n",knam,utimes);

    printf("\n------- old style segments ---------\n");
    printf("\n ipat     la1  la2  la3  la4  la5  la6   group\n");
    for(j=0; j<segments_npat; j++)
          printf("%5d   %5d%5d%5d%5d%5d%5d  %5d\n",j,
          segments[j].ipat[0],segments[j].ipat[1],segments[j].ipat[2],
          segments[j].ipat[3],segments[j].ipat[4],segments[j].ipat[5],grsegm[j]);
    printf("\n");

    /* make link groups */

    if(lanal1 != 3)
    {
      int skip, ll, igr, iw3, count, nelem, ifound, isg, isum, la, a[NGROUP][6][16], idtim[16][6];
      int iw, nw[NLAY], ns[NLAY], npat[NGROUP], ipat[NGROUP][MAX_NPAT][9]; /* [0-5]ipat,[8]number */
      int its, nbytes1, nbytes2;

      nshift = 8;
      iw3 = nshift+1; /* starting from 0 */


      /* cut by hands the length of segments which out of range */
      for(k=0; k<segments_npat; k++)
      {
        if(segments[k].ipat[0]+iw3 > 15) segments[k].ipat[0] = 15 - iw3;
        if(segments[k].ipat[0]+iw3 < 0)  segments[k].ipat[0] = -iw3;
        if(segments[k].ipat[5]+iw3 > 15) segments[k].ipat[5] = 15 - iw3;
        if(segments[k].ipat[5]+iw3 < 0)  segments[k].ipat[5] = -iw3;
      }

      for(l=0;l<NGROUP;l++) for(i=0; i<16; i++) for(j=0; j<6; j++) a[l][j][i] = 0;
      l=0;
      for(igr=0; igr<NGROUP; igr++)
      {
        npat[l] = 0;
        for(k=0; k<segments_npat; k++)
        {
          if(grsegm[k] == igr || grsegm[k] == igr+1)
          {
            for(i=0; i<6; i++) {ipat[l][npat[l]][i] = segments[k].ipat[i];printf(" %d",segments[k].ipat[i]);}
            printf("\n");
            npat[l] ++;
            for(i=0; i<6; i++) a[l][i][segments[k].ipat[i]+iw3] = 1;
          }
        }
        printf("==> igr=%d",igr);
        GETOFFSET(l);
        printf("nwires[%1d][%1d]=%d\n",l-1,0,nwires[l-1][0]);
        /*if(nwires[l-1][0] == 0) {l--; break;}*/ /* group 0 could not exist for intermediate segment lists ! */
      }
      ngroup = l;
      printf("\nFound %1d groups (maximum %1d groups).\n",ngroup,NGROUP);

      /* cleanup pointers */
      for(j=0; j<ngroup; j++) for(i=0; i<NSGM; i++) segm[j][i] = 0;

      for(k=0; k<ngroup; k++)
      {
        count = nelem = 0;
        for(adr=1; adr<NSGM; adr++)
        {
          ifound = 0;
          getIdtime(offset[k], nwires[k], adr, iw3, idtim);
          for(isg=0; isg<npat[k]; isg++)
          {
            isum = 0;
            for(la=0; la<NLAY; la++)
            {
              nw[la] = iw = iw3 + ipat[k][isg][la];
              if(idtim[iw][la] > NO_HIT)
              {
                ns[la] = 1;
                isum++;
              }
              else
              {
                ns[la] = 0;
              }
            }
            if(isum < MINHITSEGM) continue;
            skip = 0;
            for(i=0; i<ifound; i++)
            {
              its = 0;
              for(la=0; la<NLAY; la++)
              {
                if(nw[la]==nwall[i][la] && ns[la]==1 && nsall[i][la]==1) its++;
              }
              if(its == isum && its <= nwhit[i])
              {
                skip = 1;
                break;
              }
            }
            if(skip) continue;
            if(ifound < NFOUND)
            {
              for(la=0; la<NLAY; la++)
              {
                nwall[ifound][la] = nw[la];
                nsall[ifound][la] = ns[la];
              }
              nwhit[ifound] = isum;
              ifound++;
            }
            else
            {
              continue;
            }
          }

          if(ifound)
          {
            DCsegm *sgmptr;
            count++;
            nelem += ifound;
            nbytes1 += sizeof(DCsegm);
            nbytes2 += ifound * sizeof(DCsegm);
            nsegm[k][adr] = ifound;
            segm[k][adr] = (DCsegm *) malloc(ifound * sizeof(DCsegm));
            for(i=0; i<ifound; i++)
            {
              sgmptr = &segm[k][adr][i];
              if(nwhit[i]>6 || nwhit[i]<3)
              {
                printf("prlib: ERROR nwhit=%d, should be 3<=nwhit<=6 - exit.\n",nwhit[i]);
                exit(1);
              }
              sgmptr->nwhit = 6-nwhit[i];
              sgmptr->sign0 = nsall[i][0];
              sgmptr->sign1 = nsall[i][1];
              sgmptr->sign2 = nsall[i][2];
              sgmptr->sign3 = nsall[i][3];
              sgmptr->sign4 = nsall[i][4];
              sgmptr->sign5 = nsall[i][5];
              sgmptr->iw0 = nwall[i][0];
              sgmptr->iw1 = nwall[i][1];
              sgmptr->iw2 = nwall[i][2];
              sgmptr->iw3 = nwall[i][3];
              sgmptr->iw4 = nwall[i][4];
              sgmptr->iw5 = nwall[i][5];
            }

            /* sort */
            qsort((void *)segm[k][adr], ifound, sizeof(DCsegm), (int (*) (const void *, const void *))ipatsort);

            /* cleanuplownw */
            i = 0;
            sgmptr = segm[k][adr];
            while(i < ifound && sgmptr[i].nwhit == sgmptr[0].nwhit) i++;
            if(i < ifound)
            {
              while(i < ifound)
              {
                take = 1;
                if(!sgmptr[i].sign0) take = 0;
                for(j=0; j<i; j++) if(sgmptr[i].iw0 == sgmptr[j].iw0) take = 0;
                if(take) {i++; continue;}
                take = 1;
                if(!sgmptr[i].sign1) take = 0;
                for(j=0; j<i; j++) if(sgmptr[i].iw1 == sgmptr[j].iw1) take = 0;
                if(take) {i++; continue;}
                take = 1;
                if(!sgmptr[i].sign2) take = 0;
                for(j=0; j<i; j++) if(sgmptr[i].iw2 == sgmptr[j].iw2) take = 0;
                if(take) {i++; continue;}
                take = 1;
                if(!sgmptr[i].sign3) take = 0;
                for(j=0; j<i; j++) if(sgmptr[i].iw3 == sgmptr[j].iw3) take = 0;
                if(take) {i++; continue;}
                take = 1;
                if(!sgmptr[i].sign4) take = 0;
                for(j=0; j<i; j++) if(sgmptr[i].iw4 == sgmptr[j].iw4) take = 0;
                if(take) {i++; continue;}
                take = 1;
                if(!sgmptr[i].sign5) take = 0;
                for(j=0; j<i; j++) if(sgmptr[i].iw5 == sgmptr[j].iw5) take = 0;
                if(take) {i++; continue;}

                /* skip it */
                for(j=i+1; j<ifound; j++)
                {
                  sgmptr[j-1].nwhit = sgmptr[j].nwhit;
                  sgmptr[j-1].sign0 = sgmptr[j].sign0;
                  sgmptr[j-1].sign1 = sgmptr[j].sign1;
                  sgmptr[j-1].sign2 = sgmptr[j].sign2;
                  sgmptr[j-1].sign3 = sgmptr[j].sign3;
                  sgmptr[j-1].sign4 = sgmptr[j].sign4;
                  sgmptr[j-1].sign5 = sgmptr[j].sign5;
                  sgmptr[j-1].iw0   = sgmptr[j].iw0;
                  sgmptr[j-1].iw1   = sgmptr[j].iw1;
                  sgmptr[j-1].iw2   = sgmptr[j].iw2;
                  sgmptr[j-1].iw3   = sgmptr[j].iw3;
                  sgmptr[j-1].iw4   = sgmptr[j].iw4;
                  sgmptr[j-1].iw5   = sgmptr[j].iw5;
                }
                ifound--;
              }

              nsegm[k][adr] = ifound;
            }

          }
        }
        printf("group[%1d]: npat=%d count=%d nelem=%d\n",k,npat[k],count,nelem);
      }
    }

    /*****************************/
    /* END OF SEGMENT DICTIONARY */
    /*****************************/


E 6
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


D 13
    /* fill alist[] temporary by bytes# should be allocated */
E 13
I 13
    /* fill alist[] and slist[] temporary with bytes# should be allocated */
E 13

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
I 13
        alist[i]->road = (DCroad *) malloc(tmp * sizeof(DCroad));
E 13
D 6
#ifdef SL5
        alist[i]->road = (DCroad *) malloc(tmp * sizeof(DCroad));
#endif
E 6
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
D 7
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),strlen(format));
E 7
I 7
    frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,strlen(hname),NFMT);
E 7
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
I 13


        /************ fill alist[] **************/

E 13
        adr =  GETAXADR(link1[j]);
        if(adr >= NLISTAX) printf("prlib: ERROR: adr1=%d %x\n",adr,adr);
        k = alist[adr]->nroads ++;
I 13
#ifdef Linux
        alist[adr]->road[k].path = link2[j]; /* stereo pattern */
#else
        alist[adr]->road[k].path = (link2[j]>>8)&0xffffff;
#endif
E 13
D 6
#ifdef SL5
#ifdef Linux
        alist[adr]->road[k].path = link2[j]; /* stereo pattern */
#else
        alist[adr]->road[k].path = (link2[j]>>8)&0xffffff;
#endif
#endif
E 6
        ch = (unsigned char *)&link2[j];

I 12
D 13
#ifdef DEBUG
E 13
I 13
		/*
E 13
        {
          unsigned char *ch1, *ch2;
          ch1 = (unsigned char *)&link1[j];
          ch2 = (unsigned char *)&link2[j];

D 13
          if(ch1[1]==82&&ch1[2]==92&ch2[1]==85&&ch2[2]==95)
E 13
I 13
          if(ch1[0]==14&&ch1[1]==12&ch1[2]==3)
E 13
          {
            printf("===> (%3d %3d %3d) (%3d %3d %3d)\n",
              ch1[0],ch1[1],ch1[2],ch2[0],ch2[1],ch2[2]);
          }
        }
D 13
#endif
E 13
I 13
		*/
E 13

I 13

E 13
E 12
/* apply corrections !!! TEMPORARY !!! */
#define C0 0
#define C1 10
#define C2 5
D 13
        if(alist[adr]->max[0] < (ch[0]+C0))
                    alist[adr]->max[0] = ch[0]+C0;
        if(alist[adr]->max[1] < (ch[1]+C1))
                    alist[adr]->max[1] = ch[1]+C1;
        if(alist[adr]->max[2] < (ch[2]+C2))
                    alist[adr]->max[2] = ch[2]+C2;
        if(alist[adr]->min[0] > MAX(0,(ch[0]-C0)))
                    alist[adr]->min[0] = MAX(0,(ch[0]-C0));
        if(alist[adr]->min[1] > MAX(0,(ch[1]-C1)))
                    alist[adr]->min[1] = MAX(0,(ch[1]-C1));
        if(alist[adr]->min[2] > MAX(0,(ch[2]-C2)))
                    alist[adr]->min[2] = MAX(0,(ch[2]-C2));
E 13
I 13
        if(alist[adr]->max[0] < (ch[0]+C0)) alist[adr]->max[0] = ch[0]+C0;
        if(alist[adr]->max[1] < (ch[1]+C1)) alist[adr]->max[1] = ch[1]+C1;
        if(alist[adr]->max[2] < (ch[2]+C2)) alist[adr]->max[2] = ch[2]+C2;
        if(alist[adr]->min[0] > MAX(0,(ch[0]-C0))) alist[adr]->min[0] = MAX(0,(ch[0]-C0));
        if(alist[adr]->min[1] > MAX(0,(ch[1]-C1))) alist[adr]->min[1] = MAX(0,(ch[1]-C1));
        if(alist[adr]->min[2] > MAX(0,(ch[2]-C2))) alist[adr]->min[2] = MAX(0,(ch[2]-C2));
E 13

I 13


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


E 13
D 6
#ifdef SL5

        alist[adr]->road[k].p = IMOM(nlink3[1][j]/VECT7); /* momentum MeV/c -> (0-255) */
        /* alist[adr]->road[k].u.s.np   = nlink3[0][j]; how many momenta - DO WE NEED THAT HERE !!! */
        alist[adr]->road[k].u.r.charge = link4[j*NVEC+10];

        alist[adr]->road[k].x      = link4[j*NVEC+0];
        alist[adr]->road[k].y      = link4[j*NVEC+1];
        alist[adr]->road[k].z      = link4[j*NVEC+2];
        alist[adr]->road[k].dx     = link4[j*NVEC+3];
        alist[adr]->road[k].dy     = link4[j*NVEC+4];
        alist[adr]->road[k].dz     = link4[j*NVEC+5];
        alist[adr]->road[k].ntof   = link4[j*NVEC+6];
        alist[adr]->road[k].nu     = link4[j*NVEC+7];
        alist[adr]->road[k].nv     = link4[j*NVEC+8];
        alist[adr]->road[k].nw     = link4[j*NVEC+9];

        for(is=0; is<6; is++)
        {
          int min, max, tmp;
          unsigned int ngr[6];
          GETNGR;
          alist[adr]->road[k].u.r.gr0 = ngr[0];
          alist[adr]->road[k].u.r.gr1 = ngr[1];
          alist[adr]->road[k].u.r.gr2 = ngr[2];
          alist[adr]->road[k].u.r.gr3 = ngr[3];
          alist[adr]->road[k].u.r.gr4 = ngr[4];
          alist[adr]->road[k].u.r.gr5 = ngr[5];
        }

/*
#ifdef SIM
        for(is=0; is<6; is++)
        {
          alist[adr]->road[k].npat[is] = nlink4[j].npat[is];
          for(in=0; in<alist[adr]->road[k].npat[is]; in++)
            alist[adr]->road[k].ipat[is][in] = nlink4[j].ipat[is][in];
        }
#endif
*/

#endif


E 6
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


D 13
        slist[adr]->road[k].p = IMOM(nlink3[1][j]/VECT7);/* momentum MeV/c -> (0-255) */
E 13

D 6
        /* slist[adr]->road[k].u.s.np = nlink3[0][j]; how many momenta - DO WE NEED IT HERE ??? */
E 6
I 6
D 13
        /* slist[adr]->road[k].np = nlink3[0][j]; how many momenta - DO WE NEED IT HERE ??? */
E 13
E 6

I 13
        slist[adr]->road[k].p = IMOM(nlink3[1][j]/VECT7);/* momentum MeV/c -> (0-255) */
        /* slist[adr]->road[k].np = nlink3[0][j]; how many momenta - DO WE NEED IT HERE ??? */
E 13
D 6
        slist[adr]->road[k].u.r.charge = link4[j*NVEC+10];

E 6
I 6
        slist[adr]->road[k].charge = link4[j*NVEC+10];
E 6
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

I 13



E 13
D 6
        for(is=0; is<6; is++)
        {
          int min, max, tmp;
          unsigned int ngr[6];
          GETNGR;
          slist[adr]->road[k].u.r.gr0 = ngr[0];
          slist[adr]->road[k].u.r.gr1 = ngr[1];
          slist[adr]->road[k].u.r.gr2 = ngr[2];
          slist[adr]->road[k].u.r.gr3 = ngr[3];
          slist[adr]->road[k].u.r.gr4 = ngr[4];
          slist[adr]->road[k].u.r.gr5 = ngr[5];
        }

/*
#ifdef SIM
        for(is=0; is<6; is++)
        {
          slist[adr]->road[k].npat[is] = nlink4[j].npat[is];
          for(in=0; in<slist[adr]->road[k].npat[is]; in++)
            slist[adr]->road[k].ipat[is][in] = nlink4[j].ipat[is][in];
        }
#endif
*/

E 6
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
D 6
    buf[k++] = linksegments_npat;
E 6
I 6


	/*****************************/
	/* temporary - fake segments */
    buf[k++] = 0;
E 6
    for(j=0; j<max_npat; j++)
      for(i=0; i<6; i++)
D 6
        buf[k++] = linksegments[j].ipat[i];
E 6
I 6
        buf[k++] = 0;
	/*****************************/



E 6
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



D 6
	      /*
              printf("st=%d\n",spath);
              for(is=0; is<6; is++)
              {
                int iseg,i_m,j_m,nwires[6],offset[6];
                int a[6][16];
                for(i_m=0; i_m<6; i_m++)
                  for(j_m=0; j_m<16; j_m++)
                    a[i_m][j_m] = 0;
                printf("is=%1d npat=%d :",is+1,slist[adr]->road[k].npat[is]);
                for(in=0; in<slist[adr]->road[k].npat[is]; in++)
                {
                  printf(" %d",slist[adr]->road[k].ipat[is][in]);
                  iseg = slist[adr]->road[k].ipat[is][in];
                  for(i_m=0; i_m<6; i_m++) a[i_m][linksegments[iseg].ipat[i_m]+8] = 1;
                }
                printf("\n");
                for(i_m=5; i_m>=0; i_m--)
                {
                  nwires[i_m] = 0;
                  offset[i_m] = 0;
                  for(j_m=15; j_m>=0; j_m--)
                  {
                    if(a[i_m][j_m]) nwires[i_m] ++;
                    if(a[i_m][j_m] && !a[i_m][j_m-1]) offset[i_m] = j_m;
                    if(a[i_m][j_m]) printf("x");
                    else printf(".");
                  }
                  printf("  nwires=%d   offset=%d\n",nwires[i_m],offset[i_m]);
                }
	      }
              printf("\n");
	      */
E 6

D 6



              nlink3[lnk[i]][0] = slist[adr]->road[k].u.s.np;
              /*nlink3[lnk[i]][1] = IMOM((float)slist[adr]->road[k].u.s.p/VECT7);*/
              nlink3[lnk[i]][1] = slist[adr]->road[k].u.s.p;
E 6
#ifdef SIM
I 6
              nlink3[lnk[i]][0] = slist[adr]->road[k].np;
              /*nlink3[lnk[i]][1] = IMOM((float)slist[adr]->road[k].u.s.p/VECT7);*/
              nlink3[lnk[i]][1] = slist[adr]->road[k].psim;

E 6
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
D 6
              vect[lnk[i]][10] = slist[adr]->road[k].u.r.charge;
E 6
I 6
              vect[lnk[i]][10] = slist[adr]->road[k].charge;
E 6
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

D 6
fflush(stdout);
printf("min=%3d %3d %3d   max=%3d %3d %3d\n",min[0],min[1],min[2],max[0],max[1],max[2]);
printf("minax: %3d %3d %3d %3d %3d %3d\n",minax[0],minax[1],minax[2],minax[3],minax[4],minax[5]);
printf("maxax: %3d %3d %3d %3d %3d %3d\n",maxax[0],maxax[1],maxax[2],maxax[3],maxax[4],maxax[5]);
printf("minst: %3d %3d %3d %3d %3d %3d\n",minst[0],minst[1],minst[2],minst[3],minst[4],minst[5]);
printf("maxst: %3d %3d %3d %3d %3d %3d\n",maxst[0],maxst[1],maxst[2],maxst[3],maxst[4],maxst[5]);
fflush(stdout);
E 6
I 6
    fflush(stdout);
    printf("min=%3d %3d %3d   max=%3d %3d %3d\n",
            min[0],min[1],min[2],max[0],max[1],max[2]);
    fflush(stdout);
E 6


    fwend_(&ierr);
    if(ierr != 0) printf(" *** error *** \n");
    fweod_();

    fparm_("CLOSE UNIT=56",13);

D 6
    /* Print new (or updated) templates */

    {int i; for(i=0; i<25; i++) utimes[i] = ' '; utimes[24] = '\0';}
    getasciitime_(&utime,utimes,strlen(utimes));
    printf(" Written 'prlink.bos' for %8.8s created on  %24.24s\n",knam,utimes); fflush(stdout);


    printf("\n ipat     la1  la2  la3  la4  la5  la6  (linksegments)\n");
    for(j=1; j<=linksegments_npat; j++)
          printf("%5d   %5d%5d%5d%5d%5d%5d\n",j,
          linksegments[j-1].ipat[0],linksegments[j-1].ipat[1],
          linksegments[j-1].ipat[2],linksegments[j-1].ipat[3],
          linksegments[j-1].ipat[4],linksegments[j-1].ipat[5]);
    fflush(stdout);


    printf("\n ipat     la1  la2  la3  la4  la5  la6\n");
    for(j=1; j<=segments_npat; j++)
          printf("%5d   %5d%5d%5d%5d%5d%5d\n",j,
          segments[j-1].ipat[0],segments[j-1].ipat[1],segments[j-1].ipat[2],
          segments[j-1].ipat[3],segments[j-1].ipat[4],segments[j-1].ipat[5]);
    fflush(stdout);


E 6
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
D 6
    buf[0]                          = npat - the number of segments
    buf[1..6]                       = ipat[0]..ipat[5] - segment 1
    buf[7..12]                      = ipat[0]..ipat[5] - segment 2
    .................................................................
    buf[(npat-1)*6+1..(npat-1)*6+6] = ipat[0]..ipat[5] - segment npat
E 6
I 6
    buf[0]                          = npat - former segments
    .............                     dictionary
    buf[max_npat*6]                   - obsolete
E 6
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
I 12













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


D 13

E 13



E 12
E 1
