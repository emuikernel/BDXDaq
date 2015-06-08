h47477
s 00001/00001/00127
d D 1.5 07/11/04 14:35:04 boiarino 6 5
c *** empty log message ***
e
s 00004/00004/00124
d D 1.4 07/11/03 12:07:38 boiarino 5 4
c *** empty log message ***
e
s 00002/00002/00126
d D 1.3 07/11/03 12:00:19 boiarino 4 3
c *** empty log message ***
e
s 00054/00000/00074
d D 1.2 07/11/03 11:59:27 boiarino 3 1
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 19:00:37 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/sclib.h
e
s 00074/00000/00000
d D 1.1 01/11/19 19:00:36 boiarino 1 0
c date and time created 01/11/19 19:00:36 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef _SCLIB_

/* piece from sdageom.h */
#define npl_sc  4

#ifdef	__cplusplus
extern "C" {
#endif

/* sclib.h */

typedef struct scdata *SCDataPtr;
typedef struct scdata
{
#ifdef Linux
  unsigned char  slab;     /* layer number */
  unsigned char  layer;     /* strip number */
#else
  unsigned char  layer;
  unsigned char  slab;
#endif
  unsigned short tdcl;		/* tdc left */
  unsigned short adcl;		/* adc left */
  unsigned short tdcr;		/* tdc right */
  unsigned short adcr;		/* adc right */
} SCData;




/* functions */

void scmatch_(int *jw, TRevent *ana, int *isec, DCscid *scid, float x0[npl_sc], int *iflg);
void scmatch(int *jw, TRevent *ana, int isec, DCscid *scid, float x0[npl_sc], int *iflg);

/* macros */

/* get SC raw data; input: jw, isec; output: scndig, scdigi */
#define GETSCDATA \
I 3
  /* first, get ADCs from old bank */ \
E 3
  scndig = 0; \
  if((ind=etNlink(jw,"SC  ",isec)) > 0) \
I 3
  { \
    int i, j, id, idhigh, nrow, place; \
    unsigned short *i16; \
    i16 = (unsigned short *) &jw[ind]; \
    nrow = etNrow(jw,ind); \
    for(i=0; i<nrow; i++) \
    { \
D 6
      id = i16[0]; \
E 6
I 6
      id = i16[0] & 0xFF; \
E 6
      if(id < 1 || id > 57) continue; \
      scdigi[scndig][0] = id; \
      scdigi[scndig][1] = 0; /* reserved for left TDC */ \
      scdigi[scndig][2] = i16[2]; \
      scdigi[scndig][3] = 0; /* reserved for right TDC */ \
      scdigi[scndig][4] = i16[4]; \
      scndig++; \
      if(scndig >= 57) break; \
      i16 += 5; \
    } \
D 5
printf("\nSC ======================================\n"); \
E 5
I 5
/*printf("\nSC ======================================\n"); \
E 5
D 4
for(i=0; i<scndig; i++) printf("[] %5d %5d %5d %5d\n", \
E 4
I 4
for(i=0; i<scndig; i++) printf("[%2d] %5d %5d %5d %5d\n", \
E 4
D 5
scdigi[i][0],scdigi[i][1],scdigi[i][2],scdigi[i][3],scdigi[i][4]); \
E 5
I 5
scdigi[i][0],scdigi[i][1],scdigi[i][2],scdigi[i][3],scdigi[i][4]);*/ \
E 5
    /* now get TDCs from new bank */ \
    if((ind=etNlink(jw,"SCT ",isec)) > 0) \
    { \
      i16 = (unsigned short *) &jw[ind]; \
      nrow = etNrow(jw,ind); \
      for(i=0; i<nrow; i++) \
      { \
        id = i16[0]&0xFF; /*low byte is counter number from 1 to 57*/ \
        idhigh = (i16[0]>>8)&&0x1; /*low bit in high byte is left-right flag*/ \
        if(idhigh==0) place = 1; \
        else          place = 3; \
        if(id < 1 || id > 57) continue; \
        for(j=0; j<scndig; j++) \
        { \
          if(id == scdigi[j][0] && scdigi[j][place] == 0) /* take only first TDC hit */ \
          { \
            scdigi[j][place] = i16[1]; \
            break; \
          } \
        } \
        i16 += 2; \
      } \
    } \
  } \
D 5
printf("\nSCT =====================================\n"); \
E 5
I 5
/*printf("\nSCT =====================================\n"); \
E 5
D 4
for(i=0; i<scndig; i++) printf("[] %5d %5d %5d %5d\n", \
E 4
I 4
for(i=0; i<scndig; i++) printf("[%2d] %5d %5d %5d %5d\n", \
E 4
scdigi[i][0],scdigi[i][1],scdigi[i][2],scdigi[i][3],scdigi[i][4]); \
D 5
printf("\n");
E 5
I 5
printf("\n")*/;
E 5

#define GETSCDATA_OLD \
  scndig = 0; \
  if((ind=etNlink(jw,"SC  ",isec)) > 0) \
E 3
  { \
    int nd, nd2; \
    int i32[7000]; \
    short *i16; \
    i16 = (short *) i32; \
    if((nd = etNdata(jw,ind)) > 0) \
    { \
      for(i=0; i<nd; i++) i32[i] = jw[ind+i]; \
      nd2 = 2*nd - 1; \
      for(i=0; i<nd2; i+=5) \
      { \
        int id; \
        id = i16[i]; \
        if(id < 1 || id > 48) continue; \
        scdigi[scndig][0] = id; \
        scdigi[scndig][1] = i16[i+1]; \
        scdigi[scndig][2] = i16[i+2]; \
        scdigi[scndig][3] = i16[i+3]; \
        scdigi[scndig][4] = i16[i+4]; \
        scndig++; \
        if(scndig >= 48) break; \
      } \
    } \
  }


#ifdef	__cplusplus
}
#endif


#define _SCLIB_
#endif
E 1
